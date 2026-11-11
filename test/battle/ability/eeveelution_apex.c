#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("High Voltage deals neutral damage to Ground types and bypasses Volt Absorb")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_JOLTEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GLIGAR) { Ability(ABILITY_VOLT_ABSORB); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_VOLT_ABSORB);
            MESSAGE("It's mostly ineffective…");
            MESSAGE("It's not very effective…");
            MESSAGE("It's super effective!");
            MESSAGE("It's extremely effective!");
        }
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("High Voltage applies a legal status only on damaging Electric hits")
{
    enum Species species;
    enum Move move;
    u32 status;
    PARAMETRIZE { species = SPECIES_SLUGMA; move = MOVE_SHOCK_WAVE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { species = SPECIES_PIKACHU; move = MOVE_SHOCK_WAVE; status = STATUS1_BURN; }
    PARAMETRIZE { species = SPECIES_PIKACHU; move = MOVE_SWIFT; status = STATUS1_NONE; }
    GIVEN {
        WITH_CONFIG(B_PARALYZE_ELECTRIC, GEN_6);
        PLAYER(SPECIES_JOLTEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Aquatic Form blocks physical damage and doubles special damage only in rain")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    ResetStartingStatuses();
    if (rain)
        SetStartingStatus(STARTING_STATUS_WEATHER_RAIN);
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Item(ITEM_APEX_METEORITE); HP(240); MaxHP(240); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(40); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        if (rain) {
            ABILITY_POPUP(player, ABILITY_AQUATIC_FORM);
            HP_BAR(player, damage: 80);
        } else {
            HP_BAR(player, damage: 40);
            HP_BAR(player, damage: 40);
        }
    }
}

SINGLE_BATTLE_TEST("Inner Furnace boosts Guts and Facade without inflicting burn damage", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FLAREON) { Ability(ABILITY_GUTS); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(400); MaxHP(400); Status1(apex ? STATUS1_NONE : STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FACADE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        if (apex) {
            EXPECT_EQ(player->hp, 400);
            EXPECT_EQ(player->status1, STATUS1_NONE);
            EXPECT_EQ(GetEffectiveBattlerStatus(B_BATTLER_0, ABILITY_INNER_FURNACE), STATUS1_BURN);
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Inner Furnace absorbs Water moves to set Mist and prevents other major status")
{
    GIVEN {
        PLAYER(SPECIES_FLAREON) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_SPORE); }
    } THEN {
        EXPECT_EQ(player->hp, 400);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_MIST);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Photosynthetic summons five-turn sun and activates Chlorophyll and healing")
{
    GIVEN {
        PLAYER(SPECIES_LEAFEON) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(240); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_MAGICAL_LEAF); }
    } SCENE {
        HP_BAR(player, hp: 130);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_SUN_NORMAL);
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, ABILITY_PHOTOSYNTHETIC, HOLD_EFFECT_APEX_METEORITE), 200);
        EXPECT_EQ(player->hp, 130);
    }
}

SINGLE_BATTLE_TEST("Frosted Fur summons snow and heals with Ice Body in snow", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GLACEON) { Ability(ABILITY_FUR_COAT); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(160); MaxHP(320); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        if (apex)
            TURN { MOVE(player, MOVE_ICE_BEAM, secondaryEffect: FALSE); MOVE(opponent, MOVE_TACKLE, criticalHit: FALSE); }
        else
            TURN { MOVE(player, MOVE_SNOWSCAPE); MOVE(opponent, MOVE_TACKLE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_SNOW);
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
        EXPECT_EQ(player->hp, 160 - results[i].damage + (apex ? 20 : 0));
        struct BattleCalcValues cv = { .battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0, .move = MOVE_TACKLE };
        cv.abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0);
        EXPECT_EQ(GetTotalAccuracy(&cv, B_WEATHER_SNOW), apex ? 80 : 100);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Predictive Mind dodges priority and inaccurate moves but allows accurate attacks")
{
    GIVEN {
        PLAYER(SPECIES_ESPEON) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_ATTACK); }
        TURN { MOVE(opponent, MOVE_HYDRO_PUMP); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PREDICTIVE_MIND);
        ABILITY_POPUP(player, ABILITY_PREDICTIVE_MIND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, opponent);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Umbral Thirst absorbs Fairy moves and uses Defense and Sp. Def for attacks", s16 physical, s16 special)
{
    bool32 empower;
    PARAMETRIZE { empower = FALSE; }
    PARAMETRIZE { empower = TRUE; }
    GIVEN {
        PLAYER(SPECIES_UMBREON) { Item(ITEM_APEX_METEORITE); Attack(empower ? 50 : 140); SpAttack(empower ? 50 : 180); Defense(140); SpDefense(180); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, empower ? MOVE_MOONBLAST : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
        TURN { MOVE(player, MOVE_SWIFT, criticalHit: FALSE); }
    } SCENE {
        NOT HP_BAR(player);
        HP_BAR(opponent, captureDamage: &results[i].physical);
        HP_BAR(opponent, captureDamage: &results[i].special);
    } THEN {
        EXPECT_EQ((u32)player->volatiles.umbralThirst, empower);
    } FINALLY {
        EXPECT_EQ(results[0].physical, results[1].physical);
        EXPECT_EQ(results[0].special, results[1].special);
    }
}

SINGLE_BATTLE_TEST("Rainbow Skies clears weather and terrain and creates a permanent rainbow only when needed")
{
    bool32 existingRainbow;
    PARAMETRIZE { existingRainbow = FALSE; }
    PARAMETRIZE { existingRainbow = TRUE; }
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_WEATHER_RAIN_TEMPORARY);
    SetStartingStatus(STARTING_STATUS_PSYCHIC_TERRAIN_TEMPORARY);
    if (existingRainbow)
        SetStartingStatus(STARTING_STATUS_RAINBOW_PLAYER_TEMPORARY);
    GIVEN {
        PLAYER(SPECIES_SYLVEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_DUSKULL) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ((enum BattleTerrain)gFieldTimers.terrain, B_TERRAIN_NONE);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].rainbowTimer, existingRainbow ? 3 : 0);
    }
}
