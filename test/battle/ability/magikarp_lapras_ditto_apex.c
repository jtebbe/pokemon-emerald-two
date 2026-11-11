#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sacrificial Carp raises the five core stats of Magikarp's replacement")
{
    bool32 apex;
    PARAMETRIZE { apex = TRUE; }
    PARAMETRIZE { apex = FALSE; }
    GIVEN {
        PLAYER(SPECIES_MAGIKARP) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + apex);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT(!gBattleStruct->battlerState[B_BATTLER_0].storedSacrificialCarp);
    }
}

DOUBLE_BATTLE_TEST("Mindless Rampage intimidates both foes and spreads attacks without hitting its ally")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DRAGON_BREATH; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Item(ITEM_APEX_METEORITE); Moves(move, MOVE_DRAGON_DANCE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, move, target: opponentLeft, secondaryEffect: FALSE); }
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(CheckMoveLimitations(B_BATTLER_0, 0, MOVE_LIMITATION_TAUNT), 1u << 1);
    }
}

SINGLE_BATTLE_TEST("Mindless Rampage grants Dragon STAB and ignores defensive stat changes", s16 damage)
{
    enum Move setup;
    PARAMETRIZE { setup = MOVE_CELEBRATE; }
    PARAMETRIZE { setup = MOVE_IRON_DEFENSE; }
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, setup); MOVE(player, MOVE_DRAGON_CLAW, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        struct DamageContext ctx = {
            .battlerAtk = B_BATTLER_0,
            .move = MOVE_DRAGON_CLAW,
            .moveType = TYPE_DRAGON,
            .abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0),
        };
        EXPECT_EQ(GetSameTypeAttackBonusModifier(&ctx), UQ_4_12(1.5));
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Sea Guardian redirects and absorbs Water moves aimed at its ally")
{
    GIVEN {
        WITH_CONFIG(B_REDIRECT_ABILITY_IMMUNITY, GEN_5);
        PLAYER(SPECIES_LAPRAS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WATER_GUN, target: playerRight); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Sea Guardian protects Lapras from critical hits and reduces damage to its ally", s16 allyDamage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_LAPRAS) { Ability(ABILITY_WATER_ABSORB); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_STORM_THROW, target: playerLeft); }
        TURN { MOVE(opponentRight, MOVE_TACKLE, target: playerRight, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(playerLeft);
        if (apex)
            NOT MESSAGE("A critical hit!");
        else
            MESSAGE("A critical hit!");
        HP_BAR(playerRight, captureDamage: &results[i].allyDamage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].allyDamage, Q_4_12(0.75), results[1].allyDamage);
    }
}

SINGLE_BATTLE_TEST("True Adaptation restores Ditto's HP and adds two stages to the copied core stats")
{
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_IMPOSTER); Item(ITEM_APEX_METEORITE); HP(80); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_TRANSFORM); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(player->hp, 400);
        EXPECT_EQ(player->maxHP, 400);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TRUE_ADAPTATION);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2 + (stat == STAT_ATK ? 2 : 0));
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("True Adaptation does not heal or boost Ditto when it transforms into an ally")
{
    GIVEN {
        PLAYER(SPECIES_DITTO) { Item(ITEM_APEX_METEORITE); HP(80); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TRANSFORM, target: playerRight); }
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(playerLeft->hp, 80);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE);
    }
}
