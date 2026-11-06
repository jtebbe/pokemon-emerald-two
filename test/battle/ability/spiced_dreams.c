#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Spiced Dreams puts Galarian Slowpoke and Slowbro to sleep on entry while allowing attacks and status moves")
{
    enum Species species;
    enum Move move;
    PARAMETRIZE { species = SPECIES_SLOWPOKE_GALAR; move = MOVE_WATER_GUN; }
    PARAMETRIZE { species = SPECIES_SLOWBRO_GALAR; move = MOVE_CALM_MIND; }
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPICED_DREAMS);
        if (species == SPECIES_SLOWPOKE_GALAR) MESSAGE("Slowpoke fell asleep!");
        else MESSAGE("Slowbro fell asleep!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(0), ABILITY_SPICED_DREAMS);
        EXPECT(player->status1 & STATUS1_SLEEP);
        if (move == MOVE_CALM_MIND)
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        else
            EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams boosts physical and special move power only while asleep", s16 damage)
{
    bool32 asleep;
    enum Move move;
    PARAMETRIZE { asleep = FALSE; move = MOVE_STRENGTH; }
    PARAMETRIZE { asleep = TRUE; move = MOVE_STRENGTH; }
    PARAMETRIZE { asleep = FALSE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { asleep = TRUE; move = MOVE_WATER_GUN; }
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Attack(200); SpAttack(200); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(asleep ? ABILITY_SHADOW_TAG : ABILITY_ELECTRIC_SURGE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(100); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams uses normal sleep duration and loses its bonuses when it wakes", s16 first; s16 last)
{
    PARAMETRIZE {}
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        for (u32 turn = 0; turn < 4; turn++)
            TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        HP_BAR(opponent);
        HP_BAR(opponent);
        MESSAGE("Slowbro woke up!");
        HP_BAR(opponent, captureDamage: &results[i].last);
        NOT MESSAGE("Slowbro is fast asleep.");
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_GT(results[i].first, results[i].last);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams adds a 50 percent burn secondary effect while asleep")
{
    PASSES_RANDOMLY(1, 2, RNG_SPICED_DREAMS);
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams burn respects Fire immunity Shield Dust Covert Cloak and existing status")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    u32 status;
    PARAMETRIZE { species = SPECIES_CHARIZARD; ability = ABILITY_BLAZE; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHADOW_TAG; item = ITEM_COVERT_CLOAK; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHADOW_TAG; item = ITEM_NONE; status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); Status1(status); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams does not burn through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams respects sleep prevention and cannot burn while awake")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_ELECTRIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_MISTY_SURGE; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, TRUE)); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Spiced Dreams is blocked by an ally's Sweet Veil")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SWEET_VEIL); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams does not replace an existing status")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(player->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams permits all hits of a multi-hit move while asleep")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Spiced Dreams can attack after falling asleep through Rest")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_SLEEP_TURN(1)); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_REST); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    } SCENE {
        HP_BAR(player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(player->status1 & STATUS1_SLEEP);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Spiced Dreams self-inflicted entry sleep does not occupy Sleep Clause")
{
    GIVEN {
        FLAG_SET(B_FLAG_SLEEP_CLAUSE);
        PLAYER(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HYPNOSIS, target: playerRight); }
    } THEN {
        EXPECT(playerLeft->status1 & STATUS1_SLEEP);
        EXPECT(playerRight->status1 & STATUS1_SLEEP);
    }
}
