#include "global.h"
#include "battle.h"
#include "battle_ai_record.h"
#include "battle_multivolve.h"
#include "battle_util.h"
#include "data.h"
#include "event_data.h"
#include "link.h"
#include "random.h"

enum MultivolveBadgeTier
{
    MULTIVOLVE_BADGES_0,
    MULTIVOLVE_BADGES_1,
    MULTIVOLVE_BADGES_2,
    MULTIVOLVE_BADGES_3,
    MULTIVOLVE_BADGES_4,
    MULTIVOLVE_BADGES_5,
    MULTIVOLVE_BADGES_6,
    MULTIVOLVE_BADGES_7,
    MULTIVOLVE_BADGES_8,
    MULTIVOLVE_BADGE_TIER_COUNT,
};

STATIC_ASSERT(MULTIVOLVE_BADGE_TIER_COUNT == NUM_BADGES + 1, MultivolveBadgeTierCount);

static const struct Trainer sMultivolveSets[DIFFICULTY_COUNT][MULTIVOLVE_BADGE_TIER_COUNT] =
{
    #include "data/eeveelutions.h"
};

static const struct Trainer *GetMultivolvePool(enum BattlerId battler)
{
    u32 badges;
    if (HasAnyBattleFlags(BATTLE_TYPE_LINK))
    {
        u32 id = HasAnyBattleFlags(BATTLE_TYPE_MULTI) ? battler : battler & BIT_SIDE;
        u32 player = GetBattlerMultiplayerId(id);
        badges = player < MAX_LINK_PLAYERS ? gLinkPlayers[player].badgeCount : 0;
    }
    else
    {
        badges = GetBadgeCount();
    }
    return &sMultivolveSets[DIFFICULTY_NORMAL][min(NUM_BADGES, badges)];
}

static u16 ScaleHp(u32 hp, u32 oldMax, u32 newMax)
{
    if (hp == 0)
        return 0;
    if (hp >= oldMax || newMax <= 1)
        return newMax;
    return min(newMax - 1, max(1, (hp * newMax + oldMax / 2) / oldMax));
}

static void RefreshStats(enum BattlerId battler, const struct TrainerMon *set)
{
    struct BattlePokemon *mon = &gBattleMons[battler];
    struct Pokemon *partyMon = GetBattlerMon(battler);
    struct PartyState *state = GetBattlerPartyState(battler);
    u16 partyHp = GetMonData(partyMon, MON_DATA_HP);
    u16 partyMaxHp = GetMonData(partyMon, MON_DATA_MAX_HP);
    // Trainer presets store EVs in Showdown order.
    const u8 *ev = set->ev;

    // Keep the last unrounded ratio when the party HP still matches our projection.
    if (state->multivolveMaxHp == 0 || ScaleHp(state->multivolveHp, state->multivolveMaxHp, partyMaxHp) != partyHp)
    {
        state->multivolveHp = partyHp;
        state->multivolveMaxHp = partyMaxHp;
    }

    mon->level = GetMonData(partyMon, MON_DATA_LEVEL);
    mon->experience = GetMonData(partyMon, MON_DATA_EXP);
    mon->maxHP = (2 * GetSpeciesBaseStat(set->species, STAT_HP) + 31 + ev[0] / 4) * mon->level / 100 + mon->level + 10;
    mon->hp = ScaleHp(state->multivolveHp, state->multivolveMaxHp, mon->maxHP);
    mon->attack = CalculateNonHpStat(GetSpeciesBaseStat(set->species, STAT_ATK), mon->level, ev[1], set->nature, STAT_ATK);
    mon->defense = CalculateNonHpStat(GetSpeciesBaseStat(set->species, STAT_DEF), mon->level, ev[2], set->nature, STAT_DEF);
    mon->spAttack = CalculateNonHpStat(GetSpeciesBaseStat(set->species, STAT_SPATK), mon->level, ev[3], set->nature, STAT_SPATK);
    mon->spDefense = CalculateNonHpStat(GetSpeciesBaseStat(set->species, STAT_SPDEF), mon->level, ev[4], set->nature, STAT_SPDEF);
    mon->speed = CalculateNonHpStat(GetSpeciesBaseStat(set->species, STAT_SPEED), mon->level, ev[5], set->nature, STAT_SPEED);
}

void ApplyMultivolve(enum BattlerId battler)
{
    struct BattlePokemon *mon = &gBattleMons[battler];
    const struct Trainer *pool = GetMultivolvePool(battler);
    const struct TrainerMon *set = &pool->party[RandomUniform(RNG_MULTIVOLVE, 0, pool->poolSize - 1)];

    mon->volatiles.transformedMonSpecies = mon->species;
    mon->volatiles.transformedMonPID = mon->personality;
    mon->volatiles.isTransformedMonShiny = mon->isShiny;
    mon->volatiles.transformed = TRUE;
    mon->volatiles.multivolved = TRUE;
    mon->species = set->species;
    mon->types[0] = GetSpeciesType(set->species, 0);
    mon->types[1] = GetSpeciesType(set->species, 1);
    mon->types[2] = TYPE_MYSTERY;
    mon->ability = GetSpeciesApexAbility(set->species);
    RefreshStats(battler, set);
    for (enum MoveSlot slot = 0; slot < MAX_MON_MOVES; slot++)
        SetBattleMonMoveSlot(mon, set->moves[slot], slot);

    RecordAbilityBattle(battler, mon->ability);
}

void RefreshMultivolveStats(enum BattlerId battler)
{
    const struct Trainer *pool = GetMultivolvePool(battler);
    for (u32 i = 0; i < pool->poolSize; i++)
    {
        if (pool->party[i].species == gBattleMons[battler].species)
        {
            RefreshStats(battler, &pool->party[i]);
            break;
        }
    }
}

u16 GetMultivolvePartyHp(enum BattlerId battler, u16 hp)
{
    struct PartyState *state = GetBattlerPartyState(battler);
    u16 formMaxHp = gBattleMons[battler].maxHP;

    if (ScaleHp(state->multivolveHp, state->multivolveMaxHp, formMaxHp) != hp)
    {
        state->multivolveHp = hp;
        state->multivolveMaxHp = formMaxHp;
    }
    return ScaleHp(state->multivolveHp, state->multivolveMaxHp, GetMonData(GetBattlerMon(battler), MON_DATA_MAX_HP));
}
