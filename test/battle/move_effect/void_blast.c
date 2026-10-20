#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveType(MOVE_VOID_BLAST) == TYPE_DARK);
    ASSUME(GetMoveCategory(MOVE_VOID_BLAST) == DAMAGE_CATEGORY_SPECIAL);
    ASSUME(GetMovePower(MOVE_VOID_BLAST) == 130);
    ASSUME(GetMoveAccuracy(MOVE_VOID_BLAST) == 90);
    ASSUME(GetMovePP(MOVE_VOID_BLAST) == 8); // 5 base PP, always PP Max'd.
    ASSUME(GetMoveTarget(MOVE_VOID_BLAST) == TARGET_SELECTED);
    ASSUME(IsBallisticMove(MOVE_VOID_BLAST));
    ASSUME(!MoveMakesContact(MOVE_VOID_BLAST));
}

SINGLE_BATTLE_TEST("Void Blast lowers all five core stats after damage, including a KO")
{
    bool32 knockout;
    PARAMETRIZE { knockout = FALSE; }
    PARAMETRIZE { knockout = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(knockout ? 1 : 1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VOID_BLAST, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Void Blast follows Contrary, Simple and self-drop protection rules")
{
    enum Ability ability;
    s32 change;
    PARAMETRIZE { ability = ABILITY_CONTRARY; change = 1; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; change = -2; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; change = -1; }
    PARAMETRIZE { ability = ABILITY_SHEER_FORCE; change = -1; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); }
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + change);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("White Herb restores all five Void Blast stat drops")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); }
    } SCENE {
        HP_BAR(opponent);
        ITEM_POPUP(player, ITEM_WHITE_HERB);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Void Blast does not lower stats on a miss, Protect or ability immunity")
{
    u32 blocked;
    PARAMETRIZE { blocked = 0; }
    PARAMETRIZE { blocked = 1; }
    PARAMETRIZE { blocked = 2; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EEVEE) { HP(1000); MaxHP(1000); Ability(blocked == 2 ? ABILITY_WONDER_GUARD : ABILITY_RUN_AWAY); }
    } WHEN {
        TURN {
            if (blocked == 0) { MOVE(player, MOVE_VOID_BLAST, hit: FALSE); }
            else if (blocked == 1) { MOVE(player, MOVE_VOID_BLAST); MOVE(opponent, MOVE_PROTECT); }
            else { MOVE(player, MOVE_VOID_BLAST); }
        }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Bulletproof blocks Void Blast without triggering its self drops")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHESPIN) { Ability(ABILITY_BULLETPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BULLETPROOF);
        NOT HP_BAR(opponent);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Void Blast still lowers stats when it hits a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); MOVE(opponent, MOVE_SUBSTITUTE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VOID_BLAST, player);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, 750);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Void Blast's Special Attack drop affects subsequent damage")
{
    s16 damageBefore, damageAfter;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOID_BLAST); }
        TURN { MOVE(player, MOVE_VOID_BLAST); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damageBefore);
        HP_BAR(opponent, captureDamage: &damageAfter);
    } THEN {
        EXPECT_LT(damageAfter, damageBefore);
    }
}

DOUBLE_BATTLE_TEST("Void Blast hits only its selected target and lowers only the user's stats")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_VOID_BLAST, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) {
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(playerRight->statStages[stat], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentLeft->statStages[stat], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->statStages[stat], DEFAULT_STAT_STAGE);
        }
    }
}
