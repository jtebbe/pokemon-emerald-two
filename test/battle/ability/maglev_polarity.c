#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Maglev Polarity is the Magnemite line's Apex ability")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_MAGNEMITE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MAGNETON; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MAGNEZONE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MAGNEZONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_MAGLEV_POLARITY,
                  item == ITEM_APEX_METEORITE && species != SPECIES_WOBBUFFET);
        if (species != SPECIES_WOBBUFFET)
            EXPECT_EQ(CanMonParticipateInSkyBattle(&gParties[B_TRAINER_PLAYER][0]), item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity doubles physical and special damage once against either qualifying type", s16 damage)
{
    enum Item item = ITEM_NONE;
    enum Species species = SPECIES_PIKACHU;
    enum Move move = MOVE_SCRATCH;
    const enum Species speciesList[] = { SPECIES_PIKACHU, SPECIES_REGISTEEL, SPECIES_MAGNEZONE, SPECIES_WOBBUFFET };
    const enum Move moves[] = { MOVE_SCRATCH, MOVE_WATER_GUN };
    for (u32 j = 0; j < ARRAY_COUNT(speciesList); j++) {
        for (u32 k = 0; k < ARRAY_COUNT(moves); k++) {
            PARAMETRIZE { species = speciesList[j]; move = moves[k]; item = ITEM_NONE; }
            PARAMETRIZE { species = speciesList[j]; move = moves[k]; item = ITEM_APEX_METEORITE; }
        }
    }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(item); Attack(200); SpAttack(200); }
        OPPONENT(species) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < 8; j++)
            EXPECT_MUL_EQ(results[j * 2].damage, j < 6 ? UQ_4_12(2.0) : UQ_4_12(1.0), results[j * 2 + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity heals one third of actual boosted damage even below half HP")
{
    enum Move move;
    u32 hp;
    PARAMETRIZE { move = MOVE_SCRATCH; hp = 1; }
    PARAMETRIZE { move = MOVE_SCRATCH; hp = 600; }
    PARAMETRIZE { move = MOVE_WATER_GUN; hp = 1; }
    PARAMETRIZE { move = MOVE_WATER_GUN; hp = 600; }
    s16 damage, recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNETON) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(1000); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity follows the target's current Tera type for damage and healing", s16 damage)
{
    enum Item item;
    enum Species species = SPECIES_PIKACHU;
    enum Type type = TYPE_NORMAL;
    PARAMETRIZE { item = ITEM_NONE; species = SPECIES_PIKACHU; type = TYPE_NORMAL; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; species = SPECIES_PIKACHU; type = TYPE_NORMAL; }
    PARAMETRIZE { item = ITEM_NONE; species = SPECIES_WOBBUFFET; type = TYPE_ELECTRIC; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; species = SPECIES_WOBBUFFET; type = TYPE_ELECTRIC; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(item); HP(1); MaxHP(1000); }
        OPPONENT(species) { Ability(ABILITY_NONE); TeraType(type); HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_TERA); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (item == ITEM_APEX_METEORITE && type == TYPE_ELECTRIC) {
            ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
            HP_BAR(player);
        } else {
            NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity grants Ground immunity including against Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        MESSAGE("It doesn't affect Magnezone…");
        NOT HP_BAR(player);
    } THEN {
        EXPECT(!IsBattlerGrounded(B_BATTLER_0, ABILITY_MAGLEV_POLARITY, GetBattlerHoldEffect(B_BATTLER_0)));
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity is grounded by Gravity and Smack Down")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GRAVITY; }
    PARAMETRIZE { move = MOVE_SMACK_DOWN; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
    } THEN {
        EXPECT(IsBattlerGrounded(B_BATTLER_0, ABILITY_MAGLEV_POLARITY, GetBattlerHoldEffect(B_BATTLER_0)));
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity avoids Spikes and Sticky Web on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MAGNEMITE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        NONE_OF { HP_BAR(player); ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(!IsBattlerGrounded(B_BATTLER_0, ABILITY_MAGLEV_POLARITY, GetBattlerHoldEffect(B_BATTLER_0)));
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity uses actual damage rather than overkill and rounds recovery to at least one HP")
{
    u32 hp;
    PARAMETRIZE { hp = 1; }
    PARAMETRIZE { hp = 9; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(hp); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); SEND_OUT(opponent, 1); }
    } SCENE {
        HP_BAR(opponent, damage: hp);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, damage: -max(1, hp / 3));
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity caps healing at maximum HP and does not activate at full HP")
{
    u32 hp;
    PARAMETRIZE { hp = 399; }
    PARAMETRIZE { hp = 400; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(400); }
        OPPONENT(SPECIES_PIKACHU) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent);
        if (hp == 399) {
            ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
            HP_BAR(player, hp: 400);
        } else {
            NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
        }
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity heals from actual Substitute damage")
{
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Speed(1); SpAttack(500); }
        OPPONENT(SPECIES_PIKACHU) { HP(240); MaxHP(240); Speed(100); SpDefense(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        MESSAGE("The opposing Pikachu's substitute faded!");
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, damage: -20);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity accumulates all qualifying hits of a multi-hit move")
{
    s16 damage[2], recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT); }
    } SCENE {
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_HIT, player);
            HP_BAR(opponent, captureDamage: &damage[j]);
        }
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
        NOT ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
    } THEN {
        EXPECT_EQ(-recovery, (damage[0] + damage[1]) / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity records damage before Color Change and checks the new type on the next move", s16 first; s16 second)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    s16 recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(item); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_COLOR_CHANGE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        if (item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
            HP_BAR(player, captureDamage: &recovery);
        }
        HP_BAR(opponent, captureDamage: &results[i].second);
        NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
    } THEN {
        if (item == ITEM_APEX_METEORITE)
            EXPECT_EQ(-recovery, results[i].first / 3);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].first, UQ_4_12(2.0), results[1].first);
        EXPECT_EQ(results[0].second, results[1].second);
    }
}

DOUBLE_BATTLE_TEST("Maglev Polarity heals only from qualifying recipients of a spread move")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_REGISTEEL; }
    s16 damage[2], recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_PIKACHU) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(species) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &damage[0]);
        HP_BAR(playerRight);
        HP_BAR(opponentRight, captureDamage: &damage[1]);
        ABILITY_POPUP(playerLeft, ABILITY_MAGLEV_POLARITY);
        HP_BAR(playerLeft, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, (damage[0] + (species == SPECIES_REGISTEEL ? damage[1] : 0)) / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity does not heal from nonqualifying targets, misses, Protect, or immunities")
{
    enum Species species;
    enum Move move;
    bool32 protect, hit;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; move = MOVE_WATER_GUN; protect = FALSE; hit = TRUE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; move = MOVE_THUNDER; protect = FALSE; hit = FALSE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; move = MOVE_WATER_GUN; protect = TRUE; hit = TRUE; }
    PARAMETRIZE { species = SPECIES_STUNFISK; move = MOVE_THUNDER; protect = FALSE; hit = TRUE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; move = MOVE_TOXIC; protect = FALSE; hit = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        OPPONENT(species) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, move, hit: hit); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity does not heal through Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Speed(1); }
        OPPONENT(SPECIES_PIKACHU) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity recovery is unaffected by Liquid Ooze")
{
    s16 damage, recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_LIQUID_OOZE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity never revives a user KO'd by contact punishment")
{
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_ROUGH_SKIN); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player, hp: 0);
        NOT ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity still takes move recoil before healing")
{
    s16 damage, recoil, recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &recoil);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_GT(recoil, 0);
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity clears its recovery accumulator between attacks")
{
    s16 damage, recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_PIKACHU) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
        HP_BAR(opponent);
        NONE_OF { ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Maglev Polarity recovery stacks with a draining move")
{
    s16 damage, drain, recovery;
    GIVEN {
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &drain);
        ABILITY_POPUP(player, ABILITY_MAGLEV_POLARITY);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-drain, (damage + 1) / 2);
        EXPECT_EQ(-recovery, damage / 3);
    }
}
