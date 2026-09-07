#include "global.h"
#include "battle_bingo.h"

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

static const u8 sBattleBingoBossCornerPositions[BATTLE_BINGO_BOSS_CORNER_COUNT] =
{
    0, 4,
    20, 24,
};

const struct BattleBingoBoardRules *GetBattleBingoBoardRules(enum BattleBingoBoardId boardId)
{
    if (boardId >= BATTLE_BINGO_BOARD_COUNT)
        return NULL;

    return &gBattleBingoBoardRules[boardId];
}

u8 BattleBingoCountBoardRuleSquares(const struct BattleBingoBoardRules *rules)
{
    u32 i;
    u8 count = 0;

    if (rules == NULL)
        return 0;

    for (i = 0; i < rules->itemRuleCount; i++)
        count += rules->items[i].count;

    for (i = 0; i < rules->healRuleCount; i++)
        count += rules->heals[i].count;

    for (i = 0; i < rules->wildRuleCount; i++)
        count += rules->wilds[i].count;

    count += rules->bossCount;
    return count;
}

u8 BattleBingoCountPossibleLines(void)
{
    return BATTLE_BINGO_MAX_LINES;
}

u8 BattleBingoGetBossCornerPosition(u8 cornerIndex)
{
    if (cornerIndex >= ARRAY_COUNT(sBattleBingoBossCornerPositions))
        return 0xFF;

    return sBattleBingoBossCornerPositions[cornerIndex];
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
    u8 healCount = 0;
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

    for (i = 0; i < rules->healRuleCount; i++)
        healCount += rules->heals[i].count;

    for (i = 0; i < rules->wildRuleCount; i++)
        wildCount += rules->wilds[i].count;

    if (BattleBingoCountBoardRuleSquares(rules) != BATTLE_BINGO_NUM_SQUARES)
        return FALSE;

    if (rules->mysteryWildCount > wildCount)
        return FALSE;

    if (rules->mysteryItemCount > itemCount)
        return FALSE;

    if (rules->mysteryWildCount + rules->mysteryItemCount > BATTLE_BINGO_NUM_SQUARES - rules->bossCount - healCount)
        return FALSE;

    return TRUE;
}
