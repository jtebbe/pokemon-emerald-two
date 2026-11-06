#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Earthen Mother boosts physical and special Ground moves by 50 percent in the whole line", s16 damage)
{
    enum Species species = SPECIES_NIDORAN_F;
    enum Move move = MOVE_SCRATCH;
    enum Item item = ITEM_NONE;
    const enum Species line[] = {SPECIES_NIDORAN_F, SPECIES_NIDORINA, SPECIES_NIDOQUEEN};
    for (u32 j = 0; j < ARRAY_COUNT(line); j++) {
        PARAMETRIZE { species = line[j]; move = MOVE_SCRATCH; item = ITEM_NONE; }
        PARAMETRIZE { species = line[j]; move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; }
        PARAMETRIZE { species = line[j]; move = MOVE_EARTHQUAKE; item = ITEM_NONE; }
        PARAMETRIZE { species = line[j]; move = MOVE_EARTHQUAKE; item = ITEM_APEX_METEORITE; }
        PARAMETRIZE { species = line[j]; move = MOVE_EARTH_POWER; item = ITEM_NONE; }
        PARAMETRIZE { species = line[j]; move = MOVE_EARTH_POWER; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_POISON_POINT); Item(item); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(line); j++) {
            EXPECT_EQ(results[6 * j].damage, results[6 * j + 1].damage);
            EXPECT_MUL_EQ(results[6 * j + 2].damage, UQ_4_12(1.5), results[6 * j + 3].damage);
            EXPECT_MUL_EQ(results[6 * j + 4].damage, UQ_4_12(1.5), results[6 * j + 5].damage);
        }
    }
}

SINGLE_BATTLE_TEST("Earthen Mother belongs only to the female Nidoran line with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_NIDORAN_F; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORINA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDOQUEEN; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORAN_F; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_NIDORAN_M; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORINO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDOKING; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SAND_ATTACK); }
    } SCENE {
        if (item == ITEM_APEX_METEORITE && (species == SPECIES_NIDORAN_F || species == SPECIES_NIDORINA || species == SPECIES_NIDOQUEEN)) {
            ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER);
            HP_BAR(player, damage: -250);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_EARTHEN_MOTHER,
                  item == ITEM_APEX_METEORITE && (species == SPECIES_NIDORAN_F || species == SPECIES_NIDORINA || species == SPECIES_NIDOQUEEN));
    }
}

SINGLE_BATTLE_TEST("Earthen Mother absorbs physical, special and status Ground moves and heals one quarter max HP")
{
    enum Move move;
    enum Ability ability;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { move = MOVE_EARTHQUAKE; ability = j ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_MUD_SLAP; ability = j ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_SAND_ATTACK; ability = j ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
    }
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER);
        HP_BAR(player, damage: -25);
        MESSAGE("Nidoqueen had its HP restored.");
    } THEN {
        EXPECT_EQ(player->hp, 26);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Earthen Mother still blocks Ground moves at full HP or under Heal Block")
{
    bool32 healBlocked;
    PARAMETRIZE { healBlocked = FALSE; }
    PARAMETRIZE { healBlocked = TRUE; }
    GIVEN {
        WITH_CONFIG(B_HEAL_BLOCKING, GEN_9);
        PLAYER(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); HP(healBlocked ? 1 : 100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (healBlocked) TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(opponent, MOVE_MUD_SLAP); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, healBlocked ? 1 : 100);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Earthen Mother does not absorb a move blocked by Protect")
{
    GIVEN {
        PLAYER(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Earthen Mother cannot be suppressed by Neutralizing Gas or Gastro Acid")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GASTRO_ACID); }
        TURN { MOVE(opponent, MOVE_EARTH_POWER); }
    } SCENE {
        MESSAGE("But it failed!");
        ABILITY_POPUP(player, ABILITY_EARTHEN_MOTHER);
        HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_EARTHEN_MOTHER);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother reduces physical and special damage to its ally by 25 percent", s16 damage)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(item); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, move, target: playerRight); }
    } SCENE {
        if (move == MOVE_SWIFT) HP_BAR(playerLeft);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.75), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.75), results[3].damage);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother protects its ally even against Mold Breaker and Neutralizing Gas", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { ability = ABILITY_NEUTRALIZING_GAS; }
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother does not reduce damage to its own holder", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(item); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother stops protecting its ally when the holder faints")
{
    s16 guardedDamage, unguardedDamage;
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); HP(40); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
        TURN { MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft); }
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &guardedDamage);
        HP_BAR(playerLeft, damage: 40);
        HP_BAR(playerRight, captureDamage: &unguardedDamage);
    } THEN {
        EXPECT_MUL_EQ(unguardedDamage, UQ_4_12(0.75), guardedDamage);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother absorbs an ally's Earthquake")
{
    GIVEN {
        PLAYER(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(100); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_EARTHQUAKE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_EARTHEN_MOTHER);
        HP_BAR(playerLeft, damage: -25);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 26);
    }
}

DOUBLE_BATTLE_TEST("Earthen Mother does not reduce fixed damage to its ally")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, damage: 40);
    }
}
