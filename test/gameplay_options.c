#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "daycare.h"
#include "event_data.h"
#include "new_game.h"
#include "test/test.h"

TEST("Gameplay options default to off")
{
    gSaveData.optionsLevelCaps = TRUE;
    gSaveData.optionsTrainerScaling = TRUE;
    ResetSaveDataAndOptions();
    EXPECT_EQ((u32)gSaveData.optionsLevelCaps, FALSE);
    EXPECT_EQ((u32)gSaveData.optionsTrainerScaling, FALSE);
}

TEST("Level Caps option toggles the existing badge cap progression")
{
    static const u16 milestones[] = {
        FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET,
        FLAG_BADGE04_GET, FLAG_BADGE05_GET, FLAG_BADGE06_GET,
        FLAG_BADGE07_GET, FLAG_BADGE08_GET, FLAG_IS_CHAMPION,
    };
    static const u8 caps[] = {15, 19, 24, 29, 31, 33, 42, 46, 58, MAX_LEVEL};
    u32 progress = 0;
    for (u32 i = 0; i <= ARRAY_COUNT(milestones); i++)
        PARAMETRIZE { progress = i; }

    for (u32 i = 0; i < ARRAY_COUNT(milestones); i++)
    {
        FlagClear(milestones[i]);
        if (i < progress)
            FlagSet(milestones[i]);
    }
    gSaveData.optionsLevelCaps = FALSE;
    EXPECT(!IsLevelCapEnabled());
    EXPECT_EQ(GetCurrentLevelCap(), MAX_LEVEL);
    EXPECT_EQ(GetSoftLevelCapExpValue(50, 1000), 1000);
    gSaveData.optionsLevelCaps = TRUE;
    EXPECT(IsLevelCapEnabled());
    EXPECT_EQ(GetCurrentLevelCap(), caps[progress]);
    EXPECT_EQ(GetSoftLevelCapExpValue(caps[progress], 1000), 0);
}

TEST("Level Caps option stops level gains at the cap but never lowers existing levels")
{
    bool32 enabled;
    PARAMETRIZE { enabled = FALSE; }
    PARAMETRIZE { enabled = TRUE; }
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    gSaveData.optionsLevelCaps = enabled;
    CreateMon(mon, SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PRESET(0));
    u32 exp = gExperienceTables[GROWTH_MEDIUM_SLOW][20];
    SetMonData(mon, MON_DATA_EXP, &exp);
    EXPECT(TryIncrementMonLevel(mon));
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 15);
    EXPECT_EQ(TryIncrementMonLevel(mon), !enabled);

    CreateMon(mon, SPECIES_BULBASAUR, 30, 0, OTID_STRUCT_PRESET(0));
    exp = gExperienceTables[GROWTH_MEDIUM_SLOW][31];
    SetMonData(mon, MON_DATA_EXP, &exp);
    EXPECT_EQ(TryIncrementMonLevel(mon), !enabled);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), enabled ? 30 : 31);
}

TEST("Level Caps option applies to Rare and EXP Candies")
{
    bool32 enabled;
    enum Item candy;
    PARAMETRIZE { enabled = FALSE; candy = ITEM_RARE_CANDY; }
    PARAMETRIZE { enabled = TRUE; candy = ITEM_RARE_CANDY; }
    PARAMETRIZE { enabled = FALSE; candy = ITEM_EXP_CANDY_XL; }
    PARAMETRIZE { enabled = TRUE; candy = ITEM_EXP_CANDY_XL; }
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    gSaveData.optionsLevelCaps = enabled;
    gBattleTypeFlags = 0;
    CreateMon(mon, SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PRESET(0));
    EXPECT(!ExecuteTableBasedItemEffect(mon, candy, 0, 0));
    if (enabled || candy == ITEM_RARE_CANDY)
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 15);
    else
        EXPECT_GT(GetMonData(mon, MON_DATA_LEVEL), 15);
    if (enabled)
    {
        EXPECT(ExecuteTableBasedItemEffect(mon, candy, 0, 0));
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 15);
    }
}

TEST("Level Caps option applies to daycare without negative level gains for over-cap Pokemon")
{
    bool32 enabled;
    u32 level;
    PARAMETRIZE { enabled = FALSE; level = 14; }
    PARAMETRIZE { enabled = TRUE; level = 14; }
    PARAMETRIZE { enabled = FALSE; level = 30; }
    PARAMETRIZE { enabled = TRUE; level = 30; }
    gSaveData.optionsLevelCaps = enabled;
    ZeroPlayerPartyMons();
    memset(&gSaveData.daycare, 0, sizeof(gSaveData.daycare));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, level, 0, OTID_STRUCT_PRESET(0));
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveData.daycare.mons[0]);
    gSaveData.daycare.mons[0].steps = 50000;
    gSpecialVar_0x8004 = 0;
    if (enabled)
        EXPECT_EQ(GetNumLevelsGainedFromDaycare(), level < 15 ? 15 - level : 0);
    else
        EXPECT_GT(GetNumLevelsGainedFromDaycare(), 0);
    TakePokemonFromDaycare();
    if (enabled)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), level < 15 ? 15 : level);
    else
        EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), level);
}
