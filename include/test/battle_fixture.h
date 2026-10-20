#ifndef GUARD_TEST_BATTLE_FIXTURE_H
#define GUARD_TEST_BATTLE_FIXTURE_H

#include "random.h"
#include "link.h"

#define TEST_BATTLE_MAX_ACTIONS 388
#define TEST_BATTLE_DEFAULT_TARGET 0xF

enum TestBattleActionType
{
    TEST_BATTLE_BYTE,
    TEST_BATTLE_ACTION_TYPE,
    TEST_BATTLE_MOVE_SLOT,
    TEST_BATTLE_MOVE_TARGET,
    TEST_BATTLE_PARTY_INDEX,
    TEST_BATTLE_ITEM_ID,
    TEST_BATTLE_ITEM_TARGET,
    TEST_BATTLE_ITEM_MOVE,
};

// An in-memory test fixture, never read from or written to a game save.
struct BattleTestFixture
{
    struct Pokemon parties[MAX_BATTLE_TRAINERS][PARTY_SIZE];
    // Synthetic starting battle PP; Pokémon save data has no current-PP fields.
    u8 startingPP[MAX_BATTLE_TRAINERS][PARTY_SIZE][MAX_MON_MOVES];
    u8 startingPPMask[MAX_BATTLE_TRAINERS][PARTY_SIZE];
    u8 playersName[MAX_LINK_PLAYERS][PLAYER_NAME_LENGTH + 1];
    u8 playersLanguage[MAX_LINK_PLAYERS];
    rng_value_t rngSeed;
    u32 battleFlags;
    u16 opponentA;
    u16 opponentB;
    u16 partnerId;
    u8 textSpeed;
    u8 playersBattlers;
    bool8 scriptedOpponents;
    u64 AI_scripts[MAX_BATTLERS_COUNT];
    u8 actions[MAX_BATTLERS_COUNT][TEST_BATTLE_MAX_ACTIONS];
};

void TestRunner_Battle_ApplyFixture(void);
void TestRunner_Battle_ApplyPPFixture(enum BattleTrainer trainer, enum PartySlot partyIndex, u8 pp[MAX_MON_MOVES]);
void TestRunner_Battle_EndFixture(void);
u8 TestRunner_Battle_GetAction(u32 actionType, enum BattlerId battler);

#endif // GUARD_TEST_BATTLE_FIXTURE_H
