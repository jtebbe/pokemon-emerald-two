#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Sea Herald makes Slowking's Water attacks ignore accuracy and evasion")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); }
    } SCENE { HP_BAR(opponent); }
}

DOUBLE_BATTLE_TEST("Sea Herald grants its ally perfect accuracy only on Water moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; }
    PARAMETRIZE { move = MOVE_ZAP_CANNON; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, move, target: opponentLeft, hit: FALSE); }
    } SCENE {
        if (move == MOVE_HYDRO_PUMP) HP_BAR(opponentLeft);
        else NOT HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Sea Herald lets its holder and ally bypass Substitute with damaging and status Water moves")
{
    bool32 ally;
    enum Move move;
    PARAMETRIZE { ally = FALSE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { ally = TRUE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { ally = FALSE; move = MOVE_SOAK; }
    PARAMETRIZE { ally = TRUE; move = MOVE_SOAK; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        if (ally)
            TURN { MOVE(opponentLeft, MOVE_SUBSTITUTE); MOVE(playerRight, move, target: opponentLeft, hit: FALSE); }
        else
            TURN { MOVE(opponentLeft, MOVE_SUBSTITUTE); MOVE(playerLeft, move, target: opponentLeft, hit: FALSE); }
    } THEN {
        EXPECT_EQ((u32)opponentLeft->volatiles.substituteHP, 100);
        if (move == MOVE_SOAK) EXPECT_EQ(opponentLeft->types[0], TYPE_WATER);
        else EXPECT_LT(opponentLeft->hp, 300);
    }
}

DOUBLE_BATTLE_TEST("Sea Herald only bypasses Protect in rain for its holder and ally")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(rain ? ABILITY_DRIZZLE : ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PROTECT); MOVE(playerLeft, MOVE_WATER_GUN, target: opponentLeft); MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft); }
    } THEN {
        if (rain) EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        else EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
    }
}

SINGLE_BATTLE_TEST("Sea Herald pierces contact protection in rain without suffering its retaliation")
{
    enum Move protect;
    PARAMETRIZE { protect = MOVE_PROTECT; }
    PARAMETRIZE { protect = MOVE_SPIKY_SHIELD; }
    PARAMETRIZE { protect = MOVE_BANEFUL_BUNKER; }
    PARAMETRIZE { protect = MOVE_KINGS_SHIELD; }
    PARAMETRIZE { protect = MOVE_OBSTRUCT; }
    PARAMETRIZE { protect = MOVE_SILK_TRAP; }
    PARAMETRIZE { protect = MOVE_BURNING_BULWARK; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, protect); MOVE(player, MOVE_AQUA_TAIL, hit: FALSE); }
    } SCENE { HP_BAR(opponent); } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Sea Herald bypasses Wide Guard in rain")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WIDE_GUARD); MOVE(playerLeft, MOVE_MUDDY_WATER, hit: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Sea Herald's rain protection bypass stops under Cloud Nine")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CLOUD_NINE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PROTECT); MOVE(playerLeft, MOVE_WATER_GUN, target: opponentLeft); }
    } SCENE { NOT HP_BAR(opponentLeft); }
}

SINGLE_BATTLE_TEST("Sea Herald does not hit semi-invulnerable targets")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FLY); MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); }
    } SCENE { NOT HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Sea Herald preserves Water absorption abilities")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_WATER_ABSORB); }
    } WHEN { TURN { MOVE(player, MOVE_WATER_GUN); } } SCENE {
        ABILITY_POPUP(opponent, ABILITY_WATER_ABSORB);
        NOT HP_BAR(opponent);
    }
}

DOUBLE_BATTLE_TEST("Sea Herald's aura stops when its holder faints")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); Speed(200); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PROTECT); MOVE(opponentRight, MOVE_QUICK_ATTACK, target: playerLeft); MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(GetSeaHeraldBattler(2, TYPE_WATER, NULL), MAX_BATTLERS_COUNT);
    }
}

SINGLE_BATTLE_TEST("Sea Herald uses Weather Ball's resolved type for perfect accuracy and rain protection bypass")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(rain ? ABILITY_DRIZZLE : ABILITY_DROUGHT); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_WEATHER_BALL, hit: FALSE); }
    } THEN {
        if (rain) EXPECT_LT(opponent->hp, opponent->maxHP);
        else EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
