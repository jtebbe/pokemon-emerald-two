#include "global.h"
#include "gba/flash_internal.h"
#include "agb_flash.h"
#include "debug_pc_fill.h"
#include "pc_storage.h"
#include "pc_move_journal.h"
#include "pokemon_storage_system.h"
#include "load_save.h"
#include "save.h"
#include "hall_of_fame.h"
#include "bg.h"
#include "window.h"
#include "new_game.h"
#include "malloc.h"
#include "item.h"
#include "daycare.h"
#include "event_data.h"
#include "fake_rtc.h"
#include "link_protocol.h"
#include "overworld.h"
#include "test/test.h"

static struct BoxPokemon MakeMon(enum Species species, u32 marker)
{
    struct Pokemon mon;
    CreateMon(&mon, species, 30, marker, OTID_STRUCT_PRESET(marker));
    CalculateMonStats(&mon);
    struct BoxPokemon box;
    SaveMonToBox(&box, &mon);
    return box;
}

static u8 *SetUpStorage(void)
{
    u8 *flash = Alloc(NUM_SECTORS_PER_SLOT * SECTOR_SIZE);
    EXPECT(flash != NULL);
    memset(flash, 0xFF, NUM_SECTORS_PER_SLOT * SECTOR_SIZE);
    PCStorageTestFlash(flash, -1);
    ResetSaveData();
    ZeroPlayerPartyMons();
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    gSaveData.money = 3000;
    gSaveData.playerTrainerId[0] = 123;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 30, 123, OTID_STRUCT_PRESET(123));
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    Save_ResetSaveCounters();
    EXPECT(PCStorageCommit(TRUE));
    EXPECT(PCStorageWindow(0));
    return flash;
}

static void TearDownStorage(u8 *flash)
{
    PCStorageTestFlash(NULL, -1);
    Free(flash);
    ResetSaveData();
    gDifferentSaveFile = FALSE;
}

static void Restart(void)
{
    PCStorageTestFailure(-1);
    ResetSaveData();
    ZeroPlayerPartyMons();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
}

TEST("PC streaming keeps exactly three boxes and wraps both neighbors at box forty")
{
    u8 *flash = SetUpStorage();
    EXPECT_EQ(TOTAL_BOXES_COUNT, 40);
    EXPECT_EQ(PCStorageTestResidentCount(), 3);
    EXPECT(PCStorageTestResident(39));
    EXPECT(PCStorageTestResident(0));
    EXPECT(PCStorageTestResident(1));
    EXPECT(PCStorageWindow(39));
    EXPECT_EQ(PCStorageTestResidentCount(), 3);
    EXPECT(PCStorageTestResident(38));
    EXPECT(PCStorageTestResident(39));
    EXPECT(PCStorageTestResident(0));
    EXPECT(!PCStorageTestResident(1));
    EXPECT(!PCStorageWindow(40));
    TearDownStorage(flash);
}

TEST("PC occupancy and egg filtering use direct data across eviction and restart")
{
    u8 headerBits;
    PARAMETRIZE { headerBits = 0; }
    PARAMETRIZE { headerBits = 7; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    ((u8 *)&mon)[offsetof(struct BoxPokemon, otName) - 1] = headerBits;
    SetBoxMonAt(0, 0, &mon);
    EXPECT(IsBoxMonNonEggAt(0, 0));
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT(PCStorageWindow(20));
    Restart();
    EXPECT(IsBoxMonNonEggAt(0, 0));
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT_EQ(CountMonsInBox(0), 1);
    GetBoxedMonPtr(0, 0)->data.isEgg = TRUE;
    EXPECT(!IsBoxMonNonEggAt(0, 0));
    EXPECT_EQ(CountMonsInBox(0), 1);
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    EXPECT(PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_IS_EGG), TRUE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES_OR_EGG), SPECIES_EGG);
    EXPECT(!IsBoxMonNonEggAt(TOTAL_BOXES_COUNT, 0));
    EXPECT(!IsBoxMonNonEggAt(0, IN_BOX_COUNT));
    GetBoxedMonPtr(0, 0)->data.species = SPECIES_NONE;
    EXPECT_EQ(CountMonsInBox(0), 0);
    TearDownStorage(flash);
}

TEST("PC streaming read-only scans neither allocate all boxes nor write flash")
{
    u8 *flash = SetUpStorage();
    u32 writes = PCStorageTestWrites();
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    EXPECT(CheckFreePokemonStorageSpace());
    EXPECT(!AnyStorageMonWithMove(MOVE_PSYCHIC));
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        EXPECT_EQ(CountMonsInBox(box), 0);
        EXPECT_LE(PCStorageTestResidentCount(), 3);
    }
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(!PCStorageFaulted());
    TearDownStorage(flash);
}

TEST("PC streaming dirty eviction saves distant boxes, metadata and player state")
{
    u8 *flash = SetUpStorage();
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        struct BoxPokemon mon = MakeMon(SPECIES_BULBASAUR, box + 1);
        SetBoxMonAt(box, IN_BOX_COUNT - 1, &mon);
        gSaveData.boxWallpapers[box] = box % 4;
    }
    gSaveData.money = 9876;
    EXPECT(PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(gSaveData.money, 9876);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        EXPECT_EQ(GetBoxMonDataAt(box, 29, MON_DATA_SPECIES), SPECIES_BULBASAUR);
        EXPECT_EQ(GetBoxMonDataAt(box, 29, MON_DATA_PERSONALITY), box + 1);
        EXPECT_EQ(gSaveData.boxWallpapers[box], box % 4);
    }
    EXPECT_EQ(CountStorageNonEggMons(), 40);
    EXPECT_LE(PCStorageTestResidentCount(), 3);
    TearDownStorage(flash);
}

TEST("PC streaming stores and reloads all twelve hundred slots without a full RAM image")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_ABRA, 1);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            mon.personality = box * IN_BOX_COUNT + slot + 1;
            SetBoxMonAt(box, slot, &mon);
        }
    EXPECT(PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1200);
    EXPECT(!CheckFreePokemonStorageSpace());
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            EXPECT_EQ(GetBoxMonDataAt(box, slot, MON_DATA_PERSONALITY), box * IN_BOX_COUNT + slot + 1);
    EXPECT_LE(PCStorageTestResidentCount(), 3);
    TearDownStorage(flash);
}

TEST("PC streaming raw pointers detect edits and box copies survive source eviction")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 999);
    *GetBoxedMonPtr(0, 0) = mon; // Legacy pointer callers must not lose writes.
    SetBoxMonAt(20, 29, GetBoxedMonPtr(0, 0));
    EXPECT(PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 999);
    EXPECT_EQ(GetBoxMonDataAt(20, 29, MON_DATA_PERSONALITY), 999);
    TearDownStorage(flash);
}

TEST("PC streaming unsealed writes roll back and sealed writes recover every home page")
{
    u32 cut;
    bool32 committed;
    PARAMETRIZE { cut = 0; committed = FALSE; }
    PARAMETRIZE { cut = 64; committed = FALSE; }
    PARAMETRIZE { cut = SECTOR_SIZE - 1; committed = FALSE; }
    PARAMETRIZE { cut = SECTOR_SIZE; committed = FALSE; }
    PARAMETRIZE { cut = 3 * SECTOR_SIZE - 1; committed = FALSE; }
    PARAMETRIZE { cut = 3 * SECTOR_SIZE; committed = TRUE; }
    PARAMETRIZE { cut = 3 * SECTOR_SIZE + 100; committed = TRUE; }
    PARAMETRIZE { cut = 5 * SECTOR_SIZE + 100; committed = TRUE; }
    PARAMETRIZE { cut = 6 * SECTOR_SIZE; committed = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 999);
    SetBoxMonAt(0, 0, &mon);
    gSaveData.money = 7000;
    PCStorageTestFailure(cut);
    EXPECT(!PCStorageCommit(FALSE));
    EXPECT(PCStorageFaulted());
    Restart();
    EXPECT_EQ(gSaveData.money, committed ? 7000 : 3000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), committed ? SPECIES_MEW : SPECIES_NONE);
    EXPECT(!PCStorageFaulted());
    TearDownStorage(flash);
}

TEST("PC streaming carries a Pokemon across arbitrary boxes and returns it once after reset")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 999);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    struct Pokemon carried;
    BoxMonAtToMon(0, 0, &carried);
    PCStorageCarrySingle(0, 0, &carried);
    ZeroBoxMonAt(0, 0);
    EXPECT(PCStorageAutosave());
    EXPECT(PCStorageWindow(20));
    EXPECT(PCStorageWindow(35));
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 999);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT(!PCStorageCarrying());
    Restart(); // Recovery is idempotent, not another delivery.
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC streaming party pickup preserves compacted teammates, damage, status and mail")
{
    u8 *flash = SetUpStorage();
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_ABRA, 30, 2, OTID_STRUCT_PRESET(2));
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][1]);
    gPartiesCount[B_TRAINER_PLAYER] = 2;
    gParties[B_TRAINER_PLAYER][0].hp -= 7;
    gParties[B_TRAINER_PLAYER][0].status = STATUS1_POISON;
    gParties[B_TRAINER_PLAYER][0].mail = 2;
    gSaveData.mail[2].words[0] = 321;
    struct Pokemon carried = gParties[B_TRAINER_PLAYER][0];
    PCStorageCarrySingle(TOTAL_BOXES_COUNT, 0, &carried);
    ZeroMonData(&gParties[B_TRAINER_PLAYER][0]);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ABRA);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].hp, carried.hp);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].status, STATUS1_POISON);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].mail, 2);
    EXPECT_EQ(gSaveData.mail[2].words[0], 321);
    TearDownStorage(flash);
}

TEST("PC streaming swaps retain the displaced Pokemon in the original free slot on reset")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon first = MakeMon(SPECIES_MEW, 1), second = MakeMon(SPECIES_ABRA, 2);
    SetBoxMonAt(0, 0, &first);
    SetBoxMonAt(20, 0, &second);
    EXPECT(PCStorageCommit(FALSE));
    struct Pokemon carried, displaced;
    BoxMonAtToMon(0, 0, &carried);
    PCStorageCarrySingle(0, 0, &carried);
    ZeroBoxMonAt(0, 0);
    EXPECT(PCStorageAutosave());
    BoxMonAtToMon(20, 0, &displaced);
    SetBoxMonAt(20, 0, &carried.box);
    carried = displaced;
    PCStorageCarrySingle(20, 0, &carried);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_ABRA);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT_EQ(CountStorageNonEggMons(), 2);
    TearDownStorage(flash);
}

TEST("PC streaming multi-move can recover an entire carried final box")
{
    bool32 placed;
    PARAMETRIZE { placed = FALSE; }
    PARAMETRIZE { placed = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon *group = Alloc(sizeof(*group) * IN_BOX_COUNT);
    EXPECT(group != NULL);
    group[0] = MakeMon(SPECIES_MEW, 1);
    for (u32 i = 0; i < IN_BOX_COUNT; i++)
    {
        group[i] = group[0];
        group[i].personality = i + 1;
        SetBoxMonAt(39, i, &group[i]);
    }
    EXPECT(PCStorageCommit(FALSE));
    PCStorageCarryGroup(39, 0, IN_BOX_COLUMNS, IN_BOX_ROWS, group);
    for (u32 i = 0; i < IN_BOX_COUNT; i++) ZeroBoxMonAt(39, i);
    EXPECT(PCStorageAutosave());
    EXPECT(PCStorageWindow(13));
    if (placed)
    {
        for (u32 i = 0; i < IN_BOX_COUNT; i++) SetBoxMonAt(13, i, &group[i]);
        PCStorageCarryEnd();
        EXPECT(PCStorageAutosave());
    }
    Free(group);
    Restart();
    EXPECT_EQ(CountMonsInBox(placed ? 13 : 39), 30);
    EXPECT_EQ(CountMonsInBox(placed ? 39 : 13), 0);
    EXPECT_EQ(CountStorageNonEggMons(), 30);
    for (u32 i = 0; i < IN_BOX_COUNT; i++)
        EXPECT_EQ(GetBoxMonDataAt(placed ? 13 : 39, i, MON_DATA_PERSONALITY), i + 1);
    TearDownStorage(flash);
}

TEST("PC streaming cursor items survive reset without duplicating the source held item")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 1);
    enum Item item = ITEM_LEFTOVERS, none = ITEM_NONE;
    SetBoxMonData(&mon, MON_DATA_HELD_ITEM, &item);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    SetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM, &none);
    PCStorageCarryItem(item);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(CheckBagHasItem(ITEM_LEFTOVERS, 1));
    EXPECT(!CheckBagHasItem(ITEM_LEFTOVERS, 2));
    Restart();
    EXPECT(!CheckBagHasItem(ITEM_LEFTOVERS, 2));
    TearDownStorage(flash);
}

TEST("PC streaming clock ticks do not repeatedly auto-save an idle PC")
{
    u8 *flash = SetUpStorage();
    u32 writes = PCStorageTestWrites();
    for (u32 i = 0; i < 60; i++)
    {
        gSaveData.playTimeVBlanks++;
        EXPECT(PCStorageAutosave());
    }
    EXPECT_EQ(PCStorageTestWrites(), writes);
    TearDownStorage(flash);
}

TEST("PC streaming preserves resident fusion, daycare, box name and wallpaper data")
{
    u8 *flash = SetUpStorage();
    gSaveData.fusions[3] = MakeMon(SPECIES_SPECTRIER, 17);
    gSaveData.fusionMail[3] = 4;
    gSaveData.daycare.mons[0].mon = MakeMon(SPECIES_DITTO, 28);
    gSaveData.daycare.mons[0].steps = 12345;
    memcpy(GetBoxNamePtr(39), COMPOUND_STRING("LAST"), 5);
    gSaveData.boxWallpapers[39] = 3;
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(gSaveData.fusions[3].data.species, SPECIES_SPECTRIER);
    EXPECT_EQ(gSaveData.fusionMail[3], 4);
    EXPECT_EQ(gSaveData.daycare.mons[0].mon.data.species, SPECIES_DITTO);
    EXPECT_EQ(gSaveData.daycare.mons[0].steps, 12345);
    EXPECT_EQ(GetBoxNamePtr(39)[0], COMPOUND_STRING("LAST")[0]);
    EXPECT_EQ(gSaveData.boxWallpapers[39], 3);
    TearDownStorage(flash);
}

TEST("PC streaming no longer reserves boxed Pokemon inside the resident save image")
{
    EXPECT_EQ(NUM_SECTORS_PER_SLOT, 24);
    EXPECT_LE(sizeof(struct SaveData), NUM_CORE_SAVE_SECTORS * SECTOR_DATA_SIZE);
    EXPECT_LE(sizeof(struct SaveData), 8192);
    EXPECT_EQ(PC_CACHE_BOXES, 3);
}

TEST("PC streaming item placement clears cursor ownership before animation finishes")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 1);
    SetBoxMonAt(0, 0, &mon);
    PCStorageCarryItem(ITEM_LEFTOVERS);
    EXPECT(PCStorageAutosave());
    enum Item item = ITEM_LEFTOVERS;
    SetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM, &item);
    PCStorageCarryItem(ITEM_NONE);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT(!CheckBagHasItem(ITEM_LEFTOVERS, 1));
    TearDownStorage(flash);
}

TEST("PC streaming daycare deposits remove the selected boxed Pokemon exactly once")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_DITTO, 35);
    SetBoxMonAt(20, 29, &mon);
    EXPECT(PCStorageCommit(FALSE));
    gSpecialVar_0x8004 = PC_MON_CHOSEN;
    gSpecialVar_MonBoxId = 20;
    gSpecialVar_MonBoxPos = 29;
    StoreSelectedPokemonInDaycare();
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    EXPECT_EQ(gSaveData.daycare.mons[0].mon.data.species, SPECIES_DITTO);
    EXPECT_EQ(gSaveData.daycare.mons[0].mon.personality, 35);
    EXPECT_EQ(CountPokemonInDaycare(&gSaveData.daycare), 1);
    EXPECT(!PCStorageCarrying());
    TearDownStorage(flash);
}

TEST("PC streaming selected NPC trade replacements finish the durable transfer")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 1);
    SetBoxMonAt(20, 29, &mon);
    EXPECT(PCStorageCommit(FALSE));
    gSpecialVar_MonBoxId = 20;
    gSpecialVar_MonBoxPos = 29;
    struct Pokemon exchanged;
    RemoveSelectedPcMon(&exchanged);
    EXPECT_EQ(exchanged.box.data.species, SPECIES_MEW);
    CreateMon(&exchanged, SPECIES_ABRA, 30, 2, OTID_STRUCT_PRESET(2));
    EXPECT_EQ(CopyMonToPC(&exchanged), MON_GIVEN_TO_PC);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT_EQ(GetBoxMonDataAt(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos, MON_DATA_SPECIES), SPECIES_ABRA);
    EXPECT(!PCStorageCarrying());
    TearDownStorage(flash);
}

TEST("PC streaming interrupted placement either restores pickup or finishes the destination")
{
    u32 cut;
    bool32 placed;
    PARAMETRIZE { cut = 0; placed = FALSE; }
    PARAMETRIZE { cut = 5 * SECTOR_SIZE - 1; placed = FALSE; }
    PARAMETRIZE { cut = 5 * SECTOR_SIZE; placed = TRUE; }
    PARAMETRIZE { cut = 6 * SECTOR_SIZE + 100; placed = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 999);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    struct Pokemon carried;
    BoxMonAtToMon(0, 0, &carried);
    PCStorageCarrySingle(0, 0, &carried);
    ZeroBoxMonAt(0, 0);
    EXPECT(PCStorageAutosave());
    EXPECT(PCStorageWindow(20));
    // Box 20 spans two PC pages, plus two core pages and the carry page.
    SetBoxMonAt(20, 0, &mon);
    PCStorageCarryEnd();
    PCStorageTestFailure(cut);
    EXPECT(!PCStorageAutosave());
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(placed ? 20 : 0, 0, MON_DATA_PERSONALITY), 999);
    EXPECT_EQ(GetBoxMonDataAt(placed ? 0 : 20, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC streaming dirty wraparound neighbors and carry fit one transaction")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 77);
    SetBoxMonAt(39, 29, &mon);
    SetBoxMonAt(0, 29, &mon);
    SetBoxMonAt(1, 29, &mon);
    PCStorageCarryItem(ITEM_LEFTOVERS);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 3);
    EXPECT(CheckBagHasItem(ITEM_LEFTOVERS, 1));
    TearDownStorage(flash);
}

TEST("PC streaming never uses Hall of Fame sectors for its journal")
{
    u8 *flash = SetUpStorage();
    memset(&gSaveDataBuffer, 0x5A, sizeof(gSaveDataBuffer));
    EXPECT(SaveFlashWriteBuffer(SECTOR_ID_HOF_1));
    EXPECT(SaveFlashWriteBuffer(SECTOR_ID_HOF_2));
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 1);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++) SetBoxMonAt(box, 0, &mon);
    EXPECT(PCStorageAutosave());
    for (u32 sector = SECTOR_ID_HOF_1; sector <= SECTOR_ID_HOF_2; sector++)
    {
        SaveFlashRead(sector, 0, &gSaveDataBuffer, SECTOR_SIZE);
        const u8 *bytes = (const void *)&gSaveDataBuffer;
        for (u32 i = 0; i < SECTOR_SIZE; i++) EXPECT_EQ(bytes[i], 0x5A);
    }
    TearDownStorage(flash);
}

static bool32 PrepareStorageTrade(void)
{
    struct TradeJournalOffer offers[2];
    if (!TradeJournalGetOffer(&offers[0], 0)) return FALSE;
    offers[1] = offers[0];
    offers[1].trainerId = 987;
    memcpy(offers[1].name, COMPOUND_STRING("BOB"), 4);
    offers[1].monHash = 456;
    return TradeJournalPrepare(offers, 0);
}

TEST("PC streaming refuses to reuse the active trade recovery journal")
{
    u8 *flash = SetUpStorage();
    EXPECT(PrepareStorageTrade());
    u32 writes = PCStorageTestWrites();
    EXPECT(TradeJournalNeedsPeer());
    EXPECT(!PCStorageOpen());
    EXPECT(!PCStorageBeginSession());
    EXPECT(!PCStorageAutosave());
    EXPECT(!PCStorageCommit(FALSE));
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(!PCStorageFaulted());
    TearDownStorage(flash);
}

TEST("PC streaming completed trade receipts survive reuse of journal sectors")
{
    bool32 repairHome;
    PARAMETRIZE { repairHome = FALSE; }
    PARAMETRIZE { repairHome = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 1);
    SetBoxMonAt(20, 0, &mon);
    EXPECT(PCStorageAutosave()); // PC pages deliberately have differing generations.
    EXPECT(PrepareStorageTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ABRA, 30, 2, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalFinish());
    if (repairHome) EXPECT(SaveFlashErase(0));
    Restart();
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].box.data.species, SPECIES_ABRA);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT(PCStorageWindow(0));
    // Force a six-page transaction to actually reuse both trade journal pages.
    SetBoxMonAt(39, 29, &mon);
    SetBoxMonAt(0, 0, &mon);
    SetBoxMonAt(1, 0, &mon);
    EXPECT(PCStorageOpen()); // Reuses sectors 30/31 after DONE is in core sector zero.
    struct TradeJournalOffer offer;
    EXPECT(TradeJournalGetOffer(&offer, PARTY_SIZE));
    EXPECT_EQ(offer.phase, TRADE_JOURNAL_DONE);
    EXPECT_EQ(offer.sequence, 1);
    EXPECT_EQ(offer.offeredHash, 456);
    EXPECT(PrepareStorageTrade());
    EXPECT_EQ(gSaveData.linkTrade.sequence, 2);
    TearDownStorage(flash);
}

TEST("PC streaming corrupt backing pages fail closed without overwriting them")
{
    u8 *flash = SetUpStorage();
    u32 page = NUM_CORE_SAVE_SECTORS + 20 * IN_BOX_COUNT * sizeof(struct BoxPokemon) / SECTOR_DATA_SIZE;
    flash[page * SECTOR_SIZE] ^= 1;
    u32 writes = PCStorageTestWrites();
    EXPECT(!PCStorageWindow(20));
    EXPECT(PCStorageFaulted());
    EXPECT(!PCStorageAutosave());
    EXPECT_EQ(PCStorageTestWrites(), writes);
    TearDownStorage(flash);
}

TEST("PC streaming first dirty eviction initializes an unsaved new game's backing")
{
    u8 *flash = SetUpStorage();
    ResetSaveData();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    gSaveData.money = 1234;
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 17);
    SetBoxMonAt(0, 0, &mon);
    SetBoxMonAt(20, 0, &mon); // Eviction performs the initial new-file save.
    EXPECT(!gDifferentSaveFile);
    EXPECT(PCStorageAutosave());
    Restart();
    EXPECT_EQ(gSaveData.money, 1234);
    EXPECT_EQ(CountStorageNonEggMons(), 2);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 17);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_PERSONALITY), 17);
    TearDownStorage(flash);
}

TEST("PC streaming link battles permit read-only scans but cannot save or open the PC")
{
    u8 *flash = SetUpStorage();
    SnapshotLinkBattleState();
    u32 writes = PCStorageTestWrites();
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    EXPECT(!PCStorageOpen());
    EXPECT(!PCStorageAutosave());
    EXPECT(!PCStorageCommit(FALSE));
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(!PCStorageFaulted());
    RestoreLinkBattleState();
    TearDownStorage(flash);
}

TEST("PC streaming interrupted journal reuse cannot regress a durable trade receipt")
{
    bool32 aborted;
    PARAMETRIZE { aborted = FALSE; }
    PARAMETRIZE { aborted = TRUE; }
    u8 *flash = SetUpStorage();
    EXPECT(PrepareStorageTrade());
    EXPECT(TradeJournalReady());
    u32 phase = aborted ? TRADE_JOURNAL_ABORTED : TRADE_JOURNAL_DONE;
    if (aborted)
        EXPECT(TradeJournalAbort());
    else
    {
        EXPECT(TradeJournalCommit());
        EXPECT(TradeJournalFinish());
    }
    // Simulate power loss after erasing only the final receipt, leaving an
    // older pending phase in the other journal sector. ABORTED lives in 30.
    for (u32 sector = SECTOR_ID_TRADE_A; sector <= SECTOR_ID_TRADE_B; sector++)
    {
        SaveFlashRead(sector, 0, &gSaveDataBuffer, SECTOR_SIZE);
        const struct TradeJournalMetadata *meta = (const void *)(gSaveDataBuffer.data + offsetof(struct SaveData, linkTrade));
        if (meta->phase == phase) EXPECT(SaveFlashErase(sector));
    }
    Restart();
    struct TradeJournalMetadata receipt;
    EXPECT(TradeJournalRead(&receipt));
    EXPECT_EQ(receipt.phase, phase);
    EXPECT(!TradeJournalNeedsPeer());
    EXPECT(PrepareStorageTrade());
    EXPECT(TradeJournalRead(&receipt));
    EXPECT_EQ(receipt.sequence, 2);
    EXPECT_EQ(receipt.phase, TRADE_JOURNAL_PREPARED);
    TearDownStorage(flash);
}

TEST("Debug fast fill saves all forty boxes in bounded batches and preserves occupied slots")
{
    bool32 occupied;
    PARAMETRIZE { occupied = FALSE; }
    PARAMETRIZE { occupied = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon original = MakeMon(SPECIES_MEW, 0x12345678);
    if (occupied)
    {
        SetBoxMonAt(0, 0, &original);
        SetBoxMonAt(20, 15, &original);
        SetBoxMonAt(39, 29, &original);
        EXPECT(PCStorageAutosave());
    }
    gSaveData.currentBox = 17;
    EXPECT(PCStorageWindow(17));
    struct DebugPCFill fill = {0};
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_WORKING);
    EXPECT_EQ(fill.nextBox, 0); // Initial save has its own progress frame.
    u32 steps = 1;
    enum DebugPCFillResult result;
    do
    {
        u32 previous = fill.nextBox, writes = PCStorageTestWrites();
        result = DebugPCFillStep(&fill);
        EXPECT(result != DEBUG_PC_FILL_ERROR);
        EXPECT_GT(fill.nextBox, previous);
        EXPECT_LE(fill.nextBox - previous, PC_CACHE_BOXES);
        EXPECT_LE(PCStorageTestWrites() - writes, 10);
        EXPECT_LE(PCStorageTestResidentCount(), 3);
        steps++;
    } while (result == DEBUG_PC_FILL_WORKING && steps < 20);
    EXPECT_EQ(result, DEBUG_PC_FILL_DONE);
    EXPECT_EQ(fill.nextBox, 40);
    EXPECT_EQ(steps, 15);
    u32 writes = PCStorageTestWrites();
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_DONE);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(PCStorageTestResident(16));
    EXPECT(PCStorageTestResident(17));
    EXPECT(PCStorageTestResident(18));

    Restart(); // No extra commit: the final two boxes must already be durable.
    EXPECT_EQ(CountStorageNonEggMons(), 1200);
    EXPECT_EQ(StorageGetCurrentBox(), 17);
    EXPECT(FlagGet(FLAG_SYS_POKEMON_GET));
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            struct BoxPokemon *mon = GetBoxedMonPtr(box, slot);
            if (occupied && ((box == 0 && slot == 0) || (box == 20 && slot == 15) || (box == 39 && slot == 29)))
                EXPECT_EQ(memcmp(mon, &original, sizeof(original)), 0);
            else
            {
                EXPECT(IsSpeciesEnabled(GetBoxMonData(mon, MON_DATA_SPECIES)));
                EXPECT_EQ(GetLevelFromBoxMonExp(mon), 100);
            }
        }
    TearDownStorage(flash);
}

TEST("Debug fast fill stops on failed batch writes without reporting unsaved progress")
{
    u32 cut;
    u32 recoveredBoxes;
    PARAMETRIZE { cut = 0; recoveredBoxes = 3; }
    PARAMETRIZE { cut = 5 * SECTOR_SIZE - 1; recoveredBoxes = 3; }
    PARAMETRIZE { cut = 5 * SECTOR_SIZE; recoveredBoxes = 6; }
    PARAMETRIZE { cut = 6 * SECTOR_SIZE + 100; recoveredBoxes = 6; }
    u8 *flash = SetUpStorage();
    struct DebugPCFill fill = {0};
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_WORKING);
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_WORKING);
    EXPECT_EQ(fill.nextBox, 3);
    PCStorageTestFailure(cut);
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_ERROR);
    EXPECT_EQ(fill.nextBox, 3);
    EXPECT(fill.failed);
    u32 writes = PCStorageTestWrites();
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_ERROR);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), recoveredBoxes * IN_BOX_COUNT);
    TearDownStorage(flash);
}

TEST("Debug fast fill refuses active link sessions before editing any box")
{
    bool32 trade;
    PARAMETRIZE { trade = FALSE; }
    PARAMETRIZE { trade = TRUE; }
    u8 *flash = SetUpStorage();
    if (trade) EXPECT(PrepareStorageTrade());
    else SnapshotLinkBattleState();
    u32 writes = PCStorageTestWrites();
    struct DebugPCFill fill = {0};
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_ERROR);
    EXPECT_EQ(fill.nextBox, 0);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    EXPECT(!PCStorageFaulted());
    if (!trade) RestoreLinkBattleState();
    TearDownStorage(flash);
}

TEST("Debug fast fill of a full PC does not rewrite box pages")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon original = MakeMon(SPECIES_MEW, 12);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++) SetBoxMonAt(box, slot, &original);
    FlagSet(FLAG_SYS_POKEMON_GET);
    EXPECT(PCStorageAutosave());
    struct DebugPCFill fill = {0};
    EXPECT_EQ(DebugPCFillStep(&fill), DEBUG_PC_FILL_WORKING);
    u32 writes = PCStorageTestWrites();
    for (u32 i = 0; i < 14; i++)
        EXPECT_EQ(DebugPCFillStep(&fill), i == 13 ? DEBUG_PC_FILL_DONE : DEBUG_PC_FILL_WORKING);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(GetBoxMonDataAt(39, 29, MON_DATA_PERSONALITY), 12);
    TearDownStorage(flash);
}

TEST("Debug fast fill completes through the emulated cartridge flash driver")
{
    // Unlike the fault-injection fixtures, this exercises the shipping byte
    // programmer and flash timer. The test runner uses its own temporary ROM.
    bool32 flashWasPresent = gFlashMemoryPresent;
    IntrFunc timeoutHandler = gIntrTable[7];
    IntrFunc timer3Handler = gIntrTable[2];
    CheckForFlashMemory();
    // Keep the test runner's timer-2 timeout active while exercising flash.
    gIntrTable[7] = timeoutHandler;
    SetFlashTimerIntr(3, &gIntrTable[2]);
    EXPECT(gFlashMemoryPresent);
    ResetSaveData();
    ZeroPlayerPartyMons();
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    Save_ResetSaveCounters();
    struct DebugPCFill fill = {0};
    for (u32 i = 0; i < 15; i++)
        EXPECT_EQ(DebugPCFillStep(&fill), i == 14 ? DEBUG_PC_FILL_DONE : DEBUG_PC_FILL_WORKING);
    ResetSaveData();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(CountStorageNonEggMons(), 1200);
    EXPECT_EQ(CountMonsInBox(39), 30);
    ResetSaveData();
    gDifferentSaveFile = FALSE;
    gFlashMemoryPresent = flashWasPresent;
    InitFlashTimer();
    gIntrTable[7] = timeoutHandler;
    gIntrTable[2] = timer3Handler;
}

TEST("PC performance cartridge browsing and idle benchmark")
{
    bool32 flashWasPresent = gFlashMemoryPresent;
    IntrFunc timeoutHandler = gIntrTable[7], timer3Handler = gIntrTable[2];
    CheckForFlashMemory();
    gIntrTable[7] = timeoutHandler;
    SetFlashTimerIntr(3, &gIntrTable[2]);
    EXPECT(gFlashMemoryPresent);
    ResetSaveData();
    ZeroPlayerPartyMons();
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    Save_ResetSaveCounters();
    EXPECT(PCStorageOpen());
    u32 writes = PCStorageTestWrites(), reads = PCStorageTestReadBytes();
    u32 erases = PCStorageTestErases();
    u32 start = gMain.vblankCounter1;
    for (u32 box = 1; box <= 2; box++)
    {
        gSaveData.currentBox = box;
        EXPECT(PCStorageWindow(box));
        EXPECT(PCStorageSaveIfRequested());
    }
    u32 browseFrames = gMain.vblankCounter1 - start;
    Test_MgbaPrintf("browse: %d frames, %d writes, %d bytes read", browseFrames,
        PCStorageTestWrites() - writes, PCStorageTestReadBytes() - reads);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_LE(browseFrames, 4);
    writes = PCStorageTestWrites();
    reads = PCStorageTestReadBytes();
    start = gMain.vblankCounter1;
    for (u32 i = 0; i < 1000; i++)
    {
        if (i % 60 == 0) FakeRtc_AdvanceTimeBy(0, 0, 0, 1);
        gSaveData.playTimeVBlanks++;
        EXPECT(PCStorageSaveIfRequested());
    }
    Test_MgbaPrintf("idle: %d frames, %d writes, %d bytes read (fake RTC %d)",
        gMain.vblankCounter1 - start, PCStorageTestWrites() - writes,
        PCStorageTestReadBytes() - reads, OW_USE_FAKE_RTC);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestReadBytes(), reads);
    EXPECT_EQ(PCStorageTestErases(), erases);
    ResetSaveData();
    gDifferentSaveFile = FALSE;
    gFlashMemoryPresent = flashWasPresent;
    InitFlashTimer();
    gIntrTable[7] = timeoutHandler;
    gIntrTable[2] = timer3Handler;
}

TEST("PC clean navigation reads only the incoming box pages and never writes flash")
{
    u8 *flash = SetUpStorage();
    u32 writes = PCStorageTestWrites(), erases = PCStorageTestErases();
    for (u32 i = 1; i <= 2 * TOTAL_BOXES_COUNT; i++)
    {
        u32 reads = PCStorageTestReads(), bytes = PCStorageTestReadBytes();
        gSaveData.currentBox = i % TOTAL_BOXES_COUNT;
        EXPECT(PCStorageWindow(gSaveData.currentBox));
        EXPECT(PCStorageSaveIfRequested());
        EXPECT_LE(PCStorageTestReads() - reads, 2);
        EXPECT_LE(PCStorageTestReadBytes() - bytes, 2 * SECTOR_SIZE);
        EXPECT_EQ(PCStorageTestResidentCount(), 3);
    }
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    TearDownStorage(flash);
}

TEST("PC idle frames do no flash IO even when clocks and the selected box change")
{
    u8 *flash = SetUpStorage();
    u32 reads = PCStorageTestReads(), writes = PCStorageTestWrites(), erases = PCStorageTestErases();
    gSaveData.currentBox = 1;
    for (u32 i = 0; i < 1000; i++)
    {
        FakeRtc_AdvanceTimeBy(0, 0, 0, 1);
        gSaveData.playTimeVBlanks++;
        EXPECT(PCStorageSaveIfRequested());
    }
    EXPECT_EQ(PCStorageTestReads(), reads);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    // Even an explicit check must not turn a selection/clock tick into a save.
    EXPECT(PCStorageAutosave());
    EXPECT_EQ(PCStorageTestWrites(), writes);
    TearDownStorage(flash);
}

TEST("PC event saves run once for actual edits and keep the following frames idle")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 55);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageSaveIfRequested());
    gSaveData.boxWallpapers[39] = 3;
    memcpy(GetBoxNamePtr(39), COMPOUND_STRING("LAST"), 5);
    PCStorageRequestAutosave(); // Naming/summary screens can edit raw pointers.
    EXPECT(PCStorageSaveIfRequested());
    u32 reads = PCStorageTestReads(), writes = PCStorageTestWrites();
    for (u32 i = 0; i < 100; i++) EXPECT(PCStorageSaveIfRequested());
    EXPECT_EQ(PCStorageTestReads(), reads);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 55);
    EXPECT_EQ(gSaveData.boxWallpapers[39], 3);
    EXPECT_EQ(GetBoxNamePtr(39)[0], COMPOUND_STRING("LAST")[0]);
    TearDownStorage(flash);
}

TEST("PC exit persists the browsing position once rather than on every switch")
{
    u8 *flash = SetUpStorage();
    u32 writes = PCStorageTestWrites();
    gSaveData.currentBox = 39;
    EXPECT(PCStorageWindow(39));
    EXPECT(PCStorageSaveIfRequested());
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(PCStorageClose());
    EXPECT_GT(PCStorageTestWrites(), writes);
    writes = PCStorageTestWrites();
    EXPECT(PCStorageClose());
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(StorageGetCurrentBox(), 39);
    TearDownStorage(flash);
}

TEST("PC journal writes erase only their destination pages and the completed seal")
{
    u8 *flash = SetUpStorage();
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    gSaveData.money++;
    EXPECT(PCStorageAutosave());
    EXPECT_EQ(PCStorageTestWrites() - writes, 4); // Two journal + two core pages.
    EXPECT_EQ(PCStorageTestErases() - erases, 5); // No redundant six-page pre-erase.
    TearDownStorage(flash);
}

TEST("PC exit saves elapsed time without background clock saves")
{
    u8 *flash = SetUpStorage();
    u32 writes = PCStorageTestWrites();
    gSaveData.playTimeMinutes = 5;
    EXPECT(PCStorageSaveIfRequested());
    EXPECT(PCStorageAutosave());
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT(PCStorageClose());
    EXPECT_GT(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(gSaveData.playTimeMinutes, 5);
    TearDownStorage(flash);
}

TEST("PC cursor item edits request a save even when only party data changed")
{
    u8 *flash = SetUpStorage();
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    EXPECT(PCStorageCommit(FALSE));
    enum Item none = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &none);
    PCStorageCarryItem(item);
    EXPECT(PCStorageSaveIfRequested());
    Restart();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(CheckBagHasItem(item, 1));
    EXPECT(!CheckBagHasItem(item, 2));
    TearDownStorage(flash);
}

static bool32 DrainMoveSave(void)
{
    for (u32 i = 0; i < 4000 && PCStorageBusy(); i++)
        if (!PCStorageTick()) return FALSE;
    return !PCStorageBusy() && !PCStorageFaulted();
}

static void BeginMoveSession(void)
{
    EXPECT(PCStorageBeginSession());
    EXPECT(DrainMoveSave());
}

static void PickupForTest(u32 box, u32 slot, struct Pokemon *carried)
{
    BoxMonAtToMon(box, slot, carried);
    PCStorageCarrySingle(box, slot, carried);
    ZeroBoxMonAt(box, slot);
}

static void MoveAndSuspendForTest(u32 destBox)
{
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    SetBoxMonAt(destBox, 1, &carried.box);
    PCStorageCarryEnd();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueClose());
    EXPECT(DrainMoveSave());
    EXPECT(PCMoveJournalPending());
    EXPECT(!PCMoveJournalSession());
}

TEST("PC persistent journals reopen without erasing and keep overworld changes durable")
{
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    u32 reads = PCStorageTestReadBytes();
    for (u32 frame = 0; frame < 1000; frame++)
    {
        gSaveData.playTimeVBlanks++;
        EXPECT(PCStorageSaveIfRequested());
    }
    EXPECT_EQ(PCStorageTestReadBytes(), reads);
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites(), counter = gSaveCounter;
    for (u32 visit = 0; visit < 8; visit++)
    {
        // These happened after leaving the PC. Reopening must append them,
        // not replace the durable baseline with this unsaved live state.
        gSaveData.money++;
        EXPECT(PCStorageOpen());
        EXPECT(!PCStorageBusy());
        EXPECT(!PCMoveJournalSession());
        BeginMoveSession();
        EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
        EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
        gSaveData.currentBox = visit;
        EXPECT(PCStorageQueueClose());
        EXPECT(DrainMoveSave());
        EXPECT_EQ(CountStorageNonEggMons(), 1); // Force cache eviction/overlays.
        EXPECT_EQ(PCStorageTestResidentCount(), 3);
    }
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(gSaveCounter, counter);
    gSaveData.money = 9999; // Unsaved overworld play must not leak into recovery.
    Restart();
    EXPECT_EQ(gSaveData.money, 7008);
    EXPECT_EQ(gSaveData.currentBox, 7);
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC persistent reopen captures party damage status mail and bag changes")
{
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_ABRA, 30, 2, OTID_STRUCT_PRESET(2));
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][1]);
    gPartiesCount[B_TRAINER_PLAYER] = 2;
    gParties[B_TRAINER_PLAYER][0].hp -= 7;
    u32 hp = gParties[B_TRAINER_PLAYER][0].hp;
    gParties[B_TRAINER_PLAYER][0].status = STATUS1_POISON;
    gParties[B_TRAINER_PLAYER][0].mail = 2;
    gSaveData.mail[2].words[0] = 321;
    EXPECT(AddBagItem(ITEM_POTION, 3));
    u32 erases = PCStorageTestErases();
    EXPECT(PCStorageOpen());
    EXPECT_EQ(PCStorageTestErases(), erases);
    Restart();
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 2);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].hp, hp);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].status, STATUS1_POISON);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].mail, 2);
    EXPECT_EQ(gSaveData.mail[2].words[0], 321);
    EXPECT(CheckBagHasItem(ITEM_POTION, 3));
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC persistent final record remains reset safe at every publication byte")
{
    u32 cut = 0;
    for (u32 i = 0; i <= 32; i++) PARAMETRIZE { cut = i; }
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    BeginMoveSession();
    gSaveData.money = 8000;
    EXPECT(PCStorageQueueClose());
    EXPECT_EQ(PCMoveJournalTestRecordSize(), 32);
    PCStorageTestFailure(cut);
    EXPECT_EQ(DrainMoveSave(), cut == 32);
    Restart();
    EXPECT_EQ(gSaveData.money, cut == 32 ? 8000 : 7000);
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("Link checkpoints report failed writes and retain the previous durable save")
{
    s32 cut;
    PARAMETRIZE { cut = -1; }
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = SECTOR_SIZE - 1; }
    u8 *flash = SetUpStorage();
    MainCallback callback = gMain.callback2;
    gSaveData.money = 9000;
    PCStorageTestFailure(cut);
    EXPECT_EQ(TrySavingData(SAVE_LINK), cut < 0 ? SAVE_STATUS_OK : SAVE_STATUS_ERROR);
    EXPECT_EQ(PCStorageFaulted(), cut >= 0);
    SetMainCallback2(callback);
    LinkProtocolSetError(LINK_PROTOCOL_DEFAULT);
    Restart();
    EXPECT_EQ(gSaveData.money, cut < 0 ? 9000 : 3000);
    EXPECT_EQ(gSaveData.linkTrade.phase, TRADE_JOURNAL_EMPTY);
    TearDownStorage(flash);
}

TEST("Hall of Fame save failures select a read-only restart screen without wiping data")
{
    s32 failedSector;
    PARAMETRIZE { failedSector = 24; } // First main-save recovery image.
    PARAMETRIZE { failedSector = SECTOR_ID_HOF_1; }
    PARAMETRIZE { failedSector = SECTOR_ID_HOF_2; }
    u8 *flash = SetUpStorage();
    MainCallback callback = gMain.callback2;
    IntrCallback vblank = gMain.vblankCallback;
    IntrCallback hblank = gMain.hblankCallback;
    u8 state = gMain.state;
    bool8 resetDisabled = gSoftResetDisabled;
    gHoFSaveBuffer = AllocZeroed(SECTOR_SIZE * NUM_HOF_SECTORS);
    EXPECT(gHoFSaveBuffer != NULL);
    gSaveData.money = 9000;
    gSoftResetDisabled = TRUE;
    PCStorageTestWriteFailure(failedSector);
    EXPECT(!TrySavingHallOfFame());
    EXPECT_EQ(gMain.callback2, CB2_SaveErrorScreen);
    EXPECT_EQ(gMain.state, 0);
    EXPECT_EQ(gMain.vblankCallback, NULL);
    EXPECT_EQ(gMain.hblankCallback, NULL);
    EXPECT_EQ(gSoftResetDisabled, FALSE);
    EXPECT(gDamagedSaveSectors & (1u << failedSector));
    EXPECT_EQ(PCStorageFaulted(), failedSector == 24);
    // A failed history write must not erase the already completed main save.
    Free(gHoFSaveBuffer);
    gHoFSaveBuffer = NULL;
    SetMainCallback2(callback);
    gMain.state = state;
    SetVBlankCallback(vblank);
    SetHBlankCallback(hblank);
    gSoftResetDisabled = resetDisabled;
    LinkProtocolSetError(LINK_PROTOCOL_DEFAULT);
    PCStorageTestWriteFailure(-1);
    Restart();
    EXPECT_EQ(gSaveData.money, failedSector == 24 ? 3000 : 9000);
    EXPECT_EQ(gSaveData.playerParty[0].data.species, SPECIES_PIKACHU);
    TearDownStorage(flash);
}

TEST("Hall of Fame success persists its history and does not switch to an error screen")
{
    u8 *flash = SetUpStorage();
    MainCallback callback = gMain.callback2;
    gHoFSaveBuffer = AllocZeroed(SECTOR_SIZE * NUM_HOF_SECTORS);
    EXPECT(gHoFSaveBuffer != NULL);
    gHoFSaveBuffer[0].mon[0].species = SPECIES_MEW;
    gSaveData.money = 9000;
    EXPECT(TrySavingHallOfFame());
    EXPECT_EQ(gMain.callback2, callback);
    memset(gHoFSaveBuffer, 0, SECTOR_SIZE * NUM_HOF_SECTORS);
    EXPECT_EQ(LoadGameSave(SAVE_HALL_OF_FAME), SAVE_STATUS_OK);
    EXPECT_EQ((u32)gHoFSaveBuffer[0].mon[0].species, SPECIES_MEW);
    Free(gHoFSaveBuffer);
    gHoFSaveBuffer = NULL;
    Restart();
    EXPECT_EQ(gSaveData.money, 9000);
    EXPECT_EQ(GetHallOfFameEntryCount(), 1);
    TearDownStorage(flash);
}

TEST("Hall of Fame missing-buffer errors do not claim success or write flash")
{
    u8 *flash = SetUpStorage();
    MainCallback callback = gMain.callback2;
    IntrCallback vblank = gMain.vblankCallback;
    IntrCallback hblank = gMain.hblankCallback;
    u8 state = gMain.state;
    bool8 resetDisabled = gSoftResetDisabled;
    gHoFSaveBuffer = NULL;
    u32 writes = PCStorageTestWrites(), erases = PCStorageTestErases();
    EXPECT(!TrySavingHallOfFame());
    EXPECT_EQ(gDamagedSaveSectors, 0);
    EXPECT_EQ(gMain.callback2, CB2_SaveErrorScreen);
    EXPECT_EQ(GetHallOfFameEntryCount(), 0);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    SetMainCallback2(callback);
    gMain.state = state;
    SetVBlankCallback(vblank);
    SetHBlankCallback(hblank);
    gSoftResetDisabled = resetDisabled;
    TearDownStorage(flash);
}

TEST("Save error and missing-flash screens perform no recovery writes or erases")
{
    bool32 missingFlash;
    PARAMETRIZE { missingFlash = FALSE; }
    PARAMETRIZE { missingFlash = TRUE; }
    u8 *flash = SetUpStorage();
    MainCallback callback = gMain.callback2;
    IntrCallback vblank = gMain.vblankCallback;
    IntrCallback hblank = gMain.hblankCallback;
    u8 state = gMain.state;
    u16 keys = gMain.newKeys;
    MainCallback screen = missingFlash ? CB2_FlashNotDetectedScreen : CB2_SaveErrorScreen;
    u32 writes = PCStorageTestWrites(), erases = PCStorageTestErases();
    u32 programmed = PCStorageTestProgramBytes();
    gMain.newKeys = 0;
    SetMainCallback2(screen);
    for (u32 i = 0; i < 3; i++)
        screen();
    EXPECT_EQ(gMain.callback2, screen);
    EXPECT_EQ(gMain.state, 1);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestProgramBytes(), programmed);
    FreeAllWindowBuffers();
    UnsetBgTilemapBuffer(0);
    SetMainCallback2(callback);
    gMain.state = state;
    gMain.newKeys = keys;
    SetVBlankCallback(vblank);
    SetHBlankCallback(hblank);
    TearDownStorage(flash);
}

TEST("PC persistent logs drain before normal and link saves without losing evicted box edits")
{
    u8 saveType;
    PARAMETRIZE { saveType = SAVE_NORMAL; }
    PARAMETRIZE { saveType = SAVE_LINK; }
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    struct BoxPokemon other = MakeMon(SPECIES_ABRA, 77);
    SetBoxMonAt(39, 29, &other);
    gSaveData.money = 9000;
    EXPECT_EQ(TrySavingData(saveType), SAVE_STATUS_OK);
    EXPECT(!PCMoveJournalPending());
    EXPECT(!PCMoveJournalSession());
    Restart();
    EXPECT_EQ(gSaveData.money, 9000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(GetBoxMonDataAt(39, 29, MON_DATA_PERSONALITY), 77);
    EXPECT_EQ(CountStorageNonEggMons(), 2);
    TearDownStorage(flash);
}

TEST("PC persistent logs survive interrupted drain and subsequent normal save")
{
    u32 cut;
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = 1; }
    PARAMETRIZE { cut = 2 * SECTOR_SIZE - 1; }
    PARAMETRIZE { cut = 2 * SECTOR_SIZE; }
    PARAMETRIZE { cut = 6 * SECTOR_SIZE - 1; }
    PARAMETRIZE { cut = 6 * SECTOR_SIZE; }
    PARAMETRIZE { cut = 8 * SECTOR_SIZE - 1; }
    PARAMETRIZE { cut = 8 * SECTOR_SIZE; }
    PARAMETRIZE { cut = 10 * SECTOR_SIZE - 1; }
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    gSaveData.money = 9000;
    PCStorageTestFailure(cut);
    EXPECT(!PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(gSaveData.money, cut >= 8 * SECTOR_SIZE ? 9000 : 7000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC persistent journals cannot be displaced by a trade without its checkpoint")
{
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    EXPECT(!PrepareStorageTrade());
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT(!PCMoveJournalPending());
    EXPECT(PrepareStorageTrade());
    Restart();
    EXPECT(TradeJournalNeedsPeer());
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC persistent journals stay read only during link battles")
{
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    SnapshotLinkBattleState();
    u32 erases = PCStorageTestErases(), bytes = PCStorageTestProgramBytes();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT(!PCStorageOpen());
    EXPECT(!PCStorageBeginSession());
    EXPECT(!PCStorageQueueClose());
    EXPECT(!PCStorageCommit(FALSE));
    EXPECT(PCMoveJournalPending());
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestProgramBytes(), bytes);
    RestoreLinkBattleState();
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(20, 1, MON_DATA_PERSONALITY), 919);
    TearDownStorage(flash);
}

TEST("PC persistent exit resumes correctly when its final record rolls over the journal")
{
    u8 *flash = SetUpStorage();
    BeginMoveSession();
    for (u32 i = 0; i < 128; i++)
    {
        gSaveData.money++;
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(DrainMoveSave());
    }
    u32 erases = PCStorageTestErases();
    gSaveData.playTimeMinutes = 5;
    EXPECT(PCStorageQueueClose());
    EXPECT(DrainMoveSave());
    EXPECT_GT(PCStorageTestErases(), erases);
    EXPECT(!PCMoveJournalSession());
    EXPECT(PCMoveJournalPending());
    EXPECT(PCStorageOpen());
    Restart();
    EXPECT_EQ(gSaveData.money, 3128);
    EXPECT_EQ(gSaveData.playTimeMinutes, 5);
    TearDownStorage(flash);
}

TEST("PC persistent journals permit an explicit new game overwrite without replaying old edits")
{
    u8 *flash = SetUpStorage();
    MoveAndSuspendForTest(20);
    ResetSaveData();
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    SetBagItemsPointers();
    gDifferentSaveFile = TRUE;
    gSaveData.money = 1234;
    EXPECT(PCStorageOpen());
    Restart();
    EXPECT_EQ(gSaveData.money, 1234);
    EXPECT_EQ(CountStorageNonEggMons(), 0);
    TearDownStorage(flash);
}

TEST("PC persistent journal cartridge exit benchmark")
{
    u32 scenario;
    PARAMETRIZE { scenario = 0; } // Browse/clock only.
    PARAMETRIZE { scenario = 1; } // Move inside one page.
    PARAMETRIZE { scenario = 2; } // Move across distant pages.
    PARAMETRIZE { scenario = 3; } // Return to the original slot.
    bool32 flashWasPresent = gFlashMemoryPresent;
    IntrFunc timeoutHandler = gIntrTable[7], timer3Handler = gIntrTable[2];
    CheckForFlashMemory();
    gIntrTable[7] = timeoutHandler;
    SetFlashTimerIntr(3, &gIntrTable[2]);
    EXPECT(gFlashMemoryPresent);
    ResetSaveData();
    ZeroPlayerPartyMons();
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    Save_ResetSaveCounters();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(TRUE));
    BeginMoveSession();
    u32 destBox = scenario == 2 ? 20 : 0, destSlot = scenario == 3 ? 0 : 1;
    if (scenario)
    {
        struct Pokemon carried;
        PickupForTest(0, 0, &carried);
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(DrainMoveSave());
        SetBoxMonAt(destBox, destSlot, &carried.box);
        PCStorageCarryEnd();
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(DrainMoveSave());
    }
    gSaveData.playTimeMinutes = 5;
    u32 erases = PCStorageTestErases(), bytes = PCStorageTestProgramBytes();
    u32 start = gMain.vblankCounter1, ticks = 0;
    EXPECT(PCStorageQueueClose());
    while (PCStorageBusy() && ticks < 1000)
    {
        EXPECT(PCStorageTick());
        VBlankIntrWait(); // One callback per rendered frame, as in the PC.
        ticks++;
    }
    u32 frames = gMain.vblankCounter1 - start;
    Test_MgbaPrintf("exit %d: %d frames, %d ticks, %d erases, %d programmed bytes", scenario,
        frames, ticks, PCStorageTestErases() - erases, PCStorageTestProgramBytes() - bytes);
    EXPECT_EQ(PCStorageTestProgramBytes() - bytes, 32);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_LE(frames, 8);
    EXPECT_LE(ticks, 2);
    EXPECT(PCMoveJournalPending());
    EXPECT(!PCMoveJournalSession());
    EXPECT(!PCStorageBusy());
    gSaveData.money = 7000;
    start = gMain.vblankCounter1;
    bytes = PCStorageTestProgramBytes();
    EXPECT(PCStorageOpen());
    u32 reopenFrames = gMain.vblankCounter1 - start;
    Test_MgbaPrintf("reopen %d: %d frames, %d erases, %d programmed bytes", scenario,
        reopenFrames, PCStorageTestErases() - erases, PCStorageTestProgramBytes() - bytes);
    EXPECT_LE(reopenFrames, 10);
    EXPECT_LE(PCStorageTestProgramBytes() - bytes, 128);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT(!PCMoveJournalSession());
    ResetSaveData();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(gSaveData.money, 7000);
    EXPECT_EQ(gSaveData.playTimeMinutes, 5);
    EXPECT_EQ(GetBoxMonDataAt(scenario ? destBox : 0, scenario ? destSlot : 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    ResetSaveData();
    gDifferentSaveFile = FALSE;
    gFlashMemoryPresent = flashWasPresent;
    InitFlashTimer();
    gIntrTable[7] = timeoutHandler;
    gIntrTable[2] = timer3Handler;
}

TEST("PC checkpoint skips pages restored to their original contents")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    SetBoxMonAt(0, 0, &carried.box);
    PCStorageCarryEnd();
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    u32 erases = PCStorageTestErases(), bytes = PCStorageTestProgramBytes();
    EXPECT(PCMoveJournalClose());
    EXPECT(DrainMoveSave());
    EXPECT_EQ(PCStorageTestProgramBytes(), bytes);
    EXPECT_EQ(PCStorageTestErases() - erases, 1); // Retire the log only.
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC checkpoint page skipping never trusts a colliding sector checksum")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    u16 checksum = SaveSectorChecksum(&mon, sizeof(mon));
    mon.personality++;
    mon.otId--;
    EXPECT_EQ(SaveSectorChecksum(&mon, sizeof(mon)), checksum);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    u32 bytes = PCStorageTestProgramBytes();
    EXPECT(PCMoveJournalClose());
    EXPECT(DrainMoveSave());
    EXPECT_EQ(PCStorageTestProgramBytes() - bytes, 2 * SECTOR_SIZE);
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 920);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC checkpoint recovers after reset at every tick and shadow handoff")
{
    u32 tick = 0;
    for (u32 i = 0; i <= 64; i++) PARAMETRIZE { tick = i; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    SetBoxMonAt(20, 0, &carried.box);
    PCStorageCarryEnd();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT(PCMoveJournalClose());
    for (u32 i = 0; i < tick && PCStorageBusy(); i++) EXPECT(PCStorageTick());
    Restart();
    EXPECT_EQ(gSaveData.money, 7000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC compact preparation can be interrupted without changing the previous save")
{
    u32 cut;
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = 1; }
    PARAMETRIZE { cut = 12; }
    PARAMETRIZE { cut = 15; }
    PARAMETRIZE { cut = 16; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    EXPECT(PCStorageBeginSession());
    PCStorageTestFailure(cut);
    EXPECT_EQ(DrainMoveSave(), cut == 16);
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC compact sessions cannot flush or append during a link battle")
{
    u8 *flash = SetUpStorage();
    BeginMoveSession();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    SnapshotLinkBattleState();
    u32 bytes = PCStorageTestProgramBytes(), erases = PCStorageTestErases();
    EXPECT(!PCStorageBeginSession());
    EXPECT(!PCStorageQueueSave(FALSE));
    EXPECT(!PCStorageQueueClose());
    EXPECT(!PCStorageAutosave());
    EXPECT(!PCStorageCommit(FALSE));
    EXPECT_EQ(PCStorageTestProgramBytes(), bytes);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT(!PCStorageFaulted());
    RestoreLinkBattleState();
    Restart();
    EXPECT_EQ(gSaveData.money, 7000);
    TearDownStorage(flash);
}

TEST("PC compact checkpoints preserve completed trade receipts and Hall of Fame")
{
    u8 *flash = SetUpStorage();
    EXPECT(PrepareStorageTrade());
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalFinish());
    EXPECT(PCStorageOpen());
    memset(&gSaveDataBuffer, 0x5A, sizeof(gSaveDataBuffer));
    EXPECT(SaveFlashWriteBuffer(SECTOR_ID_HOF_1));
    EXPECT(SaveFlashWriteBuffer(SECTOR_ID_HOF_2));
    BeginMoveSession();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT(PCStorageQueueClose());
    EXPECT(DrainMoveSave());
    Restart();
    struct TradeJournalOffer offer;
    EXPECT(TradeJournalGetOffer(&offer, PARTY_SIZE));
    EXPECT_EQ(offer.phase, TRADE_JOURNAL_DONE);
    EXPECT_EQ(offer.sequence, 1);
    EXPECT_EQ(gSaveData.money, 7000);
    for (u32 sector = SECTOR_ID_HOF_1; sector <= SECTOR_ID_HOF_2; sector++)
    {
        SaveFlashRead(sector, 0, &gSaveDataBuffer, SECTOR_SIZE);
        const u8 *bytes = (const void *)&gSaveDataBuffer;
        for (u32 i = 0; i < SECTOR_SIZE; i++) EXPECT_EQ(bytes[i], 0x5A);
    }
    TearDownStorage(flash);
}

TEST("PC compact cartridge moves finish in small bounded write steps")
{
    bool32 flashWasPresent = gFlashMemoryPresent;
    IntrFunc timeoutHandler = gIntrTable[7], timer3Handler = gIntrTable[2];
    CheckForFlashMemory();
    gIntrTable[7] = timeoutHandler;
    SetFlashTimerIntr(3, &gIntrTable[2]);
    EXPECT(gFlashMemoryPresent);
    ResetSaveData();
    ZeroPlayerPartyMons();
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    gDifferentSaveFile = TRUE;
    Save_ResetSaveCounters();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(TRUE));
    BeginMoveSession();
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    struct Pokemon carried;
    for (u32 move = 0; move < 2; move++)
    {
        u32 start = gMain.vblankCounter1, bytes = PCStorageTestProgramBytes();
        if (!move) PickupForTest(0, 0, &carried);
        else { SetBoxMonAt(0, 1, &carried.box); PCStorageCarryEnd(); }
        EXPECT(PCStorageQueueSave(FALSE));
        u32 staging = gMain.vblankCounter1 - start, steps = 0, maximum = 0;
        while (PCStorageBusy() && steps < 100)
        {
            start = gMain.vblankCounter1;
            EXPECT(PCStorageTick());
            maximum = max(maximum, gMain.vblankCounter1 - start);
            steps++;
        }
        Test_MgbaPrintf("compact move %d: staging %d frames, %d ticks, max tick %d frames, %d bytes",
            move, staging, steps, maximum, PCStorageTestProgramBytes() - bytes);
        EXPECT_LE(staging, 2);
        EXPECT_LE(maximum, 2);
        EXPECT_LE(steps, 3);
        EXPECT_LE(PCStorageTestProgramBytes() - bytes, 512);
        EXPECT_EQ(PCStorageTestErases(), erases);
        EXPECT_EQ(PCStorageTestWrites(), writes);
    }
    ResetSaveData();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(0, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    ResetSaveData();
    gDifferentSaveFile = FALSE;
    gFlashMemoryPresent = flashWasPresent;
    InitFlashTimer();
    gIntrTable[7] = timeoutHandler;
    gIntrTable[2] = timer3Handler;
}

TEST("PC compact full-box moves fit without falling back to a synchronous save")
{
    bool32 placed;
    PARAMETRIZE { placed = FALSE; }
    PARAMETRIZE { placed = TRUE; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon *group = Alloc(sizeof(*group) * IN_BOX_COUNT);
    EXPECT(group != NULL);
    for (u32 i = 0; i < IN_BOX_COUNT; i++)
    {
        group[i] = MakeMon(SPECIES_MEW, i + 1);
        SetBoxMonAt(39, i, &group[i]);
    }
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    PCStorageCarryGroup(39, 0, IN_BOX_COLUMNS, IN_BOX_ROWS, group);
    for (u32 i = 0; i < IN_BOX_COUNT; i++) ZeroBoxMonAt(39, i);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(PCStorageBusy());
    EXPECT(DrainMoveSave());
    if (placed)
    {
        EXPECT(PCStorageWindow(13));
        for (u32 i = 0; i < IN_BOX_COUNT; i++) SetBoxMonAt(13, i, &group[i]);
        PCStorageCarryEnd();
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(PCStorageBusy());
        EXPECT(DrainMoveSave());
    }
    EXPECT(PCMoveJournalSession());
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Free(group);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 30);
    for (u32 i = 0; i < IN_BOX_COUNT; i++)
        EXPECT_EQ(GetBoxMonDataAt(placed ? 13 : 39, i, MON_DATA_PERSONALITY), i + 1);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 30);
    TearDownStorage(flash);
}

TEST("PC compact item and metadata edits survive reset without duplicating cursor items")
{
    u8 *flash = SetUpStorage();
    enum Item item = ITEM_LEFTOVERS, none = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &none);
    PCStorageCarryItem(item);
    gSaveData.boxWallpapers[39] = 3;
    memcpy(GetBoxNamePtr(39), COMPOUND_STRING("LAST"), 5);
    PCStorageRequestAutosave();
    EXPECT(PCStorageSaveIfRequested());
    EXPECT(DrainMoveSave());
    Restart();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(CheckBagHasItem(item, 1));
    EXPECT(!CheckBagHasItem(item, 2));
    EXPECT_EQ(gSaveData.boxWallpapers[39], 3);
    EXPECT_EQ(GetBoxNamePtr(39)[0], COMPOUND_STRING("LAST")[0]);
    Restart();
    EXPECT(!CheckBagHasItem(item, 2));
    TearDownStorage(flash);
}

TEST("PC compact sealed record corruption fails closed")
{
    u8 *flash = SetUpStorage();
    BeginMoveSession();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    // Clear a set bit in the first sealed record's CRC, retaining its seal.
    u8 crc[4];
    SaveFlashRead(24, 32 + 12, crc, sizeof(crc));
    for (u32 i = 0; i < sizeof(crc); i++)
        if (crc[i]) { crc[i] &= crc[i] - 1; break; }
    EXPECT(SaveFlashProgram(24, 32 + 12, crc, sizeof(crc)));
    ResetSaveData();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_CORRUPT);
    TearDownStorage(flash);
}

TEST("PC compact moves program a small record without erasing or rewriting home sectors")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites(), bytes = PCStorageTestProgramBytes();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(PCStorageBusy());
    EXPECT(DrainMoveSave());
    EXPECT_LE(PCStorageTestProgramBytes() - bytes, 512);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    SetBoxMonAt(0, 1, &carried.box);
    PCStorageCarryEnd();
    bytes = PCStorageTestProgramBytes();
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT_LE(PCStorageTestProgramBytes() - bytes, 512);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(0, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC compact swaps preserve the original recovery hole across distant boxes")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon first = MakeMon(SPECIES_MEW, 1), second = MakeMon(SPECIES_ABRA, 2);
    SetBoxMonAt(0, 0, &first);
    SetBoxMonAt(20, 0, &second);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried, displaced;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    BoxMonAtToMon(20, 0, &displaced);
    SetBoxMonAt(20, 0, &carried.box);
    carried = displaced;
    PCStorageCarrySingle(20, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_ABRA);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT_EQ(CountStorageNonEggMons(), 2);
    TearDownStorage(flash);
}

TEST("PC compact direct distant deposits seal only small records before eviction")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    // Deliberately no queued save between pickup and the distant lookup.
    SetBoxMonAt(20, 0, &carried.box);
    PCStorageCarryEnd();
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT(PCMoveJournalSession());
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    Restart();
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC compact records overlay evicted boxes without checkpoint stalls")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT(PCStorageWindow(20));
    SetBoxMonAt(20, 0, &carried.box);
    PCStorageCarryEnd();
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    u32 erases = PCStorageTestErases(), writes = PCStorageTestWrites();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(PCStorageTestErases(), erases);
    EXPECT_EQ(PCStorageTestWrites(), writes);
    EXPECT_EQ(PCStorageTestResidentCount(), 3);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT_EQ(GetBoxMonDataAt(20, 0, MON_DATA_PERSONALITY), 919);
    TearDownStorage(flash);
}

TEST("PC compact pickup records recover a carried Pokemon once after reset")
{
    bool32 party;
    PARAMETRIZE { party = FALSE; }
    PARAMETRIZE { party = TRUE; }
    u8 *flash = SetUpStorage();
    struct Pokemon carried;
    if (party)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_ABRA, 30, 2, OTID_STRUCT_PRESET(2));
        gPartiesCount[B_TRAINER_PLAYER] = 2;
        gParties[B_TRAINER_PLAYER][0].hp -= 7;
        gParties[B_TRAINER_PLAYER][0].status = STATUS1_POISON;
        gParties[B_TRAINER_PLAYER][0].mail = 2;
        gSaveData.mail[2].words[0] = 321;
    }
    else
    {
        struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
        SetBoxMonAt(39, 29, &mon);
    }
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    if (party)
    {
        carried = gParties[B_TRAINER_PLAYER][0];
        PCStorageCarrySingle(TOTAL_BOXES_COUNT, 0, &carried);
        ZeroMonData(&gParties[B_TRAINER_PLAYER][0]);
    }
    else PickupForTest(39, 29, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    Restart();
    if (party)
    {
        EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 2);
        EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].hp, carried.hp);
        EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].status, carried.status);
        EXPECT_EQ(gParties[B_TRAINER_PLAYER][1].mail, 2);
        EXPECT_EQ(gSaveData.mail[2].words[0], 321);
    }
    else EXPECT_EQ(GetBoxMonDataAt(39, 29, MON_DATA_PERSONALITY), 919);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), party ? 0 : 1);
    TearDownStorage(flash);
}

TEST("PC compact torn records retain the previous commit and sealed records replay")
{
    u32 cutKind;
    PARAMETRIZE { cutKind = 0; }
    PARAMETRIZE { cutKind = 1; }
    PARAMETRIZE { cutKind = 2; }
    PARAMETRIZE { cutKind = 3; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    u32 length = PCMoveJournalTestRecordSize();
    u32 cut = cutKind == 0 ? 0 : cutKind == 1 ? 17 : cutKind == 2 ? length - 1 : length;
    PCStorageTestFailure(cut);
    EXPECT_EQ(DrainMoveSave(), cutKind == 3);
    Restart();
    EXPECT_EQ(gSaveData.money, cutKind == 3 ? 7000 : 3000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC compact page folding survives resets during shadow and home writes")
{
    u32 cut;
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = 1; }
    PARAMETRIZE { cut = SECTOR_SIZE - 1; }
    PARAMETRIZE { cut = SECTOR_SIZE; }
    PARAMETRIZE { cut = SECTOR_SIZE + 1; }
    PARAMETRIZE { cut = 2 * SECTOR_SIZE - 1; }
    PARAMETRIZE { cut = 2 * SECTOR_SIZE; }
    PARAMETRIZE { cut = 3 * SECTOR_SIZE + 100; }
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    PickupForTest(0, 0, &carried);
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    SetBoxMonAt(0, 1, &carried.box);
    PCStorageCarryEnd();
    gSaveData.money = 7000;
    EXPECT(PCStorageQueueSave(FALSE));
    EXPECT(DrainMoveSave());
    EXPECT(PCMoveJournalClose());
    PCStorageTestFailure(cut);
    EXPECT(!DrainMoveSave());
    Restart();
    EXPECT_EQ(gSaveData.money, 7000);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(0, 1, MON_DATA_PERSONALITY), 919);
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    TearDownStorage(flash);
}

TEST("PC persistent journal rolls over across repeated distant moves and sessions")
{
    u8 *flash = SetUpStorage();
    struct BoxPokemon mon = MakeMon(SPECIES_MEW, 919);
    SetBoxMonAt(0, 0, &mon);
    EXPECT(PCStorageCommit(FALSE));
    BeginMoveSession();
    struct Pokemon carried;
    for (u32 i = 0; i < 100; i++)
    {
        PickupForTest(i & 1 ? 20 : 0, 0, &carried);
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(DrainMoveSave());
        SetBoxMonAt(i & 1 ? 0 : 20, 0, &carried.box);
        PCStorageCarryEnd();
        EXPECT(PCStorageQueueSave(FALSE));
        EXPECT(DrainMoveSave());
        EXPECT_EQ(PCStorageTestResidentCount(), 3);
        EXPECT(PCStorageQueueClose());
        EXPECT(DrainMoveSave());
        EXPECT(PCMoveJournalPending());
        EXPECT(!PCMoveJournalSession());
        EXPECT(PCStorageOpen());
        BeginMoveSession();
    }
    EXPECT(PCStorageCommit(FALSE));
    Restart();
    EXPECT_EQ(CountStorageNonEggMons(), 1);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_PERSONALITY), 919);
    TearDownStorage(flash);
}
