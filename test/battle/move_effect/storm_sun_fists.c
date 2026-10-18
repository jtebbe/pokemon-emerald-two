#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Storm Sacrifice pays a rounded-down quarter HP even with Magic Guard")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; }
    GIVEN {
        ASSUME(GetMovePP(MOVE_STORM_SACRIFICE) == 8); // Always PP Max'd.
        PLAYER(SPECIES_WOBBUFFET) { HP(103); MaxHP(103); Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STORM_SACRIFICE, player);
        HP_BAR(player, hp: 78);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gBattleStruct->weatherDurationTotal, 0);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
        EXPECT_EQ(gFieldTimers.terrainTimerTotal, 0);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice can faint its user and keeps both field effects")
{
    u32 hp;
    PARAMETRIZE { hp = 1; }
    PARAMETRIZE { hp = 25; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(100); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STORM_SACRIFICE, player);
        HP_BAR(player, hp: 0);
        MESSAGE("Wobbuffet fainted!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Storm Sacrifice pays HP once in doubles and activates terrain items")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); }
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_ELECTRIC_SEED); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_STORM_SACRIFICE); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 75);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice's minimum payment is one HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(3); MaxHP(3); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
    } THEN {
        EXPECT_EQ(player->hp, 2);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice's rain and terrain persist past eight turns and switching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_MAGIKARP);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
        TURN { SWITCH(player, 1); }
        for (u32 j = 0; j < 9; j++)
            TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice upgrades existing rain and terrain to permanent effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
    } THEN {
        EXPECT_EQ(player->hp, 50);
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice's weather and terrain can be overwritten independently")
{
    bool32 replaceWeather;
    PARAMETRIZE { replaceWeather = FALSE; }
    PARAMETRIZE { replaceWeather = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
        TURN { MOVE(player, replaceWeather ? MOVE_SUNNY_DAY : MOVE_MISTY_TERRAIN); }
    } THEN {
        EXPECT_EQ(gBattleWeather, replaceWeather ? B_WEATHER_SUN_NORMAL : B_WEATHER_RAIN_NORMAL);
        EXPECT_EQ((u32)gFieldTimers.terrain, replaceWeather ? B_TERRAIN_ELECTRIC : B_TERRAIN_MISTY);
        EXPECT_EQ(gBattleStruct->weatherDuration, replaceWeather ? 4 : 0);
        EXPECT_EQ(gFieldTimers.terrainTimer, replaceWeather ? 0 : 4);
    }
}

SINGLE_BATTLE_TEST("Storm Sacrifice cannot replace primal sun and does not charge HP on failure")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); }
        OPPONENT(SPECIES_GROUDON_PRIMAL) { Ability(ABILITY_DESOLATE_LAND); }
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT(gBattleWeather & B_WEATHER_SUN_PRIMAL);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_NONE);
    }
}

SINGLE_BATTLE_TEST("Sunbloom sets five turns of sun, or eight with Heat Rock")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HEAT_ROCK; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUNBLOOM); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT_EQ(gBattleStruct->weatherDuration, item == ITEM_HEAT_ROCK ? 7 : 4);
        EXPECT_EQ(gBattleStruct->weatherDurationTotal, item == ITEM_HEAT_ROCK ? 8 : 5);
    }
}

SINGLE_BATTLE_TEST("Sunbloom gains 1.5x power in existing sun without refreshing it", s16 damage)
{
    bool32 sunny;
    PARAMETRIZE { sunny = FALSE; }
    PARAMETRIZE { sunny = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, sunny ? MOVE_SUNNY_DAY : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SUNBLOOM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNBLOOM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(gBattleStruct->weatherDuration, sunny ? 3 : 4);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Sunbloom does not set sun on a miss or through Protect")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, MOVE_SUNBLOOM, hit: protect); }
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Sunbloom's sun expires after its normal duration")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HEAT_ROCK; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUNBLOOM); }
        for (u32 j = 1; j < (item == ITEM_HEAT_ROCK ? 8 : 5); j++)
            TURN {}
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

SINGLE_BATTLE_TEST("Sunbloom does not gain power from suppressed sun", s16 damage)
{
    bool32 sunny;
    PARAMETRIZE { sunny = FALSE; }
    PARAMETRIZE { sunny = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GOLDUCK) { Ability(ABILITY_CLOUD_NINE); }
    } WHEN {
        TURN { MOVE(player, sunny ? MOVE_SUNNY_DAY : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SUNBLOOM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNBLOOM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Sunbloom does not set sun when a target is immune")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BOUFFALANT) { Ability(ABILITY_SAP_SIPPER); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNBLOOM); }
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Sunbloom sets sun when it hits a substitute or knocks out its target")
{
    bool32 substitute;
    PARAMETRIZE { substitute = FALSE; }
    PARAMETRIZE { substitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(substitute ? 1000 : 1); MaxHP(1000); Speed(2); }
        OPPONENT(SPECIES_WYNAUT) { Speed(2); }
    } WHEN {
        TURN {
            if (substitute) { MOVE(opponent, MOVE_SUBSTITUTE); }
            MOVE(player, MOVE_SUNBLOOM);
            if (!substitute) { SEND_OUT(opponent, 1); }
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
    }
}

SINGLE_BATTLE_TEST("Infernal Fists hits exactly three times")
{
    GIVEN {
        ASSUME(GetMoveStrikeCount(MOVE_INFERNAL_FISTS) == 3);
        ASSUME(IsPunchingMove(MOVE_INFERNAL_FISTS));
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFERNAL_FISTS, secondaryEffect: FALSE); }
    } SCENE {
        for (u32 j = 0; j < 3; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
            HP_BAR(opponent);
        }
        MESSAGE("The Pokémon was hit 3 times!");
    }
}

SINGLE_BATTLE_TEST("Infernal Fists crits immediately after an earlier punch burns")
{
    s16 damage[3];
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFERNAL_FISTS, criticalHit: FALSE, secondaryEffect: TRUE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        STATUS_ICON(opponent, burn: TRUE);
        for (u32 j = 1; j < 3; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
            HP_BAR(opponent, captureDamage: &damage[j]);
            MESSAGE("A critical hit!");
        }
    } THEN {
        EXPECT_MUL_EQ(damage[0], Q_4_12(1.5), damage[1]);
        EXPECT_EQ(damage[1], damage[2]);
    }
}

SINGLE_BATTLE_TEST("Infernal Fists honors Battle Armor against a burned target")
{
    bool32 armor;
    PARAMETRIZE { armor = FALSE; }
    PARAMETRIZE { armor = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(armor ? ABILITY_BATTLE_ARMOR : ABILITY_SHADOW_TAG); Status1(STATUS1_BURN); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFERNAL_FISTS, criticalHit: FALSE); }
    } SCENE {
        for (u32 j = 0; j < 3; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
            HP_BAR(opponent);
            if (!armor)
                MESSAGE("A critical hit!");
        }
        if (armor)
            NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Infernal Fists has a 30 percent burn chance on its first punch")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFERNAL_FISTS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
        HP_BAR(opponent);
        STATUS_ICON(opponent, burn: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
    }
}

SINGLE_BATTLE_TEST("Infernal Fists stops after knocking out its target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_INFERNAL_FISTS); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
        HP_BAR(opponent, hp: 0);
        MESSAGE("The opposing Wobbuffet fainted!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_INFERNAL_FISTS, player);
    }
}
