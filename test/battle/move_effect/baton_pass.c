#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_BATON_PASS) == EFFECT_BATON_PASS);
}

// This softlocked the game before.
SINGLE_BATTLE_TEST("Baton Pass used after Memento works correctly")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_CATERPIE);
    } WHEN {
        TURN { MOVE(player, MOVE_MEMENTO); SEND_OUT(player, 1); MOVE(opponent, MOVE_BATON_PASS); SEND_OUT(opponent, 1); }
    } SCENE {
        MESSAGE("Wobbuffet used Memento!");
        MESSAGE("Wobbuffet fainted!");
        MESSAGE("The opposing Wynaut used Baton Pass!");
        MESSAGE("2 sent out Caterpie!");
        MESSAGE("Go! Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Baton Pass switches out the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        MESSAGE("Go! Wynaut!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Baton Pass fails if there's no valid party Pokemon left")
{
    bool32 faintedPartner;
    PARAMETRIZE { faintedPartner = FALSE; }
    PARAMETRIZE { faintedPartner = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        if (faintedPartner)
            PLAYER(SPECIES_WYNAUT) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS); }
    } SCENE {
        MESSAGE("Wobbuffet used Baton Pass!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Baton Pass passes both positive and negative stat changes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WORK_UP); MOVE(opponent, MOVE_TAIL_WHIP); }
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}
AI_SINGLE_BATTLE_TEST("AI avoids Baton Pass when only its ace can switch in")
{
    u64 aceFlag;
    PARAMETRIZE { aceFlag = AI_FLAG_ACE_POKEMON; }
    PARAMETRIZE { aceFlag = AI_FLAG_DOUBLE_ACE_POKEMON; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_BATON_PASS | aceFlag);
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_SWAGGER, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Item(ITEM_PERSIM_BERRY); Moves(MOVE_BATON_PASS, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
        if (aceFlag == AI_FLAG_DOUBLE_ACE_POKEMON)
            OPPONENT(SPECIES_WYNAUT) { Speed(4); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWAGGER); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
SINGLE_BATTLE_TEST("Baton Pass doesn't pass non-volatile status conditions")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), STATUS1_BURN);
    }
}
TO_DO_BATTLE_TEST("Baton Pass doesn't pass infatuation");
TO_DO_BATTLE_TEST("Baton Pass doesn't pass type changes");
TO_DO_BATTLE_TEST("Baton Pass doesn't pass ability changes");

//
// Move these to the corresponding effect files.
//
TO_DO_BATTLE_TEST("Baton Pass passes confusion status");                                                    // test/battle/volatiles/confusion.c

TO_DO_BATTLE_TEST("Baton Pass passes Focus Energy's effect");                                               // test/battle/move_effect/focus_energy.c
TO_DO_BATTLE_TEST("Baton Pass passes Heal Block's effect");                                                 // test/battle/move_effect/heal_block.c
TO_DO_BATTLE_TEST("Baton Pass passes Ingrain's effect");                                                    // test/battle/move_effect/ingrain.c
TO_DO_BATTLE_TEST("Baton Pass passes Magnet Rise's effect");                                                // test/battle/move_effect/magnet_rise.c
TO_DO_BATTLE_TEST("Baton Pass doesn't pass Mimic's learnt move");                                           // test/battle/move_effect/mimic.c
TO_DO_BATTLE_TEST("Baton Pass passes Minimize's evasion but not the weakness to stomping moves");           // test/battle/move_effect/minimize.c
TO_DO_BATTLE_TEST("Baton Pass passes No Retreat's escape prevention effect");                               // test/battle/move_effect/no_retreat.c
TO_DO_BATTLE_TEST("Baton Pass passes Octolock's escape prevention effect");                                 // test/battle/move_effect/octolock.c
TO_DO_BATTLE_TEST("Baton Pass passes Perish Song's effect");                                                // test/battle/move_effect/perish_song.c
TO_DO_BATTLE_TEST("Baton Pass passes Power Trick's effect");                                                // test/battle/move_effect/power_trick.c
TO_DO_BATTLE_TEST("Baton Pass doesn't pass Rollout's multiplier");                                          // test/battle/move_effect/rollout.c
TO_DO_BATTLE_TEST("Baton Pass passes Sappy Seed's effect");                                                 // test/battle/move_effect/sappy_seed.c
TO_DO_BATTLE_TEST("Baton Pass doesn't pass Transformations");                                               // test/battle/move_effect/transform.c
TO_DO_BATTLE_TEST("Baton Pass doesn't pass Yawn's effect");                                                 // test/battle/move_effect/yawn.c

TO_DO_BATTLE_TEST("Baton Pass passes Core Enforcer's effect");                                              // test/battle/move_effect_secondary/core_enforcer.c
TO_DO_BATTLE_TEST("Baton Pass passes escape prevention secondary effect");                                  // test/battle/move_effect_secondary/prevent_escape.c (Thousand Waves, Spirit Shackle, Anchor Shot)
TO_DO_BATTLE_TEST("Baton Pass doesn't pass escape prevention secondary effect if it's used by the user");   // test/battle/move_effect_secondary/prevent_escape.c (Thousand Waves, Spirit Shackle, Anchor Shot)

// Unconfirmed by Bulbapedia, should be tested in-game:
// - Nightmare
// - Encore
// - Spotlight
// - Taunt
// - Throat Chop
// - Torment
// - Splinters
// - Power Boost
// - Power Drop
// - Guard Boost
