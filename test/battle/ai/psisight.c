#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_util.h"

// Capture the prediction at selection; the cache is refreshed for the next turn
// after the battle, when another Psisight boost is already being considered.
extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sPsisightPrediction;
static bool32 sObservedPsisightPrediction;
static u32 sPsisightPredictionTurn;
static u32 sPsisightPredictionStage;

static s32 ObservePsisightPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sPsisightPredictionTurn
     && (move == MOVE_PSYSHOCK || move == MOVE_WATER_GUN))
    {
        sPsisightPrediction = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sPsisightPredictionStage = gBattleMons[battlerAtk].statStages[STAT_SPATK];
        sObservedPsisightPrediction = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Psisight AI predicts Psychic STAB and the same-turn special boost without leaking simulated stages")
{
    u32 turns;
    s16 damage;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PSYSHOCK, MOVE_GUST); }
    } WHEN {
        sObservedPsisightPrediction = FALSE;
        sPsisightPredictionTurn = turns - 1;
        sDynamicAiFunc = ObservePsisightPrediction;
        for (u32 turn = 0; turn < turns; turn++)
            TURN { SCORE_GT(opponent, MOVE_PSYSHOCK, MOVE_GUST); EXPECT_MOVE(opponent, MOVE_PSYSHOCK); }
    } SCENE {
        for (u32 turn = 0; turn < turns; turn++)
            HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedPsisightPrediction);
        EXPECT_EQ(sPsisightPredictionStage, DEFAULT_STAT_STAGE + sPsisightPredictionTurn);
        struct SimulatedDamage predicted = sPsisightPrediction;
        EXPECT_GE(damage, predicted.minimum);
        EXPECT_LE(damage, predicted.maximum);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + turns);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + turns);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_PSISIGHT);
    }
}

AI_SINGLE_BATTLE_TEST("Psisight AI includes Compound Eyes in attack and status accuracy")
{
    enum Move move;
    u32 accuracy;
    PARAMETRIZE { move = MOVE_THUNDER; accuracy = 91; }
    PARAMETRIZE { move = MOVE_SLEEP_POWDER; accuracy = 97; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); Moves(move); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], accuracy);
    }
}

AI_SINGLE_BATTLE_TEST("Psisight AI values the free special boosts on Psychic status moves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_AGILITY, MOVE_ROCK_POLISH, MOVE_PSYSHOCK); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_AGILITY, MOVE_ROCK_POLISH); }
    }
}

AI_SINGLE_BATTLE_TEST("Psisight AI predicts the defender's turn-start Sp Def boost before a faster attack")
{
    s16 damage;
    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); Moves(MOVE_PSYSHOCK); }
        OPPONENT(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sObservedPsisightPrediction = FALSE;
        sPsisightPredictionTurn = 0;
        sDynamicAiFunc = ObservePsisightPrediction;
        TURN { MOVE(player, MOVE_PSYSHOCK); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedPsisightPrediction);
        struct SimulatedDamage predicted = sPsisightPrediction;
        EXPECT_GE(damage, predicted.minimum);
        EXPECT_LE(damage, predicted.maximum);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
