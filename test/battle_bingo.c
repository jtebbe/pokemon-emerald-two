#include "global.h"
#include "battle_transition.h"
#include "battle_bingo.h"
#include "bingo_mons.h"
#include "data.h"
#include "constants/battle_ai.h"
#include "constants/species.h"
#include "test/test.h"

static const struct Trainer sBattleBingoTestTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT] =
{
    #include "../src/data/trainers.h"
};

static u32 CountBingoMonsWithTag(u64 tag)
{
    u32 count = 0;
    u32 i;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if (gBingoMons[i].tags & tag)
            count++;
    }

    return count;
}

static u32 BingoSquareMask(u8 row, u8 col)
{
    return 1 << (row * BATTLE_BINGO_BOARD_SIZE + col);
}

TEST("Battle Bingo has 12 possible 5x5 bingo lines")
{
    EXPECT_EQ(BattleBingoCountPossibleLines(), 12);
}

TEST("Battle Bingo prize money follows the fixed payout table")
{
    EXPECT_EQ(BattleBingoGetPrizeMoney(0, BATTLE_BINGO_PRIZE_MULTIPLIER_1X), 0);
    EXPECT_EQ(BattleBingoGetPrizeMoney(1, BATTLE_BINGO_PRIZE_MULTIPLIER_1X), 1000);
    EXPECT_EQ(BattleBingoGetPrizeMoney(5, BATTLE_BINGO_PRIZE_MULTIPLIER_1X), 6000);
    EXPECT_EQ(BattleBingoGetPrizeMoney(12, BATTLE_BINGO_PRIZE_MULTIPLIER_1X), 24000);
    EXPECT_EQ(BattleBingoGetPrizeMoney(99, BATTLE_BINGO_PRIZE_MULTIPLIER_1X), 24000);
}

TEST("Battle Bingo prize money supports board multipliers")
{
    EXPECT_EQ(BattleBingoGetPrizeMoney(2, BATTLE_BINGO_PRIZE_MULTIPLIER_1X / 2), 1000);
    EXPECT_EQ(BattleBingoGetPrizeMoney(2, BATTLE_BINGO_PRIZE_MULTIPLIER_1X * 2), 4000);
}

TEST("Battle Bingo can start on any square when none are cleared")
{
    EXPECT(BattleBingoSquareIsReachable(0, 0, 0));
    EXPECT(BattleBingoSquareIsReachable(0, 2, 2));
    EXPECT(BattleBingoSquareIsReachable(0, 4, 4));
}

TEST("Battle Bingo only unlocks orthogonally adjacent squares after the first clear")
{
    u32 clearedMask = BingoSquareMask(2, 2);

    EXPECT(BattleBingoSquareIsReachable(clearedMask, 1, 2));
    EXPECT(BattleBingoSquareIsReachable(clearedMask, 2, 1));
    EXPECT(BattleBingoSquareIsReachable(clearedMask, 2, 3));
    EXPECT(BattleBingoSquareIsReachable(clearedMask, 3, 2));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 1, 1));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 1, 3));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 3, 1));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 3, 3));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 4, 4));
}

TEST("Battle Bingo reachability respects board edges")
{
    u32 clearedMask = BingoSquareMask(0, 0);

    EXPECT(BattleBingoSquareIsReachable(clearedMask, 0, 1));
    EXPECT(BattleBingoSquareIsReachable(clearedMask, 1, 0));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 1, 1));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 4, 4));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, BATTLE_BINGO_BOARD_SIZE, 0));
    EXPECT(!BattleBingoSquareIsReachable(clearedMask, 0, BATTLE_BINGO_BOARD_SIZE));
}

TEST("Battle Bingo Fire Board has valid square counts and starter choice")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_FWG);
    u32 itemCount = 0;
    u32 healCount = 0;
    u32 wildCount = 0;
    u32 i;

    EXPECT(rules != NULL);
    EXPECT(BattleBingoBoardRulesAreValid(rules));
    EXPECT_EQ(BattleBingoCountBoardRuleSquares(rules), BATTLE_BINGO_NUM_SQUARES);
    for (i = 0; i < rules->itemRuleCount; i++)
        itemCount += rules->items[i].count;
    for (i = 0; i < rules->healRuleCount; i++)
        healCount += rules->heals[i].count;
    for (i = 0; i < rules->wildRuleCount; i++)
        wildCount += rules->wilds[i].count;
    EXPECT_EQ(wildCount, 15);
    EXPECT_EQ(rules->mysteryWildCount, 4);
    EXPECT_EQ(itemCount, 8);
    EXPECT_EQ(healCount, 1);
    EXPECT_EQ(rules->mysteryItemCount, 6);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_LANCE);
    EXPECT_EQ(rules->wildPokemonAi, AI_FLAG_BASIC_TRAINER);
    EXPECT_EQ(rules->bossPokemonAi, AI_FLAG_BASIC_TRAINER
        | AI_FLAG_SMART_SWITCHING
        | AI_FLAG_SMART_MON_CHOICES
        | AI_FLAG_PP_STALL_PREVENTION
        | AI_FLAG_SMART_TERA
        | AI_FLAG_RANDOMIZE_SWITCHIN
        | AI_FLAG_PREDICTION
        | AI_FLAG_ASSUMPTIONS);
    EXPECT(!(rules->bossPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_CHOICE);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 3);
    EXPECT(!rules->startWithoutBall);
    EXPECT_EQ(rules->starters[0], BINGO_MON_CHARIZARD_KANTO_STARTER_1);
    EXPECT_EQ(rules->starters[1], BINGO_MON_BLASTOISE_KANTO_STARTER_1);
    EXPECT_EQ(rules->starters[2], BINGO_MON_VENUSAUR_KANTO_STARTER_1);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_CHARIZARD);
    EXPECT_EQ(gBingoMons[rules->starters[1]].species, SPECIES_BLASTOISE);
    EXPECT_EQ(gBingoMons[rules->starters[2]].species, SPECIES_VENUSAUR);
}

TEST("Battle Bingo Fire Board splits wild encounters between Fire Water and Grass")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_FWG);

    EXPECT(rules != NULL);
    EXPECT_EQ(rules->wildRuleCount, 3);
    EXPECT_EQ(rules->wilds[0].count, 5);
    EXPECT(rules->wilds[0].requiredTags & MON_POOL_TAG_FIRE);
    EXPECT_EQ(rules->wilds[1].count, 5);
    EXPECT(rules->wilds[1].requiredTags & MON_POOL_TAG_WATER);
    EXPECT_EQ(rules->wilds[2].count, 5);
    EXPECT(rules->wilds[2].requiredTags & MON_POOL_TAG_GRASS);
}

TEST("Battle Bingo Fire Board has diverse Water and Grass encounter pools")
{
    EXPECT(CountBingoMonsWithTag(MON_POOL_TAG_WATER) >= 15);
    EXPECT(CountBingoMonsWithTag(MON_POOL_TAG_GRASS) >= 15);
}

TEST("Battle Bingo Fire Board boss is Lance with grass fire water trio")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_LANCE];

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->partySize, 3);
    EXPECT_EQ(trainer->party[0].species, SPECIES_SERPERIOR);
    EXPECT_EQ(trainer->party[1].species, SPECIES_CHARIZARD);
    EXPECT_EQ(trainer->party[2].species, SPECIES_FERALIGATR);
}
