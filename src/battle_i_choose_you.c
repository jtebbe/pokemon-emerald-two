#include "global.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_gimmick.h"
#include "battle_i_choose_you.h"
#include "battle_message.h"
#include "battle_scripts.h"
#include "battle_stat_change.h"
#include "battle_util.h"
#include "rtc.h"
#include "global.fieldmap.h"
#include "constants/map_types.h"

static const s8 sBallEntryBoosts[POKEBALL_COUNT][NUM_BATTLE_STATS] = {
    [BALL_POKE] = { [STAT_ATK] = 2 },
    [BALL_GREAT] = { [STAT_DEF] = 2, [STAT_SPDEF] = 2 },
    [BALL_ULTRA] = { [STAT_SPATK] = 2 },
    [BALL_MASTER] = { [STAT_ATK] = 1, [STAT_DEF] = 1, [STAT_SPEED] = 1, [STAT_SPATK] = 1, [STAT_SPDEF] = 1 },
    [BALL_SAFARI] = { [STAT_EVASION] = 1 },
    [BALL_HEAVY] = { [STAT_ATK] = 3, [STAT_DEF] = 3, [STAT_SPATK] = 3, [STAT_SPDEF] = 3 },
    [BALL_FAST] = { [STAT_ATK] = 2, [STAT_SPATK] = 2 },
    [BALL_SPORT] = { [STAT_ACC] = 2 },
    [BALL_LUXURY] = { [STAT_ATK] = 1, [STAT_DEF] = 1, [STAT_SPEED] = 1, [STAT_SPATK] = 1, [STAT_SPDEF] = 1 },
    [BALL_QUICK] = { [STAT_SPEED] = 6 },
};

enum PokeBall GetBattlerIChooseYouBall(enum BattlerId battler, enum Ability ability)
{
    if (ability == ABILITY_I_CHOOSE_YOU)
        return GetMonData(GetBattlerMon(battler), MON_DATA_POKEBALL);
    return POKEBALL_COUNT;
}

bool32 HasComatoseAbility(enum BattlerId battler, enum Ability ability)
{
    return ability == ABILITY_COMATOSE || GetBattlerIChooseYouBall(battler, ability) == BALL_DREAM;
}

bool32 IsDuskBallBoosted(void)
{
    enum TimeOfDay time = GetTimeOfDay();
    return time == TIME_EVENING || time == TIME_NIGHT || gMapHeader.cave || gMapHeader.mapType == MAP_TYPE_UNDERGROUND;
}

static enum Ability GetEntryAbility(enum BattlerId battler, bool32 simulate)
{
    return simulate ? gAiLogicData->abilities[battler] : GetBattlerAbility(battler);
}

static u32 GetEntrySpeed(enum BattlerId battler, bool32 simulate)
{
    enum HoldEffect holdEffect = simulate ? gAiLogicData->holdEffects[battler] : GetBattlerHoldEffect(battler);
    return GetBattlerTotalSpeedStat(battler, GetEntryAbility(battler, simulate), holdEffect);
}

static bool32 ChangeBallEntryStat(enum BattlerId source, enum BattlerId target, enum Stat stat, s32 stages, bool32 simulate)
{
    enum Ability ability = GetEntryAbility(target, simulate);
    if (!CompareStat(target, stat, stages > 0 ? MAX_STAT_STAGE : MIN_STAT_STAGE,
        stages > 0 ? CMP_LESS_THAN : CMP_GREATER_THAN, ability))
        return FALSE;

    if (!simulate)
    {
        SetStatChange(target, stat, stages);
        return TRUE;
    }

    if (ability == ABILITY_CONTRARY)
        stages = -stages;
    if (stages < 0 && source != target)
    {
        if (gBattleMons[target].volatiles.substitute
         || (gSideStatuses[GetBattlerSide(target)] & SIDE_STATUS_MIST)
         || gAiLogicData->holdEffects[target] == HOLD_EFFECT_CLEAR_AMULET
         || (IS_BATTLER_OF_TYPE(target, TYPE_GRASS) && AI_IsAbilityOnSide(target, ABILITY_FLOWER_VEIL))
         || HasClearBodyAbility(ability) || ability == ABILITY_WHITE_SMOKE || ability == ABILITY_FULL_METAL_BODY
         || (ability == ABILITY_GLORIOUS_MANE && IsBattlerAtMaxHp(target))
         || (stat == STAT_ATK && (HasHyperCutterAbility(ability) || ability == ABILITY_PIERCING_BEAK)))
            return FALSE;
        if (ability == ABILITY_MIRROR_ARMOR)
            return ChangeBallEntryStat(target, source, stat, stages, TRUE);
        if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_WHITE_HERB)
            return FALSE;
        if (HasDefiantAbility(ability))
            gBattleMons[target].statStages[STAT_ATK] = min(MAX_STAT_STAGE, gBattleMons[target].statStages[STAT_ATK] + 2);
        if (ability == ABILITY_COMPETITIVE)
            gBattleMons[target].statStages[STAT_SPATK] = min(MAX_STAT_STAGE, gBattleMons[target].statStages[STAT_SPATK] + 2);
    }
    if (HasSimpleAbility(ability))
        stages *= 2;
    gBattleMons[target].statStages[stat] = min(MAX_STAT_STAGE, max(MIN_STAT_STAGE, gBattleMons[target].statStages[stat] + stages));
    return TRUE;
}

static bool32 FriendBallBlocks(enum Ability ability)
{
    return HasInnerFocusAbility(ability) || HasScrappyAbility(ability) || HasObliviousAbility(ability);
}

static const u8 *ApplyBallEntry(enum BattlerId battler, bool32 simulate)
{
    enum PokeBall ball = GetBattlerIChooseYouBall(battler, ABILITY_I_CHOOSE_YOU);
    enum BattlerId opposite = GetOppositeBattler(battler);
    enum BattlerId boostTarget = battler;
    bool32 changed = FALSE;

    switch (ball)
    {
    case BALL_FRIEND:
    case BALL_NET:
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
            if (!IsBattlerAlly(battler, foe) && IsBattlerAlive(foe)
             && (ball == BALL_NET || !FriendBallBlocks(GetEntryAbility(foe, simulate))))
                changed |= ChangeBallEntryStat(battler, foe, ball == BALL_FRIEND ? STAT_ATK : STAT_SPEED, -1, simulate);
        return changed ? BattleScript_IChooseYouStats : NULL;
    case BALL_DIVE:
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (IsBattlerAlly(battler, foe) || !IsBattlerAlive(foe) || GetActiveGimmick(foe) == GIMMICK_TERA)
                continue;
            if (gBattleMons[foe].types[0] == TYPE_WATER
             && gBattleMons[foe].types[1] == TYPE_WATER && gBattleMons[foe].types[2] == TYPE_MYSTERY)
                continue;
            gBattleMons[foe].types[0] = TYPE_WATER;
            gBattleMons[foe].types[1] = TYPE_WATER;
            gBattleMons[foe].types[2] = TYPE_MYSTERY;
            changed = TRUE;
        }
        return changed ? BattleScript_IChooseYouDive : NULL;
    case BALL_MOON:
    case BALL_PREMIER:
    {
        if (GetActiveGimmick(battler) == GIMMICK_TERA)
            return NULL;
        enum Type type = ball == BALL_MOON ? TYPE_FAIRY : TYPE_NORMAL;
        if (gBattleMons[battler].types[1] == type)
            return NULL;
        gBattleMons[battler].types[1] = type;
        if (!simulate)
            PREPARE_TYPE_BUFFER(gBattleTextBuff1, type);
        return BattleScript_IChooseYouType;
    }
    case BALL_LOVE:
        if (simulate || !IsBattlerAlive(opposite) || gBattleMons[opposite].volatiles.infatuation
         || HasObliviousAbility(GetBattlerAbility(opposite)) || IsAbilityOnSide(opposite, ABILITY_AROMA_VEIL))
            return NULL;
        gBattleMons[opposite].volatiles.infatuation = battler + 1;
        return BattleScript_IChooseYouLove;
    case BALL_TIMER:
        if (simulate)
            return NULL;
        gBattleMons[battler].volatiles.timerBallTurns = 3;
        return BattleScript_IChooseYouTimer;
    case BALL_DREAM:
        if (simulate)
            return NULL;
        gBattleCommunication[MULTISTRING_CHOOSER] = B_MSG_SWITCHIN_COMATOSE;
        return BattleScript_SwitchInAbilityMsg;
    case BALL_FAST:
        if (!IsBattlerAlive(opposite) && IsDoubleBattle())
            opposite = GetPartnerBattler(opposite);
        if (!IsBattlerAlive(opposite) || GetEntrySpeed(battler, simulate) <= GetEntrySpeed(opposite, simulate))
            return NULL;
        break;
    case BALL_LUXURY:
        if (!IsDoubleBattle() || !IsBattlerAlive(GetPartnerBattler(battler)) || GetBattlerPartyState(battler)->luxuryBallUsed)
            return NULL;
        boostTarget = GetPartnerBattler(battler);
        break;
    default:
        if (ball >= POKEBALL_COUNT)
            return NULL;
        break;
    }

    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        if (sBallEntryBoosts[ball][stat])
            changed |= ChangeBallEntryStat(battler, boostTarget, stat, sBallEntryBoosts[ball][stat], simulate);

    if (!changed)
        return NULL;
    if (ball == BALL_LUXURY)
    {
        if (!simulate)
            GetBattlerPartyState(battler)->luxuryBallUsed = TRUE;
        return BattleScript_NoblesseObligeEntry;
    }
    return BattleScript_IChooseYouStats;
}

bool32 TryIChooseYouEntry(enum BattlerId battler)
{
    const u8 *script = ApplyBallEntry(battler, FALSE);
    if (script == NULL)
        return FALSE;
    gBattlerAbility = gBattleScripting.battler = battler;
    gLastUsedAbility = ABILITY_I_CHOOSE_YOU;
    gEffectBattler = script == BattleScript_IChooseYouLove ? GetOppositeBattler(battler) : battler;
    BattleScriptCall(script);
    return TRUE;
}

void AI_ApplyIChooseYouEntry(enum BattlerId battler)
{
    ApplyBallEntry(battler, TRUE);
}

uq4_12_t GetIChooseYouDamageModifier(struct DamageContext *ctx)
{
    enum BattlerId attacker = ctx->battlerAtk;
    enum BattlerId defender = ctx->battlerDef;
    enum Species attackerSpecies = gBattleMons[attacker].species;
    enum Species defenderSpecies = gBattleMons[defender].species;

    if (ctx->isSelfInflicted)
        return UQ_4_12(1.0);

    switch (GetBattlerIChooseYouBall(attacker, ctx->abilities[attacker]))
    {
    case BALL_REPEAT:
        if (ctx->move == gBattleMons[attacker].volatiles.repeatBallMove)
            return UQ_4_12(1.0) + gBattleMons[attacker].volatiles.repeatBallUses * UQ_4_12(0.5);
        break;
    case BALL_NEST:
        if (GetSpeciesBaseStatTotal(defenderSpecies) < GetSpeciesBaseStatTotal(attackerSpecies))
            return UQ_4_12(2.0);
        break;
    case BALL_DUSK:
        if (IsDuskBallBoosted())
            return UQ_4_12(2.0);
        break;
    case BALL_PARK:
    {
        const enum EggGroup *attackerGroups = gSpeciesInfo[attackerSpecies].eggGroups;
        const enum EggGroup *defenderGroups = gSpeciesInfo[defenderSpecies].eggGroups;
        u32 matching = max((attackerGroups[0] == defenderGroups[0]) + (attackerGroups[1] == defenderGroups[1]),
                           (attackerGroups[0] == defenderGroups[1]) + (attackerGroups[1] == defenderGroups[0]));
        return UQ_4_12(2.0) - matching * UQ_4_12(0.5);
    }
    case BALL_DREAM:
        if (ctx->chosenMove == MOVE_SLEEP_TALK && ctx->move != MOVE_SLEEP_TALK)
            return UQ_4_12(2.0);
        break;
    case BALL_STRANGE:
    {
        u32 attackerWeight = GetBattlerWeight(attacker, ctx->abilities[attacker], ctx->holdEffects[attacker]);
        u32 defenderWeight = GetBattlerWeight(defender, ctx->abilities[defender], ctx->holdEffects[defender]);
        if (defenderWeight < attackerWeight)
            return UQ_4_12(0.5);
        if (defenderWeight * 2 <= attackerWeight * 3)
            return UQ_4_12(1.0);
        if (defenderWeight <= attackerWeight * 2)
            return UQ_4_12(1.5);
        return UQ_4_12(2.0);
    }
    default:
        break;
    }
    return UQ_4_12(1.0);
}
