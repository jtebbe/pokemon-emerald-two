#include "global.h"
#include "battle_bingo.h"
#include "constants/battle_ai.h"
#include "constants/flags.h"
#include "constants/pokemon.h"
#include "constants/songs.h"

static const u8 sBattleBingoText_FWGBoard[] = _("FWG Board");
static const u8 sBattleBingoText_NormalBoard[] = _("Normal Board");
static const u8 sBattleBingoText_LCBoard[] = _("Little Cup");
static const u8 sBattleBingoText_CipherBoard[] = _("Cipher Board");
static const u8 sBattleBingoText_LegendBoard[] = _("Legendary");
static const u8 sBattleBingoText_DragonBoard[] = _("Dragon Board");
static const u8 sBattleBingoText_StrangeBoard[] = _("Strange Board");
static const u8 sBattleBingoText_MusicBoard[] = _("Music Board");
static const u8 sBattleBingoText_MonsterBoard[] = _("Monster Board");

static const struct BattleBingoItemRule sBattleBingoFWGBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_FULL_RESTORE, 2},
    {ITEM_MAX_REVIVE, 1},
    {ITEM_FOCUS_SASH, 1},
    {ITEM_CHOICE_SCARF, 1},
    {ITEM_LIFE_ORB, 1},
};

static const struct BattleBingoHealRule sBattleBingoFWGBoardHealRules[] =
{
    {1},
};

static const struct BattleBingoWildRule sBattleBingoFWGBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_FIRE,
        .forbiddenTags = 0,
        .count = 5,
        .type = TYPE_FIRE,
    },
    {
        .requiredTags = MON_POOL_TAG_WATER,
        .forbiddenTags = 0,
        .count = 5,
        .type = TYPE_WATER,
    },
    {
        .requiredTags = MON_POOL_TAG_GRASS,
        .forbiddenTags = 0,
        .count = 5,
        .type = TYPE_GRASS,
    },
};

static const u16 sBattleBingoFWGBoardBosses[] =
{
    TRAINER_BINGO_BOSS_LANCE,
};

static const struct BattleBingoItemRule sBattleBingoNormalBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 3},
    {ITEM_SILK_SCARF, 1},
    {ITEM_MAX_REVIVE, 1},
    {ITEM_FULL_RESTORE, 2},
    {ITEM_MAX_ELIXIR, 1},
};

static const struct BattleBingoHealRule sBattleBingoNormalBoardHealRules[] =
{
    {1},
};

static const struct BattleBingoWildRule sBattleBingoNormalBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_NORMAL,
        .forbiddenTags = 0,
        .count = 15,
        .type = TYPE_NORMAL,
    },
};

static const u16 sBattleBingoNormalBoardBosses[] =
{
    TRAINER_NORMAN_3,
};

static const struct BattleBingoItemRule sBattleBingoLCBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_EVIOLITE, 1},
    {ITEM_BERRY_JUICE, 1},
    {ITEM_WEAKNESS_POLICY, 1},
};

static const struct BattleBingoHealRule sBattleBingoLCBoardHealRules[] =
{
    {4},
};

static const struct BattleBingoWildRule sBattleBingoLCBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_BABY,
        .forbiddenTags = 0,
        .count = 15,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoLCBoardBosses[] =
{
    TRAINER_BINGO_BOSS_SUZIE,
};

static const struct BattleBingoItemRule sBattleBingoCipherBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_MAX_REVIVE, 1},
    {ITEM_LIFE_ORB, 1},
    {ITEM_ASSAULT_VEST, 1},
};

static const struct BattleBingoHealRule sBattleBingoCipherBoardHealRules[] =
{
    {5},
};

static const struct BattleBingoWildRule sBattleBingoCipherBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_CIPHER,
        .forbiddenTags = 0,
        .count = 10,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoCipherBoardBosses[] =
{
    TRAINER_BINGO_BOSS_NASCOUR,
    TRAINER_BINGO_BOSS_DAKIM,
    TRAINER_BINGO_BOSS_EIN,
    TRAINER_BINGO_BOSS_MIROR_B,
    TRAINER_BINGO_BOSS_VENUS,
};

static const struct BattleBingoItemRule sBattleBingoLegendBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_MAX_REVIVE, 2},
    {ITEM_FOCUS_SASH, 1},
    {ITEM_CHOICE_SCARF, 1},
    {ITEM_LEFTOVERS, 1},
};

static const struct BattleBingoHealRule sBattleBingoLegendBoardHealRules[] =
{
    {2},
};

static const struct BattleBingoWildRule sBattleBingoLegendBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_LEGENDARY,
        .forbiddenTags = 0,
        .count = 15,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoLegendBoardBosses[] =
{
    TRAINER_BINGO_BOSS_ANABEL,
};

static const struct BattleBingoItemRule sBattleBingoDragonBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_MAX_REVIVE, 2},
    {ITEM_FULL_RESTORE, 1},
    {ITEM_EXPERT_BELT, 1},
};

static const struct BattleBingoHealRule sBattleBingoDragonBoardHealRules[] =
{
    {3},
};

static const struct BattleBingoWildRule sBattleBingoDragonBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_DRAGON,
        .forbiddenTags = MON_POOL_TAG_LEGENDARY
            | MON_POOL_TAG_SUBLEGENDARY
            | MON_POOL_TAG_MYTHICAL
            | MON_POOL_TAG_PARADOX
            | MON_POOL_TAG_ULTRA_BEAST,
        .count = 15,
        .type = TYPE_DRAGON,
    },
};

static const u16 sBattleBingoDragonBoardBosses[] =
{
    TRAINER_BINGO_BOSS_DRAGON_LANCE,
};

static const struct BattleBingoItemRule sBattleBingoStrangeBoardItemRules[] =
{
    {ITEM_FULL_RESTORE, 2},
    {ITEM_MAX_REVIVE, 2},
    {ITEM_MASTER_BALL, 2},
    {ITEM_ASSAULT_VEST, 1},
    {ITEM_CHOICE_BAND, 1},
    {ITEM_CHOICE_SPECS, 1},
    {ITEM_BOOSTER_ENERGY, 1},
};

static const struct BattleBingoWildRule sBattleBingoStrangeBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_STRANGE,
        .forbiddenTags = 0,
        .count = 14,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoStrangeBoardBosses[] =
{
    TRAINER_BINGO_BOSS_TUCKER,
};

static const struct BattleBingoItemRule sBattleBingoMusicBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_FULL_RESTORE, 2},
    {ITEM_MAX_REVIVE, 2},
    {ITEM_THROAT_SPRAY, 1},
};

static const struct BattleBingoHealRule sBattleBingoMusicBoardHealRules[] =
{
    {2},
};

static const struct BattleBingoWildRule sBattleBingoMusicBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_MUSIC,
        .forbiddenTags = 0,
        .count = 15,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoMusicBoardBosses[] =
{
    TRAINER_BINGO_BOSS_EDDIE,
};

static const struct BattleBingoItemRule sBattleBingoMonsterBoardItemRules[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_FULL_RESTORE, 2},
    {ITEM_MAX_REVIVE, 2},
};

static const struct BattleBingoHealRule sBattleBingoMonsterBoardHealRules[] =
{
    {3},
};

static const struct BattleBingoWildRule sBattleBingoMonsterBoardWildRules[] =
{
    {
        .requiredTags = MON_POOL_TAG_MONSTER,
        .forbiddenTags = 0,
        .count = 15,
        .type = TYPE_MYSTERY,
    },
};

static const u16 sBattleBingoMonsterBoardBosses[] =
{
    TRAINER_BINGO_BOSS_STEVEN,
};

const struct BattleBingoBoardRules gBattleBingoBoardRules[BATTLE_BINGO_BOARD_COUNT] =
{
    [BATTLE_BINGO_BOARD_FWG] =
    {
        .title = sBattleBingoText_FWGBoard,
        .clearedFlag = FLAG_CLEARED_FWG_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_CHARIZARD_KANTO_STARTER_1,
            BINGO_MON_BLASTOISE_KANTO_STARTER_1,
            BINGO_MON_VENUSAUR_KANTO_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 50,
        .items = sBattleBingoFWGBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoFWGBoardItemRules),
        .heals = sBattleBingoFWGBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoFWGBoardHealRules),
        .wilds = sBattleBingoFWGBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoFWGBoardWildRules),
        .bosses = sBattleBingoFWGBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoFWGBoardBosses),
        .mysteryWildCount = 4,
        .mysteryItemCount = 6,
        .music = MUS_BRAMBLE_BLAST,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_1X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_NORMAL] =
    {
        .title = sBattleBingoText_NormalBoard,
        .clearedFlag = FLAG_CLEARED_NORMAL_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_FIXED,
        .starters = {
            BINGO_MON_TAUROS_NORMAL_STARTER_1,
        },
        .starterCount = 1,
        .starterLevel = 50,
        .items = sBattleBingoNormalBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoNormalBoardItemRules),
        .heals = sBattleBingoNormalBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoNormalBoardHealRules),
        .wilds = sBattleBingoNormalBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoNormalBoardWildRules),
        .bosses = sBattleBingoNormalBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoNormalBoardBosses),
        .mysteryWildCount = 5,
        .mysteryItemCount = 5,
        .music = MUS_BINGO_BOARD,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_1X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_LC] =
    {
        .title = sBattleBingoText_LCBoard,
        .clearedFlag = FLAG_CLEARED_LC_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_GROOKEY_LC_STARTER_1,
            BINGO_MON_FUECOCO_LC_STARTER_1,
            BINGO_MON_QUAXLY_LC_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 5,
        .items = sBattleBingoLCBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoLCBoardItemRules),
        .heals = sBattleBingoLCBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoLCBoardHealRules),
        .wilds = sBattleBingoLCBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoLCBoardWildRules),
        .bosses = sBattleBingoLCBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoLCBoardBosses),
        .mysteryWildCount = 3,
        .mysteryItemCount = 3,
        .music = MUS_ORDON_VILLAGE,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_2X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_CIPHER] =
    {
        .title = sBattleBingoText_CipherBoard,
        .clearedFlag = FLAG_CLEARED_CIPHER_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_ESPEON_CIPHER_STARTER_1,
            BINGO_MON_UMBREON_CIPHER_STARTER_1,
        },
        .starterCount = 2,
        .starterLevel = 50,
        .items = sBattleBingoCipherBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoCipherBoardItemRules),
        .heals = sBattleBingoCipherBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoCipherBoardHealRules),
        .wilds = sBattleBingoCipherBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoCipherBoardWildRules),
        .bosses = sBattleBingoCipherBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoCipherBoardBosses),
        .mysteryWildCount = 0,
        .mysteryItemCount = 0,
        .music = MUS_BIOLIZARD,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_10X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_LEGENDARY] =
    {
        .title = sBattleBingoText_LegendBoard,
        .clearedFlag = FLAG_CLEARED_LEGEND_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_GROUDON_LEGEND_STARTER_1,
            BINGO_MON_KYOGRE_LEGEND_STARTER_1,
            BINGO_MON_RAYQUAZA_LEGEND_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 50,
        .items = sBattleBingoLegendBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoLegendBoardItemRules),
        .heals = sBattleBingoLegendBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoLegendBoardHealRules),
        .wilds = sBattleBingoLegendBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoLegendBoardWildRules),
        .bosses = sBattleBingoLegendBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoLegendBoardBosses),
        .mysteryWildCount = 3,
        .mysteryItemCount = 3,
        .music = MUS_CORRIDORS_OF_TIME,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_3X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_DRAGON] =
    {
        .title = sBattleBingoText_DragonBoard,
        .clearedFlag = FLAG_CLEARED_DRAGON_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_FIXED,
        .starters = {
            BINGO_MON_DRAGONITE_DRAGON_STARTER_1,
        },
        .starterCount = 1,
        .starterLevel = 50,
        .items = sBattleBingoDragonBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoDragonBoardItemRules),
        .heals = sBattleBingoDragonBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoDragonBoardHealRules),
        .wilds = sBattleBingoDragonBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoDragonBoardWildRules),
        .bosses = sBattleBingoDragonBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoDragonBoardBosses),
        .mysteryWildCount = 5,
        .mysteryItemCount = 3,
        .music = MUS_MOLGERA,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_2X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_STRANGE] =
    {
        .title = sBattleBingoText_StrangeBoard,
        .clearedFlag = FLAG_CLEARED_STRANGE_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_ZAPDOS_GALAR_STRANGE_STARTER_1,
            BINGO_MON_SANDY_SHOCKS_STRANGE_STARTER_1,
            BINGO_MON_KARTANA_STRANGE_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 50,
        .items = sBattleBingoStrangeBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoStrangeBoardItemRules),
        .wilds = sBattleBingoStrangeBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoStrangeBoardWildRules),
        .bosses = sBattleBingoStrangeBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoStrangeBoardBosses),
        .mysteryWildCount = 5,
        .mysteryItemCount = 5,
        .music = MUS_RASPUTIN,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_2X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
    [BATTLE_BINGO_BOARD_MUSIC] =
    {
        .title = sBattleBingoText_MusicBoard,
        .clearedFlag = FLAG_CLEARED_MUSIC_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_FIXED,
        .starters = {
            BINGO_MON_EXPLOUD_MUSIC_STARTER_1,
        },
        .starterCount = 1,
        .starterLevel = 50,
        .items = sBattleBingoMusicBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoMusicBoardItemRules),
        .heals = sBattleBingoMusicBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoMusicBoardHealRules),
        .wilds = sBattleBingoMusicBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoMusicBoardWildRules),
        .bosses = sBattleBingoMusicBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoMusicBoardBosses),
        .mysteryWildCount = 5,
        .mysteryItemCount = 4,
        .music = MUS_GET_LUCKY,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_1X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER,
    },
    [BATTLE_BINGO_BOARD_MONSTER] =
    {
        .title = sBattleBingoText_MonsterBoard,
        .clearedFlag = FLAG_CLEARED_MONSTER_BINGO_CARD,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_METAGROSS_MONSTER_STARTER_1,
            BINGO_MON_TYRANITAR_MONSTER_STARTER_1,
            BINGO_MON_SALAMENCE_MONSTER_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 50,
        .items = sBattleBingoMonsterBoardItemRules,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoMonsterBoardItemRules),
        .heals = sBattleBingoMonsterBoardHealRules,
        .healRuleCount = ARRAY_COUNT(sBattleBingoMonsterBoardHealRules),
        .wilds = sBattleBingoMonsterBoardWildRules,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoMonsterBoardWildRules),
        .bosses = sBattleBingoMonsterBoardBosses,
        .bossCount = ARRAY_COUNT(sBattleBingoMonsterBoardBosses),
        .mysteryWildCount = 0,
        .mysteryItemCount = 0,
        .music = MUS_LIVE_AND_LEARN,
        .prizeMultiplierQ8_8 = BATTLE_BINGO_PRIZE_MULTIPLIER_3X,
        .wildPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
        .bossPokemonAi = AI_FLAG_BASIC_TRAINER
            | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_SMART_TERA
            | AI_FLAG_RANDOMIZE_SWITCHIN
            | AI_FLAG_PREDICTION
            | AI_FLAG_ASSUMPTIONS,
    },
};
