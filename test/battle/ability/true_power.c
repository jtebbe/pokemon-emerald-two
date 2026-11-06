#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("True Power is the Apex ability of the Machop line")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_MACHOP; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MACHOKE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MACHAMP; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MACHAMP_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MACHAMP; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_TRUE_POWER, item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("True Power doubles physical Attack like Huge Power", s16 damage)
{
    enum Item item;
    enum Ability ability;
    u32 attack;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; attack = 200; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_HUGE_POWER; attack = 100; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(item); Ability(ability); Attack(attack); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("True Power stacks Huge Power and Guts to triple Attack and ignores the burn penalty", s16 damage)
{
    enum Item item;
    enum Ability ability;
    u32 attack, status;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; attack = 300; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_GUTS; attack = 200; status = STATUS1_BURN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; status = STATUS1_BURN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; status = STATUS1_POISON; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_GUTS; attack = 100; status = STATUS1_FROSTBITE; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(item); Ability(ability); Attack(attack); Status1(status); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 1; j < 7; j++) EXPECT_EQ(results[0].damage, results[j].damage);
    }
}

SINGLE_BATTLE_TEST("True Power does not boost special attacks", s16 damage)
{
    enum Item item;
    u32 status;
    PARAMETRIZE { item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_BURN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_POISON; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(item); Ability(ABILITY_NONE); SpAttack(100); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 1; j < 4; j++) EXPECT_EQ(results[0].damage, results[j].damage);
    }
}

SINGLE_BATTLE_TEST("True Power does not remove frostbite's special damage penalty", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(item); Ability(ABILITY_NONE); Status1(STATUS1_FROSTBITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("True Power does not multiply fixed damage")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_BURN; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
    }
}

SINGLE_BATTLE_TEST("True Power follows Huge Power and Guts when Body Press uses Defense", s16 damage)
{
    enum Item item;
    enum Ability ability;
    u32 defense, status;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; defense = 300; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_GUTS; defense = 200; status = STATUS1_BURN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; defense = 100; status = STATUS1_BURN; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(item); Ability(ability); Attack(10); Defense(defense); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_BODY_PRESS, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("True Power blocks Fake Out and secondary flinches even through Mold Breaker")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_FAKE_OUT; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_ROCK_SLIDE; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_FAKE_OUT; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { move = MOVE_ROCK_SLIDE; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, move, secondaryEffect: TRUE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        NOT MESSAGE("Machamp flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("True Power prevents King's Rock and Razor Fang flinches")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_KINGS_ROCK; }
    PARAMETRIZE { item = ITEM_RAZOR_FANG; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_HOLD_EFFECT_FLINCH, TRUE)); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("True Power follows Inner Focus's generation-dependent Intimidate protection")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_7; }
    PARAMETRIZE { generation = GEN_8; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, generation);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], generation >= GEN_8 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("True Power's Inner Focus also blocks Guardian's Growl in modern generations")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_7; }
    PARAMETRIZE { generation = GEN_8; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, generation);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], generation >= GEN_8 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], generation >= GEN_8 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("True Power does not protect against ordinary Attack drops or self-inflicted drops")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_SUPERPOWER; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (move == MOVE_GROWL) TURN { MOVE(opponent, MOVE_GROWL); }
        else TURN { MOVE(player, MOVE_SUPERPOWER); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], move == MOVE_SUPERPOWER ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("True Power responds to gaining and curing status during battle", s16 clean, s16 burned, s16 cured)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); Attack(100); HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
        TURN { MOVE(opponent, MOVE_WILL_O_WISP); MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
        TURN { MOVE(player, MOVE_REFRESH); }
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].clean);
        STATUS_ICON(player, burn: TRUE);
        HP_BAR(opponent, captureDamage: &results[i].burned);
        STATUS_ICON(player, burn: FALSE);
        HP_BAR(opponent, captureDamage: &results[i].cured);
    } THEN {
        EXPECT_GT(results[i].burned, results[i].clean);
        EXPECT_EQ(results[i].clean, results[i].cured);
    }
}

SINGLE_BATTLE_TEST("True Power does not prevent burn's residual damage")
{
    GIVEN {
        WITH_CONFIG(B_BURN_DAMAGE, GEN_7);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_BURN); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(player, damage: 50);
    } THEN {
        EXPECT_EQ(player->hp, 750);
    }
}

SINGLE_BATTLE_TEST("True Power's Guts boost also applies to physical attacks called by Sleep Talk", s16 damage)
{
    bool32 sleeping;
    PARAMETRIZE { sleeping = FALSE; }
    PARAMETRIZE { sleeping = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) {
            Item(sleeping ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_NONE);
            Attack(sleeping ? 100 : 300); Status1(sleeping ? STATUS1_SLEEP_TURN(3) : STATUS1_NONE);
            Moves(MOVE_TACKLE, MOVE_SLEEP_TALK);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, sleeping ? MOVE_SLEEP_TALK : MOVE_TACKLE, criticalHit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
