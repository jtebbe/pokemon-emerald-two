#include "global.h"
#include "pc_move_journal.h"
#include "pc_storage.h"
#include "save.h"
#include "link_protocol.h"

#define LOG_MAGIC (SECTOR_SIGNATURE ^ 0x50434C31u)
#define RECORD_MAGIC (SECTOR_SIGNATURE ^ 0x50435231u)
#define RECORD_SEAL (SECTOR_SIGNATURE ^ 0x50435331u)
#define PAGE_SEAL (SECTOR_SIGNATURE ^ 0x50435031u)
#define LOG_FIRST 24
#define LOG_PAGES 4
#define SHADOW_PAGE 30
#define LOG_START 32
#define LOG_CAPACITY (LOG_PAGES * SECTOR_DATA_SIZE)
#define RECORD_BYTES 6144
#define MAX_RECORDS 128
#define WRITE_CHUNK 128
#define CHECKPOINT_CHUNK 512
#define PATCH_ZERO 1

struct LogHeader { u32 magic, generation, crc, seal; };
struct RecordHeader { u32 magic; u16 length, patches; u32 sequence, crc; };
struct Patch { u32 address; u16 size, flags; };
struct RecordIndex { u16 start, length; u32 pages; };
enum Phase { IDLE, PREPARE, HEADER, APPEND, APPEND_SEAL, FOLD_LOAD, SHADOW_ERASE,
    SHADOW_BODY, SHADOW_SIGNATURE, HOME_ERASE, HOME_BODY, HOME_SIGNATURE, SHADOW_RETIRE, LOG_RETIRE };

// These are serialized edit records and a core-data baseline, not extra PC boxes.
static EWRAM_DATA u8 sRecord[RECORD_BYTES];
static EWRAM_DATA struct SaveData sCore;
static EWRAM_DATA struct RecordIndex sIndex[MAX_RECORDS];
static EWRAM_DATA struct {
    u32 pages, recordPages, cursor, position, generation, shadowCounter;
    u16 count, used;
    u8 phase, page;
    bool8 session, active, failed, closing, staged, shadowSealed, suspending;
} sLog;

STATIC_ASSERT(sizeof(struct SaveData) <= NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE, LogCoreFits);

static void ReadLog(u32 offset, void *dest, u32 size)
{
    while (size)
    {
        u32 page = offset / SECTOR_DATA_SIZE, inPage = offset % SECTOR_DATA_SIZE;
        u32 count = min(size, SECTOR_DATA_SIZE - inPage);
        SaveFlashRead(LOG_FIRST + page, inPage, dest, count);
        dest = (u8 *)dest + count;
        offset += count;
        size -= count;
    }
}

static bool32 ProgramLog(u32 offset, const void *data, u32 size)
{
    while (size)
    {
        u32 page = offset / SECTOR_DATA_SIZE, inPage = offset % SECTOR_DATA_SIZE;
        u32 count = min(size, SECTOR_DATA_SIZE - inPage);
        if (!SaveFlashProgram(LOG_FIRST + page, inPage, data, count)) return FALSE;
        data = (const u8 *)data + count;
        offset += count;
        size -= count;
    }
    return TRUE;
}

static u32 ShadowCRC(void)
{
    return LinkDataHash(gSaveDataBuffer.data, SECTOR_DATA_SIZE)
        ^ LinkDataHash(&gSaveDataBuffer.counter, sizeof(gSaveDataBuffer.counter));
}

static bool32 ValidShadow(void)
{
    u32 crc = ShadowCRC();
    return gSaveDataBuffer.signature == PAGE_SEAL
        && (gSaveDataBuffer.counter & 31) < NUM_SECTORS_PER_SLOT
        && gSaveDataBuffer.id == (u16)crc && gSaveDataBuffer.checksum == crc >> 16;
}

static void NormalFooter(u32 page, u32 counter)
{
    gSaveDataBuffer.id = page;
    gSaveDataBuffer.checksum = SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(page));
    gSaveDataBuffer.counter = counter;
    gSaveDataBuffer.signature = SECTOR_SIGNATURE;
}

static bool32 RepairShadow(void)
{
    u32 signature;
    SaveFlashRead(SHADOW_PAGE, SECTOR_SIGNATURE_OFFSET, &signature, sizeof(signature));
    if (signature != PAGE_SEAL) return TRUE;
    SaveFlashRead(SHADOW_PAGE, 0, &gSaveDataBuffer, SECTOR_SIZE);
    if (!ValidShadow()) return FALSE;
    u32 page = gSaveDataBuffer.counter & 31, counter = gSaveDataBuffer.counter >> 5;
    NormalFooter(page, counter);
    return SaveFlashWriteBuffer(page) && SaveFlashErase(SHADOW_PAGE);
}

bool32 PCMoveJournalOverlay(u32 address, void *dest, u32 size)
{
    if (!sLog.active || !size) return TRUE;
    u32 wanted = 0;
    for (u32 page = address / SECTOR_DATA_SIZE; page <= (address + size - 1) / SECTOR_DATA_SIZE; page++)
        wanted |= 1u << page;
    for (u32 i = 0; i < sLog.count; i++)
    {
        if (!(sIndex[i].pages & wanted)) continue;
        struct RecordHeader header;
        ReadLog(sIndex[i].start, &header, sizeof(header));
        u32 pos = sIndex[i].start + sizeof(header);
        for (u32 n = 0; n < header.patches; n++)
        {
            struct Patch patch;
            ReadLog(pos, &patch, sizeof(patch));
            pos += sizeof(patch);
            u32 first = max(address, patch.address), end = min(address + size, patch.address + patch.size);
            if (first < end)
            {
                u8 *out = (u8 *)dest + first - address;
                if (patch.flags == PATCH_ZERO) memset(out, 0, end - first);
                else ReadLog(pos + first - patch.address, out, end - first);
            }
            if (patch.flags != PATCH_ZERO) pos += patch.size;
        }
    }
    return TRUE;
}

static bool32 ScanRecords(void)
{
    sLog.used = LOG_START;
    sLog.count = 0;
    sLog.pages = 0;
    while (sLog.used + sizeof(struct RecordHeader) + 4 <= LOG_CAPACITY)
    {
        struct RecordHeader header;
        ReadLog(sLog.used, &header, sizeof(header));
        if (header.magic != RECORD_MAGIC) break; // Erased or torn trailing record.
        if (header.length < sizeof(header) + 4 || header.length > RECORD_BYTES
            || sLog.used + header.length > LOG_CAPACITY || (header.length & 3)) break;
        u32 seal;
        ReadLog(sLog.used + header.length - 4, &seal, sizeof(seal));
        if (seal != RECORD_SEAL) break;
        ReadLog(sLog.used, sRecord, header.length);
        struct RecordHeader *record = (void *)sRecord;
        u32 crc = record->crc;
        record->crc = 0;
        if (LinkDataHash(sRecord, header.length - 4) != crc
            || header.sequence != sLog.count + 1 || sLog.count == MAX_RECORDS) return FALSE;
        u32 pos = sizeof(header), pages = 0;
        for (u32 i = 0; i < header.patches; i++)
        {
            struct Patch patch;
            if (pos + sizeof(patch) > header.length - 4) return FALSE;
            memcpy(&patch, sRecord + pos, sizeof(patch));
            pos += sizeof(patch);
            if (!patch.size || patch.flags > PATCH_ZERO
                || patch.address >= NUM_SECTORS_PER_SLOT * SECTOR_DATA_SIZE
                || patch.size > NUM_SECTORS_PER_SLOT * SECTOR_DATA_SIZE - patch.address) return FALSE;
            for (u32 p = patch.address / SECTOR_DATA_SIZE; p <= (patch.address + patch.size - 1) / SECTOR_DATA_SIZE; p++)
                pages |= 1u << p;
            if (patch.flags != PATCH_ZERO) pos += patch.size;
            if (pos > header.length - 4) return FALSE;
        }
        sIndex[sLog.count++] = (struct RecordIndex){sLog.used, header.length, pages};
        sLog.pages |= pages;
        sLog.used += header.length;
    }
    return TRUE;
}

void PCMoveJournalReset(void) { memset(&sLog, 0, sizeof(sLog)); }
bool32 PCMoveJournalSession(void) { return sLog.session; }
bool32 PCMoveJournalPending(void) { return sLog.active; }
bool32 PCMoveJournalBusy(void) { return sLog.phase != IDLE; }
bool32 PCMoveJournalFailed(void) { return sLog.failed; }
#if TESTING
u32 PCMoveJournalTestRecordSize(void) { return ((struct RecordHeader *)sRecord)->length; }
#endif

bool32 PCMoveJournalStart(void)
{
    if (sLog.failed || PCMoveJournalBusy()) return FALSE;
    if (sLog.session) return !sLog.failed;
    if (sLog.active)
    {
        // Keep the index, sequence, append position and durable core baseline.
        // Overworld changes since the previous visit must be diffed against
        // that baseline, not silently treated as already saved.
        sLog.session = TRUE;
        sLog.closing = sLog.suspending = FALSE;
        return TRUE;
    }
    memset(&sLog, 0, sizeof(sLog));
    sCore = gSaveData;
    sLog.session = TRUE;
    sLog.generation = gSaveCounter;
    sLog.phase = PREPARE;
    return TRUE;
}

void PCMoveJournalBegin(void)
{
    memset(sRecord, 0, sizeof(struct RecordHeader));
    sLog.position = sizeof(struct RecordHeader);
    sLog.recordPages = 0;
}

static bool32 AddPatch(u32 address, const u8 *data, u32 size, bool32 zero)
{
    struct Patch patch = {address, size, zero ? PATCH_ZERO : 0};
    u32 needed = sizeof(patch) + (zero ? 0 : size);
    if (sLog.position + needed + 7 > RECORD_BYTES) return FALSE;
    memcpy(sRecord + sLog.position, &patch, sizeof(patch));
    sLog.position += sizeof(patch);
    if (!zero)
    {
        memcpy(sRecord + sLog.position, data, size);
        sLog.position += size;
    }
    ((struct RecordHeader *)sRecord)->patches++;
    for (u32 p = address / SECTOR_DATA_SIZE; p <= (address + size - 1) / SECTOR_DATA_SIZE; p++)
        sLog.recordPages |= 1u << p;
    return TRUE;
}

bool32 PCMoveJournalPatch(u32 address, const void *data, u32 size)
{
    const u8 *bytes = data;
    u32 pos = 0, first = 0;
    while (pos < size)
    {
        if (bytes[pos] != 0) { pos++; continue; }
        u32 end = pos;
        while (end < size && bytes[end] == 0) end++;
        if (end - pos < 16) { pos = end; continue; }
        if (pos > first && !AddPatch(address + first, bytes + first, pos - first, FALSE)) return FALSE;
        if (!AddPatch(address + pos, NULL, end - pos, TRUE)) return FALSE;
        first = pos = end;
    }
    return pos == first || AddPatch(address + first, bytes + first, pos - first, FALSE);
}

bool32 PCMoveJournalCoreChanges(void)
{
    const u8 *now = (const void *)&gSaveData, *old = (const void *)&sCore;
    for (u32 i = 0; i < sizeof(sCore);)
    {
        if (now[i] == old[i]) { i++; continue; }
        u32 first = i++, last = i;
        while (i < sizeof(sCore) && i - last < 8)
        {
            if (now[i] != old[i]) last = i + 1;
            i++;
        }
        if (!PCMoveJournalPatch(first, now + first, last - first)) return FALSE;
    }
    return TRUE;
}

static void ApplyCoreRecord(void)
{
    struct RecordHeader *header = (void *)sRecord;
    u32 pos = sizeof(*header);
    for (u32 i = 0; i < header->patches; i++)
    {
        struct Patch patch;
        memcpy(&patch, sRecord + pos, sizeof(patch));
        pos += sizeof(patch);
        if (patch.address < sizeof(sCore))
        {
            u32 size = min(patch.size, sizeof(sCore) - patch.address);
            if (patch.flags == PATCH_ZERO) memset((u8 *)&sCore + patch.address, 0, size);
            else memcpy((u8 *)&sCore + patch.address, sRecord + pos, size);
        }
        if (patch.flags != PATCH_ZERO) pos += patch.size;
    }
}

bool32 PCMoveJournalSubmit(void)
{
    struct RecordHeader *header = (void *)sRecord;
    if (!header->patches)
    {
        PCStorageJournalCommitted(&sCore);
        return TRUE;
    }
    u32 length = (sLog.position + 7) & ~3;
    memset(sRecord + sLog.position, 0, length - sLog.position);
    header->magic = RECORD_MAGIC;
    header->length = length;
    header->sequence = sLog.count + 1;
    header->crc = LinkDataHash(sRecord, length - 4);
    u32 seal = RECORD_SEAL;
    memcpy(sRecord + length - 4, &seal, sizeof(seal));
    sLog.staged = TRUE;
    sLog.cursor = 0;
    sLog.phase = sLog.used + length > LOG_CAPACITY || sLog.count == MAX_RECORDS ? FOLD_LOAD : APPEND;
    return TRUE;
}

bool32 PCMoveJournalClose(void)
{
    sLog.closing = TRUE;
    sLog.suspending = FALSE;
    if (sLog.phase == IDLE && sLog.active) sLog.phase = FOLD_LOAD;
    if (!sLog.active && sLog.phase == IDLE) sLog.session = FALSE;
    return !sLog.failed;
}

bool32 PCMoveJournalSuspend(void)
{
    if (!sLog.failed && !sLog.active && sLog.phase == IDLE)
    {
        sLog.session = sLog.closing = sLog.suspending = FALSE;
        return TRUE;
    }
    if (sLog.failed || sLog.closing) return FALSE;
    sLog.suspending = TRUE;
    if (sLog.phase == IDLE)
    {
        sLog.session = FALSE;
        sLog.suspending = FALSE;
    }
    return TRUE;
}

// One page body is emitted in bounded chunks; the final signature is separate.
static bool32 ProgramPage(u32 sector)
{
    // Checkpoints already show a wait state. Larger page chunks avoid adding
    // dozens of empty UI frames to every shadow/home page pair.
    u32 count = min(CHECKPOINT_CHUNK, SECTOR_SIZE - 4 - sLog.cursor);
    const u8 *bytes = (const void *)&gSaveDataBuffer;
    u32 first = sLog.cursor;
    while (count)
    {
        u32 offset = first < SECTOR_SIGNATURE_OFFSET ? first : first + 4;
        u32 length = min(count, first < SECTOR_SIGNATURE_OFFSET ? SECTOR_SIGNATURE_OFFSET - first : count);
        if (!SaveFlashProgram(sector, offset, bytes + offset, length)) return FALSE;
        first += length;
        count -= length;
    }
    sLog.cursor = first;
    return TRUE;
}

static bool32 PageUnchanged(void)
{
    // A checksum mismatch proves a change; a match still requires exact byte
    // comparison. Never discard a record merely because its checksum collides.
    if (gSaveDataBuffer.checksum != SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(sLog.page)))
        return FALSE;
    u8 bytes[256];
    for (u32 offset = 0; offset < SECTOR_DATA_SIZE; offset += sizeof(bytes))
    {
        u32 size = min(sizeof(bytes), SECTOR_DATA_SIZE - offset);
        SaveFlashRead(sLog.page, offset, bytes, size);
        if (memcmp(bytes, gSaveDataBuffer.data + offset, size)) return FALSE;
    }
    return TRUE;
}

bool32 PCMoveJournalTick(void)
{
    if (sLog.failed) return FALSE;
nextPhase:
    switch (sLog.phase)
    {
    case IDLE: return TRUE;
    case PREPARE:
        if (!SaveFlashErase(LOG_FIRST + sLog.cursor)) goto failed;
        if (++sLog.cursor == LOG_PAGES) { sLog.cursor = 0; sLog.phase = HEADER; }
        break;
    case HEADER:
    {
        struct LogHeader header = {LOG_MAGIC, sLog.generation, 0, LOG_MAGIC};
        header.crc = LinkDataHash(&header, offsetof(struct LogHeader, crc));
        if (!SaveFlashProgram(LOG_FIRST, 0, &header, offsetof(struct LogHeader, seal))
            || !SaveFlashProgram(LOG_FIRST, offsetof(struct LogHeader, seal), &header.seal, sizeof(header.seal))) goto failed;
        sLog.active = TRUE;
        sLog.used = LOG_START;
        sLog.count = 0;
        sLog.pages = 0;
        if (sLog.staged)
        {
            struct RecordHeader *record = (void *)sRecord;
            record->sequence = 1;
            record->crc = 0;
            record->crc = LinkDataHash(sRecord, record->length - 4);
            sLog.phase = APPEND;
        }
        else
        {
            sLog.phase = sLog.closing ? FOLD_LOAD : IDLE;
            if (sLog.suspending && sLog.phase == IDLE)
            {
                sLog.session = FALSE;
                sLog.suspending = FALSE;
            }
        }
        break;
    }
    case APPEND:
    {
        struct RecordHeader *header = (void *)sRecord;
        u32 count = min(WRITE_CHUNK, header->length - 4 - sLog.cursor);
        if (!ProgramLog(sLog.used + sLog.cursor, sRecord + sLog.cursor, count)) goto failed;
        sLog.cursor += count;
        if (sLog.cursor == header->length - 4) sLog.phase = APPEND_SEAL;
        break;
    }
    case APPEND_SEAL:
    {
        struct RecordHeader *header = (void *)sRecord;
        if (!ProgramLog(sLog.used + header->length - 4, sRecord + header->length - 4, 4)) goto failed;
        sIndex[sLog.count++] = (struct RecordIndex){sLog.used, header->length, sLog.recordPages};
        sLog.used += header->length;
        sLog.pages |= sLog.recordPages;
        sLog.staged = FALSE;
        ApplyCoreRecord();
        PCStorageJournalCommitted(&sCore);
        sLog.phase = sLog.closing ? FOLD_LOAD : IDLE;
        if (sLog.suspending && sLog.phase == IDLE)
        {
            sLog.session = FALSE;
            sLog.suspending = FALSE;
        }
        break;
    }
    case FOLD_LOAD:
        if (!sLog.pages)
        {
            sLog.phase = sLog.shadowSealed ? SHADOW_RETIRE : LOG_RETIRE;
            goto nextPhase;
        }
        sLog.page = __builtin_ctz(sLog.pages);
        SaveFlashRead(sLog.page, 0, &gSaveDataBuffer, SECTOR_SIZE);
        if (gSaveDataBuffer.signature != SECTOR_SIGNATURE || gSaveDataBuffer.id != sLog.page
            || gSaveDataBuffer.checksum != SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(sLog.page))) goto failed;
        PCMoveJournalOverlay(sLog.page * SECTOR_DATA_SIZE, gSaveDataBuffer.data, SECTOR_DATA_SIZE);
        if (PageUnchanged())
        {
            sLog.pages &= ~(1u << sLog.page);
            // Yield after a page comparison, but do not write an unchanged
            // page or erase its shadow. This also accelerates boot replay.
            break;
        }
        sLog.shadowCounter = gSaveDataBuffer.counter;
        gSaveDataBuffer.counter = (gSaveDataBuffer.counter << 5) | sLog.page;
        gSaveDataBuffer.signature = PAGE_SEAL;
        u32 crc = ShadowCRC();
        gSaveDataBuffer.id = crc;
        gSaveDataBuffer.checksum = crc >> 16;
        sLog.phase = SHADOW_ERASE;
        goto nextPhase;
    case SHADOW_ERASE:
        if (!SaveFlashErase(SHADOW_PAGE)) goto failed;
        sLog.shadowSealed = FALSE;
        sLog.cursor = 0;
        sLog.phase = SHADOW_BODY;
        break;
    case SHADOW_BODY:
        if (!ProgramPage(SHADOW_PAGE)) goto failed;
        if (sLog.cursor == SECTOR_SIZE - 4) { sLog.phase = SHADOW_SIGNATURE; goto nextPhase; }
        break;
    case SHADOW_SIGNATURE:
        if (!SaveFlashProgram(SHADOW_PAGE, SECTOR_SIGNATURE_OFFSET, &gSaveDataBuffer.signature, 4)) goto failed;
        sLog.shadowSealed = TRUE;
        NormalFooter(sLog.page, sLog.shadowCounter);
        sLog.phase = HOME_ERASE;
        break;
    case HOME_ERASE:
        if (!SaveFlashErase(sLog.page)) goto failed;
        sLog.cursor = 0;
        sLog.phase = HOME_BODY;
        break;
    case HOME_BODY:
        if (!ProgramPage(sLog.page)) goto failed;
        if (sLog.cursor == SECTOR_SIZE - 4) { sLog.phase = HOME_SIGNATURE; goto nextPhase; }
        break;
    case HOME_SIGNATURE:
        if (!SaveFlashProgram(sLog.page, SECTOR_SIGNATURE_OFFSET, &gSaveDataBuffer.signature, 4)) goto failed;
        // Keep the sealed shadow until the next page needs it. Its verified
        // home is already complete, so resetting and replaying it is harmless.
        sLog.pages &= ~(1u << sLog.page);
        sLog.phase = FOLD_LOAD;
        break;
    case SHADOW_RETIRE:
        // Only the final shadow needs a separate retirement erase. This must
        // finish BEFORE clearing the log header, including after reset.
        if (!SaveFlashErase(SHADOW_PAGE)) goto failed;
        sLog.shadowSealed = FALSE;
        sLog.phase = LOG_RETIRE;
        break;
    case LOG_RETIRE:
        if (!SaveFlashErase(LOG_FIRST)) goto failed;
        sLog.active = FALSE;
        sLog.count = 0;
        sLog.cursor = 0;
        if (sLog.staged) sLog.phase = PREPARE;
        else { sLog.phase = IDLE; sLog.session = FALSE; }
        break;
    }
    return TRUE;
failed:
    sLog.failed = TRUE;
    return FALSE;
}

bool32 PCMoveJournalFlush(void)
{
    if (!PCMoveJournalClose()) return FALSE;
    while (PCMoveJournalBusy()) if (!PCMoveJournalTick()) return FALSE;
    return !sLog.failed;
}

bool32 PCMoveJournalRecover(void)
{
    if (sLog.session || sLog.active) return PCMoveJournalFlush();
    struct LogHeader header;
    SaveFlashRead(LOG_FIRST, 0, &header, sizeof(header));
    if (header.magic != LOG_MAGIC || header.seal != LOG_MAGIC) return TRUE;
    if (header.crc != LinkDataHash(&header, offsetof(struct LogHeader, crc))) return FALSE;
    memset(&sLog, 0, sizeof(sLog));
    sLog.active = TRUE;
    sLog.closing = TRUE;
    sLog.generation = header.generation;
    if (!RepairShadow() || !ScanRecords()) return FALSE;
    sLog.phase = FOLD_LOAD;
    return PCMoveJournalFlush();
}
