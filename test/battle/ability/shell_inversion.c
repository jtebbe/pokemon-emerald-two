#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Shell Inversion trades both defenses for Attack Sp. Atk and Speed on entry for the Squirtle line")
{
    enum Species species;
    bool32 opposing;
    PARAMETRIZE { species = SPECIES_SQUIRTLE; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_WARTORTLE; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE_MEGA; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_SQUIRTLE; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_WARTORTLE; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE_MEGA; opposing = TRUE; }
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : species) { Item(opposing ? ITEM_NONE : ITEM_APEX_METEORITE); }
        OPPONENT(opposing ? species : SPECIES_WOBBUFFET) { Item(opposing ? ITEM_APEX_METEORITE : ITEM_NONE); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_SHELL_INVERSION);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opposing ? opponent : player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opposing ? opponent : player);
    } THEN {
        enum BattlerId holder = opposing ? B_BATTLER_1 : B_BATTLER_0;
        enum BattlerId other = opposing ? B_BATTLER_0 : B_BATTLER_1;
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_SHELL_INVERSION);
        EXPECT_EQ(GetBattlerAbility(holder), ABILITY_SHELL_INVERSION);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[holder].hp, gBattleMons[holder].maxHP);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(gBattleMons[other].statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion requires an Apex Meteorite for both entry changes and flinch immunity")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_SQUIRTLE; }
    PARAMETRIZE { species = SPECIES_WARTORTLE; }
    PARAMETRIZE { species = SPECIES_BLASTOISE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_TORRENT); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHELL_INVERSION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TORRENT);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion activates once per entry and activates again after switching back in")
{
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN {}
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_INVERSION);
        MESSAGE("Squirtle's Defense fell!");
        MESSAGE("Squirtle's Sp. Def fell!");
        MESSAGE("Squirtle's Attack rose!");
        MESSAGE("Squirtle's Sp. Atk rose!");
        MESSAGE("Squirtle's Speed rose!");
        ABILITY_POPUP(player, ABILITY_SHELL_INVERSION);
        NOT ABILITY_POPUP(player, ABILITY_SHELL_INVERSION);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion respects stage caps when repeatedly entering with Baton Passed stages")
{
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        for (u32 i = 0; i < 6; i++)
        {
            TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
            TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 0); }
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], MIN_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], MIN_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion's self-inflicted defense drops bypass Mist and a Baton Passed Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(player, MOVE_MIST); }
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
    } THEN {
        EXPECT(player->volatiles.substitute);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion prevents guaranteed and secondary move flinches")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FAKE_OUT; }
    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_ROCK_SLIDE; }
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); Speed(10); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, move, secondaryEffect: TRUE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
        NOT MESSAGE("Squirtle flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mold Breaker Teravolt and Turboblaze cannot bypass Shell Inversion's flinch immunity")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_PINSIR; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { species = SPECIES_ZEKROM; ability = ABILITY_TERAVOLT; }
    PARAMETRIZE { species = SPECIES_RESHIRAM; ability = ABILITY_TURBOBLAZE; }
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Blastoise flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion prevents King's Rock and Razor Fang flinches")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_KINGS_ROCK; }
    PARAMETRIZE { item = ITEM_RAZOR_FANG; }
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); Speed(10); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH, WITH_RNG(RNG_HOLD_EFFECT_FLINCH, TRUE)); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
        NOT MESSAGE("Squirtle flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion prevents Stench and Fling flinches")
{
    bool32 fling;
    PARAMETRIZE { fling = FALSE; }
    PARAMETRIZE { fling = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); Speed(10); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_STUNKY) { Ability(fling ? ABILITY_AFTERMATH : ABILITY_STENCH); Item(fling ? ITEM_KINGS_ROCK : ITEM_NONE); Speed(100); }
    } WHEN {
        if (fling)
            TURN { MOVE(opponent, MOVE_FLING); MOVE(player, MOVE_TACKLE); }
        else
            TURN { MOVE(opponent, MOVE_SCRATCH, WITH_RNG(RNG_STENCH, TRUE)); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
        NOT MESSAGE("Squirtle flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Shell Inversion's flinch immunity does not grant Inner Focus's Intimidate protection")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_9);
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Shell Inversion changes each holder's stats exactly once in doubles")
{
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WARTORTLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WARTORTLE) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN {}
        TURN {}
    } THEN {
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        {
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        }
    }
}

DOUBLE_BATTLE_TEST("Opportunist and Mirror Herb copy Shell Inversion's boosts without copying its defense drops")
{
    bool32 herb;
    PARAMETRIZE { herb = FALSE; }
    PARAMETRIZE { herb = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(herb ? SPECIES_WOBBUFFET : SPECIES_ESPATHRA) { Ability(herb ? ABILITY_TELEPATHY : ABILITY_OPPORTUNIST); Item(herb ? ITEM_MIRROR_HERB : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas cannot suppress Shell Inversion's entry changes or flinch immunity")
{
    GIVEN {
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Squirtle flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
