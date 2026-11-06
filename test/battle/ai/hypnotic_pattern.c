#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sHypnoticDamage;
static bool32 sHypnoticDamageObserved;
static s32 ObserveHypnoticDamage(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sHypnoticDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sHypnoticDamageObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI predicts doubled physical and special damage against sleep and Comatose", s16 damage)
{
    enum Move move;
    u32 status;
    enum Ability ability;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; status = STATUS1_BURN; ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_SLEEP_TURN(2); ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_NONE; ability = ABILITY_COMATOSE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(1); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); Speed(100); Moves(move); }
    } WHEN {
        sHypnoticDamageObserved = FALSE;
        sDynamicAiFunc = ObserveHypnoticDamage;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHypnoticDamageObserved);
        EXPECT_GE(results[i].damage, sHypnoticDamage.minimum);
        EXPECT_LE(results[i].damage, sHypnoticDamage.maximum);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
            EXPECT_MUL_EQ(results[j * 3].damage, UQ_4_12(2.0), results[j * 3 + 1].damage);
            EXPECT_EQ(results[j * 3 + 1].damage, results[j * 3 + 2].damage);
        }
    }
}

static s32 sHypnoticEntryScore;
static bool32 sHypnoticCandidateObserved;
static bool32 sHypnoticCandidateLeaked;
static void ObserveHypnoticCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1 && gBattlerPartyIndexes[context->battler] == 0)
    {
        sHypnoticCandidateObserved = TRUE;
        sHypnoticEntryScore = AI_GetHypnoticPatternEntryScore(context->battler);
        if (gBattleMons[context->opposingBattler].status1 != STATUS1_NONE)
            sHypnoticCandidateLeaked = TRUE;
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI favors useful entry sleep and avoids immune or instantly waking foes")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_INSOMNIA; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; }
    PARAMETRIZE { ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { ability = ABILITY_COMATOSE; }
    PARAMETRIZE { ability = ABILITY_EARLY_BIRD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_POLIWRATH) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sHypnoticCandidateObserved = FALSE;
        sHypnoticCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveHypnoticCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, ability == ABILITY_NONE ? 1 : 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sHypnoticCandidateObserved);
        EXPECT(!sHypnoticCandidateLeaked);
        if (ability == ABILITY_NONE) EXPECT_GT(sHypnoticEntryScore, 0);
        else EXPECT_EQ(sHypnoticEntryScore, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI values consuming a curing berry below preventing an action", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CHESTO_BERRY; }
    PARAMETRIZE { item = ITEM_LUM_BERRY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(item); HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_POLIWRATH) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sHypnoticCandidateObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveHypnoticCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sHypnoticCandidateObserved);
        results[i].score = sHypnoticEntryScore;
    } FINALLY {
        EXPECT_GT(results[0].score, results[1].score);
        EXPECT_GT(results[1].score, 0);
        EXPECT_EQ(results[1].score, results[2].score);
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI does not favor entry sleep against an already statused foe")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); HP(120); MaxHP(1000); Attack(100); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_POLIWRATH) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 2); }
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI respects Electric and Misty Terrain when valuing entry sleep")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_ELECTRIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_MISTY_SURGE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { Ability(ability); HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_POLIWRATH) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sHypnoticCandidateObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveHypnoticCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sHypnoticCandidateObserved);
        EXPECT_EQ(sHypnoticEntryScore, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI recognizes opposing Apex users without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INSOMNIA); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_HYPNOTIC_PATTERN);
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI anticipates a faster foe waking before damage is dealt", s16 damage)
{
    u32 status;
    enum Ability ability;
    PARAMETRIZE { status = STATUS1_BURN; ability = ABILITY_NONE; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(1); ability = ABILITY_NONE; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(2); ability = ABILITY_NONE; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(2); ability = ABILITY_EARLY_BIRD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); SpDefense(100); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); SpAttack(300); Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sHypnoticDamageObserved = FALSE;
        sDynamicAiFunc = ObserveHypnoticDamage;
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHypnoticDamageObserved);
        EXPECT_GE(results[i].damage, sHypnoticDamage.minimum);
        EXPECT_LE(results[i].damage, sHypnoticDamage.maximum);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[2].damage);
        EXPECT_EQ(results[0].damage, results[3].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Hypnotic Pattern AI accounts for priority hitting before the foe wakes", s16 damage)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(1); }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); Defense(100); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); Attack(300); Speed(1); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        sHypnoticDamageObserved = FALSE;
        sDynamicAiFunc = ObserveHypnoticDamage;
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHypnoticDamageObserved);
        EXPECT_GE(results[i].damage, sHypnoticDamage.minimum);
        EXPECT_LE(results[i].damage, sHypnoticDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}
