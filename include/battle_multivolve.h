#ifndef GUARD_BATTLE_MULTIVOLVE_H
#define GUARD_BATTLE_MULTIVOLVE_H

void ApplyMultivolve(enum BattlerId battler);
void RefreshMultivolveStats(enum BattlerId battler);
u16 GetMultivolvePartyHp(enum BattlerId battler, u16 hp);

#endif
