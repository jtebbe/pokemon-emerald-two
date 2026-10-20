#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("Void Blast AI recognizes Bulletproof")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CHESPIN) { Ability(ABILITY_BULLETPROOF); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_VOID_BLAST, MOVE_DARK_PULSE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_DARK_PULSE); }
    }
}

AI_SINGLE_BATTLE_TEST("Void Blast AI values its Contrary boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EEVEE) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CONTRARY); Moves(MOVE_VOID_BLAST, MOVE_DARK_PULSE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_VOID_BLAST, MOVE_DARK_PULSE); }
    } THEN {
        EXPECT(HasMoveThatLowersOwnStats(B_BATTLER_1));
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}

AI_SINGLE_BATTLE_TEST("Void Blast AI damage ranges match actual damage before and after self drops")
{
    bool32 lowered;
    enum Ability ability;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { lowered = j; ability = ABILITY_SHADOW_TAG; }
        PARAMETRIZE { lowered = j; ability = ABILITY_CONTRARY; }
        PARAMETRIZE { lowered = j; ability = ABILITY_SIMPLE; }
    }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EEVEE) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_VOID_BLAST); }
    } WHEN {
        if (lowered)
            TURN { EXPECT_MOVE(opponent, MOVE_VOID_BLAST); }
        TURN { EXPECT_MOVE(opponent, MOVE_VOID_BLAST); }
    } SCENE {
        if (lowered)
            HP_BAR(player);
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        s32 stageChange = ability == ABILITY_CONTRARY ? 1 : ability == ABILITY_SIMPLE ? -2 : -1;
        s8 currentStage = opponent->statStages[STAT_SPATK];
        EXPECT_EQ(currentStage, DEFAULT_STAT_STAGE + stageChange * (lowered ? 2 : 1));

        // THEN runs after the AI cache is refreshed for the next turn, after
        // this hit's self drops. Reconstruct the attacking stage for this hit
        // instead of comparing its damage against the next turn's estimate.
        struct SimulatedDamage nextTurnDamage = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
        uq4_12_t effectiveness;
        opponent->statStages[STAT_SPATK] = currentStage - stageChange;
        struct SimulatedDamage attackDamage = AI_CalcDamageSaveBattlers(MOVE_VOID_BLAST, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        opponent->statStages[STAT_SPATK] = currentStage;

        EXPECT_GE(damage, attackDamage.minimum);
        EXPECT_LE(damage, attackDamage.maximum);
        if (stageChange < 0)
            EXPECT_LT(nextTurnDamage.maximum, attackDamage.maximum);
        else
            EXPECT_GT(nextTurnDamage.maximum, attackDamage.maximum);
    }
}
