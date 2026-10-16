#include "global.h"
#include "save.h"
#include "load_save.h"
#include "pc_storage.h"
#include "hall_of_fame.h"
#include "link.h"

COMMON_DATA u32 gDamagedSaveSectors = 0;
COMMON_DATA u32 gSaveCounter = 0;
COMMON_DATA u16 gSaveFileStatus = 0;
COMMON_DATA u16 gSaveAttemptStatus = 0;
EWRAM_DATA struct SaveSector gSaveDataBuffer = {0};

u16 GetHallOfFameEntryCount(void) { return gSaveData.hallOfFameEntries; }
u32 GetFirstHallOfFameTime(void) { return gSaveData.firstHallOfFameTime; }

void RecordHallOfFameEntry(void)
{
    if (gSaveData.hallOfFameEntries == 0)
        gSaveData.firstHallOfFameTime = ((u32)gSaveData.playTimeHours << 16)
                                    | (gSaveData.playTimeMinutes << 8) | gSaveData.playTimeSeconds;
    if (gSaveData.hallOfFameEntries < 999)
        gSaveData.hallOfFameEntries++;
}

void ClearSaveData(void)
{
    for (u32 i = 0; i < SECTORS_COUNT; i++)
        SaveFlashErase(i);
    PCStorageReset(TRUE);
}

void Save_ResetSaveCounters(void)
{
    gSaveCounter = 0;
    gDamagedSaveSectors = 0;
}

static bool32 WriteHallOfFame(void)
{
    if (gHoFSaveBuffer == NULL)
        return FALSE;
    for (u32 i = 0; i < NUM_HOF_SECTORS; i++)
    {
        memset(&gSaveDataBuffer, 0, sizeof(gSaveDataBuffer));
        memcpy(gSaveDataBuffer.data, (u8 *)gHoFSaveBuffer + i * SECTOR_DATA_SIZE, SECTOR_DATA_SIZE);
        gSaveDataBuffer.id = SaveSectorChecksum(gSaveDataBuffer.data, SECTOR_DATA_SIZE);
        gSaveDataBuffer.signature = SECTOR_SIGNATURE;
        if (!SaveFlashWriteBuffer(SECTOR_ID_HOF_1 + i))
            return FALSE;
    }
    return TRUE;
}

u8 HandleSavingData(u8 saveType)
{
    bool32 overwrite = saveType == SAVE_OVERWRITE_DIFFERENT_FILE;
    if (LinkBattleStateActive() || (!overwrite && TradeJournalBlocksSaving()))
        return SAVE_STATUS_ERROR;
    if (saveType == SAVE_HALL_OF_FAME && gHoFSaveBuffer == NULL)
        return SAVE_STATUS_ERROR;
    if (saveType == SAVE_HALL_OF_FAME)
        RecordHallOfFameEntry();
    if (!PCStorageCommit(overwrite))
        return SAVE_STATUS_ERROR;
    if (saveType == SAVE_HALL_OF_FAME && !WriteHallOfFame())
        return SAVE_STATUS_ERROR;
    return SAVE_STATUS_OK;
}

u8 TrySavingData(u8 saveType)
{
    if (!SaveFlashAvailable() || LinkBattleStateActive()
        || (saveType != SAVE_OVERWRITE_DIFFERENT_FILE && TradeJournalBlocksSaving()))
        return SAVE_STATUS_ERROR;
    gSaveAttemptStatus = HandleSavingData(saveType);
    if (gSaveAttemptStatus != SAVE_STATUS_OK && PCStorageFaulted())
        PCStorageReportFault(); // Never run the legacy sector-wiping recovery.
    return gSaveAttemptStatus;
}

bool32 TrySavingHallOfFame(void)
{
    if (TrySavingData(SAVE_HALL_OF_FAME) == SAVE_STATUS_OK)
        return TRUE;

    // Stop callbacks before callers free their Hall of Fame screen buffers.
    // A history-page write can fail even when the main save committed safely.
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    gSoftResetDisabled = FALSE;
    SetMainCallback2(CB2_SaveErrorScreen);
    return FALSE;
}

static u8 LoadHallOfFame(void)
{
    if (gHoFSaveBuffer == NULL)
        return SAVE_STATUS_ERROR;
    for (u32 i = 0; i < NUM_HOF_SECTORS; i++)
    {
        SaveFlashRead(SECTOR_ID_HOF_1 + i, 0, &gSaveDataBuffer, SECTOR_SIZE);
        if (gSaveDataBuffer.signature != SECTOR_SIGNATURE
            || gSaveDataBuffer.id != SaveSectorChecksum(gSaveDataBuffer.data, SECTOR_DATA_SIZE))
            return SAVE_STATUS_CORRUPT;
        memcpy((u8 *)gHoFSaveBuffer + i * SECTOR_DATA_SIZE, gSaveDataBuffer.data, SECTOR_DATA_SIZE);
    }
    return SAVE_STATUS_OK;
}

u8 LoadGameSave(u8 saveType)
{
    if (!SaveFlashAvailable())
    {
        gSaveFileStatus = SAVE_STATUS_NO_FLASH;
        return SAVE_STATUS_ERROR;
    }
    if (saveType == SAVE_HALL_OF_FAME)
        return LoadHallOfFame();
    PCStorageReset(FALSE);
    if (!PCStorageRecover() || !TradeJournalRestoreHome())
        goto corrupt;
    bool32 anySignature = FALSE, allValid = TRUE;
    u32 counter = 0;
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        SaveFlashRead(i, 0, &gSaveDataBuffer, SECTOR_SIZE);
        bool32 valid = gSaveDataBuffer.signature == SECTOR_SIGNATURE && gSaveDataBuffer.id == i
            && gSaveDataBuffer.checksum == SaveSectorChecksum(gSaveDataBuffer.data, SaveSectorDataSize(i));
        anySignature |= gSaveDataBuffer.signature == SECTOR_SIGNATURE;
        allValid &= valid;
        if (i == 0) counter = gSaveDataBuffer.counter;
        if (i < NUM_CORE_SAVE_SECTORS && valid)
        {
            allValid &= gSaveDataBuffer.counter == counter;
            memcpy((u8 *)&gSaveData + i * SECTOR_DATA_SIZE, gSaveDataBuffer.data, SaveSectorDataSize(i));
        }
    }
    if (!allValid)
    {
        gSaveFileStatus = anySignature ? SAVE_STATUS_CORRUPT : SAVE_STATUS_EMPTY;
        PCStorageReset(TRUE);
        return gSaveFileStatus;
    }
    gSaveCounter = counter;
    gDamagedSaveSectors = 0;
    CopyPartyAndObjectsFromSave();
    if (!PCStorageRecoverCarry() || !PCStorageWindow(gSaveData.currentBox))
        goto corrupt;
    gSaveFileStatus = SAVE_STATUS_OK;
    return SAVE_STATUS_OK;
corrupt:
    gSaveFileStatus = SAVE_STATUS_CORRUPT;
    return SAVE_STATUS_CORRUPT;
}
