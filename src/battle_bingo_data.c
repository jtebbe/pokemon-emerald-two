#include "global.h"
#include "battle_bingo.h"
#include "constants/battle_ai.h"
#include "constants/pokemon.h"
#include "constants/songs.h"

static const u8 BAsBattleBingoText_FWGBoard[] = _("FWG Board");

static const struct BattleBingoItemRule sBattleBingoFWGBoardItems[] =
{
    {ITEM_MASTER_BALL, 2},
    {ITEM_MAX_ELIXIR, 1},
    {ITEM_FULL_RESTORE, 1},
    {ITEM_REVIVE, 1},
    {ITEM_SUPER_POTION, 2},
    {ITEM_FOCUS_SASH, 1},
    {ITEM_CHARTI_BERRY, 1},
};

static const struct BattleBingoWildRule sBattleBingoFWGBoardWilds[] =
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

static const struct BattleBingoBoardRules sBattleBingoBoardRules[BATTLE_BINGO_BOARD_COUNT] =
{
    [BATTLE_BINGO_BOARD_FWG] =
    {
        .title = BAsBattleBingoText_FWGBoard,
        .starterMode = BATTLE_BINGO_STARTER_CHOICE,
        .starters = {
            BINGO_MON_CHARIZARD_KANTO_STARTER_1,
            BINGO_MON_BLASTOISE_KANTO_STARTER_1,
            BINGO_MON_VENUSAUR_KANTO_STARTER_1,
        },
        .starterCount = 3,
        .starterLevel = 50,
        .items = sBattleBingoFWGBoardItems,
        .itemRuleCount = ARRAY_COUNT(sBattleBingoFWGBoardItems),
        .wilds = sBattleBingoFWGBoardWilds,
        .wildRuleCount = ARRAY_COUNT(sBattleBingoFWGBoardWilds),
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
};

static const u16 sBattleBingoPrizeMoney[BATTLE_BINGO_MAX_LINES] =
{
    1000,
    2000,
    3000,
    4000,
    6000,
    8000,
    10000,
    12000,
    15000,
    18000,
    21000,
    24000,
};

const struct BattleBingoBoardRules *GetBattleBingoBoardRules(enum BattleBingoBoardId boardId)
{
    if (boardId >= BATTLE_BINGO_BOARD_COUNT)
        return NULL;

    return &sBattleBingoBoardRules[boardId];
}

u8 BattleBingoCountBoardRuleSquares(const struct BattleBingoBoardRules *rules)
{
    u32 i;
    u8 count = 0;

    if (rules == NULL)
        return 0;

    for (i = 0; i < rules->itemRuleCount; i++)
        count += rules->items[i].count;

    for (i = 0; i < rules->wildRuleCount; i++)
        count += rules->wilds[i].count;

    count += rules->bossCount;
    return count;
}

u8 BattleBingoCountPossibleLines(void)
{
    return BATTLE_BINGO_MAX_LINES;
}

u32 BattleBingoGetPrizeMoney(u8 bingoCount, u16 multiplierQ8_8)
{
    u32 prize;

    if (bingoCount == 0)
        return 0;

    if (bingoCount > BATTLE_BINGO_MAX_LINES)
        bingoCount = BATTLE_BINGO_MAX_LINES;

    prize = sBattleBingoPrizeMoney[bingoCount - 1];
    return (prize * multiplierQ8_8) / BATTLE_BINGO_PRIZE_MULTIPLIER_1X;
}

bool32 BattleBingoSquareIsReachable(u32 clearedMask, u8 row, u8 col)
{
    if (row >= BATTLE_BINGO_BOARD_SIZE || col >= BATTLE_BINGO_BOARD_SIZE)
        return FALSE;
    if (clearedMask == 0)
        return TRUE;
    if (row > 0 && (clearedMask & (1 << ((row - 1) * BATTLE_BINGO_BOARD_SIZE + col))))
        return TRUE;
    if (row + 1 < BATTLE_BINGO_BOARD_SIZE && (clearedMask & (1 << ((row + 1) * BATTLE_BINGO_BOARD_SIZE + col))))
        return TRUE;
    if (col > 0 && (clearedMask & (1 << (row * BATTLE_BINGO_BOARD_SIZE + col - 1))))
        return TRUE;
    if (col + 1 < BATTLE_BINGO_BOARD_SIZE && (clearedMask & (1 << (row * BATTLE_BINGO_BOARD_SIZE + col + 1))))
        return TRUE;

    return FALSE;
}

bool32 BattleBingoBoardRulesAreValid(const struct BattleBingoBoardRules *rules)
{
    u32 i;
    u8 itemCount = 0;
    u8 wildCount = 0;

    if (rules == NULL)
        return FALSE;

    if (rules->starterCount == 0 || rules->starterCount > BATTLE_BINGO_MAX_STARTERS)
        return FALSE;

    for (i = 0; i < rules->starterCount; i++)
    {
        if (rules->starters[i] >= BINGO_MON_COUNT)
            return FALSE;
    }

    if (rules->bossCount == 0 || rules->bossCount > BATTLE_BINGO_MAX_BOSSES)
        return FALSE;

    for (i = 0; i < rules->itemRuleCount; i++)
        itemCount += rules->items[i].count;

    for (i = 0; i < rules->wildRuleCount; i++)
        wildCount += rules->wilds[i].count;

    if (BattleBingoCountBoardRuleSquares(rules) != BATTLE_BINGO_NUM_SQUARES)
        return FALSE;

    if (rules->mysteryWildCount > wildCount)
        return FALSE;

    if (rules->mysteryItemCount > itemCount)
        return FALSE;

    if (rules->mysteryWildCount + rules->mysteryItemCount > BATTLE_BINGO_NUM_SQUARES - rules->bossCount)
        return FALSE;

    return TRUE;
}
