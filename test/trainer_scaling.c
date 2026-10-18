#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "data.h"
#include "malloc.h"
#include "trainer_util.h"
#include "test/test.h"

static void SetUpScalingParty(void)
{
    ZeroPlayerPartyMons();
    CreateMonWithStats(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, 20, 0, OTID_STRUCT_PRESET(0));
    CreateMonWithStats(&gParties[B_TRAINER_PLAYER][1], SPECIES_BULBASAUR, MAX_LEVEL, 0, OTID_STRUCT_PRESET(0));
    u32 value = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_IS_EGG, &value);
    CreateMonWithStats(&gParties[B_TRAINER_PLAYER][2], SPECIES_BULBASAUR, 47, 0, OTID_STRUCT_PRESET(0));
    value = 0;
    SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HP, &value);
    CreateMonWithStats(&gParties[B_TRAINER_PLAYER][5], SPECIES_BULBASAUR, 35, 0, OTID_STRUCT_PRESET(0));
}

TEST("Trainer Scaling uses the highest non-egg party level and preserves trainer customization")
{
    bool32 enabled, caps;
    PARAMETRIZE { enabled = FALSE; caps = FALSE; }
    PARAMETRIZE { enabled = TRUE; caps = FALSE; }
    PARAMETRIZE { enabled = FALSE; caps = TRUE; }
    PARAMETRIZE { enabled = TRUE; caps = TRUE; }
    SetUpScalingParty();
    gSaveData.optionsTrainerScaling = enabled;
    gSaveData.optionsLevelCaps = caps;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    const struct Trainer *trainer = GetTrainerStructFromId(3);
    struct Pokemon *party = Alloc(PARTY_SIZE * sizeof(*party));
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    EXPECT_EQ(GetTrainerScalingLevel(), enabled ? 47 : 0);
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        u32 level = enabled ? 47 : trainer->party[i].lvl;
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_LEVEL), level);
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_EXP), gExperienceTables[gSpeciesInfo[trainer->party[i].species].growthRate][level]);
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), trainer->party[i].species);
    }
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE1), MOVE_AIR_SLASH);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HELD_ITEM), ITEM_ASSAULT_VEST);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP_EV), 252);
    struct Pokemon expected;
    CreateMon(&expected, trainer->party[1].species, enabled ? 47 : trainer->party[1].lvl, 0, OTID_STRUCT_PRESET(0));
    CalculateMonStats(&expected);
    GiveMonInitialMoveset(&expected);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE4), GetMonData(&expected, MON_DATA_MOVE4));
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_MAX_HP), GetMonData(&expected, MON_DATA_MAX_HP));
    Free(party);
}

TEST("Trainer Scaling affects both opponent parties")
{
    SetUpScalingParty();
    gSaveData.optionsTrainerScaling = TRUE;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_TWO_OPPONENTS;
    const struct Trainer *trainer = GetTrainerStructFromId(3);
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_A], trainer);
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_B], trainer);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 47);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_B][0], MON_DATA_LEVEL), 47);
}

TEST("Trainer Scaling leaves wild static and scripted player Pokemon unchanged")
{
    SetUpScalingParty();
    gSaveData.optionsTrainerScaling = TRUE;
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_MEW, 5, 0, OTID_STRUCT_PRESET(0));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 5);
    const struct Trainer *trainer = GetTrainerStructFromId(3);
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_PLAYER], trainer);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), trainer->party[0].lvl);
}

TEST("Trainer Scaling falls back to configured levels when no non-egg party exists")
{
    gSaveData.optionsTrainerScaling = TRUE;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, MAX_LEVEL, 0, OTID_STRUCT_PRESET(0));
    u32 egg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &egg);
    EXPECT_EQ(GetTrainerScalingLevel(), 0);
    const struct Trainer *trainer = GetTrainerStructFromId(3);
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_A], trainer);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), trainer->party[0].lvl);
}

TEST("NPC partner generators use Trainer Scaling too")
{
    SetUpScalingParty();
    gSaveData.optionsTrainerScaling = TRUE;
    struct TrainerGenerator generator = {0};
    const struct Trainer *trainer = GetTrainerStructFromId(3);
    MakePartnerGenerator(&generator, trainer);
    EXPECT_EQ(generator.levelOverride, 47);
    struct Pokemon mon;
    GenerateMonFromTrainerMon(&mon, &trainer->party[0], &generator);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 47);
}
