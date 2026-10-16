#ifndef GUARD_SAVE_H
#define GUARD_SAVE_H

#include "main.h"

// Each 4 KiB flash sector contains one contiguous save-image chunk and a footer.
#define SECTOR_DATA_SIZE 4084
#define SECTOR_FOOTER_SIZE 12
#define SECTOR_SIZE (SECTOR_DATA_SIZE + SECTOR_FOOTER_SIZE)

// If the sector's signature field is not this value then the sector is either invalid or empty.
// Streamed forty-box layout; older saves are incompatible.
#define SECTOR_SIGNATURE 0x801202C


#define NUM_CORE_SAVE_SECTORS         2
#define NUM_PC_SAVE_SECTORS          22
#define NUM_SECTORS_PER_SLOT         (NUM_CORE_SAVE_SECTORS + NUM_PC_SAVE_SECTORS)
// 0-1: resident save data; 2-23: streamed boxes; 24-27 and 30-31: recovery.
#define SECTOR_ID_HOME                0
#define SECTOR_ID_HOF_1              28
#define SECTOR_ID_HOF_2              29
#define SECTOR_ID_TRADE_A            30
#define SECTOR_ID_TRADE_B            31
#define SECTORS_COUNT                32

#define NUM_HOF_SECTORS 2

#define SAVE_STATUS_EMPTY    0
#define SAVE_STATUS_OK       1
#define SAVE_STATUS_CORRUPT  2
#define SAVE_STATUS_NO_FLASH 4
#define SAVE_STATUS_ERROR    0xFF

// Do save types
enum
{
    SAVE_NORMAL,
    SAVE_LINK, // Link
    SAVE_HALL_OF_FAME,
    SAVE_OVERWRITE_DIFFERENT_FILE,
};

struct SaveSector
{
    u8 data[SECTOR_DATA_SIZE];
    u16 id;
    u16 checksum;
    u32 signature;
    u32 counter;
}; // size is SECTOR_SIZE (0x1000)

#define SECTOR_SIGNATURE_OFFSET offsetof(struct SaveSector, signature)
#define SECTOR_COUNTER_OFFSET   offsetof(struct SaveSector, counter)

extern u32 gDamagedSaveSectors;
extern u32 gSaveCounter;
extern u16 gSaveFileStatus;

extern struct SaveSector gSaveDataBuffer;

u16 GetHallOfFameEntryCount(void);
u32 GetFirstHallOfFameTime(void);
void RecordHallOfFameEntry(void);
void ClearSaveData(void);
void Save_ResetSaveCounters(void);
u8 HandleSavingData(u8 saveType);
u8 TrySavingData(u8 saveType);
bool32 TrySavingHallOfFame(void);
u8 LoadGameSave(u8 saveType);
bool32 SaveFlashAvailable(void);
void SaveFlashRead(u32 sector, u32 offset, void *dest, u32 size);
bool32 SaveFlashErase(u32 sector);
bool32 SaveFlashWriteBuffer(u32 sector);
bool32 SaveFlashProgram(u32 sector, u32 offset, const void *data, u32 size);
u16 SaveSectorChecksum(const void *data, u32 size);
u32 SaveSectorDataSize(u32 sector);

// Read-only fatal error screens. Neither retries nor erases flash.
void CB2_SaveErrorScreen(void);
void CB2_FlashNotDetectedScreen(void);

#endif // GUARD_SAVE_H
