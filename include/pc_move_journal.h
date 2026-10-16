#ifndef GUARD_PC_MOVE_JOURNAL_H
#define GUARD_PC_MOVE_JOURNAL_H

// Internal PC redo log. Virtual addresses are logical save-page data offsets.
void PCMoveJournalReset(void);
bool32 PCMoveJournalSession(void);
bool32 PCMoveJournalPending(void);
bool32 PCMoveJournalBusy(void);
bool32 PCMoveJournalFailed(void);
bool32 PCMoveJournalStart(void);
bool32 PCMoveJournalTick(void);
bool32 PCMoveJournalClose(void);
// Leave sealed records available for later sessions without checkpointing.
bool32 PCMoveJournalSuspend(void);
bool32 PCMoveJournalFlush(void);
bool32 PCMoveJournalRecover(void);
bool32 PCMoveJournalOverlay(u32 address, void *dest, u32 size);
void PCMoveJournalBegin(void);
bool32 PCMoveJournalPatch(u32 address, const void *data, u32 size);
bool32 PCMoveJournalCoreChanges(void);
bool32 PCMoveJournalSubmit(void);

// Called only after the compact record's final seal has verified.
void PCStorageJournalCommitted(const struct SaveData *durableCore);
#if TESTING
u32 PCMoveJournalTestRecordSize(void);
#endif

#endif
