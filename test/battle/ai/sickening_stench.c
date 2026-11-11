#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

static EWRAM_DATA bool32 sStenchSeen[PARTY_SIZE];
static EWRAM_DATA u8 sStenchTargetStages[PARTY_SIZE][NUM_BATTLE_STATS];
static EWRAM_DATA u8 sStenchSourceStages[PARTY_SIZE][NUM_BATTLE_STATS];

static void ObserveStenchCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sStenchSeen[slot] = TRUE;
    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
    {
        sStenchTargetStages[slot][stat] = gBattleMons[context->opposingBattler].statStages[stat];
        sStenchSourceStages[slot][stat] = gBattleMons[context->battler].statStages[stat];
    }
}

AI_SINGLE_BATTLE_TEST("Sickening Stench AI predicts poison-dependent drops protections and reactions without candidate leakage")
{
    static const enum Ability abilities[] = {
        ABILITY_TELEPATHY, ABILITY_OBLIVIOUS, ABILITY_TOTAL_ZEN, ABILITY_CLEAR_BODY, ABILITY_BIG_PECKS,
        ABILITY_INNER_FOCUS, ABILITY_SOUNDPROOF, ABILITY_SHIELD_DUST, ABILITY_SIMPLE,
        ABILITY_CONTRARY, ABILITY_DEFIANT, ABILITY_COMPETITIVE, ABILITY_MIRROR_ARMOR,
    };
    enum Ability ability = ABILITY_TELEPATHY;
    enum Item item = ITEM_NONE;
    bool32 poisoned = FALSE;
    for (u32 j = 0; j < ARRAY_COUNT(abilities); j++)
    {
        PARAMETRIZE { ability = abilities[j]; poisoned = FALSE; item = ITEM_NONE; }
        PARAMETRIZE { ability = abilities[j]; poisoned = TRUE; item = ITEM_NONE; }
    }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; poisoned = TRUE; item = ITEM_CLEAR_AMULET; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; poisoned = TRUE; item = ITEM_WHITE_HERB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Speed(100); Status1(poisoned ? STATUS1_TOXIC_POISON : STATUS1_NONE); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_GRIMER) { Ability(ABILITY_STICKY_HOLD); HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
        OPPONENT(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN {
        sStenchSeen[1] = sStenchSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveStenchCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sStenchSeen[1]); EXPECT(sStenchSeen[2]);
        s32 stages = poisoned ? 2 : 1;
        s32 change = ability == ABILITY_CONTRARY ? stages : -stages * (ability == ABILITY_SIMPLE ? 2 : 1);
        bool32 blocked = ability == ABILITY_OBLIVIOUS || ability == ABILITY_TOTAL_ZEN || ability == ABILITY_CLEAR_BODY || ability == ABILITY_MIRROR_ARMOR
            || item == ITEM_CLEAR_AMULET || item == ITEM_WHITE_HERB;
        EXPECT_EQ(sStenchTargetStages[1][STAT_DEF], DEFAULT_STAT_STAGE + (blocked || ability == ABILITY_BIG_PECKS ? 0 : change));
        EXPECT_EQ(sStenchTargetStages[1][STAT_SPDEF], DEFAULT_STAT_STAGE + (blocked ? 0 : change));
        EXPECT_EQ(sStenchTargetStages[1][STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_DEFIANT ? 4 : 0));
        EXPECT_EQ(sStenchTargetStages[1][STAT_SPATK], DEFAULT_STAT_STAGE + (ability == ABILITY_COMPETITIVE ? 4 : 0));
        EXPECT_EQ(sStenchSourceStages[1][STAT_DEF], DEFAULT_STAT_STAGE - (ability == ABILITY_MIRROR_ARMOR ? stages : 0));
        EXPECT_EQ(sStenchSourceStages[1][STAT_SPDEF], DEFAULT_STAT_STAGE - (ability == ABILITY_MIRROR_ARMOR ? stages : 0));
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        {
            EXPECT_EQ(sStenchTargetStages[2][stat], DEFAULT_STAT_STAGE);
            EXPECT_EQ(sStenchSourceStages[2][stat], DEFAULT_STAT_STAGE);
        }
    }
}

extern AiScoreFunc sDynamicAiFunc;
static EWRAM_DATA bool32 sStenchScoreSeen;
static EWRAM_DATA s32 sStenchScore;
static s32 ObserveStenchScore(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && !gAiLogicData->aiPredictionInProgress)
    {
        sStenchScoreSeen = TRUE;
        sStenchScore = AI_GetSickeningStenchEntryScore(attacker);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Sickening Stench AI values useful entry drops and avoids immunities and harmful reactions", s32 score)
{
    enum Ability ability;
    bool32 poisoned = FALSE;
    s32 sign = 0;
    PARAMETRIZE { ability = ABILITY_TELEPATHY; sign = 1; poisoned = FALSE; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; sign = 1; poisoned = TRUE; }
    PARAMETRIZE { ability = ABILITY_OBLIVIOUS; sign = 0; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; sign = 0; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; sign = -1; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; sign = -1; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; sign = -1; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; sign = -1; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); Status1(poisoned ? STATUS1_POISON : STATUS1_NONE); }
        // Evaluate the prospective entry before applying it to the real target.
        OPPONENT(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN {
        sStenchScoreSeen = FALSE;
        sDynamicAiFunc = ObserveStenchScore;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sStenchScoreSeen);
        if (sign > 0) EXPECT_GT(sStenchScore, 0);
        else if (sign < 0) EXPECT_LT(sStenchScore, 0);
        else EXPECT_EQ(sStenchScore, 0);
        results[i].score = sStenchScore;
    } FINALLY {
        EXPECT_EQ(results[1].score, 2 * results[0].score);
    }
}
