#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Blazing Glory costs one sixth max HP and boosts Attack Sp. Atk and Speed for the entire Charmander line")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_CHARMANDER; }
    PARAMETRIZE { species = SPECIES_CHARMELEON; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; }
    PARAMETRIZE { species = SPECIES_CHARIZARD_MEGA_X; }
    PARAMETRIZE { species = SPECIES_CHARIZARD_MEGA_Y; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLAZING_GLORY);
        HP_BAR(player);
        if (species == SPECIES_CHARMANDER)
            MESSAGE("Charmander is burning up!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_BLAZING_GLORY);
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Opposing Blazing Glory uses the opposing Pokemon's name and only boosts its holder")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BLAZING_GLORY);
        HP_BAR(opponent);
        MESSAGE("The opposing Charmander is burning up!");
    } THEN {
        EXPECT_EQ(opponent->hp, 100);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory does not activate without an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLAZING_GLORY);
            MESSAGE("Charmander is burning up!");
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 120);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory activates each turn even while sleeping")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); Status1(STATUS1_SLEEP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        MESSAGE("Charmander is burning up!");
        MESSAGE("Charmander is burning up!");
        MESSAGE("Charmander is burning up!");
    } THEN {
        EXPECT_EQ(player->hp, 60);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 3);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 3);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 3);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory activates on the switch-in turn but not while benched")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        MESSAGE("Charmander is burning up!");
        NOT MESSAGE("Charmander is burning up!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 100);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory announces lethal HP loss without granting stat boosts")
{
    u32 hp, maxHP;
    PARAMETRIZE { hp = 20; maxHP = 120; }
    PARAMETRIZE { hp = 19; maxHP = 120; }
    PARAMETRIZE { hp = 1; maxHP = 5; }
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(maxHP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(player);
        MESSAGE("Charmander burned too bright!");
        MESSAGE("Charmander fainted!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Charmander is burning up!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory rounds its cost down with a minimum of one HP")
{
    u32 maxHP, cost;
    PARAMETRIZE { maxHP = 125; cost = 20; }
    PARAMETRIZE { maxHP = 5; cost = 1; }
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(maxHP); MaxHP(maxHP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->hp, maxHP - cost);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Blazing Glory still costs HP after all three boosted stats reach their caps")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(600); MaxHP(600); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 i = 0; i < 7; i++)
            TURN { MOVE(opponent, MOVE_HEAL_PULSE, target: player); }
    } THEN {
        EXPECT_EQ(player->hp, 500);
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Protect and Substitute cannot prevent Blazing Glory's cost or boosts")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_PROTECT; }
    PARAMETRIZE { move = MOVE_SUBSTITUTE; }
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(player->hp, move == MOVE_SUBSTITUTE ? 70 : 100);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Multiple Blazing Glory holders each activate once per turn without crossing stat queues")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        PLAYER(SPECIES_CHARMELEON) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_CHARMELEON) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
    } WHEN {
        TURN {}
    } THEN {
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        {
            EXPECT_EQ(gBattleMons[battler].hp, 100);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        }
    }
}

DOUBLE_BATTLE_TEST("Opportunist copies surviving Blazing Glory boosts but not a holder's lethal tick")
{
    u32 hp;
    PARAMETRIZE { hp = 120; }
    PARAMETRIZE { hp = 20; }
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(120); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ESPATHRA) { Ability(ABILITY_OPPORTUNIST); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        u32 expected = DEFAULT_STAT_STAGE + (hp > 20);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], expected);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], expected);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], expected);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas does not suppress Blazing Glory")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
