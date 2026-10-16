#include "global.h"
#include "malloc.h"
#include "fake_rtc.h"
#include "follower_npc.h"
#include "item.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pc_storage.h"
#include "random.h"
#include "save_location.h"
#include "script_pokemon_util.h"
#include "wild_encounter_ow.h"
#include "gba/flash_internal.h"
#include "agb_flash.h"
#include "event_data.h"
#include "constants/event_objects.h"

struct LinkInventorySnapshot
{
    struct Bag bag;
    struct Mail mail[MAIL_COUNT];
};

// Resident persistent data; boxed Pokemon are streamed through PCStorage.
EWRAM_DATA struct SaveData gSaveData = {0};

static EWRAM_DATA struct LinkInventorySnapshot sLinkInventorySnapshot = {0};
static EWRAM_DATA bool8 sLinkBattleActive = FALSE;

// IWRAM common
COMMON_DATA bool32 gFlashMemoryPresent = 0;

// code
void CheckForFlashMemory(void)
{
    if (!IdentifyFlash())
    {
        gFlashMemoryPresent = TRUE;
        InitFlashTimer();
    }
    else
    {
        gFlashMemoryPresent = FALSE;
    }
}









void ResetSaveData(void)
{
    sLinkBattleActive = FALSE;
    PCStorageReset(TRUE);
    memset(&gSaveData, 0, sizeof(gSaveData));
    FakeRtc_Reset();
}

void ResetGameProgress(void)
{
    sLinkBattleActive = FALSE;
    PCStorageReset(TRUE);
    // Preserve player identity, options, and clock settings chosen before starting.
    memset(&gSaveData.pos, 0, sizeof(gSaveData) - offsetof(struct SaveData, pos));
    FakeRtc_Reset();
}

void ResetHeapPreservingSaveData(void)
{
    void *vblankCB, *hblankCB;

    // save interrupt functions and turn them off
    vblankCB = gMain.vblankCallback;
    hblankCB = gMain.hblankCallback;
    gMain.vblankCallback = NULL;
    gMain.hblankCallback = NULL;

    // The save data no longer moves through the heap.
    InitHeap(gHeap, HEAP_SIZE);
    SetBagItemsPointers();

    // restore interrupt functions
    gMain.hblankCallback = hblankCB;
    gMain.vblankCallback = vblankCB;

}

u32 UseContinueGameWarp(void)
{
    return gSaveData.specialSaveWarpFlags & CONTINUE_GAME_WARP;
}

void ClearContinueGameWarpStatus(void)
{
    gSaveData.specialSaveWarpFlags &= ~CONTINUE_GAME_WARP;
}

void SetContinueGameWarpStatus(void)
{
    gSaveData.specialSaveWarpFlags |= CONTINUE_GAME_WARP;
}

void SetContinueGameWarpStatusToDynamicWarp(void)
{
    SetContinueGameWarpToDynamicWarp(0);
    gSaveData.specialSaveWarpFlags |= CONTINUE_GAME_WARP;
}

void ClearContinueGameWarpStatus2(void)
{
    gSaveData.specialSaveWarpFlags &= ~CONTINUE_GAME_WARP;
}

void SavePlayerParty(void)
{
    int i;
    *GetSavedPlayerPartyCount() = gPartiesCount[B_TRAINER_PLAYER];

    for (i = 0; i < PARTY_SIZE; i++)
        SavePlayerPartyMon(i, &gParties[B_TRAINER_PLAYER][i]);
}

void LoadPlayerParty(void)
{
    gPartiesCount[B_TRAINER_PLAYER] = *GetSavedPlayerPartyCount();
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetBoxMonData(GetSavedPlayerPartyMon(i), MON_DATA_SPECIES) == SPECIES_NONE)
            ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
        else
        {
            BoxMonToMon(GetSavedPlayerPartyMon(i), &gParties[B_TRAINER_PLAYER][i]);
            gParties[B_TRAINER_PLAYER][i].mail = gSaveData.playerPartyMail[i];
        }
    }
}

void SaveObjectEvents(void)
{
    for (u32 i = 0; i < OBJECT_EVENTS_COUNT; i++)
        gSaveData.objectEvents[i] = gObjectEvents[i];
}

void LoadObjectEvents(void)
{
    for (u32 i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        gObjectEvents[i] = gSaveData.objectEvents[i];
        gObjectEvents[i].spriteId = 0;
    }
    SetMinimumOWESpawnTimer();
}

void CopyPartyAndObjectsToSave(void)
{
    SavePlayerParty();
    SaveObjectEvents();
}

void CopyPartyAndObjectsFromSave(void)
{
    LoadPlayerParty();
    LoadObjectEvents();
}

void SnapshotLinkInventory(void)
{
    int i;

    // Snapshot inventory so link battles can restore temporary item consumption.
    memcpy(&sLinkInventorySnapshot.bag, &gSaveData.bag, sizeof(struct Bag));

    // load mail.
    for (i = 0; i < MAIL_COUNT; i++)
        sLinkInventorySnapshot.mail[i] = gSaveData.mail[i];

}

void RestoreLinkInventory(void)
{
    int i;
    // Restore the link-session inventory snapshot.
    memcpy(&gSaveData.bag, &sLinkInventorySnapshot.bag, sizeof(struct Bag));

    // save mail.
    for (i = 0; i < MAIL_COUNT; i++)
        gSaveData.mail[i] = sLinkInventorySnapshot.mail[i];

}

void SnapshotLinkBattleState(void)
{
    if (sLinkBattleActive)
        return;
    SavePlayerParty();
    SnapshotLinkInventory();
    sLinkBattleActive = TRUE;
}

void RestoreLinkBattleState(void)
{
    if (!sLinkBattleActive)
        return;
    LoadPlayerParty();
    RestoreLinkInventory();
    sLinkBattleActive = FALSE;
}

bool32 LinkBattleStateActive(void)
{
    return sLinkBattleActive;
}
