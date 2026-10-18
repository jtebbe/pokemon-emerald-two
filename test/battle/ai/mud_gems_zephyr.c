#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("AI avoids Mud Bath when already Ground-typed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SANDSHREW) { HP(50); MaxHP(300); Status1(STATUS1_BURN); Moves(MOVE_MUD_BATH, MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("Mud Bath AI values curing harmful status even at full HP")
{
    bool32 poisoned;
    PARAMETRIZE { poisoned = FALSE; }
    PARAMETRIZE { poisoned = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(poisoned ? STATUS1_POISON : STATUS1_NONE); Moves(MOVE_MUD_BATH); }
    } WHEN {
        if (poisoned)
            TURN { SCORE_GT_VAL(opponent, MOVE_MUD_BATH, AI_SCORE_DEFAULT); }
        else
            TURN { SCORE_EQ_VAL(opponent, MOVE_MUD_BATH, AI_SCORE_DEFAULT); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->types[0], TYPE_PSYCHIC);
        EXPECT_EQ(opponent->types[1], TYPE_GROUND);
        EXPECT(!HasHealingEffect(B_BATTLER_1));
    }
}

AI_SINGLE_BATTLE_TEST("AI sees Gemshot's conditional double hit when comparing damage")
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    ResetStartingStatuses();
    if (rocks)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GEMSHOT, MOVE_POWER_GEM); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, rocks ? MOVE_GEMSHOT : MOVE_POWER_GEM); }
    }
}

AI_SINGLE_BATTLE_TEST("Gemshot AI damage ranges match its actual one or two hits")
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    ResetStartingStatuses();
    if (rocks)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    s16 damage, secondHit;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GEMSHOT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_GEMSHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, opponent);
        HP_BAR(player, captureDamage: &damage);
        if (rocks)
            HP_BAR(player, captureDamage: &secondHit);
    } THEN {
        s32 total = damage;
        if (rocks)
            total += secondHit;
        EXPECT_GE(total, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].minimum);
        EXPECT_LE(total, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Gemshot AI recognizes a two-hit KO through Focus Sash")
{
    SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); SpDefense(100); Item(ITEM_FOCUS_SASH); Ability(ABILITY_MAGIC_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); SpAttack(300); Moves(MOVE_GEMSHOT, MOVE_POWER_GEM); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_GEMSHOT); }
    }
}

AI_DOUBLE_BATTLE_TEST("Harsh Zephyr AI targeting and spread damage follow its own Tailwind")
{
    bool32 tailwind;
    PARAMETRIZE { tailwind = FALSE; }
    PARAMETRIZE { tailwind = TRUE; }
    ResetStartingStatuses();
    if (tailwind)
        SetStartingStatus(STARTING_STATUS_TAILWIND_OPPONENT);
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_BULBASAUR) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HARSH_ZEPHYR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_HARSH_ZEPHYR, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &damage);
        if (tailwind)
            HP_BAR(playerRight);
        else
            NOT HP_BAR(playerRight);
    } THEN {
        EXPECT_EQ(AI_GetBattlerMoveTargetType(B_BATTLER_1, MOVE_HARSH_ZEPHYR), tailwind ? TARGET_BOTH : TARGET_SELECTED);
        EXPECT_GE(damage, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].minimum);
        EXPECT_LE(damage, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Harsh Zephyr AI avoids Wind Rider immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Ability(ABILITY_WIND_RIDER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HARSH_ZEPHYR, MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}
