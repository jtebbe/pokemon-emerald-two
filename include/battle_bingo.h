#ifndef GUARD_BATTLE_BINGO_H
#define GUARD_BATTLE_BINGO_H

#include "constants/items.h"
#include "constants/bingo_mons.h"
#include "constants/opponents.h"
#include "constants/species.h"
#include "trainer_pools.h"

#define BATTLE_BINGO_BOARD_SIZE 5
#define BATTLE_BINGO_NUM_SQUARES (BATTLE_BINGO_BOARD_SIZE * BATTLE_BINGO_BOARD_SIZE)
#define BATTLE_BINGO_MAX_PARTY_SIZE 4
#define BATTLE_BINGO_MAX_STARTERS 3
#define BATTLE_BINGO_MAX_BOSSES 5
#define BATTLE_BINGO_MAX_LINES (BATTLE_BINGO_BOARD_SIZE * 2 + 2)
#define BATTLE_BINGO_PRIZE_MULTIPLIER_1X 256

enum BattleBingoBoardId
{
    BATTLE_BINGO_BOARD_FWG,
    BATTLE_BINGO_BOARD_COUNT,
};

enum BattleBingoStarterMode
{
    BATTLE_BINGO_STARTER_FIXED,
    BATTLE_BINGO_STARTER_CHOICE,
    BATTLE_BINGO_STARTER_RANDOM,
};

enum BattleBingoSquareKind
{
    BATTLE_BINGO_SQUARE_WILD,
    BATTLE_BINGO_SQUARE_ITEM,
    BATTLE_BINGO_SQUARE_BOSS,
    BATTLE_BINGO_SQUARE_MYSTERY,
};

enum BattleBingoResult
{
    BATTLE_BINGO_RESULT_LOSS,
    BATTLE_BINGO_RESULT_PRIZE,
    BATTLE_BINGO_RESULT_COMPLETE,
};

struct BattleBingoItemRule
{
    u16 item;
    u8 count;
};

struct BattleBingoWildRule
{
    u64 requiredTags;
    u64 forbiddenTags;
    u8 count;
    u8 type;
};

struct BattleBingoBoardRules
{
    const u8 *title;
    enum BattleBingoStarterMode starterMode;
    u16 starters[BATTLE_BINGO_MAX_STARTERS];
    u8 starterCount;
    u8 starterLevel;
    const struct BattleBingoItemRule *items;
    u8 itemRuleCount;
    const struct BattleBingoWildRule *wilds;
    u8 wildRuleCount;
    const u16 *bosses;
    u8 bossCount;
    u8 mysteryWildCount;
    u8 mysteryItemCount;
    u16 music;
    u16 prizeMultiplierQ8_8;
    u64 wildPokemonAi;
    u64 bossPokemonAi;
    bool8 startWithoutBall;
};

const struct BattleBingoBoardRules *GetBattleBingoBoardRules(enum BattleBingoBoardId boardId);
u8 BattleBingoCountBoardRuleSquares(const struct BattleBingoBoardRules *rules);
u8 BattleBingoCountPossibleLines(void);
u32 BattleBingoGetPrizeMoney(u8 bingoCount, u16 multiplierQ8_8);
bool32 BattleBingoBoardRulesAreValid(const struct BattleBingoBoardRules *rules);
u64 BattleBingoGetWildPokemonAiFlags(void);
u64 BattleBingoGetBossPokemonAiFlags(void);
u16 BattleBingoGetLastResult(void);
u16 BattleBingoGetLastPrizeMoney(void);

#endif // GUARD_BATTLE_BINGO_H
