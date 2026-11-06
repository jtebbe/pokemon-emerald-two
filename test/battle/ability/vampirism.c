#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Vampirism is the Apex ability of Zubat, Golbat, and Crobat")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_ZUBAT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLBAT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CROBAT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CROBAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_INNER_FOCUS); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_VAMPIRISM, item == ITEM_APEX_METEORITE && species != SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Vampirism heals one third of physical, special, and fixed damage")
{
    enum Move move;
    s16 damage, recovery;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_SEISMIC_TOSS; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(300); MaxHP(400); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Vampirism heals one half strictly below 50 percent HP, including odd maximum HP")
{
    u32 hp, maxHP, divisor;
    PARAMETRIZE { hp = 99; maxHP = 200; divisor = 2; }
    PARAMETRIZE { hp = 100; maxHP = 200; divisor = 3; }
    PARAMETRIZE { hp = 101; maxHP = 200; divisor = 3; }
    PARAMETRIZE { hp = 100; maxHP = 201; divisor = 2; }
    PARAMETRIZE { hp = 101; maxHP = 201; divisor = 3; }
    GIVEN {
        PLAYER(SPECIES_ZUBAT) { Item(ITEM_APEX_METEORITE); Level(60); HP(hp); MaxHP(maxHP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent, damage: 60);
        HP_BAR(player, damage: -(60 / divisor));
    }
}

SINGLE_BATTLE_TEST("Vampirism rounds recovery down with a minimum of one HP")
{
    u32 level;
    PARAMETRIZE { level = 1; }
    PARAMETRIZE { level = 2; }
    PARAMETRIZE { level = 5; }
    GIVEN {
        PLAYER(SPECIES_ZUBAT) { Item(ITEM_APEX_METEORITE); Level(level); HP(100); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent, damage: level);
        HP_BAR(player, damage: -1);
    }
}

SINGLE_BATTLE_TEST("Vampirism uses actual damage on a knockout rather than overkill damage")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Level(100); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(9); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); SEND_OUT(opponent, 1); }
    } SCENE {
        HP_BAR(opponent, damage: 9);
        HP_BAR(player, damage: -4);
    }
}

SINGLE_BATTLE_TEST("Vampirism caps recovery at maximum HP and does not activate at full HP")
{
    u32 hp;
    PARAMETRIZE { hp = 399; }
    PARAMETRIZE { hp = 400; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent);
        if (hp == 399) {
            ABILITY_POPUP(player, ABILITY_VAMPIRISM);
            HP_BAR(player, hp: 400);
        } else {
            NONE_OF { ABILITY_POPUP(player, ABILITY_VAMPIRISM); HP_BAR(player); }
        }
    }
}

SINGLE_BATTLE_TEST("Vampirism heals from actual Substitute damage without counting overkill")
{
    u32 level;
    PARAMETRIZE { level = 60; }
    PARAMETRIZE { level = 100; }
    GIVEN {
        PLAYER(SPECIES_ZUBAT) { Item(ITEM_APEX_METEORITE); Level(level); HP(1); MaxHP(400); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(240); MaxHP(240); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, player);
        MESSAGE("The opposing Wobbuffet's substitute faded!");
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, damage: -30);
    }
}

SINGLE_BATTLE_TEST("Vampirism heals once from the sum of all hits of a multi-hit move")
{
    s16 damage[5], recovery;
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        for (u32 j = 0; j < 5; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
            HP_BAR(opponent, captureDamage: &damage[j]);
        }
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, captureDamage: &recovery);
        NOT ABILITY_POPUP(player, ABILITY_VAMPIRISM);
    } THEN {
        s32 total = 0;
        for (u32 j = 0; j < 5; j++) total += damage[j];
        EXPECT_EQ(-recovery, total / 2);
    }
}

DOUBLE_BATTLE_TEST("Vampirism includes every damaged target of a spread move in one recovery")
{
    s16 damage[3], recovery;
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &damage[1]);
        HP_BAR(playerRight, captureDamage: &damage[0]);
        HP_BAR(opponentRight, captureDamage: &damage[2]);
        ABILITY_POPUP(playerLeft, ABILITY_VAMPIRISM);
        HP_BAR(playerLeft, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, (damage[0] + damage[1] + damage[2]) / 2);
    }
}

SINGLE_BATTLE_TEST("Vampirism stacks with a draining move and checks HP after its normal recovery")
{
    s16 damage, drain, recovery;
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(199); MaxHP(400); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &drain);
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_GE(-drain, 1);
        EXPECT_EQ(-recovery, damage / 3);
    }
}

SINGLE_BATTLE_TEST("Vampirism recovery is unaffected by Liquid Ooze")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Level(60); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_TENTACRUEL) { Ability(ABILITY_LIQUID_OOZE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent, damage: 60);
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, damage: -30);
    }
}

SINGLE_BATTLE_TEST("Vampirism does not heal through Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF { ABILITY_POPUP(player, ABILITY_VAMPIRISM); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Vampirism does not heal from missed, protected, immune, or zero damage attacks")
{
    enum Move move;
    enum Species species;
    bool32 protect;
    PARAMETRIZE { move = MOVE_THUNDER; species = SPECIES_WOBBUFFET; protect = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; species = SPECIES_WOBBUFFET; protect = TRUE; }
    PARAMETRIZE { move = MOVE_SCRATCH; species = SPECIES_GASTLY; protect = FALSE; }
    PARAMETRIZE { move = MOVE_FALSE_SWIPE; species = SPECIES_WOBBUFFET; protect = FALSE; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        OPPONENT(species) { HP(1); }
    } WHEN {
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, move, hit: move != MOVE_THUNDER); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_VAMPIRISM); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Vampirism does not heal from poison or Leech Seed damage")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_LEECH_SEED; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_VAMPIRISM);
    } THEN {
        EXPECT_EQ(player->hp, move == MOVE_LEECH_SEED ? 100 + opponent->maxHP / 8 : 100);
    }
}

SINGLE_BATTLE_TEST("Vampirism never revives a user fainted by a contact punishment")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        PLAYER(SPECIES_ZUBAT);
        OPPONENT(SPECIES_GARCHOMP) { Ability(ABILITY_ROUGH_SKIN); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player, hp: 0);
        MESSAGE("Crobat fainted!");
        NOT ABILITY_POPUP(player, ABILITY_VAMPIRISM);
    }
}

SINGLE_BATTLE_TEST("Vampirism prevents ordinary, maximum-HP, and Struggle recoil")
{
    enum Move move = MOVE_DOUBLE_EDGE;
    const enum Move moves[] = {MOVE_DOUBLE_EDGE, MOVE_BRAVE_BIRD, MOVE_HEAD_SMASH, MOVE_FLARE_BLITZ, MOVE_CHLOROBLAST, MOVE_STEEL_BEAM, MOVE_MIND_BLOWN, MOVE_STRUGGLE};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) PARAMETRIZE { move = moves[j]; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Vampirism prevents crash damage on a miss, Protect, or type immunity")
{
    enum Move move;
    enum Species species;
    bool32 protect;
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; species = SPECIES_WOBBUFFET; protect = FALSE; }
    PARAMETRIZE { move = MOVE_AXE_KICK; species = SPECIES_WOBBUFFET; protect = FALSE; }
    PARAMETRIZE { move = MOVE_SUPERCELL_SLAM; species = SPECIES_WOBBUFFET; protect = FALSE; }
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; species = SPECIES_WOBBUFFET; protect = TRUE; }
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; species = SPECIES_GASTLY; protect = FALSE; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, move, hit: FALSE); }
    } SCENE {
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Vampirism prevents recoil even when the hit does not deal damage")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_STEEL_BEAM; }
    PARAMETRIZE { move = MOVE_MIND_BLOWN; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, move); }
    } SCENE {
        NOT HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Vampirism does not prevent direct HP costs or self-destruction")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_STORM_SACRIFICE; }
    PARAMETRIZE { move = MOVE_SUBSTITUTE; }
    PARAMETRIZE { move = MOVE_EXPLOSION; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_ZUBAT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); if (move == MOVE_EXPLOSION) SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player, damage: move == MOVE_EXPLOSION ? 400 : 100);
        NOT ABILITY_POPUP(player, ABILITY_VAMPIRISM);
    }
}

SINGLE_BATTLE_TEST("Vampirism deals 30 percent more physical and special damage only to injured targets", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    u32 hp = 3000;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; hp = 3000; }
        PARAMETRIZE { move = moves[j]; hp = 2999; }
    }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Vampirism checks target HP again between hits of a multi-hit move", s16 damage[2])
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(item); Attack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage[0]);
        HP_BAR(opponent, captureDamage: &results[i].damage[1]);
    } FINALLY {
        EXPECT_EQ(results[0].damage[0], results[1].damage[0]);
        EXPECT_MUL_EQ(results[0].damage[1], UQ_4_12(1.3), results[1].damage[1]);
    }
}

SINGLE_BATTLE_TEST("Vampirism does not multiply fixed damage against an injured target")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Level(60); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2999); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent, damage: 60);
    }
}

SINGLE_BATTLE_TEST("Vampirism heals from Future Sight while its user remains active")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Vampirism cannot heal a replacement from an absent user's Future Sight")
{
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        PLAYER(SPECIES_ZUBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE {
        HP_BAR(opponent);
        NONE_OF { ABILITY_POPUP(player, ABILITY_VAMPIRISM); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Vampirism heals after contact punishment and checks its updated HP threshold")
{
    s16 damage, recovery;
    GIVEN {
        PLAYER(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(201); MaxHP(400); Attack(100); }
        OPPONENT(SPECIES_GARCHOMP) { Ability(ABILITY_ROUGH_SKIN); HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
        HP_BAR(player, damage: 50);
        ABILITY_POPUP(player, ABILITY_VAMPIRISM);
        HP_BAR(player, captureDamage: &recovery);
    } THEN {
        EXPECT_EQ(-recovery, damage / 2);
    }
}
