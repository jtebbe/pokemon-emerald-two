#include "global.h"
#include "test/battle.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Vicious Stings grants Dark STAB to Beedrill and Mega Beedrill", s16 damage)
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_BEEDRILL; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL_MEGA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL_MEGA; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_SWARM); Item(item); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_VICIOUS_STINGS);
        EXPECT_EQ(player->types[0], TYPE_BUG);
        EXPECT_EQ(player->types[1], TYPE_POISON);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Vicious Stings includes Merciless on every attack type against either poison status")
{
    enum Move move = MOVE_TACKLE;
    u32 poison = STATUS1_POISON;
    u32 gen = GEN_9;
    for (u32 i = GEN_1; i <= GEN_9; i++)
    {
        PARAMETRIZE { gen = i; move = MOVE_TACKLE; poison = STATUS1_POISON; }
        PARAMETRIZE { gen = i; move = MOVE_BITE; poison = STATUS1_TOXIC_POISON; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SNORLAX) { Ability(ABILITY_THICK_FAT); Status1(poison); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings respects critical-hit protection")
{
    enum Species species;
    enum Ability ability;
    bool32 chant;
    PARAMETRIZE { species = SPECIES_ARMALDO; ability = ABILITY_BATTLE_ARMOR; chant = FALSE; }
    PARAMETRIZE { species = SPECIES_KINGLER; ability = ABILITY_SHELL_ARMOR; chant = FALSE; }
    PARAMETRIZE { species = SPECIES_SNORLAX; ability = ABILITY_THICK_FAT; chant = TRUE; }
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(species) { Ability(ability); Status1(STATUS1_POISON); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, chant ? MOVE_LUCKY_CHANT : MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Vicious Stings does not force critical hits against an unpoisoned target")
{
    PASSES_RANDOMLY(23, 24, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings moves Dark attacks and status moves first within their priority bracket")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_DARK_PULSE; }
    PARAMETRIZE { move = MOVE_TAUNT; }
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        MESSAGE("The opposing Snorlax used Tackle!");
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_VICIOUS_STINGS, move), GetMovePriority(move));
    }
}

SINGLE_BATTLE_TEST("Vicious Stings does not elevate non-Dark moves or moves without the Meteorite")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_TACKLE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_BITE; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Ability(ABILITY_SWARM); Item(item); Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The opposing Snorlax used Tackle!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Vicious Stings cannot outrun a higher priority bracket")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        MESSAGE("The opposing Snorlax used Quick Attack!");
        MESSAGE("Beedrill used Bite!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings works inside a positive priority bracket without gaining another priority level")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUCKER_PUNCH); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        MESSAGE("Beedrill used Sucker Punch!");
        MESSAGE("The opposing Snorlax used Quick Attack!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings preserves its ordering in Trick Room")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_SNORLAX) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The opposing Snorlax used Trick Room!");
        MESSAGE("Beedrill used Bite!");
        MESSAGE("The opposing Snorlax used Tackle!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings ties use Speed when both battlers select Dark moves")
{
    bool32 trickRoom;
    PARAMETRIZE { trickRoom = FALSE; }
    PARAMETRIZE { trickRoom = TRUE; }
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(100); }
    } WHEN {
        if (trickRoom)
            TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); MOVE(opponent, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        if (trickRoom) {
            MESSAGE("Beedrill used Bite!");
            MESSAGE("The opposing Beedrill used Bite!");
        } else {
            MESSAGE("The opposing Beedrill used Bite!");
            MESSAGE("Beedrill used Bite!");
        }
    }
}

SINGLE_BATTLE_TEST("Vicious Stings Dark ordering is not blocked by Psychic Terrain or Dazzling")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_BRUXISH) { Ability(ABILITY_DAZZLING); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC_TERRAIN); }
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The opposing Bruxish used Psychic Terrain!");
        MESSAGE("Beedrill used Bite!");
        HP_BAR(opponent);
        MESSAGE("The opposing Bruxish used Tackle!");
    }
}

SINGLE_BATTLE_TEST("Vicious Stings remains active under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Speed(100); Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Beedrill used Bite!");
        MESSAGE("A critical hit!");
        MESSAGE("The opposing Weezing used Tackle!");
    }
}
