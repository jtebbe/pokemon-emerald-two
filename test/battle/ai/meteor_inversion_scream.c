#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI uses Inversion as soon as possible")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_INVERSION, MOVE_PSYCHIC); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_INVERSION); }
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids Inversion when matchups are already inverted")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_POWERFUL_STATUS);
        FLAG_SET(B_FLAG_INVERSE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_INVERSION, MOVE_PSYCHIC); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_PSYCHIC, MOVE_INVERSION); }
    }
}

AI_SINGLE_BATTLE_TEST("AI prioritizes Meteoric Wrath without an immediate KO")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_METEORIC_WRATH, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_METEORIC_WRATH, MOVE_SCRATCH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI takes an immediate KO instead of using Meteoric Wrath")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_METEORIC_WRATH, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_METEORIC_WRATH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids Meteoric Wrath while a meteor is en route")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_METEORIC_WRATH, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); EXPECT_MOVES(opponent, MOVE_METEORIC_WRATH, MOVE_SCRATCH); }
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_METEORIC_WRATH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI accounts for Clothesline's boosted power")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CLOTHESLINE, MOVE_ROCK_SMASH, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); EXPECT_MOVES(opponent, MOVE_CLOTHESLINE, MOVE_ROCK_SMASH, MOVE_SCRATCH); }
        TURN { SCORE_GT(opponent, MOVE_CLOTHESLINE, MOVE_ROCK_SMASH); }
    }
}

AI_SINGLE_BATTLE_TEST("AI values Angsty Scream while in danger or statused")
{
    u32 hp, status;
    PARAMETRIZE { hp = 49; status = STATUS1_NONE; }
    PARAMETRIZE { hp = 100; status = STATUS1_POISON; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(hp); Status1(status); Moves(MOVE_ANGSTY_SCREAM, MOVE_DARK_PULSE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_ANGSTY_SCREAM, MOVE_DARK_PULSE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI damage ranges match Clothesline and Angsty Scream")
{
    enum Move move;
    bool32 boosted;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { move = MOVE_CLOTHESLINE; boosted = j; }
        PARAMETRIZE { move = MOVE_ANGSTY_SCREAM; boosted = j; }
    }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(boosted ? 1499 : 3000); MaxHP(3000); Moves(move); }
    } WHEN {
        if (move == MOVE_CLOTHESLINE && boosted)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        if (move == MOVE_CLOTHESLINE && boosted)
            HP_BAR(player);
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        EXPECT_GE(damage, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].minimum);
        EXPECT_LE(damage, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum);
    }
}
