#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sShrapnelDamage;
static bool32 sShrapnelObserved;
static s32 ObserveShrapnelDamage(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && !gAiLogicData->aiPredictionInProgress && gBattleResults.battleTurnCounter == 0)
    {
        sShrapnelDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sShrapnelObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Frozen Shrapnel AI predicts its damage reduction and ordinary frostbite damage reduction together")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_NONE; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Item(item); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_FROSTBITE); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sShrapnelObserved = FALSE;
        sDynamicAiFunc = ObserveShrapnelDamage;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sShrapnelObserved);
        EXPECT_GE(damage, sShrapnelDamage.minimum);
        EXPECT_LE(damage, sShrapnelDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Frozen Shrapnel AI avoids contact that sets Frigid Spikes and respects Protective Pads")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_PROTECTIVE_PADS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Attack(100); SpAttack(100); Moves(MOVE_SCRATCH, MOVE_GUST); }
    } WHEN {
        TURN {
            if (item == ITEM_NONE) {
                SCORE_LT(opponent, MOVE_SCRATCH, MOVE_GUST);
                EXPECT_MOVE(opponent, MOVE_GUST);
            } else {
                SCORE_EQ(opponent, MOVE_SCRATCH, MOVE_GUST);
            }
        }
    }
}

static bool32 sFrigidCandidateObserved;
static u32 sFrigidCandidateStatus;
static void ObserveFrigidCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1 && gBattleResults.battleTurnCounter >= 1)
    {
        sFrigidCandidateObserved = TRUE;
        sFrigidCandidateStatus = gBattleMons[context->battler].status1;
    }
}

AI_SINGLE_BATTLE_TEST("Frozen Shrapnel switch AI predicts frostbite for grounded candidates and respects Boots")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HEAVY_DUTY_BOOTS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WYNAUT) { Item(item); HP(800); MaxHP(800); Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sFrigidCandidateObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveFrigidCandidate;
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sFrigidCandidateObserved);
        EXPECT_EQ(sFrigidCandidateStatus, item == ITEM_HEAVY_DUTY_BOOTS ? STATUS1_NONE : STATUS1_FROSTBITE);
        EXPECT_EQ(opponent->status1, item == ITEM_HEAVY_DUTY_BOOTS ? STATUS1_NONE : STATUS1_FROSTBITE);
    }
}

AI_SINGLE_BATTLE_TEST("Frozen Shrapnel switch AI avoids a candidate that would faint to Frigid Spikes")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WYNAUT) { HP(40); MaxHP(800); Speed(1); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_HEAVY_DUTY_BOOTS); HP(800); MaxHP(800); Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_SEND_OUT(opponent, 2); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_HEAVY_DUTY_BOOTS);
        EXPECT_EQ(opponent->hp, 800);
    }
}
