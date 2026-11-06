#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObservedFists;
static struct SimulatedDamage sFistsDamage;
static s32 sFistsScore;
static u32 sFistsSpeed;
static s32 ObserveFistsOfFury(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == gBattleMons[attacker].moves[0]
     && !gAiLogicData->aiPredictionInProgress && gBattleResults.battleTurnCounter == 0)
    {
        sObservedFists = TRUE;
        sFistsDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sFistsScore = score;
        sFistsSpeed = gBattleMons[attacker].statStages[STAT_SPEED];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI predicts boosted punching damage without applying the later Speed boost", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_MEGA_PUNCH; }
    PARAMETRIZE { move = MOVE_THUNDER_PUNCH; }
    PARAMETRIZE { move = MOVE_MACH_PUNCH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(60); Moves(move); }
    } WHEN { sObservedFists = FALSE; sDynamicAiFunc = ObserveFistsOfFury; TURN { EXPECT_MOVE(opponent, move); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedFists);
        EXPECT_EQ(sFistsSpeed, DEFAULT_STAT_STAGE);
        EXPECT_GE(results[i].damage, sFistsDamage.minimum);
        EXPECT_LE(results[i].damage, sFistsDamage.maximum);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_FISTS_OF_FURY);
    }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI includes the power accuracy and Speed benefits when choosing a punch")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(60); Moves(MOVE_MEGA_PUNCH, MOVE_STRENGTH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_MEGA_PUNCH, MOVE_STRENGTH); EXPECT_MOVE(opponent, MOVE_MEGA_PUNCH); } }
    THEN { EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], 100); EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_1], 100); }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI gives no accuracy benefit to non-punching moves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_HYDRO_PUMP); } }
    THEN { EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], 80); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI values Speed gained from a punching move", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PRIMEAPE) { Ability(ABILITY_VITAL_SPIRIT); Item(item); Speed(80); Moves(MOVE_MEGA_PUNCH); }
    } WHEN { sObservedFists = FALSE; sDynamicAiFunc = ObserveFistsOfFury; TURN { EXPECT_MOVE(opponent, MOVE_MEGA_PUNCH); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObservedFists); results[i].score = sFistsScore; }
    FINALLY { EXPECT_GT(results[1].score, results[0].score); }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI can use an immune punch as Speed setup and stops once Speed is capped")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTLY) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(80); Moves(MOVE_MEGA_PUNCH, MOVE_GROWL); }
    } WHEN {
        for (u32 turn = 0; turn < 6; turn++) TURN { SCORE_GT(opponent, MOVE_MEGA_PUNCH, MOVE_GROWL); EXPECT_MOVE(opponent, MOVE_MEGA_PUNCH); }
        TURN { SCORE_GT(opponent, MOVE_GROWL, MOVE_MEGA_PUNCH); EXPECT_MOVE(opponent, MOVE_GROWL); }
    } THEN { EXPECT_EQ(opponent->statStages[STAT_SPEED], MAX_STAT_STAGE); EXPECT_EQ(player->hp, 3000); EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI uses a priority punch instead of assuming its later Speed boost acts first")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Attack(4000); Speed(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); HP(1); Speed(80); Moves(MOVE_MEGA_PUNCH, MOVE_MACH_PUNCH); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_MACH_PUNCH, MOVE_MEGA_PUNCH); EXPECT_MOVE(opponent, MOVE_MACH_PUNCH); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(opponent->hp, 1); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

AI_SINGLE_BATTLE_TEST("Fists of Fury AI recognizes an opposing Apex holder's accuracy without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_ANNIHILAPE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { MOVE(player, MOVE_MEGA_PUNCH); EXPECT_MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_FISTS_OF_FURY); EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_0][B_BATTLER_1][MOVESLOT_0], 100); }
}
