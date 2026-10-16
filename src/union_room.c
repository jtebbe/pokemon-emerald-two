#include "global.h"
#include "malloc.h"
#include "battle.h"
#include "bg.h"
#include "cable_club.h"
#include "data.h"
#include "decompress.h"
#include "dynamic_placeholder_text_util.h"
#include "easy_chat.h"
#include "event_data.h"
#include "event_object_lock.h"
#include "fieldmap.h"
#include "field_control_avatar.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "international_string_util.h"
#include "librfu.h"
#include "link.h"
#include "link_trade.h"
#include "link_protocol.h"
#include "link_rfu.h"
#include "list_menu.h"
#include "load_save.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "random.h"
#include "save_location.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "start_menu.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "trade.h"
#include "trainer_card.h"
#include "union_room.h"
#include "union_room_battle.h"
#include "union_room_chat.h"
#include "union_room_player_avatar.h"
#include "window.h"
#include "constants/cable_club.h"
#include "constants/party_menu.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// States for Task_RunUnionRoom
enum {
    UR_STATE_INIT,
    UR_STATE_INIT_OBJECTS,
    UR_STATE_INIT_LINK,
    UR_STATE_CHECK_SELECTING_MON,
    UR_STATE_MAIN,
    UR_STATE_DO_SOMETHING_PROMPT,
    UR_STATE_HANDLE_DO_SOMETHING_PROMPT_INPUT,
    UR_STATE_DO_SOMETHING_PROMPT_2,
    UR_STATE_PRINT_MSG,
    UR_STATE_HANDLE_ACTIVITY_REQUEST,
    UR_STATE_DECLINE_ACTIVITY_REQUEST,
    UR_STATE_PLAYER_CONTACTED_YOU,
    UR_STATE_RECV_CONTACT_DATA,
    UR_STATE_PRINT_START_ACTIVITY_MSG,
    UR_STATE_START_ACTIVITY_LINK,
    UR_STATE_START_ACTIVITY_WAIT_FOR_LINK,
    UR_STATE_START_ACTIVITY_FREE_UROOM,
    UR_STATE_START_ACTIVITY_FADE,
    UR_STATE_START_ACTIVITY,
    UR_STATE_RECV_JOIN_CHAT_REQUEST,
    UR_STATE_TRY_ACCEPT_CHAT_REQUEST_DELAY,
    UR_STATE_TRY_ACCEPT_CHAT_REQUEST,
    UR_STATE_ACCEPT_CHAT_REQUEST,
    UR_STATE_WAIT_FOR_START_MENU,
    UR_STATE_INTERACT_WITH_PLAYER,
    UR_STATE_TRY_COMMUNICATING,
    UR_STATE_PRINT_AND_EXIT,
    UR_STATE_SEND_ACTIVITY_REQUEST,
    UR_STATE_TRAINER_APPEARS_BUSY,
    UR_STATE_WAIT_FOR_RESPONSE_TO_REQUEST,
    UR_STATE_CANCEL_ACTIVITY_LINK_ERROR,
    UR_STATE_SEND_TRADE_REQUST,
    UR_STATE_REQUEST_DECLINED,
    UR_STATE_PRINT_CONTACT_MSG,
    UR_STATE_HANDLE_CONTACT_DATA,
    UR_STATE_RECV_ACTIVITY_REQUEST,
    UR_STATE_CANCEL_REQUEST_PRINT_MSG,
    UR_STATE_CANCEL_REQUEST_RESTART_LINK,
    UR_STATE_COMMUNICATING_WAIT_FOR_DATA,
    UR_STATE_WAIT_FOR_CONTACT_DATA,
    UR_STATE_PRINT_CARD_INFO,
    UR_STATE_WAIT_FINISH_READING_CARD,
    UR_STATE_INTERACT_WITH_ATTENDANT,
    UR_STATE_REGISTER_PROMPT,
    UR_STATE_CANCEL_REGISTRATION_PROMPT,
    UR_STATE_CHECK_TRADING_BOARD,
    UR_STATE_TRADING_BOARD_LOAD,
    UR_STATE_REGISTER_PROMPT_HANDLE_INPUT,
    UR_STATE_TRADING_BOARD_HANDLE_INPUT,
    UR_STATE_TRADE_PROMPT,
    UR_STATE_TRADE_SELECT_MON,
    UR_STATE_TRADE_OFFER_MON,
    UR_STATE_REGISTER_REQUEST_TYPE,
    UR_STATE_REGISTER_SELECT_MON_FADE,
    UR_STATE_REGISTER_SELECT_MON,
    UR_STATE_REGISTER_COMPLETE,
    UR_STATE_CANCEL_REGISTRATION,
};

// States for sUnionRoomTrade.state
enum {
    URTRADE_STATE_NONE,
    URTRADE_STATE_REGISTERING,
    URTRADE_STATE_OFFERING,
};

// States for Task_TryBecomeLinkLeader
enum {
    LL_STATE_INIT,
    LL_STATE_INIT2 = 3,
    LL_STATE_GET_AWAITING_PLAYERS_TEXT,
    LL_STATE_PRINT_AWAITING_PLAYERS,
    LL_STATE_AWAIT_PLAYERS,
    LL_STATE_ACCEPT_NEW_MEMBER_PROMPT,
    LL_STATE_WAIT_DISCONNECT_CHILD = 9,
    LL_STATE_MEMBER_LEFT,
    LL_STATE_ACCEPT_NEW_MEMBER_PROMPT_HANDLE_INPUT,
    LL_STATE_UPDATE_AFTER_JOIN_REQUEST,
    LL_STATE_ACCEPTED_FINAL_MEMBER,
    LL_STATE_WAIT_AND_CONFIRM_MEMBERS,
    LL_STATE_MEMBERS_OK_PROMPT,
    LL_STATE_MEMBERS_OK_PROMPT_HANDLE_INPUT,
    LL_STATE_CONFIRMED_MEMBERS,
    LL_STATE_FINAL_MEMBER_CHECK,
    LL_STATE_CANCEL_PROMPT,
    LL_STATE_CANCEL_PROMPT_HANDLE_INPUT,
    LL_STATE_SHUTDOWN_AND_RETRY,
    LL_STATE_RETRY,
    LL_STATE_SHUTDOWN_AND_FAIL,
    LL_STATE_FAILED,
    LL_STATE_TRY_START_ACTIVITY = 26,
    LL_STATE_MEMBER_DISCONNECTED = 29,
    LL_STATE_CANCEL_WITH_MSG
};

// States for Task_TryJoinLinkGroup
enum {
    LG_STATE_INIT,
    LG_STATE_CHOOSE_LEADER_MSG,
    LG_STATE_INIT_WINDOWS,
    LG_STATE_CHOOSE_LEADER_HANDLE_INPUT,
    LG_STATE_ASK_JOIN_GROUP = 5,
    LG_STATE_MAIN,
    LG_STATE_ASK_LEAVE_GROUP,
    LG_STATE_ASK_LEAVE_GROUP_HANDLE_INPUT,
    LG_STATE_WAIT_LEAVE_GROUP,
    LG_STATE_CANCEL_CHOOSE_LEADER,
    LG_STATE_CANCELED,
    LG_STATE_RFU_ERROR,
    LG_STATE_RFU_ERROR_SHUTDOWN,
    LG_STATE_DISCONNECTED,
    LG_STATE_RETRY_CONNECTION,
    LG_STATE_TRADE_NOT_READY = 18,
    LG_STATE_TRADE_NOT_READY_RETRY,
    LG_STATE_READY_START_ACTIVITY,
    LG_STATE_START_ACTIVITY,
    LG_STATE_SHUTDOWN = 23,
};

// Color types for PrintUnionRoomText
enum {
    UR_COLOR_DEFAULT,
    UR_COLOR_RED,
    UR_COLOR_GREEN,
    UR_COLOR_WHITE,
    UR_COLOR_CANCEL,
    UR_COLOR_TRADE_BOARD_SELF,
    UR_COLOR_TRADE_BOARD_OTHER,
};

// Return values for HandlePlayerListUpdate
enum {
    PLIST_NONE,
    PLIST_NEW_PLAYER,
    PLIST_RECENT_UPDATE,
    PLIST_UNUSED,
    PLIST_CONTACTED,
};

EWRAM_DATA u8 gPlayerCurrActivity = 0;
static EWRAM_DATA union
{
    struct WirelessLink_Leader *leader;
    struct WirelessLink_Group *group;
    struct WirelessLink_URoom *uRoom;
} sWirelessLinkMain = {};
EWRAM_DATA struct RfuGameCompatibilityData gRfuPartnerCompatibilityData = {};
EWRAM_DATA enum Species gUnionRoomOfferedSpecies = SPECIES_NONE;
EWRAM_DATA enum Type gUnionRoomRequestedMonType = TYPE_NONE;
static EWRAM_DATA struct UnionRoomTrade sUnionRoomTrade = {};

static struct WirelessLink_Group *sGroup;

static void PrintUnionRoomText(u8, u8, const u8 *, u8, u8, u8);
static void Task_ListenToWireless(u8);
static void ClearIncomingPlayerList(struct RfuIncomingPlayerList *, u8);
static void ClearRfuPlayerList(struct RfuPlayer *, u8);
static u8 CreateTask_ListenForCompatiblePartners(struct RfuIncomingPlayerList *, u32);
static void CopyAndTranslatePlayerName(u8 *, struct RfuPlayer *);
static void PrintGroupCandidateOnWindow(u8, u8, u8, struct RfuPlayer *, u8, u8 );
static u32 GetNewIncomingPlayerId(struct RfuPlayer *, struct RfuIncomingPlayer *);
static u8 TryAddIncomingPlayerToList(struct RfuPlayer *, struct RfuIncomingPlayer *, u8);
static u8 GetNewLeaderCandidate(void);
static void PrintGroupMemberOnWindow(u8, u8, u8, struct RfuPlayer *, u8, u8);
static bool32 ArePlayerDataDifferent(struct RfuPlayerData *, struct RfuPlayerData *);
static void ResetUnionRoomTrade(struct UnionRoomTrade *);
static bool8 ArePlayersDifferent(struct RfuPlayerData *, const struct RfuPlayerData *);
static void ItemPrintFunc_PossibleGroupMembers(u8, u32, u8);
static void ListMenuItemPrintFunc_UnionRoomGroups(u8, u32, u8);
static void TradeBoardListMenuItemPrintFunc(u8, u32, u8);
static void ItemPrintFunc_EmptyList(u8, u32, u8);

#include "data/union_room.h"












static void ItemPrintFunc_PossibleGroupMembers(u8 windowId, u32 id, u8 y)
{
    struct WirelessLink_Leader *data = sWirelessLinkMain.leader;
    u8 colorIdx = UR_COLOR_DEFAULT;

    switch (data->playerList->players[id].groupScheduledAnim)
    {
    case UNION_ROOM_SPAWN_IN:
        if (data->playerList->players[id].newPlayerCountdown != 0)
            colorIdx = UR_COLOR_GREEN;
        break;
    case UNION_ROOM_SPAWN_OUT:
        colorIdx = UR_COLOR_RED;
        break;
    }

    PrintGroupCandidateOnWindow(windowId, 0, y, &data->playerList->players[id], colorIdx, id);
}







u8 CreateTask_ListenToWireless(void)
{
    u8 taskId;
    struct WirelessLink_Group *data;

    taskId = CreateTask(Task_ListenToWireless, 0);
    sWirelessLinkMain.group = data = (void *)(gTasks[taskId].data);

    data->state = 0;
    data->textState = 0;

    sGroup = data;

    return taskId;
}

static void Task_ListenToWireless(u8 taskId)
{
    struct WirelessLink_Group *data = sWirelessLinkMain.group;

    switch (data->state)
    {
    case 0:
        SetHostRfuGameData(ACTIVITY_NONE, 0, FALSE);
        SetWirelessCommType1();
        OpenLink();
        InitializeRfuLinkManager_JoinGroup();
        RfuSetIgnoreError(TRUE);
        data->incomingPlayerList = AllocZeroed(RFU_CHILD_MAX * sizeof(struct RfuIncomingPlayer));
        data->playerList = AllocZeroed(MAX_RFU_PLAYER_LIST_SIZE * sizeof(struct RfuPlayer));
        data->state = 2;
        break;
    case 2:
        ClearIncomingPlayerList(data->incomingPlayerList, RFU_CHILD_MAX);
        ClearRfuPlayerList(data->playerList->players, MAX_RFU_PLAYER_LIST_SIZE);
        data->listenTaskId = CreateTask_ListenForCompatiblePartners(data->incomingPlayerList, 0xFF);
        data->leaderId = 0;
        data->state = 3;
        break;
    case 3:
        if (GetNewLeaderCandidate() == 1)
            PlaySE(SE_PC_LOGIN);
        if (gTasks[taskId].data[15] == 0xFF)
            data->state = 10;
        break;
    case 10:
        DestroyTask(data->listenTaskId);
        Free(data->playerList);
        Free(data->incomingPlayerList);
        LinkRfu_Shutdown();
        data->state++;
        break;
    case 11:
        LinkRfu_Shutdown();
        DestroyTask(taskId);
        break;
    }
}

bool32 IsUnionRoomActivityAcceptable(u32 activity, u32 linkGroup)
{
    // Retired activity slots must never be accepted, even during a broad scan.
    if ((activity & ~IN_UNION_ROOM) == ACTIVITY_UNUSED_14
        || (activity & ~IN_UNION_ROOM) == ACTIVITY_UNUSED_15
        || (activity & ~IN_UNION_ROOM) == ACTIVITY_UNUSED_28)
        return FALSE;
    if (linkGroup == 0xFF)
        return TRUE;

    if (linkGroup < ARRAY_COUNT(sAcceptedActivityIds) && sAcceptedActivityIds[linkGroup] != NULL)
    {
        const u8 *bytes = sAcceptedActivityIds[linkGroup];

        while ((*(bytes) != 0xFF))
        {
            if ((*bytes) == activity)
                return TRUE;
            bytes++;
        }
    }

    return FALSE;
}

static u8 GetGroupListTextColor(struct WirelessLink_Group *data, u32 id)
{
    if (data->playerList->players[id].groupScheduledAnim == UNION_ROOM_SPAWN_IN)
    {
        if (data->playerList->players[id].rfu.data.startedActivity)
            return UR_COLOR_WHITE;
        else if (data->playerList->players[id].useRedText)
            return UR_COLOR_RED;
        else if (data->playerList->players[id].newPlayerCountdown != 0)
            return UR_COLOR_GREEN;
    }
    return UR_COLOR_DEFAULT;
}

static void ListMenuItemPrintFunc_UnionRoomGroups(u8 windowId, u32 id, u8 y)
{
    struct WirelessLink_Group *data = sWirelessLinkMain.group;
    u8 colorId = GetGroupListTextColor(data, id);

    PrintGroupMemberOnWindow(windowId, 8, y, &data->playerList->players[id], colorId, id);
}

static u8 GetNewLeaderCandidate(void)
{
    struct WirelessLink_Group *data = sWirelessLinkMain.group;
    u8 ret = 0;
    u8 i;
    s32 id;

    for (i = 0; i < MAX_RFU_PLAYER_LIST_SIZE; i++)
    {
        if (data->playerList->players[i].groupScheduledAnim != UNION_ROOM_SPAWN_NONE)
        {
            id = GetNewIncomingPlayerId(&data->playerList->players[i], data->incomingPlayerList->players);
            if (id != 0xFF)
            {
                if (data->playerList->players[i].groupScheduledAnim == UNION_ROOM_SPAWN_IN)
                {
                    if (ArePlayerDataDifferent(&data->playerList->players[i].rfu, &data->incomingPlayerList->players[id].rfu))
                    {
                        data->playerList->players[i].rfu = data->incomingPlayerList->players[id].rfu;
                        data->playerList->players[i].newPlayerCountdown = 64;
                        ret = 1;
                    }
                    else
                    {
                        if (data->playerList->players[i].newPlayerCountdown != 0)
                        {
                            data->playerList->players[i].newPlayerCountdown--;
                            if (data->playerList->players[i].newPlayerCountdown == 0)
                                ret = 2;
                        }
                    }
                }
                else
                {
                    data->playerList->players[i].groupScheduledAnim = UNION_ROOM_SPAWN_IN;
                    data->playerList->players[i].newPlayerCountdown = 64;
                    ret = 1;
                }

                data->playerList->players[i].timeoutCounter = 0;
            }
            else
            {
                if (data->playerList->players[i].groupScheduledAnim != UNION_ROOM_SPAWN_OUT)
                {
                    data->playerList->players[i].timeoutCounter++;
                    if (data->playerList->players[i].timeoutCounter >= 300)
                    {
                        data->playerList->players[i].groupScheduledAnim = UNION_ROOM_SPAWN_OUT;
                        ret = 2;
                    }
                }
            }
        }
    }

    for (id = 0; id < RFU_CHILD_MAX; id++)
    {
        if (TryAddIncomingPlayerToList(data->playerList->players, &data->incomingPlayerList->players[id], MAX_RFU_PLAYER_LIST_SIZE) != 0xFF)
            ret = 1;
    }

    return ret;
}

static void Task_CreateTradeMenu(u8 taskId)
{
    CB2_StartCreateTradeMenu();
    DestroyTask(taskId);
}

u8 CreateTask_CreateTradeMenu(void)
{
    return CreateTask(Task_CreateTradeMenu, 0);
}




void StartUnionRoomBattle(u16 battleFlags)
{
    SnapshotLinkBattleState();
    HealPlayerParty();
    gLinkPlayers[0].linkType = LINKTYPE_BATTLE;
    gLinkPlayers[GetMultiplayerId()].id = GetMultiplayerId();
    gLinkPlayers[GetMultiplayerId() ^ 1].id = GetMultiplayerId() ^ 1;
    gMain.savedCallback = CB2_ReturnFromCableClubBattle;
    gBattleTypeFlags = battleFlags;
    PlayBattleBGM();
}





























void SetUsingUnionRoomStartMenu(void)
{
}












static void Task_ListenForCompatiblePartners(u8 taskId)
{
    s32 i, j;
    struct RfuIncomingPlayerList **list = (void *) gTasks[taskId].data;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        Rfu_GetCompatiblePlayerData(&list[0]->players[i].rfu.data, list[0]->players[i].rfu.name, i);
        if (!IsUnionRoomActivityAcceptable(list[0]->players[i].rfu.data.activity, gTasks[taskId].data[2]))
        {
            list[0]->players[i].rfu = sUnionRoomPlayer_DummyRfu;
        }
        for (j = 0; j < i; j++)
        {
            if (!ArePlayersDifferent(&list[0]->players[j].rfu, &list[0]->players[i].rfu))
                list[0]->players[i].rfu = sUnionRoomPlayer_DummyRfu;
        }
        list[0]->players[i].active = ArePlayersDifferent(&list[0]->players[i].rfu, &sUnionRoomPlayer_DummyRfu);
    }
}





static u8 CreateTask_ListenForCompatiblePartners(struct RfuIncomingPlayerList *list, u32 linkGroup)
{
    u8 taskId = CreateTask(Task_ListenForCompatiblePartners, 0);
    struct RfuIncomingPlayerList **oldList = (void *) gTasks[taskId].data;
    oldList[0] = list;
    gTasks[taskId].data[2] = linkGroup;
    return taskId;
}













static void PrintUnionRoomText(u8 windowId, u8 fontId, const u8 *str, u8 x, u8 y, u8 colorIdx)
{
    struct TextPrinterTemplate printerTemplate;

    printerTemplate.currentChar = str;
    printerTemplate.type = WINDOW_TEXT_PRINTER;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = fontId;
    printerTemplate.x = x;
    printerTemplate.y = y;
    printerTemplate.currentX = x;
    printerTemplate.currentY = y;

    gTextFlags.useAlternateDownArrow = FALSE;
    switch (colorIdx)
    {
    case UR_COLOR_DEFAULT:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_COLOR_WHITE;
        printerTemplate.color.foreground = TEXT_COLOR_DARK_GRAY;
        printerTemplate.color.background = TEXT_COLOR_WHITE;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_GRAY;
        break;
    case UR_COLOR_RED:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_COLOR_WHITE;
        printerTemplate.color.foreground = TEXT_COLOR_RED;
        printerTemplate.color.background = TEXT_COLOR_WHITE;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_RED;
        break;
    case UR_COLOR_GREEN:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_COLOR_WHITE;
        printerTemplate.color.foreground = TEXT_COLOR_GREEN;
        printerTemplate.color.background = TEXT_COLOR_WHITE;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_GREEN;
        break;
    case UR_COLOR_WHITE:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_COLOR_WHITE;
        printerTemplate.color.foreground = TEXT_COLOR_WHITE;
        printerTemplate.color.background = TEXT_COLOR_WHITE;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_GRAY;
        break;
    case UR_COLOR_CANCEL:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_COLOR_DARK_GRAY;
        printerTemplate.color.foreground = TEXT_COLOR_WHITE;
        printerTemplate.color.background = TEXT_COLOR_DARK_GRAY;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_GRAY;
        break;
    case UR_COLOR_TRADE_BOARD_SELF:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_DYNAMIC_COLOR_6;
        printerTemplate.color.foreground = TEXT_COLOR_LIGHT_GREEN;
        printerTemplate.color.background = TEXT_DYNAMIC_COLOR_6;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_BLUE;
        break;
    case UR_COLOR_TRADE_BOARD_OTHER:
        printerTemplate.letterSpacing = 0;
        printerTemplate.lineSpacing = 0;
        printerTemplate.color.accent = TEXT_DYNAMIC_COLOR_6;
        printerTemplate.color.foreground = TEXT_DYNAMIC_COLOR_5;
        printerTemplate.color.background = TEXT_DYNAMIC_COLOR_6;
        printerTemplate.color.shadow = TEXT_COLOR_LIGHT_BLUE;
        break;
    }

    AddTextPrinter(&printerTemplate, TEXT_SKIP_DRAW, NULL);
}

static void ClearRfuPlayerList(struct RfuPlayer *players, u8 count)
{
    s32 i;

    for (i = 0; i < count; i++)
    {
        players[i].rfu = sUnionRoomPlayer_DummyRfu;
        players[i].timeoutCounter = 255;
        players[i].groupScheduledAnim = UNION_ROOM_SPAWN_NONE;
        players[i].useRedText = FALSE;
        players[i].newPlayerCountdown = 0;
    }
}

static void ClearIncomingPlayerList(struct RfuIncomingPlayerList *list, u8 count)
{
    s32 i;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        list->players[i].rfu = sUnionRoomPlayer_DummyRfu;
        list->players[i].active = FALSE;
    }
}

// Checks player name and trainer id, returns TRUE if they are not the same
static bool8 ArePlayersDifferent(struct RfuPlayerData *player1, const struct RfuPlayerData *player2)
{
    s32 i;

    for (i = 0; i < 2; i++)
    {
        if (player1->data.compatibility.playerTrainerId[i] != player2->data.compatibility.playerTrainerId[i])
            return TRUE;
    }

    for (i = 0; i < PLAYER_NAME_LENGTH + 1; i++)
    {
        if (player1->name[i] != player2->name[i])
            return TRUE;
    }

    return FALSE;
}

static bool32 ArePlayerDataDifferent(struct RfuPlayerData *player1, struct RfuPlayerData *player2)
{
    s32 i;

    if (player1->data.activity != player2->data.activity)
        return TRUE;

    if (player1->data.startedActivity != player2->data.startedActivity)
        return TRUE;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        if (player1->data.partnerInfo[i] != player2->data.partnerInfo[i])
            return TRUE;
    }

    if (player1->data.tradeSpecies != player2->data.tradeSpecies)
        return TRUE;

    if (player1->data.tradeType != player2->data.tradeType)
        return TRUE;

    return FALSE;
}

static u32 GetNewIncomingPlayerId(struct RfuPlayer *player, struct RfuIncomingPlayer *incomingPlayer)
{
    u8 result = 0xFF; // None
    s32 i;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        if (incomingPlayer[i].active && !ArePlayersDifferent(&player->rfu, &incomingPlayer[i].rfu))
        {
            result = i;
            incomingPlayer[i].active = FALSE;
        }
    }

    return result;
}

static u8 TryAddIncomingPlayerToList(struct RfuPlayer *players, struct RfuIncomingPlayer *incomingPlayer, u8 max)
{
    s32 i;

    if (incomingPlayer->active)
    {
        for (i = 0; i < max; i++)
        {
            if (players[i].groupScheduledAnim == UNION_ROOM_SPAWN_NONE)
            {
                players[i].rfu = incomingPlayer->rfu;
                players[i].timeoutCounter = 0;
                players[i].groupScheduledAnim = UNION_ROOM_SPAWN_IN;
                players[i].newPlayerCountdown = 64;
                incomingPlayer->active = FALSE;
                return i;
            }
        }
    }

    return 0xFF;
}

static void PrintGroupMemberOnWindow(u8 windowId, u8 x, u8 y, struct RfuPlayer *player, u8 colorIdx, u8 id)
{
    u8 activity;
    u8 trainerId[6];

    ConvertIntToDecimalStringN(gStringVar4, id + 1, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar4, sText_Colon);
    PrintUnionRoomText(windowId, FONT_NORMAL, gStringVar4, x, y, UR_COLOR_DEFAULT);
    x += 18;
    activity = player->rfu.data.activity;
    if (player->groupScheduledAnim == UNION_ROOM_SPAWN_IN && !(activity & IN_UNION_ROOM))
    {
        CopyAndTranslatePlayerName(gStringVar4, player);
        PrintUnionRoomText(windowId, FONT_NORMAL, gStringVar4, x, y, colorIdx);
        ConvertIntToDecimalStringN(trainerId, player->rfu.data.compatibility.playerTrainerId[0] | (player->rfu.data.compatibility.playerTrainerId[1] << 8), STR_CONV_MODE_LEADING_ZEROS, 5);
        StringCopy(gStringVar4, sText_ID);
        StringAppend(gStringVar4, trainerId);
        PrintUnionRoomText(windowId, FONT_NORMAL, gStringVar4, GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x88), y, colorIdx);
    }
}

static void PrintGroupCandidateOnWindow(u8 windowId, u8 x, u8 y, struct RfuPlayer *player, u8 colorIdx, u8 id)
{
    u8 trainerId[6];

    if (player->groupScheduledAnim == UNION_ROOM_SPAWN_IN)
    {
        CopyAndTranslatePlayerName(gStringVar4, player);
        PrintUnionRoomText(windowId, FONT_NORMAL, gStringVar4, x, y, colorIdx);
        ConvertIntToDecimalStringN(trainerId, player->rfu.data.compatibility.playerTrainerId[0] | (player->rfu.data.compatibility.playerTrainerId[1] << 8), STR_CONV_MODE_LEADING_ZEROS, 5);
        StringCopy(gStringVar4, sText_ID);
        StringAppend(gStringVar4, trainerId);
        PrintUnionRoomText(windowId, FONT_NORMAL, gStringVar4, GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0x68), y, colorIdx);
    }
}






static void ItemPrintFunc_EmptyList(u8 windowId, u32 itemId, u8 y)
{
}

static void TradeBoardPrintItemInfo(u8 windowId, u8 y, struct RfuGameData *data, const u8 *playerName, u8 colorIdx)
{
    u8 levelStr[4];
    enum Species species = data->tradeSpecies;
    enum Type type = data->tradeType;
    u8 level = data->tradeLevel;

    PrintUnionRoomText(windowId, FONT_NORMAL, playerName, 8, y, colorIdx);
    if (species == SPECIES_EGG)
    {
        PrintUnionRoomText(windowId, FONT_NORMAL, sText_EggTrade, 68, y, colorIdx);
    }
    else
    {
        BlitMenuInfoIcon(windowId, type + 1, 68, y);
        PrintUnionRoomText(windowId, FONT_NORMAL, GetSpeciesName(species), 118, y, colorIdx);
        ConvertIntToDecimalStringN(levelStr, level, STR_CONV_MODE_RIGHT_ALIGN, 3);
        PrintUnionRoomText(windowId, FONT_NORMAL, levelStr, 198, y, colorIdx);
    }
}

static void TradeBoardListMenuItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    struct WirelessLink_Leader *leader = sWirelessLinkMain.leader;
    struct RfuGameData *gameData;
    s32 i, j;
    u8 playerName[RFU_USER_NAME_LENGTH + 1];

    if (itemId == LIST_HEADER && y == sTradeBoardListMenuTemplate.upText_Y)
    {
        gameData = GetHostRfuGameData();
        if (gameData->tradeSpecies != SPECIES_NONE)
            TradeBoardPrintItemInfo(windowId, y, gameData, gSaveData.playerName, UR_COLOR_TRADE_BOARD_SELF);
    }
    else
    {
        j = 0;
        for (i = 0; i < MAX_UNION_ROOM_LEADERS; i++)
        {
            if (leader->playerList->players[i].groupScheduledAnim == UNION_ROOM_SPAWN_IN && leader->playerList->players[i].rfu.data.tradeSpecies != SPECIES_NONE)
                j++;

            if (j == itemId + 1)
            {
                CopyAndTranslatePlayerName(playerName, &leader->playerList->players[i]);
                TradeBoardPrintItemInfo(windowId, y, &leader->playerList->players[i].rfu.data, playerName, UR_COLOR_TRADE_BOARD_OTHER);
                break;
            }
        }
    }
}










static void ResetUnionRoomTrade(struct UnionRoomTrade *trade)
{
    trade->state = URTRADE_STATE_NONE;
    trade->type = 0;
    trade->playerPersonality = 0;
    trade->playerSpecies = SPECIES_NONE;
    trade->playerLevel = 0;
    trade->species = SPECIES_NONE;
    trade->level = 0;
    trade->personality = 0;
}

void Script_ResetUnionRoomTrade(void)
{
    ResetUnionRoomTrade(&sUnionRoomTrade);
}








static void CopyAndTranslatePlayerName(u8 *dest, struct RfuPlayer *player)
{
    StringCopy_PlayerName(dest, player->rfu.name);
    ConvertInternationalString(dest, player->rfu.data.compatibility.language);
}
