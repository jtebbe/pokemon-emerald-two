#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Peak Voltage is the Apex ability of Pichu, Pikachu and non-Alolan Raichu")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_PICHU; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PICHU_SPIKY_EARED; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIKACHU_COSPLAY; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIKACHU_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_RAICHU; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_RAICHU_MEGA_X; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_RAICHU_MEGA_Y; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_RAICHU_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
    } SCENE {
        if (species != SPECIES_RAICHU_ALOLA && item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
            MESSAGE("An electric current ran across the battlefield!");
        } else {
            NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_PEAK_VOLTAGE,
                  species != SPECIES_RAICHU_ALOLA && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage sets terrain before damage and boosts the triggering Electric attack", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_TERRAIN_TYPE_BOOST, GEN_8);
        PLAYER(SPECIES_RAICHU) { Ability(ABILITY_STATIC); Item(item); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        if (item == ITEM_APEX_METEORITE) {
            MESSAGE("Raichu used Thunderbolt!");
            ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
            MESSAGE("An electric current ran across the battlefield!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage sets terrain before an Electric move misses, is protected or meets a Ground immunity")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(variant == 2 ? SPECIES_SANDSHREW : SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            if (variant == 1) MOVE(opponent, MOVE_PROTECT);
            MOVE(player, MOVE_THUNDER, hit: variant != 0);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        MESSAGE("An electric current ran across the battlefield!");
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage works on Electric status moves and activates terrain seeds before the move")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ELECTRIC_SEED); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); }
    } SCENE {
        MESSAGE("Pikachu used Thunder Wave!");
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        MESSAGE("An electric current ran across the battlefield!");
        ITEM_POPUP(opponent, ITEM_ELECTRIC_SEED);
        MESSAGE("The opposing Wobbuffet's Defense rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, player);
        STATUS_ICON(opponent, paralysis: TRUE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage uses the effective move type for Revelation Dance")
{
    enum Type type;
    PARAMETRIZE { type = TYPE_ELECTRIC; }
    PARAMETRIZE { type = TYPE_FIRE; }
    GIVEN {
        PLAYER(type == TYPE_ELECTRIC ? SPECIES_RAICHU : SPECIES_GROWLITHE) { Ability(ABILITY_PEAK_VOLTAGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_REVELATION_DANCE); }
    } SCENE {
        if (type == TYPE_ELECTRIC) {
            ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
            MESSAGE("An electric current ran across the battlefield!");
        } else {
            NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        }
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, type == TYPE_ELECTRIC ? B_TERRAIN_ELECTRIC : B_TERRAIN_NONE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage replaces another terrain but does not refresh existing Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); }
    } SCENE {
        MESSAGE("Grass grew to cover the battlefield!");
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        MESSAGE("An electric current ran across the battlefield!");
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrainTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage does not set terrain on entry, on other move types, or when flinched")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_THUNDER_SHOCK); }
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
            MESSAGE("An electric current ran across the battlefield!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_NONE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage takes Electric damage then raises all five core stats by one stage")
{
    enum Ability attackerAbility;
    PARAMETRIZE { attackerAbility = ABILITY_NONE; }
    PARAMETRIZE { attackerAbility = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(attackerAbility); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER_SHOCK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        MESSAGE("Pikachu's Attack rose!");
        MESSAGE("Pikachu's Defense rose!");
        MESSAGE("Pikachu's Speed rose!");
        MESSAGE("Pikachu's Sp. Atk rose!");
        MESSAGE("Pikachu's Sp. Def rose!");
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_NONE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage does not boost after non-Electric damage, a protected hit, a miss, or a Substitute hit")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    PARAMETRIZE { variant = 3; }
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(1); Speed(1); }
    } WHEN {
        if (variant == 3) TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN {
            if (variant == 1) MOVE(player, MOVE_PROTECT);
            MOVE(opponent, variant == 0 ? MOVE_WATER_GUN : variant == 2 ? MOVE_THUNDER : MOVE_THUNDER_SHOCK, hit: variant != 2);
        }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage does not raise stats on a KO")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_THUNDER_SHOCK); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        MESSAGE("Pikachu fainted!");
    }
}

SINGLE_BATTLE_TEST("Peak Voltage boosts between successive Electric hits and caps each core stat at plus six")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(1); }
    } WHEN {
        for (u32 turn = 0; turn < 7; turn++)
            TURN { MOVE(opponent, MOVE_THUNDER_SHOCK); }
    } SCENE {
        for (u32 turn = 0; turn < 6; turn++) {
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        }
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Peak Voltage boosts after each multistrike hit using the effective Electric type")
{
    s16 first, second;
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_GALVANIZE); Attack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_HIT); }
    } SCENE {
        HP_BAR(player, captureDamage: &first);
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
        HP_BAR(player, captureDamage: &second);
        ABILITY_POPUP(player, ABILITY_PEAK_VOLTAGE);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2);
        EXPECT_GT(first, second);
    }
}

DOUBLE_BATTLE_TEST("Peak Voltage also boosts when hit by an ally's Electric move")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_THUNDER_SHOCK, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_PEAK_VOLTAGE);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}
