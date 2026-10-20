#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Rootbound AI values doubled Growth and avoids redundant Ingrain")
{
    bool32 sunny;
    PARAMETRIZE { sunny = FALSE; }
    PARAMETRIZE { sunny = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(sunny ? SPECIES_TORKOAL : SPECIES_WOBBUFFET) { Ability(sunny ? ABILITY_DROUGHT : ABILITY_TELEPATHY); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_GROWTH, MOVE_INGRAIN, MOVE_SCRATCH, MOVE_VINE_WHIP); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_GROWTH, MOVE_INGRAIN); EXPECT_MOVE(opponent, MOVE_GROWTH); }
    } THEN {
        EXPECT_EQ(AI_GetAdjustedStatStage(B_BATTLER_1, MOVE_GROWTH, 1), sunny ? 4 : 2);
        EXPECT_EQ(AI_GetAdjustedStatStage(B_BATTLER_1, MOVE_WORK_UP, 1), 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (sunny ? 4 : 2));
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (sunny ? 4 : 2));
    }
}

AI_SINGLE_BATTLE_TEST("Rootbound's reveal informs non-omniscient AI and makes phazing ineffective")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_IVYSAUR);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_ROAR, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_ROOTBOUND);
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids Roar and Whirlwind against ordinary Ingrain too")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_ROAR; }
    PARAMETRIZE { move = MOVE_WHIRLWIND; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_INGRAIN); }
        TURN { SCORE_LT(opponent, move, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_DOUBLE_BATTLE_TEST("Rootbound switch-in prediction restores live roots and defensive stages")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_VENUSAUR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_VINE_WHIP); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH); EXPECT_MOVE(opponentRight, MOVE_SCRATCH); }
    } THEN {
        u32 def = opponentLeft->statStages[STAT_DEF];
        u32 spdef = opponentLeft->statStages[STAT_SPDEF];
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_OPTIONAL);
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], def);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], spdef);
        EXPECT(!opponentLeft->volatiles.root);
    }
}
