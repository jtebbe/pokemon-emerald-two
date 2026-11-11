#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Shell Burst blocks the first physical move and applies Shell Smash for Shellder and Cloyster")
{
    enum Species species = SPECIES_SHELLDER;
    PARAMETRIZE { species = SPECIES_SHELLDER; }
    PARAMETRIZE { species = SPECIES_CLOYSTER; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_SCRATCH); } } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
        if (species == SPECIES_SHELLDER) MESSAGE("Shellder's Defense fell!");
        else MESSAGE("Cloyster's Defense fell!");
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_SHELL_BURST);
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT(gBattleStruct->battlerState[B_BATTLER_0].shellBurstUsed);
    }
}

SINGLE_BATTLE_TEST("Shell Burst needs an Apex Meteorite and stops protecting after the first physical move")
{
    enum Item item = ITEM_NONE;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE ? 2 : 0));
        EXPECT_EQ((u32)gBattleStruct->battlerState[B_BATTLER_0].shellBurstUsed, item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Shell Burst blocks every strike of a multihit physical move and fixed or OHKO physical damage")
{
    enum Move move = MOVE_DOUBLE_HIT;
    PARAMETRIZE { move = MOVE_DOUBLE_HIT; }
    PARAMETRIZE { move = MOVE_BULLET_SEED; }
    PARAMETRIZE { move = MOVE_SEISMIC_TOSS; }
    PARAMETRIZE { move = MOVE_FISSURE; }
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN { TURN { MOVE(opponent, move, hit: TRUE); } } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shell Burst is not spent on misses or Protect")
{
    bool32 protect = FALSE;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (protect) {
            TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_SCRATCH); }
        } else {
            TURN { MOVE(opponent, MOVE_ROCK_THROW, hit: FALSE); }
        }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Shell Burst ignores special and status moves")
{
    enum Move move = MOVE_WATER_GUN;
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_LEER; }
    GIVEN {
        PLAYER(SPECIES_SHELLDER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, move); } } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SHELL_BURST);
    } THEN {
        EXPECT(!gBattleStruct->battlerState[B_BATTLER_0].shellBurstUsed);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        if (move == MOVE_WATER_GUN) EXPECT_LT(player->hp, player->maxHP);
        else EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shell Burst leaves Substitute hits alone but blocks a physical move that bypasses Substitute")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SPECTRAL_THIEF); }
    } SCENE {
        SUB_HIT(player);
        ABILITY_POPUP(player, ABILITY_SHELL_BURST);
    } THEN {
        EXPECT_EQ(player->hp, 2250);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT(player->volatiles.substitute);
    }
}

SINGLE_BATTLE_TEST("Shell Burst resets on ordinary switching and Baton Pass")
{
    bool32 batonPass = FALSE;
    PARAMETRIZE { batonPass = FALSE; }
    PARAMETRIZE { batonPass = TRUE; }
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(50); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        if (batonPass) {
            TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
            TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 0); MOVE(opponent, MOVE_SCRATCH); }
        } else {
            TURN { SWITCH(player, 1); }
            TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SCRATCH); }
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHELL_BURST);
        ABILITY_POPUP(player, ABILITY_SHELL_BURST);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (batonPass ? 4 : 2));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (batonPass ? 2 : 1));
    }
}

DOUBLE_BATTLE_TEST("Shell Burst protects only its holder from spread attacks and leaves Dragon Darts' other target intact")
{
    enum Move move = MOVE_EARTHQUAKE;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    PARAMETRIZE { move = MOVE_DRAGON_DARTS; }
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DRAGAPULT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponentLeft, move, target: playerLeft); } } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_LT(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Shell Burst works on friendly fire and under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_SCRATCH, target: playerLeft); } } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_SHELL_BURST);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Haze does not restore a spent Shell Burst")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_HAZE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT(gBattleStruct->battlerState[B_BATTLER_0].shellBurstUsed);
    }
}

SINGLE_BATTLE_TEST("Shell Burst protects even at stat caps and clamps Shell Smash's boosts")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(player, MOVE_SHELL_SMASH); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 4);
    }
}

SINGLE_BATTLE_TEST("Entry hazards do not consume Shell Burst")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->hp, 2625);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}
