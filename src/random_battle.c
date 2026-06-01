#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "data.h"
#include "event_data.h"
#include "item.h"
#include "load_save.h"
#include "move.h"
#include "pokemon.h"
#include "random.h"
#include "random_battle.h"
#include "script.h"
#include "string_util.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_ai.h"
#include "constants/characters.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokeball.h"
#include "constants/species.h"
#include "constants/trainers.h"

#define RANDOM_BATTLE_LEVEL 100
#define RANDOM_BATTLE_POOL 0

enum RandomBattleOptionArg
{
    RBO_ARG_TRAINER_PIC,
    RBO_ARG_TRAINER_NAME,
    RBO_ARG_IS_DOUBLE,
    RBO_ARG_PRE_EVOS,
    RBO_ARG_NO_SPECIES_CLAUSE,
    RBO_ARG_NO_ITEM_CLAUSE,
    RBO_ARG_NO_TYPE_DIVERSITY,
    RBO_ARG_NO_LEGENDARIES,
    RBO_ARG_PRESELECTED_MOVES,
    RBO_ARG_ITEMS_OFF,
    RBO_ARG_ITEMS_PRESELECTED,
    RBO_ARG_TERA_TYPES_OFF,
    RBO_ARG_Z_CRYSTALS_OFF,
    RBO_ARG_MEGA_STONES_OFF,
};

static void RandomBattle_InitTrainer(const struct RandomBattleOptions *options);
static bool32 RandomBattle_Generate(struct TrainerMon *dst, u8 count, const struct RandomBattleOptions *options, bool32 fromPool);
static bool32 RandomBattle_SaveAndReplacePlayerParty(const struct TrainerMon *party, u8 count);
static void RandomBattle_CreatePokemonParty(struct Pokemon *party, const struct TrainerMon *partyData, u8 count);
static void RandomBattle_SetTrainerMonBasics(struct TrainerMon *mon, enum Species species);
static enum Species RandomBattle_PickSpecies(const struct RandomBattleOptions *options, const struct TrainerMon *selected, u8 count);
static bool32 RandomBattle_IsAllowedSpecies(enum Species species, const struct RandomBattleOptions *options, const struct TrainerMon *selected, u8 count);
static bool32 RandomBattle_IsNewType(enum Species species, const bool8 *usedTypes);
static void RandomBattle_UpdateUsedTypes(enum Species species, bool8 *usedTypes);
static void RandomBattle_GenerateMoves(struct TrainerMon *mon, const struct RandomBattleOptions *options);
static enum Move RandomBattle_PickTeachableMove(enum Species species, bool32 wantStatus, const enum Move *moves, u8 count, bool32 preselected);
static bool32 RandomBattle_SpeciesCanLearnMove(enum Species species, enum Move move);
static void RandomBattle_AssignItems(struct TrainerMon *party, u8 count, const struct RandomBattleOptions *options);
static void RandomBattle_AssignTeraTypes(struct TrainerMon *party, u8 count, const struct RandomBattleOptions *options);
static enum Item RandomBattle_PickGeneralItem(bool32 preselected, const enum Item *usedItems, u8 usedItemCount, bool32 noItemClause);
static bool32 RandomBattle_IsItemUsed(enum Item item, const enum Item *usedItems, u8 usedItemCount);
static enum Item RandomBattle_GetZCrystalForMove(enum Move move);
static enum Item RandomBattle_GetMegaStone(enum Species species);
static void RandomBattle_CopyTrainerName(const u8 *name);
static bool32 RandomBattle_SelectFromPool(struct TrainerMon *dst, u8 count, const struct RandomBattleOptions *options);
static u8 RandomBattle_GetPoolSize(void);
static const struct TrainerMon *RandomBattle_GetPoolParty(void);
static void RandomBattle_ReadCommonOptions(struct ScriptContext *ctx, struct RandomBattleOptions *options);
static u16 RandomBattle_ReadVarArg(struct ScriptContext *ctx);
static bool32 RandomBattle_RunFromScript(struct ScriptContext *ctx, bool32 fromPool);

static const struct Trainer sRandomBattlePoolTrainers[DIFFICULTY_COUNT][1] =
{
#include "data/random_battle_mons.h"
};

EWRAM_DATA static struct Pokemon sSavedPlayerParty[PARTY_SIZE] = {0};
EWRAM_DATA static u8 sSavedPlayerPartyCount = 0;
EWRAM_DATA static bool8 sSavedPlayerPartyValid = FALSE;
EWRAM_DATA static struct Trainer sRandomBattleTrainer = {0};
EWRAM_DATA static struct Trainer sRandomBattlePlayerTrainer = {0};
EWRAM_DATA static struct TrainerMon sRandomBattleOpponentMons[PARTY_SIZE] = {0};
EWRAM_DATA static struct TrainerMon sRandomBattlePlayerMons[PARTY_SIZE] = {0};
EWRAM_DATA static u8 sRandomBattleTrainerName[TRAINER_NAME_LENGTH + 1] = {0};

static const u8 sRandomTrainerNames[][TRAINER_NAME_LENGTH + 1] =
{
    _("ALEX"),
    _("BLAIR"),
    _("CASEY"),
    _("DREW"),
    _("JORDAN"),
    _("MORGAN"),
    _("QUINN"),
    _("RILEY"),
};

static const enum Move sPreselectedMoves[] =
{
    MOVE_PROTECT,
    MOVE_SWORDS_DANCE,
    MOVE_NASTY_PLOT,
    MOVE_CALM_MIND,
    MOVE_DRAGON_DANCE,
    MOVE_STEALTH_ROCK,
    MOVE_THUNDER_WAVE,
    MOVE_WILL_O_WISP,
    MOVE_RECOVER,
    MOVE_ROOST,
    MOVE_EARTHQUAKE,
    MOVE_FLAMETHROWER,
    MOVE_SURF,
    MOVE_THUNDERBOLT,
    MOVE_ICE_BEAM,
    MOVE_PSYCHIC,
    MOVE_SHADOW_BALL,
    MOVE_SLUDGE_BOMB,
    MOVE_DARK_PULSE,
    MOVE_DRAGON_PULSE,
    MOVE_MOONBLAST,
    MOVE_FLASH_CANNON,
    MOVE_STONE_EDGE,
    MOVE_CLOSE_COMBAT,
    MOVE_ENERGY_BALL,
};

static const enum Item sPreselectedItems[] =
{
    ITEM_CHOICE_BAND,
    ITEM_CHOICE_SCARF,
    ITEM_CHOICE_SPECS,
    ITEM_LEFTOVERS,
    ITEM_LIFE_ORB,
    ITEM_SITRUS_BERRY,
    ITEM_ASSAULT_VEST,
    ITEM_FOCUS_SASH,
    ITEM_EXPERT_BELT,
    ITEM_LUM_BERRY,
    ITEM_ROCKY_HELMET,
    ITEM_BLACK_SLUDGE,
};

bool32 IsRandomBattleTrainer(u16 trainerId)
{
    return trainerId == TRAINER_RANDOM_BATTLE;
}

const struct Trainer *RandomBattle_GetTrainerStruct(void)
{
    return &sRandomBattleTrainer;
}

bool32 RandomBattle_IsLegendarySpecies(enum Species species)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[species];

    return speciesInfo->isRestrictedLegendary
        || speciesInfo->isSubLegendary
        || speciesInfo->isMythical
        || speciesInfo->isUltraBeast
        || speciesInfo->isParadox;
}

bool32 RandomBattle_IsFullyEvolvedSpecies(enum Species species)
{
    const struct Evolution *evolutions = GetSpeciesEvolutions(species);

    if (evolutions == NULL)
        return TRUE;

    for (u32 i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
    {
        if (evolutions[i].method != EVO_NONE && IsSpeciesEnabled(evolutions[i].targetSpecies))
            return FALSE;
    }

    return TRUE;
}

void RandomBattle_RestorePlayerParty(void)
{
    if (!sSavedPlayerPartyValid)
        return;

    for (u32 i = 0; i < PARTY_SIZE; i++)
        CopyMon(&gParties[B_TRAINER_PLAYER][i], &sSavedPlayerParty[i], sizeof(struct Pokemon));

    gPartiesCount[B_TRAINER_PLAYER] = sSavedPlayerPartyCount;
    SavePlayerParty();
    sSavedPlayerPartyValid = FALSE;
}

bool32 RandomBattle_GenerateTrueBattle(const struct RandomBattleOptions *options)
{
    struct RandomBattleOptions resolved = *options;

    if (resolved.itemsOff)
    {
        resolved.zCrystalsOff = TRUE;
        resolved.megaStonesOff = TRUE;
    }

    if (!RandomBattle_Generate(sRandomBattlePlayerMons, resolved.numPlayerMons, &resolved, FALSE)
     || !RandomBattle_Generate(sRandomBattleOpponentMons, resolved.numOpponentMons, &resolved, FALSE))
        return FALSE;

    RandomBattle_InitTrainer(&resolved);
    return RandomBattle_SaveAndReplacePlayerParty(sRandomBattlePlayerMons, resolved.numPlayerMons);
}

bool32 RandomBattle_GeneratePoolBattle(const struct RandomBattleOptions *options)
{
    if (!RandomBattle_Generate(sRandomBattlePlayerMons, options->numPlayerMons, options, TRUE)
     || !RandomBattle_Generate(sRandomBattleOpponentMons, options->numOpponentMons, options, TRUE))
        return FALSE;

    RandomBattle_InitTrainer(options);
    return RandomBattle_SaveAndReplacePlayerParty(sRandomBattlePlayerMons, options->numPlayerMons);
}

static bool32 RandomBattle_Generate(struct TrainerMon *dst, u8 count, const struct RandomBattleOptions *options, bool32 fromPool)
{
    if (count < 1 || count > PARTY_SIZE)
        return FALSE;

    memset(dst, 0, sizeof(*dst) * PARTY_SIZE);
    if (fromPool)
        return RandomBattle_SelectFromPool(dst, count, options);

    for (u32 i = 0; i < count; i++)
    {
        enum Species species = RandomBattle_PickSpecies(options, dst, i);

        if (species == SPECIES_NONE)
            return FALSE;

        RandomBattle_SetTrainerMonBasics(&dst[i], species);
        RandomBattle_GenerateMoves(&dst[i], options);
    }

    RandomBattle_AssignItems(dst, count, options);
    RandomBattle_AssignTeraTypes(dst, count, options);
    return TRUE;
}

static void RandomBattle_InitTrainer(const struct RandomBattleOptions *options)
{
    memset(&sRandomBattleTrainer, 0, sizeof(sRandomBattleTrainer));
    memset(&sRandomBattlePlayerTrainer, 0, sizeof(sRandomBattlePlayerTrainer));

    RandomBattle_CopyTrainerName(options->trainerName);

    sRandomBattleTrainer.aiFlags = AI_FLAG_BASIC_TRAINER;
    sRandomBattleTrainer.party = sRandomBattleOpponentMons;
    sRandomBattleTrainer.trainerClass = TRAINER_CLASS_PKMN_TRAINER_1;
    sRandomBattleTrainer.encounterMusic = TRAINER_ENCOUNTER_MUSIC_MALE;
    sRandomBattleTrainer.gender = TRAINER_GENDER_MALE;
    sRandomBattleTrainer.battleType = options->isDouble ? TRAINER_BATTLE_TYPE_DOUBLES : TRAINER_BATTLE_TYPE_SINGLES;
    sRandomBattleTrainer.partySize = options->numOpponentMons;
    if (options->trainerPic == 0xFFFF)
        sRandomBattleTrainer.trainerPic = RandomUniform(RNG_NONE, TRAINER_PIC_HIKER, TRAINER_PIC_COUNT - 1);
    else if (options->trainerPic < TRAINER_PIC_COUNT)
        sRandomBattleTrainer.trainerPic = options->trainerPic;
    else
        sRandomBattleTrainer.trainerPic = TRAINER_PIC_HIKER;
    StringCopyN(sRandomBattleTrainer.trainerName, sRandomBattleTrainerName, TRAINER_NAME_LENGTH + 1);

    sRandomBattlePlayerTrainer.party = sRandomBattlePlayerMons;
    sRandomBattlePlayerTrainer.trainerClass = TRAINER_CLASS_PKMN_TRAINER_1;
    sRandomBattlePlayerTrainer.gender = TRAINER_GENDER_MALE;
    sRandomBattlePlayerTrainer.battleType = sRandomBattleTrainer.battleType;
    sRandomBattlePlayerTrainer.partySize = options->numPlayerMons;
}

static void RandomBattle_CopyTrainerName(const u8 *name)
{
    if (name == NULL)
    {
        u32 index = RandomUniform(RNG_NONE, 0, ARRAY_COUNT(sRandomTrainerNames) - 1);
        StringCopyN(sRandomBattleTrainerName, sRandomTrainerNames[index], TRAINER_NAME_LENGTH + 1);
    }
    else
    {
        StringCopyN(sRandomBattleTrainerName, name, TRAINER_NAME_LENGTH + 1);
    }
    sRandomBattleTrainerName[TRAINER_NAME_LENGTH] = EOS;
}

static bool32 RandomBattle_SaveAndReplacePlayerParty(const struct TrainerMon *party, u8 count)
{
    if (count < 1 || count > PARTY_SIZE)
        return FALSE;

    for (u32 i = 0; i < PARTY_SIZE; i++)
        CopyMon(&sSavedPlayerParty[i], &gParties[B_TRAINER_PLAYER][i], sizeof(struct Pokemon));

    sSavedPlayerPartyCount = gPartiesCount[B_TRAINER_PLAYER];
    sSavedPlayerPartyValid = TRUE;
    RandomBattle_CreatePokemonParty(gParties[B_TRAINER_PLAYER], party, count);
    gPartiesCount[B_TRAINER_PLAYER] = count;
    SavePlayerParty();
    return TRUE;
}

static void RandomBattle_CreatePokemonParty(struct Pokemon *party, const struct TrainerMon *partyData, u8 count)
{
    ZeroPartyMons(party);

    for (u32 i = 0; i < count; i++)
    {
        u32 personality = 0x88;
        struct OriginalTrainerId otId = OTID_STRUCT_RANDOM_NO_SHINY;
        s32 ball = partyData[i].ball;

        ModifyPersonalityForNature(&personality, partyData[i].nature);
        CreateMon(&party[i], partyData[i].species, partyData[i].lvl, personality, otId);
        SetMonData(&party[i], MON_DATA_HELD_ITEM, &partyData[i].heldItem);
        CustomTrainerPartyAssignMoves(&party[i], &partyData[i]);
        SetMonData(&party[i], MON_DATA_IVS, &partyData[i].iv);
        SetMonData(&party[i], MON_DATA_FRIENDSHIP, &partyData[i].friendship);
        SetMonData(&party[i], MON_DATA_POKEBALL, &ball);
        if (partyData[i].teraType > 0)
        {
            enum Type teraType = partyData[i].teraType;
            SetMonData(&party[i], MON_DATA_TERA_TYPE, &teraType);
        }
        CalculateMonStats(&party[i]);
    }
}

static void RandomBattle_SetTrainerMonBasics(struct TrainerMon *mon, enum Species species)
{
    memset(mon, 0, sizeof(*mon));
    mon->iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31);
    mon->species = species;
    mon->lvl = RANDOM_BATTLE_LEVEL;
    mon->ball = BALL_POKE;
    mon->friendship = MAX_FRIENDSHIP;
    mon->nature = NATURE_HARDY;
    mon->gender = TRAINER_MON_RANDOM_GENDER;
}

static enum Species RandomBattle_PickSpecies(const struct RandomBattleOptions *options, const struct TrainerMon *selected, u8 count)
{
    bool8 usedTypes[NUMBER_OF_MON_TYPES] = {0};
    enum Species fallback = SPECIES_NONE;

    for (u32 i = 0; i < count; i++)
        RandomBattle_UpdateUsedTypes(selected[i].species, usedTypes);

    for (u32 tries = 0; tries < NUM_SPECIES * 2; tries++)
    {
        enum Species species = RandomUniform(RNG_NONE, 1, NUM_SPECIES - 1);

        if (!RandomBattle_IsAllowedSpecies(species, options, selected, count))
            continue;
        if (fallback == SPECIES_NONE)
            fallback = species;
        if (options->noTypeDiversity || RandomBattle_IsNewType(species, usedTypes))
            return species;
    }

    if (!options->noTypeDiversity)
    {
        for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
        {
            if (RandomBattle_IsAllowedSpecies(species, options, selected, count)
             && RandomBattle_IsNewType(species, usedTypes))
                return species;
        }
    }

    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        if (RandomBattle_IsAllowedSpecies(species, options, selected, count))
            return species;
    }

    return fallback;
}

static bool32 RandomBattle_IsAllowedSpecies(enum Species species, const struct RandomBattleOptions *options, const struct TrainerMon *selected, u8 count)
{
    const struct SpeciesInfo *speciesInfo;
    enum Species baseSpecies;

    if (species == SPECIES_NONE || species == SPECIES_EGG || !IsSpeciesEnabled(species))
        return FALSE;

    speciesInfo = &gSpeciesInfo[species];
    if (speciesInfo->isMegaEvolution
     || speciesInfo->isGigantamax
     || speciesInfo->isTotem
     || speciesInfo->isUltraBurst
     || speciesInfo->cannotBeTraded
     || speciesInfo->isTeraForm
     || speciesInfo->isPrimalReversion)
        return FALSE;
    if (options->noLegendaries && RandomBattle_IsLegendarySpecies(species))
        return FALSE;
    if (!options->preEvos && !RandomBattle_IsFullyEvolvedSpecies(species))
        return FALSE;

    if (!options->noSpeciesClause)
    {
        baseSpecies = GET_BASE_SPECIES_ID(species);
        for (u32 i = 0; i < count; i++)
        {
            if (baseSpecies == GET_BASE_SPECIES_ID(selected[i].species))
                return FALSE;
        }
    }

    return TRUE;
}

static bool32 RandomBattle_IsNewType(enum Species species, const bool8 *usedTypes)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[species];

    for (u32 i = 0; i < ARRAY_COUNT(speciesInfo->types); i++)
    {
        enum Type type = speciesInfo->types[i];

        if (type < NUMBER_OF_MON_TYPES && type != TYPE_NONE && !usedTypes[type])
            return TRUE;
    }

    return FALSE;
}

static void RandomBattle_UpdateUsedTypes(enum Species species, bool8 *usedTypes)
{
    const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[species];

    for (u32 i = 0; i < ARRAY_COUNT(speciesInfo->types); i++)
    {
        enum Type type = speciesInfo->types[i];

        if (type < NUMBER_OF_MON_TYPES && type != TYPE_NONE)
            usedTypes[type] = TRUE;
    }
}

static void RandomBattle_GenerateMoves(struct TrainerMon *mon, const struct RandomBattleOptions *options)
{
    mon->moves[0] = RandomBattle_PickTeachableMove(mon->species, TRUE, mon->moves, 0, options->preselectedMoves);
    for (u32 i = 1; i < MAX_MON_MOVES; i++)
        mon->moves[i] = RandomBattle_PickTeachableMove(mon->species, FALSE, mon->moves, i, options->preselectedMoves);

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        if (mon->moves[i] == MOVE_NONE)
            mon->moves[i] = MOVE_TACKLE;
    }
}

static enum Move RandomBattle_PickTeachableMove(enum Species species, bool32 wantStatus, const enum Move *moves, u8 count, bool32 preselected)
{
    u16 candidates[MOVES_COUNT];
    u32 candidateCount = 0;

    if (preselected)
    {
        for (u32 i = 0; i < ARRAY_COUNT(sPreselectedMoves); i++)
        {
            enum Move move = sPreselectedMoves[i];

            if (RandomBattle_SpeciesCanLearnMove(species, move)
             && (wantStatus == (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS)))
            {
                bool32 duplicate = FALSE;

                for (u32 j = 0; j < count; j++)
                {
                    if (moves[j] == move)
                        duplicate = TRUE;
                }
                if (!duplicate)
                    candidates[candidateCount++] = move;
            }
        }
    }
    else
    {
        const u16 *learnset = GetSpeciesTeachableLearnset(species);

        for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
        {
            enum Move move = learnset[i];
            bool32 duplicate = FALSE;

            if (move == MOVE_NONE || move >= MOVES_COUNT)
                continue;
            if (wantStatus != (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS))
                continue;
            for (u32 j = 0; j < count; j++)
            {
                if (moves[j] == move)
                    duplicate = TRUE;
            }
            if (!duplicate)
                candidates[candidateCount++] = move;
        }
    }

    if (candidateCount == 0 && preselected)
    {
        for (u32 i = 0; i < ARRAY_COUNT(sPreselectedMoves); i++)
        {
            enum Move move = sPreselectedMoves[i];
            bool32 duplicate = FALSE;

            if (!RandomBattle_SpeciesCanLearnMove(species, move))
                continue;
            for (u32 j = 0; j < count; j++)
            {
                if (moves[j] == move)
                    duplicate = TRUE;
            }
            if (!duplicate)
                candidates[candidateCount++] = move;
        }
    }

    if (candidateCount == 0 && wantStatus)
        return RandomBattle_PickTeachableMove(species, FALSE, moves, count, preselected);
    if (candidateCount == 0)
        return MOVE_NONE;

    return candidates[RandomUniform(RNG_NONE, 0, candidateCount - 1)];
}

static bool32 RandomBattle_SpeciesCanLearnMove(enum Species species, enum Move move)
{
    const u16 *learnset;

    if (species == SPECIES_NONE || move == MOVE_NONE)
        return FALSE;

    learnset = GetSpeciesTeachableLearnset(species);
    for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
    {
        if (learnset[i] == move)
            return TRUE;
    }
    return FALSE;
}

static void RandomBattle_AssignItems(struct TrainerMon *party, u8 count, const struct RandomBattleOptions *options)
{
    enum Item usedItems[PARTY_SIZE] = {0};
    u8 usedItemCount = 0;

    if (options->itemsOff)
        return;

    if (!options->zCrystalsOff)
    {
        u8 start = RandomUniform(RNG_NONE, 0, count - 1);

        for (u32 n = 0; n < count; n++)
        {
            u8 i = (start + n) % count;

            for (u32 j = 0; j < MAX_MON_MOVES; j++)
            {
                enum Item item = RandomBattle_GetZCrystalForMove(party[i].moves[j]);

                if (item != ITEM_NONE && (options->noItemClause || !RandomBattle_IsItemUsed(item, usedItems, usedItemCount)))
                {
                    party[i].heldItem = item;
                    usedItems[usedItemCount++] = item;
                    n = count;
                    break;
                }
            }
        }
    }

    if (!options->megaStonesOff)
    {
        u8 start = RandomUniform(RNG_NONE, 0, count - 1);

        for (u32 n = 0; n < count; n++)
        {
            u8 i = (start + n) % count;
            enum Item item = RandomBattle_GetMegaStone(party[i].species);

            if (item != ITEM_NONE && party[i].heldItem == ITEM_NONE && (options->noItemClause || !RandomBattle_IsItemUsed(item, usedItems, usedItemCount)))
            {
                party[i].heldItem = item;
                usedItems[usedItemCount++] = item;
                break;
            }
        }
    }

    for (u32 i = 0; i < count; i++)
    {
        if (party[i].heldItem == ITEM_NONE)
        {
            party[i].heldItem = RandomBattle_PickGeneralItem(options->itemsPreselected, usedItems, usedItemCount, options->noItemClause);
            if (party[i].heldItem != ITEM_NONE)
                usedItems[usedItemCount++] = party[i].heldItem;
        }
    }
}

static void RandomBattle_AssignTeraTypes(struct TrainerMon *party, u8 count, const struct RandomBattleOptions *options)
{
    if (options->teraTypesOff)
        return;

    for (u32 i = 0; i < count; i++)
    {
        u8 candidates[MAX_MON_MOVES + 2];
        u8 candidateCount = 0;
        const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[party[i].species];

        for (u32 j = 0; j < MAX_MON_MOVES; j++)
        {
            enum Type type = GetMoveType(party[i].moves[j]);

            if (party[i].moves[j] != MOVE_NONE
             && GetMovePower(party[i].moves[j]) > 0
             && type < NUMBER_OF_MON_TYPES
             && type != TYPE_NONE)
                candidates[candidateCount++] = type;
        }

        for (u32 j = 0; j < ARRAY_COUNT(speciesInfo->types); j++)
        {
            enum Type type = speciesInfo->types[j];

            if (type < NUMBER_OF_MON_TYPES && type != TYPE_NONE)
                candidates[candidateCount++] = type;
        }

        if (candidateCount != 0)
            party[i].teraType = candidates[RandomUniform(RNG_NONE, 0, candidateCount - 1)];
    }
}

static enum Item RandomBattle_PickGeneralItem(bool32 preselected, const enum Item *usedItems, u8 usedItemCount, bool32 noItemClause)
{
    if (preselected)
    {
        for (u32 tries = 0; tries < ARRAY_COUNT(sPreselectedItems); tries++)
        {
            enum Item item = sPreselectedItems[RandomUniform(RNG_NONE, 0, ARRAY_COUNT(sPreselectedItems) - 1)];

            if (noItemClause || !RandomBattle_IsItemUsed(item, usedItems, usedItemCount))
                return item;
        }
        for (u32 i = 0; i < ARRAY_COUNT(sPreselectedItems); i++)
        {
            enum Item item = sPreselectedItems[i];

            if (noItemClause || !RandomBattle_IsItemUsed(item, usedItems, usedItemCount))
                return item;
        }
        return ITEM_NONE;
    }

    for (u32 tries = 0; tries < ITEMS_COUNT; tries++)
    {
        enum Item item = RandomUniform(RNG_NONE, 1, ITEMS_COUNT - 1);
        enum HoldEffect holdEffect = GetItemHoldEffect(item);

        if (holdEffect == HOLD_EFFECT_NONE
         || holdEffect == HOLD_EFFECT_Z_CRYSTAL
         || holdEffect == HOLD_EFFECT_MEGA_STONE
         || GetItemPocket(item) == POCKET_KEY_ITEMS
         || (GetItemPocket(item) == POCKET_TM_HM && GetItemPrice(item) == 0))
            continue;
        if (!noItemClause && RandomBattle_IsItemUsed(item, usedItems, usedItemCount))
            continue;

        return item;
    }

    return ITEM_NONE;
}

static bool32 RandomBattle_IsItemUsed(enum Item item, const enum Item *usedItems, u8 usedItemCount)
{
    for (u32 i = 0; i < usedItemCount; i++)
    {
        if (usedItems[i] == item)
            return TRUE;
    }

    return FALSE;
}

static enum Item RandomBattle_GetZCrystalForMove(enum Move move)
{
    if (move == MOVE_NONE || move >= MOVES_COUNT || GetMovePower(move) == 0)
        return ITEM_NONE;

    switch (GetMoveType(move))
    {
    case TYPE_NORMAL:   return ITEM_NORMALIUM_Z;
    case TYPE_FIRE:     return ITEM_FIRIUM_Z;
    case TYPE_WATER:    return ITEM_WATERIUM_Z;
    case TYPE_ELECTRIC: return ITEM_ELECTRIUM_Z;
    case TYPE_GRASS:    return ITEM_GRASSIUM_Z;
    case TYPE_ICE:      return ITEM_ICIUM_Z;
    case TYPE_FIGHTING: return ITEM_FIGHTINIUM_Z;
    case TYPE_POISON:   return ITEM_POISONIUM_Z;
    case TYPE_GROUND:   return ITEM_GROUNDIUM_Z;
    case TYPE_FLYING:   return ITEM_FLYINIUM_Z;
    case TYPE_PSYCHIC:  return ITEM_PSYCHIUM_Z;
    case TYPE_BUG:      return ITEM_BUGINIUM_Z;
    case TYPE_ROCK:     return ITEM_ROCKIUM_Z;
    case TYPE_GHOST:    return ITEM_GHOSTIUM_Z;
    case TYPE_DRAGON:   return ITEM_DRAGONIUM_Z;
    case TYPE_DARK:     return ITEM_DARKINIUM_Z;
    case TYPE_STEEL:    return ITEM_STEELIUM_Z;
    case TYPE_FAIRY:    return ITEM_FAIRIUM_Z;
    default:            return ITEM_NONE;
    }
}

static enum Item RandomBattle_GetMegaStone(enum Species species)
{
    switch (GET_BASE_SPECIES_ID(species))
    {
    case SPECIES_VENUSAUR:  return ITEM_VENUSAURITE;
    case SPECIES_CHARIZARD: return ITEM_CHARIZARDITE_X;
    case SPECIES_BLASTOISE: return ITEM_BLASTOISINITE;
    default:                return ITEM_NONE;
    }
}

static bool32 RandomBattle_SelectFromPool(struct TrainerMon *dst, u8 count, const struct RandomBattleOptions *options)
{
    const struct TrainerMon *pool = RandomBattle_GetPoolParty();
    u8 poolSize = RandomBattle_GetPoolSize();
    bool8 selected[256] = {0};
    bool8 usedTypes[NUMBER_OF_MON_TYPES] = {0};

    if (poolSize == 0)
        return FALSE;

    for (u32 dstIndex = 0; dstIndex < count; dstIndex++)
    {
        u8 candidates[256];
        u8 candidateCount = 0;
        bool32 hasNewTypeCandidate = FALSE;

        for (u32 i = 0; i < poolSize; i++)
        {
            if (selected[i] || !RandomBattle_IsAllowedSpecies(pool[i].species, options, dst, dstIndex))
                continue;
            if (!options->noItemClause && pool[i].heldItem != ITEM_NONE)
            {
                bool32 duplicateItem = FALSE;
                for (u32 j = 0; j < dstIndex; j++)
                {
                    if (dst[j].heldItem == pool[i].heldItem)
                        duplicateItem = TRUE;
                }
                if (duplicateItem)
                    continue;
            }
            if (!options->noTypeDiversity && RandomBattle_IsNewType(pool[i].species, usedTypes))
                hasNewTypeCandidate = TRUE;
        }

        for (u32 i = 0; i < poolSize; i++)
        {
            if (selected[i] || !RandomBattle_IsAllowedSpecies(pool[i].species, options, dst, dstIndex))
                continue;
            if (!options->noItemClause && pool[i].heldItem != ITEM_NONE)
            {
                bool32 duplicateItem = FALSE;

                for (u32 j = 0; j < dstIndex; j++)
                {
                    if (dst[j].heldItem == pool[i].heldItem)
                        duplicateItem = TRUE;
                }
                if (duplicateItem)
                    continue;
            }
            if (hasNewTypeCandidate && !RandomBattle_IsNewType(pool[i].species, usedTypes))
                continue;

            candidates[candidateCount++] = i;
        }

        if (candidateCount == 0)
            return FALSE;

        u8 selectedIndex = candidates[RandomUniform(RNG_NONE, 0, candidateCount - 1)];
        selected[selectedIndex] = TRUE;
        dst[dstIndex] = pool[selectedIndex];
        RandomBattle_UpdateUsedTypes(dst[dstIndex].species, usedTypes);
    }

    return TRUE;
}

static u8 RandomBattle_GetPoolSize(void)
{
    const struct Trainer *trainer = &sRandomBattlePoolTrainers[DIFFICULTY_NORMAL][RANDOM_BATTLE_POOL];

    return trainer->poolSize != 0 ? trainer->poolSize : trainer->partySize;
}

static const struct TrainerMon *RandomBattle_GetPoolParty(void)
{
    return sRandomBattlePoolTrainers[DIFFICULTY_NORMAL][RANDOM_BATTLE_POOL].party;
}

static u16 RandomBattle_ReadVarArg(struct ScriptContext *ctx)
{
    return VarGet(ScriptReadHalfword(ctx));
}

static void RandomBattle_ReadCommonOptions(struct ScriptContext *ctx, struct RandomBattleOptions *options)
{
    memset(options, 0, sizeof(*options));

    options->numPlayerMons = RandomBattle_ReadVarArg(ctx);
    options->numOpponentMons = RandomBattle_ReadVarArg(ctx);
    options->trainerPic = RandomBattle_ReadVarArg(ctx);
    options->trainerName = (const u8 *)ScriptReadWord(ctx);
    options->isDouble = RandomBattle_ReadVarArg(ctx) != FALSE;
    options->preEvos = RandomBattle_ReadVarArg(ctx) != FALSE;
    options->noSpeciesClause = RandomBattle_ReadVarArg(ctx) != FALSE;
    options->noItemClause = RandomBattle_ReadVarArg(ctx) != FALSE;
    options->noTypeDiversity = RandomBattle_ReadVarArg(ctx) != FALSE;
    options->noLegendaries = RandomBattle_ReadVarArg(ctx) != FALSE;
}

static bool32 RandomBattle_RunFromScript(struct ScriptContext *ctx, bool32 fromPool)
{
    struct RandomBattleOptions options;
    bool32 ok;

    RandomBattle_ReadCommonOptions(ctx, &options);
    if (!fromPool)
    {
        options.preselectedMoves = RandomBattle_ReadVarArg(ctx) != FALSE;
        options.itemsOff = RandomBattle_ReadVarArg(ctx) != FALSE;
        options.itemsPreselected = RandomBattle_ReadVarArg(ctx) != FALSE;
        options.teraTypesOff = RandomBattle_ReadVarArg(ctx) != FALSE;
        options.zCrystalsOff = RandomBattle_ReadVarArg(ctx) != FALSE;
        options.megaStonesOff = RandomBattle_ReadVarArg(ctx) != FALSE;
    }

    Script_RequestEffects(SCREFF_V1);
    ok = fromPool ? RandomBattle_GeneratePoolBattle(&options) : RandomBattle_GenerateTrueBattle(&options);
    gSpecialVar_Result = ok;
    if (ok)
        BattleSetup_StartRandomBattle(options.isDouble);
    return ok;
}

void ScrCmd_truerandombattle(struct ScriptContext *ctx)
{
    RandomBattle_RunFromScript(ctx, FALSE);
}

void ScrCmd_poolrandombattle(struct ScriptContext *ctx)
{
    RandomBattle_RunFromScript(ctx, TRUE);
}
