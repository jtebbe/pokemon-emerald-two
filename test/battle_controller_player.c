#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_interface.h"
#include "bg.h"
#include "data.h"
#include "item_menu.h"
#include "item_use.h"
#include "link.h"
#include "main.h"
#include "m4a.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeball.h"
#include "pokemon.h"
#include "test/battle_fixture.h"
#include "reshow_battle_screen.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "test_runner.h"
#include "text.h"
#include "trainer.h"
#include "util.h"
#include "window.h"
#include "constants/battle_anim.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "test/battle.h"

TEST("Single battle intro bookkeeping never shares an absent partner slot")
{
    u8 previousCount = gBattlersCount;
    u8 previousPositions[MAX_BATTLERS_COUNT];
    memcpy(previousPositions, gBattlerPositions, sizeof(previousPositions));
    gBattlersCount = 2;
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
    EXPECT_EQ(GetBattleIntroPartner(0), 0);
    EXPECT_EQ(GetBattleIntroPartner(1), 1);
    gBattlerPositions[0] = B_POSITION_OPPONENT_LEFT;
    gBattlerPositions[1] = B_POSITION_PLAYER_LEFT;
    EXPECT_EQ(GetBattleIntroPartner(0), 0);
    EXPECT_EQ(GetBattleIntroPartner(1), 1);
    gBattlersCount = previousCount;
    memcpy(gBattlerPositions, previousPositions, sizeof(previousPositions));
}

TEST("Double battle intro bookkeeping follows actual battler positions")
{
    u8 previousCount = gBattlersCount;
    u8 previousPositions[MAX_BATTLERS_COUNT];
    memcpy(previousPositions, gBattlerPositions, sizeof(previousPositions));
    gBattlersCount = 4;
    gBattlerPositions[0] = B_POSITION_OPPONENT_RIGHT;
    gBattlerPositions[1] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[2] = B_POSITION_OPPONENT_LEFT;
    gBattlerPositions[3] = B_POSITION_PLAYER_RIGHT;
    EXPECT_EQ(GetBattleIntroPartner(0), 2);
    EXPECT_EQ(GetBattleIntroPartner(1), 3);
    EXPECT_EQ(GetBattleIntroPartner(2), 0);
    EXPECT_EQ(GetBattleIntroPartner(3), 1);
    gBattlersCount = previousCount;
    memcpy(gBattlerPositions, previousPositions, sizeof(previousPositions));
}

static void TestPlayerHandleDrawTrainerPic(enum BattlerId battler);
static void TestPlayerHandleTrainerSlideBack(enum BattlerId battler);
static void TestPlayerHandleChooseAction(enum BattlerId battler);
static void TestPlayerHandleChooseMove(enum BattlerId battler);
static void TestPlayerHandleChooseItem(enum BattlerId battler);
static void TestPlayerHandleChoosePokemon(enum BattlerId battler);
static void TestPlayerHandleStatusAnimation(enum BattlerId battler);
static void TestPlayerHandleIntroTrainerBallThrow(enum BattlerId battler);
static void TestPlayerHandleDrawPartyStatusSummary(enum BattlerId battler);
static void TestPlayerHandleEndLinkBattle(enum BattlerId battler);
static void TestPlayerBufferRunCommand(enum BattlerId battler);

static void (*const sTestPlayerBufferCommands[CONTROLLER_CMDS_COUNT])(enum BattlerId battler) =
{
    [CONTROLLER_GETMONDATA]               = BtlController_HandleGetMonData,
    [CONTROLLER_GETRAWMONDATA]            = BtlController_Empty,
    [CONTROLLER_SETMONDATA]               = BtlController_HandleSetMonData,
    [CONTROLLER_SETRAWMONDATA]            = BtlController_HandleSetRawMonData,
    [CONTROLLER_LOADMONSPRITE]            = BtlController_HandleLoadMonSprite,
    [CONTROLLER_SWITCHINANIM]             = BtlController_HandleSwitchInAnim,
    [CONTROLLER_RETURNMONTOBALL]          = BtlController_HandleReturnMonToBall,
    [CONTROLLER_DRAWTRAINERPIC]           = TestPlayerHandleDrawTrainerPic,
    [CONTROLLER_TRAINERSLIDE]             = BtlController_Empty,
    [CONTROLLER_TRAINERSLIDEBACK]         = TestPlayerHandleTrainerSlideBack,
    [CONTROLLER_FAINTANIMATION]           = BtlController_HandleFaintAnimation,
    [CONTROLLER_PALETTEFADE]              = BtlController_Empty,
    [CONTROLLER_BALLTHROWANIM]            = BtlController_HandleBallThrowAnim,
    [CONTROLLER_PAUSE]                    = BtlController_Empty,
    [CONTROLLER_MOVEANIMATION]            = BtlController_HandleMoveAnimation,
    [CONTROLLER_PRINTSTRING]              = BtlController_HandlePrintString,
    [CONTROLLER_PRINTSTRINGPLAYERONLY]    = BtlController_Empty,
    [CONTROLLER_CHOOSEACTION]             = TestPlayerHandleChooseAction,
    [CONTROLLER_YESNOBOX]                 = BtlController_Empty,
    [CONTROLLER_CHOOSEMOVE]               = TestPlayerHandleChooseMove,
    [CONTROLLER_OPENBAG]                  = TestPlayerHandleChooseItem,
    [CONTROLLER_CHOOSEPOKEMON]            = TestPlayerHandleChoosePokemon,
    [CONTROLLER_23]                       = BtlController_Empty,
    [CONTROLLER_HEALTHBARUPDATE]          = BtlController_HandleHealthBarUpdate,
    [CONTROLLER_EXPUPDATE]                = PlayerHandleExpUpdate,
    [CONTROLLER_STATUSICONUPDATE]         = BtlController_HandleStatusIconUpdate,
    [CONTROLLER_STATUSANIMATION]          = TestPlayerHandleStatusAnimation,
    [CONTROLLER_STATUSXOR]                = BtlController_Empty,
    [CONTROLLER_DATATRANSFER]             = BtlController_Empty,
    [CONTROLLER_DMA3TRANSFER]             = BtlController_Empty,
    [CONTROLLER_PLAYBGM]                  = BtlController_Empty,
    [CONTROLLER_32]                       = BtlController_Empty,
    [CONTROLLER_TWORETURNVALUES]          = BtlController_Empty,
    [CONTROLLER_CHOSENMONRETURNVALUE]     = BtlController_Empty,
    [CONTROLLER_ONERETURNVALUE]           = BtlController_Empty,
    [CONTROLLER_ONERETURNVALUE_DUPLICATE] = BtlController_Empty,
    [CONTROLLER_HITANIMATION]             = BtlController_HandleHitAnimation,
    [CONTROLLER_CANTSWITCH]               = BtlController_Empty,
    [CONTROLLER_PLAYSE]                   = BtlController_HandlePlaySE,
    [CONTROLLER_PLAYFANFAREORBGM]         = BtlController_HandlePlayFanfareOrBGM,
    [CONTROLLER_FAINTINGCRY]              = BtlController_HandleFaintingCry,
    [CONTROLLER_INTROSLIDE]               = BtlController_HandleIntroSlide,
    [CONTROLLER_INTROTRAINERBALLTHROW]    = TestPlayerHandleIntroTrainerBallThrow,
    [CONTROLLER_DRAWPARTYSTATUSSUMMARY]   = TestPlayerHandleDrawPartyStatusSummary,
    [CONTROLLER_HIDEPARTYSTATUSSUMMARY]   = BtlController_HandleHidePartyStatusSummary,
    [CONTROLLER_ENDBOUNCE]                = BtlController_Empty,
    [CONTROLLER_SPRITEINVISIBILITY]       = BtlController_HandleSpriteInvisibility,
    [CONTROLLER_BATTLEANIMATION]          = BtlController_HandleBattleAnimation,
    [CONTROLLER_LINKSTANDBYMSG]           = BtlController_Empty,
    [CONTROLLER_RESETACTIONMOVESELECTION] = BtlController_Empty,
    [CONTROLLER_ENDLINKBATTLE]            = TestPlayerHandleEndLinkBattle,
    [CONTROLLER_DEBUGMENU]                = BtlController_Empty,
    [CONTROLLER_TERMINATOR_NOP]           = BtlController_TerminatorNop
};

void SetControllerToTestPlayer(enum BattlerId battler)
{
    gBattlerBattleController[battler] = BATTLE_CONTROLLER_TEST_PLAYER;
    gBattlerControllerEndFuncs[battler] = TestPlayerBufferExecCompleted;
    gBattlerControllerFuncs[battler] = TestPlayerBufferRunCommand;
}

static void TestPlayerBufferRunCommand(enum BattlerId battler)
{
    if (IsBattleControllerActiveOnLocal(battler))
    {
        if (gBattleResources->bufferA[battler][0] < ARRAY_COUNT(sTestPlayerBufferCommands))
            sTestPlayerBufferCommands[gBattleResources->bufferA[battler][0]](battler);
        else
            BtlController_Complete(battler);
    }
}

static void Intro_WaitForShinyAnimAndHealthbox(enum BattlerId battler)
{
    bool32 healthboxAnimDone = FALSE;

    if (GetBattlerPosition(battler) == B_POSITION_PLAYER_LEFT)
    {
        if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
        {
            if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy)
                healthboxAnimDone = TRUE;
        }
        else
        {
            if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy
                && gSprites[gHealthboxSpriteIds[GetBattleIntroPartner(battler)]].callback == SpriteCallbackDummy)
            {
                healthboxAnimDone = TRUE;
            }
        }

        if (healthboxAnimDone && gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim
            && gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].finishedShinyMonAnim)
        {
            gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim = FALSE;
            gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim = FALSE;

            gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].triedShinyMonAnim = FALSE;
            gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].finishedShinyMonAnim = FALSE;

            FreeShinyStars();

            HandleLowHpMusicChange(GetBattlerMon(battler), battler);
            if (IsDoubleBattle())
                HandleLowHpMusicChange(GetBattlerMon(GetBattleIntroPartner(battler)), GetBattleIntroPartner(battler));

            gBattleSpritesDataPtr->healthBoxesData[battler].introEndDelay = 3;
            gBattlerControllerFuncs[battler] = BtlController_Intro_DelayAndEnd;
        }
    }
    else
    {
        if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
        {
            if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy)
                healthboxAnimDone = TRUE;
        }
        else
        {
            if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy
                && gSprites[gHealthboxSpriteIds[GetBattleIntroPartner(battler)]].callback == SpriteCallbackDummy)
            {
                healthboxAnimDone = TRUE;
            }
        }

        if (IsCryPlayingOrClearCrySongs())
            healthboxAnimDone = FALSE;

        if (healthboxAnimDone)
        {
            gBattleSpritesDataPtr->healthBoxesData[battler].introEndDelay = 3;
            gBattlerControllerFuncs[battler] = BtlController_Intro_DelayAndEnd;
        }
    }
}

static void Intro_TryShinyAnimShowHealthbox(enum BattlerId battler)
{
    bool32 bgmRestored = FALSE;

    if (GetBattlerPosition(battler) == B_POSITION_PLAYER_LEFT)
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim
        && !gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive)
            TryShinyAnimation(battler, GetBattlerMon(battler));

        if (!gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].triedShinyMonAnim
        && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].ballAnimActive)
            TryShinyAnimation(GetBattleIntroPartner(battler), GetBattlerMon(GetBattleIntroPartner(battler)));
    }

    if (!gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive
        && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].ballAnimActive)
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted)
        {
            if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            {
                UpdateHealthboxAttribute(gHealthboxSpriteIds[GetBattleIntroPartner(battler)], GetBattlerMon(GetBattleIntroPartner(battler)), HEALTHBOX_ALL);
                StartHealthboxSlideIn(GetBattleIntroPartner(battler));
                SetHealthboxSpriteVisible(gHealthboxSpriteIds[GetBattleIntroPartner(battler)]);
            }
            UpdateHealthboxAttribute(gHealthboxSpriteIds[battler], GetBattlerMon(battler), HEALTHBOX_ALL);
            StartHealthboxSlideIn(battler);
            SetHealthboxSpriteVisible(gHealthboxSpriteIds[battler]);
        }
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted = TRUE;
    }

    if (gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted
        && !gBattleSpritesDataPtr->healthBoxesData[battler].waitForCry
        && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].waitForCry
        && !IsCryPlayingOrClearCrySongs())
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored)
        {
            if ((gBattleTypeFlags & BATTLE_TYPE_LINK) && (gBattleTypeFlags & BATTLE_TYPE_MULTI))
            {
                if (GetBattlerPosition(battler) == B_POSITION_PLAYER_LEFT)
                    m4aMPlayContinue(&gMPlayInfo_BGM);
            }
            else
            {
                m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 0x100);
            }

        }
        gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored = TRUE;
        bgmRestored = TRUE;
    }

    if (bgmRestored && gSprites[gBattleControllerData[battler]].callback == SpriteCallbackDummy
        && gSprites[gBattlerSpriteIds[battler]].callback == SpriteCallbackDummy)
    {
        if (IsDoubleBattle() && !(gBattleTypeFlags & BATTLE_TYPE_MULTI))
            DestroySprite(&gSprites[gBattleControllerData[GetBattleIntroPartner(battler)]]);

        DestroySprite(&gSprites[gBattleControllerData[battler]]);
        gBattleSpritesDataPtr->animationData->introAnimActive = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored = FALSE;
        gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted = FALSE;
        gBattlerControllerFuncs[battler] = Intro_WaitForShinyAnimAndHealthbox;
    }
}

void TestPlayerBufferExecCompleted(enum BattlerId battler)
{
    gBattlerControllerFuncs[battler] = TestPlayerBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(battler, B_COMM_CONTROLLER_IS_DONE, 4, &playerId);
        gBattleResources->bufferA[battler][0] = CONTROLLER_TERMINATOR_NOP;
    }
    else
    {
        MarkBattleControllerIdleOnLocal(battler);
    }
}

static void TestPlayerHandleDrawTrainerPic(enum BattlerId battler)
{
    enum TrainerPicID trainerPicId = TRAINER_PIC_BRENDAN;
    s16 xPos = gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER ? 32 : 80;
    s16 yPos = (8 - GetTrainerBackPicCoords(trainerPicId)->size) * 4 + 80;

    BtlController_HandleDrawTrainerPic(battler, trainerPicId, FALSE, xPos, yPos, -1);
}

static void TestPlayerHandleTrainerSlideBack(enum BattlerId battler)
{
    BtlController_HandleTrainerSlideBack(battler, 35, FALSE);
}



static void TestPlayerHandleChooseAction(enum BattlerId battler)
{
    BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, TestRunner_Battle_GetAction(TEST_BATTLE_ACTION_TYPE, battler), 0);
    BtlController_Complete(battler);
}

static void TestPlayerHandleChooseMove(enum BattlerId battler)
{
    enum MoveSlot moveIndex = (enum MoveSlot)TestRunner_Battle_GetAction(TEST_BATTLE_MOVE_SLOT, battler);
    u8 target = TestRunner_Battle_GetAction(TEST_BATTLE_MOVE_TARGET, battler);
    if (target == TEST_BATTLE_DEFAULT_TARGET)
    {
        struct ChooseMoveStruct *moveInfo = (struct ChooseMoveStruct *)(&gBattleResources->bufferA[battler][4]);
        target = GetDefaultSelectionTarget(battler, GetBattlerMoveSelectionTargetType(battler, moveInfo->moves[moveIndex & ~RET_GIMMICK]));
    }
    BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT, moveIndex | (target << 8));
    BtlController_Complete(battler);
}

static void TestPlayerHandleChooseItem(enum BattlerId battler)
{
    u8 byte1 = TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_ID, battler);
    u8 byte2 = TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_ID, battler);
    gBattleStruct->chosenItem[battler] = (byte1 << 8) | byte2;

    if (TESTING)
    {
        assertf(CheckBagHasItem(gBattleStruct->chosenItem[battler], 1), "Tried to used an item not present in bag");
        if (!GetItemImportance(gBattleStruct->chosenItem[battler]))
            RemoveBagItem(gBattleStruct->chosenItem[battler], 1);
    }

    gBattleStruct->itemPartyIndex[battler] = (enum PartyMon)TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_TARGET, battler);
    gBattleStruct->itemMoveIndex[battler] = (enum MoveSlot)TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_MOVE, battler);
    BtlController_EmitOneReturnValue(battler, B_COMM_TO_ENGINE, gBattleStruct->chosenItem[battler]);
    BtlController_Complete(battler);
}

static void TestPlayerHandleChoosePokemon(enum BattlerId battler)
{
    gBattleStruct->monToSwitchIntoId[battler] = (enum PartyMon)TestRunner_Battle_GetAction(TEST_BATTLE_PARTY_INDEX, battler);
    gSelectedMonPartyId = gBattleStruct->monToSwitchIntoId[battler]; // Revival Blessing
    BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, gBattleStruct->monToSwitchIntoId[battler], NULL);
    BtlController_Complete(battler);
}

static void TestPlayerHandleStatusAnimation(enum BattlerId battler)
{
    BtlController_HandleStatusAnimation(battler);
}

static void TestPlayerHandleIntroTrainerBallThrow(enum BattlerId battler)
{
    enum TrainerPicID trainerPicId = GetPlayerTrainerPic(gSaveData.playerGender, GAME_VERSION);
    const u16 *trainerPal = GetTrainerBackPicPalette(trainerPicId);

    BtlController_HandleIntroTrainerBallThrow(battler, 0xD6F9, trainerPal, 24, Intro_TryShinyAnimShowHealthbox);
}

static void TestPlayerHandleDrawPartyStatusSummary(enum BattlerId battler)
{
    BtlController_HandleDrawPartyStatusSummary(battler, B_SIDE_PLAYER, TRUE);
}

static void TestPlayerHandleEndLinkBattle(enum BattlerId battler)
{
    gBattleOutcome = gBattleResources->bufferA[battler][1];
    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    BtlController_Complete(battler);
    gBattlerControllerFuncs[battler] = SetBattleEndCallbacks;
}
