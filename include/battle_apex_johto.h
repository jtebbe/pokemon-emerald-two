#ifndef GUARD_BATTLE_APEX_JOHTO_H
#define GUARD_BATTLE_APEX_JOHTO_H

bool32 TryJohtoEntryAbility(enum BattlerId battler, enum Ability ability);
enum BattlerId GetCopyrightFraudTarget(enum BattlerId battler);
bool32 ApplyCopyrightFraud(enum BattlerId battler, bool32 recordHistory);
enum BattlerId GetLonLonBredTarget(enum BattlerId battler);
u32 GetEmblazenedBurnDamage(enum BattlerId battler);

bool32 TryThunderRunnerTerrain(enum BattlerId battler);
bool32 PurifyingNomadHasStatus(enum BattlerId battler);
u32 GetGoldenRainbowCandidates(enum BattlerId battler);
bool32 TryEruptionHeraldFollowUp(bool32 endTurn);
void StartEndTurnEruptionHerald(enum BattlerId battler);

#endif
