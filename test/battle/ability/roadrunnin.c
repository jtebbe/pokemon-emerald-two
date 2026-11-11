#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Roadrunnin requires the Apex Meteorite and boosts Doduo and Dodrio on entry")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_DODUO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DODRIO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DODUO; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_DODRIO; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_RUN_AWAY); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_NONE ? ABILITY_RUN_AWAY : ABILITY_ROADRUNNIN);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE));
    }
}

SINGLE_BATTLE_TEST("Roadrunnin boosts Speed once after an attack including multihits and substitutes")
{
    enum Move move;
    bool32 substitute;
    PARAMETRIZE { move = MOVE_TACKLE; substitute = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; substitute = FALSE; }
    PARAMETRIZE { move = MOVE_DOUBLE_KICK; substitute = FALSE; }
    PARAMETRIZE { move = MOVE_DOUBLE_KICK; substitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (substitute)
            TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
        MESSAGE("Doduo's Speed rose!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
        MESSAGE("Doduo's Speed rose!");
        NOT ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin uses exact effective Speed tiers for physical and special damage", s16 damage; uq4_12_t modifier)
{
    static const u16 speeds[] = {200, 150, 101, 100, 76, 75, 61, 60, 51, 50, 1};
    static const uq4_12_t modifiers[] = {
        UQ_4_12(1.0), UQ_4_12(1.0), UQ_4_12(1.0), UQ_4_12(1.25),
        UQ_4_12(1.25), UQ_4_12(1.5), UQ_4_12(1.5), UQ_4_12(1.75),
        UQ_4_12(1.75), UQ_4_12(2.0), UQ_4_12(2.0),
    };
    static const enum Move moves[] = {MOVE_TACKLE, MOVE_WATER_GUN};
    enum Move move = MOVE_TACKLE;
    enum Item item = ITEM_NONE;
    u16 speed = 100;
    uq4_12_t modifier = UQ_4_12(1.0);
    for (u32 m = 0; m < ARRAY_COUNT(moves); m++)
    {
        PARAMETRIZE { move = moves[m]; item = ITEM_NONE; speed = 100; modifier = UQ_4_12(1.0); }
        for (u32 j = 0; j < ARRAY_COUNT(speeds); j++)
            PARAMETRIZE { move = moves[m]; item = ITEM_APEX_METEORITE; speed = speeds[j]; modifier = modifiers[j]; }
    }
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(item); Speed(100); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(speed); Defense(100); SpDefense(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        results[i].modifier = modifier;
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves) * (ARRAY_COUNT(speeds) + 1); j++)
        {
            u32 baseline = j / (ARRAY_COUNT(speeds) + 1) * (ARRAY_COUNT(speeds) + 1);
            EXPECT_MUL_EQ(results[baseline].damage, results[j].modifier, results[j].damage);
        }
    }
}

SINGLE_BATTLE_TEST("Roadrunnin compares effective Speed including paralysis Tailwind and Trick Room", s16 damage)
{
    u32 mode;
    PARAMETRIZE { mode = 0; }
    PARAMETRIZE { mode = 1; }
    PARAMETRIZE { mode = 2; }
    PARAMETRIZE { mode = 3; }
    GIVEN {
        WITH_CONFIG(B_PARALYSIS_SPEED, GEN_7);
        PLAYER(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); Speed(200); Status1(mode == 1 ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { if (mode == 2) MOVE(opponent, MOVE_TAILWIND); if (mode == 3) MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[3].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.6), results[0].damage);
    }
}

DOUBLE_BATTLE_TEST("Roadrunnin checks both spread targets and boosts Speed only once", s16 left; s16 right)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_DODRIO) { Item(item); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(100); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(50); HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].left);
        HP_BAR(opponentRight, captureDamage: &results[i].right);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE ? 2 : 0));
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].left, UQ_4_12(1.25), results[1].left);
        EXPECT_MUL_EQ(results[0].right, UQ_4_12(2.0), results[1].right);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin still boosts Speed after misses Protect and type immunity")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(block == 2 ? SPECIES_GASTLY : SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, hit: block != 0); if (block == 1) MOVE(opponent, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin does not trigger if sleep paralysis or flinching prevents the attack")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); Status1(block == 0 ? STATUS1_SLEEP : block == 1 ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, TRUE)); if (block == 2) MOVE(opponent, MOVE_FAKE_OUT); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin boosts only after the attacking turn of a charged move")
{
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
        NOT ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin caps Speed normally and does not multiply fixed damage")
{
    GIVEN {
        PLAYER(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); Level(50); Speed(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
    } WHEN {
        for (u32 turn = 0; turn < 8; turn++)
            TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        for (u32 turn = 0; turn < 8; turn++)
            HP_BAR(opponent, damage: 50);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin does not boost Speed while storing energy with Bide")
{
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BIDE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin does not boost a user fainted by contact punishment")
{
    GIVEN {
        PLAYER(SPECIES_DODUO) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_GARCHOMP) { Ability(ABILITY_ROUGH_SKIN); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
        HP_BAR(opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_ROADRUNNIN);
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Roadrunnin reactivates on reentry and is not suppressed by Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_ROADRUNNIN);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
