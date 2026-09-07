#include "global.h"
#include "battle_transition.h"
#include "battle_bingo.h"
#include "bingo_mons.h"
#include "data.h"
#include "event_data.h"
#include "move.h"
#include "constants/battle_ai.h"
#include "constants/abilities.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/songs.h"
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

static u32 CountBingoMonsMatchingTags(u64 requiredTags, u64 forbiddenTags)
{
    u32 count = 0;
    u32 i;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if ((gBingoMons[i].tags & requiredTags) == requiredTags && !(gBingoMons[i].tags & forbiddenTags))
            count++;
    }

    return count;
}

static bool32 BingoMonHasMoveType(u16 bingoMonId, enum Type type)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 move = gBingoMons[bingoMonId].moves[i];

        if (move != MOVE_NONE && GetMoveType(move) == type)
            return TRUE;
    }

    return FALSE;
}

static bool32 BingoMonHasMove(u16 bingoMonId, enum Move move)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBingoMons[bingoMonId].moves[i] == move)
            return TRUE;
    }

    return FALSE;
}

static bool32 TrainerMonHasMoveType(const struct TrainerMon *mon, enum Type type)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        u16 move = mon->moves[i];

        if (move != MOVE_NONE && GetMoveType(move) == type)
            return TRUE;
    }

    return FALSE;
}

static bool32 TrainerMonHasMove(const struct TrainerMon *mon, enum Move move)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (mon->moves[i] == move)
            return TRUE;
    }

    return FALSE;
}

static bool32 IsBingoSoundMove(enum Move move)
{
    switch (move)
    {
    case MOVE_ALLURING_VOICE:
    case MOVE_BOOMBURST:
    case MOVE_BUG_BUZZ:
    case MOVE_CHATTER:
    case MOVE_CLANGING_SCALES:
    case MOVE_CLANGOROUS_SOUL:
    case MOVE_DISARMING_VOICE:
    case MOVE_ECHOED_VOICE:
    case MOVE_HYPER_VOICE:
    case MOVE_OVERDRIVE:
    case MOVE_PARTING_SHOT:
    case MOVE_PSYCHIC_NOISE:
    case MOVE_ROUND:
    case MOVE_SNARL:
    case MOVE_SPARKLING_ARIA:
    case MOVE_TORCH_SONG:
    case MOVE_UPROAR:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 BingoMonHasSoundMove(u16 bingoMonId)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (IsBingoSoundMove(gBingoMons[bingoMonId].moves[i]))
            return TRUE;
    }

    return FALSE;
}

static u32 BingoSquareMask(u8 row, u8 col)
{
    return 1 << (row * BATTLE_BINGO_BOARD_SIZE + col);
}

TEST("Battle Bingo has 12 possible 5x5 bingo lines")
{
    EXPECT_EQ(BattleBingoCountPossibleLines(), 12);
}

TEST("Battle Bingo counts cleared boards through board rule flags")
{
    const struct BattleBingoBoardRules *rules;
    u32 i;

    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
    {
        rules = GetBattleBingoBoardRules(i);
        EXPECT(rules != NULL);
        EXPECT_NE(rules->clearedFlag, 0);
        FlagClear(rules->clearedFlag);
    }

    EXPECT_EQ(BattleBingoCountClearedBoards(), 0);

    FlagSet(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_FWG)->clearedFlag);
    EXPECT_EQ(BattleBingoCountClearedBoards(), 1);

    FlagSet(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_LC)->clearedFlag);
    FlagSet(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_DRAGON)->clearedFlag);
    EXPECT_EQ(BattleBingoCountClearedBoards(), 3);

    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
        FlagClear(GetBattleBingoBoardRules(i)->clearedFlag);
}

TEST("Battle Bingo unlocks the Cipher card after every other board is cleared")
{
    u32 i;

    FlagClear(FLAG_RECEIVED_CIPHER_BINGO_CARD_ATTENDANT);
    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
        FlagClear(GetBattleBingoBoardRules(i)->clearedFlag);

    EXPECT(!BattleBingoShouldAwardCipherCard());

    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
    {
        if (i != BATTLE_BINGO_BOARD_CIPHER)
            FlagSet(GetBattleBingoBoardRules(i)->clearedFlag);
    }
    EXPECT(BattleBingoShouldAwardCipherCard());

    FlagSet(FLAG_RECEIVED_CIPHER_BINGO_CARD_ATTENDANT);
    EXPECT(!BattleBingoShouldAwardCipherCard());
    FlagClear(FLAG_RECEIVED_CIPHER_BINGO_CARD_ATTENDANT);

    FlagSet(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_CIPHER)->clearedFlag);
    EXPECT(!BattleBingoShouldAwardCipherCard());

    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
        FlagClear(GetBattleBingoBoardRules(i)->clearedFlag);
}

TEST("Battle Bingo extra bosses are constrained to corner squares")
{
    EXPECT_EQ(BattleBingoGetBossCornerPosition(0), 0);
    EXPECT_EQ(BattleBingoGetBossCornerPosition(1), 4);
    EXPECT_EQ(BattleBingoGetBossCornerPosition(2), 20);
    EXPECT_EQ(BattleBingoGetBossCornerPosition(3), 24);
    EXPECT_EQ(BattleBingoGetBossCornerPosition(4), 0xFF);
}

TEST("Battle Bingo board bosses all have mugshots")
{
    u32 board, boss;

    for (board = 0; board < BATTLE_BINGO_BOARD_COUNT; board++)
    {
        const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(board);

        EXPECT(rules != NULL);
        for (boss = 0; boss < rules->bossCount; boss++)
        {
            const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][rules->bosses[boss]];

            EXPECT_NE((u32)trainer->mugshotColor, MUGSHOT_COLOR_NONE);
            EXPECT_LT((u32)trainer->mugshotColor, MUGSHOT_COLOR_COUNT);
        }
    }
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

TEST("Battle Bingo special boards use requested prize multipliers")
{
    EXPECT_EQ(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_LC)->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_2X);
    EXPECT_EQ(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_LEGENDARY)->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_3X);
    EXPECT_EQ(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_DRAGON)->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_2X);
    EXPECT_EQ(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_STRANGE)->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_2X);
    EXPECT_EQ(GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_CIPHER)->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_10X);
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

TEST("Battle Bingo Fire Board boss presents as Professor Oak with grass fire water trio")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_LANCE];

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->trainerClass, TRAINER_CLASS_PKMN_PROF_FRLG);
    EXPECT_EQ((u32)trainer->trainerPic, TRAINER_PIC_PROFESSOR_OAK_FRLG);
    EXPECT_EQ((u32)trainer->partySize, 3);
    EXPECT_EQ(trainer->party[0].species, SPECIES_SERPERIOR);
    EXPECT_EQ(trainer->party[1].species, SPECIES_CHARIZARD);
    EXPECT_EQ(trainer->party[2].species, SPECIES_FERALIGATR);
}

TEST("Battle Bingo Normal Board has valid square counts and fixed Tauros starter")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_NORMAL);
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
    EXPECT_EQ(rules->mysteryWildCount, 5);
    EXPECT_EQ(itemCount, 8);
    EXPECT_EQ(healCount, 1);
    EXPECT_EQ(rules->mysteryItemCount, 5);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_NORMAN_3);
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_FIXED);
    EXPECT_EQ(rules->starterCount, 1);
    EXPECT_EQ(rules->starters[0], BINGO_MON_TAUROS_NORMAL_STARTER_1);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_TAUROS);
    EXPECT_EQ(gBingoMons[rules->starters[0]].heldItem, ITEM_CHOPLE_BERRY);
}

TEST("Battle Bingo Normal Board uses Normal encounter tags and has enough unique mons")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_NORMAL);

    EXPECT(rules != NULL);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].count, 15);
    EXPECT(rules->wilds[0].requiredTags & MON_POOL_TAG_NORMAL);
    EXPECT_EQ(rules->wilds[0].type, TYPE_NORMAL);
    EXPECT(CountBingoMonsWithTag(MON_POOL_TAG_NORMAL) >= 16);
}

TEST("Battle Bingo Normal Board boss is Norman with level 50 singles team")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_NORMAN_3];
    u32 i, j;

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_SINGLES);
    EXPECT_EQ((u32)trainer->partySize, 3);
    EXPECT_EQ(trainer->party[0].species, SPECIES_SNORLAX);
    EXPECT_EQ(trainer->party[0].lvl, 50);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_LEFTOVERS);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_THICK_FAT);
    EXPECT_EQ(trainer->party[0].moves[2], MOVE_SLEEP_TALK);
    EXPECT_EQ(trainer->party[1].species, SPECIES_EXPLOUD);
    EXPECT_EQ(trainer->party[1].lvl, 50);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_CHOICE_SPECS);
    EXPECT_EQ(trainer->party[1].ability, ABILITY_SCRAPPY);
    EXPECT_EQ(trainer->party[2].species, SPECIES_SLAKING);
    EXPECT_EQ(trainer->party[2].lvl, 50);
    EXPECT_EQ(trainer->party[2].heldItem, ITEM_CHOICE_BAND);
    EXPECT_EQ(trainer->party[2].ability, ABILITY_TRUANT);

    for (i = 0; i < trainer->partySize; i++)
        for (j = 0; j < MAX_MON_MOVES; j++)
            EXPECT_NE(trainer->party[i].moves[j], MOVE_PROTECT);
}

TEST("Battle Bingo Cipher Board has valid smart AI square counts and starters")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_CIPHER);
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

    EXPECT_EQ(wildCount, 10);
    EXPECT_EQ(rules->mysteryWildCount, 0);
    EXPECT_EQ(itemCount, 5);
    EXPECT_EQ(healCount, 5);
    EXPECT_EQ(rules->mysteryItemCount, 0);
    EXPECT_EQ(rules->bossCount, 5);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_NASCOUR);
    EXPECT_EQ(rules->bosses[1], TRAINER_BINGO_BOSS_DAKIM);
    EXPECT_EQ(rules->bosses[2], TRAINER_BINGO_BOSS_EIN);
    EXPECT_EQ(rules->bosses[3], TRAINER_BINGO_BOSS_MIROR_B);
    EXPECT_EQ(rules->bosses[4], TRAINER_BINGO_BOSS_VENUS);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_CIPHER);
    EXPECT_EQ(rules->wilds[0].type, TYPE_MYSTERY);
    EXPECT_EQ(CountBingoMonsWithTag(MON_POOL_TAG_CIPHER), 10);
    EXPECT(rules->wildPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(rules->bossPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(!(rules->wildPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT(!(rules->bossPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_CHOICE);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 2);
    EXPECT_EQ(rules->music, MUS_BIOLIZARD);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_ESPEON);
    EXPECT_EQ(gBingoMons[rules->starters[1]].species, SPECIES_UMBREON);
    EXPECT_EQ(gBingoMons[rules->starters[0]].tags, 0);
    EXPECT_EQ(gBingoMons[rules->starters[1]].tags, 0);
}

TEST("Battle Bingo Cipher Board boss teams are wired to the five Cipher bosses")
{
    const struct Trainer *nascour = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_NASCOUR];
    const struct Trainer *dakim = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_DAKIM];
    const struct Trainer *ein = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_EIN];
    const struct Trainer *mirorB = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_MIROR_B];
    const struct Trainer *venus = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_VENUS];

    EXPECT_EQ((u32)nascour->partySize, 3);
    EXPECT_EQ(nascour->party[0].species, SPECIES_METAGROSS);
    EXPECT_EQ(nascour->party[0].heldItem, ITEM_WEAKNESS_POLICY);
    EXPECT_EQ(nascour->party[1].species, SPECIES_DUSCLOPS);
    EXPECT_EQ(nascour->party[1].heldItem, ITEM_EVIOLITE);
    EXPECT_EQ(nascour->party[2].species, SPECIES_BLAZIKEN);
    EXPECT_EQ(nascour->party[2].moves[0], MOVE_PROTECT);

    EXPECT_EQ((u32)dakim->partySize, 3);
    EXPECT_EQ(dakim->party[0].species, SPECIES_FORRETRESS);
    EXPECT_EQ(dakim->party[1].species, SPECIES_ENTEI);
    EXPECT_EQ(dakim->party[1].heldItem, ITEM_CHOICE_BAND);
    EXPECT_EQ(dakim->party[2].species, SPECIES_SWAMPERT);
    EXPECT_EQ(dakim->party[2].heldItem, ITEM_ASSAULT_VEST);

    EXPECT_EQ((u32)ein->partySize, 3);
    EXPECT_EQ(ein->party[0].species, SPECIES_CROBAT);
    EXPECT_EQ(ein->party[0].heldItem, ITEM_CHOICE_BAND);
    EXPECT_EQ(ein->party[1].species, SPECIES_PELIPPER);
    EXPECT_EQ(ein->party[1].heldItem, ITEM_CHOICE_SPECS);
    EXPECT_EQ(ein->party[2].species, SPECIES_RAIKOU);
    EXPECT_EQ(ein->party[2].moves[0], MOVE_CALM_MIND);

    EXPECT_EQ((u32)mirorB->partySize, 3);
    EXPECT_EQ(mirorB->party[0].species, SPECIES_POLITOED);
    EXPECT_EQ(mirorB->party[0].heldItem, ITEM_CHOICE_SPECS);
    EXPECT_EQ(mirorB->party[1].species, SPECIES_LUDICOLO);
    EXPECT_EQ(mirorB->party[1].heldItem, ITEM_LIFE_ORB);
    EXPECT_EQ(mirorB->party[2].species, SPECIES_SUDOWOODO);
    EXPECT_EQ(mirorB->party[2].heldItem, ITEM_CHOICE_BAND);

    EXPECT_EQ((u32)venus->partySize, 3);
    EXPECT_EQ(venus->party[0].species, SPECIES_STEELIX);
    EXPECT_EQ(venus->party[0].heldItem, ITEM_ASSAULT_VEST);
    EXPECT_EQ(venus->party[1].species, SPECIES_BANETTE);
    EXPECT_EQ(venus->party[1].heldItem, ITEM_LIFE_ORB);
    EXPECT_EQ(venus->party[2].species, SPECIES_SUICUNE);
    EXPECT_EQ(venus->party[2].moves[0], MOVE_CALM_MIND);
}

TEST("Battle Bingo Legendary Board has valid smart AI square counts and starters")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_LEGENDARY);
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
    EXPECT_EQ(rules->mysteryWildCount, 3);
    EXPECT_EQ(itemCount, 7);
    EXPECT_EQ(healCount, 2);
    EXPECT_EQ(rules->mysteryItemCount, 3);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_ANABEL);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_LEGENDARY);
    EXPECT_EQ(rules->wilds[0].type, TYPE_MYSTERY);
    EXPECT_EQ(CountBingoMonsWithTag(MON_POOL_TAG_LEGENDARY), 15);
    EXPECT(rules->wildPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(rules->bossPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(!(rules->wildPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT(!(rules->bossPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_CHOICE);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 3);
    EXPECT_EQ(rules->music, MUS_CORRIDORS_OF_TIME);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_GROUDON);
    EXPECT_EQ(gBingoMons[rules->starters[1]].species, SPECIES_KYOGRE);
    EXPECT_EQ(gBingoMons[rules->starters[2]].species, SPECIES_RAYQUAZA);
    EXPECT_EQ(gBingoMons[rules->starters[0]].tags, 0);
    EXPECT_EQ(gBingoMons[rules->starters[1]].tags, 0);
    EXPECT_EQ(gBingoMons[rules->starters[2]].tags, 0);
}

TEST("Battle Bingo Legendary Board wild encounters have no items protect or rest")
{
    static const u16 expectedSpecies[] =
    {
        SPECIES_ARCEUS,
        SPECIES_CALYREX_ICE,
        SPECIES_DEOXYS_ATTACK,
        SPECIES_DIALGA,
        SPECIES_PALKIA,
        SPECIES_GIRATINA,
        SPECIES_LUGIA,
        SPECIES_HO_OH,
        SPECIES_KORAIDON,
        SPECIES_MIRAIDON,
        SPECIES_NECROZMA_DAWN_WINGS,
        SPECIES_KYUREM_WHITE,
        SPECIES_ZEKROM,
        SPECIES_XERNEAS,
        SPECIES_YVELTAL,
    };
    u32 expected, i, j;

    for (expected = 0; expected < ARRAY_COUNT(expectedSpecies); expected++)
    {
        bool32 found = FALSE;

        for (i = 0; i < BINGO_MON_COUNT; i++)
        {
            if (gBingoMons[i].species != expectedSpecies[expected])
                continue;
            if (!(gBingoMons[i].tags & MON_POOL_TAG_LEGENDARY))
                continue;

            found = TRUE;
            EXPECT_EQ(gBingoMons[i].heldItem, ITEM_NONE);
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                EXPECT_NE(gBingoMons[i].moves[j], MOVE_PROTECT);
                EXPECT_NE(gBingoMons[i].moves[j], MOVE_REST);
            }
            break;
        }

        EXPECT(found);
    }
}

TEST("Battle Bingo Legendary Board boss is Anabel with requested legendary team")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_ANABEL];
    u32 i, j;

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_SINGLES);
    EXPECT_EQ((u32)trainer->partySize, 3);

    EXPECT_EQ(trainer->party[0].species, SPECIES_MEWTWO);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_FOCUS_SASH);
    EXPECT_EQ(trainer->party[0].moves[0], MOVE_NASTY_PLOT);

    EXPECT_EQ(trainer->party[1].species, SPECIES_ZACIAN_CROWNED);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_RUSTED_SWORD);
    EXPECT_EQ(trainer->party[1].moves[0], MOVE_BEHEMOTH_BLADE);

    EXPECT_EQ(trainer->party[2].species, SPECIES_ETERNATUS);
    EXPECT_EQ(trainer->party[2].heldItem, ITEM_LIFE_ORB);
    EXPECT_EQ(trainer->party[2].moves[0], MOVE_DYNAMAX_CANNON);

    for (i = 0; i < trainer->partySize; i++)
        for (j = 0; j < MAX_MON_MOVES; j++)
            EXPECT_NE(trainer->party[i].moves[j], MOVE_REST);
}

TEST("Battle Bingo Dragon Board has valid smart AI square counts and fixed Dragonite starter")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_DRAGON);
    u64 forbiddenTags = MON_POOL_TAG_LEGENDARY
        | MON_POOL_TAG_SUBLEGENDARY
        | MON_POOL_TAG_MYTHICAL
        | MON_POOL_TAG_PARADOX
        | MON_POOL_TAG_ULTRA_BEAST;
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
    EXPECT_EQ(rules->mysteryWildCount, 5);
    EXPECT_EQ(itemCount, 6);
    EXPECT_EQ(healCount, 3);
    EXPECT_EQ(rules->mysteryItemCount, 3);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_DRAGON_LANCE);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_DRAGON);
    EXPECT_EQ(rules->wilds[0].forbiddenTags, forbiddenTags);
    EXPECT_EQ(rules->wilds[0].type, TYPE_DRAGON);
    EXPECT_EQ(CountBingoMonsMatchingTags(rules->wilds[0].requiredTags, rules->wilds[0].forbiddenTags), 15);
    EXPECT(rules->wildPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(rules->bossPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(!(rules->wildPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT(!(rules->bossPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_FIXED);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 1);
    EXPECT_EQ(rules->music, MUS_MOLGERA);
    EXPECT_EQ(rules->starters[0], BINGO_MON_DRAGONITE_DRAGON_STARTER_1);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_DRAGONITE);
    EXPECT_EQ(gBingoMons[rules->starters[0]].tags, 0);
}

TEST("Battle Bingo Dragon Board mons and boss use no Dragon type moves")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_DRAGON);
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_DRAGON_LANCE];
    u32 i;

    EXPECT(rules != NULL);
    EXPECT(!BingoMonHasMoveType(rules->starters[0], TYPE_DRAGON));

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if ((gBingoMons[i].tags & rules->wilds[0].requiredTags) != rules->wilds[0].requiredTags)
            continue;
        if (gBingoMons[i].tags & rules->wilds[0].forbiddenTags)
            continue;

        EXPECT_EQ(gBingoMons[i].heldItem, ITEM_NONE);
        EXPECT(!BingoMonHasMoveType(i, TYPE_DRAGON));
        EXPECT_NE(gBingoMons[i].moves[0], MOVE_PROTECT);
        EXPECT_NE(gBingoMons[i].moves[1], MOVE_PROTECT);
        EXPECT_NE(gBingoMons[i].moves[2], MOVE_PROTECT);
        EXPECT_NE(gBingoMons[i].moves[3], MOVE_PROTECT);
        EXPECT_NE(gBingoMons[i].moves[0], MOVE_REST);
        EXPECT_NE(gBingoMons[i].moves[1], MOVE_REST);
        EXPECT_NE(gBingoMons[i].moves[2], MOVE_REST);
        EXPECT_NE(gBingoMons[i].moves[3], MOVE_REST);
    }

    EXPECT_EQ((u32)trainer->partySize, 3);
    EXPECT_EQ(trainer->party[0].species, SPECIES_KINGDRA);
    EXPECT_EQ(trainer->party[1].species, SPECIES_DRAGAPULT);
    EXPECT_EQ(trainer->party[2].species, SPECIES_DRAGONITE);
    for (i = 0; i < trainer->partySize; i++)
        EXPECT(!TrainerMonHasMoveType(&trainer->party[i], TYPE_DRAGON));
}

TEST("Battle Bingo Strange Board has valid smart AI square counts and starters")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_STRANGE);
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

    EXPECT_EQ(wildCount, 14);
    EXPECT_EQ(rules->mysteryWildCount, 5);
    EXPECT_EQ(itemCount, 10);
    EXPECT_EQ(healCount, 0);
    EXPECT_EQ(rules->mysteryItemCount, 5);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_TUCKER);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_STRANGE);
    EXPECT_EQ(rules->wilds[0].type, TYPE_MYSTERY);
    EXPECT_EQ(CountBingoMonsWithTag(MON_POOL_TAG_STRANGE), 33);
    EXPECT(rules->wildPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(rules->bossPokemonAi & AI_FLAG_PREDICTION);
    EXPECT(!(rules->wildPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT(!(rules->bossPokemonAi & AI_FLAG_OMNISCIENT));
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_CHOICE);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 3);
    EXPECT_EQ(rules->music, MUS_RASPUTIN);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_ZAPDOS_GALAR);
    EXPECT_EQ(gBingoMons[rules->starters[1]].species, SPECIES_SANDY_SHOCKS);
    EXPECT_EQ(gBingoMons[rules->starters[2]].species, SPECIES_KARTANA);
    EXPECT_EQ(gBingoMons[rules->starters[0]].tags, 0);
    EXPECT_EQ(gBingoMons[rules->starters[1]].tags, 0);
    EXPECT_EQ(gBingoMons[rules->starters[2]].tags, 0);
}

TEST("Battle Bingo Strange Board wild encounters have no items protect or rest")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_STRANGE);
    u32 i, j;

    EXPECT(rules != NULL);
    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if (!(gBingoMons[i].tags & MON_POOL_TAG_STRANGE))
            continue;

        EXPECT_EQ(gBingoMons[i].heldItem, ITEM_NONE);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            EXPECT_NE(gBingoMons[i].moves[j], MOVE_PROTECT);
            EXPECT_NE(gBingoMons[i].moves[j], MOVE_REST);
        }
    }
}

TEST("Battle Bingo Strange Board boss is Tucker with requested strange team")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_TUCKER];
    u32 i, j;

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_SINGLES);
    EXPECT_EQ((u32)trainer->partySize, 3);

    EXPECT_EQ(trainer->party[0].species, SPECIES_ARTICUNO_GALAR);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_WEAKNESS_POLICY);
    EXPECT_EQ(trainer->party[0].moves[0], MOVE_AGILITY);

    EXPECT_EQ(trainer->party[1].species, SPECIES_WALKING_WAKE);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_BOOSTER_ENERGY);
    EXPECT_EQ(trainer->party[1].moves[0], MOVE_HYDRO_STEAM);

    EXPECT_EQ(trainer->party[2].species, SPECIES_STAKATAKA);
    EXPECT_EQ(trainer->party[2].moves[0], MOVE_TRICK_ROOM);
    EXPECT_EQ(trainer->party[2].moves[1], MOVE_GYRO_BALL);
    EXPECT_EQ(trainer->party[2].moves[2], MOVE_BODY_PRESS);

    for (i = 0; i < trainer->partySize; i++)
        for (j = 0; j < MAX_MON_MOVES; j++)
            EXPECT_NE(trainer->party[i].moves[j], MOVE_REST);
}

TEST("Battle Bingo Music Board has valid basic AI square counts and fixed Exploud starter")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_MUSIC);
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
    EXPECT_EQ(rules->mysteryWildCount, 5);
    EXPECT_EQ(itemCount, 7);
    EXPECT_EQ(healCount, 2);
    EXPECT_EQ(rules->mysteryItemCount, 4);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_EDDIE);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_MUSIC);
    EXPECT_EQ(rules->wilds[0].type, TYPE_MYSTERY);
    EXPECT_EQ(CountBingoMonsWithTag(MON_POOL_TAG_MUSIC), 23);
    EXPECT_EQ(rules->wildPokemonAi, AI_FLAG_BASIC_TRAINER);
    EXPECT_EQ(rules->bossPokemonAi, AI_FLAG_BASIC_TRAINER);
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_FIXED);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 1);
    EXPECT_EQ(rules->music, MUS_GET_LUCKY);
    EXPECT_EQ(rules->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_1X);
    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_EXPLOUD);
    EXPECT_EQ(gBingoMons[rules->starters[0]].heldItem, ITEM_THROAT_SPRAY);
    EXPECT_EQ(gBingoMons[rules->starters[0]].tags, 0);
    EXPECT(BingoMonHasSoundMove(rules->starters[0]));
}

TEST("Battle Bingo Music Board wild encounters are itemless and sound themed")
{
    u32 i;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if (!(gBingoMons[i].tags & MON_POOL_TAG_MUSIC))
            continue;

        EXPECT_EQ(gBingoMons[i].heldItem, ITEM_NONE);
        EXPECT(BingoMonHasSoundMove(i));
    }
}

TEST("Battle Bingo Music Board boss is Eddie with requested sound team")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_EDDIE];

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->trainerClass, TRAINER_CLASS_GUITARIST);
    EXPECT_EQ((u32)trainer->trainerPic, TRAINER_PIC_GUITARIST);
    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_SINGLES);
    EXPECT_EQ((u32)trainer->partySize, 3);

    EXPECT_EQ(trainer->party[0].species, SPECIES_OBSTAGOON);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_FLAME_ORB);
    EXPECT_EQ(trainer->party[0].ability, ABILITY_GUTS);
    EXPECT_EQ(trainer->party[0].moves[0], MOVE_FACADE);
    EXPECT_EQ(trainer->party[0].moves[1], MOVE_PARTING_SHOT);

    EXPECT_EQ(trainer->party[1].species, SPECIES_LAPRAS);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_ASSAULT_VEST);
    EXPECT_EQ(trainer->party[1].moves[0], MOVE_ALLURING_VOICE);
    EXPECT_EQ(trainer->party[1].moves[1], MOVE_PSYCHIC_NOISE);
    EXPECT_EQ(trainer->party[1].moves[2], MOVE_SPARKLING_ARIA);
    EXPECT_EQ(trainer->party[1].moves[3], MOVE_HYPER_VOICE);

    EXPECT_EQ(trainer->party[2].species, SPECIES_KOMMO_O);
    EXPECT_EQ(trainer->party[2].heldItem, ITEM_SITRUS_BERRY);
    EXPECT_EQ(trainer->party[2].moves[0], MOVE_CLANGOROUS_SOUL);
    EXPECT_EQ(trainer->party[2].moves[1], MOVE_CLANGING_SCALES);
    EXPECT_EQ(trainer->party[2].moves[2], MOVE_HYPER_VOICE);
    EXPECT_EQ(trainer->party[2].moves[3], MOVE_DRACO_METEOR);
}

TEST("Battle Bingo Monster Board has valid smart AI square counts and starters")
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_MONSTER);
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
    EXPECT_EQ(rules->mysteryWildCount, 0);
    EXPECT_EQ(itemCount, 6);
    EXPECT_EQ(healCount, 3);
    EXPECT_EQ(rules->mysteryItemCount, 0);
    EXPECT_EQ(rules->bossCount, 1);
    EXPECT_EQ(rules->bosses[0], TRAINER_BINGO_BOSS_STEVEN);
    EXPECT_EQ(rules->wildRuleCount, 1);
    EXPECT_EQ(rules->wilds[0].requiredTags, MON_POOL_TAG_MONSTER);
    EXPECT_EQ(rules->wilds[0].type, TYPE_MYSTERY);
    EXPECT_EQ(CountBingoMonsWithTag(MON_POOL_TAG_MONSTER), 35);
    EXPECT_EQ(rules->starterMode, BATTLE_BINGO_STARTER_CHOICE);
    EXPECT_EQ(rules->starterLevel, 50);
    EXPECT_EQ(rules->starterCount, 3);
    EXPECT_EQ(rules->music, MUS_BIOLIZARD);
    EXPECT_EQ(rules->prizeMultiplierQ8_8, BATTLE_BINGO_PRIZE_MULTIPLIER_3X);
    EXPECT(rules->wildPokemonAi & AI_FLAG_SMART_MON_CHOICES);
    EXPECT(rules->bossPokemonAi & AI_FLAG_SMART_MON_CHOICES);

    EXPECT_EQ(gBingoMons[rules->starters[0]].species, SPECIES_METAGROSS);
    EXPECT_EQ(gBingoMons[rules->starters[0]].heldItem, ITEM_WEAKNESS_POLICY);
    EXPECT(BingoMonHasMove(rules->starters[0], MOVE_AGILITY));

    EXPECT_EQ(gBingoMons[rules->starters[1]].species, SPECIES_TYRANITAR);
    EXPECT_EQ(gBingoMons[rules->starters[1]].heldItem, ITEM_CHOPLE_BERRY);
    EXPECT(BingoMonHasMove(rules->starters[1], MOVE_DRAGON_DANCE));

    EXPECT_EQ(gBingoMons[rules->starters[2]].species, SPECIES_SALAMENCE);
    EXPECT_EQ(gBingoMons[rules->starters[2]].heldItem, ITEM_NONE);
    EXPECT_EQ(gBingoMons[rules->starters[2]].ability, ABILITY_MOXIE);
    EXPECT(BingoMonHasMove(rules->starters[2], MOVE_DRAGON_DANCE));
}

TEST("Battle Bingo Monster Board wild encounters have no items protect or rest")
{
    u32 i;
    u32 j;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if (!(gBingoMons[i].tags & MON_POOL_TAG_MONSTER))
            continue;

        EXPECT_EQ(gBingoMons[i].heldItem, ITEM_NONE);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            EXPECT_NE(gBingoMons[i].moves[j], MOVE_PROTECT);
            EXPECT_NE(gBingoMons[i].moves[j], MOVE_REST);
        }
    }
}

TEST("Battle Bingo Monster Board boss is Steven with requested monster team")
{
    const struct Trainer *trainer = &sBattleBingoTestTrainers[DIFFICULTY_NORMAL][TRAINER_BINGO_BOSS_STEVEN];

    EXPECT(trainer != NULL);
    EXPECT_EQ((u32)trainer->trainerClass, TRAINER_CLASS_CHAMPION);
    EXPECT_EQ((u32)trainer->trainerPic, TRAINER_PIC_STEVEN);
    EXPECT_EQ((u32)trainer->battleType, TRAINER_BATTLE_TYPE_SINGLES);
    EXPECT_EQ((u32)trainer->partySize, 3);

    EXPECT_EQ(trainer->party[0].species, SPECIES_METAGROSS);
    EXPECT_EQ(trainer->party[0].heldItem, ITEM_ASSAULT_VEST);
    EXPECT(TrainerMonHasMove(&trainer->party[0], MOVE_METEOR_MASH));

    EXPECT_EQ(trainer->party[1].species, SPECIES_AGGRON);
    EXPECT_EQ(trainer->party[1].heldItem, ITEM_AIR_BALLOON);
    EXPECT(TrainerMonHasMove(&trainer->party[1], MOVE_HEAD_SMASH));

    EXPECT_EQ(trainer->party[2].species, SPECIES_HYDREIGON);
    EXPECT_EQ(trainer->party[2].heldItem, ITEM_CHOICE_SCARF);
    EXPECT(TrainerMonHasMove(&trainer->party[2], MOVE_DRACO_METEOR));
    EXPECT(TrainerMonHasMove(&trainer->party[2], MOVE_DARK_PULSE));
}
