#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Infinite Stash AI accounts for resistance berries when choosing damage")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RATICATE_ALOLA) { Ability(ABILITY_THICK_FAT); Item(item); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BUG_BITE, MOVE_SLASH); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE) {
                SCORE_GT(opponent, MOVE_SLASH, MOVE_BUG_BITE);
                EXPECT_MOVE(opponent, MOVE_SLASH);
            } else {
                SCORE_GT(opponent, MOVE_BUG_BITE, MOVE_SLASH);
                EXPECT_MOVE(opponent, MOVE_BUG_BITE);
            }
        }
    }
}

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sStashPrediction;
static bool32 sObservedStashPrediction;
static bool32 sStashOneUseBerryHeuristic;
static s32 ObserveStashPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sStashPrediction = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sStashOneUseBerryHeuristic = gAiLogicData->resistBerryAffected[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sObservedStashPrediction = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Infinite Stash AI does not pretend its renewable resistance berry disappears for a KO")
{
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(75); MaxHP(1200); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BUG_BITE); }
    } WHEN {
        sObservedStashPrediction = FALSE;
        sDynamicAiFunc = ObserveStashPrediction;
        TURN { EXPECT_MOVE(opponent, MOVE_BUG_BITE); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedStashPrediction);
        EXPECT(!sStashOneUseBerryHeuristic);
        EXPECT_LT(sStashPrediction.maximum, 75);
        EXPECT_GE(damage, sStashPrediction.minimum);
        EXPECT_LE(damage, sStashPrediction.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Infinite Stash AI applies resistance to both hits of Double Kick")
{
    s16 damage[2];
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        sObservedStashPrediction = FALSE;
        sDynamicAiFunc = ObserveStashPrediction;
        TURN { EXPECT_MOVE(opponent, MOVE_DOUBLE_KICK); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage[0]);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedStashPrediction);
        EXPECT_GE(damage[0] + damage[1], sStashPrediction.minimum);
        EXPECT_LE(damage[0] + damage[1], sStashPrediction.maximum);
        EXPECT_EQ(damage[0], damage[1]);
    }
}

AI_SINGLE_BATTLE_TEST("Infinite Stash AI values Protect for a helpful end turn berry and respects Unnerve")
{
    enum Item item;
    enum Ability ability;
    u32 initialHp = 600;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_UNNERVE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; initialHp = 1200; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREDICT_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_RATICATE_ALOLA) { Ability(ABILITY_THICK_FAT); Item(item); HP(initialHp); MaxHP(1200); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_SCRATCH);
            SCORE_EQ_VAL(opponent, MOVE_PROTECT, AI_SCORE_DEFAULT + DECENT_EFFECT + (item == ITEM_APEX_METEORITE && ability != ABILITY_UNNERVE ? GOOD_EFFECT : 0));
            EXPECT_MOVE(opponent, MOVE_PROTECT);
        }
    } THEN {
        if (initialHp == 1200) {
            s32 boosts = 0;
            for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
                boosts += opponent->statStages[stat] - DEFAULT_STAT_STAGE;
            EXPECT_GT(boosts, 0);
        } else if (item == ITEM_APEX_METEORITE && ability != ABILITY_UNNERVE) {
            EXPECT_GT(opponent->hp, initialHp);
        } else {
            EXPECT_EQ(opponent->hp, initialHp);
        }
    }
}
