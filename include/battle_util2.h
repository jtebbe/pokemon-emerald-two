#ifndef GUARD_BATTLE_UTIL2_H
#define GUARD_BATTLE_UTIL2_H

void AllocateBattleResources(void);
void FreeBattleResources(void);
void SwitchPartyOrderInGameMulti(enum BattlerId battler, enum PartyMon partyId);
void AllocateBattleGfxResources(void);
void CloseMainBattleScreen(void);

#endif // GUARD_BATTLE_UTIL_H
