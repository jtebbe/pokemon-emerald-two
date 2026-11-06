#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sFrostfeltDamage;
static bool32 sFrostfeltObserved;
static s32 sFrostfeltScore;
static u32 sFrostfeltTurn;
static s32 ObserveFrostfelt(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sFrostfeltTurn && !gAiLogicData->aiPredictionInProgress)
    {
        sFrostfeltObserved = TRUE;
        sFrostfeltDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sFrostfeltScore = score;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Frostfelt Hatred AI predicts half damage only after an earlier damaging hit", s16 damage)
{
    enum Move move = MOVE_WATER_GUN, incoming = MOVE_CELEBRATE;
    bool32 first = TRUE;
    const enum Move moves[] = { MOVE_BITE, MOVE_WATER_GUN, MOVE_DRAGON_RAGE };
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; incoming = MOVE_CELEBRATE; first = TRUE; }
        PARAMETRIZE { move = moves[j]; incoming = MOVE_TACKLE; first = TRUE; }
        PARAMETRIZE { move = moves[j]; incoming = MOVE_TACKLE; first = FALSE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(first ? 100 : 1); Defense(100); SpDefense(100); Moves(incoming); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(first ? 1 : 100); Attack(100); SpAttack(100); Moves(move); }
    } WHEN {
        sFrostfeltObserved = FALSE; sFrostfeltTurn = 0; sDynamicAiFunc = ObserveFrostfelt;
        TURN { MOVE(player, incoming); EXPECT_MOVE(opponent, move, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFrostfeltObserved);
        EXPECT_GE(results[i].damage, sFrostfeltDamage.minimum);
        EXPECT_LE(results[i].damage, sFrostfeltDamage.maximum);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
            EXPECT_EQ(results[j * 3].damage / 2, results[j * 3 + 1].damage);
            EXPECT_EQ(results[j * 3].damage, results[j * 3 + 2].damage);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Frostfelt Hatred AI chooses priority to avoid an earlier hit's damage penalty")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(100); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Attack(100); SpAttack(100); Speed(1); Moves(MOVE_AQUA_JET, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); SCORE_GT(opponent, MOVE_AQUA_JET, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_AQUA_JET); }
    }
}

AI_SINGLE_BATTLE_TEST("Frostfelt Hatred AI values PP draining on a selected status move only when PP can be lost", s32 score)
{
    u32 pp;
    PARAMETRIZE { pp = 1; }
    PARAMETRIZE { pp = 20; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); MovesWithPP({MOVE_CELEBRATE, pp}); }
        OPPONENT(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sFrostfeltObserved = FALSE; sFrostfeltTurn = 0; sDynamicAiFunc = ObserveFrostfelt;
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFrostfeltObserved);
        results[i].score = sFrostfeltScore;
    } FINALLY {
        EXPECT_GT(results[1].score, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Frostfelt Hatred AI does not predict its damage penalty through Substitute", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_SINGLES, SCORE_TIE_LO, 0);
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_SUBSTITUTE, MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SUBSTITUTE); }
        sFrostfeltObserved = FALSE; sFrostfeltTurn = 1; sDynamicAiFunc = ObserveFrostfelt;
        TURN { MOVE(player, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFrostfeltObserved);
        EXPECT_EQ(results[i].damage, 40);
        EXPECT_EQ(sFrostfeltDamage.minimum, 40);
        EXPECT_EQ(sFrostfeltDamage.maximum, 40);
    }
}

static bool32 sFrostfeltCandidateSeen[PARTY_SIZE], sFrostfeltCandidateField[PARTY_SIZE];
static u32 sFrostfeltCandidateDamage[PARTY_SIZE];
static void ObserveFrostfeltCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sFrostfeltCandidateSeen[slot] = TRUE;
    sFrostfeltCandidateField[slot] = gAiLogicData->hasFrostfeltHatred;
    sFrostfeltCandidateDamage[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][1].median;
}

AI_SINGLE_BATTLE_TEST("Frostfelt Hatred switch AI forecasts its counterattack reduction without leaking it between candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_AQUA_JET, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_ZIGZAGOON) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); SpDefense(100); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_FROSLASS) { Ability(ABILITY_SNOW_CLOAK); HP(1000); MaxHP(1000); Speed(100); SpDefense(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sFrostfeltCandidateSeen[1] = sFrostfeltCandidateSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveFrostfeltCandidate;
        TURN { MOVE(player, MOVE_AQUA_JET); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sFrostfeltCandidateSeen[1]); EXPECT(sFrostfeltCandidateSeen[2]);
        EXPECT(sFrostfeltCandidateField[1]); EXPECT(!sFrostfeltCandidateField[2]);
        EXPECT_EQ(sFrostfeltCandidateDamage[2] / 2, sFrostfeltCandidateDamage[1]);
    }
}
