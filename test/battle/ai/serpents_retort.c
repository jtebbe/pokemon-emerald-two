#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("Serpent's Retort AI avoids a fatal attack but knows Steel is immune and Magic Guard does not block it")
{
    u32 hp;
    enum Item item;
    enum Ability ability;
    enum Species species;
    PARAMETRIZE { hp = 800; item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { hp = 1; item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { hp = 1; item = ITEM_NONE; ability = ABILITY_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { hp = 1; item = ITEM_APEX_METEORITE; ability = ABILITY_MAGIC_GUARD; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { hp = 1; item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; species = SPECIES_MAGNEMITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_SHED_SKIN); Item(item); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(species) { Ability(ability); HP(hp); MaxHP(800); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            if (hp == 1 && item == ITEM_APEX_METEORITE && species != SPECIES_MAGNEMITE) {
                SCORE_LT(opponent, MOVE_SCRATCH, MOVE_CELEBRATE);
                EXPECT_MOVE(opponent, MOVE_CELEBRATE);
            } else {
                SCORE_GT(opponent, MOVE_SCRATCH, MOVE_CELEBRATE);
                EXPECT_MOVE(opponent, MOVE_SCRATCH);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Serpent's Retort AI prefers one strike over a similar multihit move to reduce retaliation")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_SHED_SKIN); Item(item); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Attack(100); Moves(MOVE_DOUBLE_HIT, MOVE_SLASH); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE) {
                SCORE_GT(opponent, MOVE_SLASH, MOVE_DOUBLE_HIT);
                EXPECT_MOVE(opponent, MOVE_SLASH);
            } else {
                SCORE_GT(opponent, MOVE_DOUBLE_HIT, MOVE_SLASH);
                EXPECT_MOVE(opponent, MOVE_DOUBLE_HIT);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Serpent's Retort AI knows a single hit KO prevents retaliation")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(800); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 1);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

AI_SINGLE_BATTLE_TEST("Serpent's Retort AI recognizes that the defender's Substitute blocks retaliation")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(800); Attack(1); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(opponent->hp, 1);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

extern AiScoreFunc sDynamicAiFunc;
static s32 sRetortMoveScore;
static u32 sRetortPredictedDamage;
static bool32 sRetortPredictionObserved;
static s32 ObserveRetortPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0 && move == MOVE_SCRATCH
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sRetortMoveScore = score;
        sRetortPredictedDamage = AI_GetSerpentsRetortDamage(battlerAtk, battlerDef, move);
        sRetortPredictionObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Serpent's Retort AI distinguishes harmful poison from immunity and beneficial poison", s32 score)
{
    enum Ability ability;
    s16 damage;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_IMMUNITY; }
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; }
    PARAMETRIZE { ability = ABILITY_POISON_HEAL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(800); MaxHP(800); Moves(MOVE_SCRATCH); }
    } WHEN {
        sRetortPredictionObserved = FALSE;
        sDynamicAiFunc = ObserveRetortPrediction;
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sRetortPredictionObserved);
        EXPECT_EQ(sRetortPredictedDamage, 100);
        EXPECT_EQ(damage, 100);
        results[i].score = sRetortMoveScore;
    } FINALLY {
        for (u32 j = 1; j < 4; j++)
            EXPECT_EQ(results[0].score + DECENT_EFFECT, results[j].score);
    }
}
