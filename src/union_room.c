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
static EWRAM_DATA u8 sPlayerActivityGroupSize = 0;
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

static struct WirelessLink_Leader *sLeader;
static struct WirelessLink_Group *sGroup;
static struct WirelessLink_URoom *sURoom;

static void PrintUnionRoomText(u8, u8, const u8 *, u8, u8, u8);
static u16 ReadAsU16(const u8 *);
static void Task_ListenToWireless(u8);
static void ClearIncomingPlayerList(struct RfuIncomingPlayerList *, u8);
static void ClearRfuPlayerList(struct RfuPlayer *, u8);
static u8 CreateTask_ListenForCompatiblePartners(struct RfuIncomingPlayerList *, u32);
static bool8 PrintOnTextbox(u8 *, const u8 *);
static bool8 Leader_SetStateIfMemberListChanged(struct WirelessLink_Leader *, u32, u32);
static u8 LeaderPrunePlayerList(struct RfuPlayerList *);
static s8 UnionRoomHandleYesNo(u8 *, bool32);
static void CopyAndTranslatePlayerName(u8 *, struct RfuPlayer *);
static void Leader_DestroyResources(struct WirelessLink_Leader *);
static u8 LeaderUpdateGroupMembership(struct RfuPlayerList *);
static void PrintGroupCandidateOnWindow(u8, u8, u8, struct RfuPlayer *, u8, u8 );
static u32 GetNewIncomingPlayerId(struct RfuPlayer *, struct RfuIncomingPlayer *);
static u8 TryAddIncomingPlayerToList(struct RfuPlayer *, struct RfuIncomingPlayer *, u8);
static u8 GetNewLeaderCandidate(void);
static u32 IsTryingToTradeAcrossVersionTooSoon(struct WirelessLink_Group *, s32);
static void AskToJoinRfuGroup(struct WirelessLink_Group *, s32);
static void JoinGroup_EnableScriptContexts(void);
static void PrintGroupMemberOnWindow(u8, u8, u8, struct RfuPlayer *, u8, u8);
static bool32 ArePlayerDataDifferent(struct RfuPlayerData *, struct RfuPlayerData *);
static u32 GetPartyPositionOfRegisteredMon(struct UnionRoomTrade *, u8);
static void ResetUnionRoomTrade(struct UnionRoomTrade *);
static u8 CreateTask_SearchForChildOrParent(struct RfuIncomingPlayerList *, struct RfuIncomingPlayerList *, u32);
static bool32 RegisterTradeMonAndGetIsEgg(u32, struct UnionRoomTrade *);
static void RegisterTradeMon(u32, struct UnionRoomTrade *);
static void StartScriptInteraction(void);
static bool32 IsPlayerFacingTradingBoard(void);
static u8 HandlePlayerListUpdate(void);
static bool32 PollPartnerYesNoResponse(struct WirelessLink_URoom *);
static void ReceiveUnionRoomActivityPacket(struct WirelessLink_URoom *);
static u8 GetActivePartnersInfo(struct WirelessLink_URoom *);
static bool32 HandleContactFromOtherPlayer(struct WirelessLink_URoom *);
static bool32 UR_RunTextPrinters(void);
static s32 GetUnionRoomPlayerGender(s32, struct RfuPlayerList *);
static s32 UnionRoomGetPlayerInteractionResponse(struct RfuPlayerList *, u8, u8, u32);
static void HandleCancelActivity(bool32);
static s32 ListMenuHandler_AllItemsAvailable(u8 *, u8 *, u8 *, const struct WindowTemplate *, const struct ListMenuTemplate *);
static s32 TradeBoardMenuHandler(u8 *, u8 *, u8 *, u8 *, const struct WindowTemplate *, const struct ListMenuTemplate *, struct RfuPlayerList *);
static s32 GetIndexOfNthTradeBoardOffer(struct RfuPlayer *, s32);
static bool32 HasAtLeastTwoMonsOfLevel30OrLower(void);
static u32 GetResponseIdx_InviteToURoomActivity(s32);
static void ViewURoomPartnerTrainerCard(u8 *, struct WirelessLink_URoom *, bool8);
static void GetURoomActivityRejectMsg(u8 *, s32, u32);
static void GetURoomActivityStartMsg(u8 *, u8);
static void UR_ClearBg0(void);
static s32 IsRequestedTradeInPlayerParty(enum Type, enum Species);
static bool32 UR_PrintFieldMessage(const u8 *);
static s32 GetChatLeaderActionRequestMessage(u8 *, u32, u16 *, struct WirelessLink_URoom *);
static bool8 ArePlayersDifferent(struct RfuPlayerData *, const struct RfuPlayerData *);
static void ItemPrintFunc_PossibleGroupMembers(u8, u32, u8);
static void ListMenuItemPrintFunc_UnionRoomGroups(u8, u32, u8);
static void TradeBoardListMenuItemPrintFunc(u8, u32, u8);
static void ItemPrintFunc_EmptyList(u8, u32, u8);

#include "data/union_room.h"

static void PrintNumPlayersWaitingForMsg(u8 windowId, u8 capacityCode, u8 stringId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    switch (capacityCode << 8)
    {
    case LINK_GROUP_CAPACITY(0, 2):
        PrintUnionRoomText(windowId, FONT_NORMAL, sPlayersNeededOrModeTexts[0][stringId - 1], 0, 1, UR_COLOR_DEFAULT);
        break;
    case LINK_GROUP_CAPACITY(0, 4):
        PrintUnionRoomText(windowId, FONT_NORMAL, sPlayersNeededOrModeTexts[1][stringId - 1], 0, 1, UR_COLOR_DEFAULT);
        break;
    case LINK_GROUP_CAPACITY(2, 5):
        PrintUnionRoomText(windowId, FONT_NORMAL, sPlayersNeededOrModeTexts[2][stringId - 1], 0, 1, UR_COLOR_DEFAULT);
        break;
    case LINK_GROUP_CAPACITY(3, 5):
        PrintUnionRoomText(windowId, FONT_NORMAL, sPlayersNeededOrModeTexts[3][stringId - 1], 0, 1, UR_COLOR_DEFAULT);
        break;
    case LINK_GROUP_CAPACITY(2, 4):
        PrintUnionRoomText(windowId, FONT_NORMAL, sPlayersNeededOrModeTexts[4][stringId - 1], 0, 1, UR_COLOR_DEFAULT);
        break;
    }

    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void PrintPlayerNameAndIdOnWindow(u8 windowId)
{
    u8 text[30];
    u8 *txtPtr;

    PrintUnionRoomText(windowId, FONT_NORMAL, gSaveData.playerName, 0, 1, UR_COLOR_DEFAULT);
    txtPtr = StringCopy(text, sText_ID);
    ConvertIntToDecimalStringN(txtPtr, ReadAsU16(gSaveData.playerTrainerId), STR_CONV_MODE_LEADING_ZEROS, 5);
    PrintUnionRoomText(windowId, FONT_NORMAL, text, 0, 17, UR_COLOR_DEFAULT);
}

static void GetAwaitingCommunicationText(u8 *dst, u8 activity)
{
    switch (activity)
    {
    case ACTIVITY_BATTLE_SINGLE:
    case ACTIVITY_BATTLE_DOUBLE:
    case ACTIVITY_BATTLE_MULTI:
    case ACTIVITY_TRADE:
        // BUG: argument *dst isn't used, instead it always prints to gStringVar4
        // not an issue in practice since Gamefreak never used any other arguments here besides gStringVar4
    #ifndef BUGFIX
        StringExpandPlaceholders(gStringVar4, sText_AwaitingCommunication);
    #else
        StringExpandPlaceholders(dst, sText_AwaitingCommunication);
    #endif
        break;
    }
}



static void Leader_DestroyResources(struct WirelessLink_Leader *data)
{
    ClearWindowTilemap(data->nPlayerModeWindowId);
    ClearStdWindowAndFrame(data->nPlayerModeWindowId, FALSE);
    DestroyListMenuTask(data->listTaskId, 0, 0);
    ClearWindowTilemap(data->bButtonCancelWindowId);
    ClearStdWindowAndFrame(data->listWindowId, FALSE);
    CopyBgTilemapBufferToVram(0);
    RemoveWindow(data->nPlayerModeWindowId);
    RemoveWindow(data->listWindowId);
    RemoveWindow(data->bButtonCancelWindowId);
    DestroyTask(data->listenTaskId);

    Free(data->playerListBackup);
    Free(data->playerList);
    Free(data->incomingPlayerList);
}

static void Leader_GetAcceptNewMemberPrompt(u8 *dst, u8 activity)
{
    switch (activity)
    {
    case ACTIVITY_BATTLE_SINGLE:
    case ACTIVITY_BATTLE_DOUBLE:
    case ACTIVITY_TRADE:
        StringExpandPlaceholders(dst, sText_PlayerContactedYouForXAccept);
        break;
    case ACTIVITY_BATTLE_MULTI:
        StringExpandPlaceholders(dst, sText_PlayerContactedYouAddToMembers);
        break;
    }
}

static void GetYouDeclinedTheOfferMessage(u8 *dst, u8 activity)
{
    switch (activity)
    {
    case ACTIVITY_BATTLE_SINGLE | IN_UNION_ROOM:
    case ACTIVITY_TRADE | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_OfferDeclined1);
        break;
    case ACTIVITY_CHAT | IN_UNION_ROOM:
    case ACTIVITY_CARD | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_OfferDeclined2);
        break;
    }
}

static void GetYouAskedToJoinGroupPleaseWaitMessage(u8 *dst, u8 activity)
{
    switch (activity)
    {
    case ACTIVITY_BATTLE_SINGLE:
    case ACTIVITY_BATTLE_DOUBLE:
    case ACTIVITY_TRADE:
        StringExpandPlaceholders(dst, sText_AwaitingPlayersResponse);
        break;
    case ACTIVITY_BATTLE_MULTI:
        StringExpandPlaceholders(dst, sText_PlayerHasBeenAskedToRegisterYouPleaseWait);
        break;
    }
}

static void GetGroupLeaderSentAnOKMessage(u8 *dst, u8 activity)
{
    switch (activity)
    {
    case ACTIVITY_BATTLE_SINGLE:
    case ACTIVITY_BATTLE_DOUBLE:
    case ACTIVITY_TRADE:
        StringExpandPlaceholders(dst, sText_PlayerSentBackOK);
        break;
    case ACTIVITY_BATTLE_MULTI:
        StringExpandPlaceholders(dst, sText_PlayerOKdRegistration);
        break;
    }
}

static bool8 Leader_SetStateIfMemberListChanged(struct WirelessLink_Leader *data, u32 joinedState, u32 droppedState)
{
    switch (LeaderUpdateGroupMembership(data->playerList))
    {
    case UNION_ROOM_SPAWN_IN:
        PlaySE(SE_PC_LOGIN);
        RedrawListMenu(data->listTaskId);
        CopyAndTranslatePlayerName(gStringVar2, &data->playerList->players[data->playerCount]);
        Leader_GetAcceptNewMemberPrompt(gStringVar4, gPlayerCurrActivity);
        data->state = joinedState;
        break;
    case UNION_ROOM_SPAWN_OUT:
        RfuSetStatus(RFU_STATUS_OK, 0);
        RedrawListMenu(data->listTaskId);
        data->state = droppedState;
        return TRUE;
    }

    return FALSE;
}

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

static u8 LeaderUpdateGroupMembership(struct RfuPlayerList *list)
{
    struct WirelessLink_Leader *data = sWirelessLinkMain.leader;
    u8 ret = UNION_ROOM_SPAWN_NONE;
    u8 i;
    s32 id;

    for (i = 1; i < MAX_RFU_PLAYERS; i++)
    {
        u16 var = data->playerList->players[i].groupScheduledAnim;
        if (var == UNION_ROOM_SPAWN_IN)
        {
            id = GetNewIncomingPlayerId(&data->playerList->players[i], data->incomingPlayerList->players);
            if (id != 0xFF)
            {
                // New incoming player
                data->playerList->players[i].rfu = data->incomingPlayerList->players[id].rfu;
                data->playerList->players[i].timeoutCounter = 1;
            }
            else
            {
                // No new incoming player
                data->playerList->players[i].groupScheduledAnim = UNION_ROOM_SPAWN_OUT;
                ret = UNION_ROOM_SPAWN_OUT;
            }
        }
    }

    for (id = 0; id < RFU_CHILD_MAX; id++)
        TryAddIncomingPlayerToList(data->playerList->players, &data->incomingPlayerList->players[id], MAX_RFU_PLAYERS);

    if (ret != UNION_ROOM_SPAWN_OUT)
    {
        for (id = 0; id < MAX_RFU_PLAYERS; id++)
        {
            if (data->playerList->players[id].newPlayerCountdown != 0)
                ret = UNION_ROOM_SPAWN_IN;
        }
    }

    return ret;
}

static u8 LeaderPrunePlayerList(struct RfuPlayerList *list)
{
    struct WirelessLink_Leader *data = sWirelessLinkMain.leader;
    u8 copiedCount;
    s32 i;
    u8 playerCount;

    for (i = 0; i < MAX_RFU_PLAYERS; i++)
        data->playerListBackup->players[i] = data->playerList->players[i];

    copiedCount = 0;
    for (i = 0; i < MAX_RFU_PLAYERS; i++)
    {
        if (data->playerListBackup->players[i].groupScheduledAnim == UNION_ROOM_SPAWN_IN)
        {
            data->playerList->players[copiedCount] = data->playerListBackup->players[i];
            copiedCount++;
        }
    }

    playerCount = copiedCount;
    for (; copiedCount < MAX_RFU_PLAYERS; copiedCount++)
    {
        data->playerList->players[copiedCount].rfu = sUnionRoomPlayer_DummyRfu;
        data->playerList->players[copiedCount].timeoutCounter = 0;
        data->playerList->players[copiedCount].groupScheduledAnim = UNION_ROOM_SPAWN_NONE;
        data->playerList->players[copiedCount].useRedText = FALSE;
        data->playerList->players[copiedCount].newPlayerCountdown = 0;
    }

    for (i = 0; i < MAX_RFU_PLAYERS; i++)
    {
        if (data->playerList->players[i].groupScheduledAnim != UNION_ROOM_SPAWN_IN)
            continue;
        if (data->playerList->players[i].newPlayerCountdown != 64)
            continue;

        playerCount = i;
        break;
    }

    return playerCount;
}



static u32 IsTryingToTradeAcrossVersionTooSoon(struct WirelessLink_Group *data, s32 id)
{
    struct RfuPlayer *partner = &data->playerList->players[id];

    if (gPlayerCurrActivity == ACTIVITY_TRADE && partner->rfu.data.compatibility.version != VERSION_EMERALD)
    {
        if (!(gSaveData.specialSaveWarpFlags & CHAMPION_SAVEWARP))
            return UR_TRADE_PLAYER_NOT_READY;
        else if (partner->rfu.data.compatibility.canLinkNationally)
            return UR_TRADE_READY;
    }
    else
    {
        return UR_TRADE_READY;
    }

    return UR_TRADE_PARTNER_NOT_READY;
}

static void AskToJoinRfuGroup(struct WirelessLink_Group *data, s32 id)
{
    data->leaderId = id;
    LoadWirelessStatusIndicatorSpriteGfx();
    CreateWirelessStatusIndicatorSprite(0, 0);
    RedrawListMenu(data->listTaskId);
    CopyAndTranslatePlayerName(gStringVar1, &data->playerList->players[data->leaderId]);
    UpdateGameData_SetActivity(sLinkGroupToURoomActivity[gSpecialVar_0x8004], 0, TRUE);
    CreateTask_RfuReconnectWithParent(data->playerList->players[data->leaderId].rfu.name, ReadAsU16(data->playerList->players[data->leaderId].rfu.data.compatibility.playerTrainerId));
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

static void Task_StartUnionRoomTrade(u8 taskId)
{
    u32 monId = GetPartyPositionOfRegisteredMon(&sUnionRoomTrade, GetMultiplayerId());

    switch (gTasks[taskId].data[0])
    {
    case 0:
        LinkTradeStart(LINK_TRADE_PREFLIGHT, monId);
        gTasks[taskId].data[0] = 4;
        break;
    case 4:
    {
        enum LinkTradeResult result = LinkTradePoll();
        if (result == LINK_TRADE_OK)
            gTasks[taskId].data[0] = 5;
        else if (result != LINK_TRADE_WAIT)
        {
            LinkProtocolSetError(result == LINK_TRADE_RECOVERED ? LINK_PROTOCOL_RECOVERED : LINK_PROTOCOL_TRADE_ERROR);
            SetMainCallback2(CB2_LinkError);
            DestroyTask(taskId);
        }
        break;
    }
    case 5:
        if (SendBlock(0, &gParties[B_TRAINER_PLAYER][monId], sizeof(struct Pokemon)))
            gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (GetBlockReceivedStatus() == 3)
        {
            gParties[B_TRAINER_OPPONENT_A][0] = *(struct Pokemon *)(gBlockRecvBuffer[GetMultiplayerId() ^ 1]);
            ResetBlockReceivedFlags();
            gTasks[taskId].data[0]++;
        }
        break;
    case 2:
        memcpy(gBlockSendBuffer, gSaveData.mail, sizeof(struct Mail) * PARTY_SIZE + 4);
        if (SendBlock(0, gBlockSendBuffer, sizeof(struct Mail) * PARTY_SIZE + 4))
            gTasks[taskId].data[0]++;
        break;
    case 3:
        if (GetBlockReceivedStatus() == 3)
        {
            memcpy(gTradeMail, gBlockRecvBuffer[GetMultiplayerId() ^ 1], sizeof(struct Mail) * PARTY_SIZE);
            ResetBlockReceivedFlags();
            gSelectedTradeMonPositions[TRADE_PLAYER] = monId;
            gSelectedTradeMonPositions[TRADE_PARTNER] = PARTY_SIZE;
            gMain.savedCallback = CB2_ReturnToField;
            SetMainCallback2(CB2_LinkTrade);
            ResetUnionRoomTrade(&sUnionRoomTrade);
            DestroyTask(taskId);
        }
        break;
    }
}

static void Task_ExchangeCards(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        if (GetMultiplayerId() == 0)
            SendBlockRequest(BLOCK_REQ_SIZE_100);
        gTasks[taskId].data[0]++;
        break;
    case 1:
        if (GetBlockReceivedStatus() == GetLinkPlayerCountAsBitFlags())
        {
            s32 i;
            u16 *recvBuff;

            for (i = 0; i < GetLinkPlayerCount(); i++)
            {
                recvBuff = gBlockRecvBuffer[i];
                CopyTrainerCardData(&gTrainerCards[i], (struct TrainerCard *)recvBuff, gLinkPlayers[i].version);
            }

            ResetBlockReceivedFlags();
            DestroyTask(taskId);
        }
        break;
    }
}

static void CB2_ShowCard(void)
{
    switch (gMain.state)
    {
    case 0:
        CreateTask(Task_ExchangeCards, 5);
        gMain.state++;
        break;
    case 1:
        if (!FuncIsActiveTask(Task_ExchangeCards))
            ShowTrainerCardInLink(GetMultiplayerId() ^ 1, CB2_ReturnToField);
        break;
    }

    RunTasks();
    RunTextPrinters();
    AnimateSprites();
    BuildOamBuffer();
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

static void WarpForCableClubActivity(s8 mapGroup, s8 mapNum, s32 x, s32 y, u16 linkService)
{
    gSpecialVar_0x8004 = linkService;
    VarSet(VAR_CABLE_CLUB_STATE, linkService);
    gFieldLinkPlayerCount = GetLinkPlayerCount();
    gLocalLinkPlayerId = GetMultiplayerId();
    SetCableClubWarp();
    SetWarpDestination(mapGroup, mapNum, WARP_ID_NONE, x, y);
    WarpIntoMap();
}

static void CB2_TransitionToCableClub(void)
{
    switch (gMain.state)
    {
    case 0:
        CreateTask(Task_ExchangeCards, 5);
        gMain.state++;
        break;
    case 1:
        if (!FuncIsActiveTask(Task_ExchangeCards))
            SetMainCallback2(CB2_ReturnToFieldCableClub);
        break;
    }

    RunTasks();
    RunTextPrinters();
    AnimateSprites();
    BuildOamBuffer();
}




















static u16 ReadAsU16(const u8 *ptr)
{
    return (ptr[1] << 8) | (ptr[0]);
}

static void ScheduleFieldMessageWithFollowupState(u32 nextState, const u8 *src)
{
    struct WirelessLink_URoom *uroom = sWirelessLinkMain.uRoom;

    uroom->state = UR_STATE_PRINT_MSG;
    uroom->stateAfterPrint = nextState;
    if (src != gStringVar4)
        StringExpandPlaceholders(gStringVar4, src);
}

static void ScheduleFieldMessageAndExit(const u8 *src)
{
    struct WirelessLink_URoom *uroom = sWirelessLinkMain.uRoom;

    uroom->state = UR_STATE_PRINT_AND_EXIT;
    if (src != gStringVar4)
        StringExpandPlaceholders(gStringVar4, src);
}

#define PLAYER_LIST_BUFFER_SIZE (MAX_UNION_ROOM_LEADERS * sizeof(struct RfuPlayer))

// Note: This probably could be alloced instead, but I'm not familiar enough with the union room system.
static EWRAM_DATA ALIGNED(4) u8 sPlayerListBuffer[PLAYER_LIST_BUFFER_SIZE];

static void CopyPlayerListToBuffer(struct WirelessLink_URoom *uroom)
{
    memcpy(sPlayerListBuffer, uroom->playerList, PLAYER_LIST_BUFFER_SIZE);
}

static void CopyPlayerListFromBuffer(struct WirelessLink_URoom *uroom)
{
    memcpy(uroom->playerList, sPlayerListBuffer, PLAYER_LIST_BUFFER_SIZE);
}


void SetUsingUnionRoomStartMenu(void)
{
}

static void ReceiveUnionRoomActivityPacket(struct WirelessLink_URoom *data)
{
    if (gRecvCmds[1][1] != 0 && (gRecvCmds[1][0] & RFUCMD_MASK) == RFUCMD_SEND_PACKET)
    {
        data->recvActivityRequest[0] = gRecvCmds[1][1];
        if (gRecvCmds[1][1] == (ACTIVITY_TRADE | IN_UNION_ROOM))
        {
            data->recvActivityRequest[1] = gRecvCmds[1][2];
            data->recvActivityRequest[2] = gRecvCmds[1][3];
        }
    }
}

static bool32 HandleContactFromOtherPlayer(struct WirelessLink_URoom *uroom)
{
    if (uroom->recvActivityRequest[0] != 0)
    {
        s32 id = GetChatLeaderActionRequestMessage(gStringVar4, gLinkPlayers[1].gender, &uroom->recvActivityRequest[0], uroom);
        if (id == 0) // Error
        {
            return TRUE;
        }
        else if (id == 1) // Recieve activity request
        {
            uroom->state = UR_STATE_RECV_ACTIVITY_REQUEST;
            gPlayerCurrActivity = uroom->recvActivityRequest[0];
            return FALSE;
        }
        else if (id == 2) // No activity
        {
            uroom->state = UR_STATE_CANCEL_REQUEST_PRINT_MSG;
            SetCloseLinkCallback();
            return FALSE;
        }
    }

    return TRUE;
}







static u8 HandlePlayerListUpdate(void)
{
    s32 i;
    u8 j;
    struct WirelessLink_URoom *data = sWirelessLinkMain.uRoom;
    s32 retVal = PLIST_NONE;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        if (ArePlayersDifferent(&data->incomingParentList->players[i].rfu, &sUnionRoomPlayer_DummyRfu) == TRUE)
        {
            data->spawnPlayer->players[0].rfu = data->incomingParentList->players[i].rfu;
            data->spawnPlayer->players[0].timeoutCounter = 0;
            data->spawnPlayer->players[0].groupScheduledAnim = UNION_ROOM_SPAWN_IN;
            data->spawnPlayer->players[0].newPlayerCountdown = 1;
            return PLIST_CONTACTED;
        }
    }
    for (j = 0; j < MAX_UNION_ROOM_LEADERS; j++)
    {
        if (data->playerList->players[j].groupScheduledAnim != UNION_ROOM_SPAWN_NONE)
        {
            i = GetNewIncomingPlayerId(&data->playerList->players[j], &data->incomingChildList->players[0]);
            if (i != 0xFF)
            {
                if (data->playerList->players[j].groupScheduledAnim == UNION_ROOM_SPAWN_IN)
                {
                    if (ArePlayerDataDifferent(&data->playerList->players[j].rfu, &data->incomingChildList->players[i].rfu))
                    {
                        data->playerList->players[j].rfu = data->incomingChildList->players[i].rfu;
                        data->playerList->players[j].newPlayerCountdown = 64;
                        retVal = PLIST_NEW_PLAYER;
                    }
                    else if (data->playerList->players[j].newPlayerCountdown != 0)
                    {
                        data->playerList->players[j].newPlayerCountdown--;
                        if (data->playerList->players[j].newPlayerCountdown == 0)
                            retVal = PLIST_RECENT_UPDATE;
                    }
                }
                else
                {
                    data->playerList->players[j].groupScheduledAnim = UNION_ROOM_SPAWN_IN;
                    data->playerList->players[j].newPlayerCountdown = 0;
                    retVal = PLIST_RECENT_UPDATE;
                }
                data->playerList->players[j].timeoutCounter = 0;
            }
            else if (data->playerList->players[j].groupScheduledAnim != UNION_ROOM_SPAWN_OUT)
            {
                data->playerList->players[j].timeoutCounter++;
                if (data->playerList->players[j].timeoutCounter >= 600)
                {
                    data->playerList->players[j].groupScheduledAnim = UNION_ROOM_SPAWN_OUT;
                    retVal = PLIST_RECENT_UPDATE;
                }
            }
            else if (data->playerList->players[j].groupScheduledAnim == UNION_ROOM_SPAWN_OUT)
            {
                data->playerList->players[j].timeoutCounter++;
                if (data->playerList->players[j].timeoutCounter >= 900)
                    ClearRfuPlayerList(&data->playerList->players[j], 1);
            }
        }
    }
    for (i = 0; i < RFU_CHILD_MAX; i++)
        if (TryAddIncomingPlayerToList(&data->playerList->players[0], &data->incomingChildList->players[i], MAX_UNION_ROOM_LEADERS) != 0xFF)
            retVal = PLIST_NEW_PLAYER;

    return retVal;
}

static void Task_SearchForChildOrParent(u8 taskId)
{
    s32 i, j;
    struct RfuPlayerData rfu;
    struct RfuIncomingPlayerList **list = (void *) gTasks[taskId].data;
    bool8 isParent;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        isParent = Rfu_GetCompatiblePlayerData(&rfu.data, rfu.name, i);

        if (!IsUnionRoomActivityAcceptable(rfu.data.activity, gTasks[taskId].data[4]))
            rfu = sUnionRoomPlayer_DummyRfu;
        if (rfu.data.compatibility.language == LANGUAGE_JAPANESE)
            rfu = sUnionRoomPlayer_DummyRfu;

        if (!isParent)
        {
            for (j = 0; j < i; j++)
            {
                if (!ArePlayersDifferent(&list[1]->players[j].rfu, &rfu))
                    rfu = sUnionRoomPlayer_DummyRfu;
            }
            list[1]->players[i].rfu = rfu;
            list[1]->players[i].active = ArePlayersDifferent(&list[1]->players[i].rfu, &sUnionRoomPlayer_DummyRfu);
        }
        else
        {
            list[0]->players[i].rfu = rfu;
            list[0]->players[i].active = ArePlayersDifferent(&list[0]->players[i].rfu, &sUnionRoomPlayer_DummyRfu);
        }
    }
}

static u8 CreateTask_SearchForChildOrParent(struct RfuIncomingPlayerList *parentList, struct RfuIncomingPlayerList *childList, u32 linkGroup)
{
    u8 taskId = CreateTask(Task_SearchForChildOrParent, 0);
    struct RfuIncomingPlayerList **data = (void *)gTasks[taskId].data;
    data[0] = parentList;
    data[1] = childList;
    gTasks[taskId].data[4] = linkGroup;
    return taskId;
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



static bool32 UR_PrintFieldMessage(const u8 *src)
{
    LoadMessageBoxAndBorderGfx();
    DrawDialogueFrame(0, TRUE);
    StringExpandPlaceholders(gStringVar4, src);
    AddTextPrinterWithCustomSpeedForMessage(FALSE, 1);
    return FALSE;
}

static bool32 UR_RunTextPrinters(void)
{
    if (!RunTextPrintersAndIsPrinter0Active())
        return TRUE;
    else
        return FALSE;
}

static bool8 PrintOnTextbox(u8 *textState, const u8 *str)
{
    switch (*textState)
    {
    case 0:
        LoadMessageBoxAndBorderGfx();
        DrawDialogueFrame(0, TRUE);
        StringExpandPlaceholders(gStringVar4, str);
        AddTextPrinterForMessage(TRUE);
        (*textState)++;
        break;
    case 1:
        if (!RunTextPrintersAndIsPrinter0Active())
        {
            *textState = 0;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static s8 UnionRoomHandleYesNo(u8 *state, bool32 noDraw)
{
    s8 input;

    switch (*state)
    {
    case 0:
        if (noDraw)
            return -3;
        DisplayYesNoMenuDefaultYes();
        (*state)++;
        break;
    case 1:
        if (noDraw)
        {
            EraseYesNoWindow();
            *state = 0;
            return -3;
        }
        input = Menu_ProcessInputNoWrapClearOnChoose();
        if (input == MENU_B_PRESSED || input == 0 || input == 1)
        {
            *state = 0;
            return input;
        }
        break;
    }
    return MENU_NOTHING_CHOSEN;
}

static u8 CreateTradeBoardWindow(const struct WindowTemplate *template)
{
    u8 windowId = AddWindow(template);
    DrawStdWindowFrame(windowId, FALSE);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(15));
    PrintUnionRoomText(windowId, FONT_NORMAL, sText_NameWantedOfferLv, 8, 1, UR_COLOR_TRADE_BOARD_OTHER);
    CopyWindowToVram(windowId, COPYWIN_GFX);
    PutWindowTilemap(windowId);
    return windowId;
}

static void DeleteTradeBoardWindow(u8 windowId)
{
    RemoveWindow(windowId);
}

static s32 ListMenuHandler_AllItemsAvailable(u8 *state, u8 *windowId, u8 *listMenuId, const struct WindowTemplate *winTemplate, const struct ListMenuTemplate *menuTemplate)
{
    s32 maxWidth, input;
    struct WindowTemplate winTemplateCopy;

    switch (*state)
    {
    case 0:
        winTemplateCopy = *winTemplate;
        maxWidth = Intl_GetListMenuWidth(menuTemplate);
        if (winTemplateCopy.width > maxWidth)
            winTemplateCopy.width = maxWidth;

        if (winTemplateCopy.tilemapLeft + winTemplateCopy.width >= DISPLAY_TILE_WIDTH)
            winTemplateCopy.tilemapLeft = max(DISPLAY_TILE_WIDTH - 1 - winTemplateCopy.width, 0);

        *windowId = AddWindow(&winTemplateCopy);
        DrawStdWindowFrame(*windowId, FALSE);
        gMultiuseListMenuTemplate = *menuTemplate;
        gMultiuseListMenuTemplate.windowId = *windowId;
        *listMenuId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
        CopyWindowToVram(*windowId, COPYWIN_MAP);
        (*state)++;
        break;
    case 1:
        input = ListMenu_ProcessInput(*listMenuId);
        if (JOY_NEW(A_BUTTON))
        {
            DestroyListMenuTask(*listMenuId, NULL, NULL);
            ClearStdWindowAndFrame(*windowId, TRUE);
            RemoveWindow(*windowId);
            *state = 0;
            return input;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            DestroyListMenuTask(*listMenuId, NULL, NULL);
            ClearStdWindowAndFrame(*windowId, TRUE);
            RemoveWindow(*windowId);
            *state = 0;
            return LIST_CANCEL;
        }
        break;
    }

    return LIST_NOTHING_CHOSEN;
}

static s32 TradeBoardMenuHandler(u8 *state, u8 *mainWindowId, u8 *listMenuId, u8 *headerWindowId,
                                const struct WindowTemplate *winTemplate,
                                const struct ListMenuTemplate *menuTemplate,
                                struct RfuPlayerList *list)
{
    s32 input;
    s32 idx;

    switch (*state)
    {
    case 0:
        *headerWindowId = CreateTradeBoardWindow(&sWindowTemplate_TradingBoardHeader);
        *mainWindowId = AddWindow(winTemplate);
        DrawStdWindowFrame(*mainWindowId, FALSE);
        gMultiuseListMenuTemplate = *menuTemplate;
        gMultiuseListMenuTemplate.windowId = *mainWindowId;
        *listMenuId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 1);
        (*state)++;
        break;
    case 1:
        CopyWindowToVram(*mainWindowId, COPYWIN_MAP);
        (*state)++;
        break;
    case 2:
        input = ListMenu_ProcessInput(*listMenuId);
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            // Exit or B button
            if (input == 8 || JOY_NEW(B_BUTTON))
            {
                DestroyListMenuTask(*listMenuId, NULL, NULL);
                RemoveWindow(*mainWindowId);
                DeleteTradeBoardWindow(*headerWindowId);
                *state = 0;
                return LIST_CANCEL;
            }
            else
            {
                idx = GetIndexOfNthTradeBoardOffer(list->players, input);
                if (idx >= 0)
                {
                    DestroyListMenuTask(*listMenuId, NULL, NULL);
                    RemoveWindow(*mainWindowId);
                    DeleteTradeBoardWindow(*headerWindowId);
                    *state = 0;
                    return idx;
                }
                else
                {
                    PlaySE(SE_WALL_HIT);
                }
            }
        }
        break;
    }

    return LIST_NOTHING_CHOSEN;
}

static void UR_ClearBg0(void)
{
    FillBgTilemapBufferRect(0, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(0);
}

static void JoinGroup_EnableScriptContexts(void)
{
    ScriptContext_Enable();
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

static bool32 IsPlayerFacingTradingBoard(void)
{
    s16 x, y;
    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);

    if (x != 2 + MAP_OFFSET)
        return FALSE;

    if (y != 1 + MAP_OFFSET)
        return FALSE;

    if (gPlayerAvatar.tileTransitionState == T_TILE_CENTER || gPlayerAvatar.tileTransitionState == T_NOT_MOVING)
        return TRUE;

    return FALSE;
}

static u32 GetResponseIdx_InviteToURoomActivity(s32 activity)
{
    switch (activity)
    {
    case ACTIVITY_CHAT:
        return 1;
    case ACTIVITY_TRADE:
        return 2;
    case ACTIVITY_CARD:
        return 3;
    case ACTIVITY_BATTLE_MULTI:
    default:
        return 0;
    }
}



static s32 UnionRoomGetPlayerInteractionResponse(struct RfuPlayerList *list, bool8 overrideGender, u8 playerIdx, u32 playerGender)
{

    struct RfuPlayer *player = &list->players[playerIdx];

    if (!player->rfu.data.startedActivity && !overrideGender)
    {
        CopyAndTranslatePlayerName(gStringVar1, player);
        if (player->rfu.data.activity == (ACTIVITY_CHAT | IN_UNION_ROOM))
        {
            StringExpandPlaceholders(gStringVar4, sJoinChatTexts[0][playerGender]);
            return 2;
        }
        else
        {
            UR_PrintFieldMessage(sCommunicatingWaitTexts[0]);
            return 1;
        }
    }
    else
    {
        CopyAndTranslatePlayerName(gStringVar1, player);
        if (overrideGender)
        {
            playerGender = (player->rfu.data.compatibility.playerTrainerId[overrideGender - 1] >> 3) & 1;
        }
        switch (player->rfu.data.activity & 0x3F)
        {
        case ACTIVITY_BATTLE_SINGLE:
            StringExpandPlaceholders(gStringVar4, sBattleReactionTexts[playerGender][Random() % ARRAY_COUNT(sBattleReactionTexts[0])]);
            break;
        case ACTIVITY_TRADE:
            StringExpandPlaceholders(gStringVar4, sTradeReactionTexts[playerGender][Random() % 2]);
            break;
        case ACTIVITY_CHAT:
            StringExpandPlaceholders(gStringVar4, sChatReactionTexts[playerGender][Random() % ARRAY_COUNT(sChatReactionTexts[0])]);
            break;
        case ACTIVITY_CARD:
            StringExpandPlaceholders(gStringVar4, sTrainerCardReactionTexts[playerGender][Random() % ARRAY_COUNT(sTrainerCardReactionTexts[0])]);
            break;
        default:
            StringExpandPlaceholders(gStringVar4, sText_TrainerAppearsBusy);
            break;
        }
        return 0;
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

static s32 GetIndexOfNthTradeBoardOffer(struct RfuPlayer *players, s32 n)
{
    s32 i;
    s32 j = 0;

    for (i = 0; i < MAX_UNION_ROOM_LEADERS; i++)
    {
        if (players[i].groupScheduledAnim == UNION_ROOM_SPAWN_IN && players[i].rfu.data.tradeSpecies != SPECIES_NONE)
            j++;

        if (j == n + 1)
            return i;
    }

    return -1;
}

static s32 GetUnionRoomPlayerGender(s32 playerIdx, struct RfuPlayerList *list)
{
    return list->players[playerIdx].rfu.data.playerGender;
}

static s32 IsRequestedTradeInPlayerParty(enum Type type, enum Species species)
{
    s32 i;

    if (species == SPECIES_EGG)
    {
        for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
        {
            species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
            if (species == SPECIES_EGG)
                return UR_TRADE_MATCH;
        }
        return UR_TRADE_NOEGG;
    }
    else
    {
        for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
        {
            species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
            if (GetSpeciesType(species, 0) == type || GetSpeciesType(species, 1) == type)
                return UR_TRADE_MATCH;
        }
        return UR_TRADE_NOTYPE;
    }
}

static void GetURoomActivityRejectMsg(u8 *dst, s32 acitivty, u32 playerGender)
{
    switch (acitivty)
    {
    case ACTIVITY_BATTLE_SINGLE | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sBattleDeclinedTexts[playerGender]);
        break;
    case ACTIVITY_CHAT | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sChatDeclinedTexts[playerGender]);
        break;
    case ACTIVITY_TRADE | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_TradeOfferRejected);
        break;
    case ACTIVITY_CARD | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sShowTrainerCardDeclinedTexts[playerGender]);
        break;
    }
}

static void GetURoomActivityStartMsg(u8 *dst, u8 acitivty)
{
    u8 mpId = GetMultiplayerId();
    u8 gender = gLinkPlayers[mpId ^ 1].gender;

    switch (acitivty)
    {
    case ACTIVITY_BATTLE_SINGLE | IN_UNION_ROOM:
        StringCopy(dst, sStartActivityTexts[mpId][gender][0]);
        break;
    case ACTIVITY_TRADE | IN_UNION_ROOM:
        StringCopy(dst, sStartActivityTexts[mpId][gender][2]);
        break;
    case ACTIVITY_CHAT | IN_UNION_ROOM:
        StringCopy(dst, sStartActivityTexts[mpId][gender][1]);
        break;
    }
}

static s32 GetChatLeaderActionRequestMessage(u8 *dst, u32 gender, u16 *activityData, struct WirelessLink_URoom *uroom)
{
    s32 result = 0;
    enum Species species = SPECIES_NONE;
    s32 i;

    switch (activityData[0])
    {
    case ACTIVITY_BATTLE_SINGLE | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_BattleChallenge);
        result = 1;
        break;
    case ACTIVITY_CHAT | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_ChatInvitation);
        result = 1;
        break;
    case ACTIVITY_TRADE | IN_UNION_ROOM:
        ConvertIntToDecimalStringN(uroom->activityRequestStrbufs[0], sUnionRoomTrade.playerLevel, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(uroom->activityRequestStrbufs[1], GetSpeciesName(sUnionRoomTrade.playerSpecies));
        for (i = 0; i < RFU_CHILD_MAX; i++)
        {
            if (gRfuLinkStatus->partner[i].serialNo == RFU_SERIAL_GAME)
            {
                ConvertIntToDecimalStringN(uroom->activityRequestStrbufs[2], activityData[2], STR_CONV_MODE_LEFT_ALIGN, 3);
                StringCopy(uroom->activityRequestStrbufs[3], GetSpeciesName(activityData[1]));
                species = activityData[1];
                break;
            }
        }
        if (species == SPECIES_EGG)
        {
            StringCopy(dst, sText_OfferToTradeEgg);
        }
        else
        {
            for (i = 0; i < RFU_CHILD_MAX; i++)
                DynamicPlaceholderTextUtil_SetPlaceholderPtr(i, uroom->activityRequestStrbufs[i]);
            DynamicPlaceholderTextUtil_ExpandPlaceholders(dst, sText_OfferToTradeMon);
        }
        result = 1;
        break;
    case ACTIVITY_CARD | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_ShowTrainerCard);
        result = 1;
        break;
    case ACTIVITY_NONE | IN_UNION_ROOM:
        StringExpandPlaceholders(dst, sText_ChatDropped);
        result = 2;
        break;
    }

    return result;
}

static bool32 PollPartnerYesNoResponse(struct WirelessLink_URoom *data)
{
    if (gRecvCmds[0][1] != 0)
    {
        if (gRecvCmds[0][1] == (ACTIVITY_ACCEPT | IN_UNION_ROOM))
        {
            data->partnerYesNoResponse = ACTIVITY_ACCEPT | IN_UNION_ROOM;
            return TRUE;
        }
        else if (gRecvCmds[0][1] == (ACTIVITY_DECLINE | IN_UNION_ROOM))
        {
            data->partnerYesNoResponse = ACTIVITY_DECLINE | IN_UNION_ROOM;
            return TRUE;
        }
    }
    return FALSE;
}


static bool32 HasAtLeastTwoMonsOfLevel30OrLower(void)
{
    s32 i;
    s32 count = 0;

    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL) <= UNION_ROOM_MAX_LEVEL
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG)
            count++;
    }

    if (count > 1)
        return TRUE;
    else
        return FALSE;
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

static bool32 RegisterTradeMonAndGetIsEgg(u32 monId, struct UnionRoomTrade *trade)
{
    trade->playerSpecies = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_SPECIES_OR_EGG);
    trade->playerLevel = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_LEVEL);
    trade->playerPersonality = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_PERSONALITY);
    if (trade->playerSpecies == SPECIES_EGG)
        return TRUE;
    else
        return FALSE;
}

static void RegisterTradeMon(u32 monId, struct UnionRoomTrade *trade)
{
    trade->species = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_SPECIES_OR_EGG);
    trade->level = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_LEVEL);
    trade->personality = GetMonData(&gParties[B_TRAINER_PLAYER][monId], MON_DATA_PERSONALITY);
}

static u32 GetPartyPositionOfRegisteredMon(struct UnionRoomTrade *trade, u8 multiplayerId)
{
    u16 response = 0;
    enum Species species;
    u32 personality;
    u32 cur_personality;
    enum Species cur_species;
    s32 i;

    if (multiplayerId == 0)
    {
        species = trade->playerSpecies;
        personality = trade->playerPersonality;
    }
    else
    {
        species = trade->species;
        personality = trade->personality;
    }

    // Find party position by comparing to personality and species
    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        cur_personality = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_PERSONALITY);
        if (cur_personality != personality)
            continue;
        cur_species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        if (cur_species != species)
            continue;
        response = i;
        break;
    }

    return response;
}

static void HandleCancelActivity(bool32 setData)
{
    UR_ClearBg0();
    UnlockPlayerFieldControls();
    UnionRoom_UnlockPlayerAndChatPartner();
    gPlayerCurrActivity = ACTIVITY_NONE;
    if (setData)
    {
        SetTradeBoardRegisteredMonInfo(sUnionRoomTrade.type, sUnionRoomTrade.playerSpecies, sUnionRoomTrade.playerLevel);
        UpdateGameData_SetActivity(ACTIVITY_NONE | IN_UNION_ROOM, 0, FALSE);
    }
}

static void StartScriptInteraction(void)
{
    LockPlayerFieldControls();
    FreezeObjects_WaitForPlayer();
}

static u8 GetActivePartnersInfo(struct WirelessLink_URoom *data)
{
    u8 retVal = PINFO_ACTIVE_FLAG;
    u8 i;

    for (i = 0; i < RFU_CHILD_MAX; i++)
    {
        if (data->incomingParentList->players[i].active)
        {
            retVal |= data->incomingParentList->players[i].rfu.data.playerGender << PINFO_GENDER_SHIFT;
            retVal |= data->incomingParentList->players[i].rfu.data.compatibility.playerTrainerId[0] & PINFO_TID_MASK;
            break;
        }
    }

    return retVal;
}

static void ViewURoomPartnerTrainerCard(u8 *unused, struct WirelessLink_URoom *data, bool8 isParent)
{
    struct TrainerCard *trainerCard = &gTrainerCards[GetMultiplayerId() ^ 1];
    DynamicPlaceholderTextUtil_Reset();

    StringCopy(data->trainerCardStrBuffer[0], gTrainerClasses[GetUnionRoomTrainerClass()].name);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(0, data->trainerCardStrBuffer[0]);

    DynamicPlaceholderTextUtil_SetPlaceholderPtr(1, trainerCard->playerName);

    StringCopy(data->trainerCardColorStrBuffer, sCardColorTexts[trainerCard->stars]);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(2, data->trainerCardColorStrBuffer);

    ConvertIntToDecimalStringN(data->trainerCardStrBuffer[2], trainerCard->caughtMonsCount, STR_CONV_MODE_LEFT_ALIGN, 3);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(3, data->trainerCardStrBuffer[2]);

    ConvertIntToDecimalStringN(data->trainerCardStrBuffer[3], trainerCard->playTimeHours, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(data->trainerCardStrBuffer[4], trainerCard->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(4, data->trainerCardStrBuffer[3]);
    DynamicPlaceholderTextUtil_SetPlaceholderPtr(5, data->trainerCardStrBuffer[4]);

    DynamicPlaceholderTextUtil_ExpandPlaceholders(data->trainerCardMsgStrBuffer, sText_TrainerCardInfoPage1);
    StringCopy(gStringVar4, data->trainerCardMsgStrBuffer);

    if (isParent == TRUE)
    {
        DynamicPlaceholderTextUtil_ExpandPlaceholders(data->trainerCardMsgStrBuffer, sText_FinishedCheckingPlayersTrainerCard);
        StringAppend(gStringVar4, data->trainerCardMsgStrBuffer);
    }
    else if (isParent == FALSE)
    {
        DynamicPlaceholderTextUtil_ExpandPlaceholders(data->trainerCardMsgStrBuffer, sGladToMeetYouTexts[trainerCard->gender]);
        StringAppend(gStringVar4, data->trainerCardMsgStrBuffer);
    }
}

static void CopyAndTranslatePlayerName(u8 *dest, struct RfuPlayer *player)
{
    StringCopy_PlayerName(dest, player->rfu.name);
    ConvertInternationalString(dest, player->rfu.data.compatibility.language);
}
