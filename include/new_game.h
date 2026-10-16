#ifndef GUARD_NEW_GAME_H
#define GUARD_NEW_GAME_H

extern bool8 gDifferentSaveFile;

void SetTrainerId(u32 trainerId, u8 *dst);
u32 GetTrainerId(u8 *trainerId);
void CopyTrainerId(u8 *dst, u8 *src);
void NewGameInitData(void);
void ResetMenuAndMonGlobals(void);
void ResetSaveDataAndOptions(void);

#endif // GUARD_NEW_GAME_H
