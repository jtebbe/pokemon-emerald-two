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
#define BATTLE_BINGO_BOSS_CORNER_COUNT 4
#define BATTLE_BINGO_MAX_LINES (BATTLE_BINGO_BOARD_SIZE * 2 + 2)
#define BATTLE_BINGO_PRIZE_MULTIPLIER_1X 256
#define BATTLE_BINGO_PRIZE_MULTIPLIER_2X (BATTLE_BINGO_PRIZE_MULTIPLIER_1X * 2)
#define BATTLE_BINGO_PRIZE_MULTIPLIER_3X (BATTLE_BINGO_PRIZE_MULTIPLIER_1X * 3)
#define BATTLE_BINGO_PRIZE_MULTIPLIER_10X (BATTLE_BINGO_PRIZE_MULTIPLIER_1X * 10)

enum BattleBingoBoardId
{
    BATTLE_BINGO_BOARD_FWG,
    BATTLE_BINGO_BOARD_NORMAL,
    BATTLE_BINGO_BOARD_LC,
    BATTLE_BINGO_BOARD_CIPHER,
    BATTLE_BINGO_BOARD_LEGENDARY,
    BATTLE_BINGO_BOARD_DRAGON,
    BATTLE_BINGO_BOARD_STRANGE,
    BATTLE_BINGO_BOARD_MUSIC,
    BATTLE_BINGO_BOARD_MONSTER,
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
    BATTLE_BINGO_SQUARE_HEAL,
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

struct BattleBingoHealRule
{
    u8 count;
};

struct BattleBingoBoardRules
{
    const u8 *title;
    u16 clearedFlag;
    enum BattleBingoStarterMode starterMode;
    u16 starters[BATTLE_BINGO_MAX_STARTERS];
    u8 starterCount;
    u8 starterLevel;
    const struct BattleBingoItemRule *items;
    u8 itemRuleCount;
    const struct BattleBingoHealRule *heals;
    u8 healRuleCount;
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

extern const struct BattleBingoBoardRules gBattleBingoBoardRules[BATTLE_BINGO_BOARD_COUNT];

const struct BattleBingoBoardRules *GetBattleBingoBoardRules(enum BattleBingoBoardId boardId);
u8 BattleBingoCountBoardRuleSquares(const struct BattleBingoBoardRules *rules);
u8 BattleBingoCountPossibleLines(void);
u8 BattleBingoGetBossCornerPosition(u8 cornerIndex);
u32 BattleBingoGetPrizeMoney(u8 bingoCount, u16 multiplierQ8_8);
bool32 BattleBingoBoardRulesAreValid(const struct BattleBingoBoardRules *rules);
bool32 BattleBingoSquareIsReachable(u32 clearedMask, u8 row, u8 col);
u64 BattleBingoGetWildPokemonAiFlags(void);
u64 BattleBingoGetBossPokemonAiFlags(void);
u16 BattleBingoGetLastResult(void);
u16 BattleBingoGetLastPrizeMoney(void);
u16 BattleBingoCountClearedBoards(void);
u16 BattleBingoShouldAwardCipherCard(void);
u16 BattleBingoDidFirstClearCipherBoard(void);
u16 BattleBingoGetSelectedBoardId(void);
u16 BattleBingoPrepareCardSelection(void);
void StartSelectedBattleBingoBoard(void);
void BattleBingoSetSelectedBoard(void);
void ShowBattleBingoBoardFromBlack(void);
bool32 BattleBingoIsCheatItemPurchased(enum Item itemId);
void BattleBingoSetCheatItemPurchased(enum Item itemId);
u32 BattleBingoGetCheatItemPrice(enum Item itemId);

#endif // GUARD_BATTLE_BINGO_H
