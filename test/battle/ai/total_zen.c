#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Total Zen AI avoids absorbed Psychic moves Attract Captivate and Taunt even against Mold Breaker")
{
    enum Move move = MOVE_PSYCHIC;
    enum Ability ability = ABILITY_NONE;
    const enum Move moves[] = { MOVE_PSYCHIC, MOVE_HYPNOSIS, MOVE_ATTRACT, MOVE_CAPTIVATE, MOVE_TAUNT };
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; ability = ABILITY_MOLD_BREAKER; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); Gender(MON_MALE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Gender(MON_FEMALE); Moves(move, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, move, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_TOTAL_ZEN);
        if (move == MOVE_PSYCHIC)
            EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum, 0);
    }
}

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sZenDamage[2];
static bool32 sZenObserved[2];
static s32 ObserveZenDamage(u32 attacker, u32 defender, u32 move, s32 score)
{
    u32 turn = gBattleResults.battleTurnCounter;
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && turn < ARRAY_COUNT(sZenDamage)
     && !gAiLogicData->aiPredictionInProgress)
    {
        sZenDamage[turn] = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sZenObserved[turn] = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Total Zen AI predicts Unaware physical and special damage in both directions", s16 first; s16 second)
{
    bool32 zenAttacker, physical;
    PARAMETRIZE { zenAttacker = FALSE; physical = TRUE; }
    PARAMETRIZE { zenAttacker = FALSE; physical = FALSE; }
    PARAMETRIZE { zenAttacker = TRUE; physical = TRUE; }
    PARAMETRIZE { zenAttacker = TRUE; physical = FALSE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(zenAttacker ? SPECIES_WOBBUFFET : SPECIES_SLOWBRO) { Item(zenAttacker ? ITEM_NONE : ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Defense(100); SpDefense(100); }
        OPPONENT(zenAttacker ? SPECIES_SLOWBRO : SPECIES_WOBBUFFET) { Item(zenAttacker ? ITEM_APEX_METEORITE : ITEM_NONE); Speed(1); Attack(100); SpAttack(100); Moves(physical ? MOVE_SCRATCH : MOVE_SURF); }
    } WHEN {
        sZenObserved[0] = sZenObserved[1] = FALSE;
        sDynamicAiFunc = ObserveZenDamage;
        if (zenAttacker) {
            TURN { MOVE(player, physical ? MOVE_IRON_DEFENSE : MOVE_AMNESIA); }
            TURN { MOVE(player, physical ? MOVE_IRON_DEFENSE : MOVE_AMNESIA); }
        } else {
            TURN { MOVE(player, MOVE_DECORATE, target: opponent); }
            TURN { MOVE(player, MOVE_DECORATE, target: opponent); }
        }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].first);
        HP_BAR(player, captureDamage: &results[i].second);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sZenObserved[0]); EXPECT(sZenObserved[1]);
        EXPECT_GE(results[i].first, sZenDamage[0].minimum); EXPECT_LE(results[i].first, sZenDamage[0].maximum);
        EXPECT_GE(results[i].second, sZenDamage[1].minimum); EXPECT_LE(results[i].second, sZenDamage[1].maximum);
        EXPECT_EQ(sZenDamage[0].median, sZenDamage[1].median);
        EXPECT_EQ(results[i].first, results[i].second);
    }
}

AI_SINGLE_BATTLE_TEST("Total Zen AI recognizes Unaware when deciding whether to set up")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BELLY_DRUM, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_BELLY_DRUM, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(AI_HasUnawareOnSide(B_BATTLER_0));
        EXPECT(!ShouldRaiseAnyStat(B_BATTLER_1, B_BATTLER_0));
    }
}

AI_DOUBLE_BATTLE_TEST("Total Zen AI deliberately feeds an ally a Psychic move when its special boosts are useful")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_UMBREON) { HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_UMBREON) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SCORE_GT_VAL(opponentLeft, MOVE_PSYCHIC, AI_SCORE_DEFAULT, target: opponentRight); EXPECT_MOVE(opponentLeft, MOVE_PSYCHIC, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        opponentRight->statStages[STAT_SPATK] = MAX_STAT_STAGE;
        EXPECT(ShouldTriggerAbility(B_BATTLER_1, B_BATTLER_3, ABILITY_TOTAL_ZEN));
        opponentRight->statStages[STAT_SPDEF] = MAX_STAT_STAGE;
        EXPECT(!ShouldTriggerAbility(B_BATTLER_1, B_BATTLER_3, ABILITY_TOTAL_ZEN));
    }
}

static bool32 sZenCandidateSeen[PARTY_SIZE];
static u32 sZenPsychicDamage[PARTY_SIZE], sZenOtherDamage[PARTY_SIZE];
static void ObserveZenCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sZenCandidateSeen[slot] = TRUE;
    sZenPsychicDamage[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][1].maximum;
    sZenOtherDamage[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][0].median;
}

AI_SINGLE_BATTLE_TEST("Total Zen switch AI predicts bench Psychic absorption without leaking immunity to other candidates")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_SLOWBRO) { Ability(ABILITY_OWN_TEMPO); HP(1000); MaxHP(1000); Speed(2); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sZenCandidateSeen[1] = sZenCandidateSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveZenCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sZenCandidateSeen[1]); EXPECT(sZenCandidateSeen[2]);
        EXPECT_EQ(sZenPsychicDamage[1], 0);
        EXPECT_GT(sZenPsychicDamage[2], 0);
        EXPECT_GT(sZenOtherDamage[1], 0);
        EXPECT_EQ(sZenOtherDamage[1], sZenOtherDamage[2]);
    }
}

AI_SINGLE_BATTLE_TEST("Total Zen smart switching seeks a Psychic absorber even against Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PICKUP; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ability); Moves(MOVE_PSYCHIC, MOVE_STRENGTH); }
        OPPONENT(SPECIES_ZIGZAGOON) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_PSYCHIC); EXPECT_SWITCH(opponent, 1); }
    }
}
