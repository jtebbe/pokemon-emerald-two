#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("River Esper belongs to Psyduck and Golduck with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_PSYDUCK; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLDUCK; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PSYDUCK; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_GOLDUCK; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Ability(ABILITY_DAMP); Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_RIVER_ESPER); EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_APEX_METEORITE ? ABILITY_RIVER_ESPER : ABILITY_DAMP); }
}

SINGLE_BATTLE_TEST("River Esper boosts Speed before Psychic attacks and Special Attack before Water attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CONFUSION; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE {
        ABILITY_POPUP(player, ABILITY_RIVER_ESPER);
        if (move == MOVE_CONFUSION) MESSAGE("Psyduck's Speed rose sharply!");
        else MESSAGE("Psyduck's Sp. Atk rose sharply!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (move == MOVE_CONFUSION ? 2 : 0));
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (move == MOVE_WATER_GUN ? 2 : 0));
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("River Esper's triggering Water attack uses the sharply raised Special Attack", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Ability(ABILITY_DAMP); Item(item); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN { TURN { MOVE(player, MOVE_SURF, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage); }
}

SINGLE_BATTLE_TEST("River Esper boosts both stats for Psychic and Water moves in ordinary and primal rain")
{
    enum Move move;
    bool32 primal;
    PARAMETRIZE { move = MOVE_CONFUSION; primal = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; primal = FALSE; }
    PARAMETRIZE { move = MOVE_CONFUSION; primal = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; primal = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(primal ? SPECIES_KYOGRE_PRIMAL : SPECIES_WOBBUFFET) { Ability(primal ? ABILITY_PRIMORDIAL_SEA : ABILITY_NONE); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { if (!primal) MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, move); } }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); ANIMATION(ANIM_TYPE_MOVE, move, player); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("River Esper ignores rain suppressed by Cloud Nine or Air Lock")
{
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { ability = ABILITY_CLOUD_NINE; move = MOVE_CONFUSION; }
    PARAMETRIZE { ability = ABILITY_AIR_LOCK; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, move); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (move == MOVE_CONFUSION ? 2 : 0)); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (move == MOVE_WATER_GUN ? 2 : 0)); }
}

SINGLE_BATTLE_TEST("River Esper does not boost on unrelated moves or boost the foe using Water and Psychic attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CONFUSION; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, move); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_RIVER_ESPER); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("River Esper works before Psychic and Water status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CALM_MIND; }
    PARAMETRIZE { move = MOVE_AQUA_RING; }
    GIVEN { PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, move); } }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); ANIMATION(ANIM_TYPE_MOVE, move, player); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (move == MOVE_CALM_MIND ? 2 : 0)); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (move == MOVE_CALM_MIND ? 1 : 2)); }
}

SINGLE_BATTLE_TEST("River Esper checks rain before Rain Dance creates it")
{
    GIVEN { PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_RAIN_DANCE); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); EXPECT(GetWeather() & B_WEATHER_RAIN); }
}

SINGLE_BATTLE_TEST("River Esper triggers before misses Protect and Psychic type immunity")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(block == 2 ? SPECIES_UMBREON : SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, block == 0 ? MOVE_HYDRO_PUMP : MOVE_CONFUSION, hit: block != 0); if (block == 1) MOVE(opponent, MOVE_PROTECT); } }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); NOT HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[block == 0 ? STAT_SPATK : STAT_SPEED], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(opponent->hp, opponent->maxHP); }
}

SINGLE_BATTLE_TEST("River Esper does not trigger if sleep paralysis or flinching prevents the move")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); Speed(1); Status1(block == 0 ? STATUS1_SLEEP : block == 1 ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_CONFUSION, WITH_RNG(RNG_PARALYSIS, TRUE)); if (block == 2) MOVE(opponent, MOVE_FAKE_OUT); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_RIVER_ESPER); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("River Esper repeats per move use and stops showing a popup at the stat cap")
{
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); }
    } WHEN { for (u32 turn = 0; turn < 4; turn++) TURN { MOVE(player, MOVE_CONFUSION, secondaryEffect: FALSE); } }
    SCENE { for (u32 turn = 0; turn < 3; turn++) { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); HP_BAR(opponent); } NOT ABILITY_POPUP(player, ABILITY_RIVER_ESPER); HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("River Esper raises the uncapped stat in rain even if the other is capped")
{
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); }
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN { MOVE(player, MOVE_CONFUSION, secondaryEffect: FALSE); }
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CONFUSION, secondaryEffect: FALSE); }
    } THEN { EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); }
}

DOUBLE_BATTLE_TEST("River Esper boosts once for an entire spread move")
{
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_SURF); } }
    SCENE { ABILITY_POPUP(playerLeft, ABILITY_RIVER_ESPER); ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, playerLeft); HP_BAR(opponentLeft); HP_BAR(playerRight); HP_BAR(opponentRight); }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("River Esper boosts once for a multihit move")
{
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_WATER_SHURIKEN, WITH_RNG(RNG_HITS, 5)); } }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); for (u32 hit = 0; hit < 5; hit++) HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("River Esper uses the effective type of Revelation Dance and rainy Weather Ball")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_REVELATION_DANCE; }
    PARAMETRIZE { move = MOVE_WEATHER_BALL; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { if (move == MOVE_WEATHER_BALL) MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, move); } }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (move == MOVE_WEATHER_BALL ? 2 : 0)); }
}

SINGLE_BATTLE_TEST("River Esper works for Psychic moves called by Nature Power and Metronome")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_NATURE_POWER; }
    PARAMETRIZE { move = MOVE_METRONOME; }
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC_TERRAIN); }
        TURN { if (move == MOVE_METRONOME) MOVE(player, move, WITH_RNG(RNG_METRONOME, MOVE_CONFUSION)); else MOVE(player, move); }
    } SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("River Esper boosts on Future Sight setup without boosting again on its delayed hit")
{
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_FUTURE_SIGHT); } TURN {} TURN {} }
    SCENE { ABILITY_POPUP(player, ABILITY_RIVER_ESPER); MESSAGE("Golduck foresaw an attack!"); HP_BAR(opponent); NOT ABILITY_POPUP(player, ABILITY_RIVER_ESPER); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("River Esper's Speed boost takes effect after acting rather than changing who has already moved")
{
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Item(ITEM_APEX_METEORITE); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CONFUSION, secondaryEffect: FALSE); } TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CONFUSION, secondaryEffect: FALSE); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ABILITY_POPUP(player, ABILITY_RIVER_ESPER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, player);
        ABILITY_POPUP(player, ABILITY_RIVER_ESPER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("River Esper's rain bonus is blocked by Utility Umbrella")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CONFUSION; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Ability(ABILITY_RIVER_ESPER); Item(ITEM_UTILITY_UMBRELLA); Speed(1); }
        OPPONENT(SPECIES_POLITOED) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(player, move); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (move == MOVE_CONFUSION ? 2 : 0)); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (move == MOVE_WATER_GUN ? 2 : 0)); }
}

DOUBLE_BATTLE_TEST("River Esper boosts each Psychic move used again through Instruct")
{
    GIVEN {
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_ORANGURU) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_CONFUSION, target: opponentLeft); MOVE(playerRight, MOVE_INSTRUCT, target: playerLeft); } }
    SCENE { ABILITY_POPUP(playerLeft, ABILITY_RIVER_ESPER); HP_BAR(opponentLeft); ABILITY_POPUP(playerLeft, ABILITY_RIVER_ESPER); HP_BAR(opponentLeft); }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 4); }
}
