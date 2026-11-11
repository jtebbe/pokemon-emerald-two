#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Air Purifier fully heals poisoned recipients and gains one stage per cleanse")
{
    u32 selfStatus, allyStatus;
    PARAMETRIZE { selfStatus = STATUS1_POISON; allyStatus = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { selfStatus = STATUS1_NONE;   allyStatus = STATUS1_POISON; }
    PARAMETRIZE { selfStatus = STATUS1_POISON; allyStatus = STATUS1_BURN; }
    PARAMETRIZE { selfStatus = STATUS1_NONE;   allyStatus = STATUS1_NONE; }
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALAR) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(160); Status1(selfStatus); }
        PLAYER(SPECIES_WOBBUFFET) { HP(40); MaxHP(160); Status1(allyStatus); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        bool32 selfCured = (selfStatus & STATUS1_PSN_ANY) != 0;
        bool32 allyCured = (allyStatus & STATUS1_PSN_ANY) != 0;
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerLeft->hp, selfCured ? 160 : 40);
        EXPECT_EQ(playerRight->status1, allyCured ? STATUS1_NONE : allyStatus);
        if (allyStatus != STATUS1_BURN)
            EXPECT_EQ(playerRight->hp, allyCured ? 160 : 40);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
        {
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + selfCured + allyCured);
            EXPECT_EQ(playerRight->statStages[stat], DEFAULT_STAT_STAGE);
        }
        EXPECT_EQ(playerLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Air Purifier grants Levitate even when there is no poison to cleanse")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
