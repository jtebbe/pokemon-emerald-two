#include "global.h"
#include "battle_setup.h"
#include "event_data.h"
#include "fieldmap.h"
#include "fishing.h"
#include "follower_npc.h"
#include "item.h"
#include "random.h"
#include "field_player_avatar.h"
#include "link.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "ow_abilities.h"
#include "pokemon.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "wild_encounter.h"
#include "battle_debug.h"
#include "constants/abilities.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/weather.h"

extern const u8 EventScript_SprayWoreOff[];

#define MAX_ENCOUNTER_RATE 2880

static void ApplyFluteEncounterRateMod(u32 *encRate);
static void ApplyCleanseTagEncounterRateMod(u32 *encRate);
static u8 GetMaxLevelOfSpeciesInWildTable(const struct WildPokemon *wildMon, enum Species species, enum WildPokemonArea area);
#ifdef BUGFIX
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex, u32 size);
#else
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex);
#endif

EWRAM_DATA static u8 sWildEncountersDisabled = 0;
EWRAM_DATA bool8 gIsFishingEncounter = 0;
EWRAM_DATA bool8 gIsSurfingEncounter = 0;
EWRAM_DATA u8 gChainFishingDexNavStreak = 0;

#include "data/wild_encounters.h"

// NUM_LAND_MONS_ENCOUNTER_SLOTS
u32 ChooseWildMonIndex_Land(void)
{
    u8 wildMonIndex = 0;
    bool8 swap = FALSE;
    u8 rand = Random() % ENCOUNTER_CHANCE_LAND_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_0)
        wildMonIndex = 0;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_1)
        wildMonIndex = 1;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_2)
        wildMonIndex = 2;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_3)
        wildMonIndex = 3;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_3 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_4)
        wildMonIndex = 4;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_4 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_5)
        wildMonIndex = 5;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_5 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_6)
        wildMonIndex = 6;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_6 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_7)
        wildMonIndex = 7;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_7 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_8)
        wildMonIndex = 8;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_8 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_9)
        wildMonIndex = 9;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_9 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_10)
        wildMonIndex = 10;
    else
        wildMonIndex = 11;

    if (LURE_STEP_COUNT != 0 && (Random() % 10 < 2))
        swap = TRUE;

    if (swap)
        wildMonIndex = 11 - wildMonIndex;

    return wildMonIndex;
}

// Mostly equivalent to ChooseWildMonIndex_Land
// NUM_LAND_MONS_ENCOUNTER_SLOTS
u8 GetLandEncounterSlotForMatchCall(void)
{
    int rand = Random() % ENCOUNTER_CHANCE_LAND_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_0)
        return 0;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_1)
        return 1;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_2)
        return 2;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_3)
        return 3;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_3 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_4)
        return 4;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_4 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_5)
        return 5;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_5 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_6)
        return 6;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_6 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_7)
        return 7;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_7 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_8)
        return 8;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_8 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_9)
        return 9;
    else if (rand >= ENCOUNTER_CHANCE_LAND_MONS_SLOT_9 && rand < ENCOUNTER_CHANCE_LAND_MONS_SLOT_10)
        return 10;
    else
        return 11;
}

// NUM_WATER_MONS_ENCOUNTER_SLOTS
u32 ChooseWildMonIndex_Water(void)
{
    u32 wildMonIndex = 0;
    bool8 swap = FALSE;
    u8 rand = Random() % ENCOUNTER_CHANCE_WATER_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_0)
        wildMonIndex = 0;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_1)
        wildMonIndex = 1;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_2)
        wildMonIndex = 2;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_3)
        wildMonIndex = 3;
    else
        wildMonIndex = 4;

    if (LURE_STEP_COUNT != 0 && (Random() % 10 < 2))
        swap = TRUE;

    if (swap)
        wildMonIndex = 4 - wildMonIndex;

    return wildMonIndex;
}

// Mostly equivalent to ChooseWildMonIndex_WaterRock
// NUM_WATER_MONS_ENCOUNTER_SLOTS
u8 GetWaterEncounterSlotForMatchCall(void)
{
    int rand = Random() % ENCOUNTER_CHANCE_WATER_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_0)
        return 0;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_1)
        return 1;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_2)
        return 2;
    else if (rand >= ENCOUNTER_CHANCE_WATER_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_WATER_MONS_SLOT_3)
        return 3;
    else
        return 4;
}

// NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS
u32 ChooseWildMonIndex_Rocks(void)
{
    u32 wildMonIndex = 0;
    bool8 swap = FALSE;
    u8 rand = Random() % ENCOUNTER_CHANCE_ROCK_SMASH_MONS_TOTAL;

    if (rand < ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_0)
        wildMonIndex = 0;
    else if (rand >= ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_1)
        wildMonIndex = 1;
    else if (rand >= ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_1 && rand < ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_2)
        wildMonIndex = 2;
    else if (rand >= ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_2 && rand < ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_3)
        wildMonIndex = 3;
    else
        wildMonIndex = 4;

    if (LURE_STEP_COUNT != 0 && (Random() % 10 < 2))
        swap = TRUE;

    if (swap)
        wildMonIndex = 4 - wildMonIndex;

    return wildMonIndex;
}

// NUM_FISHING_MONS_ENCOUNTER_SLOTS
static u32 ChooseWildMonIndex_Fishing(u8 rod)
{
    u8 wildMonIndex = 0;
    bool8 swap = FALSE;
    u8 rand = Random() % max(max(ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_TOTAL, ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_TOTAL),
                             ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_TOTAL);

    if (LURE_STEP_COUNT != 0 && (Random() % 10 < 2))
        swap = TRUE;

    switch (rod)
    {
    case OLD_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0)
            wildMonIndex = 0;
        else
            wildMonIndex = 1;

        if (swap)
            wildMonIndex = 1 - wildMonIndex;
        break;
    case GOOD_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2)
            wildMonIndex = 2;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2 && rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3)
            wildMonIndex = 3;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3 && rand < ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4)
            wildMonIndex = 4;

        if (swap)
            wildMonIndex = 6 - wildMonIndex;
        break;
    case SUPER_ROD:
        if (rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5)
            wildMonIndex = 5;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6)
            wildMonIndex = 6;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7)
            wildMonIndex = 7;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8)
            wildMonIndex = 8;
        if (rand >= ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8 && rand < ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9)
            wildMonIndex = 9;

        if (swap)
            wildMonIndex = 14 - wildMonIndex;
        break;
    }
    return wildMonIndex;
}

u8 ChooseWildMonLevel(const struct WildPokemon *wildPokemon, u8 wildMonIndex, enum WildPokemonArea area)
{
    u8 min;
    u8 max;
    u8 range;
    u8 rand;

    if (LURE_STEP_COUNT == 0)
    {
        // Make sure minimum level is less than maximum level
        if (wildPokemon[wildMonIndex].maxLevel >= wildPokemon[wildMonIndex].minLevel)
        {
            min = wildPokemon[wildMonIndex].minLevel;
            max = wildPokemon[wildMonIndex].maxLevel;
        }
        else
        {
            min = wildPokemon[wildMonIndex].maxLevel;
            max = wildPokemon[wildMonIndex].minLevel;
        }
        range = max - min + 1;
        rand = Random() % range;

        // check ability for max level mon
        if (!GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG))
        {
            enum Ability ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
            if (ability == ABILITY_HUSTLE || ability == ABILITY_VITAL_SPIRIT || ability == ABILITY_PRESSURE)
            {
                if (Random() % 2 == 0)
                    return max;

                if (rand != 0)
                    rand--;
            }
        }
        return min + rand;
    }
    else
    {
        // Looks for the max level of all slots that share the same species as the selected slot.
        max = GetMaxLevelOfSpeciesInWildTable(wildPokemon, wildPokemon[wildMonIndex].species, area);
        if (max > 0)
            return max + 1;
        else // Failsafe
            return wildPokemon[wildMonIndex].maxLevel + 1;
    }
}

u16 GetCurrentMapWildMonHeaderId(void)
{
    u16 i;

    for (i = 0; ; i++)
    {
        const struct WildPokemonHeader *wildHeader = &gWildMonHeaders[i];
        if (wildHeader->mapGroup == MAP_GROUP(MAP_UNDEFINED))
            break;

        if (gWildMonHeaders[i].mapGroup == gSaveData.location.mapGroup &&
            gWildMonHeaders[i].mapNum == gSaveData.location.mapNum)
        {

            return i;
        }
    }

    return HEADER_NONE;
}

enum TimeOfDay GetTimeOfDayForEncounters(u32 headerId, enum WildPokemonArea area)
{
    const struct WildPokemonInfo *wildMonInfo;
    enum TimeOfDay timeOfDay = GetTimeOfDay();

    if (!OW_TIME_OF_DAY_ENCOUNTERS)
        return TIME_OF_DAY_DEFAULT;

    ;

    switch (area)
    {
    default:
    case WILD_AREA_LAND:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo;
        break;
    case WILD_AREA_WATER:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;
        break;
    case WILD_AREA_ROCKS:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].rockSmashMonsInfo;
        break;
    case WILD_AREA_FISHING:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo;
        break;
    case WILD_AREA_HIDDEN:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].hiddenMonsInfo;
        break;
    }

    if (wildMonInfo == NULL && !OW_TIME_OF_DAY_DISABLE_FALLBACK)
        return OW_TIME_OF_DAY_FALLBACK;
    else
        return GenConfigTimeOfDay(timeOfDay);
}

static u8 PickWildMonNature(enum Species species)
{
    return GetSynchronizedNature(WILDMON_ORIGIN, species);
}

void CreateWildMon(enum Species species, u8 level)
{
    ZeroEnemyPartyMons();
    u32 personality = GetMonPersonality(species, GetSynchronizedGender(WILDMON_ORIGIN, species), PickWildMonNature(species), RANDOM_UNOWN_LETTER);
    CreateMonWithStats(&gParties[B_TRAINER_OPPONENT_A][0], species, level, personality, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);
}

#ifdef BUGFIX
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr, count)
#else
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr)
#endif

bool8 TryGenerateWildMon(const struct WildPokemonInfo *wildMonInfo, enum WildPokemonArea area, u8 flags)
{
    u8 wildMonIndex = 0;
    u8 level;

    switch (area)
    {
    case WILD_AREA_LAND:
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_STEEL, ABILITY_MAGNET_PULL, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_STATIC, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_LIGHTNING_ROD >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_LIGHTNING_ROD, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_FLASH_FIRE >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_FIRE, ABILITY_FLASH_FIRE, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_HARVEST >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_GRASS, ABILITY_HARVEST, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_STORM_DRAIN >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_WATER, ABILITY_STORM_DRAIN, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;

        wildMonIndex = ChooseWildMonIndex_Land();
        break;
    case WILD_AREA_WATER:
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_STEEL, ABILITY_MAGNET_PULL, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_STATIC, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_LIGHTNING_ROD >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_LIGHTNING_ROD, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_FLASH_FIRE >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_FIRE, ABILITY_FLASH_FIRE, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_HARVEST >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_GRASS, ABILITY_HARVEST, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_STORM_DRAIN >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_WATER, ABILITY_STORM_DRAIN, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;

        wildMonIndex = ChooseWildMonIndex_Water();
        break;
    case WILD_AREA_ROCKS:
        wildMonIndex = ChooseWildMonIndex_Rocks();
        break;
    default:
    case WILD_AREA_FISHING:
    case WILD_AREA_HIDDEN:
        break;
    }

    level = ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, area);
    if (flags & WILD_CHECK_REPEL && !IsWildLevelAllowedByRepel(level))
        return FALSE;
    if ((flags & WILD_CHECK_KEEN_EYE) && !((IsAbilityAllowingEncounter(level))))
        return FALSE;

    CreateWildMon(wildMonInfo->wildPokemon[wildMonIndex].species, level);
    return TRUE;
}

static u16 GenerateFishingWildMon(const struct WildPokemonInfo *wildMonInfo, u8 rod)
{
    u8 wildMonIndex = ChooseWildMonIndex_Fishing(rod);
    enum Species wildMonSpecies = wildMonInfo->wildPokemon[wildMonIndex].species;
    u8 level = ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, WILD_AREA_FISHING);

    UpdateChainFishingStreak();
    CreateWildMon(wildMonSpecies, level);
    return wildMonSpecies;
}

static bool8 EncounterOddsCheck(u16 encounterRate)
{
    if (Random() % MAX_ENCOUNTER_RATE < encounterRate)
        return TRUE;
    else
        return FALSE;
}

// Returns true if it will try to create a wild encounter.
static bool8 WildEncounterCheck(u32 encounterRate, bool8 ignoreAbility)
{
    encounterRate *= 16;
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
        encounterRate = encounterRate * 80 / 100;
    ApplyFluteEncounterRateMod(&encounterRate);
    ApplyCleanseTagEncounterRateMod(&encounterRate);
    if (LURE_STEP_COUNT != 0)
        encounterRate *= 2;
    if (!ignoreAbility && !GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG))
    {
        enum Ability ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);

        {
            if (ability == ABILITY_STENCH)
                encounterRate /= 2;
            else if (ability == ABILITY_ILLUMINATE)
                encounterRate *= 2;
            else if (ability == ABILITY_WHITE_SMOKE)
                encounterRate /= 2;
            else if (ability == ABILITY_ARENA_TRAP)
                encounterRate *= 2;
            else if (ability == ABILITY_SAND_VEIL && gSaveData.weather == WEATHER_SANDSTORM)
                encounterRate /= 2;
            else if (ability == ABILITY_SNOW_CLOAK && gSaveData.weather == WEATHER_SNOW)
                encounterRate /= 2;
            else if (ability == ABILITY_QUICK_FEET)
                encounterRate /= 2;
            else if (ability == ABILITY_INFILTRATOR && OW_INFILTRATOR >= GEN_8)
                encounterRate /= 2;
            else if (ability == ABILITY_NO_GUARD)
                encounterRate *= 2;
        }
    }
    if (encounterRate > MAX_ENCOUNTER_RATE)
        encounterRate = MAX_ENCOUNTER_RATE;
    return EncounterOddsCheck(encounterRate);
}

// When you first step on a different type of metatile, there's a 40% chance it
// skips the wild encounter check entirely.
static bool8 AllowWildCheckOnNewMetatile(void)
{
    if (Random() % 100 >= 60)
        return FALSE;
    else
        return TRUE;
}



bool8 StandardWildEncounter(u16 curMetatileBehavior, u16 prevMetatileBehavior)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum WildPokemonArea wildArea;
    enum TimeOfDay timeOfDay;
    const struct WildPokemonInfo *info;

    if (sWildEncountersDisabled || headerId == HEADER_NONE)
        return FALSE;

    if (MetatileBehavior_IsLandWildEncounter(curMetatileBehavior))
        wildArea = WILD_AREA_LAND;
    else if (MetatileBehavior_IsWaterWildEncounter(curMetatileBehavior)
          || (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING) && MetatileBehavior_IsBridgeOverWater(curMetatileBehavior)))
        wildArea = WILD_AREA_WATER;
    else
        return FALSE;

    timeOfDay = GetTimeOfDayForEncounters(headerId, wildArea);
    info = wildArea == WILD_AREA_LAND
        ? gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo
        : gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;
    if (info == NULL
     || (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
     || !WildEncounterCheck(info->encounterRate, FALSE)
     || !TryGenerateWildMon(info, wildArea, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE))
        return FALSE;

    gIsSurfingEncounter = wildArea == WILD_AREA_WATER;
    if (TryDoDoubleWildBattle())
    {
        struct Pokemon firstMon = gParties[B_TRAINER_OPPONENT_A][0];
        TryGenerateWildMon(info, wildArea, WILD_CHECK_KEEN_EYE);
        gParties[B_TRAINER_OPPONENT_A][1] = firstMon;
        BattleSetup_StartDoubleWildBattle();
    }
    else
    {
        BattleSetup_StartWildBattle();
    }
    return TRUE;
}

void RockSmashWildEncounter(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum TimeOfDay timeOfDay;

    if (headerId != HEADER_NONE)
    {
        timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_ROCKS);

        const struct WildPokemonInfo *wildPokemonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].rockSmashMonsInfo;

        if (wildPokemonInfo == NULL)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (WildEncounterCheck(wildPokemonInfo->encounterRate, TRUE) == TRUE
         && TryGenerateWildMon(wildPokemonInfo, WILD_AREA_ROCKS, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE) == TRUE)
        {
            if (TryDoDoubleWildBattle())
            {
                struct Pokemon mon1 = gParties[B_TRAINER_OPPONENT_A][0];
                TryGenerateWildMon(wildPokemonInfo, WILD_AREA_ROCKS, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE);
                gParties[B_TRAINER_OPPONENT_A][1] = mon1;
                BattleSetup_StartDoubleWildBattle();
                gSpecialVar_Result = TRUE;
            }
            else {
                BattleSetup_StartWildBattle();
                gSpecialVar_Result = TRUE;
            }
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

bool8 SweetScentWildEncounter(void)
{
    s16 x, y;
    u32 headerId;
    enum TimeOfDay timeOfDay;

    PlayerGetDestCoords(&x, &y);
    headerId = GetCurrentMapWildMonHeaderId();
    if (!(headerId == HEADER_NONE))
    {
        if (MetatileBehavior_IsLandWildEncounter(MapGridGetMetatileBehaviorAt(x, y)) == TRUE)
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo == NULL)
                return FALSE;

            TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, 0);

            BattleSetup_StartWildBattle();
            return TRUE;
        }
        else if (MetatileBehavior_IsWaterWildEncounter(MapGridGetMetatileBehaviorAt(x, y)) == TRUE)
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);

            if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo == NULL)
                return FALSE;

            TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo, WILD_AREA_WATER, 0);
            BattleSetup_StartWildBattle();
            return TRUE;
        }
    }

    return FALSE;
}

bool8 DoesCurrentMapHaveFishingMons(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum TimeOfDay timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_FISHING);

    if (headerId != HEADER_NONE && gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo != NULL)
        return TRUE;
    else
        return FALSE;
}

void FishingWildEncounter(u8 rod)
{
    u32 headerId;
    enum TimeOfDay timeOfDay;

    gIsFishingEncounter = TRUE;
    headerId = GetCurrentMapWildMonHeaderId();
    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_FISHING);
    GenerateFishingWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo, rod);
    BattleSetup_StartWildBattle();
}

u16 GetLocalWildMon(bool8 *isWaterMon)
{
    u32 headerId;
    enum TimeOfDay timeOfDay;
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;

    *isWaterMon = FALSE;
    headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return SPECIES_NONE;

    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);
    landMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo;

    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);
    waterMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;

    // Neither
    if (landMonsInfo == NULL && waterMonsInfo == NULL)
        return SPECIES_NONE;
    // Land Pokémon
    else if (landMonsInfo != NULL && waterMonsInfo == NULL)
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land()].species;
    // Water Pokémon
    else if (landMonsInfo == NULL && waterMonsInfo != NULL)
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water()].species;
    }
    // Either land or water Pokémon
    if ((Random() % 100) < 80)
    {
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land()].species;
    }
    else
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water()].species;
    }
}

u16 GetLocalWaterMon(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum TimeOfDay timeOfDay;

    if (headerId != HEADER_NONE)
    {
        timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);

        const struct WildPokemonInfo *waterMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;

        if (waterMonsInfo)
            return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water()].species;
    }
    return SPECIES_NONE;
}

bool8 UpdateRepelCounter(void)
{
    u16 repelLureVar = VarGet(VAR_REPEL_STEP_COUNT);
    u16 steps = REPEL_LURE_STEPS(repelLureVar);
    bool32 isLure = IS_LAST_USED_LURE(repelLureVar);

    ;

    if (steps != 0)
    {
        steps--;
        if (!isLure)
        {
            VarSet(VAR_REPEL_STEP_COUNT, steps);
            if (steps == 0)
            {
                ScriptContext_SetupScript(EventScript_SprayWoreOff);
                return TRUE;
            }
        }
        else
        {
            VarSet(VAR_REPEL_STEP_COUNT, steps | REPEL_LURE_MASK);
            if (steps == 0)
            {
                ScriptContext_SetupScript(EventScript_SprayWoreOff);
                return TRUE;
            }
        }

    }
    return FALSE;
}

bool8 IsWildLevelAllowedByRepel(u8 wildLevel)
{
    u8 i;

    if (!REPEL_STEP_COUNT)
        return TRUE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (I_REPEL_INCLUDE_FAINTED == GEN_1 || I_REPEL_INCLUDE_FAINTED >= GEN_6 || GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP))
        {
            if (!GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
                return wildLevel >= GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL);
        }
    }

    return FALSE;
}

bool8 IsAbilityAllowingEncounter(u8 level)
{
    enum Ability ability;

    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG))
        return TRUE;

    ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
    if (ability == ABILITY_KEEN_EYE || ability == ABILITY_INTIMIDATE)
    {
        u8 playerMonLevel = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL);
        if (playerMonLevel > 5 && level <= playerMonLevel - 5 && !(Random() % 2))
            return FALSE;
    }

    return TRUE;
}

static bool8 TryGetRandomWildMonIndexByType(const struct WildPokemon *wildMon, enum Type type, u8 numMon, u8 *monIndex)
{
    u8 validIndexes[numMon]; // variable length array, an interesting feature
    u8 i, validMonCount;

    for (i = 0; i < numMon; i++)
        validIndexes[i] = 0;

    for (validMonCount = 0, i = 0; i < numMon; i++)
    {
        if (GetSpeciesType(wildMon[i].species, 0) == type || GetSpeciesType(wildMon[i].species, 1) == type)
            validIndexes[validMonCount++] = i;
    }

    if (validMonCount == 0 || validMonCount == numMon)
        return FALSE;

    *monIndex = validIndexes[Random() % validMonCount];
    return TRUE;
}

#include "data.h"

static u8 GetMaxLevelOfSpeciesInWildTable(const struct WildPokemon *wildMon, enum Species species, enum WildPokemonArea area)
{
    u8 i, maxLevel = 0, numMon = 0;

    switch (area)
    {
    case WILD_AREA_LAND:
        numMon = NUM_LAND_MONS_ENCOUNTER_SLOTS;
        break;
    case WILD_AREA_WATER:
        numMon = NUM_WATER_MONS_ENCOUNTER_SLOTS;
        break;
    case WILD_AREA_ROCKS:
        numMon = NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS;
        break;
    default:
    case WILD_AREA_FISHING:
    case WILD_AREA_HIDDEN:
        break;
    }

    for (i = 0; i < numMon; i++)
    {
        if (wildMon[i].species == species && wildMon[i].maxLevel > maxLevel)
            maxLevel = wildMon[i].maxLevel;
    }

    return maxLevel;
}

#ifdef BUGFIX
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex, u32 size)
#else
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex)
#endif
{
    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG))
        return FALSE;
    else if (GetMonAbility(&gParties[B_TRAINER_PLAYER][0]) != ability)
        return FALSE;
    else if (Random() % 2 != 0)
        return FALSE;

#ifdef BUGFIX
    return TryGetRandomWildMonIndexByType(wildMon, type, size, monIndex);
#else
    return TryGetRandomWildMonIndexByType(wildMon, type, NUM_LAND_MONS_ENCOUNTER_SLOTS, monIndex);
#endif
}

static void ApplyFluteEncounterRateMod(u32 *encRate)
{
    if (FlagGet(FLAG_SYS_ENC_UP_ITEM) == TRUE)
        *encRate += *encRate / 2;
    else if (FlagGet(FLAG_SYS_ENC_DOWN_ITEM) == TRUE)
        *encRate = *encRate / 2;
}

static void ApplyCleanseTagEncounterRateMod(u32 *encRate)
{
    enum Item heldItem = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM);
    if (gItemsInfo[heldItem].holdEffect == HOLD_EFFECT_REPEL)
        *encRate = *encRate * 2 / 3;
}

bool8 TryDoDoubleWildBattle(void)
{
    if (GetSafariZoneFlag()
      || (WE_DOUBLE_WILD_REQUIRE_2_MONS && GetMonsStateToDoubles() != PLAYER_HAS_TWO_USABLE_MONS))
        return FALSE;
    if (FollowerNPCIsBattlePartner() && FNPC_FLAG_PARTNER_WILD_BATTLES != 0
     && (FNPC_FLAG_PARTNER_WILD_BATTLES == FNPC_ALWAYS || FlagGet(FNPC_FLAG_PARTNER_WILD_BATTLES)) && FNPC_NPC_FOLLOWER_WILD_BATTLE_VS_2 == TRUE)
        return TRUE;
    else if (FlagGet(WE_FLAG_FORCE_DOUBLE_WILD))
        return TRUE;
    else if (RandomPercentage(RNG_NONE, WE_DOUBLE_WILD_CHANCE))
        return TRUE;
    return FALSE;
}

u32 ChooseHiddenMonIndex(void)
{
    #ifdef ENCOUNTER_CHANCE_HIDDEN_MONS_TOTAL
        u8 rand = Random() % ENCOUNTER_CHANCE_HIDDEN_MONS_TOTAL;

        if (rand < ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_0)
            return 0;
        else if (rand >= ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_1)
            return 1;
        else
            return 2;
    #else
        return 0xFF;
    #endif
}

bool32 MapHasNoEncounterData(void)
{
    return (GetCurrentMapWildMonHeaderId() == HEADER_NONE);
}
