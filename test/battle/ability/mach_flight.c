#include "global.h"
#include "test/battle.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Mach Flight doubles Speed for the Pidgey line with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_PIDGEY; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIDGEOTTO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIDGEOT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIDGEOT_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIDGEY; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_KEEN_EYE); Item(item); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN {}
    } SCENE {
        if (item == ITEM_APEX_METEORITE) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_MACH_FLIGHT);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)), item == ITEM_APEX_METEORITE ? 120 : 60);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Mach Flight cannot set Tailwind because its holder is already in Tailwind")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
    } SCENE {
        MESSAGE("Pidgey used Tailwind!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT(IsBattlerInTailwind(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0)));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Mach Flight does not grant Tailwind to its ally and does not stack with an ally's Tailwind")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOTTO) { Item(ITEM_APEX_METEORITE); Speed(60); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(150); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_TAILWIND); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Tailwind!");
        MESSAGE("A tailwind started blowing on your side!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
    } THEN {
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), HOLD_EFFECT_APEX_METEORITE), 120);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_2, GetBattlerAbility(B_BATTLER_2), HOLD_EFFECT_NONE), 100);
    }
}

SINGLE_BATTLE_TEST("Mach Flight persists after ordinary Tailwind expires")
{
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_TAILWIND_PLAYER_TEMPORARY);
    GIVEN {
        WITH_CONFIG(B_TAILWIND_TURNS, GEN_9);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        for (u32 turn = 0; turn < 5; turn++)
            TURN {}
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), HOLD_EFFECT_APEX_METEORITE), 120);
    }
}

SINGLE_BATTLE_TEST("Mach Flight follows Tailwind's interaction with paralysis and Speed stages")
{
    bool32 paralyzed;
    PARAMETRIZE { paralyzed = FALSE; }
    PARAMETRIZE { paralyzed = TRUE; }
    GIVEN {
        WITH_CONFIG(B_PARALYSIS_SPEED, GEN_7);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(100); Status1(paralyzed ? STATUS1_PARALYSIS : 0); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCARY_FACE); }
    } THEN {
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), HOLD_EFFECT_APEX_METEORITE), paralyzed ? 50 : 100);
    }
}

SINGLE_BATTLE_TEST("Mach Flight raises critical hit ratio by one stage only when acting before the target")
{
    u32 gen = GEN_9, passes = 1, trials = 8;
    u16 foeSpeed = 100;
    for (u32 i = GEN_1; i <= GEN_9; i++)
    {
        PARAMETRIZE { gen = i; foeSpeed = 100; passes = i == GEN_1 ? 7 : 1; trials = i == GEN_1 ? 16 : 8; }
        PARAMETRIZE { gen = i; foeSpeed = 200; passes = i == GEN_1 ? 7 : 1; trials = i == GEN_1 ? 64 : (i >= GEN_7 ? 24 : 16); }
    }
    PASSES_RANDOMLY(passes, trials, RNG_CRITICAL_HIT);
    GIVEN {
        ASSUME(GetSpeciesBaseSpeed(SPECIES_PIDGEY) == 56);
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(foeSpeed); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Mach Flight's critical bonus follows priority and Trick Room rather than raw Speed")
{
    enum Move move, foeMove;
    u16 speed;
    bool32 trickRoom;
    u32 passes, trials;
    PARAMETRIZE { move = MOVE_TACKLE; foeMove = MOVE_QUICK_ATTACK; speed = 60; trickRoom = FALSE; passes = 1; trials = 24; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; foeMove = MOVE_TACKLE; speed = 10; trickRoom = FALSE; passes = 1; trials = 8; }
    PARAMETRIZE { move = MOVE_TACKLE; foeMove = MOVE_TACKLE; speed = 60; trickRoom = TRUE; passes = 1; trials = 24; }
    PARAMETRIZE { move = MOVE_TACKLE; foeMove = MOVE_TACKLE; speed = 10; trickRoom = TRUE; passes = 1; trials = 8; }
    PASSES_RANDOMLY(passes, trials, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(speed); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        if (trickRoom)
            TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, move); MOVE(opponent, foeMove, WITH_RNG(RNG_CRITICAL_HIT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Mach Flight stacks with Focus Energy to guarantee critical hits when moving first")
{
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_PIDGEOTTO) { Item(ITEM_APEX_METEORITE); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Pidgeotto used Tackle!");
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Mach Flight's critical bonus respects Battle Armor Shell Armor and Lucky Chant")
{
    enum Species species;
    enum Ability ability;
    bool32 chant;
    PARAMETRIZE { species = SPECIES_ARMALDO; ability = ABILITY_BATTLE_ARMOR; chant = FALSE; }
    PARAMETRIZE { species = SPECIES_KINGLER; ability = ABILITY_SHELL_ARMOR; chant = FALSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_TELEPATHY; chant = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(species) { Ability(ability); Speed(50); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); MOVE(opponent, chant ? MOVE_LUCKY_CHANT : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Pidgey used Tackle!");
        HP_BAR(opponent);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Mach Flight does not gain a critical bonus against a target that just switched")
{
    PASSES_RANDOMLY(1, 24, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Speed(10); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

DOUBLE_BATTLE_TEST("Mach Flight's critical bonus is checked separately for each spread target")
{
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(60); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FOCUS_ENERGY); }
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        NOT MESSAGE("A critical hit on the opposing Wobbuffet!");
        MESSAGE("A critical hit on the opposing Wynaut!");
    }
}

SINGLE_BATTLE_TEST("Mach Flight remains active under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(60); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Speed(100); }
    } WHEN {
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_MACH_FLIGHT);
    }
}

DOUBLE_BATTLE_TEST("Mach Flight counts as Tailwind for Harsh Zephyr without spreading an ally's Harsh Zephyr")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HARSH_ZEPHYR, target: opponentRight, secondaryEffect: FALSE); }
        TURN { MOVE(playerRight, MOVE_HARSH_ZEPHYR, target: opponentRight, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HARSH_ZEPHYR, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HARSH_ZEPHYR, playerRight);
        NONE_OF { HP_BAR(opponentLeft); }
        HP_BAR(opponentRight);
    }
}
