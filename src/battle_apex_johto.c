#include "global.h"
#include "battle.h"
#include "battle_multivolve.h"
#include "battle_apex_johto.h"
#include "battle_util.h"
#include "battle_stat_change.h"
#include "battle_scripts.h"
#include "random.h"

enum BattlerId GetCopyrightFraudTarget(enum BattlerId battler)
{
    enum BattlerId ally = GetPartnerBattler(battler);
    if (IsDoubleBattle() && IsBattlerPresent(ally))
        return ally;
    enum BattlerId foe = GetOppositeBattler(battler);
    if (IsBattlerPresent(foe))
        return foe;
    foe = GetPartnerBattler(foe);
    return IsDoubleBattle() && IsBattlerPresent(foe) ? foe : MAX_BATTLERS_COUNT;
}

bool32 ApplyCopyrightFraud(enum BattlerId battler, bool32 recordHistory)
{
    enum BattlerId target = GetCopyrightFraudTarget(battler);
    if (target >= gBattlersCount)
        return FALSE;
    struct BattlePokemon *mon = &gBattleMons[battler];
    const struct BattlePokemon *source = &gBattleMons[target];
    u32 hp = mon->hp, maxHP = mon->maxHP;
    mon->attack = source->attack;
    mon->defense = source->defense;
    mon->spAttack = source->spAttack;
    mon->spDefense = source->spDefense;
    mon->speed = source->speed;
    struct PartyState *state = GetBattlerPartyState(battler);
    // Keep the exact previous ratio when persistent HP still matches its
    // projection. Repeated entries must not gain or lose HP through rounding.
    if (state->multivolveMaxHp != 0
     && ScaleTemporaryBattleHp(state->multivolveHp, state->multivolveMaxHp, maxHP) == hp)
    {
        hp = state->multivolveHp;
        maxHP = state->multivolveMaxHp;
    }
    else if (recordHistory)
    {
        state->multivolveHp = hp;
        state->multivolveMaxHp = maxHP;
    }
    mon->maxHP = source->maxHP;
    mon->hp = ScaleTemporaryBattleHp(hp, maxHP, mon->maxHP);
    mon->volatiles.copyrightFraud = TRUE;
    return TRUE;
}

enum BattlerId GetLonLonBredTarget(enum BattlerId battler)
{
    enum BattlerId ally = GetPartnerBattler(battler);
    return IsDoubleBattle() && IsBattlerPresent(ally) ? ally : battler;
}

u32 GetEmblazenedBurnDamage(enum BattlerId battler)
{
    return max(1, GetBattlerMaxHP(battler) / 8) << gBattleMons[battler].volatiles.emblazenedStage;
}

static void PrepareNaughtyOrNice(enum BattlerId battler)
{
    gBattleStruct->naughtyNiceTargets = 0;
    gBattleStruct->naughtyNiceCursor = 0;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (target == battler || !IsBattlerPresent(target))
            continue;
        u32 fraction = 16 >> RandomUniform(RNG_NAUGHTY_OR_NICE, 0, 2);
        u32 amount = max(1, GetBattlerMaxHP(target) / fraction);
        if (IsBattlerAlly(battler, target))
        {
            if (IsBattlerAtMaxHp(target) || gBattleMons[target].volatiles.healBlockTimer)
                continue;
            SetHealAmount(target, amount);
        }
        else
        {
            if (HasMagicGuardAbility(GetBattlerAbility(target)))
                continue;
            SetPassiveDamageAmount(target, amount);
        }
        gBattleStruct->naughtyNiceTargets |= 1u << target;
    }
}

bool32 TryJohtoEntryAbility(enum BattlerId battler, enum Ability ability)
{
    const u8 *script = NULL;
    enum BattleSide side = GetBattlerSide(battler);
    switch (ability)
    {
    case ABILITY_THUNDER_RUNNER:
        return TryThunderRunnerTerrain(battler);
    case ABILITY_PURIFYING_NOMAD:
        if (PurifyingNomadHasStatus(battler))
            script = BattleScript_PurifyingNomad;
        break;
    case ABILITY_GOLDEN_RAINBOW:
        if (GetGoldenRainbowCandidates(battler))
            script = BattleScript_GoldenRainbow;
        break;
    case ABILITY_NAUGHTY_OR_NICE:
        PrepareNaughtyOrNice(battler);
        if (gBattleStruct->naughtyNiceTargets)
            script = BattleScript_NaughtyOrNice;
        break;
    case ABILITY_GRACEFUL_GLIDE:
        if (!(gSideStatuses[side] & SIDE_STATUS_TAILWIND))
        {
            gSideStatuses[side] |= SIDE_STATUS_TAILWIND;
            gSideTimers[side].tailwindTimer = 2;
            script = BattleScript_FangsFromAbove;
        }
        break;
    case ABILITY_UNSTOPPABLE_ROLL:
        if (gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
        {
            SetStatChange(battler, STAT_SPEED, MAX_STAT_STAGE - gBattleMons[battler].statStages[STAT_SPEED]);
            script = BattleScript_UnstoppableRoll;
        }
        break;
    case ABILITY_FOREST_WEIRDING:
        TryChangeBattleTerrain(battler, B_TERRAIN_GRASSY);
        if (gFieldTimers.terrain == B_TERRAIN_GRASSY)
            gFieldTimers.terrainTimer = gFieldTimers.terrainTimerTotal = 1;
        gFieldStatuses |= STATUS_FIELD_TRICK_ROOM;
        gFieldTimers.trickRoomTimer = 1;
        script = BattleScript_ForestWeirding;
        break;
    case ABILITY_COPYRIGHT_FRAUD:
        if (GetCopyrightFraudTarget(battler) < gBattlersCount)
            script = BattleScript_CopyrightFraud;
        break;
    case ABILITY_LON_LON_BRED:
    {
        enum BattlerId target = GetLonLonBredTarget(battler);
        if (GetBattlerPartyState(battler)->lonLonBredUses < 2
         && !IsBattlerAtMaxHp(target) && !gBattleMons[target].volatiles.healBlockTimer)
        {
            GetBattlerPartyState(battler)->lonLonBredUses++;
            gEffectBattler = target;
            SetHealAmount(target, max(1, GetBattlerMaxHP(target) / 2));
            script = BattleScript_HospitalityActivates;
        }
        break;
    }
    default:
        break;
    }
    if (script == NULL)
        return FALSE;
    gBattleScripting.battler = gBattlerAbility = battler;
    gLastUsedAbility = ability;
    BattleScriptCall(script);
    return TRUE;
}

bool32 TryThunderRunnerTerrain(enum BattlerId battler)
{
    if (gFieldTimers.terrain != B_TERRAIN_ELECTRIC || gBattleMons[battler].volatiles.terrainAbilityDone)
        return FALSE;
    gBattleMons[battler].volatiles.terrainAbilityDone = TRUE;
    bool32 changes = FALSE;
    const enum Stat stats[] = { STAT_SPEED, STAT_EVASION };
    for (u32 i = 0; i < ARRAY_COUNT(stats); i++)
        if (gBattleMons[battler].statStages[stats[i]] < MAX_STAT_STAGE)
        {
            SetStatChange(battler, stats[i], 1);
            changes = TRUE;
        }
    if (changes)
    {
        gBattleScripting.battler = gBattlerAbility = gEffectBattler = battler;
        gLastUsedAbility = ABILITY_THUNDER_RUNNER;
        BattleScriptCall(BattleScript_ThunderRunnerTerrain);
    }
    return changes;
}

bool32 PurifyingNomadHasStatus(enum BattlerId battler)
{
    for (u32 team = 0; team < (IsDoubleBattle() && HasPartnerTrainer(battler) ? 2u : 1u); team++)
    {
        struct Pokemon *party = GetBattlerParty(team ? GetPartnerBattler(battler) : battler);
        for (enum PartySlot slot = 0; slot < PARTY_SIZE; slot++)
        {
            enum Species species = GetMonData(&party[slot], MON_DATA_SPECIES_OR_EGG);
            if (species != SPECIES_NONE && species != SPECIES_EGG && GetMonData(&party[slot], MON_DATA_STATUS))
                return TRUE;
        }
    }
    return FALSE;
}

u32 GetGoldenRainbowCandidates(enum BattlerId battler)
{
    if (GetBattlerPartyState(battler)->goldenRainbowUsed)
        return 0;
    struct Pokemon *party = GetBattlerParty(battler);
    u32 choices = 0;
    for (enum PartySlot slot = 0; slot < PARTY_SIZE; slot++)
    {
        enum Species species = GetMonData(&party[slot], MON_DATA_SPECIES_OR_EGG);
        if (slot != gBattlerPartyIndexes[battler] && species != SPECIES_NONE && species != SPECIES_EGG
         && GetMonData(&party[slot], MON_DATA_HP) == 0)
            choices |= 1u << slot;
    }
    return choices;
}
