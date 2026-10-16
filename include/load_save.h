#ifndef GUARD_LOAD_SAVE_H
#define GUARD_LOAD_SAVE_H

#include "pokemon_storage_system.h"
#include "save.h"



extern bool32 gFlashMemoryPresent;

void CheckForFlashMemory(void);
void ResetSaveData(void);
void ResetGameProgress(void);
void ResetHeapPreservingSaveData(void);
u32 UseContinueGameWarp(void);
void ClearContinueGameWarpStatus(void);
void SetContinueGameWarpStatus(void);
void SetContinueGameWarpStatusToDynamicWarp(void);
void ClearContinueGameWarpStatus2(void);
void SavePlayerParty(void);
void LoadPlayerParty(void);
void SaveObjectEvents(void);
void LoadObjectEvents(void);
void CopyPartyAndObjectsToSave(void);
void CopyPartyAndObjectsFromSave(void);
void SnapshotLinkInventory(void);
void RestoreLinkInventory(void);
void SnapshotLinkBattleState(void);
void RestoreLinkBattleState(void);
bool32 LinkBattleStateActive(void);
#endif // GUARD_LOAD_SAVE_H
