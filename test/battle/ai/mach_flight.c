#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "constants/battle_util.h"

AI_SINGLE_BATTLE_TEST("Mach Flight AI knows its doubled Speed and avoids Tailwind even with setup and powerful-status flags")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FORCE_SETUP_FIRST_TURN | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PIDGEOTTO) { Item(ITEM_APEX_METEORITE); Speed(60); Moves(MOVE_TAILWIND, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_TAILWIND, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_MACH_FLIGHT);
        EXPECT_EQ(gAiLogicData->speedStats[B_BATTLER_1], 120);
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, MOVE_TACKLE, CONSIDER_PRIORITY));
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND));
    }
}

AI_SINGLE_BATTLE_TEST("Mach Flight AI recognizes the opponent's doubled Speed from its item reveal")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_PIDGEY) { Item(item); Speed(60); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gAiLogicData->speedStats[B_BATTLER_0], item == ITEM_APEX_METEORITE ? 120 : 60);
        EXPECT_EQ(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, MOVE_TACKLE, CONSIDER_PRIORITY), item == ITEM_NONE);
    }
}

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObservedMachPrediction;
static u32 sMachPredictionTurn;
static s32 sMachCritStage;
static struct SimulatedDamage sMachDamage;

static s32 ObserveMachFlightPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sMachPredictionTurn
     && !gAiLogicData->aiPredictionInProgress && move != MOVE_FOCUS_ENERGY)
    {
        struct DamageContext ctx = {0};
        ctx.battlerAtk = battlerAtk;
        ctx.battlerDef = battlerDef;
        ctx.move = ctx.chosenMove = move;
        ctx.aiCalc = TRUE;
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            ctx.abilities[battler] = gAiLogicData->abilities[battler];
            ctx.holdEffects[battler] = gAiLogicData->holdEffects[battler];
        }
        sMachCritStage = CalcCritChanceStage(&ctx);
        sMachDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sObservedMachPrediction = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Mach Flight AI predicts its critical stage using priority and Trick Room")
{
    enum Move move, enemyMove;
    u16 speed;
    bool32 trickRoom;
    u32 stage;
    PARAMETRIZE { move = MOVE_TACKLE; enemyMove = MOVE_TACKLE; speed = 60; trickRoom = FALSE; stage = 1; }
    PARAMETRIZE { move = MOVE_TACKLE; enemyMove = MOVE_QUICK_ATTACK; speed = 60; trickRoom = FALSE; stage = 0; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; enemyMove = MOVE_TACKLE; speed = 10; trickRoom = FALSE; stage = 1; }
    PARAMETRIZE { move = MOVE_TACKLE; enemyMove = MOVE_TACKLE; speed = 60; trickRoom = TRUE; stage = 0; }
    PARAMETRIZE { move = MOVE_TACKLE; enemyMove = MOVE_TACKLE; speed = 10; trickRoom = TRUE; stage = 1; }
    ResetStartingStatuses();
    if (trickRoom)
        SetStartingStatus(STARTING_STATUS_TRICK_ROOM);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREDICT_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); Moves(enemyMove); }
        OPPONENT(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(speed); HP(3000); MaxHP(3000); Moves(move); }
    } WHEN {
        sObservedMachPrediction = FALSE;
        sMachPredictionTurn = 0;
        sDynamicAiFunc = ObserveMachFlightPrediction;
        TURN { MOVE(player, enemyMove); EXPECT_MOVE(opponent, move); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedMachPrediction);
        EXPECT_EQ(sMachCritStage, stage);
    }
}

AI_SINGLE_BATTLE_TEST("Mach Flight AI predicts guaranteed critical damage with Focus Energy only when acting first")
{
    enum Move enemyMove;
    s16 damage;
    PARAMETRIZE { enemyMove = MOVE_TACKLE; }
    PARAMETRIZE { enemyMove = MOVE_QUICK_ATTACK; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FORCE_SETUP_FIRST_TURN | AI_FLAG_PREDICT_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); Moves(enemyMove); }
        OPPONENT(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Speed(60); HP(3000); MaxHP(3000); Moves(MOVE_FOCUS_ENERGY, MOVE_TACKLE); }
    } WHEN {
        sObservedMachPrediction = FALSE;
        sMachPredictionTurn = 1;
        sDynamicAiFunc = ObserveMachFlightPrediction;
        TURN { MOVE(player, enemyMove); EXPECT_MOVE(opponent, MOVE_FOCUS_ENERGY); }
        TURN { MOVE(player, enemyMove); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The opposing Pidgey used Tackle!");
        HP_BAR(player, captureDamage: &damage);
        if (enemyMove == MOVE_TACKLE)
            MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedMachPrediction);
        EXPECT_EQ(sMachCritStage, enemyMove == MOVE_TACKLE ? 3 : 2);
        if (enemyMove == MOVE_TACKLE) {
            EXPECT_GE(damage, sMachDamage.minimum);
            EXPECT_LE(damage, sMachDamage.maximum);
        }
    }
}

static bool32 sObservedMachCandidate;
static u32 sMachCandidateSpeed;

static void ObserveMachFlightCandidate(struct SwitchAiContext *context)
{
    if (gBattleMons[context->battler].species == SPECIES_PIDGEOTTO)
    {
        sObservedMachCandidate = TRUE;
        sMachCandidateSpeed = gAiLogicData->speedStats[context->battler];
    }
}

AI_SINGLE_BATTLE_TEST("Mach Flight switch-in AI applies personal Tailwind without stacking existing Tailwind")
{
    enum Item item;
    bool32 tailwind;
    PARAMETRIZE { item = ITEM_NONE; tailwind = FALSE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; tailwind = FALSE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; tailwind = TRUE; }
    ResetStartingStatuses();
    if (tailwind)
        SetStartingStatus(STARTING_STATUS_TAILWIND_OPPONENT);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIDGEOTTO) { Item(item); Speed(60); Moves(MOVE_TACKLE); }
    } WHEN {
        sObservedMachCandidate = FALSE;
        gDynamicAiMonChoiceFunc = ObserveMachFlightCandidate;
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sObservedMachCandidate);
        EXPECT_EQ(sMachCandidateSpeed, item == ITEM_APEX_METEORITE ? 120 : 60);
        EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND, tailwind ? SIDE_STATUS_TAILWIND : 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Mach Flight AI recognizes personal Tailwind for Harsh Zephyr targeting")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PIDGEY) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HARSH_ZEPHYR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_HARSH_ZEPHYR); }
    } THEN {
        EXPECT_EQ(AI_GetBattlerMoveTargetType(B_BATTLER_1, MOVE_HARSH_ZEPHYR), TARGET_BOTH);
        EXPECT_EQ(AI_GetBattlerMoveTargetType(B_BATTLER_3, MOVE_HARSH_ZEPHYR), TARGET_SELECTED);
        EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        EXPECT_LT(playerRight->hp, playerRight->maxHP);
    }
}
