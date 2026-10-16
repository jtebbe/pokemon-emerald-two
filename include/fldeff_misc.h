#ifndef GUARD_FLDEFF_MISC_H
#define GUARD_FLDEFF_MISC_H

void ComputerScreenOpenEffect(u16 increment, u16 unused, u8 priority);
void ComputerScreenCloseEffect(u16 increment, u16 unused, u8 priority);
bool8 IsComputerScreenOpenEffectActive(void);
bool8 IsComputerScreenCloseEffectActive(void);
bool8 FldEff_Nop47(void);
bool8 FldEff_Nop48(void);
bool8 FldEff_SandPillar(void);
void FldEffPoison_Start(void);
bool32 FldEffPoison_IsActive(void);
void DoWateringBerryTreeAnim(void);

extern const struct SpritePalette gFieldEffectObjectPaletteInfo9;

#endif //GUARD_FLDEFF_MISC_H
