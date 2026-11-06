#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Leek Master is exclusive to Kantonian Farfetch'd with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_FARFETCHD; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_FARFETCHD; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_FARFETCHD_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SIRFETCHD; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(0) == ABILITY_LEEK_MASTER, species == SPECIES_FARFETCHD && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Leek Master doubles physical Attack exactly like Huge Power and leaves special Attack unchanged", s16 damage)
{
    enum Item item = ITEM_NONE;
    enum Ability ability = ABILITY_NONE;
    u32 attack = 100, status = STATUS1_NONE;
    enum Move move = MOVE_SCRATCH;
    const enum Move moves[] = { MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_FOUL_PLAY, MOVE_BODY_PRESS };
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        for (u32 k = 0; k < 2; k++) {
            PARAMETRIZE { move = moves[j]; status = k ? STATUS1_BURN : STATUS1_NONE; item = ITEM_NONE; ability = ABILITY_HUGE_POWER; attack = 100; }
            PARAMETRIZE { move = moves[j]; status = k ? STATUS1_BURN : STATUS1_NONE; item = ITEM_APEX_METEORITE; ability = ABILITY_KEEN_EYE; attack = 100; }
        }
    }
    PARAMETRIZE { move = MOVE_SCRATCH; status = STATUS1_NONE; item = ITEM_NONE; ability = ABILITY_NONE; attack = 200; }
    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { Ability(ability); Item(item); Attack(attack); SpAttack(100); Defense(100); HP(1000); MaxHP(1000); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Attack(100); Defense(100); SpDefense(100); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        NOT MESSAGE("A critical hit!");
    } FINALLY {
        for (u32 j = 0; j < 8; j++) EXPECT_EQ(results[j * 2].damage, results[j * 2 + 1].damage);
        EXPECT_EQ(results[0].damage, results[16].damage);
    }
}

SINGLE_BATTLE_TEST("Leek Master guarantees physical and special critical hits in every generation")
{
    u32 gen = GEN_9;
    enum Move move = MOVE_SCRATCH;
    for (u32 j = GEN_1; j <= GEN_9; j++) {
        PARAMETRIZE { gen = j; move = MOVE_SCRATCH; }
        PARAMETRIZE { gen = j; move = MOVE_WATER_GUN; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Leek Master respects critical protection including Battle King and Lucky Chant")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Ability ability = ABILITY_NONE;
    bool32 chant = FALSE;
    enum Item item = ITEM_NONE;
    u32 gen = GEN_9;
    for (u32 j = GEN_1; j <= GEN_9; j++) {
        PARAMETRIZE { gen = j; species = SPECIES_WOBBUFFET; ability = ABILITY_BATTLE_ARMOR; item = ITEM_NONE; chant = FALSE; }
        PARAMETRIZE { gen = j; species = SPECIES_WOBBUFFET; ability = ABILITY_SHELL_ARMOR; item = ITEM_NONE; chant = FALSE; }
        PARAMETRIZE { gen = j; species = SPECIES_NIDOKING; ability = ABILITY_NONE; item = ITEM_APEX_METEORITE; chant = FALSE; }
        PARAMETRIZE { gen = j; species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_NONE; chant = TRUE; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(species) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { if (chant) MOVE(opponent, MOVE_LUCKY_CHANT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Leek Master does not bypass accuracy, immunity, or Protect")
{
    enum Move move;
    bool32 protect;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; protect = FALSE; }
    PARAMETRIZE { move = MOVE_THUNDERBOLT; protect = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SANDSHREW);
    } WHEN {
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, move, hit: move != MOVE_HYDRO_PUMP); }
    } SCENE { NOT HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Leek Master does not multiply fixed damage or make it critical")
{
    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); Level(60); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_SEISMIC_TOSS); } } SCENE {
        HP_BAR(opponent, damage: 60);
        NOT MESSAGE("A critical hit!");
    }
}
