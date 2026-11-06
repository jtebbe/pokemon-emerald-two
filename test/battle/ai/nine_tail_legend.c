#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sLegendDamage;
static s32 sLegendScore;
static bool32 sLegendObserved;
static s32 ObserveNineTailLegend(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sLegendDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sLegendScore = score;
        sLegendObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Nine-Tail Legend AI predicts guaranteed critical damage from high-crit moves", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_NINETALES) { Item(item); Moves(MOVE_SLASH); }
    } WHEN {
        sLegendObserved = FALSE;
        sDynamicAiFunc = ObserveNineTailLegend;
        TURN { EXPECT_MOVE(opponent, MOVE_SLASH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        if (item == ITEM_APEX_METEORITE) MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLegendObserved);
        EXPECT_GE(results[i].damage, sLegendDamage.minimum);
        EXPECT_LE(results[i].damage, sLegendDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Nine-Tail Legend AI favors a high-crit move when the aura guarantees a critical hit")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_NINETALES) { Item(item); Moves(MOVE_SLASH, MOVE_STRENGTH); }
    } WHEN {
        if (item == ITEM_APEX_METEORITE)
            TURN { SCORE_GT(opponent, MOVE_SLASH, MOVE_STRENGTH); EXPECT_MOVE(opponent, MOVE_SLASH); }
        else
            TURN { SCORE_GT(opponent, MOVE_STRENGTH, MOVE_SLASH); EXPECT_MOVE(opponent, MOVE_STRENGTH); }
    }
}

AI_SINGLE_BATTLE_TEST("Nine-Tail Legend AI accounts for the cost of sealing its first damaging attack", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NINETALES) { Ability(ABILITY_FLASH_FIRE); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        sLegendObserved = FALSE;
        sDynamicAiFunc = ObserveNineTailLegend;
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLegendObserved);
        results[i].score = sLegendScore;
    } FINALLY {
        EXPECT_EQ(results[0].score - GOOD_EFFECT, results[1].score);
    }
}

AI_SINGLE_BATTLE_TEST("Nine-Tail Legend AI stops selecting a permanently sealed attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STRENGTH, MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_STRENGTH); }
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_STRENGTH));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
    }
}

AI_SINGLE_BATTLE_TEST("Nine-Tail Legend AI uses Struggle when its only move is sealed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
    }
}

// Fix the target so predicted and actual damage refer to the same defender.
static s32 ObserveNineTailLegendPartner(u32 attacker, u32 defender, u32 move, s32 score)
{
    score = ObserveNineTailLegend(attacker, defender, move, score);
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0) return score + 20;
    return score;
}

AI_DOUBLE_BATTLE_TEST("Nine-Tail Legend AI predicts its partner's critical aura and respects Battle Armor", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sLegendObserved = FALSE;
        sDynamicAiFunc = ObserveNineTailLegendPartner;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SLASH, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
        if (ability == ABILITY_NONE) MESSAGE("A critical hit!");
        else NOT MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLegendObserved);
        EXPECT_GE(results[i].damage, sLegendDamage.minimum);
        EXPECT_LE(results[i].damage, sLegendDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.5), results[0].damage);
    }
}
