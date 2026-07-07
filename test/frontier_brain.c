#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "frontier_util.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/battle_frontier.h"
#include "constants/vars.h"

static void SetUpFrontierBrain(u8 facility)
{
    VarSet(VAR_FRONTIER_FACILITY, facility);
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_50;
}

static u32 CountEnemyPartyMons(void)
{
    u32 i;
    u32 count = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
            count++;
    }

    return count;
}

TEST("Anabel has three Pokemon in Battle Tower singles")
{
    SetUpFrontierBrain(FRONTIER_FACILITY_TOWER);

    CreateFrontierBrainPokemon(FRONTIER_PARTY_SIZE);

    EXPECT_EQ(CountEnemyPartyMons(), FRONTIER_PARTY_SIZE);
    EXPECT_EQ(GetMonData(&gEnemyParty[FRONTIER_PARTY_SIZE], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("Anabel has four Pokemon in Battle Tower doubles")
{
    SetUpFrontierBrain(FRONTIER_FACILITY_TOWER);

    CreateFrontierBrainPokemon(FRONTIER_DOUBLES_PARTY_SIZE);

    EXPECT_EQ(CountEnemyPartyMons(), FRONTIER_DOUBLES_PARTY_SIZE);
    EXPECT_NE(GetMonData(&gEnemyParty[FRONTIER_PARTY_SIZE], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("Non-Dome Frontier Brains still have three Pokemon when requested for doubles")
{
    u32 i;
    static const u8 facilities[] =
    {
        FRONTIER_FACILITY_PALACE,
        FRONTIER_FACILITY_ARENA,
        FRONTIER_FACILITY_FACTORY,
        FRONTIER_FACILITY_PIKE,
        FRONTIER_FACILITY_PYRAMID,
    };
    u8 facility = facilities[0];

    for (i = 0; i < ARRAY_COUNT(facilities); i++)
    {
        PARAMETRIZE
        {
            facility = facilities[i];
        }
    }

    SetUpFrontierBrain(facility);

    CreateFrontierBrainPokemon(FRONTIER_DOUBLES_PARTY_SIZE);

    EXPECT_EQ(CountEnemyPartyMons(), FRONTIER_PARTY_SIZE);
    EXPECT_EQ(GetMonData(&gEnemyParty[FRONTIER_PARTY_SIZE], MON_DATA_SPECIES), SPECIES_NONE);
}
