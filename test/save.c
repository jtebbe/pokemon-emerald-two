#include "global.h"
#include "load_save.h"
#include "save.h"
#include "overworld.h"
#include "event_object_movement.h"
#include "mail.h"
#include "test/test.h"

TEST("Unified save data fits the single flash image")
{
    EXPECT_LE(sizeof(struct SaveData), SECTOR_DATA_SIZE * NUM_SECTORS_PER_SLOT);
}

TEST("Hall of Fame missing-flash errors select the restart screen even without damaged sectors")
{
    MainCallback callback = gMain.callback2;
    IntrCallback vblank = gMain.vblankCallback;
    IntrCallback hblank = gMain.hblankCallback;
    u8 state = gMain.state;
    bool8 resetDisabled = gSoftResetDisabled;
    bool32 present = gFlashMemoryPresent;
    ResetSaveData();
    gFlashMemoryPresent = FALSE;
    gDamagedSaveSectors = 0;
    EXPECT(!TrySavingHallOfFame());
    EXPECT_EQ(gDamagedSaveSectors, 0);
    EXPECT_EQ(gMain.callback2, CB2_SaveErrorScreen);
    EXPECT_EQ(GetHallOfFameEntryCount(), 0);
    gFlashMemoryPresent = present;
    SetMainCallback2(callback);
    gMain.state = state;
    SetVBlankCallback(vblank);
    SetHBlankCallback(hblank);
    gSoftResetDisabled = resetDisabled;
}

TEST("The flat save image resets all live game data")
{
    gSaveData.money = 123;
    gSaveData.playerName[0] = 1;
    gSaveData.currentBox = 3;
    gSaveData.playerParty[0].data.species = SPECIES_PIKACHU;
    ResetSaveData();
    EXPECT_EQ(gSaveData.money, 0);
    EXPECT_EQ(gSaveData.playerName[0], 0);
    EXPECT_EQ(gSaveData.currentBox, 0);
    EXPECT_EQ(gSaveData.playerParty[0].data.species, SPECIES_NONE);
}

TEST("Resetting game progress preserves chosen player identity and options")
{
    gSaveData.playerName[0] = 12;
    gSaveData.playerGender = FEMALE;
    gSaveData.optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
    gSaveData.money = 300;
    gSaveData.currentBox = 3;
    ResetGameProgress();
    EXPECT_EQ(gSaveData.playerName[0], 12);
    EXPECT_EQ(gSaveData.playerGender, FEMALE);
    EXPECT_EQ((u32)gSaveData.optionsTextSpeed, OPTIONS_TEXT_SPEED_FAST);
    EXPECT_EQ(gSaveData.money, 0);
    EXPECT_EQ(gSaveData.currentBox, 0);
}

TEST("Hall of Fame records capture the first entry time and cap the entry count")
{
    ResetSaveData();
    EXPECT_EQ(GetHallOfFameEntryCount(), 0);
    EXPECT_EQ(GetFirstHallOfFameTime(), 0);
    gSaveData.playTimeHours = 18;
    gSaveData.playTimeMinutes = 34;
    gSaveData.playTimeSeconds = 56;
    RecordHallOfFameEntry();
    EXPECT_EQ(GetHallOfFameEntryCount(), 1);
    EXPECT_EQ(GetFirstHallOfFameTime(), (18 << 16) | (34 << 8) | 56);

    gSaveData.playTimeHours = 99;
    for (u32 i = 0; i < 1100; i++)
        RecordHallOfFameEntry();
    EXPECT_EQ(GetHallOfFameEntryCount(), 999);
    EXPECT_EQ(GetFirstHallOfFameTime(), (18 << 16) | (34 << 8) | 56);

    ResetGameProgress();
    EXPECT_EQ(GetHallOfFameEntryCount(), 0);
    EXPECT_EQ(GetFirstHallOfFameTime(), 0);
}

TEST("Saved objects retain direct graphics IDs and activity without legacy encoding")
{
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    gObjectEvents[0].graphicsId = 0x1234;
    gObjectEvents[0].active = TRUE;
    gObjectEvents[0].spriteId = 7;
    SaveObjectEvents();
    EXPECT_EQ(gSaveData.objectEvents[0].graphicsId, 0x1234);
    EXPECT(gSaveData.objectEvents[0].active);
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    LoadObjectEvents();
    EXPECT_EQ(gObjectEvents[0].graphicsId, 0x1234);
    EXPECT(gObjectEvents[0].active);
    EXPECT_EQ(gObjectEvents[0].spriteId, 0);
}

TEST("Saved party rebuilds cached stats while preserving HP, status, moves and mail")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_PIKACHU, 30, 0, OTID_STRUCT_PRESET(1234));
    CalculateMonStats(mon);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    mon->hp = mon->maxHP - 7;
    mon->status = STATUS1_POISON;
    mon->mail = 2;
    u32 hp = mon->hp;
    u32 maxHP = mon->maxHP;
    u32 attack = mon->attack;
    enum Move move = GetMonData(mon, MON_DATA_MOVE1);
    SavePlayerParty();
    ZeroPlayerPartyMons();
    LoadPlayerParty();
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    EXPECT_EQ(mon->hp, hp);
    EXPECT_EQ(mon->maxHP, maxHP);
    EXPECT_EQ(mon->attack, attack);
    EXPECT_EQ(mon->status, STATUS1_POISON);
    EXPECT_EQ(mon->mail, 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1), move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("Persistent mon conversion preserves a fainted fusion without cached party stats")
{
    struct Pokemon original, restored;
    struct BoxPokemon saved;
    CreateMon(&original, SPECIES_ZEKROM, 50, 0, OTID_STRUCT_PRESET(5678));
    CalculateMonStats(&original);
    original.hp = 0;
    original.status = STATUS1_PARALYSIS;
    SaveMonToBox(&saved, &original);
    BoxMonToMon(&saved, &restored);
    EXPECT_EQ(restored.hp, 0);
    EXPECT_EQ(restored.maxHP, original.maxHP);
    EXPECT_EQ(restored.status, STATUS1_PARALYSIS);
    EXPECT_EQ(GetMonData(&restored, MON_DATA_SPECIES), SPECIES_ZEKROM);
}

TEST("Rebuilding a saved mon clamps damage beyond its recalculated maximum HP")
{
    struct Pokemon original, restored;
    CreateMon(&original, SPECIES_PICHU, 1, 0, OTID_STRUCT_PRESET(0));
    original.box.hpLost = 1000;
    BoxMonToMon(&original.box, &restored);
    EXPECT_EQ(restored.hp, 0);
    EXPECT(restored.maxHP > 0);
}
