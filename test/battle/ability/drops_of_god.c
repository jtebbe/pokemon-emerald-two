#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Drops of God is Politoed's Apex ability and does not replace Poliwrath's")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_POLITOED; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_POLITOED; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_POLIWRATH; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); Ability(ABILITY_WATER_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_DROPS_OF_GOD,
                  species == SPECIES_POLITOED && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Drops of God sets permanent ordinary rain regardless of weather generation")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_5; }
    PARAMETRIZE { generation = GEN_9; }
    GIVEN {
        WITH_CONFIG(B_ABILITY_WEATHER, generation);
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 turn = 0; turn < 10; turn++) TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DROPS_OF_GOD);
        MESSAGE("It started to rain!");
        NOT MESSAGE("The rain stopped.");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN_NORMAL);
        EXPECT(!(gBattleWeather & B_WEATHER_PRIMAL_ANY));
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gBattleStruct->weatherDurationTotal, 0);
    }
}

SINGLE_BATTLE_TEST("Drops of God rain remains when the setter switches out")
{
    GIVEN {
        WITH_CONFIG(B_ABILITY_WEATHER, GEN_9);
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        for (u32 turn = 0; turn < 8; turn++) TURN {}
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN_NORMAL);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

SINGLE_BATTLE_TEST("Drops of God upgrades an existing move's rain to permanent rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { SWITCH(player, 1); }
        for (u32 turn = 0; turn < 8; turn++) TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_DROPS_OF_GOD);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN_NORMAL);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

SINGLE_BATTLE_TEST("Drops of God rain is overwritten by ordinary weather moves and does not return")
{
    enum Move weather;
    PARAMETRIZE { weather = MOVE_SUNNY_DAY; }
    PARAMETRIZE { weather = MOVE_SANDSTORM; }
    PARAMETRIZE { weather = MOVE_SNOWSCAPE; }
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, weather); }
        TURN { MOVE(player, MOVE_REST); }
        for (u32 turn = 0; turn < 4; turn++) TURN {}
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

SINGLE_BATTLE_TEST("Drops of God rain can be overwritten by entry weather abilities")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_DROUGHT; }
    PARAMETRIZE { ability = ABILITY_SAND_STREAM; }
    PARAMETRIZE { ability = ABILITY_SNOW_WARNING; }
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_RAIN));
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
    }
}

SINGLE_BATTLE_TEST("Drops of God restores an eighth max HP at the end of each rain turn")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DROPS_OF_GOD);
        HP_BAR(player, damage: -100);
        HP_BAR(player, damage: -100);
    } THEN {
        EXPECT_EQ(player->hp, 600);
    }
}

SINGLE_BATTLE_TEST("Drops of God healing caps at max HP and uses a minimum of one HP")
{
    u32 maxHp;
    PARAMETRIZE { maxHp = 800; }
    PARAMETRIZE { maxHp = 7; }
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(maxHp - 1); MaxHP(maxHp); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(player, damage: -1);
    } THEN {
        EXPECT_EQ(player->hp, maxHp);
    }
}

SINGLE_BATTLE_TEST("Drops of God does not activate residual healing at full HP")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        NOT HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Drops of God loses an eighth HP in sun and after the weather clears")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); }
        for (u32 turn = 0; turn < 5; turn++) TURN {}
    } SCENE {
        for (u32 turn = 0; turn < 6; turn++) HP_BAR(player, damage: 100);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(player->hp, 200);
    }
}

SINGLE_BATTLE_TEST("Drops of God loses an eighth HP when rain is suppressed")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CLOUD_NINE; }
    PARAMETRIZE { ability = ABILITY_AIR_LOCK; }
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(player, damage: 100);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN_NORMAL);
        EXPECT_EQ(player->hp, 700);
    }
}

SINGLE_BATTLE_TEST("Drops of God Heal Block prevents rain healing without causing dry-weather damage")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
    } SCENE {
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Drops of God weather damage can knock out its user")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(800); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player, damage: 1);
        MESSAGE("Politoed fainted!");
    }
}

SINGLE_BATTLE_TEST("Drops of God respects primal weather restrictions")
{
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); Speed(1); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_GROUDON) { Ability(ABILITY_DESOLATE_LAND); Speed(100); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN_PRIMAL);
        EXPECT_EQ(player->hp, 700);
    }
}

SINGLE_BATTLE_TEST("Drops of God strengthens Water attacks through its rain", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_POLITOED) { Item(item); Ability(ABILITY_WATER_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}
