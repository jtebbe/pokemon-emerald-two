#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Adaptive Build is Seel and Dewgong's Apex ability only with the Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_SEEL; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SEEL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DEWGONG; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_DEWGONG; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_HYDRATION); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_ADAPTIVE_BUILD);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_NONE ? ABILITY_HYDRATION : ABILITY_ADAPTIVE_BUILD);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build doubles Attack only in snow and Sp Atk only in rain", s16 damage)
{
    static const enum Species speciesList[] = {SPECIES_SEEL, SPECIES_DEWGONG};
    static const enum Move weathers[] = {MOVE_CELEBRATE, MOVE_RAIN_DANCE, MOVE_SNOWSCAPE, MOVE_HAIL, MOVE_SUNNY_DAY};
    static const enum Move moves[] = {MOVE_TACKLE, MOVE_WATER_GUN};
    enum Species species = SPECIES_SEEL;
    enum Move weather = MOVE_CELEBRATE, move = MOVE_TACKLE;
    bool32 apex = FALSE;
    for (u32 s = 0; s < ARRAY_COUNT(speciesList); s++)
        for (u32 w = 0; w < ARRAY_COUNT(weathers); w++)
            for (u32 m = 0; m < ARRAY_COUNT(moves); m++)
            {
                PARAMETRIZE { species = speciesList[s]; weather = weathers[w]; move = moves[m]; apex = FALSE; }
                PARAMETRIZE { species = speciesList[s]; weather = weathers[w]; move = moves[m]; apex = TRUE; }
            }
    GIVEN {
        PLAYER(species) {
            Ability(ABILITY_HYDRATION); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE);
            Attack(!apex && weather == MOVE_SNOWSCAPE ? 200 : 100);
            SpAttack(!apex && weather == MOVE_RAIN_DANCE ? 200 : 100);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(opponent, weather); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(speciesList) * ARRAY_COUNT(weathers) * ARRAY_COUNT(moves) * 2; j += 2)
            EXPECT_EQ(results[j].damage, results[j + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build doubles Defense in snow and Sp Def in rain while retaining Thick Fat", s16 damage)
{
    static const enum Species speciesList[] = {SPECIES_SEEL, SPECIES_DEWGONG};
    static const enum Move weathers[] = {MOVE_CELEBRATE, MOVE_RAIN_DANCE, MOVE_SNOWSCAPE, MOVE_HAIL, MOVE_SUNNY_DAY};
    static const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_FIRE_PUNCH, MOVE_POWDER_SNOW};
    enum Species species = SPECIES_SEEL;
    enum Move weather = MOVE_CELEBRATE, move = MOVE_SCRATCH;
    bool32 apex = FALSE;
    for (u32 s = 0; s < ARRAY_COUNT(speciesList); s++)
        for (u32 w = 0; w < ARRAY_COUNT(weathers); w++)
            for (u32 m = 0; m < ARRAY_COUNT(moves); m++)
            {
                PARAMETRIZE { species = speciesList[s]; weather = weathers[w]; move = moves[m]; apex = FALSE; }
                PARAMETRIZE { species = speciesList[s]; weather = weathers[w]; move = moves[m]; apex = TRUE; }
            }
    GIVEN {
        PLAYER(species) {
            Ability(ABILITY_THICK_FAT); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(3000); MaxHP(3000);
            Defense(!apex && weather == MOVE_SNOWSCAPE ? 200 : 100);
            SpDefense(!apex && weather == MOVE_RAIN_DANCE ? 200 : 100);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); }
    } WHEN {
        TURN { MOVE(opponent, weather); }
        TURN { MOVE(opponent, move, criticalHit: FALSE, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(speciesList) * ARRAY_COUNT(weathers) * ARRAY_COUNT(moves) * 2; j += 2)
            EXPECT_EQ(results[j].damage, results[j + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build's weather defenses follow the stat used by Psyshock and Wonder Room", s16 damage)
{
    enum Move weather;
    bool32 room, apex;
    PARAMETRIZE { weather = MOVE_RAIN_DANCE; room = FALSE; apex = FALSE; }
    PARAMETRIZE { weather = MOVE_RAIN_DANCE; room = FALSE; apex = TRUE; }
    PARAMETRIZE { weather = MOVE_RAIN_DANCE; room = TRUE; apex = FALSE; }
    PARAMETRIZE { weather = MOVE_RAIN_DANCE; room = TRUE; apex = TRUE; }
    PARAMETRIZE { weather = MOVE_SNOWSCAPE; room = FALSE; apex = FALSE; }
    PARAMETRIZE { weather = MOVE_SNOWSCAPE; room = FALSE; apex = TRUE; }
    PARAMETRIZE { weather = MOVE_SNOWSCAPE; room = TRUE; apex = FALSE; }
    PARAMETRIZE { weather = MOVE_SNOWSCAPE; room = TRUE; apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SEEL) {
            Ability(ABILITY_HYDRATION); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(3000); MaxHP(3000);
            Defense(!apex && weather == MOVE_SNOWSCAPE ? 200 : 100);
            SpDefense(!apex && weather == MOVE_RAIN_DANCE ? 200 : 100);
        }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); }
    } WHEN {
        TURN { MOVE(opponent, weather); if (room) MOVE(player, MOVE_WONDER_ROOM); }
        TURN { MOVE(opponent, MOVE_PSYSHOCK, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < 8; j += 2)
            EXPECT_EQ(results[j].damage, results[j + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build stacks with Dewgong's normal Ice-type snow Defense bonus", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DEWGONG) { Ability(ABILITY_HYDRATION); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Defense(apex ? 100 : 300); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); }
    } WHEN {
        TURN { if (apex) MOVE(opponent, MOVE_SNOWSCAPE); }
        TURN { MOVE(opponent, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build's weather boosts stop immediately when weather is overwritten")
{
    s16 damage[3];
    GIVEN {
        PLAYER(SPECIES_SEEL) { Item(ITEM_APEX_METEORITE); Speed(1); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(100); HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_SNOWSCAPE); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage[0]);
        HP_BAR(opponent, captureDamage: &damage[1]);
        HP_BAR(opponent, captureDamage: &damage[2]);
    } THEN {
        EXPECT_EQ(damage[0], damage[2]);
        EXPECT_GT(damage[1], damage[0]);
        EXPECT_EQ(player->attack, 100);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build's weather boosts are suppressed by Cloud Nine and Air Lock", s16 damage)
{
    enum Ability suppressor;
    enum Move weather, move;
    bool32 apex;
    PARAMETRIZE { suppressor = ABILITY_CLOUD_NINE; weather = MOVE_RAIN_DANCE; move = MOVE_CONFUSION; apex = FALSE; }
    PARAMETRIZE { suppressor = ABILITY_CLOUD_NINE; weather = MOVE_RAIN_DANCE; move = MOVE_CONFUSION; apex = TRUE; }
    PARAMETRIZE { suppressor = ABILITY_AIR_LOCK; weather = MOVE_SNOWSCAPE; move = MOVE_TACKLE; apex = FALSE; }
    PARAMETRIZE { suppressor = ABILITY_AIR_LOCK; weather = MOVE_SNOWSCAPE; move = MOVE_TACKLE; apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SEEL) { Ability(ABILITY_HYDRATION); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(suppressor); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, weather); }
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetWeather(), B_WEATHER_NONE);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Build retains Thick Fat and weather defenses against Mold Breaker", s16 damage)
{
    enum Ability ability;
    enum Move weather, move;
    PARAMETRIZE { ability = ABILITY_UNNERVE; weather = MOVE_RAIN_DANCE; move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; weather = MOVE_RAIN_DANCE; move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_UNNERVE; weather = MOVE_SNOWSCAPE; move = MOVE_ICE_PUNCH; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; weather = MOVE_SNOWSCAPE; move = MOVE_ICE_PUNCH; }
    GIVEN {
        PLAYER(SPECIES_DEWGONG) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_HAXORUS) { Ability(ability); Attack(100); SpAttack(100); }
    } WHEN {
        TURN { MOVE(opponent, weather); }
        TURN { MOVE(opponent, move, criticalHit: FALSE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
