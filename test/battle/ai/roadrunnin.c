#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sRoadDamage;
static bool32 sRoadSeen;
static u32 sRoadStage;

static s32 ObserveRoadrunnin(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sRoadSeen = TRUE;
        sRoadStage = gBattleMons[attacker].statStages[STAT_SPEED];
        sRoadDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Roadrunnin AI damage forecasts match the current Speed tier before the post-attack boost")
{
    u32 speed = 100;
    enum Move move = MOVE_TACKLE;
    static const u16 speeds[] = {101, 100, 76, 75, 61, 60, 51, 50};
    for (u32 j = 0; j < ARRAY_COUNT(speeds); j++)
    {
        PARAMETRIZE { speed = speeds[j]; move = MOVE_TACKLE; }
        PARAMETRIZE { speed = speeds[j]; move = MOVE_WATER_GUN; }
    }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Speed(speed); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(move); }
    } WHEN {
        sRoadSeen = FALSE;
        sDynamicAiFunc = ObserveRoadrunnin;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sRoadSeen);
        EXPECT_EQ(sRoadStage, DEFAULT_STAT_STAGE + 1);
        EXPECT_GE(damage, sRoadDamage.minimum);
        EXPECT_LE(damage, sRoadDamage.maximum);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_ROADRUNNIN);
    }
}

static bool32 sRoadCandidateSeen[PARTY_SIZE];
static u32 sRoadCandidateStage[PARTY_SIZE], sRoadCandidateSpeed[PARTY_SIZE];
static struct SimulatedDamage sRoadCandidateDamage[PARTY_SIZE];

static void ObserveRoadrunninCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 3)
        return;
    sRoadCandidateSeen[slot] = TRUE;
    sRoadCandidateStage[slot] = gBattleMons[context->battler].statStages[STAT_SPEED];
    sRoadCandidateSpeed[slot] = gAiLogicData->speedStats[context->battler];
    sRoadCandidateDamage[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0];
}

AI_SINGLE_BATTLE_TEST("Roadrunnin AI predicts entry Speed and damage without leaking boosts to other switch candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(100); Defense(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_DODUO) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Attack(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DODRIO) { Ability(ABILITY_RUN_AWAY); HP(1000); MaxHP(1000); Speed(100); Attack(100); Moves(MOVE_TACKLE); }
    } WHEN {
        sRoadCandidateSeen[1] = sRoadCandidateSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRoadrunninCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRoadCandidateSeen[1]); EXPECT(sRoadCandidateSeen[2]);
        EXPECT_EQ(sRoadCandidateStage[1], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(sRoadCandidateStage[2], DEFAULT_STAT_STAGE);
        EXPECT_EQ(sRoadCandidateSpeed[1], 150);
        EXPECT_EQ(sRoadCandidateSpeed[2], 100);
        EXPECT_MUL_EQ(sRoadCandidateDamage[2].median, UQ_4_12(1.25), sRoadCandidateDamage[1].median);
    }
}

AI_DOUBLE_BATTLE_TEST("Roadrunnin AI applies one entry boost rather than one for each opposing battler")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(90); Moves(MOVE_QUICK_ATTACK); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(80); }
        OPPONENT(SPECIES_DODUO) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DODRIO) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DODRIO) { Ability(ABILITY_RUN_AWAY); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        sRoadCandidateSeen[2] = sRoadCandidateSeen[3] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRoadrunninCandidate;
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRoadCandidateSeen[2]); EXPECT(sRoadCandidateSeen[3]);
        EXPECT_EQ(sRoadCandidateStage[2], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(sRoadCandidateStage[3], DEFAULT_STAT_STAGE);
        EXPECT_EQ(sRoadCandidateSpeed[2], 150);
        EXPECT_EQ(sRoadCandidateSpeed[3], 100);
    }
}
