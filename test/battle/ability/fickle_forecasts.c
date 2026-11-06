#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

static u32 WeatherFromBallType(enum Type type)
{
    switch (type) {
    case TYPE_ICE: return B_WEATHER_SNOW;
    case TYPE_WATER: return B_WEATHER_RAIN_NORMAL;
    case TYPE_FIRE: return B_WEATHER_SUN_NORMAL;
    default: return B_WEATHER_NONE;
    }
}

static enum Species FormFromWeather(u32 weather)
{
    if (weather & B_WEATHER_SUN) return SPECIES_CASTFORM_SUNNY;
    if (weather & B_WEATHER_RAIN) return SPECIES_CASTFORM_RAINY;
    if (weather & B_WEATHER_SNOW) return SPECIES_CASTFORM_SNOWY;
    return SPECIES_CASTFORM_NORMAL;
}

TEST("Fickle Forecasts chooses only snow rain and sun and excludes the current weather")
{
    static const u32 weathers[] = {
        B_WEATHER_NONE, B_WEATHER_SNOW, B_WEATHER_RAIN_NORMAL, B_WEATHER_SUN_NORMAL,
        B_WEATHER_RAIN_PRIMAL, B_WEATHER_SUN_PRIMAL, B_WEATHER_SANDSTORM, B_WEATHER_HAIL, B_WEATHER_FOG,
    };
    for (u32 i = 0; i < ARRAY_COUNT(weathers); i++) {
        u32 weather = weathers[i];
        enum BattleWeather choices[3];
        u32 count = GetFickleForecastsWeathers(weather, choices);
        EXPECT_EQ(count, weather & (B_WEATHER_SNOW | B_WEATHER_RAIN | B_WEATHER_SUN) ? 2 : 3);
        u32 excluded = weather & B_WEATHER_RAIN ? B_WEATHER_RAIN_NORMAL
                     : weather & B_WEATHER_SUN ? B_WEATHER_SUN_NORMAL : weather;
        for (u32 first = 0; first < count; first++) {
            EXPECT(choices[first] == BATTLE_WEATHER_SNOW || choices[first] == BATTLE_WEATHER_RAIN || choices[first] == BATTLE_WEATHER_SUN);
            EXPECT(!(gBattleWeatherInfo[choices[first]].flag & excluded));
            for (u32 second = first + 1; second < count; second++)
                EXPECT_NE(choices[first], choices[second]);
        }
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts is Castform's Apex ability in every form and changes weather on entry and turn end")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_CASTFORM_NORMAL; }
    PARAMETRIZE { species = SPECIES_CASTFORM_SUNNY; }
    PARAMETRIZE { species = SPECIES_CASTFORM_RAINY; }
    PARAMETRIZE { species = SPECIES_CASTFORM_SNOWY; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_FICKLE_FORECASTS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_FICKLE_FORECASTS);
        EXPECT_NE(WeatherFromBallType(gLastHitByType[1]), B_WEATHER_NONE);
        EXPECT_NE(gBattleWeather, WeatherFromBallType(gLastHitByType[1]));
        EXPECT(gBattleWeather == B_WEATHER_SNOW || gBattleWeather == B_WEATHER_RAIN_NORMAL || gBattleWeather == B_WEATHER_SUN_NORMAL);
        EXPECT_EQ(player->species, FormFromWeather(gBattleWeather));
        EXPECT_EQ(player->types[0], GetSpeciesType(player->species, 0));
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts retains Forecast without retriggering entry weather from its form-change script")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        MESSAGE("Castform transformed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        MESSAGE("Castform transformed!");
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts replaces existing entry weather with a different weather")
{
    enum Ability ability;
    enum Type oldType;
    PARAMETRIZE { ability = ABILITY_DRIZZLE; oldType = TYPE_WATER; }
    PARAMETRIZE { ability = ABILITY_DROUGHT; oldType = TYPE_FIRE; }
    PARAMETRIZE { ability = ABILITY_SNOW_WARNING; oldType = TYPE_ICE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } THEN {
        EXPECT_NE(WeatherFromBallType(gLastHitByType[1]), B_WEATHER_NONE);
        EXPECT_NE(gLastHitByType[1], oldType);
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts checks the weather present at turn end after an opponent changes it")
{
    enum Move move;
    u32 weather;
    PARAMETRIZE { move = MOVE_RAIN_DANCE; weather = B_WEATHER_RAIN_NORMAL; }
    PARAMETRIZE { move = MOVE_SUNNY_DAY; weather = B_WEATHER_SUN_NORMAL; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; weather = B_WEATHER_SNOW; }
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT_NE(gBattleWeather, weather);
        EXPECT_EQ(player->species, FormFromWeather(gBattleWeather));
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts requires an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FICKLE_FORECASTS);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(GetBattlerAbility(0), ABILITY_FORECAST);
        EXPECT_EQ(player->species, SPECIES_CASTFORM_NORMAL);
        EXPECT_EQ(gLastHitByType[1], TYPE_NORMAL);
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts makes Weather Ball target both opponents and spare its ally")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_WEATHER_BALL; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, move, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft);
        if (move == MOVE_WEATHER_BALL) {
            HP_BAR(opponentRight);
            NOT HP_BAR(playerRight);
        } else {
            NONE_OF { HP_BAR(opponentRight); HP_BAR(playerRight); }
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        if (move == MOVE_WEATHER_BALL)
            EXPECT_EQ(gLastHitByType[1], gLastHitByType[3]);
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts leaves ordinary Castform's Weather Ball single-targeted")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft);
        NOT HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts Weather Ball uses the normal spread damage reduction", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(item); Speed(1); SpAttack(200); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); Speed(50); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_RAIN_DANCE); MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        if (item == ITEM_APEX_METEORITE)
            HP_BAR(opponentRight);
    } FINALLY {
        // Spread rounding happens before the weather and STAB modifiers.
        EXPECT_GE(results[1].damage, results[0].damage * 3 / 4 - 3);
        EXPECT_LE(results[1].damage, results[0].damage * 3 / 4 + 3);
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts Weather Ball is blocked by Wide Guard")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_WIDE_GUARD); MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        NONE_OF { HP_BAR(opponentLeft); HP_BAR(opponentRight); }
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts Weather Ball still hits the unprotected opponent")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PROTECT); MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentLeft);
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts respects primal and locked overworld weather")
{
    bool32 primal;
    PARAMETRIZE { primal = FALSE; }
    PARAMETRIZE { primal = TRUE; }
    ResetStartingStatuses();
    if (!primal) SetStartingStatus(STARTING_STATUS_WEATHER_SUN);
    GIVEN {
        WITH_CONFIG(B_OVERWORLD_WEATHER_OVERRIDE, GEN_9);
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_GROUDON) { Ability(primal ? ABILITY_DESOLATE_LAND : ABILITY_NONE); Speed(100); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT_EQ(player->species, SPECIES_CASTFORM_SUNNY);
        if (primal) EXPECT(gBattleWeather & B_WEATHER_SUN_PRIMAL);
        ResetStartingStatuses();
    }
}

DOUBLE_BATTLE_TEST("Fickle Forecasts still changes weather under Cloud Nine but its Weather Ball remains Normal and spreads")
{
    GIVEN {
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CLOUD_NINE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, playerLeft);
    } THEN {
        EXPECT_EQ(GetWeather(), B_WEATHER_NONE);
        EXPECT_NE(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(playerLeft->species, SPECIES_CASTFORM_NORMAL);
        EXPECT_EQ(gLastHitByType[1], TYPE_NORMAL);
        EXPECT_EQ(gLastHitByType[3], TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Fickle Forecasts does not reset weather while its holder is benched")
{
    GIVEN {
        WITH_CONFIG(B_ABILITY_WEATHER, GEN_9);
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN {}
    } THEN {
        EXPECT_EQ(gBattleStruct->weatherDuration, 3);
    }
}
