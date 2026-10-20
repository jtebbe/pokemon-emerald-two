#include "global.h"
#include "malloc.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "dexnav.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "evolution_scene.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fishing.h"
#include "follower_npc.h"
#include "graphics.h"
#include "item.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "mail.h"
#include "move_relearner.h"
#include "naming_screen.h"
#include "overworld.h"
#include "ow_abilities.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_animation.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "pc_storage.h"
#include "random.h"
#include "random_mon_generation.h"
#include "regions.h"
#include "rtc.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "test_runner.h"
#include "text.h"
#include "trainer.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_partner.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_string_ids.h"
#include "constants/cries.h"
#include "constants/event_objects.h"
#include "constants/form_change_types.h"
#include "constants/item_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/regions.h"
#include "constants/songs.h"
#include "constants/trainers.h"
#include "constants/union_room.h"
#include "constants/weather.h"

extern enum Item gSpecialVar_ItemId;


void TrySpecialOverworldEvo();

EWRAM_DATA static u8 sLearningMoveTableID = 0;
EWRAM_DATA u8 gPartiesCount[MAX_BATTLE_TRAINERS] = {0};
EWRAM_DATA struct Pokemon gParties[MAX_BATTLE_TRAINERS][PARTY_SIZE] = {0};
EWRAM_DATA struct SpriteTemplate gMultiuseSpriteTemplate = {0};
EWRAM_DATA static struct MonSpritesGfxManager *sMonSpritesGfxManagers[MON_SPR_GFX_MANAGERS_COUNT] = {NULL};
EWRAM_DATA u8 gTriedEvolving = 0;
EWRAM_DATA u16 gFollowerSteps = 0;

struct Pokemon (*const gPlayerPartyPtr)[6] = &gParties[B_TRAINER_PLAYER];
u8 (*const gPlayerPartyCountPtr) = &gPartiesCount[B_TRAINER_PLAYER];

#include "data/abilities.h"

// Used in an unreferenced function in RS.
// Unreferenced here and in FRLG.
struct CombinedMove
{
    enum Move move1;
    enum Move move2;
    enum Move newMove;
};

static const struct CombinedMove sCombinedMoves[2] =
{
    {MOVE_EMBER, MOVE_GUST, MOVE_HEAT_WAVE},
    {0xFFFF, 0xFFFF, 0xFFFF}
};

// NOTE: The order of the elements in the array below is irrelevant.
// To reorder the pokedex, see the values in include/constants/pokedex.h.

#define KANTO_TO_NATIONAL(name)     [KANTO_DEX_##name - 1] = NATIONAL_DEX_##name,
#define CALICADEA_TO_NATIONAL(name)     [CALICADEA_DEX_##name - 1] = NATIONAL_DEX_##name,

static const enum NationalDexOrder sKantoToNationalOrder[KANTO_DEX_COUNT] =
{
    FOREACH_SPECIES_IN_KANTO_DEX_ORDER(KANTO_TO_NATIONAL)
};

// Assigns all Calicadea Dex Indexes to a National Dex Index
static const enum NationalDexOrder sCalicadeaToNationalOrder[CALICADEA_DEX_COUNT - 1] =
{
    FOREACH_SPECIES_IN_CALICADEA_DEX_ORDER(CALICADEA_TO_NATIONAL)
};

const struct NatureInfo gNaturesInfo[NUM_NATURES] =
{
    [NATURE_HARDY] =
    {
        .name = COMPOUND_STRING("Hardy"),
        .statUp = STAT_ATK,
        .statDown = STAT_ATK,
        .backAnim = 0,
    },
    [NATURE_LONELY] =
    {
        .name = COMPOUND_STRING("Lonely"),
        .statUp = STAT_ATK,
        .statDown = STAT_DEF,
        .backAnim = 2,
    },
    [NATURE_BRAVE] =
    {
        .name = COMPOUND_STRING("Brave"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPEED,
        .backAnim = 0,
    },
    [NATURE_ADAMANT] =
    {
        .name = COMPOUND_STRING("Adamant"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPATK,
        .backAnim = 0,
    },
    [NATURE_NAUGHTY] =
    {
        .name = COMPOUND_STRING("Naughty"),
        .statUp = STAT_ATK,
        .statDown = STAT_SPDEF,
        .backAnim = 0,
    },
    [NATURE_BOLD] =
    {
        .name = COMPOUND_STRING("Bold"),
        .statUp = STAT_DEF,
        .statDown = STAT_ATK,
        .backAnim = 1,
    },
    [NATURE_DOCILE] =
    {
        .name = COMPOUND_STRING("Docile"),
        .statUp = STAT_DEF,
        .statDown = STAT_DEF,
        .backAnim = 1,
    },
    [NATURE_RELAXED] =
    {
        .name = COMPOUND_STRING("Relaxed"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPEED,
        .backAnim = 1,
    },
    [NATURE_IMPISH] =
    {
        .name = COMPOUND_STRING("Impish"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPATK,
        .backAnim = 0,
    },
    [NATURE_LAX] =
    {
        .name = COMPOUND_STRING("Lax"),
        .statUp = STAT_DEF,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
    },
    [NATURE_TIMID] =
    {
        .name = COMPOUND_STRING("Timid"),
        .statUp = STAT_SPEED,
        .statDown = STAT_ATK,
        .backAnim = 2,
    },
    [NATURE_HASTY] =
    {
        .name = COMPOUND_STRING("Hasty"),
        .statUp = STAT_SPEED,
        .statDown = STAT_DEF,
        .backAnim = 0,
    },
    [NATURE_SERIOUS] =
    {
        .name = COMPOUND_STRING("Serious"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPEED,
        .backAnim = 1,
    },
    [NATURE_JOLLY] =
    {
        .name = COMPOUND_STRING("Jolly"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPATK,
        .backAnim = 0,
    },
    [NATURE_NAIVE] =
    {
        .name = COMPOUND_STRING("Naive"),
        .statUp = STAT_SPEED,
        .statDown = STAT_SPDEF,
        .backAnim = 0,
    },
    [NATURE_MODEST] =
    {
        .name = COMPOUND_STRING("Modest"),
        .statUp = STAT_SPATK,
        .statDown = STAT_ATK,
        .backAnim = 2,
    },
    [NATURE_MILD] =
    {
        .name = COMPOUND_STRING("Mild"),
        .statUp = STAT_SPATK,
        .statDown = STAT_DEF,
        .backAnim = 2,
    },
    [NATURE_QUIET] =
    {
        .name = COMPOUND_STRING("Quiet"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPEED,
        .backAnim = 2,
    },
    [NATURE_BASHFUL] =
    {
        .name = COMPOUND_STRING("Bashful"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPATK,
        .backAnim = 2,
    },
    [NATURE_RASH] =
    {
        .name = COMPOUND_STRING("Rash"),
        .statUp = STAT_SPATK,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
    },
    [NATURE_CALM] =
    {
        .name = COMPOUND_STRING("Calm"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_ATK,
        .backAnim = 1,
    },
    [NATURE_GENTLE] =
    {
        .name = COMPOUND_STRING("Gentle"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_DEF,
        .backAnim = 2,
    },
    [NATURE_SASSY] =
    {
        .name = COMPOUND_STRING("Sassy"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPEED,
        .backAnim = 1,
    },
    [NATURE_CAREFUL] =
    {
        .name = COMPOUND_STRING("Careful"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPATK,
        .backAnim = 2,
    },
    [NATURE_QUIRKY] =
    {
        .name = COMPOUND_STRING("Quirky"),
        .statUp = STAT_SPDEF,
        .statDown = STAT_SPDEF,
        .backAnim = 1,
    },
};

#include "data/graphics/pokemon.h"

#include "data/pokemon/trainer_class_lookups.h"
#include "data/pokemon/experience_tables.h"

#include "data/pokemon/level_up_learnsets/custom.h"

#include "data/pokemon/teachable_learnsets.h"
#include "data/pokemon/custom_egg_moves.h"
#include "data/pokemon/form_species_tables.h"
#include "data/pokemon/form_change_tables.h"
#include "data/pokemon/form_change_table_pointers.h"
#include "data/pokemon/wild_encounter_ow_behavior.h"
#include "data/object_events/object_event_pic_tables_followers.h"

#include "data/pokemon/species_info.h"

const u8 gStatStageRatios[MAX_STAT_STAGE + 1][2] =
{
    {10, 40}, // -6, MIN_STAT_STAGE
    {10, 35}, // -5
    {10, 30}, // -4
    {10, 25}, // -3
    {10, 20}, // -2
    {10, 15}, // -1
    {10, 10}, //  0, DEFAULT_STAT_STAGE
    {15, 10}, // +1
    {20, 10}, // +2
    {25, 10}, // +3
    {30, 10}, // +4
    {35, 10}, // +5
    {40, 10}, // +6, MAX_STAT_STAGE
};

// The classes used by other players in the Union Room.
// These should correspond with the overworld graphics in sUnionRoomObjGfxIds
const u16 gUnionRoomFacilityClasses[NUM_UNION_ROOM_CLASSES * GENDER_COUNT] =
{
    // Male classes
    FACILITY_CLASS_COOLTRAINER_M,
    FACILITY_CLASS_BLACK_BELT,
    FACILITY_CLASS_CAMPER,
    FACILITY_CLASS_YOUNGSTER,
    FACILITY_CLASS_PSYCHIC_M,
    FACILITY_CLASS_BUG_CATCHER,
    FACILITY_CLASS_PKMN_BREEDER_M,
    FACILITY_CLASS_GUITARIST,
    // Female classes
    FACILITY_CLASS_COOLTRAINER_F,
    FACILITY_CLASS_HEX_MANIAC,
    FACILITY_CLASS_PICNICKER,
    FACILITY_CLASS_LASS,
    FACILITY_CLASS_PSYCHIC_F,
    FACILITY_CLASS_BATTLE_GIRL,
    FACILITY_CLASS_PKMN_BREEDER_F,
    FACILITY_CLASS_BEAUTY
};

const struct SpriteTemplate gBattlerSpriteTemplates[MAX_BATTLERS_COUNT] =
{
    [B_POSITION_PLAYER_LEFT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpritePlayerSide,
        .anims = NULL,
        .images = gBattlerPicTable_PlayerLeft,
        .affineAnims = gAffineAnims_BattleSpritePlayerSide,
        .callback = SpriteCB_BattleSpriteStartSlideLeft,
    },
    [B_POSITION_OPPONENT_LEFT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpriteOpponentSide,
        .anims = NULL,
        .images = gBattlerPicTable_OpponentLeft,
        .affineAnims = gAffineAnims_BattleSpriteOpponentSide,
        .callback = SpriteCB_WildMon,
    },
    [B_POSITION_PLAYER_RIGHT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpritePlayerSide,
        .anims = NULL,
        .images = gBattlerPicTable_PlayerRight,
        .affineAnims = gAffineAnims_BattleSpritePlayerSide,
        .callback = SpriteCB_BattleSpriteStartSlideLeft,
    },
    [B_POSITION_OPPONENT_RIGHT] = {
        .tileTag = TAG_NONE,
        .paletteTag = 0,
        .oam = &gOamData_BattleSpriteOpponentSide,
        .anims = NULL,
        .images = gBattlerPicTable_OpponentRight,
        .affineAnims = gAffineAnims_BattleSpriteOpponentSide,
        .callback = SpriteCB_WildMon
    },
};

static const struct SpriteTemplate sTrainerBackSpriteTemplate =
{
    .tileTag = TAG_NONE,
    .paletteTag = 0,
    .oam = &gOamData_BattleSpritePlayerSide,
    .anims = NULL,
    .affineAnims = gAffineAnims_BattleSpritePlayerSide,
    .callback = SpriteCB_BattleSpriteStartSlideLeft,
};

static const u8 sGetMonDataEVConstants[] =
{
    MON_DATA_HP_EV,
    MON_DATA_ATK_EV,
    MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV,
    MON_DATA_SPDEF_EV,
    MON_DATA_SPATK_EV
};

// For stat-raising items
static const enum Stat sStatsToRaise[] =
{
    STAT_ATK, STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF, STAT_ACC
};

static const struct OamData sOamData_64x64 =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0
};

static const struct SpriteTemplate sSpriteTemplate_64x64 =
{
    .tileTag = TAG_NONE,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_64x64,
};

// NOTE: Reordering this array will break compatibility with existing
// saves.
static const u32 sCompressedStatuses[] =
{
    STATUS1_NONE,
    STATUS1_SLEEP_TURN(1),
    STATUS1_SLEEP_TURN(2),
    STATUS1_SLEEP_TURN(3),
    STATUS1_SLEEP_TURN(4),
    STATUS1_SLEEP_TURN(5),
    STATUS1_POISON,
    STATUS1_BURN,
    STATUS1_FREEZE,
    STATUS1_PARALYSIS,
    STATUS1_TOXIC_POISON,
    STATUS1_FROSTBITE,
};

// Attempt to detect situations where the BoxPokemon struct is unable to
// contain all the values.
// TODO: Is it possible to compute:
// - The maximum experience.
// - The maximum PP.
// - The maximum HP.
// - The maximum form countdown.

// The following definition of sBoxPokemonConstantsFit and STATIC_ASSERTS
// will prevent developers from compiling the game if the value of the
// constant on the left does not fit within BoxPokemon's stored field type.

// To successfully compile, developers will need to do one of the following:
// 1) Decrease the size of the constant.
// 2) Increase the number of bits both on the struct AND in the corresponding assert. This will likely break user's saves unless there is free space after the member that is being adjsted.
// 3) Repurpose unused IDs.

// EXAMPLES
// If a developer has added enough new items so that ITEMS_COUNT now equals 1200, they could...
// 1) remove new items until ITEMS_COUNT is 1023, the max value that will fit in 10 bits.
// 2) change heldItem:10 to heldItem:11 AND change the below assert for ITEMS_COUNT to check for (1 << 11).
// 3) repurpose IDs from other items that aren't being used, like ITEM_GOLD_TEETH or ITEM_SS_TICKET until ITEMS_COUNT equals 1023, the max value that will fit in 10 bits.

UNUSED static const struct BoxPokemon sBoxPokemonConstantsFit =
{
    .language = NUM_LANGUAGES - 1,
    .hiddenNatureModifier = NUM_NATURES - 1,
    .compressedStatus = ARRAY_COUNT(sCompressedStatuses) - 1,
    .data = {
        .species = NUM_SPECIES - 1,
        .teraType = NUMBER_OF_MON_TYPES - 1,
        .heldItem = ITEMS_COUNT - 1,
        .pokeball = POKEBALL_COUNT - 1,
        .move1 = MOVES_COUNT_ALL - 1,
        .move2 = MOVES_COUNT_ALL - 1,
        .move3 = MOVES_COUNT_ALL - 1,
        .move4 = MOVES_COUNT_ALL - 1,
        .hpEV = MAX_PER_STAT_EVS,
        .attackEV = MAX_PER_STAT_EVS,
        .defenseEV = MAX_PER_STAT_EVS,
        .speedEV = MAX_PER_STAT_EVS,
        .spAttackEV = MAX_PER_STAT_EVS,
        .spDefenseEV = MAX_PER_STAT_EVS,
        .metLocation = min(MAPSEC_COUNT, min(METLOC_SPECIAL_EGG, METLOC_IN_GAME_TRADE)),
        .metLevel = MAX_LEVEL,
        .metGame = NUM_VERSIONS, // NOTE: NUM_VERSIONS is inclusive!
        .otGender = GENDER_COUNT - 1,
        .abilityNum = NUM_ABILITY_SLOTS - 1,
    },
};

STATIC_ASSERT(MAX_LEVEL <= 100, PokemonData_experience_PotentiallyTooSmall); // Maximum of ~2 million exp.

static u32 CompressStatus(u32 status)
{
    s32 i;
    for (i = 0; i < ARRAY_COUNT(sCompressedStatuses); i++)
    {
        if (sCompressedStatuses[i] == status)
            return i;
    }
    return 0; // STATUS1_NONE
}

static u32 UncompressStatus(u32 compressedStatus)
{
    if (compressedStatus < ARRAY_COUNT(sCompressedStatuses))
        return sCompressedStatuses[compressedStatus];
    else
        return STATUS1_NONE;
}

void ZeroBoxMonData(struct BoxPokemon *boxMon)
{
    u8 *raw = (u8 *)boxMon;
    u32 i;
    for (i = 0; i < sizeof(struct BoxPokemon); i++)
        raw[i] = 0;
}

void ZeroMonData(struct Pokemon *mon)
{
    u32 arg;
    ZeroBoxMonData(&mon->box);
    arg = 0;
    SetMonData(mon, MON_DATA_STATUS, &arg);
    SetMonData(mon, MON_DATA_LEVEL, &arg);
    SetMonData(mon, MON_DATA_HP, &arg);
    SetMonData(mon, MON_DATA_MAX_HP, &arg);
    SetMonData(mon, MON_DATA_ATK, &arg);
    SetMonData(mon, MON_DATA_DEF, &arg);
    SetMonData(mon, MON_DATA_SPEED, &arg);
    SetMonData(mon, MON_DATA_SPATK, &arg);
    SetMonData(mon, MON_DATA_SPDEF, &arg);
    arg = MAIL_NONE;
    SetMonData(mon, MON_DATA_MAIL, &arg);
}

void ZeroPartyMons(struct Pokemon *party)
{
    for (s32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&party[i]);
}

void ZeroPlayerPartyMons(void)
{
    for (s32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
    gPartiesCount[B_TRAINER_PLAYER] = 0;
}

void ZeroEnemyPartyMons(void)
{
    for (s32 i = 0; i < PARTY_SIZE; i++)
    {
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][i]);
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_B][i]);
    }
    gPartiesCount[B_TRAINER_OPPONENT_A] = 0;
    gPartiesCount[B_TRAINER_OPPONENT_B] = 0;
}

void CreateRandomMon(struct Pokemon *mon, enum Species species, u8 level)
{
    CreateMonWithStats(mon, species, level, Random32(), OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(mon);
}

void CreateMon(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId)
{
    u32 mail;
    ZeroMonData(mon);
    CreateBoxMon(&mon->box, species, level, personality, trainerId);
    SetMonData(mon, MON_DATA_LEVEL, &level);
    mail = MAIL_NONE;
    SetMonData(mon, MON_DATA_MAIL, &mail);
}

void CreateMonWithStats(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId)
{
    CreateMon(mon, species, level, personality, trainerId);
    CalculateMonStats(mon);
}

bool32 ComputePlayerShinyOdds(u32 personality, u32 value)
{
    if (FlagGet(P_FLAG_FORCE_NO_SHINY))
        return FALSE;

    if (FlagGet(P_FLAG_FORCE_SHINY))
        return TRUE;

    if (P_ONLY_OBTAINABLE_SHINIES && (FlagGet(WE_FLAG_NO_CATCHING)))
        return FALSE;

    if (P_NO_SHINIES_WITHOUT_POKEBALLS && !HasAtLeastOnePokeBall() && FlagGet(FLAG_SYS_POKEDEX_GET))
        return FALSE;

    u32 totalRerolls = 0;

    if (CheckBagHasItem(ITEM_SHINY_CHARM, 1))
        totalRerolls += I_SHINY_CHARM_ADDITIONAL_ROLLS;

    if (LURE_STEP_COUNT != 0)
        totalRerolls += 1;

    totalRerolls += CalculateChainFishingShinyRolls();

    if (gDexNavSpecies)
        totalRerolls += CalculateDexNavShinyRolls();

    while (GET_SHINY_VALUE(value, personality) >= SHINY_ODDS && totalRerolls > 0)
    {
        personality = Random32();
        totalRerolls--;
    }

    return GET_SHINY_VALUE(value, personality) < SHINY_ODDS;
}

void CreateBoxMon(struct BoxPokemon *boxMon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId trainerId)
{
    u8 speciesName[POKEMON_NAME_LENGTH + 1];
    u32 value;
    bool32 isShiny;

    ZeroBoxMonData(boxMon);
    // Determine original trainer ID
    if (trainerId.method == OT_ID_RANDOM_NO_SHINY)
    {
        value = Random32();
        isShiny = FALSE;
    }
    else if (trainerId.method == OT_ID_PRESET)
    {
        value = trainerId.value;
        isShiny = GET_SHINY_VALUE(value, personality) < SHINY_ODDS;
    }
    else // Player is the OT
    {
        value = READ_OTID_FROM_SAVE;
        isShiny = ComputePlayerShinyOdds(personality, value);
    }

    SetBoxMonData(boxMon, MON_DATA_PERSONALITY, &personality);
    SetBoxMonData(boxMon, MON_DATA_OT_ID, &value);

    SetBoxMonData(boxMon, MON_DATA_IS_SHINY, &isShiny);
    StringCopy(speciesName, GetSpeciesName(species));
    SetBoxMonData(boxMon, MON_DATA_NICKNAME, speciesName);
    SetBoxMonData(boxMon, MON_DATA_LANGUAGE, &gGameLanguage);
    SetBoxMonData(boxMon, MON_DATA_OT_NAME, gSaveData.playerName);
    SetBoxMonData(boxMon, MON_DATA_SPECIES, &species);
    SetBoxMonData(boxMon, MON_DATA_EXP, &gExperienceTables[gSpeciesInfo[species].growthRate][level]);
    value = GetCurrentRegionMapSectionId();
    SetBoxMonData(boxMon, MON_DATA_MET_LOCATION, &value);
    SetBoxMonData(boxMon, MON_DATA_MET_LEVEL, &level);
    SetBoxMonData(boxMon, MON_DATA_MET_GAME, &gGameVersion);
    value = BALL_POKE;
    SetBoxMonData(boxMon, MON_DATA_POKEBALL, &value);
    SetBoxMonData(boxMon, MON_DATA_OT_GENDER, &gSaveData.playerGender);

    value = boxMon->personality & 0x1;
    enum Type teraType = value == 0 ? GetSpeciesType(species, 0) : GetSpeciesType(species, 1);
    SetBoxMonData(boxMon, MON_DATA_TERA_TYPE, &teraType);
    //using gen 3-4 ability formula, it was changed in later gens
    if (GetSpeciesAbility(species, 1))
        SetBoxMonData(boxMon, MON_DATA_ABILITY_NUM, &value);
}

static bool32 IsValidGender(u32 gender)
{
    switch (gender)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
    case MON_GENDER_RANDOM:
        return TRUE;
    default:
        return FALSE;
    }
}

static void CleanIncompatibleGenderSpecies(enum Species species, u8 *gender)
{
    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        *gender = MON_GENDER_RANDOM;
        return;
    }
    if (*gender == MON_GENDERLESS)
        *gender = MON_GENDER_RANDOM;
}

u32 GetMonPersonality(enum Species species, u8 gender, u8 nature, u8 unownLetter)
{
    u32 personality, actualLetter;

    assertf(IsValidGender(gender), "invalid gender: %d", gender)
    {
        gender = MON_GENDER_RANDOM;
    }

    assertf(nature <= NATURE_RANDOM, "invalid nature: %d", nature)
    {
        nature = NATURE_RANDOM;
    }

    assertf(unownLetter <= NUM_UNOWN_FORMS, "invalid letter: %d", unownLetter)
    {
        unownLetter = RANDOM_UNOWN_LETTER;
    }

    CleanIncompatibleGenderSpecies(species, &gender);
    do
    {
        personality = Random32();
        actualLetter = GET_UNOWN_LETTER(personality);
    }
    while ((nature != GetNatureFromPersonality(personality) && nature != NATURE_RANDOM)
            || (gender != MON_GENDER_RANDOM && gender != GetGenderFromSpeciesAndPersonality(species, personality))
            || ((actualLetter != unownLetter - 1) && unownLetter > 0));
    return personality;
}

// This is only used to create Wally's Ralts.
void CreateMaleMon(struct Pokemon *mon, enum Species species, u8 level)
{
    u32 personality = GetMonPersonality(species, MON_MALE, NATURE_RANDOM, RANDOM_UNOWN_LETTER);
    CreateMonWithStats(mon, species, level, personality, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(mon);
}

void CreateMonWithPersonality(struct Pokemon *mon, enum Species species, u8 level, u32 personality)
{
    CreateMon(mon, species, level, personality, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    GiveMonInitialMoveset(mon);
}

static void CreateEventMon(struct Pokemon *mon, enum Species species, u8 level, u32 personality, struct OriginalTrainerId otId)
{
    CreateMonWithStats(mon, species, level, personality, otId);
    GiveMonInitialMoveset(mon);
    CalculateMonStats(mon);
}

enum TrainerPicID GetUnionRoomTrainerPic(void)
{
    u8 linkId;
    u32 arrId;

    {
        linkId = GetMultiplayerId() ^ 1;
    }

    arrId = gLinkPlayers[linkId].trainerId % NUM_UNION_ROOM_CLASSES;
    arrId |= gLinkPlayers[linkId].gender * NUM_UNION_ROOM_CLASSES;
    return FacilityClassToPicIndex(gUnionRoomFacilityClasses[arrId]);
}

enum TrainerClassID GetUnionRoomTrainerClass(void)
{
    u8 linkId;
    u32 arrId;

    {
        linkId = GetMultiplayerId() ^ 1;
    }

    arrId = gLinkPlayers[linkId].trainerId % NUM_UNION_ROOM_CLASSES;
    arrId |= gLinkPlayers[linkId].gender * NUM_UNION_ROOM_CLASSES;
    return gFacilityClassToTrainerClass[gUnionRoomFacilityClasses[arrId]];
}

void CreateEnemyEventMon(void)
{
    s32 species = gSpecialVar_0x8004;
    s32 level = gSpecialVar_0x8005;
    s32 itemId = gSpecialVar_0x8006;

    ZeroEnemyPartyMons();

    CreateEventMon(&gParties[B_TRAINER_OPPONENT_A][0], species, level, Random32(), OTID_STRUCT_PLAYER_ID);
    if (itemId)
    {
        u8 heldItem[2];
        heldItem[0] = itemId;
        heldItem[1] = itemId >> 8;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, heldItem);
    }
}

void CalculateMonStats(struct Pokemon *mon)
{
    CalculateMonStatsCont(mon, TRUE);
}

void CalculateMonStatsCont(struct Pokemon *mon, bool32 updateSpeedStat)
{
    s32 oldMaxHP = GetMonData(mon, MON_DATA_MAX_HP);
    s32 currentHP = GetMonData(mon, MON_DATA_HP);
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    s32 level = GetLevelFromMonExp(mon);
    s32 newMaxHP;

    u8 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);

    SetMonData(mon, MON_DATA_LEVEL, &level);

    s32 ev[NUM_STATS];
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        ev[i] = GetMonData(mon, MON_DATA_HP_EV + i);

        if (i == STAT_HP)
            continue;

        u8 baseStat = GetSpeciesBaseStat(species, i);
        s32 n = (((2 * baseStat + 31 + ev[i] / 4) * level) / 100) + 5;
        n = ModifyStatByNature(nature, n, i);
#if TESTING
        if (gTestRunnerEnabled && gMain.inBattle)
            n = TestRunner_Battle_OverrideStat(mon, i, n);
#endif
        SetMonData(mon, MON_DATA_MAX_HP + i, &n);
    }

    if (HasShedinjaHPHandling(species))
    {
        newMaxHP = 1;
    }
    else
    {
        s32 n = 2 * GetSpeciesBaseHP(species) + 31;
        newMaxHP = (((n + ev[STAT_HP] / 4) * level) / 100) + level + 10;
    }

#if TESTING
    if (gTestRunnerEnabled && gMain.inBattle)
        newMaxHP = TestRunner_Battle_OverrideStat(mon, STAT_HP, newMaxHP);
#endif
    gBattleScripting.levelUpHP = newMaxHP - oldMaxHP;
    if (gBattleScripting.levelUpHP == 0)
        gBattleScripting.levelUpHP = 1;
    SetMonData(mon, MON_DATA_MAX_HP, &newMaxHP);

    // Since a Pokémon's maxHP data could either not have
    // been initialized at this point or this Pokémon is
    // just fainted, the check for oldMaxHP is important.
    if (currentHP == 0 && oldMaxHP != 0)
        return;

    // Only add to currentHP if newMaxHP went up.
    if (newMaxHP > oldMaxHP)
        currentHP += newMaxHP - oldMaxHP;

    // Ensure currentHP does not surpass newMaxHP.
    if (currentHP > newMaxHP)
        currentHP = newMaxHP;

    SetMonData(mon, MON_DATA_HP, &currentHP);
}

void BoxMonToMon(const struct BoxPokemon *src, struct Pokemon *dest)
{
    u32 value = 0;
    dest->box = *src;
    dest->status = GetBoxMonData(&dest->box, MON_DATA_STATUS);
    dest->hp = 0;
    dest->maxHP = 0;
    value = MAIL_NONE;
    SetMonData(dest, MON_DATA_MAIL, &value);
    value = GetBoxMonData(&dest->box, MON_DATA_HP_LOST);
    CalculateMonStats(dest);
    value = value >= dest->maxHP ? 0 : dest->maxHP - value;
    SetMonData(dest, MON_DATA_HP, &value);
}

u8 GetLevelFromMonExp(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp = GetMonData(mon, MON_DATA_EXP);
    s32 level = 1;

    while (level <= MAX_LEVEL && gExperienceTables[gSpeciesInfo[species].growthRate][level] <= exp)
        level++;

    return level - 1;
}

u8 GetLevelFromBoxMonExp(struct BoxPokemon *boxMon)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    u32 exp = GetBoxMonData(boxMon, MON_DATA_EXP);
    s32 level = 1;

    while (level <= MAX_LEVEL && gExperienceTables[gSpeciesInfo[species].growthRate][level] <= exp)
        level++;

    return level - 1;
}

u16 GiveMoveToMon(struct Pokemon *mon, enum Move move)
{
    return GiveMoveToBoxMon(&mon->box, move);
}

u16 GiveMoveToBoxMon(struct BoxPokemon *boxMon, enum Move move)
{
    s32 i;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move existingMove = GetBoxMonData(boxMon, MON_DATA_MOVE1 + i);
        if (existingMove == MOVE_NONE)
        {
            SetBoxMonData(boxMon, MON_DATA_MOVE1 + i, &move);
            return move;
        }
        if (existingMove == move)
            return MON_ALREADY_KNOWS_MOVE;
    }
    return MON_MOVE_SLOTS_FULL;
}

u16 GiveMoveToBattleMon(struct BattlePokemon *mon, enum Move move)
{
    s32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (mon->moves[i] == MOVE_NONE)
        {
            mon->moves[i] = move;
            mon->pp[i] = GetMovePP(move);
            return move;
        }
    }

    return MON_MOVE_SLOTS_FULL;
}

void SetMonMoveSlot(struct Pokemon *mon, enum Move move, enum MoveSlot slot)
{
    SetBoxMonMoveSlot(&mon->box, move, slot);
}

void SetBoxMonMoveSlot(struct BoxPokemon *mon, enum Move move, enum MoveSlot slot)
{
    SetBoxMonData(mon, MON_DATA_MOVE1 + slot, &move);
}


void SetBattleMonMoveSlot(struct BattlePokemon *mon, enum Move move, enum MoveSlot slot)
{
    mon->moves[slot] = move;
    mon->pp[slot] = GetMovePP(move);
}

void GiveMonInitialMoveset(struct Pokemon *mon)
{
    GiveBoxMonInitialMoveset(&mon->box);
}

void GiveBoxMonInitialMoveset(struct BoxPokemon *boxMon) //Credit: AsparagusEduardo
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    s32 level = GetLevelFromBoxMonExp(boxMon);
    s32 i;
    enum Move moves[MAX_MON_MOVES] = {MOVE_NONE};
    u8 addedMoves = 0;
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    for (i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        s32 j;
        bool32 alreadyKnown = FALSE;

        if (learnset[i].level > level)
            break;
        if (learnset[i].level == 0)
            continue;

        for (j = 0; j < addedMoves; j++)
        {
            if (moves[j] == learnset[i].move)
            {
                alreadyKnown = TRUE;
                break;
            }
        }

        if (!alreadyKnown)
        {
            if (addedMoves < MAX_MON_MOVES)
            {
                moves[addedMoves] = learnset[i].move;
                addedMoves++;
            }
            else
            {
                for (j = 0; j < MAX_MON_MOVES - 1; j++)
                    moves[j] = moves[j + 1];
                moves[MAX_MON_MOVES - 1] = learnset[i].move;
            }
        }
    }
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        SetBoxMonData(boxMon, MON_DATA_MOVE1 + i, &moves[i]);
    }
}

void GiveMonDefaultMove(struct Pokemon *mon, enum MoveSlot slot)
{
    GiveBoxMonDefaultMove(&mon->box, slot);
}

void GiveBoxMonDefaultMove(struct BoxPokemon *boxMon, enum MoveSlot slot)
{
    enum Move move = MOVE_NONE;
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);
    s32 level = GetLevelFromBoxMonExp(boxMon);
    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        s32 j;
        bool32 alreadyKnown = FALSE;

        if (learnset[i].level > level)
            break;
        if (learnset[i].level == 0)
            continue;

        for (j = 0; j < slot; j++)
        {
            if (GetBoxMonData(boxMon, MON_DATA_MOVE1 + j) == learnset[i].move)
            {
                alreadyKnown = TRUE;
                break;
            }
        }
        if (!alreadyKnown)
            move = learnset[i].move;
    }

    SetBoxMonData(boxMon, MON_DATA_MOVE1 + slot, &move);
}

enum Move MonTryLearningNewMoveAtLevel(struct Pokemon *mon, bool32 firstMove, u32 level)
{
    enum Move retVal = MOVE_NONE;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    // since you can learn more than one move per level
    // the game needs to know whether you decided to
    // learn it or keep the old set to avoid asking
    // you to learn the same move over and over again
    if (firstMove)
    {
        sLearningMoveTableID = 0;

        while (learnset[sLearningMoveTableID].level != level)
        {
            sLearningMoveTableID++;
            if (learnset[sLearningMoveTableID].move == LEVEL_UP_MOVE_END)
                return MOVE_NONE;
        }
    }

    //  Handler for Pokémon whose moves change upon form change.
    //  For example, if Zacian or Zamazenta should learn Iron Head,
    //  they're prevented from doing if they have Behemoth Blade/Bash,
    //  since it transforms into them while in their Crowned forms.
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (u32 i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_END_BATTLE
            && learnset[sLearningMoveTableID].move == formChanges[i].param3)
        {
            for (u32 j = 0; j < MAX_MON_MOVES; j++)
            {
                if (formChanges[i].param2 == GetMonData(mon, MON_DATA_MOVE1 + j))
                {
                    sLearningMoveTableID++;
                    return MON_ALREADY_KNOWS_MOVE;
                }
            }
        }
    }

    if (learnset[sLearningMoveTableID].level == level)
    {
        gMoveToLearn = learnset[sLearningMoveTableID].move;
        sLearningMoveTableID++;
        retVal = GiveMoveToMon(mon, gMoveToLearn);
    }

    return retVal;
}

enum Move MonTryLearningNewMove(struct Pokemon *mon, bool8 firstMove)
{
    return MonTryLearningNewMoveAtLevel(mon, firstMove, GetMonData(mon, MON_DATA_LEVEL));
}

void DeleteFirstMoveAndGiveMoveToMon(struct Pokemon *mon, enum Move move)
{
    s32 i;
    enum Move moves[MAX_MON_MOVES];

    for (i = 0; i < MAX_MON_MOVES - 1; i++)
    {
        moves[i] = GetMonData(mon, MON_DATA_MOVE2 + i);
    }

    moves[MAX_MON_MOVES - 1] = move;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        SetMonData(mon, MON_DATA_MOVE1 + i, &moves[i]);
    }
}

u32 CountAliveMonsInBattle(u8 caseId, enum BattlerId battler)
{
    enum BattlerId i;
    u32 aliveMonCount = 0;

    switch (caseId)
    {
    case BATTLE_ALIVE_EXCEPT_BATTLER:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (i != battler && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    case BATTLE_ALIVE_EXCEPT_BATTLER_SIDE:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (i != battler && i != GetPartnerBattler(battler) && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    case BATTLE_ALIVE_SIDE:
        for (i = 0; i < gBattlersCount; i++)
        {
            if (IsBattlerAlly(i, battler) && IsBattlerAlive(i))
                aliveMonCount++;
        }
        break;
    }

    return aliveMonCount;
}

u8 GetMonGender(struct Pokemon *mon)
{
    return GetBoxMonGender(&mon->box);
}

u8 GetBoxMonGender(struct BoxPokemon *boxMon)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES);
    u32 personality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);

    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        return gSpeciesInfo[species].genderRatio;
    }

    if (gSpeciesInfo[species].genderRatio > (personality & 0xFF))
        return MON_FEMALE;
    else
        return MON_MALE;
}

u8 GetGenderFromSpeciesAndPersonality(enum Species species, u32 personality)
{
    switch (gSpeciesInfo[species].genderRatio)
    {
    case MON_MALE:
    case MON_FEMALE:
    case MON_GENDERLESS:
        return gSpeciesInfo[species].genderRatio;
    }

    if (gSpeciesInfo[species].genderRatio > (personality & 0xFF))
        return MON_FEMALE;
    else
        return MON_MALE;
}

bool32 IsPersonalityFemale(enum Species species, u32 personality)
{
    return GetGenderFromSpeciesAndPersonality(species, personality) == MON_FEMALE;
}

enum Species GetUnownSpeciesId(u32 personality)
{
    u16 unownLetter = GetUnownLetterByPersonality(personality);

    if (unownLetter == 0)
        return SPECIES_UNOWN;
    return unownLetter + SPECIES_UNOWN_B - 1;
}

void SetMultiuseSpriteTemplateToPokemon(enum Species speciesTag, enum BattlerPosition battlerPosition)
{
    if (gMonSpritesGfxPtr != NULL)
        gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
    else if (sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_A])
        gMultiuseSpriteTemplate = sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_A]->templates[battlerPosition];
    else if (sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_B])
        gMultiuseSpriteTemplate = sMonSpritesGfxManagers[MON_SPR_GFX_MANAGER_B]->templates[battlerPosition];
    else
        gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];

    gMultiuseSpriteTemplate.paletteTag = speciesTag;
    if (battlerPosition == B_POSITION_PLAYER_LEFT || battlerPosition == B_POSITION_PLAYER_RIGHT)
        gMultiuseSpriteTemplate.anims = gAnims_MonPic;
    else
    {
        if (speciesTag > SPECIES_SHINY_TAG)
            speciesTag = speciesTag - SPECIES_SHINY_TAG;

        speciesTag = SanitizeSpeciesId(speciesTag);
        if (gSpeciesInfo[speciesTag].frontAnimFrames != NULL)
            gMultiuseSpriteTemplate.anims = gSpeciesInfo[speciesTag].frontAnimFrames;
        else
            gMultiuseSpriteTemplate.anims = gSpeciesInfo[SPECIES_NONE].frontAnimFrames;
    }
}

void SetMultiuseSpriteTemplateToTrainerBack(enum TrainerPicID trainerPicId, enum BattlerPosition battlerPosition)
{
    gMultiuseSpriteTemplate.paletteTag = GetTrainerPicTag(trainerPicId, FALSE);
    if (battlerPosition == B_POSITION_PLAYER_LEFT || battlerPosition == B_POSITION_PLAYER_RIGHT)
    {
        gMultiuseSpriteTemplate = sTrainerBackSpriteTemplate;
        gMultiuseSpriteTemplate.images = GetTrainerBackPicImage(trainerPicId);
        gMultiuseSpriteTemplate.anims = GetTrainerBackPicAnims(trainerPicId);
    }
    else
    {
        if (gMonSpritesGfxPtr != NULL)
            gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
        else
            gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];
        gMultiuseSpriteTemplate.anims = gAnims_Trainer;
    }
}

void SetMultiuseSpriteTemplateToTrainerFront(enum TrainerPicID trainerPicId, enum BattlerPosition battlerPosition)
{
    if (gMonSpritesGfxPtr != NULL)
        gMultiuseSpriteTemplate = gMonSpritesGfxPtr->templates[battlerPosition];
    else
        gMultiuseSpriteTemplate = gBattlerSpriteTemplates[battlerPosition];

    gMultiuseSpriteTemplate.paletteTag = GetTrainerPicTag(trainerPicId, TRUE);
    gMultiuseSpriteTemplate.anims = gAnims_Trainer;
}

/* GameFreak called GetMonData with either 2 or 3 arguments, for type
 * safety we have a GetMonData macro (in include/pokemon.h) which
 * dispatches to either GetMonData2 or GetMonData3 based on the number
 * of arguments. */
u32 GetMonData3(struct Pokemon *mon, s32 field, u8 *data)
{
    u32 ret;

    switch (field)
    {
    case MON_DATA_STATUS:
        ret = mon->status;
        break;
    case MON_DATA_LEVEL:
        ret = mon->level;
        break;
    case MON_DATA_HP:
        ret = mon->hp;
        break;
    case MON_DATA_MAX_HP:
        ret = mon->maxHP;
        break;
    case MON_DATA_ATK:
        ret = mon->attack;
        break;
    case MON_DATA_DEF:
        ret = mon->defense;
        break;
    case MON_DATA_SPEED:
        ret = mon->speed;
        break;
    case MON_DATA_SPATK:
        ret = mon->spAttack;
        break;
    case MON_DATA_SPDEF:
        ret = mon->spDefense;
        break;
    case MON_DATA_MAIL:
        ret = mon->mail;
        break;
    default:
        ret = GetBoxMonData(&mon->box, field, data);
        break;
    }
    return ret;
}

u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    return GetMonData3(mon, field, NULL);
}

union EvolutionTracker
{
    u16 combinedValue:10;
    struct {
        u16 tracker1: 5;
        u16 tracker2: 5;
    };
};

static ALWAYS_INLINE struct PokemonData *GetPokemonData(struct BoxPokemon *boxMon)
{
    return &boxMon->data;
}

/* GameFreak called GetBoxMonData with either 2 or 3 arguments, for type
 * safety we have a GetBoxMonData macro (in include/pokemon.h) which
 * dispatches to either GetBoxMonData2 or GetBoxMonData3 based on the
 * number of arguments. */
u32 GetBoxMonData3(struct BoxPokemon *boxMon, s32 field, u8 *data)
{
    s32 i;
    u32 retVal = 0;

    if (field > MON_DATA_HEADER_END)
    {
        switch (field)
        {
        case MON_DATA_NICKNAME:
        case MON_DATA_NICKNAME10:
        {
            if (GetPokemonData(boxMon)->isEgg)
            {
                StringCopy(data, gText_EggNickname);
                retVal = StringLength(data);
            }
            else if (boxMon->language == LANGUAGE_JAPANESE)
            {
                data[0] = EXT_CTRL_CODE_BEGIN;
                data[1] = EXT_CTRL_CODE_JPN;

                for (retVal = 2, i = 0;
                    i < 5 && boxMon->nickname[i] != EOS;
                    data[retVal] = boxMon->nickname[i], retVal++, i++) {}

                data[retVal++] = EXT_CTRL_CODE_BEGIN;
                data[retVal++] = EXT_CTRL_CODE_ENG;
                data[retVal] = EOS;
            }
            else
            {
                retVal = 0;
                while (retVal < min(sizeof(boxMon->nickname), POKEMON_NAME_LENGTH))
                {
                    data[retVal] = boxMon->nickname[retVal];
                    retVal++;
                }

                // Vanilla Pokémon have 0s in nickname11 and nickname12
                // so if both are 0 we assume that this is a vanilla
                // Pokémon and replace them with EOS. This means that
                // two CHAR_SPACE at the end of a nickname are trimmed.
                struct PokemonData *monData = GetPokemonData(boxMon);
                if (field != MON_DATA_NICKNAME10 && POKEMON_NAME_LENGTH >= 12)
                {
                    if (monData->nickname11 == 0 && monData->nickname12 == 0)
                    {
                        data[retVal++] = EOS;
                        data[retVal++] = EOS;
                    }
                    else
                    {
                        data[retVal++] = monData->nickname11;
                        data[retVal++] = monData->nickname12;
                    }
                }
                else if (field != MON_DATA_NICKNAME10 && POKEMON_NAME_LENGTH >= 11)
                {
                    if (monData->nickname11 == 0)
                    {
                        data[retVal++] = EOS;
                    }
                    else
                    {
                        data[retVal++] = monData->nickname11;
                    }
                }

                data[retVal] = EOS;
            }
            break;
        }
        case MON_DATA_SPECIES:
            retVal = GetPokemonData(boxMon)->species;
            break;
        case MON_DATA_HELD_ITEM:
            retVal = GetPokemonData(boxMon)->heldItem;
            break;
        case MON_DATA_EXP:
            retVal = GetPokemonData(boxMon)->experience;
            break;
        case MON_DATA_EGG_CYCLES:
            retVal = GetPokemonData(boxMon)->eggCycles;
            break;
        case MON_DATA_MOVE1:
            retVal = GetPokemonData(boxMon)->move1;
            break;
        case MON_DATA_MOVE2:
            retVal = GetPokemonData(boxMon)->move2;
            break;
        case MON_DATA_MOVE3:
            retVal = GetPokemonData(boxMon)->move3;
            break;
        case MON_DATA_MOVE4:
            retVal = GetPokemonData(boxMon)->move4;
            break;
        case MON_DATA_PP1:
            retVal = GetMovePP(GetPokemonData(boxMon)->move1);
            break;
        case MON_DATA_PP2:
            retVal = GetMovePP(GetPokemonData(boxMon)->move2);
            break;
        case MON_DATA_PP3:
            retVal = GetMovePP(GetPokemonData(boxMon)->move3);
            break;
        case MON_DATA_PP4:
            retVal = GetMovePP(GetPokemonData(boxMon)->move4);
            break;
        case MON_DATA_HP_EV:
            retVal = GetPokemonData(boxMon)->hpEV;
            break;
        case MON_DATA_ATK_EV:
            retVal = GetPokemonData(boxMon)->attackEV;
            break;
        case MON_DATA_DEF_EV:
            retVal = GetPokemonData(boxMon)->defenseEV;
            break;
        case MON_DATA_SPEED_EV:
            retVal = GetPokemonData(boxMon)->speedEV;
            break;
        case MON_DATA_SPATK_EV:
            retVal = GetPokemonData(boxMon)->spAttackEV;
            break;
        case MON_DATA_SPDEF_EV:
            retVal = GetPokemonData(boxMon)->spDefenseEV;
            break;
        case MON_DATA_MET_LOCATION:
            retVal = GetPokemonData(boxMon)->metLocation;
            break;
        case MON_DATA_MET_LEVEL:
            retVal = GetPokemonData(boxMon)->metLevel;
            break;
        case MON_DATA_MET_GAME:
            retVal = GetPokemonData(boxMon)->metGame;
            break;
        case MON_DATA_POKEBALL:
            retVal = GetPokemonData(boxMon)->pokeball;
            break;
        case MON_DATA_OT_GENDER:
            retVal = GetPokemonData(boxMon)->otGender;
            break;
        case MON_DATA_IS_EGG:
            retVal = GetPokemonData(boxMon)->isEgg;
            break;
        case MON_DATA_ABILITY_NUM:
            retVal = GetPokemonData(boxMon)->abilityNum;
            break;
        case MON_DATA_SPECIES_OR_EGG:
            retVal = GetPokemonData(boxMon)->species;
            if (retVal && GetPokemonData(boxMon)->isEgg)
                retVal = SPECIES_EGG;
            break;
        case MON_DATA_KNOWN_MOVES:
            if (GetPokemonData(boxMon)->species && !GetPokemonData(boxMon)->isEgg)
            {
                struct PokemonData *monData = GetPokemonData(boxMon);
                u16 *moves = (u16 *)data;
                s32 i = 0;

                while (moves[i] != MOVES_COUNT)
                {
                    enum Move move = moves[i];
                    if (monData->move1 == move
                        || monData->move2 == move
                        || monData->move3 == move
                        || monData->move4 == move)
                        retVal |= (1u << i);
                    i++;
                }
            }
            break;
        case MON_DATA_IS_SHADOW:
            retVal = GetPokemonData(boxMon)->isShadow;
            break;
        case MON_DATA_TERA_TYPE:
            {
                struct PokemonData *monData = GetPokemonData(boxMon);
                if (gSpeciesInfo[monData->species].forceTeraType)
                {
                    retVal = gSpeciesInfo[monData->species].forceTeraType;
                }
                else if (monData->teraType == TYPE_NONE) // Tera Type hasn't been modified so we can just use the personality
                {
                    const enum Type *types = gSpeciesInfo[monData->species].types;
                    retVal = (boxMon->personality & 0x1) == 0 ? types[0] : types[1];
                }
                else
                {
                    retVal = monData->teraType;
                }
            }
            break;
        case MON_DATA_EVOLUTION_TRACKER:
            {
                struct PokemonData *monData = GetPokemonData(boxMon);
                retVal = (union EvolutionTracker) {
                    .tracker1 = monData->evolutionTracker1,
                    .tracker2 = monData->evolutionTracker2,
                }.combinedValue;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (field)
        {
        case MON_DATA_STATUS:
            retVal = UncompressStatus(boxMon->compressedStatus);
            break;
        case MON_DATA_HP_LOST:
            retVal = boxMon->hpLost;
            break;
        case MON_DATA_PERSONALITY:
            retVal = boxMon->personality;
            break;
        case MON_DATA_OT_ID:
            retVal = boxMon->otId;
            break;
        case MON_DATA_LANGUAGE:
            retVal = boxMon->language;
            break;
        case MON_DATA_OT_NAME:
        {
            retVal = 0;

            while (retVal < PLAYER_NAME_LENGTH)
            {
                data[retVal] = boxMon->otName[retVal];
                retVal++;
            }

            data[retVal] = EOS;
            break;
        }
        case MON_DATA_MARKINGS:
            retVal = boxMon->markings;
            break;
        case MON_DATA_IS_SHINY:
        {
            u32 shinyValue = GET_SHINY_VALUE(boxMon->otId, boxMon->personality);
            retVal = (shinyValue < SHINY_ODDS) ^ boxMon->shinyModifier;
            break;
        }
        case MON_DATA_HIDDEN_NATURE:
        {
            u32 nature = GetNatureFromPersonality(boxMon->personality);
            retVal = nature ^ boxMon->hiddenNatureModifier;
            break;
        }
        case MON_DATA_DAYS_SINCE_FORM_CHANGE:
            retVal = boxMon->daysSinceFormChange;
            break;
        default:
            break;
        }
    }

    return retVal;
}

u32 GetBoxMonData2(struct BoxPokemon *boxMon, s32 field)
{
    return GetBoxMonData3(boxMon, field, NULL);
}

#define SET8(lhs) (lhs) = *data
#define SET16(lhs) (lhs) = data[0] + (data[1] << 8)
#define SET32(lhs) (lhs) = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)
//
// Prefer SET_BY_WIDTH for fields whose types might be extended (e.g.
// anything whose typedef is in gametypes.h).
//
#define SET_BY_WIDTH(lhs) \
    do { \
       if (sizeof(lhs) == 1) \
          SET8(lhs); \
       else if (sizeof(lhs) == 2) \
          SET16(lhs); \
       else if (sizeof(lhs) == 4) \
          SET32(lhs); \
   } while (0)

void SetMonData(struct Pokemon *mon, s32 field, const void *dataArg)
{
    const u8 *data = dataArg;

    switch (field)
    {
    case MON_DATA_STATUS:
        SET32(mon->status);
        SetBoxMonData(&mon->box, MON_DATA_STATUS, dataArg);
        break;
    case MON_DATA_LEVEL:
        SET8(mon->level);
        break;
    case MON_DATA_HP:
    {
        u32 hpLost;
        SET16(mon->hp);
        hpLost = mon->maxHP - mon->hp;
        SetBoxMonData(&mon->box, MON_DATA_HP_LOST, &hpLost);
        break;
    }
    case MON_DATA_HP_LOST:
    {
        u32 hpLost;
        SET16(hpLost);
        mon->hp = mon->maxHP - hpLost;
        SetBoxMonData(&mon->box, MON_DATA_HP_LOST, &hpLost);
        break;
    }
    case MON_DATA_MAX_HP:
        SET16(mon->maxHP);
        break;
    case MON_DATA_ATK:
        SET16(mon->attack);
        break;
    case MON_DATA_DEF:
        SET16(mon->defense);
        break;
    case MON_DATA_SPEED:
        SET16(mon->speed);
        break;
    case MON_DATA_SPATK:
        SET16(mon->spAttack);
        break;
    case MON_DATA_SPDEF:
        SET16(mon->spDefense);
        break;
    case MON_DATA_MAIL:
        SET8(mon->mail);
        break;
    case MON_DATA_SPECIES_OR_EGG:
        break;
    default:
        SetBoxMonData(&mon->box, field, data);
        break;
    }
}

void SetBoxMonData(struct BoxPokemon *boxMon, s32 field, const void *dataArg)
{
    const u8 *data = dataArg;

    if (field > MON_DATA_HEADER_END)
    {
        switch (field)
        {
        case MON_DATA_NICKNAME:
        case MON_DATA_NICKNAME10:
        {
            s32 i;
            struct PokemonData *monData = GetPokemonData(boxMon);
            for (i = 0; i < min(sizeof(boxMon->nickname), POKEMON_NAME_LENGTH); i++)
                boxMon->nickname[i] = data[i];
            if (field != MON_DATA_NICKNAME10)
            {
                if (POKEMON_NAME_LENGTH >= 11)
                    monData->nickname11 = data[10];
                if (POKEMON_NAME_LENGTH >= 12)
                    monData->nickname12 = data[11];
            }
            else
            {
                monData->nickname11 = EOS;
                monData->nickname12 = EOS;
            }
            break;
        }
        case MON_DATA_SPECIES:
            SET16(GetPokemonData(boxMon)->species);
            break;
        case MON_DATA_HELD_ITEM:
            SET16(GetPokemonData(boxMon)->heldItem);
            break;
        case MON_DATA_EXP:
            SET32(GetPokemonData(boxMon)->experience);
            break;
        case MON_DATA_EGG_CYCLES:
            SET8(GetPokemonData(boxMon)->eggCycles);
            break;
        case MON_DATA_MOVE1:
            SET16(GetPokemonData(boxMon)->move1);
            break;
        case MON_DATA_MOVE2:
            SET16(GetPokemonData(boxMon)->move2);
            break;
        case MON_DATA_MOVE3:
            SET16(GetPokemonData(boxMon)->move3);
            break;
        case MON_DATA_MOVE4:
            SET16(GetPokemonData(boxMon)->move4);
            break;
        case MON_DATA_HP_EV:
            SET8(GetPokemonData(boxMon)->hpEV);
            break;
        case MON_DATA_ATK_EV:
            SET8(GetPokemonData(boxMon)->attackEV);
            break;
        case MON_DATA_DEF_EV:
            SET8(GetPokemonData(boxMon)->defenseEV);
            break;
        case MON_DATA_SPEED_EV:
            SET8(GetPokemonData(boxMon)->speedEV);
            break;
        case MON_DATA_SPATK_EV:
            SET8(GetPokemonData(boxMon)->spAttackEV);
            break;
        case MON_DATA_SPDEF_EV:
            SET8(GetPokemonData(boxMon)->spDefenseEV);
            break;
        case MON_DATA_MET_LOCATION:
            SET8(GetPokemonData(boxMon)->metLocation);
            break;
        case MON_DATA_MET_LEVEL:
            SET8(GetPokemonData(boxMon)->metLevel);
            break;
        case MON_DATA_MET_GAME:
            SET8(GetPokemonData(boxMon)->metGame);
            break;
        case MON_DATA_POKEBALL:
            SET8(GetPokemonData(boxMon)->pokeball);
            break;
        case MON_DATA_OT_GENDER:
            SET8(GetPokemonData(boxMon)->otGender);
            break;
        case MON_DATA_IS_EGG:
            SET8(GetPokemonData(boxMon)->isEgg);
            break;
        case MON_DATA_ABILITY_NUM:
            SET8(GetPokemonData(boxMon)->abilityNum);
            break;
        case MON_DATA_IS_SHADOW:
            SET8(GetPokemonData(boxMon)->isShadow);
            break;
        case MON_DATA_TERA_TYPE:
            SET8(GetPokemonData(boxMon)->teraType);
            break;
        case MON_DATA_EVOLUTION_TRACKER:
        {
            union EvolutionTracker evoTracker;
            struct PokemonData *monData = GetPokemonData(boxMon);
            SET32(evoTracker.combinedValue);
            monData->evolutionTracker1 = evoTracker.tracker1;
            monData->evolutionTracker2 = evoTracker.tracker2;
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (field)
        {
        case MON_DATA_STATUS:
        {
            u32 status;
            SET32(status);
            boxMon->compressedStatus = CompressStatus(status);
            break;
        }
        case MON_DATA_HP_LOST:
            SET16(boxMon->hpLost);
            break;
        case MON_DATA_PERSONALITY:
            SET32(boxMon->personality);
            break;
        case MON_DATA_OT_ID:
            SET32(boxMon->otId);
            break;
        case MON_DATA_LANGUAGE:
            SET8(boxMon->language);
            break;
        case MON_DATA_OT_NAME:
        {
            s32 i;
            for (i = 0; i < PLAYER_NAME_LENGTH; i++)
                boxMon->otName[i] = data[i];
            break;
        }
        case MON_DATA_MARKINGS:
            SET8(boxMon->markings);
            break;
        case MON_DATA_IS_SHINY:
        {
            u32 shinyValue = GET_SHINY_VALUE(boxMon->otId, boxMon->personality);
            bool32 isShiny;
            SET8(isShiny);
            boxMon->shinyModifier = (shinyValue < SHINY_ODDS) ^ isShiny;
            break;
        }
        case MON_DATA_HIDDEN_NATURE:
        {
            u32 nature = GetNatureFromPersonality(boxMon->personality);
            u32 hiddenNature;
            SET8(hiddenNature);
            boxMon->hiddenNatureModifier = nature ^ hiddenNature;
            break;
        }
        case MON_DATA_DAYS_SINCE_FORM_CHANGE:
            SET8(boxMon->daysSinceFormChange);
            break;
        }
    }

}

void CopyMon(void *dest, void *src, size_t size)
{
    memcpy(dest, src, size);
}

u8 GiveCapturedMonToPlayer(struct Pokemon *mon)
{
    s32 i;

    SetMonData(mon, MON_DATA_OT_NAME, gSaveData.playerName);
    SetMonData(mon, MON_DATA_OT_GENDER, &gSaveData.playerGender);
    SetMonData(mon, MON_DATA_OT_ID, gSaveData.playerTrainerId);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            break;
    }

    if (i >= PARTY_SIZE)
        return CopyMonToPC(mon);

    CopyMon(&gParties[B_TRAINER_PLAYER][i], mon, sizeof(*mon));
    gPartiesCount[B_TRAINER_PLAYER] = i + 1;
    return MON_GIVEN_TO_PARTY;
}

u8 CopyMonToPC(struct Pokemon *mon)
{
    s32 boxNo, boxPos;

    SetPCBoxToSendMon(VarGet(VAR_PC_BOX_TO_SEND_MON));

    boxNo = StorageGetCurrentBox();

    do
    {
        for (boxPos = 0; boxPos < IN_BOX_COUNT; boxPos++)
        {
            struct BoxPokemon *checkingMon = GetBoxedMonPtr(boxNo, boxPos);
            if (GetBoxMonData(checkingMon, MON_DATA_SPECIES) == SPECIES_NONE)
            {
                CopyMon(checkingMon, &mon->box, sizeof(mon->box));
                gSpecialVar_MonBoxId = boxNo;
                gSpecialVar_MonBoxPos = boxPos;
                if (GetPCBoxToSendMon() != boxNo)
                    FlagClear(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
                VarSet(VAR_PC_BOX_TO_SEND_MON, boxNo);
                if (PCStorageCarrying())
                {
                    PCStorageCarryEnd();
                    PCStorageAutosave();
                }
                return MON_GIVEN_TO_PC;
            }
        }

        boxNo++;
        if (boxNo == TOTAL_BOXES_COUNT)
            boxNo = 0;
    } while (boxNo != StorageGetCurrentBox());

    return MON_CANT_GIVE;
}

u8 CalculatePartyCount(enum BattleTrainer trainer)
{
    u32 partyCount = 0;

    while (partyCount < PARTY_SIZE
        && GetMonData(&gParties[trainer][partyCount], MON_DATA_SPECIES) != SPECIES_NONE)
    {
        partyCount++;
    }

    return partyCount;
}

u8 CalculatePartyCountOfSide(enum BattlerId battler)
{
    return CalculatePartyCount(GetBattlerTrainer(battler)) + (BattleSideHasTwoTrainers(battler & BIT_SIDE) ? CalculatePartyCount(GetBattlerTrainer(GetPartnerBattler(battler))) : 0);
}

u8 CalculatePlayerPartyCount(void)
{
    gPartiesCount[B_TRAINER_PLAYER] = CalculatePartyCount(B_TRAINER_PLAYER);
    return gPartiesCount[B_TRAINER_PLAYER];
}

u8 CalculatePartnerPartyCount(void)
{
    gPartiesCount[B_TRAINER_PARTNER] = CalculatePartyCount(B_TRAINER_PARTNER);
    return gPartiesCount[B_TRAINER_PARTNER];
}

u8 CalculateEnemyPartyCount(void)
{
    gPartiesCount[B_TRAINER_OPPONENT_A] = CalculatePartyCount(B_TRAINER_OPPONENT_A);
    gPartiesCount[B_TRAINER_OPPONENT_B] = CalculatePartyCount(B_TRAINER_OPPONENT_B);
    return gPartiesCount[B_TRAINER_OPPONENT_A] + gPartiesCount[B_TRAINER_OPPONENT_B];
}

u8 GetMonsStateToDoubles(void)
{
    s32 aliveCount = 0;
    s32 i;
    CalculatePlayerPartyCount();

    if (OW_DOUBLE_APPROACH_WITH_ONE_MON)
        return PLAYER_HAS_TWO_USABLE_MONS;

    if (gPartiesCount[B_TRAINER_PLAYER] == 1)
        return gPartiesCount[B_TRAINER_PLAYER]; // PLAYER_HAS_ONE_MON

    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) != 0
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE)
            aliveCount++;
    }

    return (aliveCount > 1) ? PLAYER_HAS_TWO_USABLE_MONS : PLAYER_HAS_ONE_USABLE_MON;
}

u8 GetMonsStateToDoubles_2(void)
{
    s32 aliveCount = 0;
    s32 i;

    if (OW_DOUBLE_APPROACH_WITH_ONE_MON
     || FollowerNPCIsBattlePartner())
        return PLAYER_HAS_TWO_USABLE_MONS;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        if (species != SPECIES_EGG && species != SPECIES_NONE
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) != 0)
            aliveCount++;
    }

    if (aliveCount == 1)
        return PLAYER_HAS_ONE_MON; // may have more than one, but only one is alive

    return (aliveCount > 1) ? PLAYER_HAS_TWO_USABLE_MONS : PLAYER_HAS_ONE_USABLE_MON;
}

enum Ability GetAbilityBySpecies(enum Species species, u8 abilityNum)
{
    int i;

    if (abilityNum < NUM_ABILITY_SLOTS)
        gLastUsedAbility = GetSpeciesAbility(species, abilityNum);
    else
        gLastUsedAbility = ABILITY_NONE;

    if (abilityNum >= NUM_NORMAL_ABILITY_SLOTS) // if abilityNum is empty hidden ability, look for other hidden abilities
    {
        for (i = NUM_NORMAL_ABILITY_SLOTS; i < NUM_ABILITY_SLOTS && gLastUsedAbility == ABILITY_NONE; i++)
        {
            gLastUsedAbility = GetSpeciesAbility(species, i);
        }
    }

    for (i = 0; i < NUM_ABILITY_SLOTS && gLastUsedAbility == ABILITY_NONE; i++) // look for any non-empty ability
    {
        gLastUsedAbility = GetSpeciesAbility(species, i);
    }

    return gLastUsedAbility;
}

enum Ability GetMonAbility(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM);
    return GetAbilityBySpeciesAndItem(species, abilityNum, GetMonData(mon, MON_DATA_HELD_ITEM));
}

enum Ability GetSpeciesApexAbility(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].apexAbility;
}

enum Ability GetAbilityBySpeciesAndItem(enum Species species, u8 abilityNum, enum Item item)
{
    if (item == ITEM_APEX_METEORITE)
        return gLastUsedAbility = GetSpeciesApexAbility(species);
    return GetAbilityBySpecies(species, abilityNum);
}

bool8 IsPlayerPartyAndPokemonStorageFull(void)
{
    s32 i;

    for (i = 0; i < PARTY_SIZE; i++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
            return FALSE;

    return IsPokemonStorageFull();
}

bool8 IsPokemonStorageFull(void)
{
    s32 i, j;

    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
        for (j = 0; j < IN_BOX_COUNT; j++)
            if (GetBoxMonDataAt(i, j, MON_DATA_SPECIES) == SPECIES_NONE)
                return FALSE;

    return TRUE;
}

const u8 *GetSpeciesName(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].speciesName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].speciesName;
    return gSpeciesInfo[species].speciesName;
}

const u8 *GetSpeciesCategory(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].categoryName[0] == 0)
        return gSpeciesInfo[SPECIES_NONE].categoryName;
    return gSpeciesInfo[species].categoryName;
}

const u8 *GetSpeciesPokedexDescription(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (gSpeciesInfo[species].description == NULL)
        return gSpeciesInfo[SPECIES_NONE].description;
    return gSpeciesInfo[species].description;
}

u32 GetSpeciesHeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].height;
}

u32 GetSpeciesWeight(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].weight;
}

enum Type GetSpeciesType(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].types[slot];
}

enum Ability GetSpeciesAbility(enum Species species, u8 slot)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].abilities[slot];
}

u32 GetSpeciesBaseHP(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseHP;
}

u32 GetSpeciesBaseAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseAttack;
}

u32 GetSpeciesBaseDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseDefense;
}

u32 GetSpeciesBaseSpAttack(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpAttack;
}

u32 GetSpeciesBaseSpDefense(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpDefense;
}

u32 GetSpeciesBaseSpeed(enum Species species)
{
    return gSpeciesInfo[SanitizeSpeciesId(species)].baseSpeed;
}

u32 GetSpeciesBaseStat(enum Species species, u32 statIndex)
{
    switch (statIndex)
    {
    case STAT_HP:
        return GetSpeciesBaseHP(species);
    case STAT_ATK:
        return GetSpeciesBaseAttack(species);
    case STAT_DEF:
        return GetSpeciesBaseDefense(species);
    case STAT_SPEED:
        return GetSpeciesBaseSpeed(species);
    case STAT_SPATK:
        return GetSpeciesBaseSpAttack(species);
    case STAT_SPDEF:
        return GetSpeciesBaseSpDefense(species);
    }
    return 0;
}

u32 GetSpeciesBaseStatTotal(enum Species species)
{
    u32 total = 0;

    for (u32 i = 0; i < NUM_STATS; i++)
        total += GetSpeciesBaseStat(species, i);

    return total;
}

const struct LevelUpMove *GetSpeciesLevelUpLearnset(enum Species species)
{
    const struct LevelUpMove *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].levelUpLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].levelUpLearnset;
    return learnset;
}

const u16 *GetSpeciesTeachableLearnset(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].teachableLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].teachableLearnset;
    return learnset;
}

const u16 *GetSpeciesEggMoves(enum Species species)
{
    const u16 *learnset = gSpeciesInfo[SanitizeSpeciesId(species)].eggMoveLearnset;
    if (learnset == NULL)
        return gSpeciesInfo[SPECIES_NONE].eggMoveLearnset;
    return learnset;
}

//only used in test assumptions at the moment
bool32 SpeciesHasEggMove(enum Species species, enum Move move)
{
    const u16 *learnset = GetSpeciesEggMoves(species);
    for (u32 i = 0; learnset[i] != MOVE_UNAVAILABLE; i++)
    {
        if (learnset[i] == move)
            return TRUE;
    }
    return FALSE;
}

const struct Evolution *GetSpeciesEvolutions(enum Species species)
{
    const struct Evolution *evolutions = gSpeciesInfo[SanitizeSpeciesId(species)].evolutions;
    if (evolutions == NULL)
        return gSpeciesInfo[SPECIES_NONE].evolutions;
    return evolutions;
}

const u16 *GetSpeciesFormTable(enum Species species)
{
    const u16 *formTable = gSpeciesInfo[SanitizeSpeciesId(species)].formSpeciesIdTable;
    if (formTable == NULL)
        return gSpeciesInfo[SPECIES_NONE].formSpeciesIdTable;
    return formTable;
}

const struct FormChange *GetSpeciesFormChanges(enum Species species)
{
    const struct FormChange *formChanges = gSpeciesInfo[SanitizeSpeciesId(species)].formChangeTable;
    if (formChanges == NULL)
        return gSpeciesInfo[SPECIES_NONE].formChangeTable;
    return formChanges;
}

void PokemonToBattleMon(struct Pokemon *src, struct BattlePokemon *dst)
{
    s32 i;
    u8 nickname[POKEMON_NAME_BUFFER_SIZE];

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        dst->moves[i] = GetMonData(src, MON_DATA_MOVE1 + i);
        dst->pp[i] = GetMonData(src, MON_DATA_PP1 + i);
    }

    dst->species = GetMonData(src, MON_DATA_SPECIES);
    dst->item = GetMonData(src, MON_DATA_HELD_ITEM);
    dst->experience = GetMonData(src, MON_DATA_EXP);
    dst->personality = GetMonData(src, MON_DATA_PERSONALITY);
    dst->status1 = GetMonData(src, MON_DATA_STATUS);
    dst->level = GetMonData(src, MON_DATA_LEVEL);
    dst->hp = GetMonData(src, MON_DATA_HP);
    dst->maxHP = GetMonData(src, MON_DATA_MAX_HP);
    dst->attack = GetMonData(src, MON_DATA_ATK);
    dst->defense = GetMonData(src, MON_DATA_DEF);
    dst->speed = GetMonData(src, MON_DATA_SPEED);
    dst->spAttack = GetMonData(src, MON_DATA_SPATK);
    dst->spDefense = GetMonData(src, MON_DATA_SPDEF);
    dst->abilityNum = GetMonData(src, MON_DATA_ABILITY_NUM);
    dst->otId = GetMonData(src, MON_DATA_OT_ID);
    dst->types[0] = GetSpeciesType(dst->species, 0);
    dst->types[1] = GetSpeciesType(dst->species, 1);
    dst->types[2] = TYPE_MYSTERY;
    dst->isShiny = IsMonShiny(src);
    dst->ability = GetAbilityBySpeciesAndItem(dst->species, dst->abilityNum, dst->item);
    GetMonData(src, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(dst->nickname, nickname);
    GetMonData(src, MON_DATA_OT_NAME, dst->otName);

    for (i = 0; i < NUM_BATTLE_STATS; i++)
        dst->statStages[i] = DEFAULT_STAT_STAGE;

    memset(&dst->volatiles, 0, sizeof(struct Volatiles));
}

bool8 ExecuteTableBasedItemEffect(struct Pokemon *mon, enum Item item, enum PartySlot partyIndex, enum MoveSlot moveIndex)
{
    return PokemonUseItemEffects(mon, item, partyIndex, moveIndex, FALSE);
}

// EXP candies store an index for this table in their holdEffectParam.
const u32 sExpCandyExperienceTable[] = {
    [EXP_100 - 1] = 100,
    [EXP_800 - 1] = 800,
    [EXP_3000 - 1] = 3000,
    [EXP_10000 - 1] = 10000,
    [EXP_30000 - 1] = 30000,
};

// Returns TRUE if the item has no effect on the Pokémon, FALSE otherwise
bool8 PokemonUseItemEffects(struct Pokemon *mon, enum Item item, enum PartySlot partyIndex, enum MoveSlot moveIndex, bool8 usedByAI)
{
    u32 dataUnsigned;
    s32 dataSigned, evCap;
    s32 i;
    bool8 retVal = TRUE;
    const u8 *itemEffect;
    u8 itemEffectParam = ITEM_EFFECT_ARG_START;
    u32 temp1, temp2;
    enum BattlerId battler = MAX_BATTLERS_COUNT;
    u8 effectFlags;
    s8 evChange;
    u16 evCount;
    u8 levelBefore;
    bool8 didLevelUp = FALSE;
    bool8 isLevelUpItem;

    // Determine the EV cap to use
    u32 maxAllowedEVs = !B_EV_ITEMS_CAP ? MAX_TOTAL_EVS : GetCurrentEVCap();

    // Skip using the item if it won't do anything
    if (GetItemEffect(item) == NULL)
        return TRUE;

    // Get item effect
    itemEffect = GetItemEffect(item);
    isLevelUpItem = (itemEffect[3] & ITEM3_LEVEL_UP) != 0;
    levelBefore = GetMonData(mon, MON_DATA_LEVEL, NULL);

    // Do item effect
    for (i = 0; i < ITEM_EFFECT_ARG_START; i++)
    {
        switch (i)
        {

        // Handle ITEM0 effects (infatuation, Dire Hit, X Attack). ITEM0_SACRED_ASH is handled in party_menu.c
        // Now handled in item battle scripts.
        case 0:
            break;

        // Handle ITEM1 effects (in-battle stat boosting effects)
        // Now handled in item battle scripts.
        case 1:
            break;
        // Formerly used by the item effects of the X Sp. Atk and the X Accuracy
        case 2:
            break;

        // Handle ITEM3 effects (Guard Spec, Rare Candy, cure status)
        case 3:
            // Rare Candy / EXP Candy
            if ((itemEffect[i] & ITEM3_LEVEL_UP)
             && GetMonData(mon, MON_DATA_LEVEL) != MAX_LEVEL
             && !(B_RARE_CANDY_CAP && IsLevelCapEnabled() && GetMonData(mon, MON_DATA_LEVEL) >= GetCurrentLevelCap()))
            {
                u8 param = GetItemHoldEffectParam(item);
                dataUnsigned = 0;

                if (param == 0) // Rare Candy
                {
                    dataUnsigned = gExperienceTables[gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES)].growthRate][GetMonData(mon, MON_DATA_LEVEL) + 1];
                }
                else if (param - 1 < ARRAY_COUNT(sExpCandyExperienceTable)) // EXP Candies
                {
                    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
                    dataUnsigned = sExpCandyExperienceTable[param - 1] + GetMonData(mon, MON_DATA_EXP);

                    if (B_RARE_CANDY_CAP && IsLevelCapEnabled() && B_EXP_CAP_TYPE == EXP_CAP_HARD)
                    {
                        u32 currentLevelCap = GetCurrentLevelCap();
                        if (dataUnsigned > gExperienceTables[gSpeciesInfo[species].growthRate][currentLevelCap])
                            dataUnsigned = gExperienceTables[gSpeciesInfo[species].growthRate][currentLevelCap];
                    }
                    else if (dataUnsigned > gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL])
                    {
                        dataUnsigned = gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL];
                    }
                }

                if (dataUnsigned != 0) // Failsafe
                {
                    SetMonData(mon, MON_DATA_EXP, &dataUnsigned);
                    CalculateMonStats(mon);
                    if (GetMonData(mon, MON_DATA_LEVEL, NULL) > levelBefore)
                        didLevelUp = TRUE;
                    retVal = FALSE;
                }
            }

            // Cure status
            if ((itemEffect[i] & ITEM3_SLEEP) && HealStatusConditions(mon, STATUS1_SLEEP, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_POISON) && HealStatusConditions(mon, STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_BURN) && HealStatusConditions(mon, STATUS1_BURN, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_FREEZE) && HealStatusConditions(mon, STATUS1_ICY_ANY, battler) == 0)
                retVal = FALSE;
            if ((itemEffect[i] & ITEM3_PARALYSIS) && HealStatusConditions(mon, STATUS1_PARALYSIS, battler) == 0)
                retVal = FALSE;
            break;

        // Handle ITEM4 effects (Change HP/Atk EVs, HP heal, PP heal, Revive, and evolution stones)
        case 4:
            effectFlags = itemEffect[i];

            temp1 = 0;

            // Loop through and try each of the remaining ITEM4 effects
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    switch (temp1)
                    {
                    case 0: // ITEM4_EV_HP
                    case 1: // ITEM4_EV_ATK
                        evCount = GetMonEVCount(mon);
                        temp2 = itemEffect[itemEffectParam];
                        dataSigned = GetMonData(mon, sGetMonDataEVConstants[temp1]);
                        evChange = temp2;

                        if (evChange > 0) // Increasing EV (HP or Atk)
                        {
                            // Check if the total EV limit is reached
                            if (evCount >= maxAllowedEVs)
                                return TRUE;

                            // Ensure the increase does not exceed the max EV per stat (252)
                            evCap = (itemEffect[10] & ITEM10_IS_VITAMIN) ? EV_ITEM_RAISE_LIMIT : MAX_PER_STAT_EVS;

                            // Check if the per-stat limit is reached
                            if (dataSigned >= evCap)
                                return TRUE;  // Prevents item use if the per-stat cap is already reached

                            if (dataSigned + evChange > evCap)
                                temp2 = evCap - dataSigned;
                            else
                                temp2 = evChange;

                            // Ensure the total EVs do not exceed the maximum allowed (510)
                            if (evCount + temp2 > maxAllowedEVs)
                                temp2 = maxAllowedEVs - evCount;

                            // Prevent item use if no EVs can be increased
                            if (temp2 == 0)
                                return TRUE;

                            // Apply the EV increase
                            dataSigned += temp2;
                        }
                        else if (evChange < 0) // Decreasing EV (HP or Atk)
                        {
                            if (dataSigned == 0)
                            {
                                return TRUE;
                            }
                            dataSigned += evChange;
                            if (I_BERRY_EV_JUMP == GEN_4 && dataSigned > 100)
                                dataSigned = 100;
                            if (dataSigned < 0)
                                dataSigned = 0;
                        }
                        else // Reset EV (HP or Atk)
                        {
                            if (dataSigned == 0)
                                break;

                            dataSigned = 0;
                        }

                        // Update EVs and stats
                        SetMonData(mon, sGetMonDataEVConstants[temp1], &dataSigned);
                        CalculateMonStats(mon);
                        itemEffectParam++;
                        retVal = FALSE;
                        break;

                    case 2: // ITEM4_HEAL_HP
                    {
                        u32 currentHP = GetMonData(mon, MON_DATA_HP);
                        u32 maxHP = GetMonData(mon, MON_DATA_MAX_HP);
                        if (isLevelUpItem && !didLevelUp && (effectFlags & (ITEM4_REVIVE >> 2)))
                        {
                            itemEffectParam++;
                            break;
                        }
                        // Check use validity.
                        if ((effectFlags & (ITEM4_REVIVE >> 2) && currentHP != 0)
                              || (!(effectFlags & (ITEM4_REVIVE >> 2)) && currentHP == 0))
                        {
                            itemEffectParam++;
                            break;
                        }

                        // Get amount of HP to restore
                        dataUnsigned = itemEffect[itemEffectParam++];
                        switch (dataUnsigned)
                        {
                        case ITEM6_HEAL_HP_FULL:
                            dataUnsigned = maxHP - currentHP;
                            break;
                        case ITEM6_HEAL_HP_HALF:
                            dataUnsigned = maxHP / 2;
                            if (dataUnsigned == 0)
                                dataUnsigned = 1;
                            break;
                        case ITEM6_HEAL_HP_LVL_UP:
                            dataUnsigned = gBattleScripting.levelUpHP;
                            break;
                        case ITEM6_HEAL_HP_QUARTER:
                            dataUnsigned = maxHP / 4;
                            if (dataUnsigned == 0)
                                dataUnsigned = 1;
                            break;
                        }

                        // Only restore HP if not at max health
                        if (maxHP != currentHP)
                        {
                            // Restore HP
                            dataUnsigned = currentHP + dataUnsigned;
                            if (dataUnsigned > maxHP)
                                dataUnsigned = maxHP;
                            SetMonData(mon, MON_DATA_HP, &dataUnsigned);
                            retVal = FALSE;
                        }
                        effectFlags &= ~(ITEM4_REVIVE >> 2);
                        break;
                    }
                    case 3: // ITEM4_HEAL_PP
                        // Stored Pokémon always have full PP; battle restoration
                        // updates the active battler separately.
                        itemEffectParam++;
                        break;

                    // cases 4 and 6 are ITEM4_HEAL_PP_ONE and ITEM4_REVIVE, which
                    // are already handled above by other cases or before the loop

                    case 7: // ITEM4_EVO_STONE
                        {
                            bool32 canStopEvo = TRUE;
                            enum Species targetSpecies = GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item, NULL, &canStopEvo, CHECK_EVO);

                            if (targetSpecies != SPECIES_NONE)
                            {
                                GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_USE, item, NULL, &canStopEvo, DO_EVO);
                                BeginEvolutionScene(mon, targetSpecies, canStopEvo, partyIndex);
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;

        // Handle ITEM5 effects (Change Def/SpDef/SpAtk/Speed EVs).
        case 5:
            effectFlags = itemEffect[i];
            temp1 = 0;

            // Loop through and try each of the ITEM5 effects
            while (effectFlags != 0)
            {
                if (effectFlags & 1)
                {
                    switch (temp1)
                    {
                    case 0: // ITEM5_EV_DEF
                    case 1: // ITEM5_EV_SPEED
                    case 2: // ITEM5_EV_SPDEF
                    case 3: // ITEM5_EV_SPATK
                        evCount = GetMonEVCount(mon);
                        temp2 = itemEffect[itemEffectParam];
                        dataSigned = GetMonData(mon, sGetMonDataEVConstants[temp1 + 2]);
                        evChange = temp2;
                        if (evChange > 0) // Increasing EV
                        {
                            // Check if the total EV limit is reached
                            if (evCount >= maxAllowedEVs)
                                return TRUE;

                            // Ensure the increase does not exceed the max EV per stat (252)
                            evCap = (itemEffect[10] & ITEM10_IS_VITAMIN) ? EV_ITEM_RAISE_LIMIT : MAX_PER_STAT_EVS;

                            // Check if the per-stat limit is reached
                            if (dataSigned >= evCap)
                                return TRUE;  // Prevents item use if the per-stat cap is already reached

                            if (dataSigned + evChange > evCap)
                                temp2 = evCap - dataSigned;
                            else
                                temp2 = evChange;

                            // Ensure the total EVs do not exceed the maximum allowed (510)
                            if (evCount + temp2 > maxAllowedEVs)
                                temp2 = maxAllowedEVs - evCount;

                            // Prevent item use if no EVs can be increased
                            if (temp2 == 0)
                                return TRUE;

                            // Apply the EV increase
                            dataSigned += temp2;
                        }
                        else if (evChange < 0) // Decreasing EV
                        {
                            if (dataSigned == 0)
                            {
                                return TRUE;
                            }
                            dataSigned += evChange;
                            if (I_BERRY_EV_JUMP == GEN_4 && dataSigned > 100)
                                dataSigned = 100;
                            if (dataSigned < 0)
                                dataSigned = 0;
                        }
                        else // Reset EV
                        {
                            if (dataSigned == 0)
                                break;

                            dataSigned = 0;
                        }

                        // Update EVs and stats
                        SetMonData(mon, sGetMonDataEVConstants[temp1 + 2], &dataSigned);
                        CalculateMonStats(mon);
                        retVal = FALSE;
                        itemEffectParam++;
                        break;

                    }
                }
                temp1++;
                effectFlags >>= 1;
            }
            break;
        }
    }
    return retVal;
}

bool8 HealStatusConditions(struct Pokemon *mon, u32 healMask, enum BattlerId battler)
{
    u32 status = GetMonData(mon, MON_DATA_STATUS, 0);

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, battler, gBattlerPartyIndexes[battler]);

    if (status & healMask)
    {
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
        status &= ~healMask;
        SetMonData(mon, MON_DATA_STATUS, &status);
        if (gMain.inBattle && battler != MAX_BATTLERS_COUNT)
        {
            gBattleMons[battler].status1 &= ~healMask;
            if ((healMask & STATUS1_SLEEP))
            {
                struct Pokemon *party = GetBattlerParty(battler);

                for (enum PartySlot i = PARTY_SLOT_0; i < PARTY_SLOT_NONE; i++)
                {
                    if (&party[i] == mon)
                    {
                        TryDeactivateSleepClause(battler, i);
                        break;
                    }
                }
            }
        }
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

u8 GetItemEffectParamOffset(enum BattlerId battler, enum Item itemId, u8 effectByte, u8 effectBit)
{
    const u8 *temp;
    const u8 *itemEffect;
    u8 offset;
    int i;
    u8 j;
    u8 effectFlags;

    offset = ITEM_EFFECT_ARG_START;

    temp = GetItemEffect(itemId);

    if (temp == NULL)
        return 0;

    itemEffect = temp;

    for (i = 0; i < ITEM_EFFECT_ARG_START; i++)
    {
        switch (i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (i == effectByte)
                return 0;
            break;
        case 4:
            effectFlags = itemEffect[4];
            j = 0;
            while (effectFlags)
            {
                if (effectFlags & 1)
                {
                    switch (j)
                    {
                    case 2: // ITEM4_HEAL_HP
                        if (effectFlags & (ITEM4_REVIVE >> 2))
                            effectFlags &= ~(ITEM4_REVIVE >> 2);
                        [[fallthrough]];
                    case 0: // ITEM4_EV_HP
                        if (i == effectByte && (effectFlags & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 1: // ITEM4_EV_ATK
                        if (i == effectByte && (effectFlags & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 3: // ITEM4_HEAL_PP
                        if (i == effectByte && (effectFlags & effectBit))
                            return offset;
                        offset++;
                        break;
                    case 7: // ITEM4_EVO_STONE
                        if (i == effectByte)
                            return 0;
                        break;
                    }
                }
                j++;
                effectFlags >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        case 5:
            effectFlags = itemEffect[5];
            j = 0;
            while (effectFlags)
            {
                if (effectFlags & 1)
                {
                    switch (j)
                    {
                    case 0: // ITEM5_EV_DEF
                    case 1: // ITEM5_EV_SPEED
                    case 2: // ITEM5_EV_SPDEF
                    case 3: // ITEM5_EV_SPATK
                        if (i == effectByte && (effectFlags & effectBit))
                            return offset;
                        offset++;
                        break;
                    }
                }
                j++;
                effectFlags >>= 1;
                if (i == effectByte)
                    effectBit >>= 1;
            }
            break;
        }
    }

    return offset;
}

u8 GetNature(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_PERSONALITY, 0) % NUM_NATURES;
}

u8 GetNatureFromPersonality(u32 personality)
{
    return personality % NUM_NATURES;
}

bool32 DoesMonMeetAdditionalConditions(struct Pokemon *mon, const struct EvolutionParam *params, struct Pokemon *tradePartner, enum PartySlot partyId, bool32 *canStopEvo, enum EvoState evoState)
{
    u32 i, j;
    enum Item heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    u32 gender = GetMonGender(mon);
    u32 attack = GetMonData(mon, MON_DATA_ATK, 0);
    u32 defense = GetMonData(mon, MON_DATA_DEF, 0);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY, 0);
    u16 upperPersonality = personality >> 16;
    u32 weather = GetCurrentWeather();
    u32 nature = GetNature(mon);
    bool32 removeHoldItem = FALSE;
    enum Item removeBagItem = ITEM_NONE;
    u32 removeBagItemCount = 0;
    u32 evolutionTracker = GetMonData(mon, MON_DATA_EVOLUTION_TRACKER, 0);
    enum Species partnerSpecies;
    enum Item partnerHeldItem;
    enum HoldEffect partnerHoldEffect;

    if (tradePartner != NULL)
    {
        partnerSpecies = GetMonData(tradePartner, MON_DATA_SPECIES, 0);
        partnerHeldItem = GetMonData(tradePartner, MON_DATA_HELD_ITEM, 0);
        partnerHoldEffect = GetItemHoldEffect(partnerHeldItem);
    }
    else
    {
        partnerSpecies = SPECIES_NONE;
        partnerHeldItem = ITEM_NONE;
        partnerHoldEffect = HOLD_EFFECT_NONE;
    }

    // Check for additional conditions (only if the primary method passes). Skips if there's no additional conditions.
    for (i = 0; params != NULL && params[i].condition != CONDITIONS_END; i++)
    {
        enum EvolutionConditions condition = params[i].condition;
        bool32 currentCondition = FALSE;

        switch (condition)
        {
        // Gen 2
        case IF_GENDER:
            if (gender == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_ATK_GT_DEF:
            if (attack > defense)
                currentCondition = TRUE;
            break;
        case IF_ATK_EQ_DEF:
            if (attack == defense)
                currentCondition = TRUE;
            break;
        case IF_ATK_LT_DEF:
            if (attack < defense)
                currentCondition = TRUE;
            break;
        case IF_TIME:
            if (GetTimeOfDay() == params[i].arg1)
                currentCondition = TRUE;

            break;
        case IF_NOT_TIME:
            if (GetTimeOfDay() != params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_HOLD_ITEM:
            if (heldItem == params[i].arg1)
            {
                currentCondition = TRUE;
                removeHoldItem = TRUE;
            }
            break;
        // Gen 3
        case IF_PID_UPPER_MODULO_10_GT:
            if ((upperPersonality % 10) > params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_UPPER_MODULO_10_EQ:
            if ((upperPersonality % 10) == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_UPPER_MODULO_10_LT:
            if ((upperPersonality % 10) < params[i].arg1)
                currentCondition = TRUE;
            break;
        // Gen 4
        case IF_SPECIES_IN_PARTY:
            for (j = 0; j < PARTY_SIZE; j++)
            {
                if (GetMonData(&gParties[B_TRAINER_PLAYER][j], MON_DATA_SPECIES) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        case IF_IN_MAP:
            if (params[i].arg1 == ((gSaveData.location.mapGroup) << 8 | gSaveData.location.mapNum))
                currentCondition = TRUE;
            break;
        case IF_IN_MAPSEC:
            if (gMapHeader.regionMapSectionId == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_KNOWS_MOVE:
            if (MonKnowsMove(mon, params[i].arg1))
                currentCondition = TRUE;
            break;
        // Gen 5
        case IF_TRADE_PARTNER_SPECIES:
            if (params[i].arg1 == partnerSpecies && partnerHoldEffect != HOLD_EFFECT_PREVENT_EVOLVE)
                currentCondition = TRUE;
            break;
        // Gen 6
        case IF_TYPE_IN_PARTY:
            for (j = 0; j < PARTY_SIZE; j++)
            {
                enum Species currSpecies = GetMonData(&gParties[B_TRAINER_PLAYER][j], MON_DATA_SPECIES);
                if (GetSpeciesType(currSpecies, 0) == params[i].arg1
                 || GetSpeciesType(currSpecies, 1) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        case IF_WEATHER:
            if (params[i].arg1 == WEATHER_RAIN)
            {
                if (weather == WEATHER_RAIN || weather == WEATHER_RAIN_THUNDERSTORM || weather == WEATHER_DOWNPOUR)
                    currentCondition = TRUE;
            }
            else if (params[i].arg1 == WEATHER_FOG)
            {
                if (weather == WEATHER_FOG_DIAGONAL || weather == WEATHER_FOG_HORIZONTAL)
                    currentCondition = TRUE;
            }
            else if (weather == params[i].arg1)
            {
                currentCondition = TRUE;
            }
            break;
        case IF_KNOWS_MOVE_TYPE:
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (GetMoveType(GetMonData(mon, MON_DATA_MOVE1 + j)) == params[i].arg1)
                {
                    currentCondition = TRUE;
                    break;
                }
            }
            break;
        // Gen 8
        case IF_NATURE:
            if (nature == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_AMPED_NATURE:
            switch (nature)
            {
            case NATURE_HARDY:
            case NATURE_BRAVE:
            case NATURE_ADAMANT:
            case NATURE_NAUGHTY:
            case NATURE_DOCILE:
            case NATURE_IMPISH:
            case NATURE_LAX:
            case NATURE_HASTY:
            case NATURE_JOLLY:
            case NATURE_NAIVE:
            case NATURE_RASH:
            case NATURE_SASSY:
            case NATURE_QUIRKY:
                currentCondition = TRUE;
                break;
            }
            break;
        case IF_LOW_KEY_NATURE:
            switch (nature)
            {
            case NATURE_LONELY:
            case NATURE_BOLD:
            case NATURE_RELAXED:
            case NATURE_TIMID:
            case NATURE_SERIOUS:
            case NATURE_MODEST:
            case NATURE_MILD:
            case NATURE_QUIET:
            case NATURE_BASHFUL:
            case NATURE_CALM:
            case NATURE_GENTLE:
            case NATURE_CAREFUL:
                currentCondition = TRUE;
                break;
            }
            break;
        case IF_RECOIL_DAMAGE_GE:
            if (evolutionTracker >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_CURRENT_DAMAGE_GE:
        {
            u32 currentHp = GetMonData(mon, MON_DATA_HP);
            if (currentHp != 0 && (GetMonData(mon, MON_DATA_MAX_HP) - currentHp >= params[i].arg1))
                currentCondition = TRUE;
            break;
        }
        case IF_CRITICAL_HITS_GE:
            if (partyId < PARTY_SLOT_NONE && gPartyCriticalHits[partyId] >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_USED_MOVE_X_TIMES:
            if (evolutionTracker >= params[i].arg2)
                currentCondition = TRUE;
            break;
        // Gen 9
        case IF_DEFEAT_X_WITH_ITEMS:
            if (evolutionTracker >= params[i].arg3)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_GT:
            if ((personality % 100) > params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_EQ:
            if ((personality % 100) == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_PID_MODULO_100_LT:
            if ((personality % 100) < params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_MIN_OVERWORLD_STEPS:
            if (mon == GetFirstLiveMon() && gFollowerSteps >= params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_BAG_ITEM_COUNT:
            if (CheckBagHasItem(params[i].arg1, params[i].arg2))
            {
                currentCondition = TRUE;
                removeBagItem = params[i].arg1;
                removeBagItemCount = params[i].arg2;
                if (canStopEvo != NULL)
                    *canStopEvo = FALSE;
            }
            break;
        case IF_REGION:
            if (GetCurrentRegion() == params[i].arg1)
                currentCondition = TRUE;
            break;
        case IF_NOT_REGION:
            if (GetCurrentRegion() != params[i].arg1)
                currentCondition = TRUE;
            break;
        case CONDITIONS_END:
            break;
        }

        // check if an evolution is about to happen and items should be removed
        if (evoState == DO_EVO)
        {
            if (removeHoldItem)
            {
                enum Item heldItem = ITEM_NONE;
                SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
            }

            if (removeBagItem != ITEM_NONE)
                RemoveBagItem(removeBagItem, removeBagItemCount);
        }

        if (currentCondition == FALSE)
            return FALSE;
    }

    return TRUE;
}

enum Species GetEvolutionTargetSpecies(struct Pokemon *mon, enum EvolutionMode mode, enum Item evolutionItem, struct Pokemon *tradePartner, bool32 *canStopEvo, enum EvoState evoState)
{
    int i;
    enum Species targetSpecies = SPECIES_NONE;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES, 0);
    enum Item heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    u32 level = GetMonData(mon, MON_DATA_LEVEL, 0);
    enum HoldEffect holdEffect;
    const struct Evolution *evolutions = GetSpeciesEvolutions(species);

    if (evolutions == NULL)
        return SPECIES_NONE;

    holdEffect = GetItemHoldEffect(heldItem);

    // Prevent evolution with Everstone, unless we're just viewing the party menu with an evolution item
    if (holdEffect == HOLD_EFFECT_PREVENT_EVOLVE
        && mode != EVO_MODE_ITEM_CHECK
        && (P_KADABRA_EVERSTONE < GEN_4 || species != SPECIES_KADABRA))
        return SPECIES_NONE;

    switch (mode)
    {
    case EVO_MODE_NORMAL:
    case EVO_MODE_BATTLE_ONLY:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            // Check main primary evolution method
            switch (evolutions[i].method)
            {
            case EVO_LEVEL:
                if (evolutions[i].param <= level)
                    conditionsMet = TRUE;
                break;
            case EVO_LEVEL_BATTLE_ONLY:
                if (mode == EVO_MODE_BATTLE_ONLY && evolutions[i].param <= level)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SLOT_NONE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_TRADE:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_TRADE:
                conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, tradePartner, PARTY_SLOT_NONE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_ITEM_USE:
    case EVO_MODE_ITEM_CHECK:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_ITEM:
                if (evolutions[i].param == evolutionItem)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SLOT_NONE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                if (canStopEvo != NULL)
                    *canStopEvo = FALSE;
                break;
            }
        }
        break;
    // Battle evolution without leveling; party slot is being passed into the evolutionItem arg.
    case EVO_MODE_BATTLE_SPECIAL:
        if ((u32)evolutionItem >= PARTY_SLOT_NONE)
            break;

        enum PartySlot partyId = (enum PartySlot)evolutionItem;
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_BATTLE_END:
                conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, partyId, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    // Overworld evolution without leveling; evolution method is being passed into the evolutionItem arg.
    case EVO_MODE_OVERWORLD_SPECIAL:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            bool32 conditionsMet = FALSE;
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;

            switch (evolutions[i].method)
            {
            case EVO_SPIN:
                if (gSpecialVar_0x8000 == evolutions[i].param)
                    conditionsMet = TRUE;
                break;
            }

            if (conditionsMet && DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SLOT_NONE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    case EVO_MODE_SCRIPT_TRIGGER:
        for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
                continue;
            if (evolutions[i].method != EVO_SCRIPT_TRIGGER)
                continue;
            if (evolutions[i].param != evolutionItem)
                continue;
            if (DoesMonMeetAdditionalConditions(mon, evolutions[i].params, NULL, PARTY_SLOT_NONE, canStopEvo, evoState))
            {
                // All checks passed, so stop checking the rest of the evolutions.
                // This is different from vanilla where the loop continues.
                // If you have overlapping evolutions, put the ones you want to happen first on top of the list.
                targetSpecies = evolutions[i].targetSpecies;
                break;
            }
        }
        break;
    }

    return targetSpecies;
}

bool8 IsMonPastEvolutionLevel(struct Pokemon *mon)
{
    int i;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 level = GetMonData(mon, MON_DATA_LEVEL, 0);
    const struct Evolution *evolutions = GetSpeciesEvolutions(species);

    if (evolutions == NULL)
        return FALSE;

    for (i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
    {
        if (SanitizeSpeciesId(evolutions[i].targetSpecies) == SPECIES_NONE)
            continue;

        switch (evolutions[i].method)
        {
        case EVO_LEVEL:
            if (evolutions[i].param <= level)
                return TRUE;
            break;
        }
    }

    return FALSE;
}

enum Species NationalPokedexNumToSpecies(enum NationalDexOrder nationalNum)
{
    enum Species species;

    if (!nationalNum)
        return SPECIES_NONE;

    species = 1;

    while (species < (NUM_SPECIES) && gSpeciesInfo[species].natDexNum != nationalNum)
        species++;

    if (species == NUM_SPECIES)
        return SPECIES_NONE;

    return GetBaseSpecies(species);
}

u32 NationalToRegionalOrder(enum NationalDexOrder nationalNum)
{
    return NationalToCalicadeaOrder(nationalNum);
}

enum KantoDexOrder NationalToKantoOrder(enum NationalDexOrder nationalNum)
{
    u16 kantoNum;

    if (!nationalNum)
        return 0;

    kantoNum = 0;

    while (kantoNum < KANTO_DEX_COUNT && sKantoToNationalOrder[kantoNum] != nationalNum)
        kantoNum++;

    if (kantoNum >= KANTO_DEX_COUNT)
        return 0;

    return kantoNum + 1;
}

enum CalicadeaDexOrder NationalToCalicadeaOrder(enum NationalDexOrder nationalNum)
{
    u16 calicadeaNum;

    if (!nationalNum)
        return 0;

    calicadeaNum = 0;

    while (calicadeaNum < (CALICADEA_DEX_COUNT - 1) && sCalicadeaToNationalOrder[calicadeaNum] != nationalNum)
        calicadeaNum++;

    if (calicadeaNum >= CALICADEA_DEX_COUNT - 1)
        return 0;

    return calicadeaNum + 1;
}

enum NationalDexOrder SpeciesToNationalPokedexNum(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (!species)
        return NATIONAL_DEX_NONE;

    return gSpeciesInfo[species].natDexNum;
}

u32 SpeciesToRegionalPokedexNum(enum Species species)
{
    return SpeciesToCalicadeaPokedexNum(species);
}

enum KantoDexOrder SpeciesToKantoPokedexNum(enum Species species)
{
    if (!species)
        return 0;
    return NationalToKantoOrder(gSpeciesInfo[species].natDexNum);
}

enum CalicadeaDexOrder SpeciesToCalicadeaPokedexNum(enum Species species)
{
    return NationalToCalicadeaOrder(SpeciesToNationalPokedexNum(species));
}

enum NationalDexOrder RegionalToNationalOrder(u32 regionalNum)
{
    return CalicadeaToNationalOrder(regionalNum);
}

enum NationalDexOrder KantoToNationalOrder(enum KantoDexOrder kantoNum)
{
    if (!kantoNum || kantoNum >= (KANTO_DEX_COUNT + 1))
        return 0;

    return sKantoToNationalOrder[kantoNum - 1];
}

enum NationalDexOrder CalicadeaToNationalOrder(enum CalicadeaDexOrder calicadeaNum)
{
    if (!calicadeaNum || calicadeaNum >= CALICADEA_DEX_COUNT)
        return 0;

    return sCalicadeaToNationalOrder[calicadeaNum - 1];
}

void EvolutionRenameMon(struct Pokemon *mon, enum Species oldSpecies, enum Species newSpecies)
{
    u8 language;
    GetMonData(mon, MON_DATA_NICKNAME, gStringVar1);
    language = GetMonData(mon, MON_DATA_LANGUAGE, &language);
    if (language == GAME_LANGUAGE && !StringCompare(GetSpeciesName(oldSpecies), gStringVar1))
        SetMonData(mon, MON_DATA_NICKNAME, GetSpeciesName(newSpecies));
}

// The below two functions determine which side of a multi battle the trainer battles on
// 0 is the left (top in  party menu), 1 is right (bottom in party menu)
u8 GetPlayerFlankId(void)
{
    u8 flankId = 0;
    switch (gLinkPlayers[GetMultiplayerId()].id)
    {
    case 0:
    case 1:
        flankId = 0;
        break;
    case 2:
    case 3:
        flankId = 1;
        break;
    }
    return flankId;
}

u16 GetLinkTrainerFlankId(u8 linkPlayerId)
{
    u16 flankId = 0;
    switch (gLinkPlayers[linkPlayerId].id)
    {
    case 0:
    case 1:
        flankId = 0;
        break;
    case 2:
    case 3:
        flankId = 1;
        break;
    }
    return flankId;
}

s32 GetBattlerMultiplayerId(u16 id)
{
    s32 multiplayerId;
    for (multiplayerId = 0; multiplayerId < MAX_LINK_PLAYERS; multiplayerId++)
        if (gLinkPlayers[multiplayerId].id == id)
            break;
    return multiplayerId;
}

u8 GetTrainerEncounterMusicId(u16 trainerOpponentId)
{
    u32 sanitizedTrainerId = SanitizeTrainerId(trainerOpponentId);
    enum DifficultyLevel difficulty = GetTrainerDifficultyLevel(sanitizedTrainerId);

    {
        return gTrainers[difficulty][sanitizedTrainerId].encounterMusic;
    }
}

u16 ModifyStatByNature(u8 nature, u16 stat, enum Stat statIndex)
{
    // Don't modify HP, Accuracy, or Evasion by nature
    if (statIndex <= STAT_HP || statIndex > NUM_NATURE_STATS || gNaturesInfo[nature].statUp == gNaturesInfo[nature].statDown)
        return stat;
    else if (statIndex == gNaturesInfo[nature].statUp)
        return stat * 110 / 100;
    else if (statIndex == gNaturesInfo[nature].statDown)
        return stat * 90 / 100;
    else
        return stat;
}


void MonGainEVs(struct Pokemon *mon, enum Species defeatedSpecies)
{
    u8 evs[NUM_STATS];
    u16 evIncrease = 0;
    u16 totalEVs = 0;
    enum Item heldItem;
    enum HoldEffect holdEffect;
    enum Stat i;
    int multiplier;
    u8 stat;
    u8 bonus;
    u32 currentEVCap = GetCurrentEVCap();

    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    holdEffect = GetItemHoldEffect(heldItem);

    stat = GetItemSecondaryId(heldItem);
    bonus = GetItemHoldEffectParam(heldItem);

    for (i = 0; i < NUM_STATS; i++)
    {
        evs[i] = GetMonData(mon, MON_DATA_HP_EV + i, 0);
        totalEVs += evs[i];
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        if (totalEVs >= currentEVCap)
            break;

        multiplier = 1;

        switch (i)
        {
        case STAT_HP:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_HP)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_HP + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_HP * multiplier;
            break;
        case STAT_ATK:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_ATK)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_Attack + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_Attack * multiplier;
            break;
        case STAT_DEF:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_DEF)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_Defense + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_Defense * multiplier;
            break;
        case STAT_SPEED:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_SPEED)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_Speed + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_Speed * multiplier;
            break;
        case STAT_SPATK:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_SPATK)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_SpAttack + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_SpAttack * multiplier;
            break;
        case STAT_SPDEF:
            if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == STAT_SPDEF)
                evIncrease = (gSpeciesInfo[defeatedSpecies].evYield_SpDefense + bonus) * multiplier;
            else
                evIncrease = gSpeciesInfo[defeatedSpecies].evYield_SpDefense * multiplier;
            break;
        default:
            break;
        }

        if (holdEffect == HOLD_EFFECT_MACHO_BRACE)
            evIncrease *= 2;

        if (totalEVs + (s16)evIncrease > currentEVCap)
            evIncrease = ((s16)evIncrease + currentEVCap) - (totalEVs + evIncrease);

        if (evs[i] + (s16)evIncrease > MAX_PER_STAT_EVS)
        {
            int val1 = (s16)evIncrease + MAX_PER_STAT_EVS;
            int val2 = evs[i] + evIncrease;
            evIncrease = val1 - val2;
        }

        evs[i] += evIncrease;
        totalEVs += evIncrease;
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }
}

u16 GetMonEVCount(struct Pokemon *mon)
{
    int i;
    u16 count = 0;

    for (i = 0; i < NUM_STATS; i++)
        count += GetMonData(mon, MON_DATA_HP_EV + i, 0);

    return count;
}

bool8 TryIncrementMonLevel(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES, 0);
    u8 nextLevel = GetMonData(mon, MON_DATA_LEVEL, 0) + 1;
    u32 expPoints = GetMonData(mon, MON_DATA_EXP, 0);
    if (expPoints > gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL])
    {
        expPoints = gExperienceTables[gSpeciesInfo[species].growthRate][MAX_LEVEL];
        SetMonData(mon, MON_DATA_EXP, &expPoints);
    }
    if (nextLevel > GetCurrentLevelCap() || expPoints < gExperienceTables[gSpeciesInfo[species].growthRate][nextLevel])
    {
        return FALSE;
    }
    else
    {
        SetMonData(mon, MON_DATA_LEVEL, &nextLevel);
        return TRUE;
    }
}

bool32 CanLearnTeachableMove(enum Species species, enum Move move)
{
    const u16 *teachableLearnset = GetSpeciesTeachableLearnset(species);
    if (species == SPECIES_EGG)
        return FALSE;
    for (u32 i = 0; teachableLearnset[i] != MOVE_UNAVAILABLE; i++)
    {
        if (teachableLearnset[i] == move)
            return TRUE;
    }
    return FALSE;
}

u16 SpeciesToPokedexNum(enum Species species)
{
    if (IsNationalPokedexEnabled())
    {
        return SpeciesToNationalPokedexNum(species);
    }
    else
    {
        species = SpeciesToRegionalPokedexNum(species);
        if (species <= REGIONAL_DEX_COUNT)
            return species;
        return 0xFFFF;
    }
}

bool32 IsSpeciesInRegionalDex(enum Species species)
{
    return IsSpeciesInCalicadeaDex(species);
}

bool32 IsSpeciesInKantoDex(enum Species species)
{
    if (SpeciesToKantoPokedexNum(species) > KANTO_DEX_COUNT)
        return FALSE;
    else
        return TRUE;
}

bool32 IsSpeciesInCalicadeaDex(enum Species species)
{
    u32 regionalNum = SpeciesToCalicadeaPokedexNum(species);
    return regionalNum != 0 && regionalNum < CALICADEA_DEX_COUNT;
}

u16 GetBattleBGM(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LEGENDARY)
    {
        switch (GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES))
        {
        case SPECIES_RAYQUAZA:
            return MUS_VS_RAYQUAZA;
        case SPECIES_KYOGRE:
        case SPECIES_GROUDON:
            return MUS_VS_KYOGRE_GROUDON;
        case SPECIES_REGIROCK:
        case SPECIES_REGICE:
        case SPECIES_REGISTEEL:
        case SPECIES_REGIGIGAS:
        case SPECIES_REGIELEKI:
        case SPECIES_REGIDRAGO:
            return MUS_VS_REGI;
        default:
            return MUS_RG_VS_LEGEND;
        }
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        return MUS_VS_TRAINER;
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        enum TrainerClassID trainerClass;

        {
            trainerClass = GetTrainerClassFromId(TRAINER_BATTLE_PARAM.opponentA);
        }

        switch (trainerClass)
        {
        case TRAINER_CLASS_AQUA_LEADER:
        case TRAINER_CLASS_MAGMA_LEADER:
            return MUS_VS_AQUA_MAGMA_LEADER;
        case TRAINER_CLASS_TEAM_AQUA:
        case TRAINER_CLASS_TEAM_MAGMA:
        case TRAINER_CLASS_AQUA_ADMIN:
        case TRAINER_CLASS_MAGMA_ADMIN:
            return MUS_VS_AQUA_MAGMA;
        case TRAINER_CLASS_LEADER:
            return MUS_VS_GYM_LEADER;
        case TRAINER_CLASS_CHAMPION:
            return MUS_VS_CHAMPION;
        case TRAINER_CLASS_RIVAL:
            if (!StringCompare(GetTrainerNameFromId(TRAINER_BATTLE_PARAM.opponentA), gText_BattleWallyName))
                return MUS_VS_TRAINER;
            return MUS_VS_RIVAL;
        case TRAINER_CLASS_ELITE_FOUR:
            return MUS_VS_ELITE_FOUR;
        case TRAINER_CLASS_CHAMPION_FRLG:
            return MUS_RG_VS_CHAMPION;
        case TRAINER_CLASS_LEADER_FRLG:
        case TRAINER_CLASS_ELITE_FOUR_FRLG:
            return MUS_RG_VS_GYM_LEADER;
        default:
            if (GetCurrentRegion() == REGION_KANTO)
                return MUS_RG_VS_TRAINER;
            else
                return MUS_VS_TRAINER;
        }
    }
    else
    {
        if (GetCurrentRegion() == REGION_KANTO)
            return MUS_RG_VS_WILD;
        else
            return MUS_VS_WILD;
    }
}

void PlayBattleBGM(void)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    PlayBGM(GetBattleBGM());
}

void PlayMapChosenOrBattleBGM(u16 songId)
{
    ResetMapMusic();
    m4aMPlayAllStop();
    if (songId)
        PlayNewMapMusic(songId);
    else
        PlayNewMapMusic(GetBattleBGM());
}

const u16 *GetMonFrontSpritePal(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    bool32 isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    bool32 isEgg = GetMonData(mon, MON_DATA_IS_EGG);
    return GetMonSpritePalFromSpeciesAndPersonalityIsEgg(species, isShiny, personality, isEgg);
}

const u16 *GetMonSpritePalFromSpeciesAndPersonality(enum Species species, bool32 isShiny, u32 personality)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, IsPersonalityFemale(species, personality), FALSE);
}

const u16 *GetMonSpritePalFromSpeciesAndPersonalityIsEgg(enum Species species, bool32 isShiny, u32 personality, bool32 isEgg)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, IsPersonalityFemale(species, personality), isEgg);
}

const u16 *GetMonSpritePalFromSpecies(enum Species species, bool32 isShiny, bool32 isFemale)
{
    return GetMonSpritePalFromSpeciesIsEgg(species, isShiny, isFemale, FALSE);
}

const u16 *GetMonSpritePalFromSpeciesIsEgg(enum Species species, bool32 isShiny, bool32 isFemale, bool32 isEgg)
{
    species = SanitizeSpeciesId(species);

    if (isEgg)
    {
        if (gSpeciesInfo[species].eggId != EGG_ID_NONE)
            return gEggDatas[gSpeciesInfo[species].eggId].eggPalette;
        else
            return gSpeciesInfo[SPECIES_EGG].palette;
    }
    else if (isShiny)
    {
    #if P_GENDER_DIFFERENCES
        if (gSpeciesInfo[species].shinyPaletteFemale != NULL && isFemale)
            return gSpeciesInfo[species].shinyPaletteFemale;
        else
    #endif
        if (gSpeciesInfo[species].shinyPalette != NULL)
            return gSpeciesInfo[species].shinyPalette;
        else
            return gSpeciesInfo[SPECIES_NONE].shinyPalette;
    }
    else
    {
    #if P_GENDER_DIFFERENCES
        if (gSpeciesInfo[species].paletteFemale != NULL && isFemale)
            return gSpeciesInfo[species].paletteFemale;
        else
    #endif
        if (gSpeciesInfo[species].palette != NULL)
            return gSpeciesInfo[species].palette;
        else
            return gSpeciesInfo[SPECIES_NONE].palette;
    }
}

#define OR_MOVE_IS_HM(_hm) || (move == MOVE_##_hm)

bool32 IsMoveHM(enum Move move)
{
    return FALSE FOREACH_HM(OR_MOVE_IS_HM);
}

#undef OR_MOVE_IS_HM

bool32 CannotForgetMove(enum Move move)
{
    if (P_CAN_FORGET_HIDDEN_MOVE)
        return FALSE;

    return IsMoveHM(move);
}

bool8 IsMonSpriteNotFlipped(enum Species species)
{
    return gSpeciesInfo[species].noFlip;
}

static const s8 sNatureFlavorCompatibilityTable[NUM_NATURES * FLAVOR_COUNT] =
{
     // Spicy,  Dry, Sweet, Bitter, Sour
          0,      0,    0,     0,     0, // Hardy
          1,      0,    0,     0,    -1, // Lonely
          1,      0,   -1,     0,     0, // Brave
          1,     -1,    0,     0,     0, // Adamant
          1,      0,    0,    -1,     0, // Naughty
         -1,      0,    0,     0,     1, // Bold
          0,      0,    0,     0,     0, // Docile
          0,      0,   -1,     0,     1, // Relaxed
          0,     -1,    0,     0,     1, // Impish
          0,      0,    0,    -1,     1, // Lax
         -1,      0,    1,     0,     0, // Timid
          0,      0,    1,     0,    -1, // Hasty
          0,      0,    0,     0,     0, // Serious
          0,     -1,    1,     0,     0, // Jolly
          0,      0,    1,    -1,     0, // Naive
         -1,      1,    0,     0,     0, // Modest
          0,      1,    0,     0,    -1, // Mild
          0,      1,   -1,     0,     0, // Quiet
          0,      0,    0,     0,     0, // Bashful
          0,      1,    0,    -1,     0, // Rash
         -1,      0,    0,     1,     0, // Calm
          0,      0,    0,     1,    -1, // Gentle
          0,      0,   -1,     1,     0, // Sassy
          0,     -1,    0,     1,     0, // Careful
          0,      0,    0,     0,     0  // Quirky
};

s8 GetMonFlavorRelation(struct Pokemon *mon, enum Flavor flavor)
{
    u8 nature = GetNature(mon);
    return sNatureFlavorCompatibilityTable[nature * FLAVOR_COUNT + flavor];
}

s8 GetFlavorRelationByPersonality(u32 personality, enum Flavor flavor)
{
    u8 nature = GetNatureFromPersonality(personality);
    return sNatureFlavorCompatibilityTable[nature * FLAVOR_COUNT + flavor];
}

bool8 IsTradedMon(struct Pokemon *mon)
{
    u8 otName[PLAYER_NAME_LENGTH + 1];
    u32 otId;
    GetMonData(mon, MON_DATA_OT_NAME, otName);
    otId = GetMonData(mon, MON_DATA_OT_ID, 0);
    return IsOtherTrainer(otId, otName);
}

bool8 IsOtherTrainer(u32 otId, u8 *otName)
{
    if (otId == READ_OTID_FROM_SAVE)
    {
        int i;
        for (i = 0; otName[i] != EOS; i++)
            if (otName[i] != gSaveData.playerName[i])
                return TRUE;
        return FALSE;
    }

    return TRUE;
}


void SetMonPreventsSwitchingString(void)
{
    gLastUsedAbility = gBattleStruct->abilityPreventingSwitchout;

    gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
    gBattleTextBuff1[1] = B_BUFF_MON_NICK_WITH_PREFIX;
    gBattleTextBuff1[2] = gBattleStruct->battlerPreventingSwitchout;
    gBattleTextBuff1[4] = B_BUFF_EOS;

    if (IsOnPlayerSide(gBattleStruct->battlerPreventingSwitchout))
        gBattleTextBuff1[3] = GetBattleSlotFromBattlePartyId(gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout]);
    else
        gBattleTextBuff1[3] = gBattlerPartyIndexes[gBattleStruct->battlerPreventingSwitchout];

    PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff2, gBattlerInMenuId, GetBattleSlotFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]))

    BattleStringExpandPlaceholders(gText_PkmnsXPreventsSwitching, gStringVar4, sizeof(gStringVar4));
}


static inline bool32 CanFirstMonBoostHeldItemRarity(void)
{
    enum Ability ability;
    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG))
        return FALSE;

    ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
    if (ability == ABILITY_COMPOUND_EYES)
        return TRUE;
    else if ((OW_SUPER_LUCK >= GEN_8) && ability == ABILITY_SUPER_LUCK)
        return TRUE;
    return FALSE;
}

void SetWildMonHeldItem(void)
{
    u16 rnd;
    enum Species species;
    u16 count = (WILD_DOUBLE_BATTLE) ? 2 : 1;
    u16 i;
    bool32 itemHeldBoost = CanFirstMonBoostHeldItemRarity();
    u16 chanceNoItem = itemHeldBoost ? 20 : 45;
    u16 chanceNotRare = itemHeldBoost ? 80 : 95;

    for (i = 0; i < count; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM) != ITEM_NONE)
            continue; // prevent overwriting previously set item

        rnd = Random() % 100;
        species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPECIES, 0);
        {
            if (gSpeciesInfo[species].itemCommon == gSpeciesInfo[species].itemRare && gSpeciesInfo[species].itemCommon != ITEM_NONE)
            {
                // Both held items are the same, 100% chance to hold item
                SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemCommon);
            }
            else
            {
                if (rnd < chanceNoItem)
                    continue;
                if (rnd < chanceNotRare)
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemCommon);
                else
                    SetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_HELD_ITEM, &gSpeciesInfo[species].itemRare);
            }
        }
    }
}

bool8 IsMonShiny(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_IS_SHINY);
}

const u8 *GetTrainerPartnerName(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
    {
        return gStringVar1;
    }
    else
    {
        u8 id = GetMultiplayerId();
        return gLinkPlayers[GetBattlerMultiplayerId(gLinkPlayers[id].id ^ 2)].name;
    }
}

#define READ_PTR_FROM_TASK(taskId, dataId)                      \
    (void *)(                                                   \
    ((u16)(gTasks[taskId].data[dataId]) |                       \
    ((u16)(gTasks[taskId].data[dataId + 1]) << 16)))

#define STORE_PTR_IN_TASK(ptr, taskId, dataId)                 \
{                                                              \
    gTasks[taskId].data[dataId] = (u32)(ptr);                  \
    gTasks[taskId].data[dataId + 1] = (u32)(ptr) >> 16;        \
}

#define sAnimId    data[2]
#define sAnimDelay data[3]

static void Task_AnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        LaunchAnimationTaskForFrontSprite(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        DestroyTask(taskId);
    }
}

static void Task_PokemonSummaryAnimateAfterDelay(u8 taskId)
{
    if (--gTasks[taskId].sAnimDelay == 0)
    {
        StartMonSummaryAnimation(READ_PTR_FROM_TASK(taskId, 0), gTasks[taskId].sAnimId);
        SummaryScreen_SetAnimDelayTaskId(TASK_NONE);
        DestroyTask(taskId);
    }
}

void BattleAnimateFrontSprite(struct Sprite *sprite, enum Species species, bool8 noCry, u8 panMode)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !((gBattleTypeFlags & BATTLE_TYPE_LINK)))
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode | SKIP_FRONT_ANIM);
    else
        DoMonFrontSpriteAnimation(sprite, species, noCry, panMode);
}

void DoMonFrontSpriteAnimation(struct Sprite *sprite, enum Species species, bool8 noCry, u8 panModeAnimFlag)
{
    s8 pan;
    switch (panModeAnimFlag & (u8)~SKIP_FRONT_ANIM) // Exclude anim flag to get pan mode
    {
    case 0:
        pan = -25;
        break;
    case 1:
        pan = 25;
        break;
    default:
        pan = 0;
        break;
    }
    if (panModeAnimFlag & SKIP_FRONT_ANIM || (gBattleTypeFlags & BATTLE_TYPE_GHOST))
    {
        // No animation, only check if cry needs to be played
        if (!noCry)
            PlayCry_Normal(species, pan);
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (!noCry)
        {
            PlayCry_Normal(species, pan);
            if (HasTwoFramesAnimation(species))
                StartSpriteAnim(sprite, 1);
        }
        if (gSpeciesInfo[species].frontAnimDelay != 0)
        {
            // Animation has delay, start delay task
            u8 taskId = CreateTask(Task_AnimateAfterDelay, 0);
            STORE_PTR_IN_TASK(sprite, taskId, 0);
            gTasks[taskId].sAnimId = gSpeciesInfo[species].frontAnimId;
            gTasks[taskId].sAnimDelay = gSpeciesInfo[species].frontAnimDelay;
        }
        else
        {
            // No delay, start animation
            LaunchAnimationTaskForFrontSprite(sprite, gSpeciesInfo[species].frontAnimId);
        }
        sprite->callback = SpriteCallbackDummy_2;
    }
}

void PokemonSummaryDoMonAnimation(struct Sprite *sprite, enum Species species, bool8 oneFrame)
{
    if (!oneFrame && HasTwoFramesAnimation(species))
        StartSpriteAnim(sprite, 1);
    if (gSpeciesInfo[species].frontAnimDelay != 0)
    {
        // Animation has delay, start delay task
        u8 taskId = CreateTask(Task_PokemonSummaryAnimateAfterDelay, 0);
        STORE_PTR_IN_TASK(sprite, taskId, 0);
        gTasks[taskId].sAnimId = gSpeciesInfo[species].frontAnimId;
        gTasks[taskId].sAnimDelay = gSpeciesInfo[species].frontAnimDelay;
        SummaryScreen_SetAnimDelayTaskId(taskId);
        SetSpriteCB_MonAnimDummy(sprite);
    }
    else
    {
        // No delay, start animation
        StartMonSummaryAnimation(sprite, gSpeciesInfo[species].frontAnimId);
    }
}

void StopPokemonAnimationDelayTask(void)
{
    u8 delayTaskId = FindTaskIdByFunc(Task_PokemonSummaryAnimateAfterDelay);
    if (delayTaskId != TASK_NONE)
        DestroyTask(delayTaskId);
}

void BattleAnimateBackSprite(struct Sprite *sprite, enum Species species)
{
    if (gHitMarker & HITMARKER_NO_ANIMATIONS && !((gBattleTypeFlags & BATTLE_TYPE_LINK)))
    {
        sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        LaunchAnimationTaskForBackSprite(sprite, GetSpeciesBackAnimSet(species));
        sprite->callback = SpriteCallbackDummy_2;
    }
}

// Identical to GetOpposingLinkMultiBattlerId but for the player
// "rightSide" from that team's perspective, i.e. B_POSITION_*_RIGHT
static u8 UNUSED GetOwnOpposingLinkMultiBattlerId(bool8 rightSide)
{
    s32 i;
    s32 battler = 0;
    u8 multiplayerId = GetMultiplayerId();
    switch (gLinkPlayers[multiplayerId].id)
    {
    case 0:
    case 2:
        battler = rightSide ? 3 : 1;
        break;
    case 1:
    case 3:
        battler = rightSide ? 2 : 0;
        break;
    }
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (gLinkPlayers[i].id == (s16)battler)
            break;
    }
    return i;
}

u8 GetOpposingLinkMultiBattlerId(bool8 rightSide, u8 multiplayerId)
{
    s32 i;
    s32 battler = 0;
    switch (gLinkPlayers[multiplayerId].id)
    {
    case 0:
    case 2:
        battler = rightSide ? 3 : 1;
        break;
    case 1:
    case 3:
        battler = rightSide ? 2 : 0;
        break;
    }
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (gLinkPlayers[i].id == (s16)battler)
            break;
    }
    return i;
}

enum TrainerPicID FacilityClassToPicIndex(u16 facilityClass)
{
    return gFacilityClassToPicIndex[facilityClass];
}

enum TrainerPicID PlayerGenderToFrontTrainerPicId(enum Gender playerGender)
{
    if (playerGender != MALE)
        return FacilityClassToPicIndex(IS_FRLG ? FACILITY_CLASS_LEAF : FACILITY_CLASS_MAY);
    else
        return FacilityClassToPicIndex(IS_FRLG ? FACILITY_CLASS_RED : FACILITY_CLASS_BRENDAN);
}

void HandleSetPokedexFlag(enum NationalDexOrder nationalNum, u8 caseId, u32 personality)
{
    u8 getFlagCaseId = (caseId == FLAG_SET_SEEN) ? FLAG_GET_SEEN : FLAG_GET_CAUGHT;
    if (!GetSetPokedexFlag(nationalNum, getFlagCaseId)) // don't set if it's already set
    {
        GetSetPokedexFlag(nationalNum, caseId);
        if (NationalPokedexNumToSpecies(nationalNum) == SPECIES_UNOWN)
            gSaveData.pokedex.unownPersonality = personality;
        if (NationalPokedexNumToSpecies(nationalNum) == SPECIES_SPINDA)
            gSaveData.pokedex.spindaPersonality = personality;
    }
}

void HandleSetPokedexFlagFromMon(struct Pokemon *mon, u32 caseId)
{
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    enum NationalDexOrder nationalNum = SpeciesToNationalPokedexNum(GetMonData(mon, MON_DATA_SPECIES));

    HandleSetPokedexFlag(nationalNum, caseId, personality);
}

bool8 HasTwoFramesAnimation(enum Species species)
{
    return P_TWO_FRAME_FRONT_SPRITES
        && gSpeciesInfo[species].frontAnimFrames != sAnims_SingleFramePlaceHolder
        && species != SPECIES_UNOWN
        && !gTestRunnerHeadless;
}

// The below functions are for the 'MonSpritesGfxManager', a method of allocating
// space for Pokémon sprites. These are only used for the summary screen Pokémon
// sprites (unless gMonSpritesGfxPtr is in use), but were set up for more general use.
// Only the 'default' mode (MON_SPR_GFX_MODE_NORMAL) is used, which is set
// up to allocate 4 sprites using the battler sprite templates (gBattlerSpriteTemplates).
// MON_SPR_GFX_MODE_BATTLE is identical but never used.
// MON_SPR_GFX_MODE_FULL_PARTY is set up to allocate 7 sprites (party + trainer?)
// using a generic 64x64 template, and is also never used.

// Between the unnecessarily large sizes below, a mistake allocating the spritePointers
// field, and the fact that ultimately only 1 of the 4 sprite positions is used, this
// system wastes a good deal of memory.

#define ALLOC_FAIL_BUFFER (1 << 0)
#define ALLOC_FAIL_STRUCT (1 << 1)
#define GFX_MANAGER_ACTIVE 0xA3 // Arbitrary value

static void InitMonSpritesGfx_Battle(struct MonSpritesGfxManager *gfx)
{
    u16 i, j;
    for (i = 0; i < gfx->numSprites; i++)
    {
        gfx->templates[i] = gBattlerSpriteTemplates[i];
        for (j = 0; j < gfx->numFrames; j++)
            gfx->frameImages[i * gfx->numFrames + j].data = &gfx->spritePointers[i][j * MON_PIC_SIZE];

        gfx->templates[i].images = &gfx->frameImages[i * gfx->numFrames];
    }
}

static void InitMonSpritesGfx_FullParty(struct MonSpritesGfxManager *gfx)
{
    u16 i, j;
    for (i = 0; i < gfx->numSprites; i++)
    {
        gfx->templates[i] = sSpriteTemplate_64x64;
        for (j = 0; j < gfx->numFrames; j++)
            gfx->frameImages[i * gfx->numSprites + j].data = &gfx->spritePointers[i][j * MON_PIC_SIZE];

        gfx->templates[i].images = &gfx->frameImages[i * gfx->numSprites];
        gfx->templates[i].anims = gAnims_MonPic;
        gfx->templates[i].paletteTag = i;
    }
}

struct MonSpritesGfxManager *CreateMonSpritesGfxManager(u8 managerId, u8 mode)
{
    u8 i;
    u8 failureFlags;
    struct MonSpritesGfxManager *gfx;

    failureFlags = 0;
    managerId %= MON_SPR_GFX_MANAGERS_COUNT;
    gfx = AllocZeroed(sizeof(*gfx));
    if (gfx == NULL)
        return NULL;

    switch (mode)
    {
    case MON_SPR_GFX_MODE_FULL_PARTY:
        gfx->numSprites = PARTY_SIZE + 1;
        gfx->numSprites2 = PARTY_SIZE + 1;
        gfx->numFrames = MAX_MON_PIC_FRAMES;
        gfx->dataSize = 1;
        gfx->mode = MON_SPR_GFX_MODE_FULL_PARTY;
        break;
 // case MON_SPR_GFX_MODE_BATTLE:
    case MON_SPR_GFX_MODE_NORMAL:
    default:
        gfx->numSprites = MAX_BATTLERS_COUNT;
        gfx->numSprites2 = MAX_BATTLERS_COUNT;
        gfx->numFrames = MAX_MON_PIC_FRAMES;
        gfx->dataSize = 1;
        gfx->mode = MON_SPR_GFX_MODE_NORMAL;
        break;
    }

    // Set up sprite / sprite pointer buffers
    gfx->spriteBuffer = AllocZeroed(gfx->dataSize * MON_PIC_SIZE * MAX_MON_PIC_FRAMES * gfx->numSprites);
    gfx->spritePointers = AllocZeroed(gfx->numSprites * 32); // ? Only * 4 is necessary, perhaps they were thinking bits.
    if (gfx->spriteBuffer == NULL || gfx->spritePointers == NULL)
    {
        failureFlags |= ALLOC_FAIL_BUFFER;
    }
    else
    {
        for (i = 0; i < gfx->numSprites; i++)
            gfx->spritePointers[i] = gfx->spriteBuffer + (gfx->dataSize * MON_PIC_SIZE * MAX_MON_PIC_FRAMES * i);
    }

    // Set up sprite structs
    gfx->templates = AllocZeroed(sizeof(struct SpriteTemplate) * gfx->numSprites);
    gfx->frameImages = AllocZeroed(sizeof(struct SpriteFrameImage) * gfx->numSprites * gfx->numFrames);
    if (gfx->templates == NULL || gfx->frameImages == NULL)
    {
        failureFlags |= ALLOC_FAIL_STRUCT;
    }
    else
    {
        for (i = 0; i < gfx->numFrames * gfx->numSprites; i++)
            gfx->frameImages[i].size = MON_PIC_SIZE;

        switch (gfx->mode)
        {
        case MON_SPR_GFX_MODE_FULL_PARTY:
            InitMonSpritesGfx_FullParty(gfx);
            break;
        case MON_SPR_GFX_MODE_NORMAL:
        case MON_SPR_GFX_MODE_BATTLE:
        default:
            InitMonSpritesGfx_Battle(gfx);
            break;
        }
    }

    // If either of the allocations failed free their respective members
    if (failureFlags & ALLOC_FAIL_STRUCT)
    {
        TRY_FREE_AND_SET_NULL(gfx->frameImages);
        TRY_FREE_AND_SET_NULL(gfx->templates);
    }
    if (failureFlags & ALLOC_FAIL_BUFFER)
    {
        TRY_FREE_AND_SET_NULL(gfx->spritePointers);
        TRY_FREE_AND_SET_NULL(gfx->spriteBuffer);
    }

    if (failureFlags)
    {
        // Clear, something failed to allocate
        memset(gfx, 0, sizeof(*gfx));
        Free(gfx);
    }
    else
    {
        gfx->active = GFX_MANAGER_ACTIVE;
        sMonSpritesGfxManagers[managerId] = gfx;
    }

    return sMonSpritesGfxManagers[managerId];
}

void DestroyMonSpritesGfxManager(u8 managerId)
{
    struct MonSpritesGfxManager *gfx;

    managerId %= MON_SPR_GFX_MANAGERS_COUNT;
    gfx = sMonSpritesGfxManagers[managerId];
    if (gfx == NULL)
        return;

    if (gfx->active != GFX_MANAGER_ACTIVE)
    {
        memset(gfx, 0, sizeof(*gfx));
    }
    else
    {
        TRY_FREE_AND_SET_NULL(gfx->frameImages);
        TRY_FREE_AND_SET_NULL(gfx->templates);
        TRY_FREE_AND_SET_NULL(gfx->spritePointers);
        TRY_FREE_AND_SET_NULL(gfx->spriteBuffer);
        memset(gfx, 0, sizeof(*gfx));
        Free(gfx);
    }
}

u8 *MonSpritesGfxManager_GetSpritePtr(u8 managerId, u8 spriteNum)
{
    struct MonSpritesGfxManager *gfx = sMonSpritesGfxManagers[managerId % MON_SPR_GFX_MANAGERS_COUNT];
    if (gfx->active != GFX_MANAGER_ACTIVE)
    {
        return NULL;
    }
    else
    {
        if (spriteNum >= gfx->numSprites)
            spriteNum = 0;

        return gfx->spritePointers[spriteNum];
    }
}

enum Species GetFormSpeciesId(enum Species speciesId, u8 formId)
{
    if (GetSpeciesFormTable(speciesId) != NULL)
        return GetSpeciesFormTable(speciesId)[formId];
    else
        return speciesId;
}

u8 GetFormIdFromFormSpeciesId(enum Species formSpeciesId)
{
    u8 targetFormId = 0;

    if (GetSpeciesFormTable(formSpeciesId) != NULL)
    {
        for (targetFormId = 0; GetSpeciesFormTable(formSpeciesId)[targetFormId] != FORM_SPECIES_END; targetFormId++)
        {
            if (formSpeciesId == GetSpeciesFormTable(formSpeciesId)[targetFormId])
                break;
        }
    }
    return targetFormId;
}

// Returns the current species if no form change is possible
enum Species GetFormChangeTargetSpeciesBoxMon(struct BoxPokemon *boxMon, enum FormChanges method)
{
    enum Species species = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    if (formChanges == NULL)
        return species;

    struct FormChangeContext ctx =
    {
        .method = method,
        .currentSpecies = species,
        .heldItem = GetBoxMonData(boxMon, MON_DATA_HELD_ITEM),
        .ability = GetAbilityBySpeciesAndItem(species, GetBoxMonData(boxMon, MON_DATA_ABILITY_NUM), GetBoxMonData(boxMon, MON_DATA_HELD_ITEM)),
        .partyItemUsed = gSpecialVar_ItemId,
        .multichoiceSelection = gSpecialVar_Result,
        .status = GetBoxMonData(boxMon, MON_DATA_STATUS),
    };

    return GetFormChangeTargetSpecies_Internal(ctx);
}

// Returns the current species if no form change is possible
enum Species GetFormChangeTargetSpecies(struct Pokemon *mon, enum FormChanges method)
{
    return GetFormChangeTargetSpeciesBoxMon(&mon->box, method);
}

enum Species GetFormChangeTargetSpecies_Internal(struct FormChangeContext ctx)
{
    u32 i;
    enum Species targetSpecies = ctx.currentSpecies;
    const struct FormChange *formChanges = GetSpeciesFormChanges(ctx.currentSpecies);

    if (formChanges == NULL)
        return ctx.currentSpecies;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (!(ctx.method == formChanges[i].method && ctx.currentSpecies != formChanges[i].targetSpecies))
            continue;

        switch (ctx.method)
        {
        case FORM_CHANGE_ITEM_HOLD:
            if ((ctx.heldItem == formChanges[i].param1 || formChanges[i].param1 == ITEM_NONE)
                && (ctx.ability == formChanges[i].param2 || formChanges[i].param2 == ABILITY_NONE))
            {
                // This is to prevent reverting to base form when giving the item to the corresponding form.
                // Eg. Giving a Zap Plate to an Electric Arceus without an item (most likely to happen when using givemon)
                bool32 currentItemForm = FALSE;
                for (u32 j = 0; formChanges[j].method != FORM_CHANGE_TERMINATOR; j++)
                {
                    if (ctx.currentSpecies == formChanges[j].targetSpecies
                        && formChanges[j].param1 == ctx.heldItem
                        && formChanges[j].param1 != ITEM_NONE)
                    {
                        currentItemForm = TRUE;
                        break;
                    }
                }
                if (!currentItemForm)
                    targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_ITEM_USE:
            if (ctx.partyItemUsed == formChanges[i].param1)
            {
                bool32 pass = TRUE;
                switch (formChanges[i].param2)
                {
                case DAY:
                    if (GetTimeOfDay() == TIME_NIGHT)
                        pass = FALSE;
                    break;
                case NIGHT:
                    if (GetTimeOfDay() != TIME_NIGHT)
                        pass = FALSE;
                    break;
                }

                if (formChanges[i].param3 != STATUS1_NONE && ctx.status & formChanges[i].param3)
                    pass = FALSE;

                if (pass)
                    targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_ITEM_USE_MULTICHOICE:
            if (ctx.partyItemUsed == formChanges[i].param1
             && ctx.multichoiceSelection == formChanges[i].param2)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_MOVE:
            if (ctx.learnedMove != formChanges[i].param2)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BEGIN_BATTLE:
        case FORM_CHANGE_END_BATTLE:
            if (ctx.heldItem == formChanges[i].param1 || formChanges[i].param1 == ITEM_NONE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_END_BATTLE_ENVIRONMENT:
            if (gBattleEnvironment == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_WITHDRAW:
        case FORM_CHANGE_DEPOSIT:
        case FORM_CHANGE_DAYS_PASSED:
        case FORM_CHANGE_BEGIN_WILD_ENCOUNTER:
            targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_FAINT:
        #if TESTING
            if (GetConfig(B_FAINTING_KEEPS_FORM) < GEN_CHAMPIONS || formChanges[i].param1 != DONT_REVERT_FORM_AFTER_FAINTING_IN_BATTLE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        #else
            if (formChanges[i].param1 != DONT_REVERT_FORM_AFTER_FAINTING_IN_BATTLE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        #endif
        case FORM_CHANGE_STATUS:
            if (ctx.status & formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_TIME_OF_DAY:
            switch (formChanges[i].param1)
            {
            case DAY:
                if (GetTimeOfDay() != TIME_NIGHT)
                    targetSpecies = formChanges[i].targetSpecies;
                break;
            case NIGHT:
                if (GetTimeOfDay() == TIME_NIGHT)
                    targetSpecies = formChanges[i].targetSpecies;
                break;
            }
            break;
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
        case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
            if (ctx.heldItem == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE:
            if (ctx.moves[0] == formChanges[i].param1
                || ctx.moves[1] == formChanges[i].param1
                || ctx.moves[2] == formChanges[i].param1
                || ctx.moves[3] == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_SWITCH_OUT:
            if (formChanges[i].param1 == ctx.ability || formChanges[i].param1 == ABILITY_NONE)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_HP_PERCENT_DURING_MOVE:
            if (ctx.ability == formChanges[i].param1
                && gCurrentMove == formChanges[i].param4)
            {
                // We multiply by 100 to make sure that integer division doesn't mess with the health check.
                u32 hpCheck = ctx.hp * 100 * 100 / ctx.maxHP;
                switch (formChanges[i].param2)
                {
                case HP_HIGHER_THAN:
                    if (hpCheck > formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                case HP_LOWER_EQ_THAN:
                    if (hpCheck <= formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                }
            }
            break;
        case FORM_CHANGE_BATTLE_HP_PERCENT_TURN_END:
        case FORM_CHANGE_BATTLE_HP_PERCENT_SEND_OUT:
            if (ctx.ability == formChanges[i].param1
                && ctx.level >= formChanges[i].param4)
            {
                // We multiply by 100 to make sure that integer division doesn't mess with the health check.
                u32 hpCheck = ctx.hp * 100 * 100 / ctx.maxHP;
                switch (formChanges[i].param2)
                {
                case HP_HIGHER_THAN:
                    if (hpCheck > formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                case HP_LOWER_EQ_THAN:
                    if (hpCheck <= formChanges[i].param3 * 100)
                        targetSpecies = formChanges[i].targetSpecies;
                    break;
                }
            }
            break;
        case FORM_CHANGE_BATTLE_WEATHER:
            // Check if there is a required ability and if the battler's ability does not match it
            // or is suppressed. If so, revert to the no weather form.
            if (formChanges[i].param2
                && ctx.ability != formChanges[i].param2
                && formChanges[i].param1 == B_WEATHER_NONE)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            // We need to revert the weather form if the field is under Air Lock, too.
            else if (!HasWeatherEffect() && formChanges[i].param1 == B_WEATHER_NONE)
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            // Otherwise, just check for a match between the weather and the form change table.
            // Added a check for whether the weather is in effect to prevent end-of-turn soft locks with Cloud Nine / Air Lock
            else if (((gBattleWeather & formChanges[i].param1) && HasWeatherEffect())
                || (gBattleWeather == B_WEATHER_NONE && formChanges[i].param1 == B_WEATHER_NONE))
            {
                targetSpecies = formChanges[i].targetSpecies;
            }
            break;
        case FORM_CHANGE_BATTLE_HIT_BY_MOVE_CATEGORY:
            if (ctx.ability == formChanges[i].param1
                && formChanges[i].param2 == GetBattleMoveCategory(gCurrentMove))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_SWITCH_IN:
        case FORM_CHANGE_BATTLE_TURN_END:
        case FORM_CHANGE_BATTLE_HIT_BY_CONFUSION_SELF_DMG:
        case FORM_CHANGE_BATTLE_BOND:
            if (formChanges[i].param1 == ctx.ability)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_TERASTALLIZATION:
            if (ctx.teraType == formChanges[i].param1)
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_BEFORE_MOVE:
        case FORM_CHANGE_BATTLE_AFTER_MOVE:
            if (formChanges[i].param1 == gCurrentMove
                && (formChanges[i].param2 == ABILITY_NONE || formChanges[i].param2 == ctx.ability))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_BATTLE_BEFORE_MOVE_CATEGORY:
            if (formChanges[i].param1 == GetBattleMoveCategory(gCurrentMove)
                && (formChanges[i].param2 == ABILITY_NONE || formChanges[i].param2 == ctx.ability))
                targetSpecies = formChanges[i].targetSpecies;
            break;
        case FORM_CHANGE_OVERWORLD_WEATHER:
        case FORM_CHANGE_TERMINATOR:
            break;
        }
    }

    return targetSpecies;
}

void TrySetDayLimitToFormChange(struct Pokemon *mon)
{
    u32 i;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_DAYS_PASSED && species != formChanges[i].targetSpecies)
        {
            SetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, &formChanges[i].param1);
            break;
        }
    }
}

bool32 DoesSpeciesHaveFormChangeMethod(enum Species species, enum FormChanges method)
{
    u32 i;
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    for (i = 0; formChanges != NULL && formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (method == formChanges[i].method && species != formChanges[i].targetSpecies)
            return TRUE;
    }

    return FALSE;
}

u16 MonTryLearningNewMoveEvolution(struct Pokemon *mon, bool8 firstMove)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 level = GetMonData(mon, MON_DATA_LEVEL);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    // Since you can learn more than one move per level,
    // the game needs to know whether you decided to
    // learn it or keep the old set to avoid asking
    // you to learn the same move over and over again.
    if (firstMove)
    {
        sLearningMoveTableID = 0;
    }
    while (learnset[sLearningMoveTableID].move != LEVEL_UP_MOVE_END)
    {
        while ((learnset[sLearningMoveTableID].level == 0 || learnset[sLearningMoveTableID].level == level)
             && !(P_EVOLUTION_LEVEL_1_LEARN >= GEN_8 && learnset[sLearningMoveTableID].level == 1))
        {
            gMoveToLearn = learnset[sLearningMoveTableID].move;
            sLearningMoveTableID++;
            return GiveMoveToMon(mon, gMoveToLearn);
        }
        sLearningMoveTableID++;
    }
    return 0;
}

void TrySpecialOverworldEvo(void)
{
    bool32 canStopEvo = FALSE;

    for (enum PartySlot i = PARTY_SLOT_0; i < PARTY_SLOT_NONE; i++)
    {
        enum Species targetSpecies = GetEvolutionTargetSpecies(&gParties[B_TRAINER_PLAYER][i], EVO_MODE_OVERWORLD_SPECIAL, 0, NULL, &canStopEvo, CHECK_EVO);

        if (targetSpecies != SPECIES_NONE && !(gTriedEvolving & (1u << i)))
        {
            GetEvolutionTargetSpecies(&gParties[B_TRAINER_PLAYER][i], EVO_MODE_OVERWORLD_SPECIAL, 0, NULL, &canStopEvo, DO_EVO);
            gTriedEvolving |= 1u << i;

            if (gMain.callback2 == TrySpecialOverworldEvo) // This fixes small graphics glitches.
                EvolutionScene(&gParties[B_TRAINER_PLAYER][i], targetSpecies, canStopEvo, i);
            else
                BeginEvolutionScene(&gParties[B_TRAINER_PLAYER][i], targetSpecies, canStopEvo, i);

            gCB2_AfterEvolution = TrySpecialOverworldEvo;
            return;
        }
    }

    gTriedEvolving = 0;
    SetMainCallback2(CB2_ReturnToField);
}

bool32 SpeciesHasGenderDifferences(enum Species species)
{
#if P_GENDER_DIFFERENCES
    if (gSpeciesInfo[species].frontPicFemale != NULL
     || gSpeciesInfo[species].backPicFemale != NULL
     || gSpeciesInfo[species].paletteFemale != NULL
     || gSpeciesInfo[species].shinyPaletteFemale != NULL
     || gSpeciesInfo[species].iconSpriteFemale != NULL)
        return TRUE;
#endif

    return FALSE;
}

static struct PartyState *GetBattlerPartyStateByPokemon(struct Pokemon *partyMon, enum BattleTrainer trainer)
{
    struct Pokemon *party = GetTrainerParty(trainer);

    for (int i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &party[i];
        if (partyMon == mon)
            return &gBattleStruct->partyState[trainer][i];
    }
    return NULL;
}

bool32 TryFormChange(struct Pokemon *mon, enum FormChanges method, enum BattleTrainer trainer)
{
    if (GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_NONE
     || GetMonData(mon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_EGG)
        return FALSE;

    enum Species currentSpecies = GetMonData(mon, MON_DATA_SPECIES);
    enum Species targetSpecies = GetFormChangeTargetSpecies(mon, method);

    struct PartyState *battlePartyState = GetBattlerPartyStateByPokemon(mon, trainer);
    // If the battle ends, and there's not a specified species to change back to,
    // use the species at the start of the battle.
    if (targetSpecies == SPECIES_NONE
        && battlePartyState != NULL && battlePartyState->changedSpecies != SPECIES_NONE
        // This is added to prevent FORM_CHANGE_END_BATTLE_ENVIRONMENT from omitting move changes
        // at the end of the battle, as it was being counting as a successful form change.
        && (method == FORM_CHANGE_END_BATTLE || method == FORM_CHANGE_FAINT))
    {
        targetSpecies = battlePartyState->changedSpecies;
    }

    assertf(targetSpecies != SPECIES_NONE, "form change target returned NONE. cur:%d, method:%d", currentSpecies, method)
    {
        return FALSE;
    }

    if (targetSpecies != currentSpecies)
    {
        TryToSetBattleFormChangeMoves(mon, method);
        SetMonData(mon, MON_DATA_SPECIES, &targetSpecies);
        TrySetDayLimitToFormChange(mon);
        CalculateMonStats(mon);
        return TRUE;
    }

    return FALSE;
}

bool32 TryBoxMonFormChange(struct BoxPokemon *boxMon, enum FormChanges method)
{
    if (GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_NONE
     || GetBoxMonData(boxMon, MON_DATA_SPECIES_OR_EGG, 0) == SPECIES_EGG)
        return FALSE;

    enum Species currentSpecies = GetBoxMonData(boxMon, MON_DATA_SPECIES, NULL);
    enum Species targetSpecies = GetFormChangeTargetSpeciesBoxMon(boxMon, method);

    assertf(targetSpecies != SPECIES_NONE, "form change target returned NONE. cur:%d, method:%d", currentSpecies, method)
    {
        return FALSE;
    }

    if (targetSpecies != currentSpecies)
    {
        SetBoxMonData(boxMon, MON_DATA_SPECIES, &targetSpecies);
        return TRUE;
    }
    return FALSE;
}

enum Species SanitizeSpeciesId(enum Species species)
{
    assertf(species <= NUM_SPECIES, "invalid species: %d", species)
    {
        return SPECIES_NONE;
    }

    assertf(species == SPECIES_NONE || IsSpeciesEnabled(species), "disabled species: %d", species)
    {
        return SPECIES_NONE;
    }

    return species;
}

bool32 IsSpeciesEnabled(enum Species species)
{
    // This function should not use the GetSpeciesBaseHP function, as the included sanitation will result in an infinite loop
    return gSpeciesInfo[species].baseHP > 0 || species == SPECIES_EGG;
}

void TryToSetBattleFormChangeMoves(struct Pokemon *mon, enum FormChanges method)
{
    int i, j;
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct FormChange *formChanges = GetSpeciesFormChanges(species);

    if (formChanges == NULL
        || (method != FORM_CHANGE_BEGIN_BATTLE && method != FORM_CHANGE_END_BATTLE))
        return;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == method
            && formChanges[i].param2
            && formChanges[i].param3
            && formChanges[i].targetSpecies != species)
        {
            u16 originalMove = formChanges[i].param2;
            u16 newMove = formChanges[i].param3;

            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                u16 currMove = GetMonData(mon, MON_DATA_MOVE1 + j);
                if (currMove == originalMove)
                    SetMonMoveSlot(mon, newMove, j);
            }
            break;
        }
    }
}


void UpdateMonPersonality(struct BoxPokemon *boxMon, u32 personality)
{
    bool32 isShiny = GetBoxMonData(boxMon, MON_DATA_IS_SHINY);
    u32 hiddenNature = GetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE);
    enum Type teraType = GetBoxMonData(boxMon, MON_DATA_TERA_TYPE);

    boxMon->personality = personality;

    SetBoxMonData(boxMon, MON_DATA_IS_SHINY, &isShiny);
    SetBoxMonData(boxMon, MON_DATA_HIDDEN_NATURE, &hiddenNature);
    SetBoxMonData(boxMon, MON_DATA_TERA_TYPE, &teraType);
}

void HealPokemon(struct Pokemon *mon)
{
    u32 data;

    data = GetMonData(mon, MON_DATA_MAX_HP);
    SetMonData(mon, MON_DATA_HP, &data);

    data = STATUS1_NONE;
    SetMonData(mon, MON_DATA_STATUS, &data);
}

void HealBoxPokemon(struct BoxPokemon *boxMon)
{
    u32 data;

    data = 0;
    SetBoxMonData(boxMon, MON_DATA_HP_LOST, &data);

    data = STATUS1_NONE;
    SetBoxMonData(boxMon, MON_DATA_STATUS, &data);
}

enum PokemonCry GetCryIdBySpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    if (P_CRIES_ENABLED == FALSE || gSpeciesInfo[species].cryId >= CRY_COUNT || gTestRunnerHeadless)
        return CRY_NONE;
    return gSpeciesInfo[species].cryId;
}

enum Species GetSpeciesPreEvolution(enum Species species)
{
    int i, j;

    for (i = SPECIES_BULBASAUR; i < NUM_SPECIES; i++)
    {
        if (!IsSpeciesEnabled(i))
            continue;

        const struct Evolution *evolutions = GetSpeciesEvolutions(i);
        if (evolutions == NULL)
            continue;

        for (j = 0; evolutions[j].method != EVOLUTIONS_END; j++)
        {
            if (IsSpeciesEnabled(evolutions[j].targetSpecies) && SanitizeSpeciesId(evolutions[j].targetSpecies) == species)
                return i;
        }
    }

    return SPECIES_NONE;
}

void UpdateDaysPassedSinceFormChange(u16 days)
{
    u32 i;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species currentSpecies = GetMonData(mon, MON_DATA_SPECIES);
        u8 daysSinceFormChange;

        if (currentSpecies == SPECIES_NONE)
            continue;

        daysSinceFormChange = GetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, 0);
        if (daysSinceFormChange == 0)
            continue;

        if (daysSinceFormChange > days)
            daysSinceFormChange -= days;
        else
            daysSinceFormChange = 0;

        SetMonData(mon, MON_DATA_DAYS_SINCE_FORM_CHANGE, &daysSinceFormChange);

        if (daysSinceFormChange == 0)
            TryFormChange(mon, FORM_CHANGE_DAYS_PASSED, B_TRAINER_PLAYER);
    }
}

enum Type CheckDynamicMoveType(struct Pokemon *mon, enum Move move, enum BattlerId battler, enum MonState state)
{
    enum Type moveType = GetDynamicMoveType(mon, move, battler, GetBattlerAbility(battler), GetBattlerHoldEffect(battler), state);
    if (moveType != TYPE_NONE)
        return moveType;
    return GetMoveType(move);
}

enum Species GetBaseSpecies(enum Species species)
{
    return GetFormSpeciesId(species, 0);
}

bool32 IsSpeciesRegionalForm(enum Species species)
{
    return gSpeciesInfo[species].isAlolanForm
        || gSpeciesInfo[species].isGalarianForm
        || gSpeciesInfo[species].isHisuianForm
        || gSpeciesInfo[species].isPaldeanForm;
}

bool32 IsSpeciesRegionalFormFromRegion(enum Species species, enum Region region)
{
    switch (region)
    {
    case REGION_ALOLA:  return gSpeciesInfo[species].isAlolanForm;
    case REGION_GALAR:  return gSpeciesInfo[species].isGalarianForm;
    case REGION_HISUI:  return gSpeciesInfo[species].isHisuianForm;
    case REGION_PALDEA: return gSpeciesInfo[species].isPaldeanForm;
    default:            return FALSE;
    }
}

bool32 SpeciesHasRegionalForm(enum Species species)
{
    u32 formId;
    const u16 *formTable = GetSpeciesFormTable(species);
    for (formId = 0; formTable != NULL && formTable[formId] != FORM_SPECIES_END; formId++)
    {
        if (IsSpeciesRegionalForm(formTable[formId]))
            return TRUE;
    }
    return FALSE;
}

enum Species GetRegionalFormByRegion(enum Species species, enum Region region)
{
    u32 formId = 0;
    enum Species firstFoundSpecies = 0;
    const u16 *formTable = GetSpeciesFormTable(species);

    if (formTable != NULL)
    {
        for (formId = 0; formTable[formId] != FORM_SPECIES_END; formId++)
        {
            if (firstFoundSpecies == 0)
                firstFoundSpecies = formTable[formId];

            if (IsSpeciesRegionalFormFromRegion(formTable[formId], region))
                return formTable[formId];
        }
        if (firstFoundSpecies != 0)
            return firstFoundSpecies;
    }
    return species;
}

bool32 IsSpeciesForeignRegionalForm(enum Species species, enum Region currentRegion)
{
    for (enum Region i = 0; i < REGIONS_COUNT; i++)
    {
        if (currentRegion != i && IsSpeciesRegionalFormFromRegion(species, i))
            return TRUE;
        else if (currentRegion == i && SpeciesHasRegionalForm(species) && !IsSpeciesRegionalFormFromRegion(species, i))
            return TRUE;
    }
    return FALSE;
}

enum Type GetTeraTypeFromPersonality(struct Pokemon *mon)
{
    const u8 *types = gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES)].types;
    return (GetMonData(mon, MON_DATA_PERSONALITY) & 0x1) == 0 ? types[0] : types[1];
}

struct BoxPokemon *GetSavedPlayerPartyMon(u32 index)
{
    return &gSaveData.playerParty[index];
}

u8 *GetSavedPlayerPartyCount(void)
{
    return &gSaveData.playerPartyCount;
}

void SaveMonToBox(struct BoxPokemon *dst, const struct Pokemon *src)
{
    u32 hpLost = src->maxHP >= src->hp ? src->maxHP - src->hp : 0;
    *dst = src->box;
    SetBoxMonData(dst, MON_DATA_HP_LOST, &hpLost);
    SetBoxMonData(dst, MON_DATA_STATUS, &src->status);
}

void SavePlayerPartyMon(u32 index, struct Pokemon *mon)
{
    SaveMonToBox(&gSaveData.playerParty[index], mon);
    gSaveData.playerPartyMail[index] = mon->mail;
}

bool32 IsSpeciesOfType(enum Species species, enum Type type)
{
    if (gSpeciesInfo[species].types[0] == type
     || gSpeciesInfo[species].types[1] == type)
        return TRUE;
    return FALSE;
}

struct BoxPokemon *GetSelectedBoxMonFromPcOrParty(void)
{
    struct BoxPokemon *boxmon;
    if (gSpecialVar_0x8004 == PC_MON_CHOSEN)
        boxmon = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    else
        boxmon = &(gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004].box);
    return boxmon;
}

u32 GiveScriptedMonToPlayer(struct Pokemon *mon, u8 slot)
{
    u32 sentToPc;
    u32 i = 0;
    if (slot < PARTY_SIZE)
    {
        CopyMon(&gParties[B_TRAINER_PLAYER][slot], mon, sizeof(struct Pokemon));
        sentToPc = MON_GIVEN_TO_PARTY;
    }
    else
    {
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_NONE)
                break;
        }
        if (i >= PARTY_SIZE)
        {
            sentToPc = CopyMonToPC(mon);
        }
        else
        {
            sentToPc = MON_GIVEN_TO_PARTY;
            CopyMon(&gParties[B_TRAINER_PLAYER][i], mon, sizeof(struct Pokemon));
            gPartiesCount[B_TRAINER_PLAYER] = i + 1;
        }
    }
    if (sentToPc != MON_CANT_GIVE)
    {
        HandleSetPokedexFlagFromMon(mon, FLAG_SET_SEEN);
        HandleSetPokedexFlagFromMon(mon, FLAG_SET_CAUGHT);
    }
    CalculatePlayerPartyCount();
    return sentToPc;
}

void ChangePokemonNicknameWithCallback(void (*callback)(void))
{
    struct BoxPokemon *boxMon = GetSelectedBoxMonFromPcOrParty();
    GetBoxMonData(boxMon, MON_DATA_NICKNAME, gStringVar3);
    GetBoxMonData(boxMon, MON_DATA_NICKNAME, gStringVar2);
    DoNamingScreen(NAMING_SCREEN_NICKNAME, gStringVar2, GetBoxMonData(boxMon, MON_DATA_SPECIES), GetBoxMonGender(boxMon), GetBoxMonData(boxMon, MON_DATA_PERSONALITY), callback);
}

bool32 HasShedinjaHPHandling(enum Species species)
{
    if (species == SPECIES_SHEDINJA)
        return TRUE;
    if (P_BASE_HP_1_SHEDINJA_HANDLING && GetSpeciesBaseHP(species) == 1)
        return TRUE;
    return FALSE;
}

static u32 ResolveAbility(enum Species species, u32 abilityNum)
{
    assertf(abilityNum < NUM_ABILITY_SLOTS && GetAbilityBySpecies(species, abilityNum) != ABILITY_NONE,
            "invalid ability num %d for species %d", abilityNum, species)
    {
        return 0;
    }
    return abilityNum;
}

static enum PokeBall ResolveBall(u32 ballTemplate)
{
    if (ballTemplate < POKEBALL_COUNT)
        return ballTemplate;
    if (ballTemplate == BALL_RANDOM)
        return GetRandomBall();

    errorf("Unknown ball value %d when creating pokemon", ballTemplate);
    return BALL_STRANGE;
}

void ResolveEVs(const u16 *evsTemplate, u8 *evs, bool32 ignoreTotalEvCheck)
{
    u32 evTotal = 0;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        if (evsTemplate[i] <= MAX_PER_STAT_EVS)
        {
            evs[i] = evsTemplate[i];
        }
        else
        {
            errorf("invalid ev value of %d above maximum of %d", evs[i], MAX_PER_STAT_EVS);
            evs[i] = 0;
        }
        evTotal += evs[i];
    }
    if (!OW_CHECK_FOR_TOTAL_EVS || ignoreTotalEvCheck)
        return;
    assertf(evTotal <= MAX_TOTAL_EVS, "invalid total ev value of %d above maximum of %d", evTotal, MAX_TOTAL_EVS)
    {
        for (u32 i = 0; i < NUM_STATS; i++)
        {
            evs[i] = 0;
        }
    }
}

static enum Species ResolveSpecies(u32 speciesTemplate)
{
    assertf (IsSpeciesEnabled(speciesTemplate), "unknown species %d when creating pokemon", speciesTemplate)
    {
        return SPECIES_PORYGON;
    }
    return speciesTemplate;
}

static u32 ResolveLevel(u32 levelTemplate)
{
    assertf (MIN_LEVEL <= levelTemplate &&levelTemplate <= MAX_LEVEL, "using invalid level %d when creating pokemon", levelTemplate)
    {
        return MIN_LEVEL;
    }
    return levelTemplate;
}

static u32 ResolvePersonality(enum Species species, u32 genderTemplate, u32 natureTemplate, enum GeneratedMonOrigin originTemplate)
{
    u32 gender;
    u32 nature;
    assertf(originTemplate != UNDEFINED_MON_ORIGIN, "origin was not explicitly set when creating pokemon")
    {
        // if the origin was not explictly set, never apply Synchronize or Cute Charm effects
        if (genderTemplate == MON_GENDER_MAY_CUTE_CHARM)
            genderTemplate = MON_GENDER_RANDOM;
        if (natureTemplate == NATURE_MAY_SYNCHRONIZE)
            natureTemplate = NATURE_RANDOM;
    }
    if (genderTemplate == MON_GENDER_MAY_CUTE_CHARM)
        gender = GetSynchronizedGender(originTemplate, genderTemplate);
    else
        gender = genderTemplate;
    if (natureTemplate == NATURE_MAY_SYNCHRONIZE)
        nature = GetSynchronizedNature(originTemplate, natureTemplate);
    else
        nature = natureTemplate;
    return GetMonPersonality(species, gender, nature, RANDOM_UNOWN_LETTER);
}

static bool32 ResolveShinyness(u32 isShinyTemplate)
{
    assertf(isShinyTemplate == TRUE || isShinyTemplate == FALSE, "using non boolean isShiny value %d when creating pokemon", isShinyTemplate)
    {
        return FALSE;
    }
    return isShinyTemplate;
}

static bool32 ResolveIsEgg(u32 isEggTemplate)
{
    assertf(isEggTemplate == TRUE || isEggTemplate == FALSE, "using non boolean isEgg value %d when creating pokemon", isEggTemplate)
    {
        return FALSE;
    }
    return isEggTemplate;
}

static bool32 ResolveTeraType(u32 teraTypeTemplate)
{
    assertf(teraTypeTemplate != TYPE_NONE && teraTypeTemplate != TYPE_MYSTERY && teraTypeTemplate < NUMBER_OF_MON_TYPES, "using invalid tera type %d when creating pokemon", teraTypeTemplate)
    {
        return TYPE_STELLAR;
    }
    return teraTypeTemplate;
}

static enum Item ResolveHeldItem(u32 heldItemTemplate)
{
    assertf(heldItemTemplate < ITEMS_COUNT,"using invalid item %d when creating pokemon", heldItemTemplate)
    {
        return ITEM_NONE;
    }
    assertf(!ItemIsMail(heldItemTemplate) && !GetItemImportance(heldItemTemplate), "trying to give item %d that can't be held to newly created pokemon", heldItemTemplate)
    {
        return ITEM_NONE;
    }
    return heldItemTemplate;
}

void CreateMonFromTemplate(struct Pokemon *mon, const struct PokemonTemplate *monTemplate)
{
    enum Species species = ResolveSpecies(monTemplate->species);
    u8 level = ResolveLevel(monTemplate->level);
    u32 personality = ResolvePersonality(species, monTemplate->gender, monTemplate->nature, monTemplate->origin);
    CreateMon(mon, species, level, personality, OTID_STRUCT_PLAYER_ID);

    enum Item heldItem = ResolveHeldItem(monTemplate->heldItem);
    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);

    if (monTemplate->doNotUseDefaultBall)
    {
        enum PokeBall ball = ResolveBall(monTemplate->ball);
        SetMonData(mon, MON_DATA_POKEBALL, &ball);
    }

    if (monTemplate->doNotUseDefaultShinyness && monTemplate->isShiny != SHINY_MODE_RANDOM)
    {
        bool32 isShiny = ResolveShinyness(monTemplate->isShiny);
        SetMonData(mon, MON_DATA_IS_SHINY, &isShiny);
    }

    if (monTemplate->doNotUseDefaultAbility)
    {
        u8 abilityNum = ResolveAbility(species, monTemplate->abilityNum);
        SetMonData(mon, MON_DATA_ABILITY_NUM, &abilityNum);
    }

    u8 evs[NUM_STATS];
    ResolveEVs(monTemplate->evs, evs, monTemplate->ignoreTotalEvCheck);
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }

    enum Move moves[MAX_MON_MOVES];
    ResolveMoves(species, level, monTemplate->moves, moves);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        SetMonMoveSlot(mon, moves[i], i);
    }

    if (monTemplate->doNotUseDefaultTeraType)
    {
        enum Type teraType = ResolveTeraType(monTemplate->teraType);
        SetMonData(mon, MON_DATA_TERA_TYPE, &teraType);
    }

    bool32 isEgg = ResolveIsEgg(monTemplate->isEgg);
    SetMonData(mon, MON_DATA_IS_EGG, &isEgg);

    CalculateMonStats(mon);
    TryFormChange(mon, FORM_CHANGE_ITEM_HOLD, B_TRAINER_PLAYER);
}
