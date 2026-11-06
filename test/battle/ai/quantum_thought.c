#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Quantum Thought AI recognizes the apex ability and prefers reliable high-power attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP, MOVE_SURF); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_HYDRO_PUMP, MOVE_SURF); EXPECT_MOVE(opponent, MOVE_HYDRO_PUMP); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_QUANTUM_THOUGHT);
        EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], 100);
    }
}

AI_SINGLE_BATTLE_TEST("Quantum Thought AI predicts guaranteed critical damage and respects critical protection")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Ability ability = ABILITY_NONE;
    u32 gen = GEN_9;
    s16 damage;
    for (u32 i = GEN_1; i <= GEN_9; i++)
    {
        PARAMETRIZE { gen = i; species = SPECIES_WOBBUFFET; ability = ABILITY_TELEPATHY; }
        PARAMETRIZE { gen = i; species = SPECIES_ARMALDO; ability = ABILITY_BATTLE_ARMOR; }
        PARAMETRIZE { gen = i; species = SPECIES_KINGLER; ability = ABILITY_SHELL_ARMOR; }
        PARAMETRIZE { gen = i; species = SPECIES_CELEBI; ability = ABILITY_ILEX_WHIMSY; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) {
            Ability(ability == ABILITY_ILEX_WHIMSY ? ABILITY_NATURAL_CURE : ability);
            Item(ability == ABILITY_ILEX_WHIMSY ? ITEM_APEX_METEORITE : ITEM_NONE);
            HP(3000); MaxHP(3000);
        }
        OPPONENT(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        if (ability == ABILITY_TELEPATHY)
            MESSAGE("A critical hit!");
        else
            NOT MESSAGE("A critical hit!");
    } THEN {
        struct SimulatedDamage predicted = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
        EXPECT_GE(damage, predicted.minimum);
        EXPECT_LE(damage, predicted.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Quantum Thought AI avoids redundant critical-hit setup even against boosted defenses")
{
    enum Move setup;
    PARAMETRIZE { setup = MOVE_FOCUS_ENERGY; }
    PARAMETRIZE { setup = MOVE_LASER_FOCUS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Speed(10); Moves(setup, MOVE_SLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); EXPECT_MOVE(opponent, MOVE_SLASH); }
        TURN { SCORE_LT(opponent, setup, MOVE_SLASH); EXPECT_MOVE(opponent, MOVE_SLASH); }
    }
}

AI_SINGLE_BATTLE_TEST("Quantum Thought AI predicts semi-invulnerability and chooses an attack that can reach the target")
{
    enum Move playerMove, expectedMove;
    PARAMETRIZE { playerMove = MOVE_WATER_GUN; expectedMove = MOVE_THUNDERBOLT; }
    PARAMETRIZE { playerMove = MOVE_DIVE; expectedMove = MOVE_SURF; }
    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_MAGIKARP) { Speed(100); HP(3000); MaxHP(3000); Moves(playerMove); }
        OPPONENT(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Speed(10); Moves(MOVE_THUNDERBOLT, MOVE_SURF); }
    } WHEN {
        TURN { MOVE(player, playerMove); EXPECT_MOVE(opponent, expectedMove); }
    }
}

AI_SINGLE_BATTLE_TEST("Quantum Thought AI recognizes the opponent's apex ability without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_METANG) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_QUANTUM_THOUGHT);
    }
}
