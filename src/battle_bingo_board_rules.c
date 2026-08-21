#include "global.h"
#include "battle_bingo.h"
#include "constants/battle_ai.h"
#include "constants/pokemon.h"
#include "constants/songs.h"

static const u8 sBattleBingoText_FWGBoard[] = _("FWG Board");
static const u8 sBattleBingoText_NormalBoard[] = _("Normal Board");

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

const struct BattleBingoBoardRules gBattleBingoBoardRules[BATTLE_BINGO_BOARD_COUNT] =
{
    [BATTLE_BINGO_BOARD_FWG] =
    {
        .title = sBattleBingoText_FWGBoard,
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
    [BATTLE_BINGO_BOARD_NORMAL] =
    {
        .title = sBattleBingoText_NormalBoard,
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
};
