#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

extern AiScoreFunc sDynamicAiFunc;
static s32 sLuauScore, sLuauValue, sCopyValue;
static bool32 sLuauObserved;
static enum Move sLuauObservedMove;
static u32 sLuauObservedTurn;
static s32 ObserveLuau(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && move == sLuauObservedMove && gBattleResults.battleTurnCounter == sLuauObservedTurn
     && !gAiLogicData->aiPredictionInProgress)
    {
        sLuauObserved = TRUE;
        sLuauScore = score;
        sLuauValue = AI_GetEndlessLuauScore(attacker);
        sCopyValue = AI_GetDanceCopyScore(attacker, move);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Endless Luau AI values Protect more when a useful automatic dance can accompany it", s32 score)
{
    enum Item item = ITEM_NONE;
    enum Move dance = MOVE_QUIVER_DANCE;
    const enum Move moves[] = {MOVE_QUIVER_DANCE, MOVE_SCRATCH};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { dance = moves[j]; item = ITEM_NONE; }
        PARAMETRIZE { dance = moves[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_BELLOSSOM) { Ability(ABILITY_CHLOROPHYLL); Item(item); HP(3000); MaxHP(3000); Moves(MOVE_PROTECT, dance); }
    } WHEN {
        sLuauObserved = FALSE;
        sLuauObservedMove = MOVE_PROTECT;
        sLuauObservedTurn = 0;
        sDynamicAiFunc = ObserveLuau;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLuauObserved);
        results[i].score = sLuauScore;
        EXPECT_EQ(sLuauValue > 0, item == ITEM_APEX_METEORITE && dance == MOVE_QUIVER_DANCE);
    } FINALLY {
        EXPECT_EQ(results[0].score + DECENT_EFFECT, results[1].score);
        EXPECT_EQ(results[2].score, results[3].score);
    }
}

AI_SINGLE_BATTLE_TEST("Endless Luau AI accounts for its Dancer component when an enemy could copy setup", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_BELLOSSOM) { Item(item); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_SCRATCH); }
    } WHEN {
        sLuauObserved = FALSE;
        sLuauObservedMove = MOVE_SWORDS_DANCE;
        sLuauObservedTurn = 0;
        sDynamicAiFunc = ObserveLuau;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLuauObserved);
        results[i].score = sLuauScore;
        EXPECT_EQ(sCopyValue, item == ITEM_APEX_METEORITE ? -WEAK_EFFECT : 0);
    } FINALLY {
        EXPECT_EQ(results[0].score - WEAK_EFFECT, results[1].score);
    }
}

AI_DOUBLE_BATTLE_TEST("Endless Luau AI values giving a partner a useful Dancer copy")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_BELLOSSOM) { Item(item); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sLuauObserved = FALSE;
        sLuauObservedMove = MOVE_SWORDS_DANCE;
        sLuauObservedTurn = 0;
        sDynamicAiFunc = ObserveLuau;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLuauObserved);
        EXPECT_EQ(sCopyValue, item == ITEM_APEX_METEORITE ? WEAK_EFFECT : 0);
    }
}

static bool32 sLuauCandidateObserved, sLuauCandidateLeaked;
static void ObserveLuauCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1 && gBattlerPartyIndexes[context->battler] == 0)
    {
        sLuauCandidateObserved = TRUE;
        sLuauValue = AI_GetEndlessLuauScore(context->battler);
        if (gBattleMons[context->battler].statStages[STAT_SPATK] != DEFAULT_STAT_STAGE)
            sLuauCandidateLeaked = TRUE;
    }
}

AI_SINGLE_BATTLE_TEST("Endless Luau switch AI favors a winning candidate with a useful known dance without applying a hypothetical boost")
{
    enum Item item;
    enum Move dance;
    PARAMETRIZE { item = ITEM_NONE; dance = MOVE_QUIVER_DANCE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; dance = MOVE_QUIVER_DANCE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; dance = MOVE_LUNAR_DANCE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BELLOSSOM) { Item(item); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM, dance); }
        OPPONENT(SPECIES_BELLOSSOM) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM, dance); }
    } WHEN {
        sLuauCandidateObserved = FALSE;
        sLuauCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveLuauCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, item == ITEM_APEX_METEORITE && dance == MOVE_QUIVER_DANCE ? 1 : 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sLuauCandidateObserved);
        EXPECT(!sLuauCandidateLeaked);
        EXPECT_EQ(sLuauValue > 0, item == ITEM_APEX_METEORITE && dance == MOVE_QUIVER_DANCE);
        if (dance == MOVE_LUNAR_DANCE) EXPECT_LT(sLuauValue, 0);
    }
}

static s32 ObserveTauntedLuau(u32 attacker, u32 defender, u32 move, s32 score)
{
    score = ObserveLuau(attacker, defender, move, score);
    // Establish Taunt on the setup turn before evaluating the blocked dance.
    if (gBattleResults.battleTurnCounter == 0 && move == MOVE_PROTECT)
        return score - 100;
    return score;
}

AI_SINGLE_BATTLE_TEST("Endless Luau AI gives no automatic-dance benefit to a status move blocked by Taunt")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); HP(3000); MaxHP(3000); Moves(MOVE_TAUNT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_PROTECT, MOVE_QUIVER_DANCE, MOVE_SCRATCH); }
    } WHEN {
        sLuauObserved = FALSE;
        sLuauObservedMove = MOVE_SCRATCH;
        sLuauObservedTurn = 1;
        sDynamicAiFunc = ObserveTauntedLuau;
        TURN { MOVE(player, MOVE_TAUNT); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLuauObserved);
        EXPECT_EQ(sLuauValue, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Endless Luau AI does not value an automatic attack absorbed by the opponent")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_FLASH_FIRE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PROTECT, MOVE_FIERY_DANCE); }
    } WHEN {
        sLuauObserved = FALSE;
        sLuauObservedMove = MOVE_PROTECT;
        sLuauObservedTurn = 0;
        sDynamicAiFunc = ObserveLuau;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLuauObserved);
        EXPECT_EQ(sLuauValue > 0, ability == ABILITY_NONE);
    }
}

AI_SINGLE_BATTLE_TEST("Endless Luau reveals its automatically used known move to AI without replacing normal action history")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_0][MOVESLOT_0], MOVE_SCRATCH);
        EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_0][MOVESLOT_1], MOVE_QUIVER_DANCE);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_SCRATCH);
    }
}
