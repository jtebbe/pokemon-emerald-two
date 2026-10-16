#include "global.h"
#include "pc_storage.h"
#include "pc_move_journal.h"
#include "save.h"
#include "load_save.h"
#include "link_protocol.h"
#include "link.h"
#include "item.h"
#include "overworld.h"
#include "new_game.h"

#define BOX_BYTES (sizeof(struct BoxPokemon) * IN_BOX_COUNT)
#define PC_BYTES (BOX_BYTES * TOTAL_BOXES_COUNT)
#define PC_IMAGE_MAGIC (SECTOR_SIGNATURE ^ 0x50434931u)
#define PC_COMMIT_MAGIC (SECTOR_SIGNATURE ^ 0x50434331u)
#define CARRY_MAGIC 0x50434D31u
#define INVALID_BOX 0xFF
#define JOURNAL_COUNT 6

struct CarryHeader
{
    u32 magic;
    enum Item item;
    u8 count;
    u8 originBox;
    u8 mail;
    u8 positions[IN_BOX_COUNT];
};

#define CARRY_BYTES (sizeof(struct CarryHeader) + BOX_BYTES)
#define CARRY_OFFSET (NUM_PC_SAVE_SECTORS * SECTOR_DATA_SIZE - CARRY_BYTES)
#define CARRY_PAGE (NUM_SECTORS_PER_SLOT - 1)
#define CARRY_PAGE_OFFSET (SECTOR_DATA_SIZE - CARRY_BYTES)
STATIC_ASSERT(PC_BYTES <= CARRY_OFFSET, BoxesAndCarryFitFlash);
STATIC_ASSERT(sizeof(struct SaveData) <= NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE, ResidentSaveFitsTwoSectors);
STATIC_ASSERT(NUM_SECTORS_PER_SLOT == 24, PCJournalSectorLayout);
STATIC_ASSERT(CARRY_BYTES <= SECTOR_DATA_SIZE, CarryFitsOnePage);

static const u8 sJournalSectors[JOURNAL_COUNT] = {24, 25, 26, 27, 30, 31};
static EWRAM_DATA struct
{
    struct BoxPokemon mons[IN_BOX_COUNT];
    u32 savedHash;
    u32 monHashes[IN_BOX_COUNT];
    u8 box;
} sCache[PC_CACHE_BOXES];
static EWRAM_DATA struct
{
    struct CarryHeader header;
    const void *mons;
    u32 savedHash;
    bool8 single;
    bool8 dirty;
} sCarry;
static EWRAM_DATA bool8 sFresh = FALSE;
static EWRAM_DATA bool8 sInitialized = FALSE;
static EWRAM_DATA bool8 sFault = FALSE;
static EWRAM_DATA bool8 sBusy = FALSE;
static EWRAM_DATA u32 sCoreHash = 0;
static EWRAM_DATA u32 sClockHash = 0;
static EWRAM_DATA bool8 sSaveRequested = FALSE;
static EWRAM_DATA u8 sSavedBox = 0;

static u32 HashBytes(u32 hash, const void *data, u32 size)
{
    const u8 *bytes = data;
    // Unroll the byte loop: these RAM baselines run on the PC input/commit
    // boundary, where a branch per byte otherwise consumes extra frames.
    while (size >= 4)
    {
        hash = (hash ^ bytes[0]) * 16777619u;
        hash = (hash ^ bytes[1]) * 16777619u;
        hash = (hash ^ bytes[2]) * 16777619u;
        hash = (hash ^ bytes[3]) * 16777619u;
        bytes += 4;
        size -= 4;
    }
    while (size--) hash = (hash ^ *bytes++) * 16777619u;
    return hash;
}

static u32 ClockHashFor(const struct SaveData *save)
{
    u32 first = offsetof(struct SaveData, playTimeHours);
    u32 last = offsetof(struct SaveData, optionsButtonMode);
    u32 hash = HashBytes(2166136261u, (const u8 *)save + first, last - first);
#if OW_USE_FAKE_RTC
    hash = HashBytes(hash, &save->fakeRTC, sizeof(save->fakeRTC));
#endif
    return hash;
}

static u32 ClockHash(void) { return ClockHashFor(&gSaveData); }

static void SaveCacheBaseline(u32 i)
{
    sCache[i].savedHash = HashBytes(2166136261u, sCache[i].mons, BOX_BYTES);
    for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        sCache[i].monHashes[slot] = HashBytes(2166136261u, &sCache[i].mons[slot], sizeof(struct BoxPokemon));
}

static u32 CoreHash(void)
{
    // Clocks and the browsing position are not edits. Persist their latest
    // values with a real edit, explicit save, or PC exit instead of idle saves.
    u32 first = offsetof(struct SaveData, playTimeHours);
    u32 last = offsetof(struct SaveData, optionsButtonMode);
    u32 hash = HashBytes(2166136261u, &gSaveData, first);
#if OW_USE_FAKE_RTC
    first = offsetof(struct SaveData, fakeRTC);
    hash = HashBytes(hash, (u8 *)&gSaveData + last, first - last);
    last = first + sizeof(gSaveData.fakeRTC);
#endif
    first = offsetof(struct SaveData, currentBox);
    hash = HashBytes(hash, (u8 *)&gSaveData + last, first - last);
    last = first + sizeof(gSaveData.currentBox);
    return HashBytes(hash, (u8 *)&gSaveData + last, sizeof(gSaveData) - last);
}

static u32 CarryHash(void)
{
    u32 hash = HashBytes(2166136261u, &sCarry.header, sizeof(sCarry.header));
    if (sCarry.header.count == 0)
        return hash;
    if (sCarry.single)
    {
        struct BoxPokemon mon;
        SaveMonToBox(&mon, sCarry.mons);
        return HashBytes(hash, &mon, sizeof(mon));
    }
    return HashBytes(hash, sCarry.mons, sCarry.header.count * sizeof(struct BoxPokemon));
}

void PCStorageReset(bool32 fresh)
{
    PCMoveJournalReset();
    sInitialized = TRUE;
    memset(sCache, 0, sizeof(sCache));
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        sCache[i].box = INVALID_BOX;
    memset(&sCarry, 0, sizeof(sCarry));
    sCarry.savedHash = CarryHash();
    sFresh = fresh;
    sFault = FALSE;
    sBusy = FALSE;
    sCoreHash = 0;
    sClockHash = 0;
    sSaveRequested = FALSE;
    sSavedBox = 0;
}

bool32 PCStorageFaulted(void) { return sFault || PCMoveJournalFailed(); }
bool32 PCStorageBusy(void) { return sBusy || PCMoveJournalBusy(); }
bool32 PCStorageCarrying(void) { return sCarry.header.count != 0; }

void PCStorageReportFault(void)
{
    LinkProtocolSetError(LINK_PROTOCOL_PC_ERROR);
    SetMainCallback2(CB2_LinkError);
}

static void ReadPCBytes(u32 offset, void *dest, u32 size)
{
    if (sFresh)
    {
        memset(dest, 0, size);
        return;
    }
    while (size != 0)
    {
        u32 page = offset / SECTOR_DATA_SIZE + NUM_CORE_SAVE_SECTORS;
        u32 inPage = offset % SECTOR_DATA_SIZE;
        u32 length = min(size, SECTOR_DATA_SIZE - inPage);
        SaveFlashRead(page, inPage, dest, length);
        PCMoveJournalOverlay(NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE + offset, dest, length);
        offset += length;
        dest = (u8 *)dest + length;
        size -= length;
    }
}

static bool32 CacheDirty(u32 entry)
{
    if (sCache[entry].box >= TOTAL_BOXES_COUNT)
        return FALSE;
    // Detect legacy raw-pointer edits in RAM; never reread flash to poll dirty
    // state. The baseline is refreshed only after loading or a verified commit.
    return sCache[entry].savedHash != HashBytes(2166136261u, sCache[entry].mons, BOX_BYTES);
}

static bool32 BoxesDirty(void)
{
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (CacheDirty(i))
            return TRUE;
    return FALSE;
}

static u32 JournalCRC(void)
{
    return LinkDataHash(gSaveDataBuffer.data, SECTOR_DATA_SIZE)
         ^ LinkDataHash(&gSaveDataBuffer.counter, sizeof(gSaveDataBuffer.counter));
}

static bool32 ValidJournalImage(u32 index, u32 count, u32 generation)
{
    u32 crc = JournalCRC();
    return gSaveDataBuffer.signature == (index + 1 == count ? PC_COMMIT_MAGIC : PC_IMAGE_MAGIC)
        && ((gSaveDataBuffer.counter >> 5) & 7) == count
        && gSaveDataBuffer.counter >> 8 == generation
        && (gSaveDataBuffer.counter & 31) < NUM_SECTORS_PER_SLOT
        && gSaveDataBuffer.id == (u16)crc && gSaveDataBuffer.checksum == crc >> 16;
}

static bool32 NormalPageMatches(u32 page, u32 generation)
{
    struct { u16 id, checksum; u32 signature, counter; } footer;
    u8 bytes[64];
    SaveFlashRead(page, SECTOR_DATA_SIZE, &footer, sizeof(footer));
    if (footer.id != page || footer.signature != SECTOR_SIGNATURE || footer.counter != generation
        || footer.checksum != SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(page)))
        return FALSE;
    for (u32 offset = 0; offset < SECTOR_DATA_SIZE; offset += sizeof(bytes))
    {
        u32 size = min(sizeof(bytes), SECTOR_DATA_SIZE - offset);
        SaveFlashRead(page, offset, bytes, size);
        if (memcmp(bytes, gSaveDataBuffer.data + offset, size) != 0)
            return FALSE;
    }
    return TRUE;
}

bool32 PCStorageRecover(void)
{
    if (!SaveFlashAvailable())
        return FALSE;
    if (!PCMoveJournalRecover()) return FALSE;
    for (u32 end = 0; end < JOURNAL_COUNT; end++)
    {
        u32 signature;
        SaveFlashRead(sJournalSectors[end], SECTOR_SIGNATURE_OFFSET, &signature, sizeof(signature));
        if (signature != PC_COMMIT_MAGIC) continue;
        SaveFlashRead(sJournalSectors[end], 0, &gSaveDataBuffer, SECTOR_SIZE);
        if (gSaveDataBuffer.signature != PC_COMMIT_MAGIC)
            continue;
        u32 count = ((gSaveDataBuffer.counter >> 5) & 7);
        u32 generation = gSaveDataBuffer.counter >> 8;
        if (count != end + 1 || !ValidJournalImage(end, count, generation))
            return FALSE;
        u32 destinations = 0;
        // Validate the entire sealed transaction before changing ANY home page.
        for (u32 i = 0; i < count; i++)
        {
            SaveFlashRead(sJournalSectors[i], 0, &gSaveDataBuffer, SECTOR_SIZE);
            u32 page = gSaveDataBuffer.counter & 31;
            if (!ValidJournalImage(i, count, generation) || (destinations & (1u << page)))
                return FALSE;
            destinations |= 1u << page;
        }
        for (u32 i = 0; i < count; i++)
        {
            SaveFlashRead(sJournalSectors[i], 0, &gSaveDataBuffer, SECTOR_SIZE);
            u32 page = gSaveDataBuffer.counter & 31;
            if (NormalPageMatches(page, generation))
                continue;
            gSaveDataBuffer.id = page;
            gSaveDataBuffer.checksum = SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(page));
            gSaveDataBuffer.signature = SECTOR_SIGNATURE;
            gSaveDataBuffer.counter = generation;
            if (!SaveFlashWriteBuffer(page))
                return FALSE;
        }
        // Retire the seal only after ALL home pages have been verified.
        if (!SaveFlashErase(sJournalSectors[end]))
            return FALSE;
    }
    return TRUE;
}

static void CompactSavedParty(void)
{
    u32 dest = 0;
    for (u32 i = 0; i < PARTY_SIZE; i++)
        if (gSaveData.playerParty[i].data.species != SPECIES_NONE)
        {
            gSaveData.playerParty[dest] = gSaveData.playerParty[i];
            gSaveData.playerPartyMail[dest++] = gSaveData.playerPartyMail[i];
        }
    gSaveData.playerPartyCount = dest;
    while (dest < PARTY_SIZE)
    {
        ZeroBoxMonData(&gSaveData.playerParty[dest]);
        gSaveData.playerPartyMail[dest++] = MAIL_NONE;
    }
}

static bool32 InitializeDisk(bool32 overwrite)
{
    if (overwrite)
    {
        for (u32 i = 0; i < SECTORS_COUNT; i++)
            if (!SaveFlashErase(i)) return FALSE;
    }
    else
    {
        for (u32 i = 0; i < JOURNAL_COUNT; i++)
            if (!SaveFlashErase(sJournalSectors[i])) return FALSE;
    }
    for (u32 i = NUM_CORE_SAVE_SECTORS; i < NUM_SECTORS_PER_SLOT; i++)
    {
        memset(&gSaveDataBuffer, 0, sizeof(gSaveDataBuffer));
        gSaveDataBuffer.id = i;
        gSaveDataBuffer.signature = SECTOR_SIGNATURE;
        if (!SaveFlashWriteBuffer(i))
            return FALSE;
    }
    sFresh = FALSE;
    gSaveCounter = 0;
    return TRUE;
}

static bool32 AddPage(u8 *pages, u32 *count, u32 page)
{
    for (u32 i = 0; i < *count; i++)
        if (pages[i] == page)
            return TRUE;
    if (*count == JOURNAL_COUNT)
        return FALSE;
    pages[(*count)++] = page;
    return TRUE;
}

static void Overlay(u32 pageStart, u32 dataStart, const void *data, u32 size)
{
    u32 first = max(pageStart, dataStart);
    u32 end = min(pageStart + SECTOR_DATA_SIZE, dataStart + size);
    if (first < end)
        memcpy(gSaveDataBuffer.data + first - pageStart, (const u8 *)data + first - dataStart, end - first);
}

static bool32 BuildPage(u32 page)
{
    if (page < NUM_CORE_SAVE_SECTORS)
    {
        memset(&gSaveDataBuffer, 0, sizeof(gSaveDataBuffer));
        memcpy(gSaveDataBuffer.data, (u8 *)&gSaveData + page * SECTOR_DATA_SIZE, SaveSectorDataSize(page));
        return TRUE;
    }
    SaveFlashRead(page, 0, &gSaveDataBuffer, SECTOR_SIZE);
    if (gSaveDataBuffer.id != page || gSaveDataBuffer.signature != SECTOR_SIGNATURE
        || gSaveDataBuffer.checksum != SaveSectorChecksum(gSaveDataBuffer.data, SECTOR_DATA_SIZE))
        return FALSE;
    u32 start = (page - NUM_CORE_SAVE_SECTORS) * SECTOR_DATA_SIZE;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box < TOTAL_BOXES_COUNT)
            Overlay(start, sCache[i].box * BOX_BYTES, sCache[i].mons, BOX_BYTES);
    if (page == CARRY_PAGE && sCarry.dirty)
    {
        u8 *dest = gSaveDataBuffer.data + CARRY_PAGE_OFFSET;
        memset(dest, 0, CARRY_BYTES);
        memcpy(dest, &sCarry.header, sizeof(sCarry.header));
        dest += sizeof(sCarry.header);
        if (sCarry.header.count != 0)
        {
            if (sCarry.single)
                SaveMonToBox((void *)dest, sCarry.mons);
            else
                memcpy(dest, sCarry.mons, sCarry.header.count * sizeof(struct BoxPokemon));
        }
    }
    return TRUE;
}

bool32 PCStorageCommit(bool32 overwrite)
{
    if (!sInitialized) PCStorageReset(TRUE);
    // Check before flushing: a rejected multiplayer save must not write an
    // existing PC log, and trade probes must not overwrite an active fold's
    // shared page buffer.
    if (PCStorageBusy() || PCStorageFaulted() || !SaveFlashAvailable() || LinkBattleStateActive())
        return FALSE;
    if (!overwrite && TradeJournalBlocksSaving()) return FALSE;
    if (!PCMoveJournalFlush()) { sFault = TRUE; return FALSE; }
    u8 pages[JOURNAL_COUNT] = {0, 1};
    u32 count = NUM_CORE_SAVE_SECTORS;
    bool32 dirty[PC_CACHE_BOXES];
    if (sBusy || sFault || !SaveFlashAvailable() || LinkBattleStateActive())
        return FALSE;
    if (!overwrite && TradeJournalBlocksSaving())
        return FALSE;
    sBusy = TRUE;
    if (!overwrite && !PCStorageRecover())
        goto failed;
    // Determine dirtiness BEFORE initializing a new file's blank backing pages.
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        dirty[i] = CacheDirty(i);
    if (sFresh && !InitializeDisk(overwrite))
        goto failed;
    CopyPartyAndObjectsToSave();
    CompactSavedParty();
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (dirty[i])
        {
            u32 first = sCache[i].box * BOX_BYTES;
            u32 last = first + BOX_BYTES - 1;
            for (u32 page = first / SECTOR_DATA_SIZE; page <= last / SECTOR_DATA_SIZE; page++)
                if (!AddPage(pages, &count, page + NUM_CORE_SAVE_SECTORS))
                    goto failed;
        }
    if (sCarry.dirty && !AddPage(pages, &count, CARRY_PAGE))
        goto failed;
    // Recover retired the previous seal. Each image writer erases its own
    // destination; pre-erasing all six adds avoidable latency and flash wear.
    u32 generation = (gSaveCounter + 1) & 0xFFFFFF;
    for (u32 i = 0; i < count; i++)
    {
        if (!BuildPage(pages[i]))
            goto failed;
        gSaveDataBuffer.counter = (generation << 8) | (count << 5) | pages[i];
        gSaveDataBuffer.signature = i + 1 == count ? PC_COMMIT_MAGIC : PC_IMAGE_MAGIC;
        u32 crc = JournalCRC();
        gSaveDataBuffer.id = crc;
        gSaveDataBuffer.checksum = crc >> 16;
        if (!SaveFlashWriteBuffer(sJournalSectors[i]))
            goto failed;
    }
    if (!PCStorageRecover())
        goto failed;
    gSaveCounter = generation;
    gSaveFileStatus = SAVE_STATUS_OK;
    gDifferentSaveFile = FALSE;
    sCoreHash = CoreHash();
    sClockHash = ClockHash();
    sCarry.savedHash = CarryHash();
    sCarry.dirty = FALSE;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box < TOTAL_BOXES_COUNT)
            SaveCacheBaseline(i);
    sSavedBox = gSaveData.currentBox;
    sSaveRequested = FALSE;
    sBusy = FALSE;
    return TRUE;
failed:
    sBusy = FALSE;
    sFault = TRUE;
    return FALSE;
}

bool32 PCStorageAutosave(void)
{
    if (!sInitialized) PCStorageReset(TRUE);
    if (PCStorageBusy() || PCStorageFaulted() || LinkBattleStateActive() || TradeJournalBlocksSaving())
        return FALSE;
    if (!PCMoveJournalFlush()) { sFault = TRUE; return FALSE; }
    CopyPartyAndObjectsToSave();
    CompactSavedParty();
    if (sCarry.savedHash != CarryHash())
        sCarry.dirty = TRUE;
    if (!sCarry.dirty && !BoxesDirty() && sCoreHash == CoreHash())
    {
        sSaveRequested = FALSE;
        return TRUE;
    }
    return PCStorageCommit(gDifferentSaveFile);
}

void PCStorageRequestAutosave(void) { sSaveRequested = TRUE; }

bool32 PCStorageSaveIfRequested(void)
{
    return !PCStorageFaulted() && (!sSaveRequested || (PCMoveJournalSession() ? PCStorageQueueSave(FALSE) : PCStorageAutosave()));
}

void PCStorageJournalCommitted(const struct SaveData *core)
{
    sCoreHash = CoreHash();
    sClockHash = ClockHashFor(core);
    sSavedBox = core->currentBox;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box < TOTAL_BOXES_COUNT) SaveCacheBaseline(i);
    sCarry.savedHash = CarryHash();
    sCarry.dirty = FALSE;
    sSaveRequested = FALSE;
}

bool32 PCStorageBeginSession(void)
{
    if (PCStorageBusy() || PCStorageFaulted() || sFresh || LinkBattleStateActive() || TradeJournalBlocksSaving()) return FALSE;
    return PCMoveJournalStart();
}

bool32 PCStorageTick(void)
{
    if (LinkBattleStateActive()) return FALSE;
    if (!PCMoveJournalTick()) { sFault = TRUE; return FALSE; }
    return TRUE;
}

bool32 PCStorageQueueSave(bool32 force)
{
    if (!PCMoveJournalSession()) return force ? PCStorageClose() : PCStorageAutosave();
    // Session entry already excludes pending trades. Starting a trade first
    // checkpoints/closes this session; do not reread its flash journal for
    // every PC edit (or clobber the page staging buffer with a trade probe).
    if (PCStorageFaulted() || PCStorageBusy() || LinkBattleStateActive()) return FALSE;
    CopyPartyAndObjectsToSave();
    CompactSavedParty();
    if (sCarry.savedHash != CarryHash()) sCarry.dirty = TRUE;
    if (!force && !sCarry.dirty && !BoxesDirty() && sCoreHash == CoreHash())
    {
        sSaveRequested = FALSE;
        return TRUE;
    }
    PCMoveJournalBegin();
    if (!PCMoveJournalCoreChanges()) goto failed;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box < TOTAL_BOXES_COUNT)
            for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
                if (sCache[i].monHashes[slot] != HashBytes(2166136261u, &sCache[i].mons[slot], sizeof(struct BoxPokemon)))
                    if (!PCMoveJournalPatch(NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE + sCache[i].box * BOX_BYTES
                        + slot * sizeof(struct BoxPokemon), &sCache[i].mons[slot], sizeof(struct BoxPokemon))) goto failed;
    if (sCarry.dirty)
    {
        u8 *carry = gSaveDataBuffer.data;
        memset(carry, 0, CARRY_BYTES);
        memcpy(carry, &sCarry.header, sizeof(sCarry.header));
        if (sCarry.header.count)
        {
            if (sCarry.single) SaveMonToBox((void *)(carry + sizeof(sCarry.header)), sCarry.mons);
            else memcpy(carry + sizeof(sCarry.header), sCarry.mons, sCarry.header.count * sizeof(struct BoxPokemon));
        }
        if (!PCMoveJournalPatch(NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE + CARRY_OFFSET, carry, CARRY_BYTES)) goto failed;
    }
    if (!PCMoveJournalSubmit()) goto failed;
    return TRUE;
failed:
    // Unusually large non-PC/script edits use the existing full-save path.
    // No compact record has been published, so this fallback remains atomic.
    return PCStorageCommit(FALSE);
}

bool32 PCStorageQueueClose(void)
{
    if (!PCMoveJournalSession())
    {
        if (!PCMoveJournalPending()) return PCStorageClose();
        if (!PCStorageBeginSession()) return FALSE;
    }
    // The sealed edit log is already a durable save. Do not rewrite its home
    // pages just because the UI closes; keep overlays alive outside the PC.
    return PCStorageQueueSave(TRUE) && PCMoveJournalSuspend();
}

bool32 PCStorageClose(void)
{
    // Remember the selected box and elapsed time once on exit, not while idle.
    return gSaveData.currentBox != sSavedBox || sClockHash != ClockHash()
        ? PCStorageCommit(gDifferentSaveFile) : PCStorageAutosave();
}

static bool32 LoadCacheBox(u32 entry, u32 box, u32 *scratchPage)
{
    u32 start = box * BOX_BYTES;
    if (sFresh)
        memset(sCache[entry].mons, 0, BOX_BYTES);
    else
    {
        u32 first = start / SECTOR_DATA_SIZE;
        u32 last = (start + BOX_BYTES - 1) / SECTOR_DATA_SIZE;
        for (u32 page = first; page <= last; page++)
        {
            u32 physical = page + NUM_CORE_SAVE_SECTORS;
            if (*scratchPage != physical)
            {
                SaveFlashRead(physical, 0, &gSaveDataBuffer, SECTOR_SIZE);
                if (gSaveDataBuffer.signature != SECTOR_SIGNATURE || gSaveDataBuffer.id != physical
                    || gSaveDataBuffer.checksum != SaveSectorChecksum(gSaveDataBuffer.data, SECTOR_DATA_SIZE))
                {
                    sFault = TRUE;
                    return FALSE;
                }
                *scratchPage = physical;
            }
            // Copy from the page we just validated, reusing it for an adjacent
            // box if the two share a sector. No second flash read or box buffer.
            u32 pageStart = page * SECTOR_DATA_SIZE;
            u32 begin = max(start, pageStart), end = min(start + BOX_BYTES, pageStart + SECTOR_DATA_SIZE);
            memcpy((u8 *)sCache[entry].mons + begin - start, gSaveDataBuffer.data + begin - pageStart, end - begin);
        }
    }
    PCMoveJournalOverlay(NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE + start, sCache[entry].mons, BOX_BYTES);
    sCache[entry].box = box;
    SaveCacheBaseline(entry);
    return TRUE;
}

bool32 PCStorageWindow(u32 box)
{
    if (!sInitialized) PCStorageReset(TRUE);
    if (box >= TOTAL_BOXES_COUNT || sFault)
        return FALSE;
    u8 wanted[PC_CACHE_BOXES] = {(box + TOTAL_BOXES_COUNT - 1) % TOTAL_BOXES_COUNT, box, (box + 1) % TOTAL_BOXES_COUNT};
    bool32 missing = FALSE;
    u32 scratchPage = UINT32_MAX;
    for (u32 w = 0; w < PC_CACHE_BOXES; w++)
    {
        bool32 found = FALSE;
        for (u32 i = 0; i < PC_CACHE_BOXES; i++)
            if (sCache[i].box == wanted[w]) found = TRUE;
        missing |= !found;
    }
    if (!missing)
        return TRUE;
    if (sCarry.savedHash != CarryHash())
        sCarry.dirty = TRUE;
    if (BoxesDirty() || sCarry.dirty)
    {
        if (PCMoveJournalSession())
        {
            // Direct party-to-box deposits can request a distant window in
            // the middle of one C call. Seal the small pickup record before
            // eviction, without checkpointing the entire session/save.
            if (!PCStorageQueueSave(FALSE)) return FALSE;
            while (PCStorageBusy()) if (!PCStorageTick()) return FALSE;
        }
        else if (!PCStorageCommit(gDifferentSaveFile))
            return FALSE; // Never discard dirty data after a failed write.
    }
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box != wanted[0] && sCache[i].box != wanted[1] && sCache[i].box != wanted[2])
            sCache[i].box = INVALID_BOX;
    for (u32 w = 0; w < PC_CACHE_BOXES; w++)
    {
        bool32 found = FALSE;
        for (u32 i = 0; i < PC_CACHE_BOXES; i++)
            if (sCache[i].box == wanted[w]) found = TRUE;
        if (found) continue;
        for (u32 i = 0; i < PC_CACHE_BOXES; i++)
            if (sCache[i].box == INVALID_BOX)
            {
                if (!LoadCacheBox(i, wanted[w], &scratchPage)) return FALSE;
                break;
            }
    }
    return TRUE;
}

struct BoxPokemon *PCStorageBox(u32 box)
{
    if (!sInitialized) PCStorageReset(TRUE);
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box == box)
            return sCache[i].mons;
    if (PCStorageWindow(box))
        for (u32 i = 0; i < PC_CACHE_BOXES; i++)
            if (sCache[i].box == box)
                return sCache[i].mons;
    // Fail closed: callers can finish their current frame, but no more saving is
    // allowed. The PC callback switches to an explicit restart/recovery screen.
    sFault = TRUE;
    memset(gSaveDataBuffer.data, 0, BOX_BYTES);
    return (void *)gSaveDataBuffer.data;
}

bool32 PCStorageContainsPointer(const void *pointer)
{
    if (!sInitialized) return FALSE;
    u32 address = (u32)pointer;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box < TOTAL_BOXES_COUNT && address >= (u32)sCache[i].mons
            && address < (u32)sCache[i].mons + BOX_BYTES)
            return TRUE;
    return FALSE;
}

bool32 PCStorageOpen(void)
{
    if (PCStorageBusy() || PCStorageFaulted() || LinkBattleStateActive()
        || (!gDifferentSaveFile && TradeJournalNeedsPeer())) return FALSE;
    if (!gDifferentSaveFile && PCMoveJournalPending())
    {
        // Preserve the pre-PC autosave of party/bag/field changes, but append
        // them to the existing log instead of draining and erasing it on entry.
        // Entry guards above already checked trade ownership; do not rescan
        // the trade sectors a second time just to resume this same log.
        if (!PCMoveJournalStart() || !PCStorageQueueSave(TRUE)) return FALSE;
        while (PCStorageBusy()) if (!PCStorageTick()) return FALSE;
        // A large edit can take the full-save fallback and close the log.
        if (PCMoveJournalPending() && !PCMoveJournalSuspend()) return FALSE;
    }
    else if (!PCStorageCommit(gDifferentSaveFile)) return FALSE;
    return PCStorageWindow(gSaveData.currentBox);
}

void PCStorageCarrySingle(u32 originBox, u32 originSlot, const struct Pokemon *mon)
{
    if (!PCStorageCarrying())
    {
        if (!PCMoveJournalSession() && !PCStorageAutosave()) return;
        sCarry.header.magic = CARRY_MAGIC;
        sCarry.header.count = 1;
        sCarry.header.originBox = originBox;
        sCarry.header.positions[0] = originSlot;
    }
    sCarry.header.mail = mon->mail;
    sCarry.single = TRUE;
    sCarry.mons = mon;
    sCarry.dirty = TRUE;
    PCStorageRequestAutosave();
}

void PCStorageCarryGroup(u32 originBox, u32 firstSlot, u32 columns, u32 rows, const struct BoxPokemon *mons)
{
    if (!PCMoveJournalSession() && !PCStorageAutosave()) return;
    sCarry.header.magic = CARRY_MAGIC;
    sCarry.header.originBox = originBox;
    sCarry.header.count = columns * rows;
    for (u32 r = 0, index = 0; r < rows; r++)
        for (u32 c = 0; c < columns; c++)
            sCarry.header.positions[index++] = firstSlot + r * IN_BOX_COLUMNS + c;
    sCarry.single = FALSE;
    sCarry.mons = mons;
    sCarry.dirty = TRUE;
    PCStorageRequestAutosave();
}

void PCStorageCarryChanged(void) { sCarry.dirty = TRUE; PCStorageRequestAutosave(); }

static void ClearEmptyCarry(void)
{
    if (sCarry.header.count) return;
    enum Item item = sCarry.header.item;
    memset(&sCarry.header, 0, sizeof(sCarry.header));
    sCarry.header.item = item;
    if (item != ITEM_NONE) sCarry.header.magic = CARRY_MAGIC;
    sCarry.single = FALSE;
    sCarry.mons = NULL;
}

void PCStorageCarryEnd(void)
{
    sCarry.header.count = 0;
    sCarry.mons = NULL;
    ClearEmptyCarry();
    sCarry.dirty = TRUE;
    PCStorageRequestAutosave();
}

void PCStorageCarryItem(enum Item item)
{
    if (sCarry.header.item != item)
    {
        sCarry.header.magic = CARRY_MAGIC;
        sCarry.header.item = item;
        ClearEmptyCarry();
        sCarry.dirty = TRUE;
        PCStorageRequestAutosave();
    }
}

bool32 PCStorageRecoverCarry(void)
{
    struct CarryHeader header;
    ReadPCBytes(CARRY_OFFSET, &header, sizeof(header));
    if (header.count == 0 && header.item == ITEM_NONE)
        return TRUE;
    if (header.magic != CARRY_MAGIC || header.count > IN_BOX_COUNT
        || header.originBox > TOTAL_BOXES_COUNT || header.item >= ITEMS_COUNT)
        return FALSE;
    // Startup only: reuse the third (not yet loaded) box buffer for the carry
    // payload. No fourth box-sized RAM allocation, even during recovery.
    ReadPCBytes(CARRY_OFFSET + sizeof(header), sCache[2].mons, BOX_BYTES);
    if (header.count && header.originBox < TOTAL_BOXES_COUNT)
    {
        ReadPCBytes(header.originBox * BOX_BYTES, sCache[0].mons, BOX_BYTES);
        sCache[0].box = header.originBox;
        sCache[0].savedHash = HashBytes(2166136261u, sCache[0].mons, BOX_BYTES);
    }
    for (u32 i = 0; i < header.count; i++)
    {
        if (sCache[2].mons[i].data.species == SPECIES_NONE) continue;
        u32 slot = header.positions[i];
        if (header.originBox == TOTAL_BOXES_COUNT)
        {
            for (slot = 0; slot < PARTY_SIZE; slot++)
                if (gParties[B_TRAINER_PLAYER][slot].box.data.species == SPECIES_NONE) break;
            if (slot == PARTY_SIZE) return FALSE;
            BoxMonToMon(&sCache[2].mons[i], &gParties[B_TRAINER_PLAYER][slot]);
            gParties[B_TRAINER_PLAYER][slot].mail = header.mail;
        }
        else
        {
            if (slot >= IN_BOX_COUNT || sCache[0].mons[slot].data.species != SPECIES_NONE)
                return FALSE;
            sCache[0].mons[slot] = sCache[2].mons[i];
        }
    }
    if (header.item != ITEM_NONE && !AddBagItem(header.item, 1))
        return FALSE;
    gPartiesCount[B_TRAINER_PLAYER] = CalculatePlayerPartyCount();
    PCStorageCarryEnd();
    bool32 ok = PCStorageCommit(FALSE);
    return ok;
}

#if TESTING
u32 PCStorageTestResidentCount(void)
{
    u32 count = 0;
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        count += sCache[i].box < TOTAL_BOXES_COUNT;
    return count;
}
bool32 PCStorageTestResident(u32 box)
{
    for (u32 i = 0; i < PC_CACHE_BOXES; i++)
        if (sCache[i].box == box) return TRUE;
    return FALSE;
}
#endif
