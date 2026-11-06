#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_ai_field_statuses.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sRiderSeen;
static enum BattlerId sRiderTarget;
static struct SimulatedDamage sRiderDamage;
static struct SimulatedDamage sRiderTurnDamage[2];
static s32 ObserveRainbowDamage(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == sRiderTarget
     && !gAiLogicData->aiPredictionInProgress)
    {
        sRiderSeen = TRUE;
        sRiderDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        if (gBattleResults.battleTurnCounter < ARRAY_COUNT(sRiderTurnDamage))
            sRiderTurnDamage[gBattleResults.battleTurnCounter] = sRiderDamage;
        return score + (defender == B_BATTLER_2 ? 20 : 0);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Rainbow Rider AI predicts its own protection and stops after terrain is overwritten")
{
    s16 before, after;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RAPIDASH_GALAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); Defense(100); Moves(MOVE_CELEBRATE, MOVE_GRASSY_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Attack(300); Moves(MOVE_SCRATCH); }
    } WHEN {
        sRiderTarget = B_BATTLER_0; sRiderSeen = FALSE; sDynamicAiFunc = ObserveRainbowDamage;
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &before);
        HP_BAR(player); // Grassy Terrain heals after the first attack.
        HP_BAR(player, captureDamage: &after);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sRiderSeen);
        struct SimulatedDamage beforePrediction = sRiderTurnDamage[0];
        struct SimulatedDamage afterPrediction = sRiderTurnDamage[1];
        EXPECT_GE(before, beforePrediction.minimum); EXPECT_LE(before, beforePrediction.maximum);
        EXPECT_GE(after, afterPrediction.minimum); EXPECT_LE(after, afterPrediction.maximum);
        EXPECT_MUL_EQ(afterPrediction.median, UQ_4_12(0.67), beforePrediction.median);
    }
}

AI_DOUBLE_BATTLE_TEST("Rainbow Rider AI predicts airborne ally protection even against Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PONYTA_GALAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEVITATE); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Attack(300); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        sRiderTarget = B_BATTLER_2; sRiderSeen = FALSE; sDynamicAiFunc = ObserveRainbowDamage;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sRiderSeen);
        EXPECT_GE(damage, sRiderDamage.minimum); EXPECT_LE(damage, sRiderDamage.maximum);
        EXPECT(TerrainChecker(B_BATTLER_2, B_TERRAIN_MISTY, FIELD_EFFECT_POSITIVE));
    }
}

static bool32 sRiderCandidateSeen[PARTY_SIZE];
static u32 sRiderTerrain[PARTY_SIZE], sRiderIncoming[PARTY_SIZE], sRiderActualTerrain[PARTY_SIZE];
static void ObserveRiderCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sRiderCandidateSeen[slot] = TRUE;
    sRiderTerrain[slot] = AI_GetSwitchinTerrain(context->battler);
    sRiderActualTerrain[slot] = gFieldTimers.terrain;
    sRiderIncoming[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][0].median;
}

AI_SINGLE_BATTLE_TEST("Rainbow Rider AI forecasts entry terrain and shielding without leaking terrain between candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Attack(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RAPIDASH_GALAR) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Defense(100); Speed(2); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_RAPIDASH_GALAR) { Ability(ABILITY_RUN_AWAY); HP(1000); MaxHP(1000); Defense(100); Speed(2); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sRiderCandidateSeen[1] = sRiderCandidateSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRiderCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRiderCandidateSeen[1]); EXPECT(sRiderCandidateSeen[2]);
        EXPECT_EQ(sRiderTerrain[1], B_TERRAIN_MISTY);
        EXPECT_EQ(sRiderTerrain[2], B_TERRAIN_NONE);
        EXPECT_EQ(sRiderActualTerrain[1], B_TERRAIN_NONE);
        EXPECT_EQ(sRiderActualTerrain[2], B_TERRAIN_NONE);
        EXPECT_MUL_EQ(sRiderIncoming[2], UQ_4_12(0.67), sRiderIncoming[1]);
    }
}
