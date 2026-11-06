#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Rainbow Rider sets Misty Terrain for the Galarian Ponyta line")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_PONYTA_GALAR; }
    PARAMETRIZE { species = SPECIES_RAPIDASH_GALAR; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RAINBOW_RIDER);
        MESSAGE("Mist swirled around the battlefield!");
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_RAINBOW_RIDER);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_MISTY);
        EXPECT_EQ(gFieldTimers.terrainTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider reduces physical and special attack damage by 33 percent", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SWIFT; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH_GALAR) { Item(item); Ability(ABILITY_RUN_AWAY); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); SpAttack(300); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.67), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.67), results[3].damage);
    }
}

DOUBLE_BATTLE_TEST("Rainbow Rider shields airborne allies against single and spread attacks", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SWIFT; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_PONYTA_GALAR) { Item(item); Ability(ABILITY_RUN_AWAY); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEVITATE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, move, target: playerRight); }
    } SCENE {
        if (move == MOVE_SWIFT) HP_BAR(playerLeft);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.67), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.67), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider's shield stops when Misty Terrain is overwritten or expires", s16 before; s16 after)
{
    bool32 expire;
    PARAMETRIZE { expire = FALSE; }
    PARAMETRIZE { expire = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH_GALAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        if (expire) {
            TURN {} TURN {} TURN {} TURN {}
        } else {
            TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].before);
        if (!expire) HP_BAR(player); // Grassy Terrain healing.
        HP_BAR(player, captureDamage: &results[i].after);
    } THEN {
        EXPECT_MUL_EQ(results[i].after, UQ_4_12(0.67), results[i].before);
        EXPECT_NE((u32)gFieldTimers.terrain, B_TERRAIN_MISTY);
    }
}

DOUBLE_BATTLE_TEST("Rainbow Rider's ally protection ends when the holder leaves or faints", s16 before; s16 after)
{
    bool32 faint;
    PARAMETRIZE { faint = FALSE; }
    PARAMETRIZE { faint = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PONYTA_GALAR) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
        if (faint) {
            TURN { MOVE(playerLeft, MOVE_MEMENTO, target: opponentRight); SEND_OUT(playerLeft, 2); }
        } else {
            TURN { SWITCH(playerLeft, 2); }
        }
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &results[i].before);
        HP_BAR(playerRight, captureDamage: &results[i].after);
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_MISTY);
        EXPECT_MUL_EQ(results[i].after, UQ_4_12(0.67), results[i].before);
    }
}

DOUBLE_BATTLE_TEST("Rainbow Rider applies once with two holders and stacks with Friend Guard", s16 damage)
{
    enum Ability partnerAbility;
    PARAMETRIZE { partnerAbility = ABILITY_NONE; }
    PARAMETRIZE { partnerAbility = ABILITY_RAINBOW_RIDER; }
    PARAMETRIZE { partnerAbility = ABILITY_FRIEND_GUARD; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH_GALAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(partnerAbility); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.75), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider stacks with Misty Terrain's ordinary Dragon damage reduction", s16 damage)
{
    bool32 rider;
    PARAMETRIZE { rider = FALSE; }
    PARAMETRIZE { rider = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(rider ? ABILITY_RAINBOW_RIDER : ABILITY_MISTY_SURGE); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_PULSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.67), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider protects through Mold Breaker and critical hits", s16 damage)
{
    bool32 rider;
    PARAMETRIZE { rider = FALSE; }
    PARAMETRIZE { rider = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH_GALAR) { Item(rider ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_RUN_AWAY); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); Attack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH, criticalHit: TRUE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.67), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider does not reduce residual poison damage")
{
    GIVEN {
        PLAYER(SPECIES_PONYTA_GALAR) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_POISON); HP(160); MaxHP(160); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(player, damage: 20);
    } THEN {
        EXPECT_EQ(player->hp, 140);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider restores Misty Terrain on reentry and supports Terrain Extender")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_RAINBOW_RIDER); Item(ITEM_TERRAIN_EXTENDER); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_MISTY);
        EXPECT_EQ(gFieldTimers.terrainTimer, 7);
    }
}

SINGLE_BATTLE_TEST("Rainbow Rider cannot be suppressed by Neutralizing Gas", s16 damage)
{
    bool32 rider;
    PARAMETRIZE { rider = FALSE; }
    PARAMETRIZE { rider = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PONYTA_GALAR) { Item(rider ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_RUN_AWAY); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Attack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.67), results[1].damage);
    }
}
