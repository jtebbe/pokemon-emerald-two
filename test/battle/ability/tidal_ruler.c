#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Tidal Ruler applies Hydration and boosts Speed only in rain")
{
    bool32 rain, apex;
    PARAMETRIZE { rain = FALSE; apex = TRUE; }
    PARAMETRIZE { rain = TRUE; apex = TRUE; }
    PARAMETRIZE { rain = TRUE; apex = FALSE; }
    ResetStartingStatuses();
    if (rain)
        SetStartingStatus(STARTING_STATUS_WEATHER_RAIN);
    GIVEN {
        PLAYER(SPECIES_KINGDRA) {
            Ability(ABILITY_SNIPER); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE);
            Speed(100); Status1(STATUS1_BURN);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->status1, rain && apex ? STATUS1_NONE : STATUS1_BURN);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)), rain && apex ? 150 : 100);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Tidal Ruler multiplies Attack, Defense, Sp. Atk and Sp. Def by 1.5 in rain", s16 dealtPhysical, s16 tookPhysical, s16 dealtSpecial, s16 tookSpecial)
{
    bool32 apex;
    PARAMETRIZE { apex = TRUE; }
    PARAMETRIZE { apex = FALSE; }
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_WEATHER_RAIN);
    GIVEN {
        PLAYER(SPECIES_KINGDRA) {
            Ability(ABILITY_SNIPER); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE);
            Attack(apex ? 100 : 150); Defense(apex ? 100 : 150);
            SpAttack(apex ? 100 : 150); SpDefense(apex ? 100 : 150);
            HP(1000); MaxHP(1000); Speed(100);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_POUND, criticalHit: FALSE); MOVE(opponent, MOVE_POUND, criticalHit: FALSE); }
        TURN { MOVE(player, MOVE_SWIFT, criticalHit: FALSE); MOVE(opponent, MOVE_SWIFT, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].dealtPhysical);
        HP_BAR(player, captureDamage: &results[i].tookPhysical);
        HP_BAR(opponent, captureDamage: &results[i].dealtSpecial);
        HP_BAR(player, captureDamage: &results[i].tookSpecial);
    } FINALLY {
        EXPECT_EQ(results[0].dealtPhysical, results[1].dealtPhysical);
        EXPECT_EQ(results[0].tookPhysical, results[1].tookPhysical);
        EXPECT_EQ(results[0].dealtSpecial, results[1].dealtSpecial);
        EXPECT_EQ(results[0].tookSpecial, results[1].tookSpecial);
    }
}

SINGLE_BATTLE_TEST("Tidal Ruler only skips accuracy for Water moves in rain")
{
    enum Move move;
    bool32 rain;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; rain = TRUE; }
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; rain = FALSE; }
    PARAMETRIZE { move = MOVE_BLIZZARD; rain = TRUE; }
    ResetStartingStatuses();
    if (rain)
        SetStartingStatus(STARTING_STATUS_WEATHER_RAIN);
    GIVEN {
        PLAYER(SPECIES_SEADRA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, hit: FALSE); }
    } SCENE {
        if (rain && move == MOVE_HYDRO_PUMP) {
            HP_BAR(opponent);
        } else {
            NOT HP_BAR(opponent);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Tidal Ruler AI treats Weather Ball as a sure hit when it becomes Water type in rain")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    ResetStartingStatuses();
    if (rain)
        SetStartingStatus(STARTING_STATUS_WEATHER_RAIN);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_KINGDRA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_WEATHER_BALL); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WEATHER_BALL); }
    } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], rain ? 101 : 100);
    }
}
