#include "global.h"
#include "battle_i_choose_you.h"
#include "battle.h"
#include "battle_environment.h"
#include "battle_hold_effects.h"
#include "battle_ai_record.h"
#include "battle_util.h"
#include "battle_move_resolution.h"
#include "battle_script_commands.h"
#include "battle_stat_change.h"
#include "battle_switch_in.h"
#include "battle_scripts.h"
#include "battle_anim_scripts.h"
#include "battle_main.h"
#include "item.h"
#include "battle_controllers.h"
#include "move.h"
#include "test_runner.h"
#include "overworld.h"
#include "constants/battle_move_resolution.h"
#include "constants/songs.h"

static void ValidateBattlers(void);
static enum Move GetOriginallyUsedMove(enum Move chosenMove);
static void SetSameMoveTurnValues(enum BattleMoveEffects moveEffect);
static void TryClearChargeVolatile(enum Type moveType);
static inline bool32 IsBattlerUsingBeakBlast(enum BattlerId battler);
static void RequestNonVolatileChange(enum BattlerId battlerAtk);
static bool32 CanBattlerBounceBackMove(struct BattleCalcValues *cv);
static bool32 TryMagicBounce(struct BattleCalcValues *cv);
static bool32 TryMagicCoat(struct BattleCalcValues *cv);
static bool32 TryActivatePowderStatus(enum Move move);
static void CalculateMagnitudeDamage(void);
static void UpdateStallMons(struct BattleCalcValues *cv);
static void SetDamageContextValues(struct DamageContext *ctx, struct BattleCalcValues *cv);

// Submoves
static enum Move GetMirrorMoveMove(void);
static enum Move GetMetronomeMove(void);
static enum Move GetAssistMove(void);
static enum Move GetSleepTalkMove(void);
static enum Move GetCopycatMove(void);
static enum Move GetMeFirstMove(void);

// Move End functions shared by Substitute Block
static bool32 ShouldApplyAfterHitEffects(enum BattlerId battlerAtk, enum BattlerId effectBattler);
static bool32 CanApplyAdditionalEffect(enum BattlerId battlerAtk, enum BattlerId effectBattler, const struct AdditionalEffect *additionalEffect);
static void TryTriggerAdditionalEffect(struct BattleCalcValues *cv, const struct AdditionalEffect *additionalEffect, enum BattlerId effectBattler);
static bool32 IsDamageMoveSelfStatChange(bool32 self, enum MoveEffect moveEffect);

static bool32 ShouldApplyProtectLikeEffects(enum BattlerId battlerDef, struct BattleCalcValues *cv);
static bool32 ShouldPrintCritMessage(enum BattlerId battler);
static bool32 ShouldPrintProtectMessage(enum BattlerId battler);
static bool32 ShouldPrintEffectivenessMessage(struct BattleCalcValues *cv);

// Stat change moves
static bool32 IsStatChangeMove(enum BattlerId battlerAtk, enum Move move);
static bool32 TryBellyDrum(enum BattlerId battler);
static bool32 TryHalfHp(enum BattlerId battler);
static bool32 CutThirdOfHp(enum BattlerId battler);

// ==============
// Attackcanceler
// ==============

static enum CancelerResult CancelerClearFlags(struct BattleCalcValues *cv)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        gBattleStruct->battlerState[battler].shellBurstBlockedMove = FALSE;
    gBattleStruct->relentlessChaseDamage = 0;
    memset(gBattleStruct->technicolorMoveTypes, TYPE_NONE, sizeof(gBattleStruct->technicolorMoveTypes));
    gProtectStructs[cv->battlerAtk].unbrokenStoneBoosted = FALSE;
    gBattleMons[cv->battlerAtk].volatiles.grudge = FALSE;
    gBattleMons[cv->battlerAtk].volatiles.glaiveRush = FALSE;
    gBattleStruct->eventState.atkCancelerBattler = 0;
    gBattleStruct->eventState.moveEndBlock = 0;
    if (gSpecialStatuses[cv->battlerAtk].postActionAbilityMove)
        gBattleStruct->eventState.atkCanceler = (cv->abilities[cv->battlerAtk] == ABILITY_TONGUE_LASH ? CANCELER_STANCE_CHANGE_2 : CANCELER_DISABLED) - 1;
    return CANCELER_RESULT_SUCCESS;
}

static bool32 TryFormChangeBeforeMove(void)
{
    enum Ability ability = GetBattlerAbility(gBattlerAttacker);

    if (TryBattleFormChange(gBattlerAttacker, FORM_CHANGE_BATTLE_BEFORE_MOVE, ability)
        || TryBattleFormChange(gBattlerAttacker, FORM_CHANGE_BATTLE_BEFORE_MOVE_CATEGORY, ability))
    {
        gBattleScripting.battler = gBattlerAttacker;
        BattleScriptCall(BattleScript_BattlerFormChange);
        return TRUE;
    }
    return FALSE;
}

static enum CancelerResult CancelerStanceChangeOne(struct BattleCalcValues *cv)
{
    if (B_STANCE_CHANGE_FAIL < GEN_7 && gChosenMove == cv->move && TryFormChangeBeforeMove())
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerSkyDrop(struct BattleCalcValues *cv)
{
    // If Pokemon is being held in Sky Drop
    if (gBattleMons[cv->battlerAtk].volatiles.semiInvulnerable == STATE_SKY_DROP_TARGET)
    {
        gBattlescriptCurrInstr = BattleScript_MoveEnd;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerRecharge(struct BattleCalcValues *cv)
{
    if (AbilitySkipsRecharge(cv->abilities[cv->battlerAtk]))
    {
        gBattleMons[cv->battlerAtk].volatiles.rechargeTimer = 0;
        return CANCELER_RESULT_SUCCESS;
    }
    if (gBattleMons[cv->battlerAtk].volatiles.rechargeTimer > 0)
    {
        if (GetConfig(B_TRUANT) >= GEN_5 && cv->abilities[cv->battlerAtk] == ABILITY_TRUANT)
            gBattleMons[cv->battlerAtk].volatiles.truantToggle = 0;
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedMustRecharge;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerChillyReception(struct BattleCalcValues *cv)
{
    if (cv->moveEffect == EFFECT_WEATHER_AND_SWITCH)
    {
        BattleScriptCall(BattleScript_ChillyReceptionMessage);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    return CANCELER_RESULT_SUCCESS;
}

static void DefrostBattler(enum BattlerId battler, u32 status)
{
    GetBattlerPartyState(battler)->freezeTurns = 0;
    gBattleScripting.battler = battler;
    gBattleMons[battler].status1 &= ~status;
    RequestNonVolatileChange(battler);
}

static enum CancelerResult CancelerAsleepOrFrozen(struct BattleCalcValues *cv)
{
    enum CancelerResult result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;

    if (gBattleMons[cv->battlerAtk].status1 & STATUS1_SLEEP)
    {
        if (GetConfig(B_UPROAR) < GEN_5 && UproarWakeUpCheck(cv->battlerAtk))
        {
            TryDeactivateSleepClause(cv->battlerAtk, gBattlerPartyIndexes[cv->battlerAtk]);
            gBattleMons[cv->battlerAtk].status1 &= ~STATUS1_SLEEP;
            gBattleMons[cv->battlerAtk].volatiles.nightmare = FALSE;
            gEffectBattler = cv->battlerAtk;
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WOKE_UP_UPROAR;
            result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
            BattleScriptCall(BattleScript_MoveUsedWokeUp);
        }
        else
        {
            u32 toSub;
            if (IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_EARLY_BIRD))
                toSub = 2;
            else
                toSub = 1;

            if ((gBattleMons[cv->battlerAtk].status1 & STATUS1_SLEEP) < toSub)
                gBattleMons[cv->battlerAtk].status1 &= ~STATUS1_SLEEP;
            else
                gBattleMons[cv->battlerAtk].status1 -= toSub;

            if (gBattleMons[cv->battlerAtk].status1 & STATUS1_SLEEP)
            {
                if (cv->moveEffect == EFFECT_SNORE)
                {
                    BattleScriptCall(BattleScript_BeforeSnoreMessage);
                    result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
                }
                else if (cv->moveEffect == EFFECT_SLEEP_TALK || cv->abilities[cv->battlerAtk] == ABILITY_SPICED_DREAMS)
                {
                    result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
                }
                else
                {
                    gBattlescriptCurrInstr = BattleScript_MoveUsedIsAsleep;
                    result = CANCELER_RESULT_FAILURE;
                }
            }
            else
            {
                TryDeactivateSleepClause(cv->battlerAtk, gBattlerPartyIndexes[cv->battlerAtk]);
                gBattleMons[cv->battlerAtk].volatiles.nightmare = FALSE;
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_WOKE_UP;
                result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
                BattleScriptCall(BattleScript_MoveUsedWokeUp);
            }
        }
        RequestNonVolatileChange(cv->battlerAtk);
    }
    else if (gBattleMons[cv->battlerAtk].status1 & STATUS1_FREEZE && !MoveThawsUser(cv->move))
    {
        if (!RandomPercentage(RNG_FROZEN, (GetConfig(B_FREEZE_TURNS) >= GEN_CHAMPIONS ? 25 : 20)) && GetBattlerPartyState(cv->battlerAtk)->freezeTurns < 2)
        {
            result = CANCELER_RESULT_FAILURE;
            if (GetConfig(B_FREEZE_TURNS) >= GEN_CHAMPIONS)
                GetBattlerPartyState(cv->battlerAtk)->freezeTurns++;
            gBattlescriptCurrInstr = BattleScript_MoveUsedIsFrozen;
        }
        else // unfreeze
        {
            DefrostBattler(cv->battlerAtk, STATUS1_FREEZE);
            result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
            BattleScriptCall(BattleScript_BattlerDefrosted);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROSTED;
        }
    }

    return result;
}

static enum CancelerResult CancelerObedience(struct BattleCalcValues *cv)
{
    if (!gBattleMons[cv->battlerAtk].volatiles.multipleTurns)
    {
        enum Obedience obedienceResult = GetAttackerObedienceForAction();
        switch (obedienceResult)
        {
        case OBEYS:
            return CANCELER_RESULT_SUCCESS;
        case DISOBEYS_LOAFS:
            // Randomly select, then print a disobedient string
            // B_MSG_LOAFING, B_MSG_WONT_OBEY, B_MSG_TURNED_AWAY, or B_MSG_PRETEND_NOT_NOTICE
            gBattleCommunication[MULTISTRING_CHOOSER] = MOD(Random(), NUM_LOAF_STRINGS);
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
            AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
            return CANCELER_RESULT_FAILURE;
        case DISOBEYS_HITS_SELF:
            gBattlerTarget = cv->battlerAtk;
            struct DamageContext dmgCtx = {0};
            dmgCtx.battlerAtk = dmgCtx.battlerDef = cv->battlerAtk;
            dmgCtx.move = dmgCtx.chosenMove = MOVE_NONE;
            dmgCtx.moveType = TYPE_MYSTERY;
            dmgCtx.isCrit = FALSE;
            dmgCtx.randomFactor = FALSE;
            dmgCtx.updateFlags = TRUE;
            dmgCtx.isSelfInflicted = TRUE;
            dmgCtx.fixedBasePower = 40;
            gBattleStruct->moveDamage[cv->battlerAtk] = CalculateMoveDamage(&dmgCtx);
            gBattlescriptCurrInstr = BattleScript_IgnoresAndHitsItself;
            return CANCELER_RESULT_FAILURE; // Move doesn't fail but mon hits itself
        case DISOBEYS_FALL_ASLEEP:
            if (IsSleepClauseEnabled())
                gBattleStruct->battlerState[cv->battlerAtk].sleepClauseEffectExempt = TRUE;
            gBattlescriptCurrInstr = BattleScript_IgnoresAndFallsAsleep;
            AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
            return CANCELER_RESULT_FAILURE;
            break;
        case DISOBEYS_WHILE_ASLEEP:
            gBattlescriptCurrInstr = BattleScript_IgnoresWhileAsleep;
            AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
            return CANCELER_RESULT_FAILURE;
        case DISOBEYS_RANDOM_MOVE:
            gCurrentMove = gCalledMove = gBattleMons[cv->battlerAtk].moves[gCurrMovePos];
            BattleScriptCall(BattleScript_IgnoresAndUsesRandomMove);
            gBattlerTarget = GetBattleMoveTarget(gCalledMove, TARGET_NONE);
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerPowerPoints(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].pp[gCurrMovePos] == 0
     && cv->move != MOVE_STRUGGLE
     && !IsBattlerUsingAbilityMove(cv->battlerAtk)
     && !gBattleMons[cv->battlerAtk].volatiles.multipleTurns)
    {
        gBattlescriptCurrInstr = BattleScript_NoPPForMove;
        AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
        return CANCELER_RESULT_FAILURE;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerTruant(struct BattleCalcValues *cv)
{
    if (cv->abilities[cv->battlerAtk] != ABILITY_TRUANT)
        return CANCELER_RESULT_SUCCESS;

    bool32 shouldLoaf = gBattleMons[cv->battlerAtk].volatiles.truantToggle;
    if (GetConfig(B_TRUANT) >= GEN_5)
        gBattleMons[cv->battlerAtk].volatiles.truantToggle ^= 1;

    if (shouldLoaf)
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_LOAFING;
        gBattlerAbility = cv->battlerAtk;
        gBattlescriptCurrInstr = BattleScript_TruantLoafingAround;
        AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerFocus(struct BattleCalcValues *cv)
{
    u32 focusPunchFailureConfig = GetConfig(B_FOCUS_PUNCH_FAILURE);

    // In Gens 3-4, only check if is using Focus Punch.
    // In Gens 5-6, only check if the chosen move is Focus Punch.
    // In Gens 7+, check if chose and is using Focus Punch.
    if ((gProtectStructs[cv->battlerAtk].physicalDmg || gProtectStructs[cv->battlerAtk].specialDmg)
     && (focusPunchFailureConfig < GEN_5 || GetMoveEffect(gChosenMoveByBattler[cv->battlerAtk]) == EFFECT_FOCUS_PUNCH)
     && (focusPunchFailureConfig == GEN_5 || focusPunchFailureConfig == GEN_6 || cv->moveEffect == EFFECT_FOCUS_PUNCH)
     && !gProtectStructs[cv->battlerAtk].survivedOHKO)
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_FocusPunchLostFocus;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerFocusGen5(struct BattleCalcValues *cv)
{
    if (GetConfig(B_FOCUS_PUNCH_FAILURE) >= GEN_5)
        return CancelerFocus(cv);
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerFlinch(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].volatiles.flinched)
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        if (cv->abilities[cv->battlerAtk] == ABILITY_STEADFAST)
        {
            SetStatChange(cv->battlerAtk, STAT_SPEED, 1);
            gBattlerAbility = cv->battlerAtk;
            gBattlescriptCurrInstr = BattleScript_MoveUsedFlinchedAndSteadfast;
        }
        else
        {
            gBattlescriptCurrInstr = BattleScript_MoveUsedFlinched;
        }
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerDisabled(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].volatiles.disabledMove == MOVE_NONE && !gBattleStruct->hasNineTailLegendSeals)
        return CANCELER_RESULT_SUCCESS;

    if (GetConfig(B_DISABLE_TURNS) <= GEN_2 && gBattleMons[cv->battlerAtk].volatiles.disableTimer != 0)
        gBattleMons[cv->battlerAtk].volatiles.disableTimer--;

    if (gBattleMons[cv->battlerAtk].volatiles.disabledMove == cv->move
     || IsMoveNineTailLegendDisabled(cv->battlerAtk, cv->move))
    {
        gBattleScripting.battler = cv->battlerAtk;
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsDisabled;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerVolatileBlocked(struct BattleCalcValues *cv)
{
    enum CancelerResult result = CANCELER_RESULT_SUCCESS;

    if (IsHealBlockPreventingMove(cv->battlerAtk, cv->move))
    {
        gBattleScripting.battler = cv->battlerAtk;
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedHealBlockPrevents;
        result = CANCELER_RESULT_FAILURE;
    }
    else if (IsGravityPreventingMove(cv->move))
    {
        gBattleScripting.battler = cv->battlerAtk;
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedGravityPrevents;
        result = CANCELER_RESULT_FAILURE;
    }
    else if (gBattleMons[cv->battlerAtk].volatiles.throatChopTimer > 0 && IsSoundMove(cv->move))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsThroatChopPrevented;
        result = CANCELER_RESULT_FAILURE;
    }
    if (gBattleStruct->snatchedMoveIsUsed)
        gBattleStruct->eventState.atkCanceler = CANCELER_SET_TARGETS - 1; // minus one since state is incremented on success by default

    return result;
}

static enum CancelerResult CancelerTaunted(struct BattleCalcValues *cv)
{
    if (IsTauntPreventingMove(cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->move))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsTaunted;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerImprisoned(struct BattleCalcValues *cv)
{
    if (GetImprisonedMovesCount(cv->battlerAtk, cv->move))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsImprisoned;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerConfused(struct BattleCalcValues *cv)
{
    // Amped Bloom rechecks move restrictions without charging the user another turn.
    if (gSpecialStatuses[cv->battlerAtk].postActionAbilityMove)
    {
        gBattleStruct->eventState.atkCanceler = CANCELER_CHOICE_LOCK - 1;
        return CANCELER_RESULT_SUCCESS;
    }

    struct Volatiles *vol = &gBattleMons[cv->battlerAtk].volatiles;

    if (vol->confusionTimer)
    {
        if (vol->confusionTimer != PERMANENT_VOLATILE)
            vol->confusionTimer--;

        if (vol->confusionTimer)
        {
             // confusion dmg
            if (RandomPercentage(RNG_CONFUSION, (GetConfig(B_CONFUSION_SELF_DMG_CHANCE) >= GEN_7 ? 33 : 50)))
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = TRUE;
                gBattlerTarget = gBattlerAttacker;
                struct DamageContext dmgCtx = {0};
                dmgCtx.battlerAtk = dmgCtx.battlerDef = cv->battlerAtk;
                dmgCtx.move = dmgCtx.chosenMove = MOVE_NONE;
                dmgCtx.moveType = TYPE_MYSTERY;
                dmgCtx.isCrit = FALSE;
                dmgCtx.randomFactor = FALSE;
                dmgCtx.updateFlags = TRUE;
                dmgCtx.isSelfInflicted = TRUE;
                dmgCtx.fixedBasePower = 40;
                dmgCtx.abilities[gBattlerAttacker] = cv->abilities[gBattlerAttacker];
                dmgCtx.holdEffects[gBattlerAttacker] = cv->holdEffects[gBattlerAttacker];
                gBattleStruct->passiveHpUpdate[cv->battlerAtk] = CalculateMoveDamage(&dmgCtx);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfused;
                return CANCELER_RESULT_FAILURE;
            }
            else
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = FALSE;
                BattleScriptCall(BattleScript_MoveUsedIsConfused);
                return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
            }
        }
        else // snapped out of confusion
        {
            BattleScriptCall(BattleScript_MoveUsedIsConfusedNoMore);
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerGhost(struct BattleCalcValues *cv) // GHOST in Pokémon Tower
{
    if (IsGhostBattleWithoutScope())
    {
        if (GetBattlerSide(cv->battlerAtk) == B_SIDE_PLAYER)
            gBattlescriptCurrInstr = BattleScript_TooScaredToMove;
        else
            gBattlescriptCurrInstr = BattleScript_GhostGetOutGetOut;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerParalyzed(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].status1 & STATUS1_PARALYSIS
        && !(B_MAGIC_GUARD == GEN_4 && IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD))
        && (RandomWeighted(RNG_PARALYSIS, (GetConfig(B_PARALYSIS_CHANCE) >= GEN_CHAMPIONS ? 7 : 3), 1)))
    {
        CancelMultiTurnMoves(gBattlerAttacker);
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsParalyzed;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerInfatuation(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].volatiles.infatuation)
    {
        gBattleScripting.battler = gBattleMons[cv->battlerAtk].volatiles.infatuation - 1;
        if (!RandomPercentage(RNG_INFATUATION, 30))
        {
            BattleScriptCall(BattleScript_MoveUsedIsInLove);
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
        else
        {
            BattleScriptPush(BattleScript_MoveUsedIsInLoveCantAttack);
            CancelMultiTurnMoves(cv->battlerAtk);
            gBattlescriptCurrInstr = BattleScript_MoveUsedIsInLove;
            return CANCELER_RESULT_FAILURE;
        }
    }
    return CANCELER_RESULT_SUCCESS;
}


static enum CancelerResult CancelerChoiceLock(struct BattleCalcValues *cv)
{
    enum Move *choicedMoveAtk = &gBattleStruct->choicedMove[cv->battlerAtk];
    enum HoldEffect holdEffect = GetBattlerHoldEffect(cv->battlerAtk);

    if (gChosenMove != MOVE_STRUGGLE
     && (*choicedMoveAtk == MOVE_NONE || *choicedMoveAtk == MOVE_UNAVAILABLE)
     && (IsHoldEffectChoice(holdEffect) || cv->abilities[cv->battlerAtk] == ABILITY_GORILLA_TACTICS))
        *choicedMoveAtk = gChosenMove;

    u32 moveIndex;
    for (moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
    {
        if (gBattleMons[cv->battlerAtk].moves[moveIndex] == *choicedMoveAtk)
            break;
    }

    if (moveIndex == MAX_MON_MOVES)
        *choicedMoveAtk = MOVE_NONE;

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerCallSubmove(struct BattleCalcValues *cv)
{
    bool32 noEffect = FALSE;
    enum Move calledMove = MOVE_NONE;
    const u8 *battleScript = NULL;
    battleScript = BattleScript_SubmoveAttackstring;

    switch (cv->moveEffect)
    {
    case EFFECT_MIRROR_MOVE:
        calledMove = GetMirrorMoveMove();
        break;
    case EFFECT_METRONOME:
        calledMove = GetMetronomeMove();
        battleScript = BattleScript_MetronomeAttackstring;
        break;
    case EFFECT_ASSIST:
        calledMove = GetAssistMove();
        break;
    case EFFECT_NATURE_POWER:
        calledMove = GetNaturePowerMove();
        battleScript = BattleScript_NaturePowerAttackstring;
        break;
    case EFFECT_SLEEP_TALK:
        calledMove = GetSleepTalkMove();
        battleScript = BattleScript_SleepTalkAttackstring;
        break;
    case EFFECT_COPYCAT:
        calledMove = GetCopycatMove();
        break;
    case EFFECT_ME_FIRST:
        calledMove = GetMeFirstMove();
        break;
    case EFFECT_DARK_IMITATION:
        if (gChosenActionByBattler[cv->battlerDef] == B_ACTION_USE_MOVE
         && !gProtectStructs[cv->battlerDef].noValidMoves)
        {
            enum Move selected = GetBattlerChosenMove(cv->battlerDef);
            if (!IsMoveDarkImitationBanned(selected))
            {
                // Unlike accuracy-free callers, Dark Imitation must first hit its target.
                if (DoesMoveMissTarget(cv))
                    AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);
                else
                    calledMove = selected;
            }
        }
        break;
    default:
        noEffect = TRUE;
        break;
    }

    if (noEffect)
    {
        gBattleStruct->submoveAnnouncement = SUBMOVE_NO_EFFECT;
        return CANCELER_RESULT_SUCCESS;
    }

    if (calledMove != MOVE_NONE)
    {
        ;
        gBattleStruct->submoveAnnouncement = SUBMOVE_SUCCESS;
        gCalledMove = calledMove;
        if (GetBattlerMoveTargetType(cv->battlerAtk, cv->move) == TARGET_DEPENDS) // originally using a move without a set target
            gBattlerTarget = GetBattleMoveTarget(calledMove, TARGET_NONE);
        BattleScriptCall(battleScript);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    gBattleStruct->submoveAnnouncement = SUBMOVE_FAILURE;
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerThaw(struct BattleCalcValues *cv)
{
    enum CancelerResult result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;

    if (MoveThawsUser(cv->move))
    {
        if (gBattleMons[cv->battlerAtk].status1 & STATUS1_FREEZE)
        {
            if (!IsMoveEffectRemoveSpeciesType(cv->move, MOVE_EFFECT_REMOVE_ARG_TYPE, TYPE_FIRE) || IS_BATTLER_OF_TYPE(cv->battlerAtk, TYPE_FIRE))
            {
                DefrostBattler(cv->battlerAtk, STATUS1_FREEZE);
                result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
                BattleScriptCall(BattleScript_BattlerDefrosted);
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROSTED_BY_MOVE;
            }
            else
            {
                result = CANCELER_RESULT_FAILURE;
            }
        }
        else if (gBattleMons[cv->battlerAtk].status1 & STATUS1_FROSTBITE)
        {
            if (!IsMoveEffectRemoveSpeciesType(cv->move, MOVE_EFFECT_REMOVE_ARG_TYPE, TYPE_FIRE) || IS_BATTLER_OF_TYPE(cv->battlerAtk, TYPE_FIRE))
            {
                DefrostBattler(cv->battlerAtk, STATUS1_FROSTBITE);
                result = CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
                BattleScriptCall(BattleScript_BattlerFrostbiteHealed);
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FROSTBITE_HEALED_BY_MOVE;
            }
            else
            {
                result = CANCELER_RESULT_FAILURE;
            }
        }
    }
    return result;
}

static enum CancelerResult CancelerStanceChangeTwo(struct BattleCalcValues *cv)
{
    if (B_STANCE_CHANGE_FAIL >= GEN_7 && gChosenMove == cv->move && TryFormChangeBeforeMove())
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerAttackstring(struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].volatiles.bideTurns)
        return CANCELER_RESULT_SUCCESS;

    BattleScriptCall(BattleScript_Attackstring);
    if (!IsBattlerUsingAbilityMove(gBattlerAttacker))
    {
        gBattleMons[gBattlerAttacker].volatiles.usedMoves |= 1u << gCurrMovePos;
        gBattleStruct->battlerState[gBattlerAttacker].lastMoveTarget = gBattlerTarget;
        gLastPrintedMoves[gBattlerAttacker] = gChosenMove;
        RecordKnownMove(gBattlerAttacker, gChosenMove);
    }
    else if ((gSpecialStatuses[gBattlerAttacker].postActionAbilityMove && cv->abilities[cv->battlerAtk] != ABILITY_TONGUE_LASH)
          || (gBattleStruct->endTurnAbilityMoveActive && gBattleStruct->endTurnAbilityMoveBattler == gBattlerAttacker
           && cv->abilities[cv->battlerAtk] == ABILITY_ENDLESS_LUAU))
    {
        // These automatic moves are known by the user, unlike a Dancer copy.
        RecordKnownMove(gBattlerAttacker, cv->move);
    }
    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
}

static enum CancelerResult CancelerPledgeAttack(struct BattleCalcValues *cv)
{
    if (gBattleStruct->pledgeState == PLEDGE_COMBO_WAITING)
    {
        enum Move partnerMove = GetBattlerChosenMove(GetPartnerBattler(cv->battlerAtk));

        if (GetPledgeComboMove(cv->move) == partnerMove)
            gCurrentMove = GetPledgeResultMove(cv->move);
        else
            gCurrentMove = GetPledgeResultMove(partnerMove);

        gBattleStruct->pledgeState = PLEDGE_COMBO_ATTACK;
        gBattlescriptCurrInstr = BattleScript_MoveResolution;
        BattleScriptCall(BattleScript_EffectHitCombinedPledge);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    return CANCELER_RESULT_SUCCESS;
}

enum FailureCheck IsSingleTarget(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerDef != gBattlerTarget)
        return SKIP_MOVE_FAILURE_CHECK;
    return CHECK_MOVE_FAILURE;
}

enum FailureCheck IsSmartTarget(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (!IsBattlerAlly(gBattlerTarget, battlerDef) || battlerAtk == battlerDef)
        return SKIP_MOVE_FAILURE_CHECK;
    return CHECK_MOVE_FAILURE;
}

enum FailureCheck IsTargetingBothFoes(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerDef == GetPartnerBattler(battlerAtk) || battlerAtk == battlerDef)
        return SKIP_MOVE_FAILURE_CHECK;
    return CHECK_MOVE_FAILURE;
}

enum FailureCheck IsTargetingSelf(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef)
        return IGNORE_USER_MOVE_FAILURE;
    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingAlly(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerDef == GetPartnerBattler(battlerAtk))
        return CHECK_MOVE_FAILURE;
    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingSelfAndAlly(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerDef == GetPartnerBattler(battlerAtk))
        return CHECK_MOVE_FAILURE;

    if (battlerAtk == battlerDef && GetConfig(B_CHECK_USER_FAILURE) <= GEN_4)
        return CHECK_MOVE_FAILURE;

    if (battlerAtk == battlerDef)
        return IGNORE_USER_MOVE_FAILURE;

    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingSelfOrAlly(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef && battlerAtk == gBattlerTarget)
        return IGNORE_USER_MOVE_FAILURE;

    if (battlerDef == gBattlerTarget && battlerDef == GetPartnerBattler(battlerAtk))
        return CHECK_MOVE_FAILURE;

    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingFoesAndAlly(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef)
        return SKIP_MOVE_FAILURE_CHECK;
    return CHECK_MOVE_FAILURE;
}

enum FailureCheck IsTargetingField(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef)
        return IGNORE_USER_MOVE_FAILURE;
    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingOpponentsField(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (IsBattlerAlly(battlerDef, GetOppositeBattler(battlerAtk)))
        return CHECK_MOVE_FAILURE;
    return SKIP_MOVE_FAILURE_CHECK;
}

enum FailureCheck IsTargetingAllBattlers(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef && GetConfig(B_CHECK_USER_FAILURE) >= GEN_5)
        return IGNORE_USER_MOVE_FAILURE;
    return CHECK_MOVE_FAILURE;
}

enum FailureCheck (*const sSetTargetMoveFailureChecks[])(enum BattlerId battlerAtk, enum BattlerId battlerDef) =
{
    [TARGET_NONE] = IsTargetingField,
    [TARGET_SELECTED] = IsSingleTarget,
    [TARGET_DEPENDS] = IsSingleTarget,
    [TARGET_OPPONENT] = IsSingleTarget,
    [TARGET_RANDOM] = IsSingleTarget,
    [TARGET_BOTH] = IsTargetingBothFoes,
    [TARGET_USER] = IsTargetingSelf,
    [TARGET_SMART] = IsSmartTarget,
    [TARGET_ALLY] = IsTargetingAlly,
    [TARGET_USER_AND_ALLY] = IsTargetingSelfAndAlly,
    [TARGET_USER_OR_ALLY] = IsTargetingSelfOrAlly,
    [TARGET_FOES_AND_ALLY] = IsTargetingFoesAndAlly,
    [TARGET_FIELD] = IsTargetingField,
    [TARGET_OPPONENTS_FIELD] = IsTargetingOpponentsField,
    [TARGET_ALL_BATTLERS] = IsTargetingAllBattlers,
};

void SetTargetMoveFailureFlags(enum BattlerId battlerAtk, enum MoveTarget moveTarget)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        enum FailureCheck check = sSetTargetMoveFailureChecks[moveTarget](battlerAtk, battlerDef);
        bool32 targetNotPresent = moveTarget != TARGET_FIELD && moveTarget != TARGET_OPPONENTS_FIELD && !IsBattlerAlive(battlerDef);

        if (IsBattlerUnaffectedByMove(battlerDef))
        {
            AddBattlerMoveResultFlag(battlerDef, MOVE_RESULT_NOT_TARGETED); // For Bounced moves
        }
        else if (check == IGNORE_USER_MOVE_FAILURE)
        {
            AddBattlerMoveResultFlag(battlerDef, MOVE_RESULT_IGNORE_SELF);
        }
        else if (check == SKIP_MOVE_FAILURE_CHECK || targetNotPresent)
        {
            AddBattlerMoveResultFlag(battlerDef, MOVE_RESULT_NOT_TARGETED);
        }
    }
}

static inline bool32 IsSmartTargetMoveConsecutiveHit(enum BattlerId battlerAtk, enum Move move)
{
    if (GetBattlerMoveTargetType(battlerAtk, move) != TARGET_SMART)
        return FALSE;

    if (gMultiHitCounter < GetMoveStrikeCount(move))
        return TRUE;

    return FALSE;
}

enum BattlerId GetMushroomMadnessTarget(enum BattlerId battlerAtk, enum BattleSide defSide, enum Move move, const enum Ability *abilities, enum HoldEffect holdEffectAtk)
{
    if (!IsDoubleBattle() || GetBattlerSide(battlerAtk) == defSide)
        return MAX_BATTLERS_COUNT;
    if (abilities && abilities[defSide] != ABILITY_MUSHROOM_MADNESS && abilities[defSide | BIT_FLANK] != ABILITY_MUSHROOM_MADNESS)
        return MAX_BATTLERS_COUNT;

    enum Ability abilityAtk = abilities ? abilities[battlerAtk] : GetBattlerAbility(battlerAtk);
    enum BattleMoveEffects effect = GetMoveEffect(move);
    enum MoveTarget target = GetBattlerMoveTargetType(battlerAtk, move);
    if ((target != TARGET_SELECTED && target != TARGET_SMART && target != TARGET_RANDOM && target != TARGET_OPPONENT && effect != EFFECT_REFLECT_DAMAGE)
     || effect == EFFECT_SNIPE_SHOT || effect == EFFECT_SKY_DROP
     || (IsPursuitMove(battlerAtk, move, abilityAtk) && IsPursuitTargetSet())
     || abilityAtk == ABILITY_STALWART || abilityAtk == ABILITY_PROPELLER_TAIL
     || !IsAffectedByPowderMove(battlerAtk, abilityAtk, holdEffectAtk))
        return MAX_BATTLERS_COUNT;

    // A move-created center of attention takes precedence over the passive one.
    if (gSideTimers[defSide].followmeTimer && IsBattlerAlive(gSideTimers[defSide].followmeTarget)
     && (!gSideTimers[defSide].followmePowder || IsAffectedByPowderMove(battlerAtk, abilityAtk, holdEffectAtk)))
        return MAX_BATTLERS_COUNT;

    enum BattlerId redirector = MAX_BATTLERS_COUNT;
    u32 bestOrder = MAX_BATTLERS_COUNT;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battler) != defSide)
            continue;
        bool32 keepSmartTarget = IsSmartTargetMoveConsecutiveHit(battlerAtk, move) && gBattleStruct->moveTarget[battlerAtk] == battler;
        if ((!IsBattlerPresent(battler) && !keepSmartTarget)
         || gBattleMons[battler].volatiles.semiInvulnerable == STATE_SKY_DROP_ATTACKER
         || gBattleMons[battler].volatiles.semiInvulnerable == STATE_SKY_DROP_TARGET)
            continue;
        enum Ability ability = abilities ? abilities[battler] : GetBattlerAbility(battler);
        if (ability == ABILITY_MUSHROOM_MADNESS && (redirector == MAX_BATTLERS_COUNT || GetBattlerTurnOrderNum(battler) < bestOrder))
        {
            redirector = battler;
            bestOrder = GetBattlerTurnOrderNum(battler);
        }
    }
    return redirector;
}

static bool32 IsActiveFollowMeApplicable(enum BattlerId battlerAtk, enum BattleSide defSide, enum Move move)
{
    enum Ability ability = GetBattlerAbility(battlerAtk);
    enum BattleMoveEffects effect = GetMoveEffect(move);

    if (gSideTimers[defSide].followmeTimer == 0
        || (!IsBattlerAlive(gSideTimers[defSide].followmeTarget) && !IsSmartTargetMoveConsecutiveHit(battlerAtk, move))
        || effect == EFFECT_SNIPE_SHOT
        || effect == EFFECT_SKY_DROP
        || IsAbilityAndRecord(battlerAtk, ability, ABILITY_PROPELLER_TAIL)
        || IsAbilityAndRecord(battlerAtk, ability, ABILITY_STALWART))
        return FALSE;

    if (IsPursuitMove(battlerAtk, move, ability) && IsPursuitTargetSet())
        return FALSE;

    if (gSideTimers[defSide].followmePowder && !IsAffectedByPowderMove(battlerAtk, ability, GetBattlerHoldEffect(battlerAtk)))
        return FALSE;

    return TRUE;
}

enum BattlerId GetFollowMeTarget(enum BattlerId battlerAtk, enum BattleSide defSide, enum Move move)
{
    if (IsActiveFollowMeApplicable(battlerAtk, defSide, move))
        return gSideTimers[defSide].followmeTarget;
    return GetMushroomMadnessTarget(battlerAtk, defSide, move, NULL, GetBattlerHoldEffect(battlerAtk));
}

bool32 IsAffectedByFollowMe(enum BattlerId battlerAtk, enum BattleSide defSide, enum Move move)
{
    return GetFollowMeTarget(battlerAtk, defSide, move) < MAX_BATTLERS_COUNT;
}

static bool32 HandleMoveTargetRedirection(struct BattleCalcValues *cv, enum MoveTarget moveTarget)
{
    u32 redirectorOrderNum = MAX_BATTLERS_COUNT;
    enum BattleSide side = (GetBattlerSide(cv->battlerAtk) ^ BIT_SIDE);

    if (cv->moveEffect == EFFECT_REFLECT_DAMAGE)
    {
        enum DamageCategory reflectCategory = GetReflectDamageMoveDamageCategory(cv->battlerAtk, cv->move);

        if (reflectCategory == DAMAGE_CATEGORY_PHYSICAL)
            cv->battlerDef = gBattleStruct->moveTarget[cv->battlerAtk] = gProtectStructs[cv->battlerAtk].physicalBattlerId;
        else
            cv->battlerDef = gBattleStruct->moveTarget[cv->battlerAtk] = gProtectStructs[cv->battlerAtk].specialBattlerId;
    }

    enum BattlerId attentionTarget = GetFollowMeTarget(cv->battlerAtk, side, cv->move);
    if (attentionTarget < MAX_BATTLERS_COUNT
     && (moveTarget == TARGET_SELECTED || moveTarget == TARGET_SMART || moveTarget == TARGET_RANDOM || moveTarget == TARGET_OPPONENT || cv->moveEffect == EFFECT_REFLECT_DAMAGE)
     && !IsBattlerAlly(cv->battlerAtk, attentionTarget))
    {
        if (attentionTarget != cv->battlerDef && cv->abilities[attentionTarget] == ABILITY_MUSHROOM_MADNESS)
        {
            RecordAbilityBattle(attentionTarget, ABILITY_MUSHROOM_MADNESS);
            gSpecialStatuses[attentionTarget].abilityRedirected = TRUE;
        }
        gBattleStruct->moveTarget[cv->battlerAtk] = cv->battlerDef = attentionTarget;
        return TRUE;
    }

    enum Type moveType = GetBattleMoveType(cv->move);
    enum Ability ability = cv->abilities[cv->battlerDef];
    bool32 currTargetCantAbsorb = ((ability != ABILITY_LIGHTNING_ROD && moveType == TYPE_ELECTRIC
                                   && !IsHighVoltageElectricMove(cv->abilities[cv->battlerAtk], moveType))
                                || (!HasStormDrainAbility(ability) && moveType == TYPE_WATER));

    if (currTargetCantAbsorb
     && IsDoubleBattle()
     && !IsSpreadMove(moveTarget)
     && gSideTimers[side].followmeTimer == 0
     && moveTarget != TARGET_USER
     && moveTarget != TARGET_ALL_BATTLERS
     && moveTarget != TARGET_FIELD
     && cv->moveEffect != EFFECT_TEATIME
     && cv->moveEffect != EFFECT_SNIPE_SHOT
     && cv->moveEffect != EFFECT_PLEDGE)
    {
        // Find first battler that redirects the move (in turn order)
        enum Ability abilityAtk = cv->abilities[cv->battlerAtk];
        enum BattlerId battler;
        for (battler = 0; battler < gBattlersCount; battler++)
        {
            ability = cv->abilities[battler];
            if ((B_REDIRECT_ABILITY_ALLIES >= GEN_4 || !IsBattlerAlly(cv->battlerAtk, battler))
                && battler != cv->battlerAtk
                && battler != cv->battlerDef
                && ((ability == ABILITY_LIGHTNING_ROD && moveType == TYPE_ELECTRIC)
                 || (HasStormDrainAbility(ability) && moveType == TYPE_WATER))
                && GetBattlerTurnOrderNum(battler) < redirectorOrderNum
                && !IsAbilityAndRecord(cv->battlerAtk, abilityAtk, ABILITY_PROPELLER_TAIL)
                && !IsAbilityAndRecord(cv->battlerAtk, abilityAtk, ABILITY_STALWART))
            {
                redirectorOrderNum = GetBattlerTurnOrderNum(battler);
            }
        }
        if (redirectorOrderNum != MAX_BATTLERS_COUNT)
        {
            cv->battlerDef = gBattlerByTurnOrder[redirectorOrderNum];
            RecordAbilityBattle(cv->battlerDef, cv->abilities[cv->battlerDef]);
            gSpecialStatuses[cv->battlerDef].abilityRedirected = TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

static bool32 WasOriginalTargetAlly(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum MoveTarget moveTarget)
{
    if (!gProtectStructs[GetPartnerBattler(battlerAtk)].usedAllySwitch)
        return FALSE;

    if ((moveTarget == TARGET_ALLY || moveTarget == TARGET_USER_OR_ALLY) && battlerAtk == battlerDef)
        return TRUE;

    return FALSE;
}

static enum CancelerResult CancelerSetTargets(struct BattleCalcValues *cv)
{
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);
    bool32 isDoubleBattle = IsDoubleBattle();

    if (!HandleMoveTargetRedirection(cv, moveTarget))
    {
        if (isDoubleBattle && moveTarget == TARGET_RANDOM
         && !(cv->abilities[cv->battlerAtk] == ABILITY_BALANCE_MASTER && gSpecialStatuses[cv->battlerAtk].postActionAbilityMove))
        {
            cv->battlerDef = SetRandomTarget(cv->battlerAtk);
            if (!IsBattlerAlive(cv->battlerAtk) && !IsBattlerAlly(cv->battlerAtk, cv->battlerDef))
            {
                cv->battlerDef = GetPartnerBattler(cv->battlerDef);
            }
        }
        else if (moveTarget == TARGET_SELECTED && cv->battlerDef == cv->battlerAtk)
        {
            cv->battlerDef = SetRandomTarget(cv->battlerAtk);
        }
        else if (IsDoubleBattle() && moveTarget == TARGET_ALLY && !IsBattlerAlly(cv->battlerDef, cv->battlerAtk))
        {
            cv->battlerDef = GetPartnerBattler(cv->battlerAtk);
        }
        else if (isDoubleBattle && moveTarget == TARGET_FOES_AND_ALLY)
        {
            for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
            {
                if (battlerDef == cv->battlerAtk)
                    continue;

                if (IsBattlerAlive(battlerDef))
                {
                    cv->battlerDef = battlerDef;
                    break;
                }
            }
        }
        else if (moveTarget == TARGET_USER || moveTarget == TARGET_USER_AND_ALLY)
        {
            cv->battlerDef = cv->battlerAtk;
        }
        else if (isDoubleBattle && moveTarget == TARGET_USER_OR_ALLY && !IsBattlerAlive(cv->battlerDef))
        {
            cv->battlerDef = cv->battlerAtk;
        }
        else if (isDoubleBattle
              && moveTarget != TARGET_OPPONENTS_FIELD
              && !IsBattlerAlive(cv->battlerDef)
              && !IsBattlerAlly(cv->battlerAtk, cv->battlerDef))
        {
            cv->battlerDef = GetPartnerBattler(cv->battlerDef);
        }
    }

    gBattlerTarget = cv->battlerDef; // SetTargetMoveFailureFlags relies on gBattlerTarget
    SetTargetMoveFailureFlags(cv->battlerAtk, moveTarget);

    // An echo only affects targets that the first application reached.
    if (gSpecialStatuses[cv->battlerAtk].reverberationsEcho)
    {
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            if (!(gSpecialStatuses[cv->battlerAtk].multiHitTargets & (1u << battler)))
                AddBattlerMoveResultFlag(battler, MOVE_RESULT_NOT_TARGETED);
        }
    }

    gBattleStruct->eventState.atkCancelerBattler = 0;

    if (IsBattlerAlly(cv->battlerAtk, cv->battlerDef) && !IsBattlerAlive(cv->battlerDef))
    {
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        return CANCELER_RESULT_FAILURE;
    }
    else if (WasOriginalTargetAlly(cv->battlerAtk, cv->battlerDef, moveTarget))
    {
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        return CANCELER_RESULT_FAILURE;
    }

    SetDynamicMoveCategory(cv->battlerAtk, cv->battlerDef, cv->move);
    if (IsSpikyGripPreventingMove(cv->battlerAtk, cv->move, TRUE))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattleStruct->submoveAnnouncement = SUBMOVE_NO_EFFECT;
        gBattlescriptCurrInstr = BattleScript_MoveUsedIsSpikyGripPrevented;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerPPDeduction(struct BattleCalcValues *cv)
{
    if (gSpecialStatuses[cv->battlerAtk].reverberationsEcho
     || gBattleMons[cv->battlerAtk].volatiles.multipleTurns
     || IsBattlerUsingAbilityMove(cv->battlerAtk)
     || gBattleStruct->bouncedMoveIsUsed
     || gBattleStruct->snatchedMoveIsUsed
     || gBattleMons[cv->battlerAtk].volatiles.bideTurns
     || cv->move == MOVE_STRUGGLE)
        return CANCELER_RESULT_SUCCESS;

    s32 ppToDeduct = 1;
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);
    enum MoveSlot movePosition = gCurrMovePos;

    if (gBattleStruct->submoveAnnouncement == SUBMOVE_SUCCESS)
        movePosition = gChosenMovePos;

    if (IsSpreadMove(moveTarget)
     || moveTarget == TARGET_ALL_BATTLERS
     || moveTarget == TARGET_FIELD
     || MoveForcesPressure(cv->move))
    {
        for (u32 i = 0; i < gBattlersCount; i++)
        {
            if (!IsBattlerAlly(i, cv->battlerAtk))
                ppToDeduct += (GetBattlerAbility(i) == ABILITY_PRESSURE);
        }
    }
    else if (moveTarget != TARGET_OPPONENTS_FIELD)
    {
        if (cv->battlerAtk != cv->battlerDef && GetBattlerAbility(cv->battlerDef) == ABILITY_PRESSURE)
             ppToDeduct++;
    }

    // For item Metronome, echoed voice
    if (cv->move != gLastResultingMoves[cv->battlerAtk] || gBattleStruct->unableToUseMove)
        gBattleMons[cv->battlerAtk].volatiles.metronomeItemCounter = 0;

    if (gBattleMons[cv->battlerAtk].pp[movePosition] > ppToDeduct)
        gBattleMons[cv->battlerAtk].pp[movePosition] -= ppToDeduct;
    else
        gBattleMons[cv->battlerAtk].pp[movePosition] = 0;

    gLastMoves[cv->battlerAtk] = gChosenMove;


    if (gBattleStruct->submoveAnnouncement != SUBMOVE_NO_EFFECT)
    {
        if (gBattleStruct->submoveAnnouncement == SUBMOVE_FAILURE)
        {
            gBattleStruct->submoveAnnouncement = SUBMOVE_NO_EFFECT;
            if (cv->move == MOVE_DARK_IMITATION && IsBattlerMoveResultSet(cv->battlerDef, MOVE_RESULT_MISSED))
            {
                gBattleScripting.battler = cv->battlerDef;
                gLastLandedMoves[cv->battlerDef] = MOVE_NONE;
                gLastHitByType[cv->battlerDef] = TYPE_NONE;
                gBattleStruct->blunderPolicy = cv->holdEffects[cv->battlerAtk] == HOLD_EFFECT_BLUNDER_POLICY;
                gBattlescriptCurrInstr = BattleScript_MoveMissed;
            }
            else
                gBattlescriptCurrInstr = BattleScript_ButItFailed;
            return CANCELER_RESULT_FAILURE;
        }
        else if (CancelerVolatileBlocked(cv) == CANCELER_RESULT_FAILURE) // Check volatile restrictions for the called move.
        {
            gBattleStruct->submoveAnnouncement = SUBMOVE_NO_EFFECT;
            return CANCELER_RESULT_FAILURE;
        }
        else
        {
            gBattleStruct->submoveAnnouncement = SUBMOVE_NO_EFFECT;
            gBattleScripting.animTurn = 0;
            gBattleScripting.animTargetsHit = 0;

            // Possibly better to just move type setting and redirection to attackcanceler as a new case at this point
            SetTypeBeforeUsingMove(cv->move, cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk]);
            gBattlescriptCurrInstr = BattleScript_MoveResolution;
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
    }

    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT; // Apply power point change
}

// We don't have clear data on where this belongs to but I assume it should at least be checked before Protean
static enum CancelerResult CancelerSkyBattle(struct BattleCalcValues *cv)
{
    if (gBattleStruct->isSkyBattle && IsMoveSkyBattleBanned(gCurrentMove))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerWeatherPrimal(struct BattleCalcValues *cv)
{
    enum CancelerResult result = CANCELER_RESULT_SUCCESS;

    if (GetMovePower(cv->move) > 0 && HasWeatherEffect())
    {
        enum Type moveType = GetBattleMoveType(cv->move);
        if (moveType == TYPE_FIRE && gBattleWeather & B_WEATHER_RAIN_PRIMAL && (GetConfig(B_POWDER_STATUS_HEAVY_RAIN) >= GEN_7 || !TryActivatePowderStatus(cv->move)))
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PRIMAL_WEATHER_FIZZLED_BY_RAIN;
            result = CANCELER_RESULT_FAILURE;
        }
        else if (moveType == TYPE_WATER && gBattleWeather & B_WEATHER_SUN_PRIMAL)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_PRIMAL_WEATHER_EVAPORATED_IN_SUN;
            result = CANCELER_RESULT_FAILURE;
        }
        if (result == CANCELER_RESULT_FAILURE)
        {
            gBattleMons[cv->battlerAtk].volatiles.chargeTurn = FALSE;
            CancelMultiTurnMoves(cv->battlerAtk);
            gBattlescriptCurrInstr = BattleScript_PrimalWeatherBlocksMove;
        }
    }

    return result;
}

static enum CancelerResult CancelerFocusPreGen5(struct BattleCalcValues *cv)
{
    if (GetConfig(B_FOCUS_PUNCH_FAILURE) < GEN_5)
        return CancelerFocus(cv);
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerBide(struct BattleCalcValues *cv)
{
    if (cv->moveEffect != EFFECT_BIDE)
        return CANCELER_RESULT_SUCCESS;

    if (gBattleMons[cv->battlerAtk].volatiles.bideTurns)
    {
        if (--gBattleMons[cv->battlerAtk].volatiles.bideTurns)
        {
            gBattlescriptCurrInstr = BattleScript_BideStoringEnergy;
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT; // Jump to moveend
        }
        else if (gBideDmg[cv->battlerAtk])
        {
            gBattlerTarget = gBideTarget[cv->battlerAtk];
            gBattleMons[cv->battlerAtk].volatiles.multipleTurns = FALSE;

            if (!IsBattlerAlive(gBattlerTarget))
                gBattlerTarget = GetBattleMoveTarget(gCurrentMove, TARGET_SELECTED);

            cv->battlerDef = gBattlerTarget;
            gBattleStruct->moveResultFlags[gBattlerTarget] = 0;
            SetTargetMoveFailureFlags(cv->battlerAtk, TARGET_SELECTED);

            BattleScriptCall(BattleScript_BideAttack);
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
        else
        {
            gBattleMons[cv->battlerAtk].volatiles.multipleTurns = FALSE;
            gBattlescriptCurrInstr = BattleScript_BideNoEnergyToAttack;
            return CANCELER_RESULT_FAILURE;
        }
    }
    else
    {
        gBattleMons[gBattlerAttacker].volatiles.multipleTurns = TRUE;
        gLockedMoves[gBattlerAttacker] = gCurrentMove;
        gBideDmg[gBattlerAttacker] = 0;
        gBattleMons[gBattlerAttacker].volatiles.bideTurns = 2;
        gBattlescriptCurrInstr = BattleScript_SetUpBide;
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT; // Jump to moveend
    }

    return CANCELER_RESULT_SUCCESS;
}

static bool32 ShouldSkipFailureCheckOnBattler(enum BattlerId battlerDef)
{
    return IsBattlerMoveResultSet(battlerDef, (MOVE_RESULT_INVALID_TARGET | MOVE_RESULT_IGNORE_SELF));
}

// Insert failure here when it doesn't depend on the target
static enum CancelerResult CancelerMoveFailure(struct BattleCalcValues *cv)
{
    const u8 *battleScript = NULL;

    switch (cv->moveEffect)
    {
    case EFFECT_PLACEHOLDER:
        battleScript = BattleScript_EffectPlaceholder;
        break;
    case EFFECT_FLING:
        if (!CanFling(cv->battlerAtk, cv->abilities[cv->battlerAtk]))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_FAIL_IF_NOT_ARG_TYPE:
        if (!IS_BATTLER_OF_TYPE(cv->battlerAtk, GetMoveArgType(cv->move)))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_DARK_VOID:
        if (gBattleStruct->bouncedMoveIsUsed)
            break;
        if (GetConfig(B_DARK_VOID_FAIL) >= GEN_7 && gBattleMons[cv->battlerAtk].species != SPECIES_DARKRAI)
            battleScript = BattleScript_PokemonCantUseTheMove;
        break;
    case EFFECT_AURA_WHEEL:
        if (GetBaseSpecies(gBattleMons[cv->battlerAtk].species) != SPECIES_MORPEKO)
            battleScript = BattleScript_PokemonCantUseTheMove;
        break;
    case EFFECT_HYPERSPACE_FURY:
        if (gBattleMons[cv->battlerAtk].species == SPECIES_HOOPA_CONFINED)
            battleScript = BattleScript_ButHoopaCantUseIt;
        else if (gBattleMons[cv->battlerAtk].species != SPECIES_HOOPA_UNBOUND)
            battleScript = BattleScript_PokemonCantUseTheMove;
        break;
    case EFFECT_AURORA_VEIL:
        if (!(GetAttackerWeather(GetBattlerHoldEffect(cv->battlerAtk), GetBattlerAbility(cv->battlerAtk), GetWeather()) & B_WEATHER_ICY_ANY))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_CLANGOROUS_SOUL:
        if (gBattleMons[cv->battlerAtk].hp <= max(1, GetBattlerMaxHP(cv->battlerAtk) / 3))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_REFLECT_DAMAGE:
    {
        enum DamageCategory reflectCategory = GetReflectDamageMoveDamageCategory(cv->battlerAtk, cv->move);
        if (IsBattlerAlly(cv->battlerAtk, cv->battlerDef))
            battleScript = BattleScript_ButItFailed;
        // Counter / Metal Burst and took physical damage
        else if (reflectCategory == DAMAGE_CATEGORY_PHYSICAL
              && gProtectStructs[cv->battlerAtk].physicalDmg > 0
              && (GetConfig(B_COUNTER_TRY_HIT_PARTNER) >= GEN_5 || gBattleMons[gProtectStructs[cv->battlerAtk].physicalBattlerId].hp))
            break;
        // Mirror Coat / Metal Burst and took special damage
        else if (reflectCategory == DAMAGE_CATEGORY_SPECIAL
              && gProtectStructs[cv->battlerAtk].specialDmg > 0
              && (GetConfig(B_COUNTER_TRY_HIT_PARTNER) >= GEN_5 || gBattleMons[gProtectStructs[cv->battlerAtk].specialBattlerId].hp))
            break;
        else
            battleScript = BattleScript_ButItFailed;
        break;
    }
    case EFFECT_DESTINY_BOND:
        if (DoesDestinyBondFail(cv->battlerAtk))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_FIRST_TURN_ONLY:
        if (!IsBattlersFirstTurn(cv->battlerAtk) || gSpecialStatuses[cv->battlerAtk].backUpTarget)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_MAT_BLOCK:
        if (!IsBattlersFirstTurn(cv->battlerAtk) || gSpecialStatuses[cv->battlerAtk].backUpTarget)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_FOLLOW_ME:
        if (B_UPDATED_MOVE_DATA >= GEN_8 && !HasAnyBattleFlags(BATTLE_TYPE_DOUBLE))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_LAST_RESORT:
        if (!CanUseLastResort(cv->battlerAtk))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_NO_RETREAT:
        if (gBattleMons[cv->battlerAtk].volatiles.noRetreat)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_PROTECT:
    case EFFECT_ENDURE:
        TryResetConsecutiveUseCounter(cv->battlerAtk);
        if (IsLastMonToMove(cv->battlerAtk))
        {
            battleScript = BattleScript_ButItFailed;
        }
        else
        {
            enum ProtectMethod protectMethod = GetMoveProtectMethod(cv->move);
            bool32 canUseProtectSecondTime = CanUseMoveConsecutively(cv->battlerAtk);
            bool32 canUseWideGuard = (GetConfig(B_WIDE_GUARD) >= GEN_6 && protectMethod == PROTECT_WIDE_GUARD);
            bool32 canUseQuickGuard = (GetConfig(B_QUICK_GUARD) >= GEN_6 && protectMethod == PROTECT_QUICK_GUARD);

            if (!canUseProtectSecondTime
             && !canUseWideGuard
             && !canUseQuickGuard
             && protectMethod != PROTECT_CRAFTY_SHIELD)
                battleScript = BattleScript_ButItFailed;
        }
        if (battleScript != NULL)
        {
            gBattleMons[cv->battlerAtk].volatiles.consecutiveMoveUses = 0;
            gBattleStruct->battlerState[cv->battlerAtk].stompingTantrumTimer = 2;
        }
        break;
    case EFFECT_REST:
        battleScript = GetRestFailureScript(cv->battlerAtk, cv->abilities[cv->battlerAtk]);
        break;
    case EFFECT_SNORE:
        if (!IsAsleepOrComatose(cv->battlerAtk, cv->abilities[cv->battlerAtk]))
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_STEEL_ROLLER:
        if (gFieldTimers.terrain == B_TERRAIN_NONE)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_STOCKPILE:
        if (gBattleMons[cv->battlerAtk].volatiles.stockpileCounter >= 3)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_STUFF_CHEEKS:
        if (GetItemPocket(gBattleMons[cv->battlerAtk].item) != POCKET_BERRIES)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_SWALLOW:
    case EFFECT_SPIT_UP:
        if (gBattleMons[cv->battlerAtk].volatiles.stockpileCounter == 0 && !gBattleStruct->snatchedMoveIsUsed)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_TELEPORT:
        if ((!IsTrainerBattle() && !IsOnPlayerSide(cv->battlerAtk)) || GetConfig(B_TELEPORT_BEHAVIOR) <= GEN_7)
        {
            bool32 canRun = IsRunningFromBattleImpossible(cv->battlerAtk);

            if (IsTrainerBattle()
             || canRun == BATTLE_RUN_FORBIDDEN
             || IsCommanderActive(cv->battlerAtk)
             || IsOnPlayerSide(cv->battlerAtk))
            {
                battleScript = BattleScript_ButItFailed;

            }
            else if (canRun == BATTLE_RUN_FAILURE)
            {
                battleScript = BattleScript_PrintAbilityMadeIneffective;
            }
        }
        else if (!CanBattlerSwitch(cv->battlerAtk))
        {
            battleScript = BattleScript_ButItFailed;
        }
        break;
    case EFFECT_BATON_PASS:
        if (!CanBattlerSwitch(cv->battlerAtk))
        {
            battleScript = BattleScript_ButItFailed;
        }
        break;
    case EFFECT_HEALING_WISH:
    case EFFECT_LUNAR_DANCE:
        if (!CanBattlerSwitch(cv->battlerAtk))
        {
            battleScript = BattleScript_ButItFailed;
        }
        else
        {
            if (cv->moveEffect == EFFECT_LUNAR_DANCE)
                gBattleStruct->battlerState[cv->battlerAtk].storedLunarDance = TRUE;
            else
                gBattleStruct->battlerState[cv->battlerAtk].storedHealingWish = TRUE;
        }
        break;
    case EFFECT_NATURAL_GIFT:
        if (GetItemPocket(gBattleMons[cv->battlerAtk].item) != POCKET_BERRIES
         || gFieldStatuses & STATUS_FIELD_MAGIC_ROOM
         || cv->abilities[cv->battlerAtk] == ABILITY_KLUTZ
         || gBattleMons[cv->battlerAtk].volatiles.embargoTimer)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_BELCH:
        if (!GetBattlerPartyState(cv->battlerAtk)->ateBerry)
            battleScript = BattleScript_BelchFails;
        break;
    case EFFECT_BEAK_BLAST:
        if (!gBattleStruct->battlerState[cv->battlerAtk].focusPunchBattlers && GetConfig(B_MOVE_EFFECTS_BEFORE_MOVES) <= GEN_9)
            battleScript = BattleScript_ButItFailed;
        break;
    case EFFECT_ALLY_SWITCH:
    {
        enum BattlerId partner = GetPartnerBattler(cv->battlerAtk);
        if (!IsBattlerAlive(partner)
         || HasPartnerTrainer(cv->battlerAtk)
         || IsCommanderActive(cv->battlerAtk)
         || IsCommanderActive(partner))
        {
            battleScript = BattleScript_ButItFailed;
        }
        else if (GetConfig(B_ALLY_SWITCH_FAIL_CHANCE) >= GEN_9)
        {
            TryResetConsecutiveUseCounter(cv->battlerAtk);

            if (CanUseMoveConsecutively(cv->battlerAtk))
            {
                gBattleMons[cv->battlerAtk].volatiles.consecutiveMoveUses++;
            }
            else
            {
                gBattleMons[cv->battlerAtk].volatiles.consecutiveMoveUses = 0;
                battleScript = BattleScript_ButItFailed;
            }
        }
        break;
    }
    default:
        break;
    }

    if (battleScript != NULL)
    {
        gBattlescriptCurrInstr = battleScript;
        return CANCELER_RESULT_FAILURE;
    }

    return CANCELER_RESULT_SUCCESS;
}

// Insert failure here if it depends on target and follow the existing structure
static enum CancelerResult CancelerMoveEffectFailureTarget(struct BattleCalcValues *cv)
{
    const u8 *battleScript = NULL;
    u32 numAffectedTargets = 0;
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);

    while (gBattleStruct->eventState.atkCancelerBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = (enum BattlerId)gBattleStruct->eventState.atkCancelerBattler++;

        bool32 checkUserFailure = (battlerDef == cv->battlerAtk && moveTarget == TARGET_USER_AND_ALLY);
        if (!checkUserFailure && ShouldSkipFailureCheckOnBattler(battlerDef))
            continue;

        switch (cv->moveEffect)
        {
        case EFFECT_FUTURE_SIGHT:
            if (gBattleStruct->futureSight[battlerDef].counter > 0)
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        case EFFECT_POLTERGEIST:
            if (gBattleMons[battlerDef].item == ITEM_NONE)
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        case EFFECT_SUCKER_PUNCH:
        {
            u32 defMove = GetBattlerChosenMove(battlerDef);
            if (HasBattlerActedThisTurn(battlerDef)
             || (IsBattleMoveStatus(defMove) && !gProtectStructs[battlerDef].noValidMoves && GetMoveEffect(defMove) != EFFECT_ME_FIRST))
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }

        }
            break;
        case EFFECT_UPPER_HAND:
        {
            s32 prio = GetChosenMovePriority(battlerDef, GetBattlerAbility(battlerDef));
            if (prio < 1 || prio > 3 // Fails if priority is less than 1 or greater than 3, if target already moved, or if using a status
             || HasBattlerActedThisTurn(battlerDef)
             || gChosenMoveByBattler[battlerDef] == MOVE_NONE
             || IsBattleMoveStatus(gChosenMoveByBattler[battlerDef]))
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        }
        case EFFECT_LOW_KICK:
        case EFFECT_HEAT_CRASH:
            numAffectedTargets++;
            continue;
        case EFFECT_CAPTIVATE:
            if (!AreBattlersOfOppositeGender(cv->battlerAtk, battlerDef))
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        case EFFECT_STAT_CHANGE_ON_STATUS:
            if (!(gBattleMons[battlerDef].status1 & GetMoveStatusOnStatChange(cv->move)))
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        case EFFECT_STAT_CHANGE_MAGNETIC:
            if (cv->abilities[battlerDef] != ABILITY_PLUS && cv->abilities[battlerDef] != ABILITY_MINUS)
            {
                battleScript = BattleScript_ButItFailed;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        case EFFECT_PRESENT:
            if (gBattleStruct->presentBasePower > 0)
            {
                numAffectedTargets++;
                continue;
            }
            else if (gBattleMons[cv->battlerDef].maxHP == gBattleMons[cv->battlerDef].hp)
            {
                battleScript = BattleScript_AlreadyAtFullHp;
            }
            else
            {
                numAffectedTargets++;
                continue;
            }
            break;
        default:
            continue;
        }

        gBattleStruct->moveResultFlags[battlerDef] = MOVE_RESULT_DOESNT_AFFECT_FOE;
    }

    gBattleStruct->eventState.atkCancelerBattler = 0;

    if (numAffectedTargets == 0 && battleScript != NULL)
    {
        gBattlescriptCurrInstr = battleScript;
        return CANCELER_RESULT_FAILURE;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerPowderStatus(struct BattleCalcValues *cv)
{
    if (TryActivatePowderStatus(cv->move))
    {
        if (!IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD))
            SetPassiveDamageAmount(cv->battlerAtk, GetBattlerMaxHP(cv->battlerAtk) / 4);

        // This might be incorrect
        gBattlescriptCurrInstr = BattleScript_MoveUsedPowder;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

bool32 IsDazzlingAbility(enum BattlerId battler, enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_GLORIOUS_MANE:  return IsBattlerAtMaxHp(battler);
    case ABILITY_DAZZLING:        return TRUE;
    case ABILITY_QUEENLY_MAJESTY: return TRUE;
    case ABILITY_ARMOR_TAIL:      return TRUE;
    default: break;
    }
    return FALSE;
}

static enum CancelerResult CancelerPriorityBlock(struct BattleCalcValues *cv)
{
    bool32 effect = FALSE;
    s32 priority = GetChosenMovePriority(cv->battlerAtk, cv->abilities[cv->battlerAtk]);
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);

    if (priority <= 0 || moveTarget == TARGET_FIELD || moveTarget == TARGET_OPPONENTS_FIELD)
        return CANCELER_RESULT_SUCCESS;

    enum BattlerId battler;
    enum Ability ability = ABILITY_NONE; // ability of battler who is blocking
    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler) || IsBattlerAlly(cv->battlerAtk, battler))
            continue;
        if (ShouldSkipFailureCheckOnBattler(battler)
         && (!IsDoubleBattle() || ShouldSkipFailureCheckOnBattler(GetPartnerBattler(battler)))) // either battler or partner is affected
            continue;

        ability = cv->abilities[battler];
        if (IsDazzlingAbility(battler, ability))
        {
            effect = TRUE;
            break;
        }
    }

    if (effect)
    {
        gLastUsedAbility = ability;
        RecordAbilityBattle(battler, ability);
        gBattlerAbility = battler;
        gBattlescriptCurrInstr = BattleScript_PokemonCannotUseMove;
        return CANCELER_RESULT_FAILURE;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerExplodingDamp(struct BattleCalcValues *cv)
{
    if (!IsMoveDampBanned(cv->move))
        return CANCELER_RESULT_SUCCESS;

    u32 dampBattler = IsAbilityOnField(ABILITY_DAMP);
    if (dampBattler)
    {
        gBattlerAbility = dampBattler - 1;
        gLastUsedAbility = ABILITY_DAMP;
        RecordAbilityBattle(gBattlerAbility, ABILITY_DAMP);
        gBattlescriptCurrInstr = BattleScript_PokemonCannotUseMove;
        return CANCELER_RESULT_FAILURE;
    }
    return CANCELER_RESULT_SUCCESS;
}

static bool32 CanPledgeComboAttackHappen(struct BattleCalcValues *cv)
{
    enum BattlerId partner = GetPartnerBattler(cv->battlerAtk);
    enum Move partnerMove = GetBattlerChosenMove(partner);

    if (gBattleStruct->pledgeState != PLEDGE_COMBO_NONE)
        return FALSE; // already in progress

    if (!IsDoubleBattle()
     || GetMoveEffect(partnerMove) != EFFECT_PLEDGE
     || !IsBattlerAlive(partner)
     || cv->move == partnerMove
     || gChosenActionByBattler[partner] != B_ACTION_USE_MOVE
     || HasBattlerActedThisTurn(partner))
        return FALSE;

    if (GetPledgeComboMove(cv->move) == partnerMove || GetPledgeComboMove(partnerMove) == cv->move)
    {
        u32 k = 0;
        u32 currPledgeUser = 0;
        u32 newTurnOrder[] = {0xFF, 0xFF};

        for (u32 i = 0; i < gBattlersCount; i++)
        {
            if (gBattlerByTurnOrder[i] == gBattlerAttacker)
            {
                currPledgeUser = i + 1; // Current battler going after attacker
                break;
            }
        }
        for (u32 i = currPledgeUser; i < gBattlersCount; i++)
        {
            if (gBattlerByTurnOrder[i] != partner)
                newTurnOrder[k++] = gBattlerByTurnOrder[i];
        }

        gBattlerByTurnOrder[currPledgeUser++] = partner;

        for (u32 i = 0; i < ARRAY_COUNT(newTurnOrder); i++)
        {
            if (newTurnOrder[i] == 0xFF)
                continue;
            gBattlerByTurnOrder[currPledgeUser++] = newTurnOrder[i];
        }

        gBattleStruct->pledgeState = PLEDGE_COMBO_WAITING;
        return TRUE;
    }

    return FALSE;
}

static enum CancelerResult CancelerInterruptibleMoves(struct BattleCalcValues *cv)
{
    switch (cv->moveEffect)
    {
    case EFFECT_FUTURE_SIGHT:
        gBattleStruct->futureSight[cv->battlerDef].move = cv->move;
        gBattleStruct->futureSight[cv->battlerDef].battlerIndex = cv->battlerAtk;
        gBattleStruct->futureSight[cv->battlerDef].partyIndex = gBattlerPartyIndexes[cv->battlerAtk];
        gBattleStruct->futureSight[cv->battlerDef].counter = 3;
        gBattleStruct->futureSight[cv->battlerDef].greatMinds = cv->abilities[cv->battlerAtk] == ABILITY_GREAT_MINDS;

        if (gCurrentMove == MOVE_DOOM_DESIRE)
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DOOM_DESIRE;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_FUTURE_SIGHT;

        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        gBattlescriptCurrInstr = BattleScript_ForseeFutureSight;
        return CANCELER_RESULT_RUN_SCRIPT;
        break;
    case EFFECT_PLEDGE:
        if (CanPledgeComboAttackHappen(cv))
        {
            gBattleStruct->eventState.atkCanceler = CANCELER_END;
            gBattlescriptCurrInstr = BattleScript_PledgeWaitingForPartner;
            return CANCELER_RESULT_RUN_SCRIPT;
        }
        break;
    case EFFECT_SHELL_TRAP:
        if (!gProtectStructs[cv->battlerAtk].shellTrap)
        {
            gBattlescriptCurrInstr = BattleScript_ShellTrapFailed;
            return CANCELER_RESULT_FAILURE;
        }
        break;
    default:
        break;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerRiverEsper(struct BattleCalcValues *cv)
{
    if (cv->abilities[cv->battlerAtk] != ABILITY_RIVER_ESPER)
        return CANCELER_RESULT_SUCCESS;

    u32 weather = GetAttackerWeather(cv->holdEffects[cv->battlerAtk], ABILITY_RIVER_ESPER, GetWeather());
    u32 mask = GetRiverEsperStatMask(GetBattleMoveType(cv->move), weather);
    bool32 changed = FALSE;
    for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
    {
        if ((mask & (1u << stat)) && CompareStat(cv->battlerAtk, stat, MAX_STAT_STAGE, CMP_LESS_THAN, ABILITY_RIVER_ESPER))
        {
            SetStatChange(cv->battlerAtk, stat, 2);
            changed = TRUE;
        }
    }
    if (changed)
    {
        gLastUsedAbility = ABILITY_RIVER_ESPER;
        gEffectBattler = gBattlerAbility = cv->battlerAtk;
        BattleScriptCall(BattleScript_AbilityStatChange);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerProtean(struct BattleCalcValues *cv)
{
    enum Type moveType = GetBattleMoveType(cv->move);
    if (ProteanTryChangeType(cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->move, moveType))
    {
        if (GetConfig(B_PROTEAN_LIBERO) >= GEN_9)
            gBattleMons[cv->battlerAtk].volatiles.usedProteanLibero = TRUE;
        PREPARE_TYPE_BUFFER(gBattleTextBuff1, moveType);
        gBattlerAbility = cv->battlerAtk;
        PrepareStringBattleWithWait(STRINGID_EMPTYSTRING3, cv->battlerAtk);
        BattleScriptCall(BattleScript_ProteanActivates);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerUnbrokenStone(struct BattleCalcValues *cv)
{
    enum BattleSide side = GetBattlerSide(cv->battlerAtk);
    if (cv->abilities[cv->battlerAtk] == ABILITY_UNBROKEN_STONE
     && !IsBattleMoveStatus(cv->move) && GetBattleMoveType(cv->move) == TYPE_ROCK
     && IsHazardOnSide(side, HAZARDS_STEALTH_ROCK))
    {
        RemoveHazardFromField(side, HAZARDS_STEALTH_ROCK);
        gProtectStructs[cv->battlerAtk].unbrokenStoneBoosted = TRUE;
        gEffectBattler = gBattlerAbility = cv->battlerAtk;
        BattleScriptCall(BattleScript_UnbrokenStoneConsumesRocks);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerMoveSummonedField(struct BattleCalcValues *cv)
{
    enum Ability ability = cv->abilities[cv->battlerAtk];
    if (ability == ABILITY_PEAK_VOLTAGE
     && GetBattleMoveType(cv->move) == TYPE_ELECTRIC
     && TryChangeBattleTerrain(cv->battlerAtk, B_TERRAIN_ELECTRIC))
    {
        gBattlerAbility = cv->battlerAtk;
        PrepareStringBattleWithWait(STRINGID_EMPTYSTRING3, cv->battlerAtk);
        BattleScriptCall(BattleScript_ElectricSurgeActivates);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    if (ability == ABILITY_PHOTOSYNTHETIC || ability == ABILITY_FROSTED_FUR)
    {
        enum BattleWeather weather = GetMoveSummonedWeather(ability, GetBattleMoveType(cv->move));
        if (weather != BATTLE_WEATHER_NONE && !(gBattleWeather & B_WEATHER_PRIMAL_ANY) && !gBattleStruct->overworldWeatherPresent)
        {
            gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
            gLastUsedAbility = ability;
            if (TryChangeWeatherWithAbility(cv->battlerAtk, weather, ability))
                return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
    }
    return CANCELER_RESULT_SUCCESS;
}

static void SetValuesForTwoTurnMoveFire(struct BattleCalcValues *cv)
{
    gBattleScripting.animTurn = 1;
    gBattleScripting.animTargetsHit = 0;
    gBattleScripting.battler = cv->battlerAtk;
    gBattleMons[cv->battlerAtk].volatiles.multipleTurns = FALSE;
    gBattleMons[cv->battlerAtk].volatiles.chargeTurn = FALSE;
    if (gBattleMoveEffects[cv->moveEffect].semiInvulnerableEffect)
        gBattleMons[cv->battlerAtk].volatiles.semiInvulnerable = STATE_NONE;
}

static enum CancelerResult CancelerCharging(struct BattleCalcValues *cv)
{
    bool32 isSkyDrop = cv->moveEffect == EFFECT_SKY_DROP;

    if (gBattleStruct->setChargeTurnAdditionalEffects)
    {
        u32 numAdditionalEffects = GetMoveAdditionalEffectCount(cv->move);

        while (gBattleStruct->eventState.atkCancelerBattler < gBattlersCount)
        {
            enum BattlerId effectBattler = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.atkCancelerBattler);

            while (gBattleStruct->additionalEffectsCounter < numAdditionalEffects)
            {
                const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(cv->move, gBattleStruct->additionalEffectsCounter);

                bool32 isSelf = effectBattler == cv->battlerAtk;

                if (!additionalEffect->onChargeTurnOnly
                 || isSelf != additionalEffect->self
                 || (!isSelf && ShouldSkipFailureCheckOnBattler(effectBattler)))
                {
                    gBattleStruct->additionalEffectsCounter++;
                    continue;
                }

                TryTriggerAdditionalEffect(cv, additionalEffect, effectBattler);

                return CANCELER_RESULT_RUN_SCRIPT; // We don't know if a script should be run or not so try
            }

            gBattleStruct->eventState.atkCancelerBattler++;
            gBattleStruct->additionalEffectsCounter = 0;
        }

        cv->battlerDef = gBattlerTarget;
        gBattleStruct->additionalEffectsCounter = 0;
        gBattleStruct->eventState.atkCancelerBattler = 0;
        gBattleStruct->setChargeTurnAdditionalEffects = FALSE;
    }

    if (!gBattleMoveEffects[cv->moveEffect].twoTurnEffect)
        return CANCELER_RESULT_SUCCESS;

    if (gBattleMons[cv->battlerAtk].volatiles.multipleTurns) // Second turn
    {
        SetValuesForTwoTurnMoveFire(cv);

        if (isSkyDrop)
        {
            // Sky Drop fails if target already left the field
            if (gBattleMons[cv->battlerDef].volatiles.semiInvulnerable == STATE_NONE || gBattleMons[cv->battlerAtk].volatiles.skyDropTarget == 0)
            {
                gBattleMons[cv->battlerAtk].volatiles.skyDropTarget = 0;
                gBattlescriptCurrInstr = BattleScript_SkyDropNoTarget;
                return CANCELER_RESULT_FAILURE;
            }

            gBattleMons[cv->battlerAtk].volatiles.skyDropTarget = 0;
            gBattleMons[cv->battlerDef].volatiles.semiInvulnerable = STATE_NONE;
        }
        return CANCELER_RESULT_SUCCESS;
    }

    if (!gBattleMons[cv->battlerAtk].volatiles.chargeTurn) // First turn charge
    {
        if (isSkyDrop) // First turn failure checks for Sky Drop
        {
            bool32 isAlly = IsBattlerAlly(cv->battlerAtk, cv->battlerDef);
            bool32 isSemiInvulnerable = gBattleMons[cv->battlerDef].volatiles.semiInvulnerable != STATE_NONE;
            bool32 isSubBlocked = DoesSubstituteBlockMove(cv->battlerAtk, cv->battlerDef, cv->move);

            if (isAlly || isSemiInvulnerable || isSubBlocked)
            {
                gBattlescriptCurrInstr = BattleScript_ButItFailed;
                return CANCELER_RESULT_FAILURE;
            }
            else if (GetBattlerWeight(cv->battlerDef, cv->abilities[cv->battlerDef], cv->holdEffects[cv->battlerDef]) >= 2000)
            {
                gBattlescriptCurrInstr = BattleScript_SkyDropTargetTooHeavy;
                return CANCELER_RESULT_FAILURE;
            }
        }

        gLockedMoves[cv->battlerAtk] = cv->move;
        gBattleMons[cv->battlerAtk].volatiles.chargeTurn = TRUE;
        gBattleStruct->setChargeTurnAdditionalEffects = TRUE;
        if (gBattleMoveEffects[cv->moveEffect].semiInvulnerableEffect)
            gBattleMons[cv->battlerAtk].volatiles.semiInvulnerable = GetTwoTurnMoveSemiInvulnerability(cv->move);

        if (isSkyDrop)
        {
            if (gBattleMons[cv->battlerDef].volatiles.rampageTurns > 0)
                gBattleMons[cv->battlerDef].volatiles.confuseAfterDrop = TRUE;

            CancelMultiTurnMoves(cv->battlerDef);
            gBattleMons[cv->battlerAtk].volatiles.skyDropTarget = cv->battlerDef + 1;
            gBattleMons[cv->battlerDef].volatiles.semiInvulnerable = STATE_SKY_DROP_TARGET;
            if (gSideTimers[GetBattlerSide(cv->battlerDef)].followmeTimer != 0
             && gSideTimers[GetBattlerSide(cv->battlerDef)].followmeTarget == cv->battlerDef)
                gSideTimers[GetBattlerSide(cv->battlerDef)].followmeTimer = 0;
        }
        BattleScriptCall(BattleScript_TwoTurnMoveCharging);
        return CANCELER_RESULT_RUN_SCRIPT;
    }

    enum TwoTurnMoveActivation twoTurnMoveActivation = GetTwoTurnMoveActivation(cv, GetWeather());

    if (cv->abilities[cv->battlerAtk] == ABILITY_TIME_DOMINUS)
    {
        RecordAbilityBattle(cv->battlerAtk, ABILITY_TIME_DOMINUS);
        if (HasApexAbility(cv->battlerAtk))
            RecordItemEffectBattle(cv->battlerAtk, HOLD_EFFECT_APEX_METEORITE);
    }

    switch (twoTurnMoveActivation)
    {
    case ACTIVATION_TIME_DOMINUS:
        SetValuesForTwoTurnMoveFire(cv);
        if (isSkyDrop)
        {
            gBattleMons[cv->battlerAtk].volatiles.skyDropTarget = 0;
            gBattleMons[cv->battlerDef].volatiles.semiInvulnerable = STATE_NONE;
        }
        BattleScriptCall(BattleScript_TimeDominusCompressed);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    case ACTIVATION_WEATHER:
        SetValuesForTwoTurnMoveFire(cv);
        return CANCELER_RESULT_SUCCESS;
    case ACTIVATION_MEGA_SOL:
        SetValuesForTwoTurnMoveFire(cv);
        BattleScriptCall(BattleScript_MegaSolActivatesTwoTurnMove);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    case ACTIVATION_POWER_HERB:
        gLastUsedItem = gBattleMons[cv->battlerAtk].item;
        SetValuesForTwoTurnMoveFire(cv);
        BattleScriptCall(BattleScript_PowerHerbActivation);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    case ACTIVATION_NEXT_TURN:
        gBattleMons[cv->battlerAtk].volatiles.multipleTurns = TRUE;
        gBattlescriptCurrInstr = BattleScript_MoveEnd;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerSnatch(struct BattleCalcValues *cv)
{
    for (u32 i = 0; i < gCurrentTurnActionNumber; i++)
    {
        if (!gProtectStructs[gBattlerByTurnOrder[i]].stealMove
         || !MoveCanBeSnatched(cv->move))
            continue;

        if (B_SNATCH < GEN_5 || !gBattleStruct->snatchedMoveIsUsed)
        {
            u32 snatchBattler = gBattlerByTurnOrder[i];

            if (!gBattleStruct->snatchedMoveIsUsed) // store only once
            {
                SaveBattlerAttacker(gBattlerAttacker);
                SaveBattlerTarget(gBattlerTarget);
            }

            gProtectStructs[snatchBattler].stealMove = FALSE;
            gBattleStruct->snatchedMoveIsUsed = TRUE;

            if (cv->battlerAtk == cv->battlerDef)
            {
                gBattleScripting.battler = gBattlerTarget;
                gBattlerAttacker = gBattlerTarget = snatchBattler;
            }
            else
            {
                gBattlerTarget = gBattlerAttacker;
                gBattlerAttacker = snatchBattler;
            }

            gBattleStruct->eventState.atkCanceler = CANCELER_VOLATILE_BLOCKED;
            BattleScriptCall(BattleScript_SnatchedMove);
            return CANCELER_RESULT_RUN_SCRIPT;
        }
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerExplosion(struct BattleCalcValues *cv)
{
    // KO user of Explosion; for Final Gambit doesn't happen if target is immune or if it missed
    if (IsExplosionMove(cv->move)
     && (cv->moveEffect != EFFECT_FINAL_GAMBIT || IsAnyTargetAffected()))
    {
        if (cv->abilities[cv->battlerAtk] == ABILITY_GREAT_MINDS && IsMoveGreatMindsAffected(cv->battlerAtk, cv->move))
        {
            gSpecialStatuses[cv->battlerAtk].greatMindsExplosion = TRUE;
            return CANCELER_RESULT_SUCCESS;
        }
        BattleScriptCall(BattleScript_Explosion);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerMoveSpecificMessage(struct BattleCalcValues *cv)
{
    switch (cv->moveEffect)
    {
    case EFFECT_MAGNITUDE:
        CalculateMagnitudeDamage();
        BattleScriptCall(BattleScript_MagnitudeMessage);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    case EFFECT_FICKLE_BEAM:
        gBattleStruct->fickleBeamBoosted = RandomPercentage(RNG_FICKLE_BEAM, 30);
        if (gBattleStruct->fickleBeamBoosted)
        {
            BattleScriptCall(BattleScript_FickleBeamMessage);
            return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
        }
        break;
    default:
        break;
    }

    return CANCELER_RESULT_SUCCESS;
}

static bool32 NoTargetPresent(enum BattlerId battler, enum Move move, enum MoveTarget moveTarget)
{
    switch (moveTarget)
    {
    case TARGET_USER_AND_ALLY:
        return FALSE; // At least user is present
    case TARGET_ALLY:
        if (!IsBattlerAlive(gBattlerTarget))
            return TRUE;
        break;
    case TARGET_SELECTED:
    case TARGET_DEPENDS:
    case TARGET_RANDOM:
        if (!IsBattlerAlive(gBattlerTarget) && !IsBattlerAlive(GetBattleMoveTarget(move, TARGET_NONE)))
            return TRUE;
        break;
    case TARGET_BOTH:
    case TARGET_SMART:
        if (!IsBattlerAlive(gBattlerTarget) && !IsBattlerAlive(GetPartnerBattler(gBattlerTarget)))
            return TRUE;
        break;
    case TARGET_FOES_AND_ALLY:
        if (!IsBattlerAlive(gBattlerTarget) && !IsBattlerAlive(GetPartnerBattler(gBattlerTarget)) && !IsBattlerAlive(GetPartnerBattler(gBattlerAttacker)))
            return TRUE;
        break;
    default:
        break;
    }

    return FALSE;
}

static enum CancelerResult CancelerNoTarget(struct BattleCalcValues *cv)
{
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);

    if (NoTargetPresent(gBattlerAttacker, gCurrentMove, moveTarget))
    {
        // Fling item should be removed from attacker in this case
        gBattleStruct->flungItem = FLUNG_ITEM_REMOVE;
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        return CANCELER_RESULT_FAILURE;
    }

    if (cv->battlerAtk == cv->battlerDef && moveTarget == TARGET_ALLY)
    {
        gBattleStruct->flungItem = FLUNG_ITEM_REMOVE;
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        return CANCELER_RESULT_FAILURE;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerTookAttack(struct BattleCalcValues *cv)
{
    if (gSpecialStatuses[cv->battlerDef].abilityRedirected)
    {
        gSpecialStatuses[cv->battlerDef].abilityRedirected = FALSE;
        BattleScriptCall(BattleScript_TookAttack);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }
    return CANCELER_RESULT_SUCCESS;
}

static void SetDamageContextValues(struct DamageContext *ctx, struct BattleCalcValues *cv)
{
    ctx->battlerAtk = cv->battlerAtk;
    ctx->battlerDef = cv->battlerDef;
    ctx->move = cv->move;
    ctx->chosenMove = gChosenMove;
    ctx->moveType = GetBattleMoveType(cv->move);
    ctx->weather = GetWeather();
    ctx->terrain = gFieldTimers.terrain;
    ctx->updateFlags = TRUE;
    ctx->runScript = TRUE;
    for (enum BattlerId battler = B_BATTLER_0; battler < gBattlersCount; battler++)
    {
        ctx->abilities[battler] = cv->abilities[battler];
        ctx->holdEffects[battler] = cv->holdEffects[battler];
    }
}

static bool32 IsTargetUnaffectedByMoveEffect(struct BattleCalcValues *cv)
{
    switch (cv->moveEffect)
    {
    case EFFECT_ENDEAVOR:
        if (GetBattlerHP(cv->battlerDef) <= gBattleMons[cv->battlerAtk].hp)
        {
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_NO_EFFECT;
            BattleScriptCall(BattleScript_ItDoesntAffectScrTarget);
            return TRUE;
        }
        break;
    case EFFECT_DREAM_EATER:
        if (GetConfig(B_DREAM_EATER_SUBSTITUTE) < GEN_5 && IsSubstituteProtected(cv->battlerAtk, cv->battlerDef, cv->abilities[cv->battlerAtk], cv->move))
        {
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_NO_EFFECT;
            BattleScriptCall(BattleScript_ItDoesntAffectScrTarget);
            return TRUE;
        }
        if (!IsAsleepOrComatose(cv->battlerDef, cv->abilities[cv->battlerDef]))
        {
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_NO_EFFECT;
            BattleScriptCall(BattleScript_ItDoesntAffectScrTarget);
            return TRUE;
        }
        break;
    case EFFECT_SYNCHRONOISE:
        if (!DoBattlersShareType(cv->battlerAtk, cv->battlerDef))
        {
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_NO_EFFECT;
            BattleScriptCall(BattleScript_ItDoesntAffectScrTarget);
            return TRUE;
        }
        break;
    case EFFECT_SKY_DROP:
        if (!gBattleMons[cv->battlerAtk].volatiles.chargeTurn
         && IS_BATTLER_OF_TYPE(cv->battlerDef, TYPE_FLYING))
        {
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_NO_EFFECT;
            BattleScriptCall(BattleScript_SkyDropFlyingType);
            return TRUE;
        }
        break;
    default:
        break;
    }

    return FALSE;
}

static enum CancelerResult TargetAvoidedAttack(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    gLastLandedMoves[battlerDef] = 0; // Might need investigation on what exactly clears is
    gLastHitByType[battlerDef] = 0;
    gBattleScripting.battler = battlerDef;
    gSpecialStatuses[battlerDef].resultMessagePrinted = TRUE;
    CancelMultiTurnMoves(battlerAtk);
    return CANCELER_RESULT_RUN_SCRIPT;
}

static enum CancelerResult CancelerTargetFailure(struct BattleCalcValues *cv)
{
    bool32 bounced = FALSE;
    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);
    s32 movePriority = GetChosenMovePriority(cv->battlerAtk, cv->abilities[cv->battlerAtk]);

    struct DamageContext ctx = {0};
    SetDamageContextValues(&ctx, cv);

    switch (gBattleStruct->eventState.moveEndBlock)
    {
    case TARGET_FAILURE_SEMI_INVULNERABILITY:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (moveTarget == TARGET_OPPONENTS_FIELD)
                continue;

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            if (!CanBreakThroughSemiInvulnerablity(cv->battlerAtk, cv->battlerDef, cv->abilities[cv->battlerAtk], cv->abilities[cv->battlerDef], cv->move))
            {
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                BattleScriptCall(BattleScript_BattlerAvoidedAttack);
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case TARGET_FAILURE_PSYCHIC_TERRAIN:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = ctx.battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (moveTarget == TARGET_OPPONENTS_FIELD)
                continue;

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            if (CanPsychicTerrainProtectTarget(&ctx, movePriority))
            {
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case TARGET_FAILURE_PROTECT:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (moveTarget == TARGET_OPPONENTS_FIELD)
                continue;

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            if (IsBattlerProtected(cv))
            {
                if (cv->abilities[cv->battlerDef] == ABILITY_BALANCE_MASTER
                 && GetProtectType(gProtectStructs[cv->battlerDef].protected) == PROTECT_TYPE_SINGLE
                 && IsMoveMakingContact(cv->battlerAtk, cv->battlerDef, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->move))
                    QueuePostActionContactMove(cv->battlerDef, cv->battlerAtk, ABILITY_BALANCE_MASTER);
                SetOrClearRageVolatile();
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_PROTECTED);
                BattleScriptCall(BattleScript_TargetProtected);
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case TARGET_FAILURE_BOUNCE:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            if (!IsSemiInvulnerable(cv->battlerDef, CHECK_ALL) && CanBattlerBounceBackMove(cv))
            {
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                gLastLandedMoves[cv->battlerDef] = 0;
                gLastHitByType[cv->battlerDef] = 0;
                bounced = TRUE;
            }
        }

        if (bounced && moveTarget == TARGET_OPPONENTS_FIELD)
        {
            gBattleStruct->eventState.moveEndBlock = 0;
            gBattlescriptCurrInstr = BattleScript_MoveEnd;
            return CANCELER_RESULT_FAILURE;
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case TARGET_FAILURE_TARGET_BLOCKED:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = ctx.battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (moveTarget == TARGET_OPPONENTS_FIELD)
                continue;

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            if (CanMoveBeBlockedByTarget(&ctx, movePriority))
            {
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case TARGET_FAILURE_EFFECTIVENESS:
        for (u32 slot = 0; slot < gBattlersCount; slot++)
        {
            cv->battlerDef = ctx.battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (moveTarget == TARGET_OPPONENTS_FIELD)
                continue;

            if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
                continue;

            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            gSpecialStatuses[cv->battlerDef].storedTypeEffectiveness = ctx.typeEffectivenessModifier;

            if (ctx.abilityBlocked)
            {
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                gBattlerAbility = cv->battlerDef;
                RecordAbilityBattle(cv->battlerDef, cv->abilities[cv->battlerDef]);
                BattleScriptCall(BattleScript_AbilityProtectedTarget);
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
            else if (ctx.airBalloonBlocked)
            {
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                BattleScriptCall(BattleScript_DoesntAffectScripting);
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
            else if (ctx.typeEffectivenessModifier == UQ_4_12(0.0))
            {
                TryInitializeTrainerSlideFirstIneffectiveMove(cv->battlerDef, cv->battlerAtk);
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
                BattleScriptCall(BattleScript_DoesntAffectScripting);
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
            else if (IsTargetUnaffectedByMoveEffect(cv))
            {
                TryInitializeTrainerSlideFirstIneffectiveMove(cv->battlerDef, cv->battlerAtk);
                gSpecialStatuses[cv->battlerDef].updateStallMons = TRUE;
                return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
    }

    if (IsDoubleBattle())
    {
        if (moveTarget == TARGET_BOTH)
            gBattleStruct->numSpreadTargets = CountAliveMonsInBattle(BATTLE_ALIVE_EXCEPT_BATTLER_SIDE, gBattlerAttacker);
        else if (moveTarget == TARGET_FOES_AND_ALLY)
            gBattleStruct->numSpreadTargets = CountAliveMonsInBattle(BATTLE_ALIVE_EXCEPT_BATTLER, gBattlerAttacker);
    }

    cv->battlerDef = gBattlerTarget;
    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.atkCancelerBattler = 0;

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerStatChangeResolution(struct BattleCalcValues *cv)
{
    if (!IsStatChangeMove(cv->battlerAtk, cv->move))
        return CANCELER_RESULT_SUCCESS;

    if (DoStatChange(cv) != MOVE_RESULT_DONE)
        return CANCELER_RESULT_RUN_SCRIPT;

    gBattleStruct->eventState.atkCanceler = CANCELER_END;
    return CANCELER_RESULT_END;
}

static bool32 IsMoveParentalBondAffected(struct BattleCalcValues *cv)
{
    bool32 hasBond = cv->abilities[cv->battlerAtk] == ABILITY_PARENTAL_BOND
        || (cv->abilities[cv->battlerAtk] == ABILITY_GNASHING_TEETH && IsMoveGnashingTeethAffected(cv->battlerAtk, cv->move));
    if (!hasBond || gBattleStruct->numSpreadTargets > 1 || IsMoveParentalBondBanned(cv->move) || cv->isStatusMove || gBattleMoveEffects[cv->moveEffect].twoTurnEffect || cv->moveEffect == EFFECT_OHKO || cv->move == MOVE_STRUGGLE)
        return FALSE;
    return TRUE;
}

static void SetPossibleNewSmartTarget(u32 move)
{
    if (!IsBattlerUnaffectedByMove(gBattlerTarget)
     || !CanTargetPartner(gBattlerAttacker, gBattlerTarget)
     || IsAffectedByFollowMe(gBattlerAttacker, GetBattlerSide(gBattlerTarget), move)
     || GetBattlerMoveTargetType(gBattlerAttacker, move) != TARGET_SMART)
        return;

    enum BattlerId partner = GetPartnerBattler(gBattlerTarget);
    if (!IsBattlerUnaffectedByMove(partner))
        gBattlerTarget = partner;
}

static void SetRandomMultiHitCounter(enum HoldEffect holdEffect)
{
    if (holdEffect == HOLD_EFFECT_LOADED_DICE)
        gMultiHitCounter = RandomUniform(RNG_LOADED_DICE, 4, 5);
    else if (GetConfig(B_MULTI_HIT_CHANCE) >= GEN_5)
        gMultiHitCounter = RandomWeighted(RNG_HITS, 0, 0, 7, 7, 3, 3); // 35%: 2 hits, 35%: 3 hits, 15% 4 hits, 15% 5 hits.
    else
        gMultiHitCounter = RandomWeighted(RNG_HITS, 0, 0, 3, 3, 1, 1); // 37.5%: 2 hits, 37.5%: 3 hits, 12.5% 4 hits, 12.5% 5 hits.
}

static void SetMultiHitTargets(enum BattlerId battlerAtk)
{
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (IsBattlerAlive(target) && !IsBattlerMoveResultSet(target, MOVE_RESULT_INVALID_TARGET | MOVE_RESULT_IGNORE_SELF))
            gSpecialStatuses[battlerAtk].multiHitTargets |= 1u << target;
    }
}

static enum CancelerResult CancelerMultihitMoves(struct BattleCalcValues *cv)
{
    SetPossibleNewSmartTarget(cv->move);

    bool32 greatMinds = cv->moveEffect == EFFECT_FUTURE_SIGHT
        ? gSpecialStatuses[cv->battlerAtk].greatMindsActive
        : (cv->abilities[cv->battlerAtk] == ABILITY_GREAT_MINDS && IsMoveGreatMindsAffected(cv->battlerAtk, cv->move));
    bool32 martialMaster = IsMartialMasterDoubleHit(cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->move);

    if (cv->move == MOVE_DAZZLING_DASH)
        gBattleStruct->dazzlingDashWasSlower = GetBattlerTotalSpeedStat(cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk])
            < GetBattlerTotalSpeedStat(cv->battlerDef, cv->abilities[cv->battlerDef], cv->holdEffects[cv->battlerDef]);

    if (IsBattlerUnaffectedByMove(gBattlerTarget)
     && !((greatMinds || martialMaster || (cv->abilities[cv->battlerAtk] == ABILITY_REVERBERATIONS
       && IsMoveReverberationsAffected(cv->battlerAtk, cv->move)))
       && IsAnyTargetAffected()))
    {
        gMultiHitCounter = 0;
    }
    else if (cv->moveEffect == EFFECT_HAUNTING_SHRIEKS)
    {
        gMultiHitCounter = GetHauntingShrieksHitCount(cv->battlerAtk);
        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 1, 0)
    }
    else if (IsMultiHitMove(cv->move))
    {
        if (cv->moveEffect == EFFECT_SPECIES_POWER_OVERRIDE
         && gBattleMons[cv->battlerAtk].species == GetMoveSpeciesPowerOverride_Species(cv->move))
        {
            gMultiHitCounter = GetMoveSpeciesPowerOverride_NumOfHits(cv->move);
        }
        else if (cv->abilities[cv->battlerAtk] == ABILITY_SKILL_LINK)
        {
            gMultiHitCounter = 5;
        }
        else
        {
            SetRandomMultiHitCounter(cv->holdEffects[cv->battlerAtk]);
        }

        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 1, 0)
    }
    else if (GetBattlerMoveStrikeCount(cv->battlerAtk, cv->move) > 1)
    {
        if (GetMoveEffect(cv->move) == EFFECT_POPULATION_BOMB
         && cv->holdEffects[cv->battlerAtk] == HOLD_EFFECT_LOADED_DICE
         && cv->abilities[cv->battlerAtk] != ABILITY_SKILL_LINK)
        {
            gMultiHitCounter = RandomUniform(RNG_LOADED_DICE, 4, 10);
        }
        else
        {
            gMultiHitCounter = GetBattlerMoveStrikeCount(cv->battlerAtk, cv->move);
        }

        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 3, 0)
    }
    else if (cv->moveEffect == EFFECT_BEAT_UP)
    {
        struct Pokemon* party = GetBattlerParty(cv->battlerAtk);
        int i;
        gBattleStruct->beatUpSlot = 0;
        gMultiHitCounter = 0;
        memset(gBattleStruct->beatUpSpecies, 0xFF, sizeof(gBattleStruct->beatUpSpecies));

        for (i = 0; i < PARTY_SIZE; i++)
        {
            enum Species species = GetMonData(&party[i], MON_DATA_SPECIES);
            if (species != SPECIES_NONE
             && GetMonData(&party[i], MON_DATA_HP)
             && !GetMonData(&party[i], MON_DATA_IS_EGG)
             && !GetMonData(&party[i], MON_DATA_STATUS))
            {
                if (GetConfig(B_BEAT_UP) >= GEN_5)
                    gBattleStruct->beatUpSpecies[gMultiHitCounter] = species;
                else
                    gBattleStruct->beatUpSpecies[gMultiHitCounter] = i;
                gMultiHitCounter++;
            }
        }

        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 1, 0)
    }
    else if (greatMinds)
    {
        SetMultiHitTargets(cv->battlerAtk);
        gSpecialStatuses[cv->battlerAtk].greatMindsActive = TRUE;
        if (cv->moveEffect != EFFECT_FUTURE_SIGHT)
            RecordAbilityBattle(cv->battlerAtk, ABILITY_GREAT_MINDS);
        gMultiHitCounter = GREAT_MINDS_HITS;
        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 1, 0)
    }
    else if (IsMoveParentalBondAffected(cv) || martialMaster
          || (cv->abilities[cv->battlerAtk] == ABILITY_REVERBERATIONS
           && !cv->isStatusMove
           && IsMoveReverberationsAffected(cv->battlerAtk, cv->move)))
    {
        if (cv->abilities[cv->battlerAtk] == ABILITY_REVERBERATIONS || martialMaster)
        {
            SetMultiHitTargets(cv->battlerAtk);
            RecordAbilityBattle(cv->battlerAtk, cv->abilities[cv->battlerAtk]);
        }
        gSpecialStatuses[gBattlerAttacker].parentalBondState = PARENTAL_BOND_1ST_HIT;
        gMultiHitCounter = 2;
        PREPARE_BYTE_NUMBER_BUFFER(gBattleScripting.multihitString, 1, 0)
    }
    else
    {
        gMultiHitCounter = 0;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerSubstitute(struct BattleCalcValues *cv)
{
    if (!cv->isStatusMove
     || IsStatChangeMove(cv->battlerAtk, cv->move)
     || (cv->moveEffect == EFFECT_TRANSFORM && GetConfig(B_TRANSFORM_SUBSTITUTE_FAIL) < GEN_5))
        return CANCELER_RESULT_SUCCESS;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
            continue;

        if (IsSubstituteProtected(cv->battlerAtk, battler, cv->abilities[cv->battlerAtk], cv->move))
            gBattleStruct->moveResultFlags[battler] = MOVE_RESULT_DOESNT_AFFECT_FOE;
    }

    if (!IsAnyTargetAffected())
    {
        gBattlescriptCurrInstr = BattleScript_ButItFailed;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }

    return CANCELER_RESULT_SUCCESS;
}

static bool32 ShouldSkipAccuracyCalcPastFirstHit(enum BattlerId battlerAtk, enum Ability abilityAtk, enum HoldEffect holdEffectAtk, u32 moveEffect)
{
    if (gSpecialStatuses[battlerAtk].reverberationsEcho)
        return TRUE;
    if (gSpecialStatuses[battlerAtk].parentalBondState == PARENTAL_BOND_2ND_HIT)
        return TRUE;

    if (!gSpecialStatuses[battlerAtk].multiHitOn)
        return FALSE;

    if (abilityAtk == ABILITY_SKILL_LINK || holdEffectAtk == HOLD_EFFECT_LOADED_DICE)
        return TRUE;

    if (moveEffect == EFFECT_TRIPLE_KICK || moveEffect == EFFECT_POPULATION_BOMB)
        return FALSE;

    return TRUE; // multiHitOn is set so skip Acc check for everything else
}

static bool32 ShouldSkipFRLGAccuracyCheck(void)
{
    if (!IS_FRLG)
        return FALSE;

    ;

    if (HasAnyBattleFlags(BATTLE_TYPE_POKEDUDE))
        return TRUE;

    return FALSE;
}
static bool32 CanRedirectToPartner(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Move move)
{
    enum BattlerId partner = GetPartnerBattler(battlerDef);

    if (GetBattlerMoveTargetType(battlerAtk, move) != TARGET_SMART)
        return FALSE;

    if (!IsAffectedByFollowMe(battlerAtk, GetBattlerSide(battlerDef), move)
     && CanTargetPartner(battlerAtk, battlerDef)
     && !IsBattlerUnaffectedByMove(partner))
        return TRUE;

    AddBattlerMoveResultFlag(partner, MOVE_RESULT_NOT_TARGETED);

    return FALSE;
}

static enum CancelerResult CancelerAccuracyCheck(struct BattleCalcValues *cv)
{
    enum SmartTargetState {
        INITIAL_STATE,
        MISSED_FIRST_TARGET,
        TRY_SECOND_TARGET,
    };

    if (ShouldSkipFRLGAccuracyCheck() || IsStatChangeMove(cv->battlerAtk, cv->move) || ShouldSkipAccuracyCalcPastFirstHit(cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->moveEffect))
        return CANCELER_RESULT_SUCCESS;

    enum SmartTargetState smartTargetState = INITIAL_STATE;
    bool32 isSmartTarget = CanRedirectToPartner(cv->battlerAtk, cv->battlerDef, cv->move);
    bool32 isMultiHitOn = gSpecialStatuses[cv->battlerAtk].multiHitOn;

    while (gBattleStruct->eventState.atkCancelerBattler < gBattlersCount)
    {
        cv->battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.atkCancelerBattler);

        gBattleStruct->eventState.atkCancelerBattler++;

        if (ShouldSkipFailureCheckOnBattler(cv->battlerDef))
            continue;

        if (DoesMoveMissTarget(cv))
        {
            AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_MISSED);

            if (cv->holdEffects[cv->battlerAtk] == HOLD_EFFECT_BLUNDER_POLICY
             && cv->moveEffect != EFFECT_OHKO
             && !isMultiHitOn)
                gBattleStruct->blunderPolicy = TRUE;

            if (isSmartTarget && smartTargetState == INITIAL_STATE)
            {
                smartTargetState = MISSED_FIRST_TARGET;
                gBattlerTarget = GetPartnerBattler(cv->battlerDef); // Smart target to partner if miss
            }

            if (!isMultiHitOn)
            {
                gLastLandedMoves[cv->battlerDef] = MOVE_NONE;
                gLastHitByType[cv->battlerDef] = TYPE_NONE;
            }

            if (smartTargetState == MISSED_FIRST_TARGET) // Silent failure on first target.
            {
                smartTargetState = TRY_SECOND_TARGET;
                continue;
            }

            if (isMultiHitOn)
            {
                gMultiHitCounter = 0;
                gBattleStruct->moveDamage[cv->battlerDef] = 0;
                gSpecialStatuses[cv->battlerDef].damagedByAttack = FALSE;
                BattleScriptCall(BattleScript_BattlerAvoidedMultiHit);
            }
            else if (IsBattlerMoveResultSet(cv->battlerDef, MOVE_RESULT_ONE_HIT_KO_STURDY))
            {
                gLastUsedAbility = ABILITY_STURDY;
                gBattlerAbility = cv->battlerDef;
                BattleScriptCall(BattleScript_SturdyPreventsOHKO);
            }
            else if (IsBattlerMoveResultSet(cv->battlerDef, MOVE_RESULT_ONE_HIT_KO_NO_AFFECT))
            {
                BattleScriptCall(BattleScript_ItDoesntAffectScrTarget);
            }
            else
            {
                BattleScriptCall(BattleScript_BattlerAvoidedAttack);
            }

            if (IsBattlerMoveResultSet(cv->battlerDef, (MOVE_RESULT_ONE_HIT_KO_NO_AFFECT | MOVE_RESULT_ONE_HIT_KO_STURDY)))
                gBattleStruct->moveResultFlags[cv->battlerDef] = (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED);

            SetOrClearRageVolatile();
            gBattleScripting.battler = cv->battlerDef;
            return CANCELER_RESULT_RUN_SCRIPT;
        }
    }

    cv->battlerDef = gBattlerTarget;
    gBattleStruct->eventState.atkCancelerBattler = 0;

    if (!IsAnyTargetAffected())
    {
        gBattlescriptCurrInstr = cv->moveEffect == EFFECT_FUTURE_SIGHT && gSpecialStatuses[cv->battlerAtk].greatMindsActive
            ? BattleScript_FutureAttackMoveEnd
            : BattleScript_MoveEnd;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerShellBurst(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove)
        return CANCELER_RESULT_SUCCESS;

    struct DamageContext ctx = {0};
    ctx.battlerAtk = cv->battlerAtk;
    ctx.move = cv->move;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        ctx.abilities[battler] = cv->abilities[battler];

    // Resolve only after accuracy, Protect and type immunities. Mark this target
    // unaffected for the entire move so subsequent strikes cannot consume it twice.
    while (gBattleStruct->eventState.atkCancelerBattler < gBattlersCount)
    {
        cv->battlerDef = ctx.battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.atkCancelerBattler++);
        if (GetBattlerMoveTargetType(cv->battlerAtk, cv->move) == TARGET_SMART && cv->battlerDef != gBattlerTarget)
            continue;
        if (ShouldSkipFailureCheckOnBattler(cv->battlerDef) || !CanShellBurstBlockMove(&ctx))
            continue;

        gBattleStruct->battlerState[cv->battlerDef].shellBurstUsed = TRUE;
        gBattleStruct->battlerState[cv->battlerDef].shellBurstBlockedMove = TRUE;
        gBattleStruct->moveDamage[cv->battlerDef] = 0;
        AddBattlerMoveResultFlag(cv->battlerDef, MOVE_RESULT_FAILED);
        SetStatChange(cv->battlerDef, STAT_DEF, -1);
        SetStatChange(cv->battlerDef, STAT_SPDEF, -1);
        SetStatChange(cv->battlerDef, STAT_ATK, 2);
        SetStatChange(cv->battlerDef, STAT_SPATK, 2);
        SetStatChange(cv->battlerDef, STAT_SPEED, 2);
        gEffectBattler = gBattlerAbility = gBattleScripting.battler = cv->battlerDef;
        gLastUsedAbility = ABILITY_SHELL_BURST;
        RecordAbilityBattle(cv->battlerDef, ABILITY_SHELL_BURST);
        BattleScriptCall(BattleScript_ShellBurstActivates);
        return TargetAvoidedAttack(cv->battlerAtk, cv->battlerDef);
    }
    cv->battlerDef = gBattlerTarget;
    gBattleStruct->eventState.atkCancelerBattler = 0;
    if (!IsAnyTargetAffected())
    {
        gBattlescriptCurrInstr = BattleScript_MoveEnd;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }
    return CANCELER_RESULT_SUCCESS;
}

static bool32 ShouldSkipBattlerForDamage(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (gBattleStruct->numSpreadTargets == 0 && battlerDef != gBattlerTarget)
        return TRUE;
    if (IsBattlerMoveResultSet(battlerDef, (MOVE_RESULT_INVALID_TARGET | MOVE_RESULT_IGNORE_SELF)))
        return TRUE;
    return FALSE;
}

static enum CancelerResult CancelerPreAttackMoveEffect(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove) return CANCELER_RESULT_SUCCESS;

    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(cv->move);

    while (gBattleStruct->eventState.atkCancelerBattler < gBattlersCount)
    {
        enum BattlerId effectBattler = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.atkCancelerBattler);

        while (gBattleStruct->additionalEffectsCounter < numAdditionalEffects)
        {
            const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(cv->move, gBattleStruct->additionalEffectsCounter);

            bool32 isSelf = effectBattler == cv->battlerAtk;

            if (!additionalEffect->preAttackEffect
             || isSelf != additionalEffect->self
             || (!isSelf && ShouldSkipFailureCheckOnBattler(effectBattler)))
            {
                gBattleStruct->additionalEffectsCounter++;
                continue;
            }

            TryTriggerAdditionalEffect(cv, additionalEffect, effectBattler);

            return CANCELER_RESULT_RUN_SCRIPT; // We don't know if a script should be run or not so try
        }

        gBattleStruct->eventState.atkCancelerBattler++;
        gBattleStruct->additionalEffectsCounter = 0;
    }

    cv->battlerDef = gBattlerTarget;
    gBattleStruct->additionalEffectsCounter = 0;
    gBattleStruct->eventState.atkCancelerBattler = 0;
    return CANCELER_RESULT_SUCCESS;
}

static bool32 ShouldAvoidStatusEffectOnBattler(enum BattlerId battlerAtk, enum BattlerId battlerDef, bool32 isFieldTargetType)
{
    if (battlerAtk == battlerDef && gBattlerAttacker == gBattlerTarget)
        return FALSE;

    if (isFieldTargetType)
        return battlerDef != gBattlerTarget;

    if (IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_INVALID_TARGET))
       return TRUE;

    return FALSE;
}

static enum CancelerResult CancelerStatusEffects(struct BattleCalcValues *cv)
{
    if (!cv->isStatusMove) return CANCELER_RESULT_SUCCESS;

    u32 playMoveAnim = 0;
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(cv->move);

    if (numAdditionalEffects == 0) return CANCELER_RESULT_SUCCESS;

    cv->onlyChecking = TRUE;

    enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);
    bool32 isFieldTargetType = moveTarget == TARGET_FIELD || moveTarget == TARGET_OPPONENTS_FIELD;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (ShouldAvoidStatusEffectOnBattler(cv->battlerAtk, battler, isFieldTargetType))
            continue;

        bool32 isAllyAffected = FALSE;

        u32 i = 0;
        while (i < numAdditionalEffects)
        {
            const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(cv->move, i);
            struct SetEffect se = {0};
            enum BattlerId partner = additionalEffect->self ? GetPartnerBattler(cv->battlerAtk)
                                                            : GetPartnerBattler(battler);
            se.additionalEffect = additionalEffect;
            se.moveEffect = additionalEffect->moveEffect;
            se.script = gBattlescriptCurrInstr;
            cv->battlerDef = battler;
            se.effectBattler = additionalEffect->self ? cv->battlerAtk : battler;
            se.effectBattler = isAllyAffected ? partner : se.effectBattler;
            se.primary = TRUE;
            se.certain = TRUE;
            SetMoveEffect(cv, &se);
            AddBattlerMoveResultFlag(se.effectBattler, MOVE_RESULT_VALID_STATUS_TARGET);
            if (!se.effectFailed)
            {
                playMoveAnim |= 1u << se.effectBattler;
            }

            bool32 shouldTryEffectOnAlly = additionalEffect->onSide
                                        && !isAllyAffected
                                        && IsBattlerAlive(partner)
                                        && IsDoubleBattle();
            if (shouldTryEffectOnAlly)
            {

                AddBattlerMoveResultFlag(partner, MOVE_RESULT_VALID_STATUS_TARGET);
                isAllyAffected = TRUE;
            }
            else
            {
                isAllyAffected = FALSE;
                i++;
            }
        }
    }

    cv->onlyChecking = FALSE;
    cv->battlerDef = gBattlerTarget;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        bool32 isBattlerAffected = playMoveAnim & 1u << battler; // "possible" target
        if (!isBattlerAffected && IsBattlerMoveResultSet(battler, MOVE_RESULT_VALID_STATUS_TARGET))
        {
            AddBattlerMoveResultFlag(battler, MOVE_RESULT_DOESNT_AFFECT_FOE);
            continue;
        }
    }

    if (playMoveAnim == 0)
    {
        gBattleStruct->statusMoveFailed = TRUE;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerDamageCalc(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove) return CANCELER_RESULT_SUCCESS;

    gBattleStruct->moveCategoryResolved = FALSE;
    gBattleStruct->resolvedMoveCategory = GetBattleMoveCategory(cv->move);
    gBattleStruct->moveCategoryResolved = TRUE;

    struct DamageContext ctx = {
        .battlerAtk = cv->battlerAtk,
        .move = cv->move,
        .chosenMove = gChosenMove,
        .moveType = GetBattleMoveType(cv->move),
        .weather = GetWeather(),
        .terrain = gFieldTimers.terrain,
        .randomFactor = TRUE,
        .updateFlags = TRUE,
        .useStoredTypeEffectiveness = TRUE,
        .hitIndex = cv->moveEffect == EFFECT_HAUNTING_SHRIEKS ? gBattleScripting.multihitString[4] : 0,
    };

    for (enum BattlerId battler = B_BATTLER_0; battler < gBattlersCount; battler++)
    {
        ctx.abilities[battler] = cv->abilities[battler];
        ctx.holdEffects[battler] = cv->holdEffects[battler];
    }

    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
            continue;

        ctx.battlerDef = battlerDef;
        gProtectStructs[battlerDef].gloriousManeProtectedHit = cv->abilities[battlerDef] == ABILITY_GLORIOUS_MANE
            && IsBattlerAtMaxHp(battlerDef)
            && !DoesSubstituteBlockMove(cv->battlerAtk, battlerDef, cv->move);
        gBattleStruct->moveDamage[battlerDef] = CalculateMoveDamage(&ctx);
    }

    if (gSpecialStatuses[cv->battlerAtk].gemBoost
     && gBattleMons[cv->battlerAtk].item
     && IsAnyTargetAffected())
    {
        BattleScriptCall(BattleScript_GemActivates);
        gLastUsedItem = gBattleMons[cv->battlerAtk].item;
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerPreAnimActivations(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove) return CANCELER_RESULT_SUCCESS;

    switch (gBattleStruct->eventState.moveEndBlock)
    {
    case PRE_ANIM_STRONG_WINDS:
        if (GetWeather() & B_WEATHER_STRONG_WINDS)
        {
            for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
            {
                if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
                    continue;

                enum Type types[3];
                GetBattlerTypes(battlerDef, FALSE, types);

                if (types[0] != TYPE_FLYING && types[1] != TYPE_FLYING && types[2] != TYPE_FLYING)
                    continue;

                if (GetTypeModifier(GetBattleMoveType(cv->move), TYPE_FLYING) > UQ_4_12(1.0))
                {
                    BattleScriptCall(BattleScript_AttackWeakenedByStrongWinds);
                    gBattleStruct->eventState.moveEndBlock++;
                    return CANCELER_RESULT_RUN_SCRIPT;
                }
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
        [[fallthrough]];
    case PRE_ANIM_TERA_SHELL:
        for (u32 slot = 0; slot < MAX_BATTLERS_COUNT; slot++)
        {
            enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

            if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
                continue;

            if (gSpecialStatuses[battlerDef].teraShellAbilityDone)
            {
                gSpecialStatuses[battlerDef].teraShellAbilityDone = FALSE;
                gBattleScripting.battler = battlerDef;
                BattleScriptCall(BattleScript_TeraShellDistortingTypeMatchups);
                return CANCELER_RESULT_RUN_SCRIPT;
            }
        }
        gBattleStruct->eventState.moveEndBlock++;
    case PRE_ANIM_RESIST_BERRY:
        while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
        {
            enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

            gBattleStruct->eventState.moveEndBattler++;

            if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
                continue;

            if (!gSpecialStatuses[battlerDef].berryReduced
             || (gBattleMons[battlerDef].item == ITEM_NONE && gSpecialStatuses[battlerDef].infiniteStashBerry == ITEM_NONE))
                continue;

            if (DoesDisguiseBlockMove(battlerDef, cv->move))
            {
                gSpecialStatuses[battlerDef].berryReduced = FALSE;
                continue;
            }

            gBattleScripting.battler = battlerDef;
            if (gSpecialStatuses[battlerDef].infiniteStashBerry != ITEM_NONE)
            {
                gLastUsedItem = gSpecialStatuses[battlerDef].infiniteStashBerry;
                gBattlerAbility = battlerDef;
                GetBattlerPartyState(battlerDef)->ateBerry = TRUE;
                BattleScriptCall(BattleScript_InfiniteStashResistAnimation);
            }
            else
            {
                gLastUsedItem = gBattleMons[battlerDef].item;
                BattleScriptCall(BattleScript_BerryReduceAnimation);
            }
            return CANCELER_RESULT_RUN_SCRIPT;
        }
        gBattleStruct->eventState.moveEndBlock++;
    default:
        break;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.moveEndBattler = 0;
    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerMoveAnimation(struct BattleCalcValues *cv)
{
    if (gSpecialStatuses[cv->battlerAtk].parentalBondState == PARENTAL_BOND_2ND_HIT)
        return CANCELER_RESULT_SUCCESS;

    bool32 isAnimDisabled = (gHitMarker & (HITMARKER_NO_ANIMATIONS | HITMARKER_DISABLE_ANIMATION)
                          || gBattleStruct->attackAnimPlayed);
    if (isAnimDisabled
     && cv->moveEffect != EFFECT_TRANSFORM
     && cv->moveEffect != EFFECT_SUBSTITUTE
     && cv->moveEffect != EFFECT_ALLY_SWITCH
     // In a wild double battle gotta use the teleport animation if two wild Pokémon are alive.
     && !(cv->moveEffect == EFFECT_TELEPORT && IsWildDoubleBattle() && !IsOnPlayerSide(cv->battlerAtk) && IsBattlerAlive(GetPartnerBattler(cv->battlerAtk))))
    {
        gBattleScripting.animTurn++;
        gBattleScripting.animTargetsHit++;
        BattleScriptCall(BattleScript_PauseShort);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    // handle special move animations.
    if (GetMoveAnimationScript(cv->move) == gBattleAnimMove_ExpandingForce
     && gBattleStruct->numSpreadTargets > 1)
        gBattleScripting.animTurn = 1;

    u32 multiHit = gMultiHitCounter;
    bool32 willDamageFaintBattler = (gBattleMons[cv->battlerDef].hp <= gBattleStruct->moveDamage[cv->battlerDef]);
    if (!gBattleMons[cv->battlerDef].volatiles.substitute && willDamageFaintBattler)
        multiHit = 1;

    BtlController_EmitMoveAnimation(
        cv->battlerAtk,
        B_COMM_TO_CONTROLLER,
        cv->move,
        gBattleScripting.animTurn,
        gBattleMovePower,
        gBattleStruct->moveDamage[cv->battlerDef],
        multiHit
    );

    #if TESTING
    gCountAllocs = TRUE;
    gSpriteAllocs = 0;
    #endif

    MarkBattlerForControllerExec(gBattlerAttacker);
    BattleScriptCall(BattleScript_MoveAnimation);

    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
}

static inline bool32 DoesBattlerNegateDamage(enum BattlerId battler, enum Ability ability, enum Move move)
{
    enum Species species = gBattleMons[battler].species;

    if (gBattleMons[battler].volatiles.transformed)
        return FALSE;
    if (ability == ABILITY_DISGUISE && IsMimikyuDisguised(battler))
        return TRUE;
    if (ability == ABILITY_ICE_FACE && species == SPECIES_EISCUE_ICE && IsBattleMovePhysical(move))
        return TRUE;

    return FALSE;
}

static u32 UpdateEffectivenessResultFlagsForDoubleSpreadMoves(struct BattleCalcValues *cv)
{
    u32 resultFlags = 0;
    for (u32 battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
            continue;
        if (DoesBattlerNegateDamage(battlerDef, cv->abilities[battlerDef], cv->move))
            continue;

        resultFlags |= gBattleStruct->moveResultFlags[battlerDef];

        if (!IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_LOW_EFFECTIVENESS))
            resultFlags &= ~MOVE_RESULT_NOT_VERY_EFFECTIVE; // any battler with 1x or better effectiveness removes NVE sound
        if (IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_HIGH_EFFECTIVENESS))
            resultFlags |= MOVE_RESULT_SUPER_EFFECTIVE; // any super effective result will play SE_SUPER_EFFECTIVE
    }
    return resultFlags;
}

static enum CancelerResult CancelerMoveAllySwitch(struct BattleCalcValues *cv)
{
    if (cv->moveEffect == EFFECT_ALLY_SWITCH && !gBattleStruct->unableToUseMove)
    {
        gBattlerAttacker ^= BIT_FLANK; // adjust user id
        gProtectStructs[gBattlerAttacker].usedAllySwitch = TRUE; // set used ally switch for user
        gBattleScripting.battler = GetPartnerBattler(gBattlerAttacker);
        PrepareStringBattleWithWait(STRINGID_ALLYSWITCHPOSITION, gBattlerAttacker);
        BattleScriptCall(BattleScript_WaitMessage);
        return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
    }

    return CANCELER_RESULT_SUCCESS;
}

static enum CancelerResult CancelerEffectivenessSound(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove)
    {
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
        return CANCELER_RESULT_END;
    }

    u32 moveResultFlags = UpdateEffectivenessResultFlagsForDoubleSpreadMoves(cv);

    switch (moveResultFlags)
    {
    case MOVE_RESULT_SUPER_EFFECTIVE:
    case MOVE_RESULT_EXTREMELY_EFFECTIVE:
        #if TESTING
        if (gTestRunnerEnabled)
            TestRunner_Battle_RecordEffectivenessSound(gBattlerTarget, SE_SUPER_EFFECTIVE);
        #endif
        BtlController_EmitPlaySE(gBattlerTarget, B_COMM_TO_CONTROLLER, SE_SUPER_EFFECTIVE);
        MarkBattlerForControllerExec(gBattlerTarget);
        break;
    case MOVE_RESULT_NOT_VERY_EFFECTIVE:
    case MOVE_RESULT_MOSTLY_INEFFECTIVE:
        #if TESTING
        if (gTestRunnerEnabled)
            TestRunner_Battle_RecordEffectivenessSound(gBattlerTarget, SE_NOT_EFFECTIVE);
        #endif
        BtlController_EmitPlaySE(gBattlerTarget, B_COMM_TO_CONTROLLER, SE_NOT_EFFECTIVE);
        MarkBattlerForControllerExec(gBattlerTarget);
        break;
    case MOVE_RESULT_DOESNT_AFFECT_FOE:
    case MOVE_RESULT_FAILED:
        // no sound
        break;
    case MOVE_RESULT_FOE_ENDURED:
    case MOVE_RESULT_FOE_HUNG_ON:
    case MOVE_RESULT_STURDIED:
    default:
        if (moveResultFlags & MOVE_RESULT_HIGH_EFFECTIVENESS)
        {
            #if TESTING
            if (gTestRunnerEnabled)
                TestRunner_Battle_RecordEffectivenessSound(gBattlerTarget, SE_SUPER_EFFECTIVE);
            #endif
            BtlController_EmitPlaySE(gBattlerTarget, B_COMM_TO_CONTROLLER, SE_SUPER_EFFECTIVE);
            MarkBattlerForControllerExec(gBattlerTarget);
        }
        else if (moveResultFlags & MOVE_RESULT_LOW_EFFECTIVENESS)
        {
            #if TESTING
            if (gTestRunnerEnabled)
                TestRunner_Battle_RecordEffectivenessSound(gBattlerTarget, SE_NOT_EFFECTIVE);
            #endif
            BtlController_EmitPlaySE(gBattlerTarget, B_COMM_TO_CONTROLLER, SE_NOT_EFFECTIVE);
            MarkBattlerForControllerExec(gBattlerTarget);
        }
        else if (!(moveResultFlags & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_FAILED)))
        {
            #if TESTING
            if (gTestRunnerEnabled)
                TestRunner_Battle_RecordEffectivenessSound(gBattlerTarget, SE_EFFECTIVE);
            #endif
            BtlController_EmitPlaySE(gBattlerTarget, B_COMM_TO_CONTROLLER, SE_EFFECTIVE);
            MarkBattlerForControllerExec(gBattlerTarget);
        }
        break;
    }

    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
}

static enum CancelerResult CancelerHitAnimation(struct BattleCalcValues *cv)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef))
            continue;

        if (!(DoesSubstituteBlockMove(cv->battlerAtk, battlerDef, cv->move))
         || gBattleMons[battlerDef].volatiles.substituteHP == 0)
        {
            BtlController_EmitHitAnimation(battlerDef, B_COMM_TO_CONTROLLER);
            MarkBattlerForControllerExec(battlerDef);
        }
    }

    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
}

static enum CancelerResult CancelerSkipFrame(struct BattleCalcValues *cv)
{
    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT;
}

static enum CancelerResult CancelerHealthBarUpdate(struct BattleCalcValues *cv)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (ShouldSkipBattlerForDamage(cv->battlerAtk, battlerDef)
         || DoesSubstituteBlockMove(cv->battlerAtk, battlerDef, cv->move)
         || DoesDisguiseBlockMove(battlerDef, cv->move)
         || DoesIceFaceBlockMove(battlerDef, cv->move))
            continue;

        s32 dmgUpdate = min(gBattleStruct->moveDamage[battlerDef], 10000);
        BtlController_EmitHealthBarUpdate(battlerDef, B_COMM_TO_CONTROLLER, dmgUpdate);
        MarkBattlerForControllerExec(battlerDef);
        if (IsOnPlayerSide(battlerDef) && dmgUpdate > 0)
            gBattleResults.playerMonWasDamaged = TRUE;
    }

    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT; // Update health bar
}

static bool32 TryMoveDamageUpdate(struct BattleCalcValues *cv)
{
    if (DoesSubstituteBlockMove(cv->battlerAtk, cv->battlerDef, cv->move) && gBattleMons[cv->battlerDef].volatiles.substituteHP)
    {
        if (gBattleMons[cv->battlerDef].volatiles.substituteHP >= gBattleStruct->moveDamage[cv->battlerDef])
        {
            TestRunner_Battle_RecordSubHit(cv->battlerDef, gBattleStruct->moveDamage[cv->battlerDef], FALSE);
            gBattleMons[cv->battlerDef].volatiles.substituteHP -= gBattleStruct->moveDamage[cv->battlerDef];
        }
        else
        {
            TestRunner_Battle_RecordSubHit(cv->battlerDef, gBattleMons[cv->battlerDef].volatiles.substituteHP, TRUE);
            gBattleStruct->moveDamage[cv->battlerDef] = gBattleMons[cv->battlerDef].volatiles.substituteHP;
            gBattleMons[cv->battlerDef].volatiles.substituteHP = 0;
        }
        // check substitute fading
        if (gBattleMons[cv->battlerDef].volatiles.substituteHP == 0)
        {
            gBattleScripting.battler = cv->battlerDef;
            BattleScriptCall(BattleScript_SubstituteFade);
            return TRUE;
        }
        return FALSE;
    }
    else if (DoesDisguiseBlockMove(cv->battlerDef, cv->move) || DoesIceFaceBlockMove(cv->battlerDef, cv->move))
    {
        // Damage deals typeless 0 HP.
        gBattleStruct->moveResultFlags[cv->battlerDef] &= ~(MOVE_RESULT_HIGH_EFFECTIVENESS | MOVE_RESULT_LOW_EFFECTIVENESS);
        gBattleStruct->moveDamage[cv->battlerDef] = 0;
        if (cv->moveEffect == EFFECT_OHKO)
            gProtectStructs[cv->battlerDef].survivedOHKO = TRUE;
        if (DoesIceFaceBlockMove(cv->battlerDef, cv->move))
            gProtectStructs[cv->battlerDef].assuranceDoubled = TRUE;
    }
    else
    {
        if (gBattleStruct->moveDamage[cv->battlerDef] < 0)
        {
            // Negative damage is HP gain
            gBattleMons[cv->battlerDef].hp += -gBattleStruct->moveDamage[cv->battlerDef];
            if (gBattleMons[cv->battlerDef].hp > gBattleMons[cv->battlerDef].maxHP)
                gBattleMons[cv->battlerDef].hp = gBattleMons[cv->battlerDef].maxHP;
        }
        else
        {
            u16 hpBefore;
            u16 hpLost;

            gBideDmg[cv->battlerDef] += gBattleStruct->moveDamage[cv->battlerDef];
            gBideTarget[cv->battlerDef] = cv->battlerAtk;

            // Deal damage to the battler
            hpBefore = gBattleMons[cv->battlerDef].hp;
            if (gBattleMons[cv->battlerDef].hp > gBattleStruct->moveDamage[cv->battlerDef])
            {
                if (cv->moveEffect == EFFECT_OHKO && IsBattlerMoveResultSet(cv->battlerDef, MOVE_RESULT_FOE_HUNG_ON))
                {
                    gProtectStructs[cv->battlerDef].survivedOHKO = TRUE;
                }
                gBattleMons[cv->battlerDef].hp -= gBattleStruct->moveDamage[cv->battlerDef];
            }
            else
            {
                gBattleStruct->moveDamage[cv->battlerDef] = gBattleMons[cv->battlerDef].hp;
                gBattleMons[cv->battlerDef].hp = 0;
            }

            hpLost = hpBefore - gBattleMons[cv->battlerDef].hp;
            if (hpLost != 0)
            {
                gBattleStruct->innardsOutHpLost[cv->battlerDef] += hpLost;
                if (!IsBattlerAlly(cv->battlerAtk, cv->battlerDef))
                    gProtectStructs[cv->battlerDef].mushroomMadnessBoosted = TRUE;
            }

            gProtectStructs[cv->battlerDef].assuranceDoubled = TRUE;
            gProtectStructs[cv->battlerDef].revengeDoubled |= 1u << cv->battlerAtk;

        }
        // Send updated HP
        BtlController_EmitSetMonData(cv->battlerDef, B_COMM_TO_CONTROLLER, REQUEST_HP_BATTLE, 0, sizeof(gBattleMons[cv->battlerDef].hp), &gBattleMons[cv->battlerDef].hp);
        MarkBattlerForControllerExec(cv->battlerDef);
    }

    // Note: While physicalDmg/specialDmg below are only distinguished between for Counter/Mirror Coat,
    //       they are used in combination as general damage trackers for other purposes.
    if (GetConfig(B_COUNTER_MIRROR_COAT_ALLY) <= GEN_4 || !IsBattlerAlly(cv->battlerDef, cv->battlerAtk))
    {
        if (IsBattleMovePhysical(cv->move))
        {
            gProtectStructs[cv->battlerDef].physicalDmg = gBattleStruct->moveDamage[cv->battlerDef] + 1;
            gProtectStructs[cv->battlerDef].physicalBattlerId = cv->battlerAtk;
            gProtectStructs[cv->battlerDef].lastHitBySpecialMove = FALSE;
        }
        else // Special move
        {
            gProtectStructs[cv->battlerDef].specialDmg = gBattleStruct->moveDamage[cv->battlerDef] + 1;
            gProtectStructs[cv->battlerDef].specialBattlerId = cv->battlerAtk;
            gProtectStructs[cv->battlerDef].lastHitBySpecialMove = TRUE;
        }
    }

    GetBattlerPartyState(cv->battlerDef)->timesGotHit++;
    gSpecialStatuses[cv->battlerDef].damagedByAttack = TRUE;
    return FALSE;
}

static enum CancelerResult CancelerMoveDamageUpdate(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        cv->battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForDamage(cv->battlerAtk, cv->battlerDef))
            continue;

        if (TryMoveDamageUpdate(cv))
            return CANCELER_RESULT_RUN_SCRIPT;
    }

    cv->battlerDef = gBattlerTarget;
    gBattleStruct->eventState.moveEndBattler = 0;
    return CANCELER_RESULT_RUN_SCRIPT_AND_INCREMENT; // Update hp
}

static enum CancelerResult (*const sMoveSuccessOrderCancelers[])(struct BattleCalcValues *cv) =
{
    [CANCELER_CLEAR_FLAGS] = CancelerClearFlags,
    [CANCELER_STANCE_CHANGE_1] = CancelerStanceChangeOne,
    [CANCELER_SKY_DROP] = CancelerSkyDrop,
    [CANCELER_RECHARGE] = CancelerRecharge,
    [CANCELER_CHILLY_RECEPTION] = CancelerChillyReception,
    [CANCELER_ASLEEP_OR_FROZEN] = CancelerAsleepOrFrozen,
    [CANCELER_OBEDIENCE] = CancelerObedience,
    [CANCELER_POWER_POINTS] = CancelerPowerPoints,
    [CANCELER_TRUANT] = CancelerTruant,
    [CANCELER_FOCUS_GEN5] = CancelerFocusGen5,
    [CANCELER_FLINCH] = CancelerFlinch,
    [CANCELER_DISABLED] = CancelerDisabled,
    [CANCELER_VOLATILE_BLOCKED] = CancelerVolatileBlocked,
    [CANCELER_TAUNTED] = CancelerTaunted,
    [CANCELER_IMPRISONED] = CancelerImprisoned,
    [CANCELER_CONFUSED] = CancelerConfused,
    [CANCELER_GHOST] = CancelerGhost,
    [CANCELER_PARALYZED] = CancelerParalyzed,
    [CANCELER_INFATUATION] = CancelerInfatuation,
    [CANCELER_CHOICE_LOCK] = CancelerChoiceLock,
    [CANCELER_CALLSUBMOVE] = CancelerCallSubmove,
    [CANCELER_THAW] = CancelerThaw,
    [CANCELER_STANCE_CHANGE_2] = CancelerStanceChangeTwo,
    [CANCELER_ATTACKSTRING] = CancelerAttackstring,
    [CANCELER_PLEDGE_ATTACK] = CancelerPledgeAttack,
    [CANCELER_SET_TARGETS] = CancelerSetTargets,
    [CANCELER_PPDEDUCTION] = CancelerPPDeduction,
    [CANCELER_SKY_BATTLE] = CancelerSkyBattle,
    [CANCELER_WEATHER_PRIMAL] = CancelerWeatherPrimal,
    [CANCELER_FOCUS_PRE_GEN5] = CancelerFocusPreGen5,
    [CANCELER_BIDE] = CancelerBide,
    [CANCELER_MOVE_FAILURE] = CancelerMoveFailure,
    [CANCELER_MOVE_EFFECT_FAILURE_TARGET] = CancelerMoveEffectFailureTarget,
    [CANCELER_POWDER_STATUS] = CancelerPowderStatus,
    [CANCELER_PRIORITY_BLOCK] = CancelerPriorityBlock,
    [CANCELER_EXPLODING_DAMP] = CancelerExplodingDamp,
    // Run before delayed-attack setup so Future Sight boosts on use, once.
    [CANCELER_RIVER_ESPER] = CancelerRiverEsper,
    [CANCELER_INTERRUPTIBLE_MOVES] = CancelerInterruptibleMoves,
    [CANCELER_PROTEAN] = CancelerProtean,
    [CANCELER_MOVE_SUMMONED_FIELD] = CancelerMoveSummonedField,
    [CANCELER_CHARGING] = CancelerCharging,
    [CANCELER_UNBROKEN_STONE] = CancelerUnbrokenStone,
    [CANCELER_SNATCH] = CancelerSnatch,
    [CANCELER_EXPLOSION] = CancelerExplosion,
    [CANCELER_MOVE_SPECIFIC_MESSAGE] = CancelerMoveSpecificMessage,
    [CANCELER_NO_TARGET] = CancelerNoTarget,
    [CANCELER_TOOK_ATTACK] = CancelerTookAttack,
    [CANCELER_TARGET_FAILURE] = CancelerTargetFailure,
    [CANCELER_STAT_CHANGE_RESOLUTION] = CancelerStatChangeResolution,
    [CANCELER_MULTIHIT_MOVES] = CancelerMultihitMoves,
    [CANCELER_SUBSTITUTE] = CancelerSubstitute,
    [CANCELER_ACCURACY_CHECK] = CancelerAccuracyCheck,
    [CANCELER_SHELL_BURST] = CancelerShellBurst,
    [CANCELER_PRE_ATTACK_MOVE_EFFECT] = CancelerPreAttackMoveEffect,
    [CANCELER_STATUS_EFFECTS] = CancelerStatusEffects,
    [CANCELER_DAMAGE_CALC] = CancelerDamageCalc,
    [CANCELER_PRE_ANIM_ACTIVATIONS] = CancelerPreAnimActivations,
    [CANCELER_MOVE_ANIMATION] = CancelerMoveAnimation,
    [CANCELER_ALLY_SWITCH] = CancelerMoveAllySwitch,
    [CANCELER_EFFECTIVENESS_SOUND] = CancelerEffectivenessSound,
    [CANCELER_HIT_ANIMATION] = CancelerHitAnimation,
    [CANCELER_SKIP_FRAME] = CancelerSkipFrame,
    [CANCELER_HEALTH_BAR_UPDATE] = CancelerHealthBarUpdate,
    [CANCELER_MOVE_DAMAGE_UPDATE] = CancelerMoveDamageUpdate,
};

enum CancelerResult DoAttackCanceler(void)
{
    enum CancelerResult result = CANCELER_RESULT_SUCCESS;

    struct BattleCalcValues cv = {
        .battlerAtk = gBattlerAttacker,
        .battlerDef = gBattlerTarget,
        .move = gCurrentMove,
    };

    cv.moveEffect = GetMoveEffect(cv.move);
    cv.isStatusMove = IsBattleMoveStatus(cv.move);

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.abilities[battler] = GetBattlerAbility(battler);
        cv.holdEffects[battler] = GetBattlerHoldEffect(battler);
    }

    while (gBattleStruct->eventState.atkCanceler < CANCELER_END && result == CANCELER_RESULT_SUCCESS)
    {
        result = sMoveSuccessOrderCancelers[gBattleStruct->eventState.atkCanceler](&cv);

        if (result != CANCELER_RESULT_RUN_SCRIPT)
            gBattleStruct->eventState.atkCanceler++;
    }

    if (result == CANCELER_RESULT_FAILURE)
    {
        gBattleStruct->unableToUseMove = TRUE;
        gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;
        gBattleStruct->eventState.atkCanceler = CANCELER_END;
    }

    return result;
}

// ========
// Move End
// ========

static enum MoveEndResult MoveEndSetValues(struct BattleCalcValues *cv)
{
    enum Ability abilityAtk = cv->abilities[cv->battlerAtk];
    bool32 tongueLash = abilityAtk == ABILITY_TONGUE_LASH && gSpecialStatuses[cv->battlerAtk].postActionAbilityMove;
    if (abilityAtk == ABILITY_TONGUE_LASH && !tongueLash
     && !gBattleStruct->unableToUseMove && !gBattleMons[cv->battlerAtk].volatiles.chargeTurn
     && IsBattlerPresent(gBattlerTarget)
     && IsMoveMakingContact(cv->battlerAtk, gBattlerTarget, abilityAtk, cv->holdEffects[cv->battlerAtk], cv->move))
        QueuePostActionContactMove(cv->battlerAtk, gBattlerTarget, ABILITY_TONGUE_LASH);

    for (enum BattlerId battlerDef = B_BATTLER_0; battlerDef < gBattlersCount; battlerDef++)
    {
        gBattleStruct->accumulatedDamage += gBattleStruct->moveDamage[battlerDef];
        if (cv->abilities[battlerDef] == ABILITY_TONGUE_LASH && !tongueLash
         && gSpecialStatuses[battlerDef].damagedByAttack && gBattleStruct->moveDamage[battlerDef] > 0
         && !IsBattlerUnaffectedByMove(battlerDef)
         && IsMoveMakingContact(cv->battlerAtk, battlerDef, abilityAtk, cv->holdEffects[cv->battlerAtk], cv->move))
            QueuePostActionContactMove(battlerDef, cv->battlerAtk, ABILITY_TONGUE_LASH);
        if ((abilityAtk == ABILITY_NOBLESSE_OBLIGE || abilityAtk == ABILITY_OVERWHELM)
         && battlerDef != cv->battlerAtk && gBattleStruct->moveDamage[battlerDef] > 0
         && gSpecialStatuses[battlerDef].damagedByAttack && !gBattleStruct->battlerState[battlerDef].substituteBlocked)
        {
            if (abilityAtk == ABILITY_NOBLESSE_OBLIGE && gSpecialStatuses[battlerDef].criticalHit)
                gBattleStruct->noblesseObligeTargets |= 1u << battlerDef;
            if (abilityAtk == ABILITY_OVERWHELM && !cv->isStatusMove && !IsBattlerUnaffectedByMove(battlerDef)
             && cv->abilities[battlerDef] != ABILITY_SHIELD_DUST && cv->holdEffects[battlerDef] != HOLD_EFFECT_COVERT_CLOAK)
                gBattleStruct->overwhelmTargets |= 1u << battlerDef;
        }
        // Capture eligibility before Color Change or other move-end type changes.
        if (cv->abilities[cv->battlerAtk] == ABILITY_MAGLEV_POLARITY
         && gBattleStruct->moveDamage[battlerDef] > 0
         && IS_BATTLER_ANY_TYPE(battlerDef, TYPE_ELECTRIC, TYPE_STEEL))
            gBattleStruct->maglevPolarityDamage += gBattleStruct->moveDamage[battlerDef];
        if (cv->abilities[cv->battlerAtk] == ABILITY_RELENTLESS_CHASE
         && gBattleStruct->battlerState[battlerDef].pursuitTarget && gBattleStruct->moveDamage[battlerDef] > 0)
            gBattleStruct->relentlessChaseDamage += gBattleStruct->moveDamage[battlerDef];
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->additionalEffectsCounter = 0;
    gBattleStruct->eventState.resolution = 0;
    gBattleStruct->statChangeBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 ShouldSkipBattlerForMoveEnd(enum BattlerId battler, struct BattleCalcValues *cv)
{
    return !IsBattlerAlly(battler, cv->battlerDef)
        || IsBattlerMoveResultSet(battler, (MOVE_RESULT_NOT_TARGETED | MOVE_RESULT_IGNORE_SELF));
}

static bool32 ShouldSkipBattlerForMoveEndSubstitute(enum BattlerId battler, struct BattleCalcValues *cv)
{
    if (gBattleStruct->battlerState[battler].substituteBlocked)
        return TRUE;

    return ShouldSkipBattlerForMoveEnd(battler, cv);
}

static enum MoveEndResult MoveEndQueueDancerToxicChain(struct BattleCalcValues *cv)
{
    for (enum BattlerId effectBattler = 0; effectBattler < gBattlersCount; effectBattler++)
    {
        if (cv->abilities[cv->battlerAtk] == ABILITY_TOXIC_CHAIN
         && IsBattlerAlive(effectBattler)
         && CanBePoisoned(cv->battlerAtk, effectBattler, cv->abilities[cv->battlerAtk], cv->abilities[effectBattler])
         && IsBattlerTurnDamaged(effectBattler, EXCLUDING_SUBSTITUTES)
         && RandomWeighted(RNG_TOXIC_CHAIN, 7, 3))
            gBattleStruct->battlerState[effectBattler].toxicChainActivates = TRUE;
    }

    if (!IsDanceMove(cv->move)
     || !IsAnyTargetAffected()
     || gBattleStruct->unableToUseMove
     || gSpecialStatuses[cv->battlerAtk].dancerUsedMove
     || gBattleStruct->snatchedMoveIsUsed
     || gBattleStruct->bouncedMoveIsUsed)
    {
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_CONTINUE;
    }

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (battler == cv->battlerAtk)
            continue;

        if (GetDancerAbilitySource(battler, cv->abilities) < gBattlersCount)
            gBattleMons[battler].volatiles.activateDancer = TRUE;
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndSubstituteBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (cv->isStatusMove)
    {
        gBattleScripting.moveendState++;
        return result;
    }

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        if (!DoesSubstituteBlockMove(cv->battlerAtk, battlerDef, cv->move)
         || IsBattlerUnaffectedByMove(battlerDef)
         || ShouldSkipBattlerForMoveEnd(battlerDef, cv)
         || !IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES))
        {
            gBattleStruct->eventState.moveEndBlock = 0;
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        gBattleStruct->battlerState[battlerDef].substituteBlocked = TRUE;

        while (gBattleStruct->eventState.moveEndBlock < SUBSTITUTE_BLOCK_COUNT)
        {
            switch (gBattleStruct->eventState.moveEndBlock)
            {
            case SUBSTITUTE_BLOCK_EFFECTIVENESS_MESSAGE:
                if (ShouldPrintEffectivenessMessage(cv))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_CRIT_MESSAGE:
                if (ShouldPrintCritMessage(battlerDef))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_BYPASS_PROTECT_MESSAGE:
                if (ShouldPrintProtectMessage(battlerDef))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_DAMAGED_MESSAGE:
                gBattleScripting.battler = battlerDef;
                BattleScriptCall(BattleScript_SubstituteTookDamage);
                result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_SUBSTITUTE_DISAPPEARS:
                if (gBattleMons[battlerDef].volatiles.substituteHP == 0 && gBattleMons[battlerDef].volatiles.substitute)
                {
                    gBattleScripting.battler = battlerDef;
                    BattleScriptCall(BattleScript_SubstituteFade);
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                }
                break;
            case SUBSTITUTE_BLOCK_ADDITIONAL_EFFECTS: // Those that activate even if Substitute is active
            {
                u32 numAdditionalEffects = GetMoveAdditionalEffectCount(cv->move);
                if (numAdditionalEffects > gBattleStruct->additionalEffectsCounter
                 && !IsBattleMoveStatus(cv->move)
                 && battlerDef != cv->battlerAtk)
                {
                    const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(gCurrentMove, gBattleStruct->additionalEffectsCounter);

                    if (!CanApplyAdditionalEffect(cv->battlerAtk, battlerDef, additionalEffect)
                     || !ShouldApplyAfterHitEffects(cv->battlerAtk, battlerDef)
                     || additionalEffect->self) // handled in MoveEndAdditionalEffects
                    {
                        gBattleStruct->additionalEffectsCounter++;
                        continue;
                    }

                    TryTriggerAdditionalEffect(cv, additionalEffect, battlerDef);

                    return MOVEEND_RESULT_RUN_SCRIPT; // try to run effect script if possible and don't increment block state
                }
                gBattleStruct->additionalEffectsCounter = 0;
                gBattleScripting.moveEffect = 0;
                break;
            }
            case SUBSTITUTE_BLOCK_ITEM_EFFECT_TARGET:
                if (ItemBattleEffects(battlerDef, cv->battlerAtk, cv->holdEffects[battlerDef], IsOnTargetHitActivation))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_PROTECT_LIKE_EFFECT:
                if (ShouldApplyProtectLikeEffects(battlerDef, cv))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                break;
            case SUBSTITUTE_BLOCK_COUNT:
                break;
            }

            gBattleStruct->eventState.moveEndBlock++;

            if (result != MOVEEND_RESULT_CONTINUE)
                return result;
        }

        if (gBattleMons[battlerDef].volatiles.substituteHP == 0)
            gBattleMons[battlerDef].volatiles.substitute = FALSE;

        gBattleStruct->eventState.moveEndBattler++;
        gBattleStruct->eventState.moveEndBlock = 0;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndMoveHeavyRecoil(struct BattleCalcValues *cv)
{
    if (gBattleStruct->unableToUseMove)
    {
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_CONTINUE;
    }

    if ((IsExplosionMove(cv->move) || cv->moveEffect == EFFECT_MEMENTO)
     && !IsBattlerAlive(cv->battlerAtk)
     && !gBattleStruct->battlerState[cv->battlerAtk].notOnField)
    {
        gBattleStruct->passiveHpUpdate[cv->battlerAtk] = 0;
        BattleScriptCall(BattleScript_FaintAttackerForExplosion);
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_RUN_SCRIPT;
    }

    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    switch (cv->moveEffect)
    {
    case EFFECT_MAX_HP_50_RECOIL:
        if (IsBattlerAlive(cv->battlerAtk)
         && !(cv->abilities[cv->battlerAtk] == ABILITY_RELENTLESS_CHASE && gBattleStruct->relentlessChaseDamage > 0)
         && !gSpecialStatuses[cv->battlerAtk].mindBlownRecoil
         && !IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD)
         && !IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_VAMPIRISM))
        {
            s32 recoil = (GetBattlerMaxHP(cv->battlerAtk) + 1) / 2; // Half of Max HP Rounded UP
            SetPassiveDamageAmount(cv->battlerAtk, recoil);
            gSpecialStatuses[cv->battlerAtk].mindBlownRecoil = TRUE;
            TryUpdateEvolutionTracker(IF_RECOIL_DAMAGE_GE, gBattleStruct->passiveHpUpdate[cv->battlerAtk], MOVE_NONE);
            BattleScriptCall(BattleScript_MaxHp50Recoil);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    default:
        break;
    }

    gBattleScripting.moveendState++;
    return result;
}

static bool32 ShouldPrintEffectivenessMessageForFlag(enum BattlerId battler1, enum BattlerId battler2, u32 moveResultFlag)
{
    if (gBattleStruct->moveResultFlags[battler1] & moveResultFlag && !gSpecialStatuses[battler1].resultMessagePrinted)
    {
        if (IsDoubleSpreadMove())
        {
            if (battler2 != battler1 && gBattleStruct->moveResultFlags[battler2] & moveResultFlag)
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_TWO_TARGETS;
                gSpecialStatuses[battler2].resultMessagePrinted = TRUE;
            }
            else
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ONE_OF_TWO_TARGETS;
            }
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ONE_TARGET;
        }
    }
    else if (gBattleStruct->moveResultFlags[battler2] & moveResultFlag && !gSpecialStatuses[battler2].resultMessagePrinted)
    {
        battler1 = battler2;
        if (IsDoubleSpreadMove())
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ONE_OF_TWO_TARGETS;
        else
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ONE_TARGET;
    }
    else
    {
        return FALSE;
    }

    gSpecialStatuses[battler1].resultMessagePrinted = TRUE;
    gBattleScripting.battler = battler1;

    return TRUE;
}

static bool32 ShouldPrintEffectivenessMessage(struct BattleCalcValues *cv)
{
    enum BattlerId battler1 = cv->battlerDef;
    enum BattlerId battler2 = GetPartnerBattler(battler1);
    bool32 anyValidBattler = FALSE;

    if (ShouldSkipBattlerForMoveEnd(battler1, cv)
     || !IsBattlerTurnDamaged(battler1, INCLUDING_SUBSTITUTES)
     || gSpecialStatuses[battler1].resultMessagePrinted)
        battler1 = battler2;
    else
        anyValidBattler = TRUE;

    if (ShouldSkipBattlerForMoveEnd(battler2, cv)
     || !IsBattlerTurnDamaged(battler2, INCLUDING_SUBSTITUTES)
     || gSpecialStatuses[battler2].resultMessagePrinted)
        battler2 = battler1;
    else
        anyValidBattler = TRUE;

    // If it's a multihit move not blocked by Substitute and not Dragon Darts
    if (gMultiHitCounter != 0
     && GetBattlerMoveTargetType(cv->battlerAtk, cv->move) != TARGET_SMART
     && !(gBattleStruct->battlerState[battler1].substituteBlocked || gBattleStruct->battlerState[battler2].substituteBlocked))
        return FALSE;

    // No battlers on this side should print effectiveness messages
    if (!anyValidBattler)
        return FALSE;

    // No effect -> Extremely effective -> super effective -> not very effective -> mostly ineffective
    if (ShouldPrintEffectivenessMessageForFlag(battler1, battler2, MOVE_RESULT_DOESNT_AFFECT_FOE))
    {
        if (gSpecialStatuses[battler1].resultMessagePrinted)
            TryInitializeTrainerSlideFirstIneffectiveMove(battler1, cv->battlerAtk);
        if (battler2 != battler1 && gSpecialStatuses[battler2].resultMessagePrinted)
            TryInitializeTrainerSlideFirstIneffectiveMove(battler2, cv->battlerAtk);
        BattleScriptCall(BattleScript_PrintNoEffectMessage);
        return TRUE;
    }
    else if (ShouldPrintEffectivenessMessageForFlag(battler1, battler2, MOVE_RESULT_EXTREMELY_EFFECTIVE))
    {
        if (gSpecialStatuses[battler1].resultMessagePrinted)
            TryInitializeTrainerSlideLandsFirstSuperEffectiveHit(battler1, cv->battlerAtk);
        if (battler2 != battler1 && gSpecialStatuses[battler2].resultMessagePrinted)
            TryInitializeTrainerSlideLandsFirstSuperEffectiveHit(battler2, cv->battlerAtk);

        BattleScriptCall(BattleScript_PrintExtremelyEffectiveMessage);
        return TRUE;
    }
    else if (ShouldPrintEffectivenessMessageForFlag(battler1, battler2, MOVE_RESULT_SUPER_EFFECTIVE))
    {
        if (gSpecialStatuses[battler1].resultMessagePrinted)
            TryInitializeTrainerSlideLandsFirstSuperEffectiveHit(battler1, cv->battlerAtk);
        if (battler2 != battler1 && gSpecialStatuses[battler2].resultMessagePrinted)
            TryInitializeTrainerSlideLandsFirstSuperEffectiveHit(battler2, cv->battlerAtk);

        BattleScriptCall(BattleScript_PrintSuperEffectiveMessage);
        return TRUE;
    }
    else if (ShouldPrintEffectivenessMessageForFlag(battler1, battler2, MOVE_RESULT_NOT_VERY_EFFECTIVE))
    {
        BattleScriptCall(BattleScript_PrintNotVeryEffectiveMessage);
        return TRUE;
    }
    else if (ShouldPrintEffectivenessMessageForFlag(battler1, battler2, MOVE_RESULT_MOSTLY_INEFFECTIVE))
    {
        BattleScriptCall(BattleScript_PrintMostlyIneffectiveMessage);
        return TRUE;
    }
    return FALSE;
}

static enum MoveEndResult MoveEndEffectivenessMessage(struct BattleCalcValues *cv)
{
    if (ShouldPrintEffectivenessMessage(cv))
        return MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 GetProtectBypassMethod(enum BattlerId battlerDef)
{
    if (IsBattlerUnaffectedByMove(battlerDef))
        return PROTECT_BYPASS_NONE;
    if (MoveIgnoresProtect(gCurrentMove))
        return PROTECT_BYPASS_MOVE_IGNORES;
    if (gSpecialStatuses[battlerDef].breaksThroughProtectFully)
        return PROTECT_BYPASS_ABILITY_IGNORES;
    return PROTECT_BYPASS_NONE;
    if (GetProtectType(gProtectStructs[battlerDef].protected) == PROTECT_TYPE_SINGLE)
        return PROTECT_BYPASS_OTHER;
    return PROTECT_BYPASS_MOVE_IGNORES;
}

static bool32 ShouldPrintCritMessage(enum BattlerId battler)
{
    if (!gSpecialStatuses[battler].criticalHit || gSpecialStatuses[battler].critMessagePrinted)
        return FALSE;

    if (IsDoubleSpreadMove())
        BattleScriptCall(BattleScript_CriticalHitMessageMultiTarget);
    else
        BattleScriptCall(BattleScript_CriticalHitMessage);

    TryInitializeTrainerSlideEnemyLandsFirstCriticalHit(battler);
    TryInitializeTrainerSlidePlayerLandsFirstCriticalHit(battler);

    gSpecialStatuses[battler].critMessagePrinted = TRUE;
    gBattleScripting.battler = battler;
    return TRUE;
}

static bool32 ShouldPrintProtectMessage(enum BattlerId battler)
{
    if (gSpecialStatuses[battler].protectMessagePrinted)
        return FALSE;

    switch (GetProtectBypassMethod(battler))
    {
    case PROTECT_BYPASS_ABILITY_IGNORES:
        BattleScriptCall(BattleScript_UnseenFist);
        gBattleScripting.battler = battler;
        gSpecialStatuses[battler].protectMessagePrinted = TRUE;
        return TRUE;
        break;
    case PROTECT_BYPASS_OTHER:
        BattleScriptCall(BattleScript_CouldntFullyProtect);
        gBattleScripting.battler = battler;
        gSpecialStatuses[battler].protectMessagePrinted = TRUE;
        return TRUE;
        break;
    default:
        break;
    }
    return FALSE;
}

static enum MoveEndResult MoveEndCritProtectMessage(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        if (ShouldSkipBattlerForMoveEndSubstitute(battlerDef, cv))
        {
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        if (ShouldPrintCritMessage(battlerDef))
            return MOVEEND_RESULT_RUN_SCRIPT;

        if (ShouldPrintProtectMessage(battlerDef))
            return MOVEEND_RESULT_RUN_SCRIPT;

        gBattleStruct->eventState.moveEndBattler++;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 ShouldPrintEndureHitMessage(enum BattlerId battler)
{
    u32 *moveResultFlags = &(gBattleStruct->moveResultFlags[battler]);

    if (*moveResultFlags & MOVE_RESULT_STURDIED)
    {
        gBattleScripting.battler = gBattlerAbility = battler;
        *moveResultFlags &= ~(MOVE_RESULT_STURDIED | MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
        BattleScriptCall(BattleScript_SturdiedMsg);
        return TRUE;
    }
    else if (*moveResultFlags & MOVE_RESULT_FOE_ENDURED)
    {
        gBattleScripting.battler = battler;
        *moveResultFlags &= ~(MOVE_RESULT_FOE_ENDURED | MOVE_RESULT_FOE_HUNG_ON);
        BattleScriptCall(BattleScript_EnduredMsg);
        return TRUE;
    }
    else if (*moveResultFlags & MOVE_RESULT_FOE_HUNG_ON)
    {
        gBattleScripting.battler = gPotentialItemEffectBattler = battler;
        gLastUsedItem = gBattleMons[battler].item;
        *moveResultFlags &= ~MOVE_RESULT_FOE_HUNG_ON;
        BattleScriptCall(BattleScript_HangedOnMsg);
        return TRUE;
    }
    else ;
    return FALSE;
}

static bool32 TryDecapitator(struct BattleCalcValues *cv, enum BattlerId battlerDef)
{
    if (cv->abilities[cv->battlerAtk] != ABILITY_DECAPITATOR || !IsSlicingMove(cv->move)
     || gBattleStruct->moveDamage[battlerDef] <= 0 || !IsBattlerAlive(battlerDef)
     || IsBattlerUnaffectedByMove(battlerDef)
     || !IsBelowDecapitatorThreshold(GetBattlerHP(battlerDef), GetBattlerMaxHP(battlerDef)))
        return FALSE;

    gEffectBattler = battlerDef;
    gBattlerAbility = gBattleScripting.battler = cv->battlerAtk;
    gLastUsedAbility = ABILITY_DECAPITATOR;
    SetPassiveDamageAmount(battlerDef, GetBattlerHP(battlerDef));
    BattleScriptCall(BattleScript_DecapitatorActivates);
    return TRUE;
}

static enum MoveEndResult MoveEndEndureDamageMessage(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        if (ShouldSkipBattlerForMoveEndSubstitute(battlerDef, cv))
        {
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        // Finish any survival message for this hit before checking its remaining HP.
        if (ShouldPrintEndureHitMessage(battlerDef))
            return MOVEEND_RESULT_RUN_SCRIPT;
        gBattleStruct->eventState.moveEndBattler++;
        if (TryDecapitator(cv, battlerDef))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 ShouldApplyProtectLikeEffects(enum BattlerId battlerDef, struct BattleCalcValues *cv)
{
    if (gBattleMons[cv->battlerAtk].volatiles.chargeTurn
     || !IsBattlerAlive(cv->battlerAtk)
     || gBattleStruct->unableToUseMove)
        return FALSE;

    if (CanBattlerAvoidContactEffects(cv->battlerAtk, battlerDef, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->move))
        return FALSE;

    if (CanApexAbilityBypassProtect(cv->battlerAtk, battlerDef, cv->abilities[cv->battlerAtk], cv->move))
        return FALSE;

    if (gSpecialStatuses[battlerDef].breaksThroughProtectFully && GetConfig(B_UNSEEN_FIST_PIERCING_DRILL) <= GEN_9)
        return FALSE;

    if (IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES) && !gSpecialStatuses[battlerDef].breaksThroughProtectFully)
        return FALSE;

    enum ProtectMethod method = gProtectStructs[battlerDef].protected;
    switch (method)
    {
    case PROTECT_SPIKY_SHIELD:
        if (!IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD))
        {
            SetPassiveDamageAmount(cv->battlerAtk, GetBattlerMaxHP(cv->battlerAtk) / 8);
            PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_SPIKY_SHIELD);
            BattleScriptCall(BattleScript_SpikyShieldEffect);
            return TRUE;
        }
        break;
    case PROTECT_KINGS_SHIELD:
    {
        s32 stage = (B_KINGS_SHIELD_LOWER_ATK >= GEN_8) ? -1 : -2;
        gEffectBattler = cv->battlerAtk;
        SetStatChange(gEffectBattler, STAT_ATK, stage);
        BattleScriptCall(BattleScript_KingsShieldEffect);
        return TRUE;
        break;
    }
    case PROTECT_BANEFUL_BUNKER:
        if (CanBePoisoned(battlerDef, cv->battlerAtk, cv->abilities[battlerDef], cv->abilities[cv->battlerAtk]))
        {
            gBattleScripting.moveEffect = MOVE_EFFECT_POISON;
            BattleScriptCall(BattleScript_BanefulBunkerEffect);
            return TRUE;
        }
        break;
    case PROTECT_BURNING_BULWARK:
        if (CanBeBurned(battlerDef, cv->battlerAtk, cv->abilities[cv->battlerAtk]))
        {
            gBattleScripting.moveEffect = MOVE_EFFECT_BURN;
            BattleScriptCall(BattleScript_BanefulBunkerEffect);
            return TRUE;
        }
        break;
    case PROTECT_OBSTRUCT:
        gEffectBattler = cv->battlerAtk;
        SetStatChange(gEffectBattler, STAT_DEF, -2);
        BattleScriptCall(BattleScript_KingsShieldEffect);
        return TRUE;
        break;
    case PROTECT_SILK_TRAP:
        gEffectBattler = cv->battlerAtk;
        SetStatChange(gEffectBattler, STAT_SPEED, -1);
        BattleScriptCall(BattleScript_KingsShieldEffect);
        return TRUE;
        break;
    default:
        break;
    }
    return FALSE;
}

static enum MoveEndResult MoveEndProtectLikeEffect(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForMoveEndSubstitute(battlerDef, cv))
            continue;

        if (ShouldApplyProtectLikeEffects(battlerDef, cv))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndMimeBox(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler++);
        if (cv->abilities[battlerDef] != ABILITY_MIME_BOX
         || !IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_PROTECTED)
         || ShouldSkipBattlerForMoveEnd(battlerDef, cv)
         || gSpecialStatuses[battlerDef].mimeBoxActivated
         || !IsBattlerAlive(cv->battlerAtk)
         || CanBattlerAvoidContactEffects(cv->battlerAtk, battlerDef, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->move)
         || IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD))
            continue;

        gSpecialStatuses[battlerDef].mimeBoxActivated = TRUE;
        gBattlerAbility = gBattleScripting.battler = battlerDef;
        gLastUsedAbility = ABILITY_MIME_BOX;
        SetPassiveDamageAmount(cv->battlerAtk, GetBattlerMaxHP(cv->battlerAtk) / 6);
        BattleScriptCall(BattleScript_RoughSkinActivates);
        return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndFlingItemRemoval(struct BattleCalcValues *cv)
{
    gBattleScripting.moveendState++;

    if (gBattleStruct->unableToUseMove && gBattleStruct->flungItem != FLUNG_ITEM_REMOVE)
        return MOVEEND_RESULT_CONTINUE;

    if (cv->moveEffect != EFFECT_FLING || gBattleMons[cv->battlerAtk].item == ITEM_NONE)
        return MOVEEND_RESULT_CONTINUE;

    gLastUsedItem = gBattleStruct->flingItem = gBattleMons[cv->battlerAtk].item;
    gBattleStruct->flungItem = FLUNG_ITEM_REMOVE;
    gBattleScripting.battler = cv->battlerAtk;
    BattleScriptCall(BattleScript_RemoveItem);

    gBattleStruct->blunderPolicy = FALSE;
    return MOVEEND_RESULT_RUN_SCRIPT;
}

static bool32 ShouldApplyAfterHitEffects(enum BattlerId battlerAtk, enum BattlerId effectBattler)
{
    if (gBattleStruct->unableToUseMove)
        return FALSE;

    if (battlerAtk == effectBattler)
        return IsAnyTargetAffected();

    return !IsBattlerUnaffectedByMove(effectBattler);
}

static bool32 CanApplyAdditionalEffect(enum BattlerId battlerAtk, enum BattlerId effectBattler, const struct AdditionalEffect *additionalEffect)
{
    if (additionalEffect->preAttackEffect)
        return FALSE;

    if (additionalEffect->pledgeCombo && gBattleStruct->pledgeState != PLEDGE_COMBO_ATTACK)
        return FALSE;

    // If Toxic Chain will activate it blocks all other non volatile effects
    if (gBattleStruct->battlerState[effectBattler].toxicChainActivates && additionalEffect->moveEffect <= MOVE_EFFECT_FROSTBITE)
        return FALSE;

    // Certain move effects only apply if the target raised stats this turn (e.g. Burning Jealousy)
    if (additionalEffect->onlyIfTargetRaisedStats && !gProtectStructs[effectBattler].statRaised)
        return FALSE;

    // Don't apply chargeTurnOnly effects here
    if (additionalEffect->onChargeTurnOnly || gBattleMons[battlerAtk].volatiles.chargeTurn)
        return FALSE;

    return TRUE;
}

static void TryTriggerAdditionalEffect(struct BattleCalcValues *cv, const struct AdditionalEffect *additionalEffect, enum BattlerId effectBattler)
{
    struct SetEffect se = {0};

    bool32 percentChance = CalcSecondaryEffectChance(cv->battlerAtk, cv->abilities[cv->battlerAtk], additionalEffect, cv->move);
    bool32 isAppliedRandomly = RandomPercentage(RNG_SECONDARY_EFFECT + gBattleStruct->additionalEffectsCounter, percentChance);
    bool32 isPrimary = percentChance == 0;
    bool32 failedToTrigger = TRUE;

    // By default, onSide effects apply if they applied on the original target/user
    if (isPrimary || isAppliedRandomly || gBattleStruct->setEffectOnAlly)
    {
        if (!additionalEffect->self && !cv->isStatusMove)
            cv->battlerDef = effectBattler; // For SetMoveEffect, will be restored to previous value when run again
        else if (cv->isStatusMove)
            cv->battlerDef = gBattlerTarget;
        se.additionalEffect = additionalEffect;
        se.moveEffect = additionalEffect->moveEffect;
        se.script = gBattlescriptCurrInstr;
        se.effectBattler = effectBattler;
        se.primary = isPrimary;
        se.certain = cv->isStatusMove ? TRUE : percentChance >= 100;
        failedToTrigger = FALSE;

        if (gBattleStruct->setEffectOnAlly)
            se.effectBattler = GetPartnerBattler(se.effectBattler);
        else if (additionalEffect->onSide)
            gBattleStruct->setEffectOnAlly = TRUE;

        SetMoveEffect(cv, &se);
    }

    // If applying an onSide effect on partner or if unsuccessful, jump to next effect
    if (se.effectBattler != effectBattler || !additionalEffect->onSide || failedToTrigger)
    {
        gBattleStruct->additionalEffectsCounter++;
        gBattleStruct->setEffectOnAlly = FALSE;
    }
}

static bool32 IsDamageMoveSelfStatChange(bool32 isSelf, enum MoveEffect moveEffect)
{
    if (!isSelf)
        return FALSE;
    return moveEffect == MOVE_EFFECT_STAT_MINUS
        || moveEffect == MOVE_EFFECT_STAT_PLUS
        || moveEffect == MOVE_EFFECT_TERA_BLAST
        || moveEffect == MOVE_EFFECT_ORDER_UP;
}

static enum MoveEndResult MoveEndAdditionalEffects(struct BattleCalcValues *cv)
{
    static const struct AdditionalEffect gnashingTeeth = {
        .moveEffect = MOVE_EFFECT_STAT_MINUS,
        .defense = 1,
        .chance = 100,
    };
    static const struct AdditionalEffect conductionZone = {
        .moveEffect = MOVE_EFFECT_PARALYSIS,
        .chance = 100,
    };
    u32 originalEffects = GetMoveAdditionalEffectCount(cv->move);
    bool32 conducts = cv->abilities[cv->battlerAtk] == ABILITY_CONDUCTION_ZONE
        && IsConductionZoneMove(cv->move, GetBattleMoveType(cv->move), gFieldTimers.terrain);
    u32 numAdditionalEffects = originalEffects
        + (cv->abilities[cv->battlerAtk] == ABILITY_GNASHING_TEETH && IsBitingMove(cv->move) && !cv->isStatusMove)
        + conducts;

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId effectBattler = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        if (effectBattler == cv->battlerAtk && !IsBattlerAlly(cv->battlerDef, effectBattler))
        {
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        bool32 tryAdditionalEffect;
        if (cv->isStatusMove)
            tryAdditionalEffect = IsBattlerAlly(effectBattler, cv->battlerDef)
                && (IsBattlerMoveResultSet(effectBattler, MOVE_RESULT_VALID_STATUS_TARGET)
                 // Stat-change moves resolve before the generic status-effect checker.
                 || (conducts && IsBattlerMoveResultSet(effectBattler, MOVE_RESULT_STAT_CHANGED)));
        else
            tryAdditionalEffect = !ShouldSkipBattlerForMoveEndSubstitute(effectBattler, cv) || effectBattler == cv->battlerAtk;

        if (numAdditionalEffects > gBattleStruct->additionalEffectsCounter && tryAdditionalEffect)
        {
            bool32 conductionEffect = conducts && gBattleStruct->additionalEffectsCounter == originalEffects;
            const struct AdditionalEffect *additionalEffect = gBattleStruct->additionalEffectsCounter == originalEffects
                ? (conductionEffect ? &conductionZone : &gnashingTeeth)
                : GetMoveAdditionalEffectById(gCurrentMove, gBattleStruct->additionalEffectsCounter);

            if (conductionEffect)
            {
                enum BattlerId savedAbilityBattler = gBattlerAbility;
                bool32 canParalyze = effectBattler != cv->battlerAtk
                    && IsBattlerAlive(effectBattler)
                    && !gBattleMons[cv->battlerAtk].volatiles.chargeTurn
                    && cv->abilities[effectBattler] != ABILITY_SHIELD_DUST
                    && cv->holdEffects[effectBattler] != HOLD_EFFECT_COVERT_CLOAK
                    && !DoesSubstituteBlockMove(cv->battlerAtk, effectBattler, cv->move)
                    && CanSetNonVolatileStatus(cv->battlerAtk, effectBattler, ABILITY_CONDUCTION_ZONE,
                        cv->abilities[effectBattler], MOVE_EFFECT_PARALYSIS, CHECK_TRIGGER);
                gBattlerAbility = savedAbilityBattler;
                if (!canParalyze)
                {
                    gBattleStruct->additionalEffectsCounter++;
                    continue;
                }
            }

            switch (cv->isStatusMove)
            {
            case FALSE:
                if (!CanApplyAdditionalEffect(cv->battlerAtk, effectBattler, additionalEffect)
                 || !ShouldApplyAfterHitEffects(cv->battlerAtk, effectBattler)
                 || IsDamageMoveSelfStatChange(additionalEffect->self, additionalEffect->moveEffect)
                 || (effectBattler == cv->battlerAtk) != additionalEffect->self)
                {
                    gBattleStruct->additionalEffectsCounter++;
                    continue;
                }
                break;
            case TRUE:
                if (!IsBattlerMoveResultSet(effectBattler, MOVE_RESULT_VALID_STATUS_TARGET)
                 && !(conductionEffect && IsBattlerMoveResultSet(effectBattler, MOVE_RESULT_STAT_CHANGED)))
                {
                    gBattleStruct->additionalEffectsCounter++;
                    continue;
                }
                break;
            }

            if (conductionEffect)
                RecordAbilityBattle(cv->battlerAtk, ABILITY_CONDUCTION_ZONE);
            TryTriggerAdditionalEffect(cv, additionalEffect, effectBattler);
            if (cv->isStatusMove && gBattleStruct->statusMoveFailed)
            {
                gBattleStruct->statusMoveFailed = FALSE;
                gBattleStruct->additionalEffectsCounter = 0;
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
            }

            return MOVEEND_RESULT_RUN_SCRIPT; // try to run effect script if possible
        }
        gBattleStruct->eventState.moveEndBattler++;
        gBattleStruct->additionalEffectsCounter = 0;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleStruct->additionalEffectsCounter = 0;
    gBattleScripting.moveEffect = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndAdditionalEffectsChangeStatsAttacker(struct BattleCalcValues *cv)
{
    u32 numAdditionalEffects = GetMoveAdditionalEffectCount(cv->move);

    if (IsBattleMoveStatus(cv->move))
    {
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_CONTINUE;
    }

    if (numAdditionalEffects > gBattleStruct->additionalEffectsCounter)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(gCurrentMove, gBattleStruct->additionalEffectsCounter);

        // Various checks for if this move effect can be applied this turn
        if (!CanApplyAdditionalEffect(cv->battlerAtk, cv->battlerAtk, additionalEffect)
         || !ShouldApplyAfterHitEffects(cv->battlerAtk, cv->battlerAtk)
         || !IsDamageMoveSelfStatChange(additionalEffect->self, additionalEffect->moveEffect))
        {
            gBattleStruct->additionalEffectsCounter++;
            return MOVEEND_RESULT_BREAK; // recall the same state
        }

        TryTriggerAdditionalEffect(cv, additionalEffect, cv->battlerAtk);

        return MOVEEND_RESULT_RUN_SCRIPT; // try to run effect script if possible
    }

    gBattleStruct->additionalEffectsCounter = 0;
    gBattleScripting.moveEffect = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndAbsorb(struct BattleCalcValues *cv)
{
    if (gBattleStruct->unableToUseMove)
    {
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_CONTINUE;
    }

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForMoveEnd(battlerDef, cv))
            continue;

        switch (cv->moveEffect)
        {
        case EFFECT_STRENGTH_SAP:
            if (gBattleStruct->passiveHpUpdate[cv->battlerAtk] > 0
             && !IsBattlerUnaffectedByMove(battlerDef))
            {
                s32 healAmount = gBattleStruct->passiveHpUpdate[cv->battlerAtk];
                healAmount = GetDrainedBigRootHp(cv->battlerAtk, healAmount);
                gEffectBattler = cv->battlerAtk;

                if (cv->abilities[battlerDef] == ABILITY_LIQUID_OOZE)
                {
                    SetPassiveDamageAmount(cv->battlerAtk, healAmount);
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABSORB_OOZE;
                    gBattlerAbility = gBattleScripting.battler = battlerDef;
                    BattleScriptCall(BattleScript_EffectAbsorbLiquidOoze);
                    return MOVEEND_RESULT_RUN_SCRIPT;
                }
                else if (!IsBattlerAtMaxHp(cv->battlerAtk) || GetConfig(B_ABSORB_MESSAGE) < GEN_5)
                {
                    SetHealAmount(cv->battlerAtk, healAmount);
                    gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_ABSORB;
                    gEffectBattler = cv->battlerAtk;
                    BattleScriptCall(BattleScript_EffectAbsorb);
                    return MOVEEND_RESULT_RUN_SCRIPT;
                }
            }
            break;
        default:
            break;
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndApexTargetEffects(struct BattleCalcValues *cv)
{
    enum Ability ability = cv->abilities[cv->battlerAtk];
    bool32 canBleed = ability == ABILITY_BIG_MEATY_CLAWS && !cv->isStatusMove && IsSlicingMove(cv->move);
    bool32 canHypnotize = ability == ABILITY_HYPNOTIST && cv->isStatusMove && !gBattleStruct->statusMoveFailed;
    bool32 canScorch = !cv->isStatusMove && IsScorchingBlastFireMove(ability, GetBattleMoveType(cv->move));
    bool32 canVoltage = !cv->isStatusMove && ability == ABILITY_HIGH_VOLTAGE && GetBattleMoveType(cv->move) == TYPE_ELECTRIC;
    bool32 canGrip = ability == ABILITY_SPIKY_GRIP && !cv->isStatusMove && IsBattlerAlive(cv->battlerAtk);
    enum Type apexMoveType = TYPE_NONE;
    enum MoveEffect apexStatus = MOVE_EFFECT_NONE;
    if (HasApexMoveStatus(ability))
    {
        apexMoveType = GetBattleMoveType(cv->move);
        apexStatus = GetApexMoveStatus(ability, apexMoveType);
    }
    bool32 statusMoveBurn = ability == ABILITY_BLACKENED_WINGS && cv->isStatusMove && !gBattleStruct->statusMoveFailed;
    if (cv->isStatusMove && !statusMoveBurn)
        apexStatus = MOVE_EFFECT_NONE;
    bool32 queuedDrops = FALSE;

    if (!gBattleStruct->unableToUseMove && (canBleed || canHypnotize || canScorch || canGrip || canVoltage || apexStatus != MOVE_EFFECT_NONE))
    {
        while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
        {
            enum BattlerId target = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler++);
            if (target == cv->battlerAtk || !IsBattlerAlive(target)
             || IsBattlerMoveResultSet(target, MOVE_RESULT_INVALID_TARGET | MOVE_RESULT_IGNORE_SELF)
             || gBattleStruct->battlerState[target].substituteBlocked)
                continue;

            if (canGrip)
            {
                if (IsBattlerAlly(cv->battlerAtk, target) || gBattleMons[target].volatiles.spikyGrip
                 || gBattleStruct->moveDamage[target] <= 0 || !IsBattlerTurnDamaged(target, EXCLUDING_SUBSTITUTES)
                 || !IsMoveMakingContact(cv->battlerAtk, target, ability, cv->holdEffects[cv->battlerAtk], cv->move))
                    continue;

                gBattleMons[target].volatiles.spikyGrip = cv->battlerAtk + 1;
                gEffectBattler = target;
                gBattlerAbility = cv->battlerAtk;
                gLastUsedAbility = ability;
                BattleScriptCall(BattleScript_SpikyGripActivates);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            else if (canVoltage)
            {
                if (gBattleStruct->moveDamage[target] <= 0 || !IsBattlerTurnDamaged(target, EXCLUDING_SUBSTITUTES))
                    continue;
                enum MoveEffect status = GetHighVoltageStatus(cv->battlerAtk, target, cv->abilities[target], TRUE);
                if (status == MOVE_EFFECT_NONE)
                    continue;
                gBattleScripting.moveEffect = status;
                gEffectBattler = target;
                gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
                gLastUsedAbility = ability;
                BattleScriptCall(BattleScript_AbilityStatusEffect);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            else if (apexStatus != MOVE_EFFECT_NONE)
            {
                if (statusMoveBurn)
                {
                    if (!IsHypnotistTarget(cv->battlerAtk, target, cv->move)
                     || IsSubstituteProtected(cv->battlerAtk, target, ability, cv->move))
                        continue;
                }
                else if (gBattleStruct->moveDamage[target] <= 0 || !IsBattlerTurnDamaged(target, EXCLUDING_SUBSTITUTES)
                      || cv->abilities[target] == ABILITY_SHIELD_DUST || cv->holdEffects[target] == HOLD_EFFECT_COVERT_CLOAK)
                {
                    continue;
                }
                if (!CanSetNonVolatileStatus(cv->battlerAtk, target, ability, cv->abilities[target], apexStatus, CHECK_TRIGGER))
                    continue;
                gBattleScripting.moveEffect = apexStatus;
                gEffectBattler = target;
                gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
                gLastUsedAbility = ability;
                BattleScriptCall(BattleScript_AbilityStatusEffect);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            else if (canScorch)
            {
                if (gBattleStruct->moveDamage[target] <= 0 || (GetEffectiveBattlerStatus(target, cv->abilities[target]) & STATUS1_BURN))
                    continue;

                gEffectBattler = target;
                gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
                gLastUsedAbility = ability;
                BattleScriptCall(BattleScript_ScorchingBlastBurn);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            else if (canBleed)
            {
                if (!IsBattlerTurnDamaged(target, EXCLUDING_SUBSTITUTES) || gBattleStruct->moveDamage[target] <= 0
                 || IS_BATTLER_OF_TYPE(target, TYPE_GHOST))
                    continue;

                bool32 newBleed = gBattleMons[target].volatiles.bleedTurns == 0;
                gBattleMons[target].volatiles.bleedTurns = 3;
                if (newBleed)
                {
                    gEffectBattler = target;
                    gBattlerAbility = cv->battlerAtk;
                    gLastUsedAbility = ability;
                    RecordAbilityBattle(cv->battlerAtk, ability);
                    BattleScriptCall(BattleScript_BigMeatyClawsBleed);
                    return MOVEEND_RESULT_RUN_SCRIPT;
                }
            }
            else if (canHypnotize && IsHypnotistTarget(cv->battlerAtk, target, cv->move)
                  && !IsBattlerMoveResultSet(target, MOVE_RESULT_STAT_CHANGE_PREVENTED)
                  && !IsSubstituteProtected(cv->battlerAtk, target, ability, cv->move))
            {
                SetStatChange(target, STAT_DEF, -1);
                SetStatChange(target, STAT_SPDEF, -1);
                queuedDrops = TRUE;
            }
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    if (queuedDrops)
    {
        gBattlerAbility = cv->battlerAtk;
        gLastUsedAbility = ability;
        RecordAbilityBattle(cv->battlerAtk, ability);
        BattleScriptCall(BattleScript_AbilityTargetStatChanges);
        return MOVEEND_RESULT_RUN_SCRIPT;
    }
    if (ability == ABILITY_BLACKENED_WINGS && apexMoveType == TYPE_FLYING && !cv->isStatusMove
     && !gBattleStruct->unableToUseMove && (gBattleWeather & B_WEATHER_ANY || gFieldTimers.terrain != B_TERRAIN_NONE)
     && IsAnyTargetTurnDamaged(cv->battlerAtk, INCLUDING_SUBSTITUTES))
    {
        gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
        BattleScriptCall(BattleScript_ClearWeatherAndTerrain);
        return MOVEEND_RESULT_RUN_SCRIPT;
    }
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndSappingVines(struct BattleCalcValues *cv)
{
    gBattleScripting.moveendState++;
    enum BattlerId target = gBattlerTarget;
    if (cv->move != MOVE_SAPPING_VINES || !IsBattlerAlive(cv->battlerAtk)
     || !IsBattlerTurnDamaged(target, EXCLUDING_SUBSTITUTES)
     || gBattleStruct->moveDamage[target] <= 0)
        return MOVEEND_RESULT_CONTINUE;

    enum Stat stat = RandomUniform(RNG_SAPPING_VINES, STAT_ATK, STAT_SPDEF);
    SetStatChange(cv->battlerAtk, stat, 1);
    if (IsBattlerAlive(target) && !IsMoveEffectBlockedByTarget(cv->abilities[target])
     && cv->holdEffects[target] != HOLD_EFFECT_COVERT_CLOAK)
        SetStatChange(target, stat, -1);
    BattleScriptCall(BattleScript_MoveEffectStatChange);
    return MOVEEND_RESULT_RUN_SCRIPT;
}

static enum MoveEndResult MoveEndStatusImmunityAbilities(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForMoveEnd(battlerDef, cv))
            continue;

        if (AbilityBattleEffects(ABILITYEFFECT_IMMUNITY, battlerDef, 0, 0, TRUE))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndAttackerInvisible(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (IsSemiInvulnerable(cv->battlerAtk, CHECK_ALL)
     && gHitMarker & (HITMARKER_NO_ANIMATIONS | HITMARKER_DISABLE_ANIMATION))
    {
        BtlController_EmitSpriteInvisibility(cv->battlerAtk, B_COMM_TO_CONTROLLER, TRUE);
        MarkBattlerForControllerExec(cv->battlerAtk);
        result = MOVEEND_RESULT_BREAK;
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndAttackerVisible(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (!IsAnyTargetAffected()
     || !IsSemiInvulnerable(cv->battlerAtk, CHECK_ALL)
     || (gBattleStruct->unableToUseMove && gBattleMons[cv->battlerAtk].volatiles.semiInvulnerable != STATE_SKY_DROP_TARGET))
    {
        BtlController_EmitSpriteInvisibility(cv->battlerAtk, B_COMM_TO_CONTROLLER, FALSE);
        MarkBattlerForControllerExec(cv->battlerAtk);
        gBattleMons[cv->battlerAtk].volatiles.semiInvulnerable = STATE_NONE;
        gSpecialStatuses[cv->battlerAtk].restoredBattlerSprite = TRUE;
        result = MOVEEND_RESULT_BREAK;
    }
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndTargetVisible(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        gBattleStruct->eventState.moveEndBattler++;

        if (!gSpecialStatuses[battlerDef].restoredBattlerSprite
         && !IsSemiInvulnerable(battlerDef, CHECK_ALL))
        {
            BtlController_EmitSpriteInvisibility(battlerDef, B_COMM_TO_CONTROLLER, FALSE);
            MarkBattlerForControllerExec(battlerDef);
            gSpecialStatuses[battlerDef].restoredBattlerSprite = TRUE;
            gBattleMons[battlerDef].volatiles.semiInvulnerable = STATE_NONE;
            return MOVEEND_RESULT_BREAK;
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

// Resolve once per damaged opponent and strike, before contact reactions and
// the next hit. The new layer participates in this hit's hazard activation.
static enum MoveEndResult MoveEndScrapBrawler(struct BattleCalcValues *cv)
{
    if (cv->abilities[cv->battlerAtk] == ABILITY_SCRAP_BRAWLER
     && !IsBattleMoveStatus(cv->move) && !gBattleStruct->unableToUseMove)
    {
        while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
        {
            enum BattlerId target = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
            enum BattleSide side = GetBattlerSide(target);
            if (IsBattlerAlly(cv->battlerAtk, target) || !IsBattlerAlly(target, cv->battlerDef)
             || !IsBattlerTurnDamaged(target, INCLUDING_SUBSTITUTES) || !ShouldApplyAfterHitEffects(cv->battlerAtk, target))
            {
                gBattleStruct->eventState.moveEndBlock = 0;
                gBattleStruct->eventState.moveEndBattler++;
                continue;
            }
            if (gBattleStruct->eventState.moveEndBlock == 0)
            {
                gBattleStruct->eventState.moveEndBlock++;
                if (!gBattleStruct->isSkyBattle)
                {
                    bool32 spikes = RandomUniform(RNG_SCRAP_BRAWLER, 0, 1) == 0;
                    const u8 *script = NULL;
                    if (spikes && gSideTimers[side].spikesAmount < 3)
                    {
                        SetSpikesLayer(side, 1);
                        script = BattleScript_ScrapBrawlerSpikes;
                    }
                    else if (!spikes && !IsHazardOnSide(side, HAZARDS_STEALTH_ROCK))
                    {
                        PushHazardTypeToQueue(side, HAZARDS_STEALTH_ROCK);
                        script = BattleScript_ScrapBrawlerStealthRock;
                    }
                    if (script != NULL)
                    {
                        gBattlerAbility = gBattleScripting.battler = cv->battlerAtk;
                        gLastUsedAbility = ABILITY_SCRAP_BRAWLER;
                        RecordAbilityBattle(cv->battlerAtk, ABILITY_SCRAP_BRAWLER);
                        BattleScriptCall(script);
                        return MOVEEND_RESULT_RUN_SCRIPT;
                    }
                }
            }
            if (IsMoveMakingContact(cv->battlerAtk, target, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->move))
            {
                while (gBattleStruct->eventState.moveEndBlock <= gBattleStruct->numHazards[side])
                {
                    u32 index = gBattleStruct->eventState.moveEndBlock - 1;
                    enum Hazards hazard = gBattleStruct->hazardsQueue[side][index];
                    u32 count = gBattleStruct->numHazards[side];
                    bool32 effect = TryScrapBrawlerHazard(target, hazard);
                    // Poison types can absorb Toxic Spikes and shift the queue.
                    if (gBattleStruct->numHazards[side] == count)
                        gBattleStruct->eventState.moveEndBlock++;
                    if (effect)
                        return MOVEEND_RESULT_RUN_SCRIPT;
                }
            }
            gBattleStruct->eventState.moveEndBlock = 0;
            gBattleStruct->eventState.moveEndBattler++;
        }
    }
    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndDamagedEffectsBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        enum Ability abilityDef = cv->abilities[battlerDef];
        enum HoldEffect holdEffectDef = cv->holdEffects[battlerDef];

        if (!IsBattlerAlly(battlerDef, cv->battlerDef)
         || !IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
         || !ShouldApplyAfterHitEffects(cv->battlerAtk, battlerDef))
        {
            gBattleStruct->eventState.moveEndBlock = 0;
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        while (gBattleStruct->eventState.moveEndBlock < DAMAGED_EFFECTS_BLOCK_COUNT)
        {
            switch (gBattleStruct->eventState.moveEndBlock)
            {
            case DAMAGED_EFFECTS_BLOCK_RAGE:
                SetTechnicolorTargetType(battlerDef);
                if (gBattleMons[battlerDef].volatiles.rage
                 && IsBattlerAlive(battlerDef)
                 && !IsBattlerAlly(cv->battlerAtk, battlerDef)
                 && !IsBattleMoveStatus(cv->move)
                 && CompareStat(battlerDef, STAT_ATK, MAX_STAT_STAGE, CMP_LESS_THAN, abilityDef))
                {
                    // Does rage show any anim or does it just increase by one and print the rage message?
                    gBattleScripting.battler = battlerDef;
                    SetStatChange(battlerDef, STAT_ATK, 1);
                    BattleScriptCall(BattleScript_RageIsBuilding);
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_GRUDGE:
                if (gBattleMons[battlerDef].volatiles.grudge && !IsBattlerAlive(battlerDef) && IsBattlerAlive(cv->battlerAtk) && !IsBattlerAlly(cv->battlerAtk, battlerDef) && cv->move != MOVE_STRUGGLE && cv->moveEffect != EFFECT_FUTURE_SIGHT)
                {
                    enum MoveSlot moveIndex = gBattleStruct->chosenMovePositions[cv->battlerAtk];

                    gBattleMons[cv->battlerAtk].pp[moveIndex] = 0;

                    PREPARE_MOVE_BUFFER(gBattleTextBuff1, gBattleMons[cv->battlerAtk].moves[moveIndex])
                    BattleScriptCall(BattleScript_GrudgeTakesPP);
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_BEAK_BLAST:
                if (IsBattlerUsingBeakBlast(battlerDef)
                 && IsMoveMakingContact(cv->battlerAtk, battlerDef, cv->abilities[cv->battlerAtk], cv->holdEffects[cv->battlerAtk], cv->move)
                 && CanBeBurned(cv->battlerAtk, cv->battlerAtk, cv->abilities[cv->battlerAtk]))
                {
                    gBattleMons[cv->battlerAtk].status1 = STATUS1_BURN;
                    BtlController_EmitSetMonData(cv->battlerAtk, B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[cv->battlerAtk].status1), &gBattleMons[cv->battlerAtk].status1);
                    MarkBattlerForControllerExec(cv->battlerAtk);
                    BattleScriptCall(BattleScript_BeakBlastBurn);
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_AURORAL_HERALD:
                if ((gSideStatuses[GetBattlerSide(cv->battlerAtk)] & SIDE_STATUS_AURORA_VEIL)
                 && TryAuroralHeraldFrostbite(cv, battlerDef))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_ATTACKER_ABILITY: // Exclusively passes defender as the argument instead of ability battler
                if (AbilityBattleEffects(ABILITYEFFECT_MOVE_END_ATTACKER, battlerDef, cv->abilities[cv->battlerAtk], 0, TRUE))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_TARGET_ABILITY:
                if (AbilityBattleEffects(ABILITYEFFECT_MOVE_END, battlerDef, abilityDef, 0, TRUE)
                 || TryClearIllusion(battlerDef, abilityDef))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_RESIST_BERRY_MESSAGE:
                if (gSpecialStatuses[battlerDef].berryReduced
                 && !gSpecialStatuses[battlerDef].berryReducedMessagePrinted)
                {
                    gBattleScripting.battler = battlerDef;
                    GetBattlerPartyState(battlerDef)->ateBerry = TRUE;
                    gSpecialStatuses[battlerDef].berryReducedMessagePrinted = TRUE;
                    if (gSpecialStatuses[battlerDef].infiniteStashBerry != ITEM_NONE)
                    {
                        gLastUsedItem = gSpecialStatuses[battlerDef].infiniteStashBerry;
                        BattleScriptCall(BattleScript_InfiniteStashResistMessage);
                    }
                    else
                    {
                        gLastUsedItem = gBattleMons[battlerDef].item;
                        BattleScriptCall(BattleScript_BerryReduceDmg);
                    }
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_ITEM_EFFECTS_TARGET:
                if (ItemBattleEffects(battlerDef, cv->battlerAtk, holdEffectDef, IsOnTargetHitActivation))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_ITEM_EFFECTS_ATTACKER:
                if (ItemBattleEffects(cv->battlerAtk, battlerDef, cv->holdEffects[cv->battlerAtk], IsOnAttackerAfterHitActivation))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_JABOCA_ROWAP:
                if (ItemBattleEffects(battlerDef, cv->battlerAtk, holdEffectDef, IsJabocaRowapActivation))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_FORM_CHANGE_AFTER_HIT:
                if (AbilityBattleEffects(ABILITYEFFECT_FORM_CHANGE_ON_HIT, battlerDef, abilityDef, 0, TRUE))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_PROTECT_LIKE_EFFECTS:
                if (ShouldApplyProtectLikeEffects(battlerDef, cv))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case DAMAGED_EFFECTS_BLOCK_COUNT:
                break;
            }

            if (result != MOVEEND_RESULT_CONTINUE)
                return result;

        }
        gBattleStruct->eventState.moveEndBlock = 0;
        gBattleStruct->eventState.moveEndBattler++;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndMentalHerb(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForMoveEnd(battlerDef, cv))
            continue;

        if (ItemBattleEffects(battlerDef, cv->battlerAtk, cv->holdEffects[battlerDef], IsMentalHerbActivation))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndSymbiosis(struct BattleCalcValues *cv)
{
    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if ((battlerDef != cv->battlerAtk || !IsBattlerAlly(battlerDef, cv->battlerDef))
         && ShouldSkipBattlerForMoveEnd(battlerDef, cv))
            continue;

        enum BattlerId partnerDef = GetPartnerBattler(battlerDef);

        if ((gSpecialStatuses[battlerDef].berryReduced
          || (GetConfig(B_SYMBIOSIS_GEMS) >= GEN_7 && gSpecialStatuses[battlerDef].gemBoost))
         && TryTriggerSymbiosis(battlerDef, partnerDef))
        {
            BestowItem(partnerDef, battlerDef);
            gLastUsedAbility = gBattleMons[partnerDef].ability;
            gEffectBattler = battlerDef;
            gBattleScripting.battler = gBattlerAbility = partnerDef;
            BattleScriptPushCursor();
            gBattlescriptCurrInstr = BattleScript_SymbiosisActivates;
            return MOVEEND_RESULT_RUN_SCRIPT;
        }
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndFaintBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        if (!IsBattlerAlly(battlerDef, cv->battlerDef) && !IsBattleMoveStatus(cv->move))
        {
            gBattleStruct->eventState.moveEndBlock = 0;
            gBattleStruct->eventState.moveEndBattler++;
            continue;
        }

        while (gBattleStruct->eventState.moveEndBlock < FAINT_BLOCK_COUNT)
        {
            switch (gBattleStruct->eventState.moveEndBlock)
            {
            case FAINT_BLOCK_FINAL_GAMBIT:
                if (cv->moveEffect == EFFECT_FINAL_GAMBIT
                 && battlerDef == cv->battlerAtk
                 && IsBattlerAlive(cv->battlerAtk)
                 && !gBattleStruct->unableToUseMove
                 && IsAnyTargetAffected())
                {
                    if (cv->abilities[cv->battlerAtk] == ABILITY_GREAT_MINDS && gSpecialStatuses[cv->battlerAtk].multiHitTargets)
                        gSpecialStatuses[cv->battlerAtk].greatMindsExplosion = TRUE;
                    else
                    {
                        BattleScriptCall(BattleScript_FinalGambit);
                        result = MOVEEND_RESULT_RUN_SCRIPT;
                    }
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case FAINT_BLOCK_CHECK_TARGET_FAINTED: // Stop if target already ran the block / is alive or absent
                if (IsBattlerAlive(battlerDef)
                 || battlerDef == cv->battlerAtk // Faint attacker after target
                 || battlerDef >= gBattlersCount
                 || gBattleStruct->battlerState[battlerDef].notOnField)
                {
                    gBattleStruct->eventState.moveEndBlock = FAINT_BLOCK_COUNT;
                    break;
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case FAINT_BLOCK_TRY_DESTINY_BOND: // Checked before FAINT_BLOCK_FAINT_TARGET but occurs after since volatiles are cleared on faint
                if (gBattleMons[battlerDef].volatiles.destinyBond
                 && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
                 && IsBattlerAlive(cv->battlerAtk)
                 && !IsBattlerAlly(cv->battlerAtk, battlerDef))
                {
                    gBattleStruct->passiveHpUpdate[cv->battlerAtk] = gBattleMons[cv->battlerAtk].hp;
                    BattleScriptCall(BattleScript_DestinyBondTakesLife);
                    // Hack: don't change result here so that Faint Target's script plays first
                }
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case FAINT_BLOCK_FAINT_TARGET:
                SetValuesOnFaint(battlerDef);
                TryUpdateEvolutionTracker(IF_DEFEAT_X_WITH_ITEMS, 1, MOVE_NONE);
                BattleScriptCall(BattleScript_FaintBattler);
                result = MOVEEND_RESULT_RUN_SCRIPT;
                gBattleStruct->eventState.moveEndBlock++;
                break;
            case FAINT_BLOCK_COUNT:
                break;
            }

            if (result != MOVEEND_RESULT_CONTINUE)
                return result;

        }
        gBattleStruct->eventState.moveEndBlock = 0;
        gBattleStruct->eventState.moveEndBattler++;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndFaintAttacker(struct BattleCalcValues *cv)
{
    if (!IsBattlerAlive(cv->battlerAtk) && !gBattleStruct->battlerState[cv->battlerAtk].notOnField)
    {
        SetValuesOnFaint(cv->battlerAtk);
        BattleScriptCall(BattleScript_FaintBattler);
        return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndSetValuesForOpposingSide(struct BattleCalcValues *cv)
{
    cv->battlerDef = GetBattlerLeftFoe(cv->battlerAtk);
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndUpdateLastMoves(struct BattleCalcValues *cv)
{
    if (!IsOnPlayerSide(cv->battlerAtk))
        UpdateStallMons(cv);

    if (gBattleStruct->snatchedMoveIsUsed)
    {
        RestoreAttacker();
        RestoreTarget();
    }

    enum BattleMoveEffects originalEffect = GetMoveEffect(GetOriginallyUsedMove(gChosenMove));
    if (IsBattlerAlive(cv->battlerAtk) // Why do we need to check if user fainted? We just want to set with what move the target got hit
     && originalEffect != EFFECT_BATON_PASS
     && originalEffect != EFFECT_HEALING_WISH
     && originalEffect != EFFECT_LUNAR_DANCE)
    {
        if (!gBattleStruct->unableToUseMove)
        {
            if (!IsBattlerUsingAbilityMove(cv->battlerAtk))
            {
                gLastMoves[cv->battlerAtk] = gChosenMove;
                gLastResultingMoves[cv->battlerAtk] = cv->move;
                gLastUsedMoveType[cv->battlerAtk] = GetBattleMoveType(cv->move);
            }
        }
        else if (!gSpecialStatuses[cv->battlerAtk].postActionAbilityMove)
        {
            gLastResultingMoves[cv->battlerAtk] = MOVE_UNAVAILABLE;
            gLastUsedMoveType[cv->battlerAtk] = 0;
        }

        for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
        {
            if (!gBattleStruct->unableToUseMove
             && !IsBattlerUnaffectedByMove(battlerDef)
             && gChosenMove != MOVE_UNAVAILABLE
             && !IsBattlerUsingAbilityMove(cv->battlerAtk))
            {
                gLastUsedMove = cv->move;
            }

            if (cv->battlerAtk == battlerDef || IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_NOT_TARGETED))
                continue;

            gLastHitBy[cv->battlerDef] = cv->battlerAtk; // Used by switch AI only

            if (!gBattleStruct->unableToUseMove
             && !IsBattlerUnaffectedByMove(battlerDef))
            {
                if (gChosenMove == MOVE_UNAVAILABLE)
                {
                    gLastLandedMoves[battlerDef] = gChosenMove;
                }
                else
                {
                    gLastLandedMoves[battlerDef] = cv->move;
                    gLastHitByType[battlerDef] = gBattleStruct->technicolorMoveTypes[battlerDef] != TYPE_NONE
                        ? gBattleStruct->technicolorMoveTypes[battlerDef] : GetBattleMoveType(cv->move);
                }
            }
            else
            {
                gLastHitByType[battlerDef] = TYPE_MYSTERY;
                gLastLandedMoves[battlerDef] = MOVE_UNAVAILABLE;
            }
        }
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndMirrorMove(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);

        gBattleStruct->eventState.moveEndBattler++;

        if (ShouldSkipBattlerForMoveEnd(battlerDef, cv))
            continue;

        if (!gBattleStruct->unableToUseMove
        && cv->battlerAtk != battlerDef
        && IsBattlerAlive(cv->battlerAtk)
        && IsBattlerAlive(battlerDef)
        && !IsMoveMirrorMoveBanned(GetOriginallyUsedMove(gChosenMove)))
        {
            gBattleStruct->lastTakenMove[battlerDef] = gChosenMove;
            gBattleStruct->lastTakenMoveFrom[battlerDef][cv->battlerAtk] = gChosenMove;
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndBouncedMove(struct BattleCalcValues *cv)
{
    if (gBattleStruct->bouncedMoveIsUsed)
    {
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_CONTINUE;
    }

    if (gBattleStruct->magicBouncePending || gBattleStruct->magicCoatPending)
    {
        enum MoveTarget moveTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);

        for (u32 speedOrderIndex = 0; speedOrderIndex < gBattlersCount; speedOrderIndex++)
        {
            enum BattlerId bounceBattler = gBattlersByRawSpeed[speedOrderIndex];

            if (gBattleStruct->magicBouncePending & 1u << bounceBattler)
            {
                gBattlerAbility = bounceBattler;
                gBattlescriptCurrInstr = BattleScript_MoveResolution;
                BattleScriptCall(BattleScript_MagicBounce);
            }
            else if (gBattleStruct->magicCoatPending & 1u << bounceBattler)
            {
                gBattlescriptCurrInstr = BattleScript_MoveResolution;
                BattleScriptCall(BattleScript_MagicCoat);
            }
            else
            {
                continue;
            }

            if (moveTarget == TARGET_OPPONENTS_FIELD)
            {
                gBattleStruct->magicBouncePending = 0;
                gBattleStruct->magicCoatPending = 0;
            }
            else
            {
                gBattleStruct->magicBouncePending &= ~(1u << bounceBattler);
                gBattleStruct->magicCoatPending &= ~(1u << bounceBattler);
            }

            SaveBattlerAttacker(cv->battlerAtk);
            SaveBattlerTarget(cv->battlerDef);
            gBattlerTarget = cv->battlerAtk;
            gBattlerAttacker = bounceBattler;
            gBattleStruct->bouncedMoveIsUsed = TRUE;
            for (enum BattlerId i = B_BATTLER_0; i < gBattlersCount; i++)
            {
                gBattleStruct->savedMoveResultFlags[i] = gBattleStruct->moveResultFlags[i];
            }

            ClearDamageCalcResults();
            gBattleStruct->eventState.atkCanceler = CANCELER_SET_TARGETS;
            gBattleStruct->eventState.atkCancelerBattler = 0;
            gBattleStruct->moveTarget[cv->battlerAtk] = cv->battlerDef;
            gBattleScripting.moveendState = 0;
            gBattleScripting.animTurn = 0;
            gBattleScripting.animTargetsHit = 0;

            return MOVEEND_RESULT_RUN_SCRIPT;
        }
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 IsAnyMultiHitTargetAlive(enum BattlerId battlerAtk)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if ((gSpecialStatuses[battlerAtk].multiHitTargets & (1u << battler))
         && IsBattlerAlive(battler)
         && !IsBattlerMoveResultSet(battler, MOVE_RESULT_INVALID_TARGET))
            return TRUE;
    }
    return FALSE;
}

static enum MoveEndResult MoveEndReverberations(struct BattleCalcValues *cv)
{
    if (cv->isStatusMove
     && cv->abilities[cv->battlerAtk] == ABILITY_REVERBERATIONS
     && IsMoveReverberationsAffected(cv->battlerAtk, cv->move)
     && !gSpecialStatuses[cv->battlerAtk].reverberationsEcho
     && !gBattleStruct->unableToUseMove
     && !gBattleStruct->bouncedMoveIsUsed
     && IsBattlerAlive(cv->battlerAtk)
     && IsAnyTargetAffected())
    {
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            if (IsBattlerAlive(battler) && !IsBattlerMoveResultSet(battler, MOVE_RESULT_INVALID_TARGET))
                gSpecialStatuses[cv->battlerAtk].multiHitTargets |= 1u << battler;
        }
        gSpecialStatuses[cv->battlerAtk].reverberationsEcho = TRUE;
        RecordAbilityBattle(cv->battlerAtk, ABILITY_REVERBERATIONS);

        // Re-enter effect resolution, not action selection: no second PP cost,
        // obedience roll, sleep tick, or recursive echo.
        ClearDamageCalcResults();
        gBattleStruct->eventState.atkCanceler = CANCELER_SET_TARGETS;
        gBattleStruct->eventState.atkCancelerBattler = 0;
        gBattleStruct->eventState.moveEndBlock = 0;
        gBattleStruct->eventState.resolution = 0;
        gBattleStruct->statChangeBattler = 0;
        gBattleStruct->additionalEffectsCounter = 0;
        gBattleScripting.animTurn = 0;
        gBattleScripting.animTargetsHit = 0;
        gBattleScripting.moveendState = 0;
        BattleScriptPush(BattleScript_MoveResolution);
        gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
        return MOVEEND_RESULT_BREAK;
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndMultihitMoveBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    // Contact effects and secondary stat changes for this strike have resolved.
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        gProtectStructs[battler].gloriousManeProtectedHit = FALSE;

    // cv->battlerDef needs to change here because otherwise left foe is assumed as target
    cv->battlerDef = gBattlerTarget;

    // Blocking the first dart must not cancel the other opponent's dart.
    bool32 shellBurstSmartTarget = GetBattlerMoveTargetType(cv->battlerAtk, cv->move) == TARGET_SMART
        && gBattleStruct->battlerState[cv->battlerDef].shellBurstBlockedMove
        && gMultiHitCounter > 1 && CanTargetPartner(cv->battlerAtk, cv->battlerDef)
        && !IsAffectedByFollowMe(cv->battlerAtk, GetBattlerSide(cv->battlerDef), cv->move)
        && !IsBattlerUnaffectedByMove(GetPartnerBattler(cv->battlerDef));

    if ((gSpecialStatuses[cv->battlerAtk].multiHitTargets
            ? !IsAnyTargetAffected()
            : IsBattlerUnaffectedByMove(cv->battlerDef) && !shellBurstSmartTarget)
     || gBattleStruct->unableToUseMove)
    {
        gBattleStruct->eventState.moveEndBlock = 0;
        gBattleScripting.moveendState++;
        return result;
    }

    if (!gSpecialStatuses[cv->battlerAtk].multiHitOn
     && gMultiHitCounter == 0)
    {
        gBattleStruct->eventState.moveEndBlock = 0;
        gBattleScripting.moveendState++;
        return result;
    }

    enum MoveTarget target = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);

    while (gBattleStruct->eventState.moveEndBlock < MULTIHIT_BLOCK_COUNT)
    {
        switch (gBattleStruct->eventState.moveEndBlock)
        {
        case MULTIHIT_BLOCK_HP_THRESHOLD_ITEMS:
            if (gSpecialStatuses[cv->battlerAtk].multiHitTargets)
            {
                while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
                {
                    enum BattlerId battler = gBattleStruct->eventState.moveEndBattler++;
                    if (gMultiHitCounter
                     && (gSpecialStatuses[cv->battlerAtk].multiHitTargets & (1u << battler))
                     && IsBattlerAlive(battler)
                     && ItemBattleEffects(battler, cv->battlerAtk, cv->holdEffects[battler], IsOnHpThresholdActivation))
                        return MOVEEND_RESULT_RUN_SCRIPT;
                }
                gBattleStruct->eventState.moveEndBattler = 0;
            }
            else if (gMultiHitCounter
             && ItemBattleEffects(cv->battlerDef, cv->battlerAtk, cv->holdEffects[cv->battlerDef], IsOnHpThresholdActivation))
            {
                result = MOVEEND_RESULT_RUN_SCRIPT;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_DECREMENT_HIT:
            if (gMultiHitCounter)
            {
                gSpecialStatuses[cv->battlerAtk].multiHitOn = TRUE;
                gMultiHitCounter--;
                if (gSpecialStatuses[cv->battlerAtk].parentalBondState)
                    gSpecialStatuses[cv->battlerAtk].parentalBondState--;
                gBattleScripting.multihitString[4]++;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_SMART_REDIRECTION:
            if (gMultiHitCounter
             && target == TARGET_SMART
             && !IsAffectedByFollowMe(cv->battlerAtk, GetBattlerSide(cv->battlerDef), cv->move)
             && CanTargetPartner(cv->battlerAtk, cv->battlerDef)
             && !IsBattlerUnaffectedByMove(GetPartnerBattler(cv->battlerDef)))
            {
                gBattlerTarget = cv->battlerDef = GetPartnerBattler(cv->battlerDef); // Target the partner in doubles for second hit.
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_INTERRUPT_EXECUTION:
            if (!gMultiHitCounter
             || (gSpecialStatuses[cv->battlerAtk].multiHitTargets
                    ? !IsAnyMultiHitTargetAlive(cv->battlerAtk)
                    : !IsBattlerAlive(cv->battlerDef))
             || (cv->moveEffect != EFFECT_FUTURE_SIGHT
              && (!IsBattlerAlive(cv->battlerAtk)
               || (!IsUsableWhileAsleepEffect(GetMoveEffect(gChosenMove)) && cv->abilities[cv->battlerAtk] != ABILITY_SPICED_DREAMS && (gBattleMons[cv->battlerAtk].status1 & STATUS1_SLEEP))
               || (gBattleMons[cv->battlerAtk].status1 & STATUS1_FREEZE))))
            {
                gMultiHitCounter = 0;
                gSpecialStatuses[cv->battlerAtk].parentalBondState = PARENTAL_BOND_OFF;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_EFFECTIVENESS_MESSAGE:
            if (gMultiHitCounter == 0 && target != TARGET_SMART && !gBattleStruct->battlerState[cv->battlerDef].substituteBlocked)
            {
                if (ShouldPrintEffectivenessMessage(cv))
                    result = MOVEEND_RESULT_RUN_SCRIPT;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_NEXT_HIT:
            if (gMultiHitCounter)
            {
                if (gSpecialStatuses[cv->battlerAtk].multiHitTargets)
                {
                    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
                    {
                        if (!IsBattlerAlive(battler))
                            AddBattlerMoveResultFlag(battler, MOVE_RESULT_NOT_TARGETED);
                    }
                }
                gBattleStruct->eventState.atkCanceler = CANCELER_ACCURACY_CHECK;
                gBattleStruct->eventState.atkCancelerBattler = 0;
                gBattleScripting.animTargetsHit = 0;
                gBattleScripting.moveendState = 0;
                gBattleStruct->eventState.moveEndBlock = 0;
                gSpecialStatuses[cv->battlerDef].resultMessagePrinted = FALSE;
                gSpecialStatuses[cv->battlerDef].critMessagePrinted = FALSE;
                gSpecialStatuses[cv->battlerDef].protectMessagePrinted = FALSE;

                for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
                    gBattleStruct->battlerState[battler].substituteBlocked = FALSE;

                BattleScriptPush(cv->moveEffect == EFFECT_FUTURE_SIGHT ? BattleScript_ContinueFutureAttack : BattleScript_MoveResolution);
                gBattlescriptCurrInstr = BattleScript_FlushMessageBox;
                return MOVEEND_RESULT_BREAK;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_PRINT_NUM_OF_HITS:
            if (gMultiHitCounter == 0 && target != TARGET_SMART) // Dragon Darts doesn't show number of hits
            {
                BattleScriptCall(BattleScript_MultiHitPrintStrings);
                result = MOVEEND_RESULT_RUN_SCRIPT;
            }
            gBattleStruct->eventState.moveEndBlock++;
            break;
        case MULTIHIT_BLOCK_COUNT:
            break;
        }

        if (result != MOVEEND_RESULT_CONTINUE)
            return result;
    }

    gBattleStruct->eventState.moveEndBlock = 0;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndDefrost(struct BattleCalcValues *cv)
{
    const u8 *battleScript = NULL;

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = (enum BattlerId)gBattleStruct->eventState.moveEndBattler++;

        if (battler == gBattlerAttacker)
            continue;

        if (!(gBattleMons[battler].status1 & STATUS1_ICY_ANY)
         || !IsBattlerTurnDamaged(battler, EXCLUDING_SUBSTITUTES)
         || !IsBattlerAlive(battler))
            continue;

        if (gBattleMons[battler].status1 & STATUS1_FREEZE)
            battleScript = BattleScript_BattlerDefrosted;
        else
            battleScript = BattleScript_BattlerFrostbiteHealed;

        if ((CanFireMoveThawTarget(cv->move, GetBattleMoveType(cv->move)) || CanBurnHitThaw(cv->move)) && gBattleMons[battler].status1 & STATUS1_FREEZE)
        {
            DefrostBattler(battler, gBattleMons[battler].status1);
            BattleScriptCall(battleScript);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROSTED;
            return MOVEEND_RESULT_RUN_SCRIPT;
        }
        else if (IsBattlerAlive(cv->battlerAtk)
              && CanMoveThawTarget(cv->abilities[cv->battlerAtk], cv->move))
        {
            DefrostBattler(battler, gBattleMons[battler].status1);
            BattleScriptCall(battleScript);
            gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_DEFROSTED;
            return MOVEEND_RESULT_RUN_SCRIPT;
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndMoveBlockRecoil(struct BattleCalcValues *cv)
{
    if (gBattleStruct->hauntingShrieksBgActive)
    {
        gBattleStruct->hauntingShrieksBgActive = FALSE;
        BattleScriptCall(BattleScript_HauntingShrieksEnd);
        gBattleScripting.moveendState++;
        return MOVEEND_RESULT_RUN_SCRIPT;
    }
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    switch (cv->moveEffect)
    {
    case EFFECT_RECOIL_IF_MISS:
        if (IsBattlerAlive(cv->battlerAtk)
         && IsBattlerUnaffectedByMove(cv->battlerDef)
         && !gBattleStruct->unableToUseMove
         && !IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_VAMPIRISM))
        {
            s32 recoil = 0;
            if (B_CRASH_IF_TARGET_IMMUNE == GEN_4 && gBattleStruct->moveResultFlags[cv->battlerDef] & MOVE_RESULT_DOESNT_AFFECT_FOE)
            {
                recoil = GetBattlerMaxHP(cv->battlerDef) / 2;
            }
            if (B_RECOIL_IF_MISS_DMG >= GEN_5)
            {
                recoil = GetBattlerMaxHP(cv->battlerAtk) / 2;
            }
            else if (B_RECOIL_IF_MISS_DMG >= GEN_3)
            {
                if ((GetBattlerMaxHP(cv->battlerDef) / 2) < gBattleStruct->moveDamage[cv->battlerDef])
                    recoil = gBattleStruct->moveDamage[cv->battlerDef];
                else
                    recoil = GetBattlerMaxHP(cv->battlerDef) / 2;
            }
            else if (B_RECOIL_IF_MISS_DMG == GEN_2)
            {
                recoil = gBattleStruct->moveDamage[cv->battlerDef] / 8;
            }
            else
            {
                recoil = 1;
            }
            SetPassiveDamageAmount(cv->battlerAtk, recoil);
            BattleScriptCall(BattleScript_RecoilIfMiss);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    case EFFECT_RECOIL:
        if (gBattleStruct->moveDamage[cv->battlerDef] == 0)
            break;
        [[fallthrough]];
    case EFFECT_CHLOROBLAST:
        if (IsBattlerTurnDamaged(cv->battlerDef, INCLUDING_SUBSTITUTES) && IsBattlerAlive(cv->battlerAtk))
        {
            if (IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_ROCK_HEAD)
             || IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_MAGIC_GUARD)
             || IsAbilityAndRecord(cv->battlerAtk, cv->abilities[cv->battlerAtk], ABILITY_VAMPIRISM))
                break;

            s32 recoilDamage = gBattleStruct->accumulatedDamage;
            if (cv->abilities[cv->battlerAtk] == ABILITY_RELENTLESS_CHASE && gBattleStruct->relentlessChaseDamage > 0)
            {
                if (cv->moveEffect == EFFECT_CHLOROBLAST)
                    break;
                recoilDamage -= gBattleStruct->relentlessChaseDamage;
                if (recoilDamage <= 0)
                    break;
            }
            if (cv->moveEffect == EFFECT_CHLOROBLAST)
            {
                s32 recoil = (GetBattlerMaxHP(cv->battlerAtk) + 1) / 2; // Half of Max HP Rounded UP
                SetPassiveDamageAmount(cv->battlerAtk, recoil);
            }
            else
            {
                SetPassiveDamageAmount(cv->battlerAtk, recoilDamage * max(1, GetMoveRecoil(cv->move)) / 100);
            }
            TryUpdateEvolutionTracker(IF_RECOIL_DAMAGE_GE, gBattleStruct->passiveHpUpdate[cv->battlerAtk], MOVE_NONE);
            BattleScriptCall(BattleScript_MoveEffectRecoil);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    case EFFECT_RAPID_SPIN:
        if (GetConfig(B_SPEED_BUFFING_RAPID_SPIN) == GEN_8
         && IsAnyTargetTurnDamaged(cv->battlerAtk, INCLUDING_SUBSTITUTES)
         && IsBattlerAlive(cv->battlerAtk))
        {
            BattleScriptCall(BattleScript_RapidSpinAway);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    default:
        break;
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndSheerForce(struct BattleCalcValues *cv)
{
    if (IsSheerForceAffected(cv->move, cv->abilities[cv->battlerAtk]))
        gBattleScripting.moveendState = GetConfig(B_SHEER_FORCE_TIMING) >= GEN_CHAMPIONS ? MOVEEND_CARD_BUTTON : MOVEEND_ITEMS_EFFECTS_ALL;
    else
        gBattleScripting.moveendState++;

    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndMoveBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {

        enum BattlerId battlerDef = gEffectBattler = GetTargetBySlot(cv->battlerAtk, gBattleStruct->eventState.moveEndBattler);
        gBattleStruct->eventState.moveEndBattler++;

        if (battlerDef == cv->battlerAtk)
            continue;

        enum BattleSide side = GetBattlerSide(battlerDef);

        switch (cv->moveEffect)
        {
        case EFFECT_SPIT_UP:
        case EFFECT_SWALLOW:
            if (!gBattleStruct->unableToUseMove)
            {
                gBattleMons[cv->battlerAtk].volatiles.stockpileCounter = 0;

                if (gBattleMons[cv->battlerAtk].volatiles.stockpileDef > 0)
                {
                    SetStatChange(gBattlerAttacker, STAT_DEF, -1 * gBattleMons[gBattlerAttacker].volatiles.stockpileDef);
                    gBattleMons[gBattlerAttacker].volatiles.stockpileDef = 0;
                }
                if (gBattleMons[gBattlerAttacker].volatiles.stockpileSpDef > 0)
                {
                    SetStatChange(gBattlerAttacker, STAT_SPDEF, -1 * gBattleMons[gBattlerAttacker].volatiles.stockpileSpDef);
                    gBattleMons[gBattlerAttacker].volatiles.stockpileSpDef = 0;
                }

                BattleScriptCall(BattleScript_MoveEffectStockpileWoreOff);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_KNOCK_OFF:
            if (gBattleMons[battlerDef].item != ITEM_NONE
             && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
             && !(B_KNOCK_OFF_REMOVAL >= GEN_5 && side == B_SIDE_PLAYER && !IsTrainerBattle())
             && CanBattlerGetOrLoseItem(battlerDef, cv->battlerAtk, gBattleMons[battlerDef].item)
             && !NoAliveMonsForEitherParty())
            {
                if (!IsBattlerAlive(cv->battlerAtk) && GetConfig(B_FAINT_MOVE_EFFECT_TIMING) < GEN_CHAMPIONS)
                    break;

                if (cv->abilities[battlerDef] == ABILITY_STICKY_HOLD)
                {
                    gBattlerAbility = battlerDef;
                    BattleScriptCall(BattleScript_StickyHoldActivatesRet);
                    return MOVEEND_RESULT_RUN_SCRIPT;
                    break;
                }

                gLastUsedItem = gBattleMons[battlerDef].item;
                gBattleMons[battlerDef].item = ITEM_NONE;
                if (gBattleMons[battlerDef].ability != ABILITY_GORILLA_TACTICS)
                    gBattleStruct->choicedMove[battlerDef] = MOVE_NONE;
                CheckSetUnburden(battlerDef);

                // In Gen 5+, Knock Off removes the target's item rather than rendering it unusable
                if (B_KNOCK_OFF_REMOVAL >= GEN_5)
                {
                    BtlController_EmitSetMonData(battlerDef, B_COMM_TO_CONTROLLER, REQUEST_HELDITEM_BATTLE, 0, sizeof(gBattleMons[battlerDef].item), &gBattleMons[battlerDef].item);
                    MarkBattlerForControllerExec(battlerDef);
                    // Mark item as stolen so it will be restored after battle
                    GetLostItemState(GetBattlerTrainer(battlerDef), gBattlerPartyIndexes[battlerDef])->stolen = TRUE;
                }
                else
                {
                    GetBattlerPartyState(battlerDef)->isKnockedOff = TRUE;
                }

                BattleScriptCall(BattleScript_KnockedOff);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_SECRET_POWER:
            if (IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES) && IsBattlerAlive(cv->battlerAtk))
            {
                u32 chance = GetMoveSecondaryEffectChance(cv->move);
                bool32 hasSereneGrace = HasSereneGraceAbility(cv->abilities[cv->battlerAtk]);
                bool32 hasRainbow = gSideStatuses[GetBattlerSide(cv->battlerAtk)] & SIDE_STATUS_RAINBOW;
                bool32 hasFlinchEffect = gFieldTimers.terrain == B_TERRAIN_NONE
                                      && gBattleEnvironmentInfo[gBattleEnvironment].secretPowerEffect == MOVE_EFFECT_FLINCH;

                if (hasSereneGrace)
                    chance *= 2;
                if (hasRainbow && !(hasSereneGrace && hasFlinchEffect))
                    chance *= 2;

                if (RandomPercentage(RNG_SECONDARY_EFFECT, chance))
                {
                    const u8 *moveEndScript = gBattlescriptCurrInstr;
                    struct SetEffect se = {0};

                    se.moveEffect = MOVE_EFFECT_SECRET_POWER;
                    se.script = moveEndScript;
                    se.effectBattler = battlerDef;

                    SetMoveEffect(cv, &se);
                    if (gBattlescriptCurrInstr != moveEndScript)
                        return MOVEEND_RESULT_RUN_SCRIPT;
                }
            }
            break;
        case EFFECT_STEAL_ITEM:
            if (!IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
             || gBattleMons[cv->battlerAtk].item != ITEM_NONE
             || gBattleMons[battlerDef].item == ITEM_NONE
             || (!IsBattlerAlive(cv->battlerAtk) && GetConfig(B_FAINT_MOVE_EFFECT_TIMING) < GEN_CHAMPIONS)
             || !CanStealItem(cv->battlerAtk, battlerDef, gBattleMons[battlerDef].item))
                continue;

            if (cv->abilities[battlerDef] == ABILITY_STICKY_HOLD)
            {
                BattleScriptCall(BattleScript_StickyHoldActivatesRet);
                gBattlerAbility = battlerDef;
                gLastUsedAbility = gBattleMons[battlerDef].ability;
                RecordAbilityBattle(battlerDef, gLastUsedAbility);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            else
            {
                bool32 addStolenWildItemToBag = GetConfig(B_STEAL_WILD_ITEMS) >= GEN_9 && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER);
                StealTargetItem(cv->battlerAtk, battlerDef, ITEM_NONE, addStolenWildItemToBag);  // Attacker steals target item

                struct LostItem *lostItem = GetLostItemState(GetBattlerTrainer(battlerDef), gBattlerPartyIndexes[battlerDef]);

                if (addStolenWildItemToBag && !IsOnPlayerSide(battlerDef))
                {
                    lostItem->wildItemPending = TRUE;
                }
                else
                {
                    gBattleMons[cv->battlerAtk].item = gLastUsedItem;
                }

                if (B_RETURN_STOLEN_NPC_ITEMS >= GEN_5 && IsTrainerBattle())
                {
                    lostItem->stolen = TRUE;
                }

                gEffectBattler = cv->battlerDef;
                if (IsBattlerAlive(cv->battlerAtk))
                    BattleScriptCall(BattleScript_ItemSteal);
                else
                    BattleScriptCall(BattleScript_ItemStealNoAnim);

                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_HIT_SWITCH_TARGET:
            battlerDef = gBattlerTarget;
            if (IsBattlerTurnDamaged(battlerDef , EXCLUDING_SUBSTITUTES)
             && IsBattlerAlive(battlerDef)
             && IsBattlerAlive(cv->battlerAtk)
             && gBattleStruct->battlerState[battlerDef].commanderSpecies == SPECIES_NONE)
            {
                if (cv->abilities[battlerDef] == ABILITY_GUARD_DOG)
                {
                    gBattleStruct->eventState.moveEndBattler = 0;
                    gBattleScripting.moveendState++;
                    return MOVEEND_RESULT_CONTINUE;
                }
                else if (cv->abilities[battlerDef] == ABILITY_SUCTION_CUPS)
                {
                    gBattlerAbility = battlerDef;
                    BattleScriptCall(BattleScript_AbilityPreventsPhasingOut);
                }
                else if (gBattleMons[battlerDef].volatiles.root)
                {
                    BattleScriptCall(BattleScript_PrintMonIsRooted);
                }
                else
                {
                    gBattleScripting.switchCase = B_SWITCH_HIT;
                    BattleScriptCall(BattleScript_TryHitSwitchTarget);
                }

                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_SMACK_DOWN:
            if (IsBattlerAlive(battlerDef)
             && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
             && gBattleMons[battlerDef].volatiles.semiInvulnerable != STATE_SKY_DROP_ATTACKER
             && gBattleMons[battlerDef].volatiles.semiInvulnerable != STATE_SKY_DROP_TARGET)
            {
                bool32 onAir = gBattleMons[battlerDef].volatiles.semiInvulnerable == STATE_ON_AIR;

                if (!onAir && IsBattlerGrounded(battlerDef, cv->abilities[battlerDef], cv->holdEffects[battlerDef]))
                    break;

                gBattleMons[battlerDef].volatiles.smackDown = TRUE;
                gBattleMons[battlerDef].volatiles.telekinesis = FALSE;
                gBattleMons[battlerDef].volatiles.magnetRiseTimer = FALSE;

                if (onAir)
                {
                    gBattleMons[battlerDef].volatiles.semiInvulnerable = STATE_NONE;
                    gBattleMons[battlerDef].volatiles.multipleTurns = FALSE;
                }

                BattleScriptCall(BattleScript_MoveEffectSmackDown);
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_RAPID_SPIN:
            if (GetConfig(B_SPEED_BUFFING_RAPID_SPIN) != GEN_8
             && IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES))
            {
                if (!IsBattlerAlive(cv->battlerAtk) && GetConfig(B_FAINT_MOVE_EFFECT_TIMING) < GEN_CHAMPIONS)
                    break;

                BattleScriptCall(BattleScript_RapidSpinAway);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_FELL_STINGER:
            if (IsBattlerAlive(cv->battlerAtk)
             && !IsBattlerAlive(battlerDef)
             && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
             && !NoAliveMonsForEitherParty()
             && CompareStat(cv->battlerAtk, STAT_ATK, MAX_STAT_STAGE, CMP_LESS_THAN, cv->abilities[cv->battlerAtk]))
            {
                s32 stage = GetConfig(B_FELL_STINGER_STAT_RAISE) >= GEN_7 ? 3 : 2;
                SetStatChange(gBattlerAttacker, STAT_ATK, stage);
                BattleScriptCall(BattleScript_MoveEffectStatChange);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_STONE_AXE:
            if (!IsHazardOnSide(side, HAZARDS_STEALTH_ROCK)
             && IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES)
             && IsBattlerAlive(cv->battlerAtk))
            {
                PushHazardTypeToQueue(side, HAZARDS_STEALTH_ROCK);
                PrepareStringBattleWithWait(STRINGID_POINTEDSTONESFLOAT, battlerDef);
                BattleScriptCall(BattleScript_WaitMessage);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_CEASELESS_EDGE:
            if ((cv->move != MOVE_SERRATED_SLICE || IS_BATTLER_OF_TYPE(battlerDef, TYPE_STEEL))
             && gSideTimers[side].spikesAmount < 3
             && IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES)
             && IsBattlerAlive(cv->battlerAtk))
            {
                if (gBattleStruct->isSkyBattle)
                {
                    gBattleStruct->eventState.moveEndBattler = 0;
                    gBattleScripting.moveendState++;
                    return MOVEEND_RESULT_CONTINUE;
                }
                else
                {
                    SetSpikesLayer(side, 1);
                    PrepareStringBattleWithWait(STRINGID_SPIKESSCATTERED, battlerDef);
                    BattleScriptCall(BattleScript_WaitMessage);
                    gBattleStruct->eventState.moveEndBattler = 0;
                    gBattleScripting.moveendState++;
                    return MOVEEND_RESULT_RUN_SCRIPT;
                }
            }
            break;
        case EFFECT_DAZZLING_DASH:
            if (IsBattlerAlive(cv->battlerAtk)
             && IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES)
             && gBattleStruct->dazzlingDashWasSlower)
            {
                SetStatChange(cv->battlerAtk, STAT_SPEED, 1);
                BattleScriptCall(BattleScript_MoveEffectStatChange);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_SUNBLOOM:
            if (IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES)
             && TryChangeBattleWeather(cv->battlerAtk, BATTLE_WEATHER_SUN, ABILITY_NONE) == WEATHER_FAILURE_SUCCESS)
            {
                BattleScriptCall(BattleScript_MoveEffectSetWeather);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_SCALE_SHOT:
            if (IsBattlerAlive(cv->battlerAtk)
             && IsBattlerTurnDamaged(battlerDef, INCLUDING_SUBSTITUTES))
            {
                SetStatChange(cv->battlerAtk, STAT_DEF, -1);
                SetStatChange(cv->battlerAtk, STAT_SPEED, 1);
                BattleScriptCall(BattleScript_MoveEffectStatChange);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        case EFFECT_DEFOG:
            if (TryDefogClear(cv->battlerAtk, FALSE))
            {
                BattleScriptCall(BattleScript_Defog);
                gBattleStruct->eventState.moveEndBattler = 0;
                gBattleScripting.moveendState++;
                return MOVEEND_RESULT_RUN_SCRIPT;
            }
            break;
        default:
            result = MOVEEND_RESULT_CONTINUE;
            break;
        }
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndItemEffectsAttacker2(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (ItemBattleEffects(cv->battlerAtk, cv->battlerDef, cv->holdEffects[cv->battlerAtk], IsOnStatusChangeActivation)
     || ItemBattleEffects(cv->battlerAtk, cv->battlerDef, cv->holdEffects[cv->battlerAtk], IsOnHpThresholdActivation))
        result = MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndAbilityEffectFoesFainted(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (AbilityBattleEffects(ABILITYEFFECT_MOVE_END_FOES_FAINTED, cv->battlerAtk, cv->abilities[cv->battlerAtk], cv->move, TRUE))
        result = MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndShellTrap(struct BattleCalcValues *cv)
{
    u32 shellTrapBattlerMask = 0;

    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (battlerDef == cv->battlerAtk || IsBattlerAlly(battlerDef, cv->battlerAtk))
            continue;

        // Set ShellTrap to activate after the attacker's turn if target was hit by a physical move.
        if (GetMoveEffect(gChosenMoveByBattler[battlerDef]) == EFFECT_SHELL_TRAP
         && IsBattleMovePhysical(cv->move)
         && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
         && gProtectStructs[battlerDef].physicalBattlerId == cv->battlerAtk)
        {
            gProtectStructs[battlerDef].shellTrap = TRUE;
            shellTrapBattlerMask |= 1u << battlerDef;
        }
    }

    if (IsDoubleBattle() && shellTrapBattlerMask != 0)
    {
        enum BattlerId shellTrapBattlers[MAX_BATTLERS_COUNT];
        u32 numShellTrapBattlers = 0;

        for (u32 i = 0; i < gBattlersCount; i++)
        {
            enum BattlerId battler = gBattlersBySpeed[i];

            if (shellTrapBattlerMask & (1u << battler))
                shellTrapBattlers[numShellTrapBattlers++] = battler;
        }

        // Each reorder inserts the battler immediately after the attacker, so use reverse turn order to keep faster battlers first.
        while (numShellTrapBattlers != 0)
            ChangeOrderTargetAfterAttacker(shellTrapBattlers[--numShellTrapBattlers]);
    }

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndDeferredStatDrops(struct BattleCalcValues *cv)
{
    bool32 overwhelm = gBattleStruct->overwhelmTargets != 0;
    u32 targets = overwhelm ? gBattleStruct->overwhelmTargets : gBattleStruct->noblesseObligeTargets;
    gBattleStruct->overwhelmTargets = 0;
    gBattleStruct->noblesseObligeTargets = 0;
    gBattleScripting.moveendState++;

    // The hit already qualified while the attacker had its ability. Recoil or
    // contact punishment may have fainted it before this point.
    u32 originalPartyId = gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId;
    if (targets && !gSpecialStatuses[cv->battlerAtk].attackerInParty
     && (originalPartyId == PARTY_SIZE || originalPartyId == gBattlerPartyIndexes[cv->battlerAtk]))
    {
        bool32 changed = FALSE;
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
        {
            if (!(targets & (1u << target)) || !IsBattlerAlive(target))
                continue;
            SetStatChange(target, overwhelm ? STAT_ATK : STAT_DEF, overwhelm ? -1 : -2);
            SetStatChange(target, overwhelm ? STAT_SPATK : STAT_SPDEF, overwhelm ? -1 : -2);
            changed = TRUE;
        }
        if (changed)
        {
            gBattleScripting.battler = gBattlerAbility = cv->battlerAtk;
            gBattleScripting.abilityPopupOverwrite = overwhelm ? ABILITY_OVERWHELM : ABILITY_NOBLESSE_OBLIGE;
            BattleScriptCall(BattleScript_AbilityTargetStatChanges);
            return MOVEEND_RESULT_RUN_SCRIPT;
        }
    }
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndColorChange(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = (enum BattlerId)gBattleStruct->eventState.moveEndBattler++;

        if (battler == cv->battlerAtk)
            continue;
        SetTechnicolorTargetType(battler);
        if (AbilityBattleEffects(ABILITYEFFECT_COLOR_CHANGE, battler, cv->abilities[battler], 0, TRUE))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndKeeMarangaHpThresholdItemTarget(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battlerDef = (enum BattlerId)gBattleStruct->eventState.moveEndBattler++;

        if (battlerDef == cv->battlerAtk)
            continue;

        if (ItemBattleEffects(battlerDef, cv->battlerAtk, cv->holdEffects[battlerDef], IsKeeMarangaBerryActivation)
         || ItemBattleEffects(battlerDef, cv->battlerAtk, cv->holdEffects[battlerDef], IsOnHpThresholdActivation))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static bool32 HasAnyBattlerQueuedSwitch(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gSpecialStatuses[battler].queuedSwitch != NO_QUEUED_SWITCH)
            return TRUE;
    }
    return FALSE;
}

static bool32 TryRedCard(enum BattlerId battlerAtk, enum BattlerId redCardBattler, enum BattleMoveEffects moveEffect)
{
    if (((((!(IsBattlerAlive(redCardBattler)) || !(IsBattlerAlive(battlerAtk))) || (gBattleStruct->redCardActivated)) || !(IsBattlerTurnDamaged(redCardBattler, EXCLUDING_SUBSTITUTES))) || (moveEffect == EFFECT_FUTURE_SIGHT)) || !(HasBattlerViablePartyMonsForSwitch(battlerAtk)))
        return FALSE;

    gBattleStruct->redCardActivated = TRUE;
    gLastUsedItem = gBattleMons[redCardBattler].item;
    SaveBattlerTarget(redCardBattler); // save battler with red card
    SaveBattlerAttacker(battlerAtk);
    gBattleScripting.battler = gBattlerTarget = redCardBattler;
    gEffectBattler = battlerAtk;
    if (gBattleStruct->battlerState[battlerAtk].commanderSpecies != SPECIES_NONE
     || GetBattlerAbility(battlerAtk) == ABILITY_GUARD_DOG)
        BattleScriptCall(BattleScript_RedCardActivationNoSwitch);
    else
        BattleScriptCall(BattleScript_RedCardActivates);

    return TRUE;
}

static bool32 TryEjectButton(enum BattlerId battlerAtk, u32 ejectButtonBattler, enum BattleMoveEffects moveEffect)
{
    if (!IsBattlerTurnDamaged(ejectButtonBattler, EXCLUDING_SUBSTITUTES)
     || moveEffect == EFFECT_FUTURE_SIGHT
     || HasAnyBattlerQueuedSwitch()
     || IsBattlerInvolvedInSkyDrop(ejectButtonBattler)
     || IsPursuitTargetSet()
     || !CanBattlerSwitch(ejectButtonBattler))
        return FALSE;

    gBattleScripting.battler = ejectButtonBattler;
    gLastUsedItem = gBattleMons[ejectButtonBattler].item;
    gBattleStruct->battlerState[ejectButtonBattler].usedEjectItem = TRUE;
    gSpecialStatuses[ejectButtonBattler].queuedSwitch = QUEUED_SWITCH_OPEN_PARTY_SCREEN;
    BattleScriptCall(BattleScript_EjectItemActivates);
    gAiLogicData->ejectButtonSwitch = TRUE;
    return TRUE;
}

static enum MoveEndResult MoveEndCardButton(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = gBattlersByRawSpeed[gBattleStruct->eventState.moveEndBattler++];

        if (battler == gBattlerAttacker)
            continue;

        switch (cv->holdEffects[battler])
        {
        case HOLD_EFFECT_EJECT_BUTTON:
            if (TryEjectButton(cv->battlerAtk, battler, cv->moveEffect))
                return MOVEEND_RESULT_RUN_SCRIPT;
            break;
        case HOLD_EFFECT_RED_CARD:
            if (TryRedCard(cv->battlerAtk, battler, cv->moveEffect))
                return MOVEEND_RESULT_RUN_SCRIPT;
            break;
        default:
            break;
        }
    }

    gBattleStruct->redCardActivated = FALSE;
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndFormChange(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (IsSheerForceAffected(cv->move, cv->abilities[cv->battlerAtk]))
    {
        gBattleScripting.moveendState++;
        return result;
    }

    if (gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId == PARTY_SIZE
     && TryBattleFormChange(cv->battlerAtk, FORM_CHANGE_BATTLE_AFTER_MOVE, cv->abilities[cv->battlerAtk]))
    {
        result = MOVEEND_RESULT_RUN_SCRIPT;
        BattleScriptCall(BattleScript_AttackerFormChangeMoveEffect);
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndVampirism(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;
    enum BattlerId battler = cv->battlerAtk;

    enum Ability ability = cv->abilities[battler];
    u32 damage = ability == ABILITY_MAGLEV_POLARITY ? gBattleStruct->maglevPolarityDamage : gBattleStruct->accumulatedDamage;

    // Both accumulators contain actual HP lost, including substitutes and every
    // hit/target. Never heal a replacement or revive a user KO'd by a contact effect.
    if ((ability == ABILITY_VAMPIRISM || ability == ABILITY_MAGLEV_POLARITY)
     && damage > 0
     && !gBattleStruct->unableToUseMove
     && !gSpecialStatuses[battler].attackerInParty
     && gBattleStruct->battlerState[battler].originalBattlerPartyId == PARTY_SIZE
     && IsBattlerAlive(battler)
     && !IsBattlerAtMaxHp(battler)
     && !gBattleMons[battler].volatiles.healBlockTimer)
    {
        u32 divisor = ability == ABILITY_VAMPIRISM && gBattleMons[battler].hp * 2 < GetBattlerMaxHP(battler) ? 2 : 3;
        SetHealAmount(battler, damage / divisor);
        gBattleScripting.battler = gBattlerAbility = battler;
        RecordAbilityBattle(battler, ability);
        BattleScriptCall(BattleScript_AbilityHpHeal);
        result = MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndLifeOrbShellBell(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (GetConfig(B_SHEER_FORCE_TIMING) >= GEN_CHAMPIONS && IsSheerForceAffected(cv->move, cv->abilities[cv->battlerAtk]))
    {
        gBattleScripting.moveendState++;
        return result;
    }

    if (ItemBattleEffects(cv->battlerAtk, 0, cv->holdEffects[cv->battlerAtk], IsLifeOrbShellBellActivation))
        result = MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndEmergencyExit(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;
    u32 numEmergencyExitBattlers = 0;
    u32 emergencyExitBattlers = 0;

    if (HasAnyBattlerQueuedSwitch() && GetConfig(B_EMERGENCY_EXIT) < GEN_CHAMPIONS)
    {
        gBattleScripting.moveendState++;
        return result;
    }

    // Because sorting the battlers by speed takes lots of cycles,
    // we check if EE can be activated and count how many.
    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        if (!IsBattleMoveStatus(cv->move)
         && !gBattleStruct->unableToUseMove
         && EmergencyExitCanBeTriggered(i, cv->abilities[i]))
        {
            emergencyExitBattlers |= 1u << i;
            numEmergencyExitBattlers++;
        }
    }

    if (numEmergencyExitBattlers == 0)
    {
        gBattleScripting.moveendState++;
        return result;
    }

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
        gBattleMons[i].volatiles.tryEjectPack = FALSE;

    enum BattlerId battlers[MAX_BATTLERS_COUNT] = {0, 1, 2, 3};
    if (numEmergencyExitBattlers > 1)
        SortBattlersBySpeed(battlers, FALSE);

    for (u32 i = 0; i < gBattlersCount; i++)
    {
        enum BattlerId battler = battlers[i];

        if (!(emergencyExitBattlers & 1u << battler))
            continue;

        gBattleScripting.battler = battler;
        gSpecialStatuses[battler].queuedSwitch = QUEUED_SWITCH_OPEN_PARTY_SCREEN;
        BattleScriptCall(BattleScript_EmergencyExit);
        result = MOVEEND_RESULT_RUN_SCRIPT;
        if (GetConfig(B_EMERGENCY_EXIT) < GEN_CHAMPIONS)
            break; // Only the fastest Emergency Exit / Wimp Out activates
        else
            return result;
    }

    gBattleScripting.moveendState++;
    return result;
}

static bool32 CanPartingShotTrigger(enum BattlerId battlerAtk)
{
    if (GetConfig(B_PARTING_SHOT_SWITCH) < GEN_7 && CanBattlerSwitch(battlerAtk))
        return TRUE;

    if (gBattleStruct->allowPartingShot)
        return TRUE;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_STAT_CHANGED)
            return TRUE;
    }

    return FALSE;
}

static enum MoveEndResult MoveEndMoveSwitchUser(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (gBattleStruct->unableToUseMove)
    {
        gBattleStruct->allowPartingShot = FALSE;
        gBattleScripting.moveendState++;
        return result;
    }

    bool32 tunnelTrouble = IsTunnelTroubleMove(cv->abilities[cv->battlerAtk], cv->move, GetBattleMoveType(cv->move));
    enum BattleMoveEffects effect = tunnelTrouble ? EFFECT_HIT_ESCAPE : GetMoveEffect(cv->move);
    switch (effect)
    {
    case EFFECT_HIT_ESCAPE:
        if (!(HasAnyBattlerQueuedSwitch() && GetConfig(B_QUEUED_SWITCH_TIMINGS) < GEN_CHAMPIONS)
         && gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId == PARTY_SIZE
         && (tunnelTrouble && cv->isStatusMove ? IsAnyTargetAffected() : IsAnyTargetTurnDamaged(cv->battlerAtk, INCLUDING_SUBSTITUTES))
         && IsBattlerAlive(cv->battlerAtk)
         && CanBattlerSwitch(cv->battlerAtk)
         && !NoAliveMonsForBattlerSide(cv->battlerDef))
        {
            result = MOVEEND_RESULT_RUN_SCRIPT;
            gSpecialStatuses[cv->battlerAtk].queuedSwitch = QUEUED_SWITCH_OPEN_PARTY_SCREEN;
            if (tunnelTrouble)
            {
                gBattlerAbility = cv->battlerAtk;
                RecordAbilityBattle(cv->battlerAtk, ABILITY_TUNNEL_TROUBLE);
                BattleScriptCall(BattleScript_TunnelTroubleSwitch);
            }
            else
            {
                BattleScriptCall(BattleScript_EffectHitEscape);
            }
        }
        break;
    case EFFECT_PARTING_SHOT:
        if (CanPartingShotTrigger(cv->battlerAtk))
        {
            for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
                gBattleMons[battler].volatiles.tryEjectPack = FALSE;

            result = MOVEEND_RESULT_RUN_SCRIPT;
            BattleScriptCall(BattleScript_PartingShotEscape);
        }
        break;
    case EFFECT_SHED_TAIL:
        if (!IsBattlerUnaffectedByMove(cv->battlerAtk))
        {
            result = MOVEEND_RESULT_RUN_SCRIPT;
            BattleScriptCall(BattleScript_ShedTailSwitch);
        }
        break;
    case EFFECT_TELEPORT:
        if (!IsTrainerBattle() && !IsOnPlayerSide(cv->battlerAtk))
        {
            result = MOVEEND_RESULT_RUN_SCRIPT;
            BattleScriptCall(BattleScript_Teleport);
        }
        else
        {
            gBattleScripting.battler = cv->battlerAtk;
            result = MOVEEND_RESULT_RUN_SCRIPT;
            BattleScriptCall(BattleScript_BatonPass);
        }
        break;
    case EFFECT_BATON_PASS:
        gBattleScripting.battler = cv->battlerAtk;
        result = MOVEEND_RESULT_RUN_SCRIPT;
        BattleScriptCall(BattleScript_BatonPass);
        break;
    case EFFECT_HEALING_WISH:
    case EFFECT_LUNAR_DANCE:
        gBattleScripting.battler = cv->battlerAtk;
        result = MOVEEND_RESULT_RUN_SCRIPT;
        BattleScriptCall(BattleScript_EffectHealingWish);
        break;
    case EFFECT_WEATHER_AND_SWITCH:
        if (CanBattlerSwitch(cv->battlerAtk))
        {
            gBattleScripting.battler = cv->battlerAtk;
            result = MOVEEND_RESULT_RUN_SCRIPT;
            BattleScriptCall(BattleScript_MoveSwitchRet);
        }
        break;
    default:
        break;
    }

    gBattleStruct->allowPartingShot = FALSE;
    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndPickpocket(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;
    enum Item itemToSteal;
    enum Ability abilityAtk;
    enum HoldEffect holdEffectAtk;
    u32 originalAttackerPartyId = gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId;
    bool32 originalAttackerOnField = originalAttackerPartyId == PARTY_SIZE;

    if (gSpecialStatuses[cv->battlerAtk].attackerInParty)
    {
        gBattleScripting.moveendState++;
        return result;
    }

    if (originalAttackerOnField)
    {
        if (IsBattlerAlive(cv->battlerAtk) && !GetBattlerPartyState(cv->battlerAtk)->isKnockedOff) // Gen3 edge case where the knocked of item was not removed
        {
            itemToSteal = gBattleMons[cv->battlerAtk].item;
            holdEffectAtk = cv->holdEffects[cv->battlerAtk];
            abilityAtk = cv->abilities[cv->battlerAtk];
        }
        else
        {
            itemToSteal = ITEM_NONE;
            holdEffectAtk = HOLD_EFFECT_NONE;
            abilityAtk = ABILITY_NONE;
        }
    }
    else
    {
        itemToSteal = GetMonData(&gParties[GetBattlerTrainer(cv->battlerAtk)][originalAttackerPartyId], MON_DATA_HELD_ITEM);
        holdEffectAtk = HOLD_EFFECT_NONE;
        abilityAtk = ABILITY_NONE;
    }

    if (itemToSteal != ITEM_NONE) // valid item to steal
    {
        enum BattlerId battlers[MAX_BATTLERS_COUNT] = {0, 1, 2, 3};
        SortBattlersBySpeed(battlers, FALSE); // Pickpocket activates for fastest mon without item
        for (u32 i = 0; i < gBattlersCount; i++)
        {
            enum BattlerId battlerDef = battlers[i];
            if (battlerDef != cv->battlerAtk
              && !gBattleStruct->battlerState[battlerDef].notOnField
              && !IsBattlerUnaffectedByMove(battlerDef)
              && cv->abilities[battlerDef] == ABILITY_PICKPOCKET
              && IsMoveMakingContact(cv->battlerAtk, battlerDef, abilityAtk, holdEffectAtk, gCurrentMove)
              && IsBattlerTurnDamaged(battlerDef, EXCLUDING_SUBSTITUTES)
              && gBattleMons[battlerDef].item == ITEM_NONE
              && CanStealItem(battlerDef, cv->battlerAtk, itemToSteal))
            {
                gBattlerTarget = gBattlerAbility = battlerDef;
                gEffectBattler = cv->battlerAtk;
                // Battle scripting is super brittle so we shall do the item exchange now (if possible)
                if (abilityAtk != ABILITY_STICKY_HOLD)
                {
                    if (originalAttackerOnField)
                    {
                        StealTargetItem(battlerDef, cv->battlerAtk, ITEM_NONE, FALSE);  // Target takes attacker's item
                    }
                    else
                    {
                        struct LostItem *lostItem = GetLostItemState(GetBattlerTrainer(cv->battlerAtk), originalAttackerPartyId);
                        StealTargetItem(battlerDef, cv->battlerAtk, itemToSteal, FALSE); // Don't change cv->battlerAtk's item
                        PREPARE_MON_NICK_WITH_PREFIX_LOWER_BUFFER(gBattleTextBuff2, cv->battlerAtk, originalAttackerPartyId);

                        if (B_TRAINERS_KNOCK_OFF_ITEMS == TRUE
                         && gBattleTypeFlags & BATTLE_TYPE_TRAINER
                         && IsOnPlayerSide(cv->battlerAtk)
                         && itemToSteal == lostItem->originalItem)
                            lostItem->stolen = TRUE;
                        itemToSteal = ITEM_NONE;
                        BtlController_EmitSetMonData(
                            cv->battlerAtk,
                            B_COMM_TO_CONTROLLER,
                            REQUEST_HELDITEM_BATTLE,
                            1u << originalAttackerPartyId,
                            sizeof(itemToSteal),
                            &itemToSteal);
                        MarkBattlerForControllerExec(cv->battlerAtk);
                    }
                    BattleScriptCall(BattleScript_Pickpocket);
                }
                else
                {
                    BattleScriptCall(BattleScript_PickpocketPrevented);
                }
                result = MOVEEND_RESULT_RUN_SCRIPT;
                break; // Pickpocket activates on fastest mon, so exit loop.
            }
        }
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndItemsEffectsAll(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = (enum BattlerId)gBattleStruct->eventState.moveEndBattler++;

        if (ItemBattleEffects(battler, 0, cv->holdEffects[battler], IsOnStatusChangeActivation)
         || ItemBattleEffects(battler, 0, cv->holdEffects[battler], IsOnHpThresholdActivation))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndOpportunist(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = gBattlersByRawSpeed[gBattleStruct->eventState.moveEndBattler++];

        if (AbilityBattleEffects(ABILITYEFFECT_OPPORTUNIST, battler, cv->abilities[battler], 0, TRUE))
            return MOVEEND_RESULT_RUN_SCRIPT;
    }

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndThirdMoveBlock(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (gSpecialStatuses[cv->battlerAtk].greatMindsExplosion)
    {
        gSpecialStatuses[cv->battlerAtk].greatMindsExplosion = FALSE;
        if (IsBattlerAlive(cv->battlerAtk) && gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId == PARTY_SIZE)
        {
            BattleScriptCall(BattleScript_DeferredExplosion);
            gBattleScripting.moveendState++;
            return MOVEEND_RESULT_RUN_SCRIPT;
        }
    }

    switch (cv->moveEffect)
    {
    case EFFECT_STEEL_ROLLER:
        if (gFieldTimers.terrain != B_TERRAIN_NONE && IsAnyTargetTurnDamaged(cv->battlerAtk, INCLUDING_SUBSTITUTES))
        {
            BattleScriptCall(BattleScript_RemoveTerrain);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    case EFFECT_ICE_SPINNER:
        if (gFieldTimers.terrain != B_TERRAIN_NONE
         && IsAnyTargetTurnDamaged(cv->battlerAtk, INCLUDING_SUBSTITUTES))
        {
            if ((!IsBattlerAlive(cv->battlerAtk) || gLastPrintedMoves[cv->battlerAtk] != cv->move) && GetConfig(B_FAINT_MOVE_EFFECT_TIMING) < GEN_CHAMPIONS)
                break;

            BattleScriptCall(BattleScript_RemoveTerrain);
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    case EFFECT_NATURAL_GIFT:
        if (!gBattleStruct->unableToUseMove && GetItemPocket(gBattleMons[cv->battlerAtk].item) == POCKET_BERRIES)
        {
            enum Item item = gBattleMons[cv->battlerAtk].item;
            gBattleMons[gBattlerAttacker].item = ITEM_NONE;
            gBattleStruct->battlerState[cv->battlerAtk].canPickupItem = TRUE;
            GetBattlerPartyState(cv->battlerAtk)->usedHeldItem = item;
            CheckSetUnburden(cv->battlerAtk);
            BtlController_EmitSetMonData(
                cv->battlerAtk,
                B_COMM_TO_CONTROLLER,
                REQUEST_HELDITEM_BATTLE,
                0,
                sizeof(gBattleMons[cv->battlerAtk].item),
                &gBattleMons[cv->battlerAtk].item);
            MarkBattlerForControllerExec(cv->battlerAtk);
            ClearBattlerItemEffectHistory(cv->battlerAtk);

            if (!TrySymbiosis(cv->battlerAtk, item, NULL))
                result = MOVEEND_RESULT_RUN_SCRIPT;
        }
        break;
    default:
        break;
    }

    gBattleScripting.moveendState++;
    return result;
}

static inline bool32 TryEjectPack(enum BattlerId battlerAtk, enum BattlerId ejectPackBattler)
{
    if (!gBattleMons[ejectPackBattler].volatiles.tryEjectPack
     || HasAnyBattlerQueuedSwitch()
     || IsPursuitTargetSet()
     || !CanBattlerSwitch(ejectPackBattler)
     || IsBattlerInvolvedInSkyDrop(ejectPackBattler)
     || (GetMoveEffect(gCurrentMove) == EFFECT_PARTING_SHOT && CanBattlerSwitch(battlerAtk)))
        return FALSE;

    gBattleScripting.battler = ejectPackBattler;
    gLastUsedItem = gBattleMons[ejectPackBattler].item;
    gBattleStruct->battlerState[ejectPackBattler].usedEjectItem = TRUE;
    gSpecialStatuses[ejectPackBattler].queuedSwitch = QUEUED_SWITCH_OPEN_PARTY_SCREEN;
    BattleScriptCall(BattleScript_EjectItemActivates);
    gAiLogicData->ejectPackSwitch = TRUE;
    return TRUE;
}

static enum MoveEndResult MoveEndItemOnStatChange(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = gBattlersByRawSpeed[gBattleStruct->eventState.moveEndBattler++];

        switch (cv->holdEffects[battler])
        {
        case HOLD_EFFECT_WHITE_HERB:
            if (ItemBattleEffects(battler, 0, cv->holdEffects[battler], IsWhiteHerbActivation))
                return MOVEEND_RESULT_RUN_SCRIPT;
            break;
        case HOLD_EFFECT_EJECT_PACK:
            if (TryEjectPack(cv->battlerAtk, battler))
                return MOVEEND_RESULT_RUN_SCRIPT;
            break;
        case HOLD_EFFECT_MIRROR_HERB:
            if (ItemBattleEffects(battler, 0, cv->holdEffects[battler], IsMirrorHerbActivation))
                return MOVEEND_RESULT_RUN_SCRIPT;
            break;
        default:
            break;
        }
    }

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
        gBattleMons[i].volatiles.tryEjectPack = FALSE;

    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndSendOutReplacements(struct BattleCalcValues *cv)
{
    while (gBattleStruct->eventState.moveEndBattler < gBattlersCount)
    {
        enum BattlerId battler = gBattlersBySpeed[gBattleStruct->eventState.moveEndBattler++];

        switch (gSpecialStatuses[battler].queuedSwitch)
        {
        case NO_QUEUED_SWITCH:
            continue;
        case QUEUED_SWITCH_SEND_REPLACEMENT:
            gBattleScripting.battler = battler;
            BattleScriptCall(BattleScript_QueuedSwitch);
            return MOVEEND_RESULT_RUN_SCRIPT;
        case QUEUED_SWITCH_OPEN_PARTY_SCREEN:
            gBattleScripting.battler = battler;
            BattleScriptCall(BattleScript_QueuedSwitchOpenPartyScreen);
            return MOVEEND_RESULT_RUN_SCRIPT;
        default:
            errorf("Invalid value - queuedSwitch");
            break;
        }
    }
    gBattleStruct->eventState.moveEndBattler = 0;
    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndRampage(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (gBattleMons[cv->battlerAtk].volatiles.rampageTurns == 0
     || IsBattlerUsingAbilityMove(cv->battlerAtk)
     || B_RAMPAGE_CONFUSION < GEN_5)
    {
        result = MOVEEND_RESULT_CONTINUE;
    }
    else if (--gBattleMons[cv->battlerAtk].volatiles.rampageTurns == 0)
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        if (CanBeConfused(cv->battlerAtk, cv->battlerAtk))
        {
            gBattleScripting.battler = cv->battlerAtk;
            gBattleMons[cv->battlerAtk].volatiles.confusionTimer = RandomUniform(RNG_CONFUSION_TURNS, 2, B_CONFUSION_TURNS); // 2-5 turns
            BattleScriptCall(BattleScript_ConfusionAfterRampage);
            result = MOVEEND_RESULT_BREAK;
        }
    }
    else if (IsBattlerUnaffectedByMove(cv->battlerDef))
    {
        CancelMultiTurnMoves(cv->battlerAtk);
        result = MOVEEND_RESULT_CONTINUE;
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndConfusionAfterSkyDrop(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (gBattleMons[cv->battlerDef].volatiles.confuseAfterDrop
     && gBattleMons[cv->battlerDef].volatiles.semiInvulnerable != STATE_SKY_DROP_TARGET)
    {
        gBattleMons[cv->battlerDef].volatiles.confuseAfterDrop = FALSE;
        if (CanBeConfused(cv->battlerDef, cv->battlerDef))
        {
            gBattleScripting.battler = cv->battlerDef;
            gBattleMons[cv->battlerDef].volatiles.confusionTimer = RandomUniform(RNG_CONFUSION_TURNS, 2, B_CONFUSION_TURNS); // 2-5 turns
            BattleScriptCall(BattleScript_ConfusionAfterRampage);
            result = MOVEEND_RESULT_BREAK;
        }
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndSprayLeppaBlunder(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    // Throat Spray, Leppa Berry, Blunder Policy
    if (ItemBattleEffects(cv->battlerAtk, 0, cv->holdEffects[cv->battlerAtk], IsSprayLeppaBlunderActivation))
        result = MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return result;
}

static bool32 ShouldSetStompingTantrumTimer(void)
{
    u32 numNotAffectedTargets = 0;

    if (gBattleStruct->battlerState[gBattlerAttacker].originalBattlerPartyId != PARTY_SIZE) // no longer the same battler
        return FALSE;

    if (gBattleStruct->pledgeState == PLEDGE_COMBO_WAITING // Is the battler that uses the first Pledge move in the combo
     || gBattleStruct->unableToUseMove)
        return TRUE;

    if (!IsDoubleSpreadMove())
        return IsBattlerMoveResultSet(gBattlerTarget, (MOVE_RESULT_MISSED | MOVE_RESULT_FAILED | MOVE_RESULT_DOESNT_AFFECT_FOE));

    for (enum BattlerId battlerDef = 0; battlerDef < gBattlersCount; battlerDef++)
    {
        if (gBattlerAttacker == battlerDef)
            continue;
        if (IsBattlerMoveResultSet(battlerDef, (MOVE_RESULT_MISSED | MOVE_RESULT_FAILED | MOVE_RESULT_DOESNT_AFFECT_FOE)))
            numNotAffectedTargets++;
    }

    return numNotAffectedTargets == gBattleStruct->numSpreadTargets;
}

static enum MoveEndResult MoveEndClearBits(struct BattleCalcValues *cv)
{
    // go to next Bouncer or original attacker if possible
    if (gBattleStruct->bouncedMoveIsUsed)
    {
        RestoreAttacker();
        RestoreTarget();
        gBattleStruct->bouncedMoveIsUsed = FALSE;
        gBattleScripting.moveendState = MOVEEND_BOUNCED_MOVE;
        for (enum BattlerId battler = B_BATTLER_0; battler < gBattlersCount; battler++)
            gBattleStruct->moveResultFlags[battler] = gBattleStruct->savedMoveResultFlags[battler];
        return MOVEEND_RESULT_RUN_SCRIPT;
    }

    ValidateBattlers();

    gSpecialStatuses[cv->battlerAtk].multiHitTargets = 0;
    gSpecialStatuses[cv->battlerAtk].greatMindsActive = FALSE;
    gSpecialStatuses[cv->battlerAtk].reverberationsEcho = FALSE;

    enum Move originallyUsedMove = GetOriginallyUsedMove(gChosenMove);
    enum Type moveType = GetBattleMoveType(cv->move);

    if (cv->abilities[cv->battlerAtk] == ABILITY_AMPED_BLOOM
     && gCurrentTurnActionNumber < gBattlersCount
     && gBattlerByTurnOrder[gCurrentTurnActionNumber] == cv->battlerAtk
     && gBattlerAttacker == cv->battlerAtk
     && IsBattlerAlive(cv->battlerAtk)
     && !IsBattlerUsingAbilityMove(cv->battlerAtk)
     && !gBattleStruct->unableToUseMove
     && !gBattleStruct->snatchedMoveIsUsed
     && !gBattleMons[cv->battlerAtk].volatiles.chargeTurn
     && gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId == PARTY_SIZE)
    {
        struct SpecialStatus *status = &gSpecialStatuses[cv->battlerAtk];
        status->ampedBloomFollowUpType = GetAmpedBloomFollowUpType(moveType, GetWeather());
        status->ampedBloomPartySlot = gBattlerPartyIndexes[cv->battlerAtk];
    }

    ;

    if (!gSpecialStatuses[cv->battlerAtk].postActionAbilityMove && ShouldSetStompingTantrumTimer())
        gBattleStruct->battlerState[cv->battlerAtk].stompingTantrumTimer = 2;

    if (gSpecialStatuses[cv->battlerAtk].backUpTarget)
        gBattleStruct->moveTarget[cv->battlerAtk] = gSpecialStatuses[cv->battlerAtk].backUpTarget - 1;

    // If the Pokémon needs to keep track of move usage for its evolutions, do it
    if (originallyUsedMove != MOVE_NONE && !gSpecialStatuses[cv->battlerAtk].postActionAbilityMove)
        TryUpdateEvolutionTracker(IF_USED_MOVE_X_TIMES, 1, originallyUsedMove);

    if (GetBattlerIChooseYouBall(cv->battlerAtk, cv->abilities[cv->battlerAtk]) == BALL_REPEAT
     && !IsBattlerUsingAbilityMove(cv->battlerAtk)
     && gBattleStruct->battlerState[cv->battlerAtk].originalBattlerPartyId == PARTY_SIZE)
    {
        struct Volatiles *volatiles = &gBattleMons[cv->battlerAtk].volatiles;
        if (gBattleStruct->unableToUseMove)
        {
            volatiles->repeatBallMove = MOVE_NONE;
            volatiles->repeatBallUses = 0;
        }
        else
        {
            volatiles->repeatBallUses = volatiles->repeatBallMove == cv->move ? min(4, volatiles->repeatBallUses + 1) : 1;
            volatiles->repeatBallMove = cv->move;
        }
    }
    SetSameMoveTurnValues(cv->moveEffect);
    TryClearChargeVolatile(moveType);
    gProtectStructs[cv->battlerAtk].shellTrap = FALSE;
    gBattleStruct->battlerState[cv->battlerAtk].ateBoost = FALSE;
    gBattleScripting.moveEffect = MOVE_EFFECT_NONE;
    gBattleStruct->additionalEffectsCounter = 0;
    gBattleStruct->triAttackBurn = FALSE;
    gBattleStruct->fickleBeamBoosted = FALSE;
    gBattleStruct->battlerState[cv->battlerAtk].usedMicleBerry = FALSE;
    gBattleStruct->flingItem = ITEM_NONE;
    gBattleStruct->flungItem = FLUNG_ITEM_NONE;
    gBattleStruct->blunderPolicy = FALSE;
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;

    if (gBattleStruct->pledgeState == PLEDGE_COMBO_ATTACK)
        gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;

    ;

    if (gBattleMons[cv->battlerAtk].volatiles.destinyBond > 0)
        gBattleMons[cv->battlerAtk].volatiles.destinyBond--;

    // check if Stellar type boost should be used up
    if (GetActiveGimmick(cv->battlerAtk) == GIMMICK_TERA
     && GetBattlerTeraType(cv->battlerAtk) == TYPE_STELLAR
     && !IsBattleMoveStatus(cv->move)
     && IsTypeStellarBoosted(cv->battlerAtk, moveType))
        ExpendTypeStellarBoost(cv->battlerAtk, moveType);
    memset(gQueuedStatBoosts, 0, sizeof(gQueuedStatBoosts));

    for (enum BattlerId i = 0; i < gBattlersCount; i++)
    {
        gBattleStruct->battlerState[i].originalBattlerPartyId = PARTY_SIZE;
        gBattleStruct->battlerState[i].toxicChainActivates = FALSE;
        gBattleStruct->battlerState[i].substituteBlocked = FALSE;
        gBattleMons[i].volatiles.tryEjectPack = FALSE;

        if (gBattleStruct->battlerState[i].commanderSpecies != SPECIES_NONE && !IsBattlerAlive(i))
        {
            enum BattlerId partner = GetPartnerBattler(i);
            gBattleStruct->battlerState[i].commanderSpecies = SPECIES_NONE;
            if (gBattleMons[partner].volatiles.semiInvulnerable == STATE_COMMANDER)
                gBattleMons[partner].volatiles.semiInvulnerable = STATE_NONE;
        }
    }

    // Need to check a specific battler during the end turn
    if (!gSpecialStatuses[cv->battlerAtk].postActionAbilityMove)
        gBattleMons[cv->battlerAtk].volatiles.unableToUseMove = gBattleStruct->unableToUseMove;
    ClearDamageCalcResults();

    gBattleScripting.moveendState++;
    return MOVEEND_RESULT_CONTINUE;
}

static enum MoveEndResult MoveEndDancer(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    if (AbilityBattleEffects(ABILITYEFFECT_DANCER, cv->battlerAtk, ABILITY_DANCER, cv->move, TRUE))
        result = MOVEEND_RESULT_RUN_SCRIPT;

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult MoveEndPursuitNextAction(struct BattleCalcValues *cv)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    // A chasing move can itself pivot or retarget. Resume the interrupted
    // switch using its marker rather than the move's final scripting target.
    if (!gBattleStruct->battlerState[gBattlerTarget].pursuitTarget)
    {
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            if (gBattleStruct->battlerState[battler].pursuitTarget)
            {
                gBattlerTarget = battler;
                break;
            }
        }
    }

    if (gBattleStruct->battlerState[gBattlerTarget].pursuitTarget)
    {
        u32 storedTarget = gBattlerTarget;
        if (SetTargetToNextPursuiter(gBattlerTarget))
        {
            ChangeOrderTargetAfterAttacker(gBattlerTarget);
            gBattleStruct->moveTarget[gBattlerTarget] = storedTarget;
            gBattlerTarget = storedTarget;
        }
        else if (IsBattlerAlive(gBattlerTarget))
        {
            gBattlerAttacker = gBattlerTarget;
            if (gBattleStruct->pursuitStoredSwitch == PARTY_SLOT_NONE)
                BattleScriptCall(BattleScript_MoveSwitchOpenPartyScreen);
            else
                BattleScriptCall(BattleScript_DoSwitchOut);
            gBattleStruct->monToSwitchIntoId[gBattlerTarget] = gBattleStruct->pursuitStoredSwitch;
            ClearPursuitValues();
            result = MOVEEND_RESULT_RUN_SCRIPT;
        }
    }

    gBattleScripting.moveendState++;
    return result;
}

static enum MoveEndResult (*const sMoveEndHandlers[])(struct BattleCalcValues *cv) =
{
    [MOVEEND_SET_VALUES] = MoveEndSetValues,
    [MOVEEND_QUEUE_DANCER_TOXIC_CHAIN] = MoveEndQueueDancerToxicChain,
    [MOVEEND_SUBSTITUTE_BLOCK_ALLIED_SIDE] = MoveEndSubstituteBlock,
    [MOVEEND_MOVE_HEAVY_RECOIL] = MoveEndMoveHeavyRecoil,
    [MOVEEND_EFFECTIVENESS_MESSAGE_ALLIED_SIDE] = MoveEndEffectivenessMessage,
    [MOVEEND_CRIT_PROTECT_MESSAGE_ALLIED_SIDE] = MoveEndCritProtectMessage,
    [MOVEEND_ENDURE_DAMAGE_MESSAGE_ALLIED_SIDE] = MoveEndEndureDamageMessage,
    [MOVEEND_PROTECT_LIKE_EFFECTS_ALLIED_SIDE] = MoveEndProtectLikeEffect,
    [MOVEEND_MIME_BOX_ALLIED_SIDE] = MoveEndMimeBox,
    [MOVEEND_FLING_ITEM_REMOVAL] = MoveEndFlingItemRemoval,
    [MOVEEND_ADDITIONAL_EFFECTS_ALLIED_SIDE] = MoveEndAdditionalEffects,
    [MOVEEND_ABSORB_ALLIED_SIDE] = MoveEndAbsorb,
    [MOVEEND_STATUS_IMMUNITY_ABILITIES_ALLIED_SIDE] = MoveEndStatusImmunityAbilities,
    [MOVEEND_ATTACKER_INVISIBLE] = MoveEndAttackerInvisible,
    [MOVEEND_ATTACKER_VISIBLE] = MoveEndAttackerVisible,
    [MOVEEND_TARGET_VISIBLE_ALLIED_SIDE] = MoveEndTargetVisible,
    [MOVEEND_SCRAP_BRAWLER_ALLIED_SIDE] = MoveEndScrapBrawler,
    [MOVEEND_DAMAGED_EFFECTS_BLOCK_ALLIED_SIDE] = MoveEndDamagedEffectsBlock,
    [MOVEEND_MENTAL_HERB_ALLIED_SIDE] = MoveEndMentalHerb,
    [MOVEEND_SYMBIOSIS_ALLIED_SIDE] = MoveEndSymbiosis,
    [MOVEEND_FAINT_BLOCK_ALLIED_SIDE] = MoveEndFaintBlock,
    [MOVEEND_FAINT_ATTACKER_ALLIED_SIDE] = MoveEndFaintAttacker,
    [MOVEEND_SET_VALUES_FOR_OPPOSING_SIDE] = MoveEndSetValuesForOpposingSide,
    [MOVEEND_SUBSTITUTE_BLOCK_OPPOSING_SIDE] = MoveEndSubstituteBlock,
    [MOVEEND_EFFECTIVENESS_MESSAGE_OPPOSING_SIDE] = MoveEndEffectivenessMessage,
    [MOVEEND_CRIT_PROTECT_MESSAGE_OPPOSING_SIDE] = MoveEndCritProtectMessage,
    [MOVEEND_ENDURE_DAMAGE_MESSAGE_OPPOSING_SIDE] = MoveEndEndureDamageMessage,
    [MOVEEND_PROTECT_LIKE_EFFECTS_OPPOSING_SIDE] = MoveEndProtectLikeEffect,
    [MOVEEND_MIME_BOX_OPPOSING_SIDE] = MoveEndMimeBox,
    [MOVEEND_ADDITIONAL_EFFECTS_OPPOSING_SIDE] = MoveEndAdditionalEffects,
    [MOVEEND_ADDITIONAL_EFFECTS_CHANGE_STATS_ATTACKER] = MoveEndAdditionalEffectsChangeStatsAttacker,
    [MOVEEND_APEX_TARGET_EFFECTS] = MoveEndApexTargetEffects,
    [MOVEEND_SAPPING_VINES] = MoveEndSappingVines,
    [MOVEEND_ABSORB_OPPOSING_SIDE] = MoveEndAbsorb,
    [MOVEEND_STATUS_IMMUNITY_ABILITIES_OPPOSING_SIDE] = MoveEndStatusImmunityAbilities,
    [MOVEEND_TARGET_VISIBLE_OPPOSING_SIDE] = MoveEndTargetVisible,
    [MOVEEND_SCRAP_BRAWLER_OPPOSING_SIDE] = MoveEndScrapBrawler,
    [MOVEEND_DAMAGED_EFFECTS_BLOCK_OPPOSING_SIDE] = MoveEndDamagedEffectsBlock,
    [MOVEEND_MENTAL_HERB_OPPOSING_SIDE] = MoveEndMentalHerb,
    [MOVEEND_SYMBIOSIS_OPPOSING_SIDE] = MoveEndSymbiosis,
    [MOVEEND_FAINT_BLOCK_OPPOSING_SIDE] = MoveEndFaintBlock,
    [MOVEEND_FAINT_ATTACKER_OPPOSING_SIDE] = MoveEndFaintAttacker,
    [MOVEEND_UPDATE_LAST_MOVES] = MoveEndUpdateLastMoves,
    [MOVEEND_MIRROR_MOVE] = MoveEndMirrorMove,
    [MOVEEND_BOUNCED_MOVE] = MoveEndBouncedMove,
    [MOVEEND_MULTIHIT_MOVE_BLOCK] = MoveEndMultihitMoveBlock,
    [MOVEEND_REVERBERATIONS] = MoveEndReverberations,
    [MOVEEND_DEFROST] = MoveEndDefrost,
    [MOVEEND_MOVE_BLOCK_RECOIL] = MoveEndMoveBlockRecoil,
    [MOVEEND_SHEER_FORCE] = MoveEndSheerForce,
    [MOVEEND_MOVE_BLOCK] = MoveEndMoveBlock,
    [MOVEEND_ITEM_EFFECTS_ATTACKER_2] = MoveEndItemEffectsAttacker2,
    [MOVEEND_ABILITY_EFFECT_FOES_FAINTED] = MoveEndAbilityEffectFoesFainted,
    [MOVEEND_SHELL_TRAP] = MoveEndShellTrap,
    [MOVEEND_COLOR_CHANGE] = MoveEndColorChange,
    [MOVEEND_DEFERRED_STAT_DROPS] = MoveEndDeferredStatDrops,
    [MOVEEND_KEE_MARANGA_HP_THRESHOLD_ITEM_TARGET] = MoveEndKeeMarangaHpThresholdItemTarget,
    [MOVEEND_CARD_BUTTON] = MoveEndCardButton,
    [MOVEEND_FORM_CHANGE] = MoveEndFormChange,
    [MOVEEND_VAMPIRISM] = MoveEndVampirism,
    [MOVEEND_LIFE_ORB_SHELL_BELL] = MoveEndLifeOrbShellBell,
    [MOVEEND_EMERGENCY_EXIT] = MoveEndEmergencyExit,
    [MOVEEND_MOVE_SWITCH_USER] = MoveEndMoveSwitchUser,
    [MOVEEND_PICKPOCKET] = MoveEndPickpocket,
    [MOVEEND_ITEMS_EFFECTS_ALL] = MoveEndItemsEffectsAll,
    [MOVEEND_OPPORTUNIST] = MoveEndOpportunist,
    [MOVEEND_THIRD_MOVE_BLOCK] = MoveEndThirdMoveBlock,
    [MOVEEND_RAMPAGE] = MoveEndRampage,
    [MOVEEND_CONFUSION_AFTER_SKY_DROP] = MoveEndConfusionAfterSkyDrop,
    [MOVEEND_SPRAY_LEPPA_BLUNDER] = MoveEndSprayLeppaBlunder,
    [MOVEEND_ITEM_ON_STAT_CHANGE] = MoveEndItemOnStatChange,
    [MOVEEND_SEND_OUT_REPLACEMENTS] = MoveEndSendOutReplacements,
    [MOVEEND_CLEAR_BITS] = MoveEndClearBits,
    [MOVEEND_DANCER] = MoveEndDancer,
    [MOVEEND_PURSUIT_NEXT_ACTION] = MoveEndPursuitNextAction,
};

enum MoveEndResult DoMoveEnd(enum MoveEndState endMode, enum MoveEndState endState)
{
    enum MoveEndResult result = MOVEEND_RESULT_CONTINUE;

    struct BattleCalcValues cv = {
        .battlerAtk = gBattlerAttacker,
        .battlerDef = gBattlerTarget,
        .move = gCurrentMove,
        .moveEffect = GetMoveEffect(gCurrentMove),
        .isStatusMove = IsBattleMoveStatus(gCurrentMove)
    };

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.abilities[battler] = GetBattlerAbility(battler);
        cv.holdEffects[battler] = GetBattlerHoldEffect(battler);
    }

    if (gBattleScripting.moveendState < MOVEEND_SET_VALUES_FOR_OPPOSING_SIDE)
        cv.battlerDef = cv.battlerAtk;
    else
        cv.battlerDef = GetBattlerLeftFoe(cv.battlerAtk);

    do
    {
        result = sMoveEndHandlers[gBattleScripting.moveendState](&cv);

        if (endMode == 1 && result == MOVEEND_RESULT_CONTINUE)
            gBattleScripting.moveendState = MOVEEND_COUNT;
        if (endMode == 2 && endState == gBattleScripting.moveendState)
            gBattleScripting.moveendState = MOVEEND_COUNT;

    } while (gBattleScripting.moveendState != MOVEEND_COUNT && result == MOVEEND_RESULT_CONTINUE);

    return result;
}

// ======================
// Stat Change Resolution
// ======================

static bool32 ShouldSkipStatChangeOnBattler(enum BattlerId battlerAtk, enum BattlerId battlerDef)
{
    if (battlerAtk == battlerDef
     && gBattlerAttacker == gBattlerTarget
     && !IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_DOESNT_AFFECT_FOE)) // for EFFECT_STAT_CHANGE_MAGNETIC
        return FALSE;

    if (IsBattlerMoveResultSet(battlerDef, MOVE_RESULT_INVALID_TARGET))
        return TRUE;

    return FALSE;
}

// Avoid erroneous, second failure message
static bool32 ShouldSkipStatChangeResolution(enum BattlerId battlerAtk)
{
    if (battlerAtk == gBattlerTarget)
        return FALSE;

    u32 numUnaffectedTargets = 0;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (IsBattlerMoveResultSet(battler, MOVE_RESULT_INVALID_TARGET))
            numUnaffectedTargets++;
    }

    return numUnaffectedTargets == gBattlersCount;
}

static enum MoveResult StatChangeSubstitute(struct BattleCalcValues *cv)
{
    if (ShouldSkipStatChangeResolution(cv->battlerAtk))
        return MOVE_RESULT_DONE;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, battler)
         || cv->battlerAtk == battler)
            continue;

        if (IsSubstituteProtected(cv->battlerAtk, battler, cv->abilities[cv->battlerAtk], cv->move))
            gBattleStruct->moveResultFlags[battler] = MOVE_RESULT_DOESNT_AFFECT_FOE;
    }

    return MOVE_RESULT_CONTINUE;
}

static void TryAcupressureStatChange(struct BattleCalcValues *cv)
{
    u32 bits = 0;

    if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, cv->battlerDef))
        return;

    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
    {
        if (CompareStat(cv->battlerDef, stat, MAX_STAT_STAGE, CMP_LESS_THAN, cv->abilities[cv->battlerDef]))
            bits |= 1u << stat;
    }

    if (bits)
    {
        enum Stat statId;
        do
        {
            statId = (Random() % (NUM_BATTLE_STATS - 1)) + 1;
        } while (!(bits & (1u << statId)));

        SetStatChange(cv->battlerDef, statId, 2);
        gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_ATTEMPT_STAT_CHANGE;
    }
    else
    {
        gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_DOESNT_AFFECT_FOE;
    }
}

static enum MoveResult StatChangeCanAnyChange(struct BattleCalcValues *cv)
{
    if (GetMoveEffect(cv->move) == EFFECT_ACUPRESSURE)
    {
        TryAcupressureStatChange(cv);
        return MOVE_RESULT_CONTINUE;
    }

    struct StatChange st = {
        .onlyChecking = TRUE,
        .ignoreCertainFailure = TRUE,
    };

    for (u32 slot = 0; slot < gBattlersCount; slot++)
    {
        cv->battlerDef = GetTargetBySlot(cv->battlerAtk, slot);

        if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, cv->battlerDef))
            continue;

        st.certain = cv->battlerAtk == cv->battlerDef;

        if (CanAnyStatChange(cv, &st))
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_ATTEMPT_STAT_CHANGE;
        else
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_STAT_CHANGE_PREVENTED;

        if (st.additionalEffectTriggers)
            gBattleStruct->moveResultFlags[cv->battlerDef] = MOVE_RESULT_ATTEMPT_STAT_CHANGE;
    }

    cv->battlerDef = gBattlerTarget;
    gBattleStruct->additionalEffectsCounter = 0;
    gBattleStruct->statChangeBattler = 0;

    return MOVE_RESULT_CONTINUE;
}

static enum MoveResult StatChangeAccuracy(struct BattleCalcValues *cv)
{
    if (gSpecialStatuses[cv->battlerAtk].reverberationsEcho)
        return MOVE_RESULT_CONTINUE;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, battler)
         || IsBattlerMoveResultSet(battler, MOVE_RESULT_STAT_CHANGE_PREVENTED)
         || cv->battlerAtk == battler)
            continue;

        cv->battlerDef = battler;
        if (DoesMoveMissTarget(cv))
        {
            if (cv->holdEffects[cv->battlerAtk] == HOLD_EFFECT_BLUNDER_POLICY)
                gBattleStruct->blunderPolicy = TRUE;
            gBattleStruct->moveResultFlags[battler] = MOVE_RESULT_MISSED;
        }
    }

    cv->battlerDef = gBattlerTarget;

    return MOVE_RESULT_CONTINUE;
}

static enum MoveResult StatChangeMirrorArmor(struct BattleCalcValues *cv)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, battler)
         || IsBattlerMoveResultSet(battler, (MOVE_RESULT_STAT_CHANGE_PREVENTED | MOVE_RESULT_MIRROR_ARMOR_PENDING))
         || cv->battlerAtk == battler)
            continue;

        if (cv->abilities[battler] == ABILITY_MIRROR_ARMOR)
            gBattleStruct->moveResultFlags[battler] = MOVE_RESULT_MIRROR_ARMOR_PENDING;
    }

    return MOVE_RESULT_CONTINUE;
}

static bool32 WillAnyStatChange(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (IsBattlerMoveResultSet(battler, MOVE_RESULT_ATTEMPT_STAT_CHANGE))
            return TRUE;
    }
    return FALSE;
}

static enum MoveResult StatChangeBeforeChange(struct BattleCalcValues *cv)
{
    switch (GetMoveEffect(cv->move))
    {
    default:
        if (WillAnyStatChange())
        {
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        break;
    case EFFECT_STUFF_CHEEKS:
        // Back up current queue for later use
        memcpy(
            gSpecialStatuses[cv->battlerAtk].statStageQueue2,
            gSpecialStatuses[cv->battlerAtk].statStageQueue,
            sizeof(gSpecialStatuses[cv->battlerAtk].statStageQueue)
        );
        memset(gSpecialStatuses[cv->battlerAtk].statStageQueue, 0, sizeof(gSpecialStatuses[cv->battlerAtk].statStageQueue));
        gSpecialStatuses[cv->battlerAtk].statStageAmount2 = gSpecialStatuses[cv->battlerAtk].statStageAmount;
        gSpecialStatuses[cv->battlerAtk].statStageAmount = 0;
        BattleScriptCall(BattleScript_StuffCheeks);
        return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
    case EFFECT_STOCKPILE:
        gBattleMons[cv->battlerAtk].volatiles.stockpileCounter++;
        PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff1, 1, gBattleMons[cv->battlerAtk].volatiles.stockpileCounter);
        BattleScriptCall(BattleScript_Stockpile);
        return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
    case EFFECT_DEFOG:
        if (WillAnyStatChange() || TryDefogClear(gBattlerAttacker, FALSE))
        {
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        break;
    case EFFECT_TIDY_UP:
        if (TryTidyUpClear(FALSE))
        {
            BattleScriptCall(BattleScript_PlayTidyUp);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        else if (WillAnyStatChange())
        {
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        break;
    case EFFECT_TAKE_HEART:
        if (WillAnyStatChange() || gBattleMons[cv->battlerAtk].status1 != 0)
        {
            u32 status = gBattleMons[cv->battlerAtk].status1;
            if (status & STATUS1_SLEEP)
                TryDeactivateSleepClause(cv->battlerAtk, gBattlerPartyIndexes[cv->battlerAtk]);

            if (status & STATUS1_PARALYSIS)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_PARALYSIS;
            else if (status & STATUS1_POISON || status & STATUS1_TOXIC_POISON)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_POISON;
            else if (status & STATUS1_BURN)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_BURN;
            else if (status & STATUS1_SLEEP)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_SLEEP;
            else if (status & STATUS1_FREEZE)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FREEZE;
            else if (status & STATUS1_FROSTBITE)
                gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_CURED_FROSTBITE;

            gBattleScripting.battler = cv->battlerAtk;
            gBattleMons[gBattlerAttacker].status1 = 0;
            BtlController_EmitSetMonData(cv->battlerAtk, B_COMM_TO_CONTROLLER, REQUEST_STATUS_BATTLE, 0, sizeof(gBattleMons[cv->battlerAtk].status1), &gBattleMons[cv->battlerAtk].status1);
            MarkBattlerForControllerExec(cv->battlerAtk);
            BattleScriptCall(BattleScript_TakeHeart);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        break;
    case EFFECT_MEMENTO:
        gBattleStruct->passiveHpUpdate[cv->battlerAtk] = gBattleMons[cv->battlerAtk].hp;
        BtlController_EmitHealthBarUpdate(cv->battlerAtk, B_COMM_TO_CONTROLLER, INSTANT_HP_BAR_DROP);
        MarkBattlerForControllerExec(cv->battlerAtk);
        BattleScriptCall(BattleScript_Memento);
        return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        break;
    case EFFECT_PARTING_SHOT:
        if ((GetConfig(B_PARTING_SHOT_SWITCH) < GEN_7 && CanBattlerSwitch(cv->battlerAtk))
         || WillAnyStatChange())
        {
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        break;
    case EFFECT_CLANGOROUS_SOUL:
        if (WillAnyStatChange() && CutThirdOfHp(cv->battlerAtk))
        {
            BattleScriptCall(BattleScript_PlayMoveAnimAndChangeHP);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        else
        {
            gBattlescriptCurrInstr = BattleScript_StatChangeFailed;
            return MOVE_RESULT_FAILURE;
        }
        break;
    case EFFECT_STAT_CHANGE_HALF_HP:
        if (WillAnyStatChange() && TryHalfHp(cv->battlerAtk))
        {
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        else
        {
            gBattlescriptCurrInstr = BattleScript_StatChangeFailed;
            return MOVE_RESULT_FAILURE;
        }
        break;
    case EFFECT_BELLY_DRUM:
        if (WillAnyStatChange() && TryBellyDrum(cv->battlerAtk))
        {
            BattleScriptCall(BattleScript_PlayMoveAnimAndChangeHP);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        else
        {
            gBattlescriptCurrInstr = BattleScript_StatChangeFailed;
            return MOVE_RESULT_FAILURE;
        }
        break;
    case EFFECT_CHARGE:
        if (WillAnyStatChange() || gBattleMons[cv->battlerAtk].volatiles.chargeTimer == 0)
        {
            gBattleMons[cv->battlerAtk].volatiles.chargeTimer = 2;
            BattleScriptCall(BattleScript_PlayMoveAnim);
            return MOVE_RESULT_RUN_SCRIPT_INCREMENT;
        }
        else
        {
            gBattlescriptCurrInstr = BattleScript_StatChangeFailed;
            return MOVE_RESULT_FAILURE;
        }
        break;
    }

    #define MOVE_RESULT_HANDLE_FAILURE (MOVE_RESULT_MISSED | MOVE_RESULT_MIRROR_ARMOR_PENDING | MOVE_RESULT_ATTEMPT_STAT_CHANGE | MOVE_RESULT_STAT_CHANGE_PREVENTED)
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_HANDLE_FAILURE)
            return MOVE_RESULT_CONTINUE;
    }
    #undef MOVE_RESULT_HANDLE_FAILURE

    gBattlescriptCurrInstr = BattleScript_ButItFailed;
    return MOVE_RESULT_FAILURE;
}

static enum MoveResult StatChangeTryChange(struct BattleCalcValues *cv)
{
    struct StatChange st = {
        .ignoreCertainFailure = TRUE,
    };

    while (gBattleStruct->statChangeBattler < gBattlersCount)
    {
        cv->battlerDef = GetTargetBySlot(cv->battlerAtk, gBattleStruct->statChangeBattler);

        if (IsBattlerMoveResultSet(cv->battlerDef, MOVE_RESULT_MISSED))
        {
            gBattleScripting.battler = cv->battlerDef;
            gBattleStruct->statChangeBattler++;
            BattleScriptCall(BattleScript_BattlerAvoidedAttack);
            return MOVE_RESULT_RUN_SCRIPT;
        }

        if (ShouldSkipStatChangeOnBattler(cv->battlerAtk, cv->battlerDef))
        {
            gBattleStruct->statChangeBattler++;
            continue;
        }

        st.certain = cv->battlerAtk == cv->battlerDef;

        bool32 runScript = FALSE;
        st.statStageQueue = gSpecialStatuses[cv->battlerDef].statStageQueue;
        st.statStageAmount = gSpecialStatuses[cv->battlerDef].statStageAmount;

        if (TryStatChange(cv, &st) != STAT_CHANGE_DIDNT_WORK)
            runScript = TRUE;

        if (st.nextBattler) // Or done
        {
            st.nextBattler = FALSE;
            gBattleStruct->negativeAnimPlayed = 0;
            gBattleStruct->positiveAnimPlayed = 0;
            gBattleStruct->statChangeBattler++;

            if (st.moveScript != NULL)
            {
                BattleScriptCall(st.moveScript);
                if (!runScript)
                    return MOVE_RESULT_RUN_SCRIPT;
            }
        }

        if (runScript)
        {
            BattleScriptCall(st.script);
            return MOVE_RESULT_RUN_SCRIPT;
        }
    }

    cv->battlerDef = gBattlerTarget;
    return MOVE_RESULT_DONE;
}

static enum MoveResult (*const sStatChangeHandlers[])(struct BattleCalcValues *cv) =
{
    [STAT_CHANGE_SUBSTITUTE] = StatChangeSubstitute,
    [STAT_CHANGE_CAN_ANY_CHANGE] = StatChangeCanAnyChange,
    [STAT_CHANGE_ACCURACY] = StatChangeAccuracy,
    [STAT_CHANGE_MIRROR_ARMOR] = StatChangeMirrorArmor,
    [STAT_CHANGE_BEFORE_CHANGE] = StatChangeBeforeChange,
    [STAT_CHANGE_TRY_CHANGE] = StatChangeTryChange,
};

enum MoveResult DoStatChange(struct BattleCalcValues *cv)
{
    enum MoveResult result = MOVE_RESULT_DONE;

    while (gBattleStruct->eventState.resolution < STAT_CHANGE_COUNT)
    {
        result = sStatChangeHandlers[gBattleStruct->eventState.resolution](cv);

        switch (result)
        {
        case MOVE_RESULT_CONTINUE:
            gBattleStruct->eventState.resolution++;
            break;
        case MOVE_RESULT_RUN_SCRIPT_INCREMENT:
            gBattleStruct->eventState.resolution++;
            return result;
        case MOVE_RESULT_RUN_SCRIPT:
            return result;
        case MOVE_RESULT_DONE:
        case MOVE_RESULT_FAILURE:
            gBattleStruct->eventState.resolution = 0;
            gBattleStruct->statChangeBattler = 0;
            ClearStatChangeValues();
            return result;
        }
    }

    return result;
}

// Utility functions

static void ValidateBattlers(void)
{
    assertf(gBattlerAttacker < MAX_BATTLERS_COUNT, "invalid gBattlerAttacker: %d\nmove: %S", gBattlerAttacker, GetMoveName(gCurrentMove));
    assertf(gBattlerTarget < MAX_BATTLERS_COUNT, "invalid gBattlerTarget: %d\nmove: %S", gBattlerTarget, GetMoveName(gCurrentMove));

    // More calls to SaveBattlerAttacker than RestoreBattlerAttacker.
    assertf(gBattleStruct->savedAttackerCount == 0, "savedAttackerCount is greater than 0");

    // More calls to SaveBattlerTarget than RestoreBattlerTarget.
    assertf(gBattleStruct->savedTargetCount == 0, "savedTargetCount is greater than 0");
}

static enum Move GetOriginallyUsedMove(enum Move chosenMove)
{
    return (gChosenMove == MOVE_UNAVAILABLE) ? MOVE_NONE : gChosenMove;
}

static void SetSameMoveTurnValues(enum BattleMoveEffects moveEffect)
{
    bool32 increment = IsAnyTargetAffected()
                    && !gBattleStruct->unableToUseMove
                    && gLastResultingMoves[gBattlerAttacker] == gCurrentMove;

    switch (moveEffect)
    {
    case EFFECT_FURY_CUTTER:
        if (increment && gBattleMons[gBattlerAttacker].volatiles.furyCutterCounter < 5)
            gBattleMons[gBattlerAttacker].volatiles.furyCutterCounter++;
        else
            gBattleMons[gBattlerAttacker].volatiles.furyCutterCounter = 0;
        break;
    case EFFECT_ROLLOUT:
        if (increment && ++gBattleMons[gBattlerAttacker].volatiles.rolloutTimer < 5)
        {
            gBattleMons[gBattlerAttacker].volatiles.multipleTurns = TRUE;
            gLockedMoves[gBattlerAttacker] = gCurrentMove;
        }
        else
        {
            gBattleMons[gBattlerAttacker].volatiles.multipleTurns = FALSE;
            gBattleMons[gBattlerAttacker].volatiles.rolloutTimer = 0;
        }
        break;
    case EFFECT_ECHOED_VOICE:
        if (!gBattleStruct->unableToUseMove) // Increment even if targets unaffected
            gBattleStruct->incrementEchoedVoice = TRUE;
        break;
    default: // not consecutive
        gBattleMons[gBattlerAttacker].volatiles.rolloutTimer = 0;
        gBattleMons[gBattlerAttacker].volatiles.furyCutterCounter = 0;
        break;
    }

    if (increment)
        gBattleMons[gBattlerAttacker].volatiles.metronomeItemCounter++;
    else
        gBattleMons[gBattlerAttacker].volatiles.metronomeItemCounter = 0;
}

static void TryClearChargeVolatile(enum Type moveType)
{
    if (B_CHARGE < GEN_9) // Prior to gen9, charge is cleared during the end turn
        return;

    if (moveType == TYPE_ELECTRIC && gBattleMons[gBattlerAttacker].volatiles.chargeTimer == 1)
        gBattleMons[gBattlerAttacker].volatiles.chargeTimer = 0;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (gBattleMons[battler].volatiles.chargeTimer == 2) // Has been set this turn by move or ability
            gBattleMons[battler].volatiles.chargeTimer--;
    }
}

static inline bool32 IsBattlerUsingBeakBlast(enum BattlerId battler)
{
    if (gChosenActionByBattler[battler] != B_ACTION_USE_MOVE)
        return FALSE;
    if (GetMoveEffect(gChosenMoveByBattler[battler]) != EFFECT_BEAK_BLAST)
        return FALSE;
    return !HasBattlerActedThisTurn(battler);
}

static void RequestNonVolatileChange(enum BattlerId battlerAtk)
{
    BtlController_EmitSetMonData(
        battlerAtk,
        B_COMM_TO_CONTROLLER,
        REQUEST_STATUS_BATTLE,
        0,
        sizeof(gBattleMons[battlerAtk].status1),
        &gBattleMons[battlerAtk].status1);
    MarkBattlerForControllerExec(battlerAtk);
}

static enum Move GetMirrorMoveMove(void)
{
    s32 i, validMovesCount;
    enum Move move = MOVE_NONE;
    enum Move validMoves[MAX_BATTLERS_COUNT] = {MOVE_NONE};

    for (validMovesCount = 0, i = 0; i < gBattlersCount; i++)
    {
        if (i != gBattlerAttacker)
        {
            move = gBattleStruct->lastTakenMoveFrom[gBattlerAttacker][i];
            if (move != MOVE_NONE && move != MOVE_UNAVAILABLE)
            {
                validMoves[validMovesCount] = move;
                validMovesCount++;
            }
        }
    }

    move = gBattleStruct->lastTakenMove[gBattlerAttacker];
    if ((move == MOVE_NONE || move == MOVE_UNAVAILABLE) && validMovesCount != 0)
        move = validMoves[Random() % validMovesCount];

    ;

    return move;
}

static bool32 InvalidMetronomeMove(u32 move)
{
    return GetMoveEffect(move) == EFFECT_PLACEHOLDER
        || IsMoveMetronomeBanned(move);
}

static enum Move GetMetronomeMove(void)
{
    enum Move move = MOVE_NONE;

#if B_METRONOME_MOVES >= GEN_9
    u32 moveCount = MOVES_COUNT;
#elif B_METRONOME_MOVES >= GEN_8
    u32 moveCount = MOVES_COUNT_GEN8;
#elif B_METRONOME_MOVES >= GEN_7
    u32 moveCount = MOVES_COUNT_GEN7;
#elif B_METRONOME_MOVES >= GEN_6
    u32 moveCount = MOVES_COUNT_GEN6;
#elif B_METRONOME_MOVES >= GEN_5
    u32 moveCount = MOVES_COUNT_GEN5;
#elif B_METRONOME_MOVES >= GEN_4
    u32 moveCount = MOVES_COUNT_GEN4;
#elif B_METRONOME_MOVES >= GEN_3
    u32 moveCount = MOVES_COUNT_GEN3;
#elif B_METRONOME_MOVES >= GEN_2
    u32 moveCount = MOVES_COUNT_GEN2;
#else
    u32 moveCount = MOVES_COUNT_GEN1;
#endif

    move = RandomUniformExcept(RNG_METRONOME, 1, moveCount - 1, InvalidMetronomeMove);
    return move;
}

static enum Move GetAssistMove(void)
{
    enum Move move = MOVE_NONE;
    u32 chooseableMovesNo = 0;
    struct Pokemon *party;
    u8 battlerByPartyId[PARTY_SIZE];
    enum Move validMoves[PARTY_SIZE * MAX_MON_MOVES] = {MOVE_NONE};

    party = GetBattlerParty(gBattlerAttacker);

    for (enum PartySlot i = PARTY_SLOT_0; i < PARTY_SIZE; i++)
        battlerByPartyId[i] = MAX_BATTLERS_COUNT;
    for (u32 battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlly(battler, gBattlerAttacker))
            continue;

        if (gBattlerPartyIndexes[battler] < PARTY_SIZE)
            battlerByPartyId[gBattlerPartyIndexes[battler]] = battler;
    }

    for (enum PartySlot monId = PARTY_SLOT_0; monId < PARTY_SIZE; monId++)
    {
        if (monId == gBattlerPartyIndexes[gBattlerAttacker])
            continue;
        if (GetMonData(&party[monId], MON_DATA_SPECIES_OR_EGG) == SPECIES_NONE)
            continue;
        if (GetMonData(&party[monId], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG)
            continue;

        for (u32 moveId = 0; moveId < MAX_MON_MOVES; moveId++)
        {
            enum Move move;

            if (battlerByPartyId[monId] != MAX_BATTLERS_COUNT)
                move = gBattleMons[battlerByPartyId[monId]].moves[moveId];
            else
                move = GetMonData(&party[monId], MON_DATA_MOVE1 + moveId);

            if (IsMoveAssistBanned(move))
                continue;

            validMoves[chooseableMovesNo++] = move;
        }
    }

    if (chooseableMovesNo)
        move = validMoves[Random() % chooseableMovesNo];

    return move;
}

enum Move GetNaturePowerMove(void)
{
    enum Move move = gBattleEnvironmentInfo[gBattleEnvironment].naturePower;

    if (gFieldTimers.terrain != B_TERRAIN_NONE)
        move = gBattleTerrainInfo[gFieldTimers.terrain].naturePowerMove;
    else if (gBattleEnvironmentInfo[gBattleEnvironment].naturePower == MOVE_NONE)
        move = B_NATURE_POWER_MOVES >= GEN_4 ? MOVE_TRI_ATTACK : MOVE_SWIFT;

    return move;
}

static enum Move GetSleepTalkMove(void)
{
    enum Move move = MOVE_NONE;

    u32 i, unusableMovesBits = 0;
    enum MoveSlot movePosition;

    if (!IsAsleepOrComatose(gBattlerAttacker, GetBattlerAbility(gBattlerAttacker)))
        return move;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (IsMoveSleepTalkBanned(gBattleMons[gBattlerAttacker].moves[i])
            || gBattleMoveEffects[GetMoveEffect(gBattleMons[gBattlerAttacker].moves[i])].twoTurnEffect)
            unusableMovesBits |= (1 << (i));
    }

    unusableMovesBits = CheckMoveLimitations(gBattlerAttacker, unusableMovesBits, ~(MOVE_LIMITATION_PP | MOVE_LIMITATION_CHOICE_ITEM | MOVE_LIMITATION_DISABLED | MOVE_LIMITATION_UNUSABLE));
    if (unusableMovesBits == ALL_MOVES_MASK) // all 4 moves cannot be chosen
        return move;

    // Set Sleep Talk as used move, so it works with Last Resort.
    gBattleMons[gBattlerAttacker].volatiles.usedMoves |= 1u << gCurrMovePos;
    do
    {
        movePosition = (enum MoveSlot)MOD(Random(), MAX_MON_MOVES);
    } while ((1u << movePosition) & unusableMovesBits);

    move = gBattleMons[gBattlerAttacker].moves[movePosition];
    gCurrMovePos = movePosition;

    return move;
}

static enum Move GetCopycatMove(void)
{
    if (gLastUsedMove == MOVE_NONE || gLastUsedMove == MOVE_UNAVAILABLE || IsMoveCopycatBanned(gLastUsedMove))
        return MOVE_NONE;

    return gLastUsedMove;
}

static enum Move GetMeFirstMove(void)
{
    enum Move move = GetBattlerChosenMove(gBattlerTarget);

    if (IsBattleMoveStatus(move)
     || IsMoveMeFirstBanned(move)
     || HasBattlerActedThisTurn(gBattlerTarget))
        return MOVE_NONE;

    return move;
}

static bool32 CanBattlerBounceBackMove(struct BattleCalcValues *cv)
{
    return TryMagicBounce(cv) || TryMagicCoat(cv);
}

static bool32 TryMagicBounce(struct BattleCalcValues *cv)
{
    if (!MoveCanBeBouncedBack(cv->move))
        return FALSE;

    if (gBattleStruct->bouncedMoveIsUsed)
        return FALSE;

    if (cv->abilities[cv->battlerDef] != ABILITY_MAGIC_BOUNCE)
        return FALSE;

    gBattleStruct->magicBouncePending |= 1u << cv->battlerDef;
    return TRUE;
}

static bool32 TryMagicCoat(struct BattleCalcValues *cv)
{
    if (!MoveCanBeBouncedBack(cv->move))
        return FALSE;

    if (gBattleStruct->bouncedMoveIsUsed)
        return FALSE;

    if (!gProtectStructs[cv->battlerDef].bounceMove)
        return FALSE;

    gBattleStruct->magicCoatPending |= 1u << cv->battlerDef;
    return TRUE;
}

static bool32 TryActivatePowderStatus(enum Move move)
{
    if (!gBattleMons[gBattlerAttacker].volatiles.powder)
        return FALSE;
    if (GetBattleMoveType(move) == TYPE_FIRE)
        return TRUE;
    return FALSE;
}

static void CalculateMagnitudeDamage(void)
{
    u32 magnitude = RandomUniform(RNG_MAGNITUDE, 0, 99);

    if (magnitude < 5)
    {
        gBattleStruct->magnitudeBasePower = 10;
        magnitude = 4;
    }
    else if (magnitude < 15)
    {
        gBattleStruct->magnitudeBasePower = 30;
        magnitude = 5;
    }
    else if (magnitude < 35)
    {
        gBattleStruct->magnitudeBasePower = 50;
        magnitude = 6;
    }
    else if (magnitude < 65)
    {
        gBattleStruct->magnitudeBasePower = 70;
        magnitude = 7;
    }
    else if (magnitude < 85)
    {
        gBattleStruct->magnitudeBasePower = 90;
        magnitude = 8;
    }
    else if (magnitude < 95)
    {
        gBattleStruct->magnitudeBasePower = 110;
        magnitude = 9;
    }
    else
    {
        gBattleStruct->magnitudeBasePower = 150;
        magnitude = 10;
    }

    PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff1, 2, magnitude)
}

static void UpdateStallMons(struct BattleCalcValues *cv)
{
    if (IsBattlerTurnDamaged(cv->battlerDef, INCLUDING_SUBSTITUTES) || cv->isStatusMove)
        return;

    if (IsBattlerProtected(cv))
        return;

    enum MoveTarget target = GetBattlerMoveTargetType(cv->battlerAtk, cv->move);
    if (!IsDoubleBattle() || target == TARGET_SELECTED)
    {
        if (gSpecialStatuses[cv->battlerDef].updateStallMons)
            gAiBattleData->playerStallMons[gBattlerPartyIndexes[cv->battlerDef]]++;
    }
    //  Handling for moves that target multiple opponents in doubles not handled currently
}

// Move Stat Change Functions

static bool32 IsStatChangeMove(enum BattlerId battlerAtk, enum Move move)
{
    if (GetMoveEffect(move) == EFFECT_CURSE && IS_BATTLER_OF_TYPE(battlerAtk, TYPE_GHOST))
        return FALSE;

    if (GetMoveEffect(move) == EFFECT_ACUPRESSURE)
        return TRUE;

    u32 additionalEffectCount = GetMoveAdditionalEffectCount(move);
    for (u32 i = 0; i < additionalEffectCount; i++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(move, i);
        switch (additionalEffect->moveEffect)
        {
        case STAT_CHANGE_EFFECT_MINUS:
        case STAT_CHANGE_EFFECT_PLUS:
           return TRUE;
        default:
           return FALSE;
        }
    }
    return FALSE;
}

static bool32 TryBellyDrum(enum BattlerId battler)
{
    if (CompareStat(battler, STAT_ATK, MAX_STAT_STAGE, CMP_EQUAL, ABILITY_NONE))
        return FALSE;

    u32 halfHp = GetBattlerMaxHP(battler) / 2;

    if (halfHp == 0)
        halfHp = 1;

    if (gBattleMons[battler].hp > halfHp)
    {
        SetPassiveDamageAmount(battler, halfHp);
        return TRUE;
    }

    return FALSE;
}

static bool32 TryHalfHp(enum BattlerId battler)
{
    if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_STAT_CHANGE_PREVENTED)
        return FALSE;

    u32 halfHp = GetBattlerMaxHP(battler) / 2;

    if (halfHp == 0)
        halfHp = 1;

    if (gBattleMons[battler].hp > halfHp)
    {
        SetPassiveDamageAmount(battler, halfHp);
        return TRUE;
    }

    return FALSE;
}

static bool32 CutThirdOfHp(enum BattlerId battler)
{
    if (gBattleStruct->moveResultFlags[battler] & MOVE_RESULT_STAT_CHANGE_PREVENTED)
        return FALSE;

    u32 thirdHp = GetBattlerMaxHP(battler) / 3;

    if (thirdHp == 0)
        thirdHp = 1;

    if (gBattleMons[battler].hp > thirdHp)
    {
        SetPassiveDamageAmount(battler, thirdHp);
        return TRUE;
    }

    return FALSE;
}
