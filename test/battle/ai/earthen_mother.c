#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("Earthen Mother AI avoids absorbed Ground attacks and status moves even against Mold Breaker")
{
    enum Move move;
    enum Ability ability;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { move = MOVE_EARTH_POWER; ability = j ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_SAND_ATTACK; ability = j ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(move, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, move, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_EARTHEN_MOTHER);
        EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].maximum, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Earthen Mother AI rewards an ally's Earthquake only when its healing is useful")
{
    u32 partnerHP;
    enum Ability ability;
    PARAMETRIZE { partnerHP = 100; ability = ABILITY_NONE; }
    PARAMETRIZE { partnerHP = 20; ability = ABILITY_NONE; }
    PARAMETRIZE { partnerHP = 100; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { partnerHP = 20; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(90); Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); HP(partnerHP); MaxHP(100); Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (partnerHP == 20)
            TURN { SCORE_GT_VAL(opponentLeft, MOVE_EARTHQUAKE, AI_SCORE_DEFAULT, target: opponentRight); }
        else
            TURN { SCORE_LT_VAL(opponentLeft, MOVE_EARTHQUAKE, AI_SCORE_DEFAULT, target: opponentRight); }
    }
}

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sBoostedGroundDamage;
static bool32 sGroundBoostObserved;
static s32 ObserveGroundBoost(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sBoostedGroundDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sGroundBoostObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Earthen Mother AI predicts its boosted Ground damage", s16 damage)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_NIDOQUEEN) { Ability(ABILITY_POISON_POINT); Item(item); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sGroundBoostObserved = FALSE;
        sDynamicAiFunc = ObserveGroundBoost;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sGroundBoostObserved);
        EXPECT_GE(results[i].damage, sBoostedGroundDamage.minimum);
        EXPECT_LE(results[i].damage, sBoostedGroundDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.5), results[3].damage);
    }
}

static struct SimulatedDamage sGuardedDamage;
static bool32 sGuardObserved;
static s32 ObserveGuardedDamage(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_2
     && !gAiLogicData->aiPredictionInProgress)
    {
        sGuardedDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sGuardObserved = TRUE;
        // Select the protected ally so its predicted and actual damage can be compared.
        return score + 20;
    }
    return score;
}

AI_DOUBLE_BATTLE_TEST("Earthen Mother AI predicts damage to the protected ally including against Mold Breaker")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_MOLD_BREAKER; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NIDOQUEEN) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(1000); SpDefense(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Attack(300); SpAttack(300); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        sGuardObserved = FALSE;
        sDynamicAiFunc = ObserveGuardedDamage;
        if (move == MOVE_SWIFT)
            TURN { EXPECT_MOVE(opponentLeft, move); }
        else
            TURN { EXPECT_MOVE(opponentLeft, move, target: playerRight); }
    } SCENE {
        if (move == MOVE_SWIFT) HP_BAR(playerLeft);
        HP_BAR(playerRight, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sGuardObserved);
        EXPECT_GE(damage, sGuardedDamage.minimum);
        EXPECT_LE(damage, sGuardedDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Earthen Mother switch AI recognizes a bench Ground immunity even against Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TINKATON) { Ability(ability); Speed(3); Moves(MOVE_EARTH_POWER); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_NIDORINA) { HP(1); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_HYPNO) { Speed(1); Moves(MOVE_IRON_HEAD); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTH_POWER); EXPECT_SEND_OUT(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("Earthen Mother smart switching seeks a Ground absorber even against Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PICKUP; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ability); Moves(MOVE_EARTH_POWER, MOVE_STRENGTH); }
        OPPONENT(SPECIES_ZIGZAGOON) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_NIDORINA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTH_POWER); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_EARTH_POWER); EXPECT_SWITCH(opponent, 1); }
    }
}
