#include "global.h"
#include "save.h"
#include "load_save.h"
#include "gba/flash_internal.h"
#include "pc_storage.h"

#if TESTING
static EWRAM_DATA u8 sTestReserved[8][SECTOR_SIZE];
static u8 *sTestNormal;
static s32 sFailAfterBytes;
static s32 sFailWriteSector;
static u32 sTestWrites;
static u32 sTestReads;
static u32 sTestReadBytes;
static u32 sTestErases;
static u32 sTestProgramBytes;

void PCStorageTestFlash(u8 *normalSectors, s32 failAfterBytes)
{
    sTestNormal = normalSectors;
    sFailAfterBytes = failAfterBytes;
    sFailWriteSector = -1;
    sTestWrites = 0;
    sTestReads = 0;
    sTestReadBytes = 0;
    sTestErases = 0;
    sTestProgramBytes = 0;
    if (normalSectors != NULL)
        memset(sTestReserved, 0xFF, sizeof(sTestReserved));
}

void PCStorageTestFailure(s32 failAfterBytes)
{
    sFailAfterBytes = failAfterBytes;
}

void PCStorageTestWriteFailure(s32 sector)
{
    sFailWriteSector = sector;
}

u32 PCStorageTestWrites(void)
{
    return sTestWrites;
}

u32 PCStorageTestReads(void) { return sTestReads; }
u32 PCStorageTestReadBytes(void) { return sTestReadBytes; }
u32 PCStorageTestErases(void) { return sTestErases; }
u32 PCStorageTestProgramBytes(void) { return sTestProgramBytes; }

static u8 *TestSector(u32 sector)
{
    return sector < NUM_SECTORS_PER_SLOT ? sTestNormal + sector * SECTOR_SIZE
                                       : sTestReserved[sector - NUM_SECTORS_PER_SLOT];
}
#endif

bool32 SaveFlashAvailable(void)
{
#if TESTING
    if (sTestNormal != NULL)
        return TRUE;
#endif
    return gFlashMemoryPresent;
}

void SaveFlashRead(u32 sector, u32 offset, void *dest, u32 size)
{
#if TESTING
    sTestReads++;
    sTestReadBytes += size;
    if (sTestNormal != NULL)
    {
        memcpy(dest, TestSector(sector) + offset, size);
        return;
    }
#endif
    ReadFlash(sector, offset, dest, size);
}

bool32 SaveFlashErase(u32 sector)
{
#if TESTING
    sTestErases++;
    if (sTestNormal != NULL)
    {
        if (sFailAfterBytes == 0)
            return FALSE;
        memset(TestSector(sector), 0xFF, SECTOR_SIZE);
        return TRUE;
    }
#endif
    return SaveFlashAvailable() && EraseFlashSector(sector) == 0;
}

bool32 SaveFlashWriteBuffer(u32 sector)
{
    u8 verify[64];
    const u8 *src = (void *)&gSaveDataBuffer;
    if (!SaveFlashAvailable())
        goto failed;
#if TESTING
    if (sTestNormal != NULL)
    {
        if (sector == sFailWriteSector)
            goto failed;
        if (!SaveFlashErase(sector)) goto failed;
        sTestWrites++;
        for (u32 n = 0; n < SECTOR_SIZE; n++)
        {
            u32 offset = n < SECTOR_SIGNATURE_OFFSET ? n
                       : n < SECTOR_SIZE - 4 ? n + 4
                       : SECTOR_SIGNATURE_OFFSET + n - (SECTOR_SIZE - 4);
            if (sFailAfterBytes == 0)
                goto failed;
            if (sFailAfterBytes > 0)
                sFailAfterBytes--;
            TestSector(sector)[offset] &= src[offset];
        }
    }
    else
#endif
    {
        // The sector programmer sets up its bank and RAM polling routine once,
        // rather than repeating that work for all 4,096 bytes. Leave the seal
        // erased until the entire sector body has been programmed successfully.
        u32 signature = gSaveDataBuffer.signature;
        gSaveDataBuffer.signature = 0xFFFFFFFF;
#if TESTING
        sTestErases++; // The sector driver erases its destination internally.
#endif
        u16 result = ProgramFlashSector(sector, (u8 *)&gSaveDataBuffer);
        gSaveDataBuffer.signature = signature;
        if (result != 0) goto failed;
#if TESTING
        sTestWrites++;
#endif
        for (u32 offset = SECTOR_SIGNATURE_OFFSET; offset < SECTOR_SIGNATURE_OFFSET + sizeof(signature); offset++)
            if (ProgramFlashByte(sector, offset, src[offset]))
                goto failed;
    }
    for (u32 offset = 0; offset < SECTOR_SIZE; offset += sizeof(verify))
    {
        SaveFlashRead(sector, offset, verify, sizeof(verify));
        if (memcmp(verify, src + offset, sizeof(verify)) != 0)
            goto failed;
    }
    gDamagedSaveSectors &= ~(1u << sector);
    return TRUE;
failed:
    gDamagedSaveSectors |= 1u << sector;
    return FALSE;
}

bool32 SaveFlashProgram(u32 sector, u32 offset, const void *data, u32 size)
{
    const u8 *bytes = data;
    u8 verify[64];
    if (!SaveFlashAvailable() || sector >= SECTORS_COUNT || offset > SECTOR_SIZE || size > SECTOR_SIZE - offset)
        goto failed;
#if TESTING
    if (sTestNormal != NULL)
    {
        for (u32 i = 0; i < size; i++)
        {
            if (sFailAfterBytes == 0) goto failed;
            if (sFailAfterBytes > 0) sFailAfterBytes--;
            TestSector(sector)[offset + i] &= bytes[i];
            sTestProgramBytes++;
        }
    }
    else
#endif
    {
        if (ProgramFlashBytes_MX(sector, offset, bytes, size)) goto failed;
#if TESTING
        sTestProgramBytes += size;
#endif
    }
    for (u32 i = 0; i < size; i += sizeof(verify))
    {
        u32 count = min(sizeof(verify), size - i);
        SaveFlashRead(sector, offset + i, verify, count);
        if (memcmp(verify, bytes + i, count)) goto failed;
    }
    return TRUE;
failed:
    gDamagedSaveSectors |= 1u << sector;
    return FALSE;
}

u16 SaveSectorChecksum(const void *data, u32 size)
{
    const u32 *words = data;
    u32 sum = 0;
    for (u32 i = 0; i < size / 4; i++)
        sum += words[i];
    return (sum >> 16) + sum;
}

u32 SaveSectorDataSize(u32 sector)
{
    if (sector >= NUM_CORE_SAVE_SECTORS)
        return SECTOR_DATA_SIZE;
    u32 offset = sector * SECTOR_DATA_SIZE;
    return offset < sizeof(gSaveData) ? min(sizeof(gSaveData) - offset, SECTOR_DATA_SIZE) : 0;
}
