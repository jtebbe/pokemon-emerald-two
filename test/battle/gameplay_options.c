#include "global.h"
#include "test/battle.h"

WILD_BATTLE_TEST("Level Caps option prevents battle experience from crossing the current cap")
{
    bool32 enabled;
    PARAMETRIZE { enabled = FALSE; }
    PARAMETRIZE { enabled = TRUE; }
    GIVEN {
        gSaveData.optionsLevelCaps = enabled;
        PLAYER(SPECIES_WOBBUFFET) { Level(14); Speed(100); }
        OPPONENT(SPECIES_BLISSEY) { Level(100); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        if (enabled)
        {
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 15);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP), gExperienceTables[gSpeciesInfo[SPECIES_WOBBUFFET].growthRate][15]);
        }
        else
        {
            EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 15);
        }
    }
}

WILD_BATTLE_TEST("Level Caps option blocks battle experience at the cap only when on")
{
    bool32 enabled;
    PARAMETRIZE { enabled = FALSE; }
    PARAMETRIZE { enabled = TRUE; }
    GIVEN {
        gSaveData.optionsLevelCaps = enabled;
        PLAYER(SPECIES_WOBBUFFET) { Level(15); Speed(100); }
        OPPONENT(SPECIES_BLISSEY) { Level(100); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        if (enabled)
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP), gExperienceTables[gSpeciesInfo[SPECIES_WOBBUFFET].growthRate][15]);
        else
            EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 15);
    }
}
