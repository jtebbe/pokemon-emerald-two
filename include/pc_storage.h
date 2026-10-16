#ifndef GUARD_PC_STORAGE_H
#define GUARD_PC_STORAGE_H

#define PC_CACHE_BOXES 3

// A pointer is valid only until the window changes. Never retain it across an
// arbitrary box lookup; copy a mon when two distant boxes are involved.
struct BoxPokemon *PCStorageBox(u32 box);
bool32 PCStorageWindow(u32 box);
bool32 PCStorageContainsPointer(const void *pointer);
void PCStorageReset(bool32 fresh);
bool32 PCStorageCommit(bool32 overwrite);
bool32 PCStorageRecover(void);
bool32 PCStorageRecoverCarry(void);
bool32 PCStorageFaulted(void);
bool32 PCStorageOpen(void);
bool32 PCStorageAutosave(void);
void PCStorageRequestAutosave(void);
bool32 PCStorageSaveIfRequested(void);
bool32 PCStorageClose(void);
bool32 PCStorageBusy(void);
bool32 PCStorageBeginSession(void);
bool32 PCStorageQueueSave(bool32 force);
bool32 PCStorageQueueClose(void);
bool32 PCStorageTick(void);
void PCStorageReportFault(void);

// The carry payload uses the PC's existing moving-mon buffers, not another box.
void PCStorageCarrySingle(u32 originBox, u32 originSlot, const struct Pokemon *mon);
void PCStorageCarryGroup(u32 originBox, u32 firstSlot, u32 columns, u32 rows, const struct BoxPokemon *mons);
void PCStorageCarryChanged(void);
void PCStorageCarryEnd(void);
void PCStorageCarryItem(enum Item item);
bool32 PCStorageCarrying(void);

#if TESTING
// Normal sectors in the test heap; recovery/Hall of Fame sectors are test-only
// static backing. No hidden forty-box RAM allocation in the shipping ROM.
void PCStorageTestFlash(u8 *normalSectors, s32 failAfterBytes);
void PCStorageTestFailure(s32 failAfterBytes);
void PCStorageTestWriteFailure(s32 sector);
u32 PCStorageTestResidentCount(void);
bool32 PCStorageTestResident(u32 box);
u32 PCStorageTestWrites(void);
u32 PCStorageTestReads(void);
u32 PCStorageTestReadBytes(void);
u32 PCStorageTestErases(void);
u32 PCStorageTestProgramBytes(void);
#endif
#endif
