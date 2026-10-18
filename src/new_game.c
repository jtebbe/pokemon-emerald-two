#include "global.h"
#include "clock.h"
#include "new_game.h"
#include "random.h"
#include "clock.h"
#include "pokemon.h"
#include "script.h"
#include "lottery_corner.h"
#include "play_time.h"
#include "load_save.h"
#include "berry.h"
#include "rtc.h"
#include "easy_chat.h"
#include "event_data.h"
#include "money.h"
#include "coins.h"
#include "text.h"
#include "overworld.h"
#include "mail.h"
#include "item.h"
#include "pokedex.h"
#include "pokedex.h"
#include "save.h"
#include "link_rfu.h"
#include "main.h"
#include "item_menu.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "player_pc.h"
#include "field_specials.h"
#include "union_room_chat.h"
#include "constants/maps.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "difficulty.h"
#include "follower_npc.h"


static void ResetItemFlags(void);
static void ResetDexNav(void);

EWRAM_DATA bool8 gDifferentSaveFile = FALSE;


void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

u32 GetTrainerId(u8 *trainerId)
{
    return (trainerId[3] << 24) | (trainerId[2] << 16) | (trainerId[1] << 8) | (trainerId[0]);
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < TRAINER_ID_LENGTH; i++)
        dst[i] = src[i];
}

static void InitPlayerTrainerId(void)
{
    u32 trainerId = (Random() << 16) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveData.playerTrainerId);
}

// L=A isnt set here for some reason.
static void SetDefaultOptions(void)
{
    gSaveData.optionsTextSpeed = OPTIONS_TEXT_SPEED_MID;
    gSaveData.optionsWindowFrameType = 0;
    gSaveData.optionsSound = OPTIONS_SOUND_MONO;
    gSaveData.optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    gSaveData.optionsBattleSceneOff = FALSE;
    gSaveData.optionsLevelCaps = FALSE;
    gSaveData.optionsTrainerScaling = FALSE;
}

static void ClearPokedexFlags(void)
{
    memset(&gSaveData.dexCaught, 0, sizeof(gSaveData.dexCaught));
    memset(&gSaveData.dexSeen, 0, sizeof(gSaveData.dexSeen));
}





void ResetSaveDataAndOptions(void)
{
    ResetSaveData();
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = FALSE;
    ResetPokedexScrollPositions();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagScrollPositions();
}

void NewGameInitData(void)
{
#if IS_FRLG
    u8 rivalName[PLAYER_NAME_LENGTH + 1];
#endif
    if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
        RtcReset();

#if IS_FRLG
    StringCopy(rivalName, gSaveData.rivalName);
#endif
    gDifferentSaveFile = TRUE;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokedex();
    ResetGameProgress();
    ClearAllMail();
    gSaveData.specialSaveWarpFlags = 0;
    InitPlayerTrainerId();
    PlayTimeCounter_Reset();
    ClearPokedexFlags();
    InitEventData();
    SetMoney(&gSaveData.money, 3000);
    SetCoins(0);
    gPartiesCount[B_TRAINER_PLAYER] = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    gSaveData.registeredItem = ITEM_NONE;
    ClearBag();
    NewGameInitPCItems();
    UpdateDailySeed();
    SetLastHealLocationWarp(HEAL_LOCATION_OUTDOOR);
    SetWarpDestinationToHealLocation(HEAL_LOCATION_OUTDOOR);
    WarpIntoMap();
    FlagSet(FLAG_RECEIVED_RUNNING_SHOES);
    FlagSet(FLAG_SYS_POKEDEX_GET);
#if IS_FRLG
        StringCopy(gSaveData.rivalName, rivalName);
#endif
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    ResetItemFlags();
    ResetDexNav();
    ClearFollowerNPCData();
}


static void ResetItemFlags(void)
{
#if OW_SHOW_ITEM_DESCRIPTIONS == OW_ITEM_DESCRIPTIONS_FIRST_TIME
    memset(&gSaveData.itemFlags, 0, sizeof(gSaveData.itemFlags));
#endif
}

static void ResetDexNav(void)
{
#if USE_DEXNAV_SEARCH_LEVELS == TRUE
    memset(gSaveData.dexNavSearchLevels, 0, sizeof(gSaveData.dexNavSearchLevels));
#endif
    gSaveData.dexNavChain = 0;
}
