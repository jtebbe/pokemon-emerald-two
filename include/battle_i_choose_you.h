#ifndef GUARD_BATTLE_I_CHOOSE_YOU_H
#define GUARD_BATTLE_I_CHOOSE_YOU_H

#include "battle.h"

struct DamageContext;

enum PokeBall GetBattlerIChooseYouBall(enum BattlerId battler, enum Ability ability);
bool32 HasComatoseAbility(enum BattlerId battler, enum Ability ability);
bool32 IsDuskBallBoosted(void);
bool32 TryIChooseYouEntry(enum BattlerId battler);
void AI_ApplyIChooseYouEntry(enum BattlerId battler);
uq4_12_t GetIChooseYouDamageModifier(struct DamageContext *ctx);

#endif
