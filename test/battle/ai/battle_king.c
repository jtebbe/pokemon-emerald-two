#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sKingDamage;
static s32 sKingScore;
static bool32 sKingObserved;
static s32 ObserveBattleKing(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sKingDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sKingScore = score;
        sKingObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Battle King AI predicts the combined 1.95x boost and each individual boost", s16 damage)
{
    enum Move move = MOVE_FORCE_PALM;
    enum Item item = ITEM_NONE;
    const enum Move attacks[] = {MOVE_FORCE_PALM, MOVE_WATER_PULSE, MOVE_SCRATCH, MOVE_EARTH_POWER, MOVE_STRENGTH};
    for (u32 j = 0; j < ARRAY_COUNT(attacks); j++) {
        PARAMETRIZE { move = attacks[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = attacks[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_POISON_POINT); Item(item); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sKingObserved = FALSE;
        sDynamicAiFunc = ObserveBattleKing;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sKingObserved);
        EXPECT_GE(results[i].damage, sKingDamage.minimum);
        EXPECT_LE(results[i].damage, sKingDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_BATTLE_KING : ABILITY_POISON_POINT);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.95), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.95), results[3].damage);
        EXPECT_MUL_EQ(results[4].damage, UQ_4_12(1.5), results[5].damage);
        EXPECT_MUL_EQ(results[6].damage, UQ_4_12(1.3), results[7].damage);
        EXPECT_EQ(results[8].damage, results[9].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Battle King AI prefers a move boosted by both components over a stronger unboosted move")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_POISON_POINT); Item(item); Moves(MOVE_FORCE_PALM, MOVE_BRICK_BREAK); }
    } WHEN {
        if (item == ITEM_APEX_METEORITE)
            TURN { SCORE_GT(opponent, MOVE_FORCE_PALM, MOVE_BRICK_BREAK); EXPECT_MOVE(opponent, MOVE_FORCE_PALM); }
        else
            TURN { SCORE_GT(opponent, MOVE_BRICK_BREAK, MOVE_FORCE_PALM); EXPECT_MOVE(opponent, MOVE_BRICK_BREAK); }
    }
}

AI_SINGLE_BATTLE_TEST("Battle King AI does not penalize a suppressed stat drop against Contrary", s32 score)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); Moves(MOVE_ACID_SPRAY); }
    } WHEN {
        sKingObserved = FALSE;
        sDynamicAiFunc = ObserveBattleKing;
        TURN { EXPECT_MOVE(opponent, MOVE_ACID_SPRAY); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sKingObserved);
        results[i].score = sKingScore;
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    } FINALLY {
        EXPECT_EQ(results[0].score, results[1].score);
    }
}

AI_SINGLE_BATTLE_TEST("Battle King AI predicts critical immunity against Quantum Thought and Mold Breaker")
{
    enum Ability ability = ABILITY_QUANTUM_THOUGHT;
    enum Move move = MOVE_WATER_GUN;
    u32 gen = GEN_9;
    s16 damage;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { gen = j ? GEN_9 : GEN_1; ability = ABILITY_QUANTUM_THOUGHT; move = MOVE_WATER_GUN; }
        PARAMETRIZE { gen = j ? GEN_9 : GEN_1; ability = ABILITY_MOLD_BREAKER; move = MOVE_WICKED_BLOW; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(move); }
    } WHEN {
        sKingObserved = FALSE;
        sDynamicAiFunc = ObserveBattleKing;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        NOT MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sKingObserved);
        EXPECT_GE(damage, sKingDamage.minimum);
        EXPECT_LE(damage, sKingDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_BATTLE_KING);
    }
}

AI_SINGLE_BATTLE_TEST("Battle King AI avoids useless Laser Focus even with Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_LASER_FOCUS, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_LASER_FOCUS, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
