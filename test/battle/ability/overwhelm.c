#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Overwhelm lowers Attack and Sp. Atk once per multihit attack")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_COVERT_CLOAK; }
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); }
    } THEN {
        s32 expectedStage = DEFAULT_STAT_STAGE - (item == ITEM_NONE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], expectedStage);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], expectedStage);
    }
}

DOUBLE_BATTLE_TEST("Overwhelm affects each target damaged by a spread move")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SWIFT);
            MOVE(opponentLeft, protect ? MOVE_PROTECT : MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - !protect);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - !protect);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
