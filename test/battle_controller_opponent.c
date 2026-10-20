#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_message.h"
#include "battle_setup.h"
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
#include "util.h"
#include "window.h"
#include "constants/battle_anim.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "test/battle.h"

static void TestOpponentHandleDrawTrainerPic(enum BattlerId battler);
static void TestOpponentHandleTrainerSlideBack(enum BattlerId battler);
static void TestOpponentHandleChooseAction(enum BattlerId battler);
static void TestOpponentHandleChooseMove(enum BattlerId battler);
static void TestOpponentHandleChooseItem(enum BattlerId battler);
static void TestOpponentHandleChoosePokemon(enum BattlerId battler);
static void TestOpponentHandleStatusAnimation(enum BattlerId battler);
static void TestOpponentHandleIntroTrainerBallThrow(enum BattlerId battler);
static void TestOpponentHandleDrawPartyStatusSummary(enum BattlerId battler);
static void TestOpponentHandleEndLinkBattle(enum BattlerId battler);

static void TestOpponentBufferRunCommand(enum BattlerId battler);

static void (*const sTestOpponentBufferCommands[CONTROLLER_CMDS_COUNT])(enum BattlerId battler) =
{
    [CONTROLLER_GETMONDATA]               = BtlController_HandleGetMonData,
    [CONTROLLER_GETRAWMONDATA]            = BtlController_Empty,
    [CONTROLLER_SETMONDATA]               = BtlController_HandleSetMonData,
    [CONTROLLER_SETRAWMONDATA]            = BtlController_HandleSetRawMonData,
    [CONTROLLER_LOADMONSPRITE]            = BtlController_HandleLoadMonSprite,
    [CONTROLLER_SWITCHINANIM]             = BtlController_HandleSwitchInAnim,
    [CONTROLLER_RETURNMONTOBALL]          = BtlController_HandleReturnMonToBall,
    [CONTROLLER_DRAWTRAINERPIC]           = TestOpponentHandleDrawTrainerPic,
    [CONTROLLER_TRAINERSLIDE]             = OpponentHandleTrainerSlide,
    [CONTROLLER_TRAINERSLIDEBACK]         = TestOpponentHandleTrainerSlideBack,
    [CONTROLLER_FAINTANIMATION]           = BtlController_HandleFaintAnimation,
    [CONTROLLER_PALETTEFADE]              = BtlController_Empty,
    [CONTROLLER_BALLTHROWANIM]            = BtlController_Empty,
    [CONTROLLER_PAUSE]                    = BtlController_Empty,
    [CONTROLLER_MOVEANIMATION]            = BtlController_HandleMoveAnimation,
    [CONTROLLER_PRINTSTRING]              = BtlController_HandlePrintString,
    [CONTROLLER_PRINTSTRINGPLAYERONLY]    = BtlController_Empty,
    [CONTROLLER_CHOOSEACTION]             = TestOpponentHandleChooseAction,
    [CONTROLLER_YESNOBOX]                 = BtlController_Empty,
    [CONTROLLER_CHOOSEMOVE]               = TestOpponentHandleChooseMove,
    [CONTROLLER_OPENBAG]                  = TestOpponentHandleChooseItem,
    [CONTROLLER_CHOOSEPOKEMON]            = TestOpponentHandleChoosePokemon,
    [CONTROLLER_23]                       = BtlController_Empty,
    [CONTROLLER_HEALTHBARUPDATE]          = BtlController_HandleHealthBarUpdate,
    [CONTROLLER_EXPUPDATE]                = BtlController_Empty,
    [CONTROLLER_STATUSICONUPDATE]         = BtlController_HandleStatusIconUpdate,
    [CONTROLLER_STATUSANIMATION]          = TestOpponentHandleStatusAnimation,
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
    [CONTROLLER_INTROTRAINERBALLTHROW]    = TestOpponentHandleIntroTrainerBallThrow,
    [CONTROLLER_DRAWPARTYSTATUSSUMMARY]   = TestOpponentHandleDrawPartyStatusSummary,
    [CONTROLLER_HIDEPARTYSTATUSSUMMARY]   = BtlController_HandleHidePartyStatusSummary,
    [CONTROLLER_ENDBOUNCE]                = BtlController_Empty,
    [CONTROLLER_SPRITEINVISIBILITY]       = BtlController_HandleSpriteInvisibility,
    [CONTROLLER_BATTLEANIMATION]          = BtlController_HandleBattleAnimation,
    [CONTROLLER_LINKSTANDBYMSG]           = BtlController_Empty,
    [CONTROLLER_RESETACTIONMOVESELECTION] = BtlController_Empty,
    [CONTROLLER_ENDLINKBATTLE]            = TestOpponentHandleEndLinkBattle,
    [CONTROLLER_DEBUGMENU]                = BtlController_Empty,
    [CONTROLLER_TERMINATOR_NOP]           = BtlController_TerminatorNop
};

void SetControllerToTestOpponent(enum BattlerId battler)
{
    gBattlerBattleController[battler] = BATTLE_CONTROLLER_TEST_OPPONENT;
    gBattlerControllerEndFuncs[battler] = TestOpponentBufferExecCompleted;
    gBattlerControllerFuncs[battler] = TestOpponentBufferRunCommand;
}

static void TestOpponentBufferRunCommand(enum BattlerId battler)
{
    if (IsBattleControllerActiveOnLocal(battler))
    {
        if (gBattleResources->bufferA[battler][0] < ARRAY_COUNT(sTestOpponentBufferCommands))
            sTestOpponentBufferCommands[gBattleResources->bufferA[battler][0]](battler);
        else
            BtlController_Complete(battler);
    }
}

void TestOpponentBufferExecCompleted(enum BattlerId battler)
{
    gBattlerControllerFuncs[battler] = TestOpponentBufferRunCommand;
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        u8 playerId = GetMultiplayerId();

        PrepareBufferDataTransferLink(battler, 2, 4, &playerId);
        gBattleResources->bufferA[battler][0] = CONTROLLER_TERMINATOR_NOP;
    }
    else
    {
        MarkBattleControllerIdleOnLocal(battler);
    }
}

static void Intro_WaitForShinyAnimAndHealthbox(enum BattlerId battler)
{
    bool8 healthboxAnimDone = FALSE;

    if (!IsDoubleBattle() || (IsDoubleBattle() && (gBattleTypeFlags & BATTLE_TYPE_MULTI)))
    {
        if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy
         && gSprites[gBattlerSpriteIds[battler]].animEnded)
            healthboxAnimDone = TRUE;

    }
    else
    {
        if (gSprites[gHealthboxSpriteIds[battler]].callback == SpriteCallbackDummy
         && gSprites[gHealthboxSpriteIds[GetBattleIntroPartner(battler)]].callback == SpriteCallbackDummy
         && gSprites[gBattlerSpriteIds[battler]].animEnded
         && gSprites[gBattlerSpriteIds[GetBattleIntroPartner(battler)]].animEnded)
             healthboxAnimDone = TRUE;
    }

    if (healthboxAnimDone)
    {
        if (GetBattlerPosition(battler) == B_POSITION_OPPONENT_LEFT)
        {
            if (!gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim)
                return;
            if (!gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].finishedShinyMonAnim)
                return;

            gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim = FALSE;
            gBattleSpritesDataPtr->healthBoxesData[battler].finishedShinyMonAnim = FALSE;
            gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].triedShinyMonAnim = FALSE;
            gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].finishedShinyMonAnim = FALSE;
            FreeShinyStars();
        }

        gBattleSpritesDataPtr->healthBoxesData[battler].introEndDelay = 3;
        gBattlerControllerFuncs[battler] = BtlController_Intro_DelayAndEnd;
    }
}

static void Intro_TryShinyAnimShowHealthbox(enum BattlerId battler)
{
    bool32 bgmRestored = FALSE;
    bool32 battlerAnimsDone = FALSE;

    if (!gBattleSpritesDataPtr->healthBoxesData[battler].triedShinyMonAnim
     && !gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive)
        TryShinyAnimation(battler, GetBattlerMon(battler));

    if (!gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].triedShinyMonAnim
     && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].ballAnimActive)
        TryShinyAnimation(GetBattleIntroPartner(battler), GetBattlerMon(GetBattleIntroPartner(battler)));

    if (!gBattleSpritesDataPtr->healthBoxesData[battler].ballAnimActive && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].ballAnimActive)
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

    if (!gBattleSpritesDataPtr->healthBoxesData[battler].waitForCry
        && gBattleSpritesDataPtr->healthBoxesData[battler].healthboxSlideInStarted
        && !gBattleSpritesDataPtr->healthBoxesData[GetBattleIntroPartner(battler)].waitForCry
        && !IsCryPlayingOrClearCrySongs())
    {
        if (!gBattleSpritesDataPtr->healthBoxesData[battler].bgmRestored)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI && gBattleTypeFlags & BATTLE_TYPE_LINK)
            {
                if (GetBattlerPosition(battler) == B_POSITION_OPPONENT_LEFT)
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

    if (!IsDoubleBattle())
    {
        if (gSprites[gBattleControllerData[battler]].callback == SpriteCallbackDummy)
        {
            TrySetBattlerShadowSpriteCallback(battler);
            if (gSprites[gBattlerSpriteIds[battler]].callback == SpriteCallbackDummy)
                battlerAnimsDone = TRUE;
        }
    }
    else
    {
        if (gSprites[gBattleControllerData[battler]].callback == SpriteCallbackDummy
            && gSprites[gBattleControllerData[GetBattleIntroPartner(battler)]].callback == SpriteCallbackDummy)
        {
            TrySetBattlerShadowSpriteCallback(battler);
            TrySetBattlerShadowSpriteCallback(GetBattleIntroPartner(battler));
            if (gSprites[gBattlerSpriteIds[battler]].callback == SpriteCallbackDummy
                && gSprites[gBattlerSpriteIds[GetBattleIntroPartner(battler)]].callback == SpriteCallbackDummy)
            {
                battlerAnimsDone = TRUE;
            }
        }
    }

    if (bgmRestored && battlerAnimsDone)
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

static void TestOpponentHandleDrawTrainerPic(enum BattlerId battler)
{
    bool32 isLeft = GetBattlerPosition(battler) == B_POSITION_OPPONENT_LEFT;
    enum TrainerPicID trainerPicId = isLeft ? TRAINER_PIC_LEAF : TRAINER_PIC_RED;
    s16 xPos = isLeft ? ((gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) ? 200 : 176) : 152;

    BtlController_HandleDrawTrainerPic(battler, trainerPicId, TRUE, xPos, 40, -1);
}

static void TestOpponentHandleTrainerSlideBack(enum BattlerId battler)
{
    BtlController_HandleTrainerSlideBack(battler, 35, FALSE);
}

static void TestOpponentHandleChooseAction(enum BattlerId battler)
{
    BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, TestRunner_Battle_GetAction(TEST_BATTLE_ACTION_TYPE, battler), 0);
    BtlController_Complete(battler);
}

static void TestOpponentHandleChooseMove(enum BattlerId battler)
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

static void TestOpponentHandleChooseItem(enum BattlerId battler)
{
    u8 byte1 = TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_ID, battler);
    u8 byte2 = TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_ID, battler);
    gBattleStruct->chosenItem[battler] = (byte1 << 8) | byte2;
    gBattleStruct->itemPartyIndex[battler] = (enum PartySlot)TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_TARGET, battler);
    gBattleStruct->itemMoveIndex[battler] = (enum MoveSlot)TestRunner_Battle_GetAction(TEST_BATTLE_ITEM_MOVE, battler);
    BtlController_EmitOneReturnValue(battler, B_COMM_TO_ENGINE, gBattleStruct->chosenItem[battler]);
    BtlController_Complete(battler);
}

static void TestOpponentHandleChoosePokemon(enum BattlerId battler)
{
    gBattleStruct->monToSwitchIntoId[battler] = (enum PartySlot)TestRunner_Battle_GetAction(TEST_BATTLE_PARTY_INDEX, battler);
    gSelectedMonPartyId = gBattleStruct->monToSwitchIntoId[battler]; // Revival Blessing
    BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, gBattleStruct->monToSwitchIntoId[battler], NULL);
    BtlController_Complete(battler);
}

static void TestOpponentHandleStatusAnimation(enum BattlerId battler)
{
    BtlController_HandleStatusAnimation(battler);
}

static void TestOpponentHandleIntroTrainerBallThrow(enum BattlerId battler)
{
    BtlController_HandleIntroTrainerBallThrow(battler, 0, NULL, 0, Intro_TryShinyAnimShowHealthbox);
}

static void TestOpponentHandleDrawPartyStatusSummary(enum BattlerId battler)
{
    BtlController_HandleDrawPartyStatusSummary(battler, B_SIDE_OPPONENT, TRUE);
}

static void TestOpponentHandleEndLinkBattle(enum BattlerId battler)
{
    if (gBattleResources->bufferA[battler][1] == B_OUTCOME_DREW)
        gBattleOutcome = gBattleResources->bufferA[battler][1];
    else
        gBattleOutcome = gBattleResources->bufferA[battler][1] ^ B_OUTCOME_DREW;

    FadeOutMapMusic(5);
    BeginFastPaletteFade(3);
    BtlController_Complete(battler);
    gBattlerControllerFuncs[battler] = SetBattleEndCallbacks;
}
