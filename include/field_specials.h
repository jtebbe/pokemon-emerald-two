#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

u8 GetPlayerTrainerIdOnesDigit(void);
u8 GetBattleOutcome(void);
void IsGrassTypeInParty(void);
bool8 Special_AreLeadMonEVsMaxedOut(void);
bool8 ScriptCheckFreePokemonStorageSpace(void);
u8 GetLeadMonIndex(void);
enum Species ScriptGetPartyMonSpecies(void);
bool8 BufferTMHMMoveName(void);
void SetPCBoxToSendMon(u8 boxId);
u16 GetPCBoxToSendMon(void);
bool8 ShouldShowBoxWasFullMessage(void);
bool8 IsDestinationBoxFull(void);
void GetObjectPosition(u16* xPointer, u16* yPointer, u32 localId, u32 useTemplate);
bool32 CheckObjectAtXY(u32 x, u32 y);
bool32 CheckPartyHasSpecies(enum Species givenSpecies);
void SetSeenMon(void);
bool8 DoesPlayerPartyContainSpecies(void);
bool8 PlayerPartyContainsSpeciesWithPlayerID(void);

void SetHiddenItemFlag(void);
bool8 CheckAddCoins(void);

#endif // GUARD_FIELD_SPECIALS_H
