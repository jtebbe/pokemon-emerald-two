#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_controllers.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sTechnicolorDamage[2][MAX_BATTLERS_COUNT];
static enum Type sTechnicolorTypes[2][2];
static bool32 sTechnicolorSeen[2][MAX_BATTLERS_COUNT];
static s32 ObserveTechnicolor(u32 attacker, u32 defender, u32 move, s32 score)
{
    u32 turn = gBattleResults.battleTurnCounter;
    if (attacker == B_BATTLER_1 && turn < 2 && !gAiLogicData->aiPredictionInProgress)
    {
        sTechnicolorDamage[turn][defender] = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sTechnicolorSeen[turn][defender] = TRUE;
        sTechnicolorTypes[turn][0] = gBattleMons[B_BATTLER_0].types[0];
        sTechnicolorTypes[turn][1] = gBattleMons[B_BATTLER_0].types[1];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Technicolor AI predicts converted damage and selects it over an ineffective ordinary move", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); Moves(MOVE_STRENGTH, MOVE_THUNDERBOLT); }
    } WHEN {
        memset(sTechnicolorSeen, 0, sizeof(sTechnicolorSeen));
        sDynamicAiFunc = ObserveTechnicolor;
        TURN { SCORE_GT(opponent, MOVE_STRENGTH, MOVE_THUNDERBOLT); EXPECT_MOVE(opponent, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTechnicolorSeen[0][0]);
        // The observer sees the final candidate (Thunderbolt); the stored move
        // forecasts themselves retain their independent resolved types.
        struct SimulatedDamage strength = gAiLogicData->simulatedDmg[1][0][0];
        EXPECT_GE(results[i].damage, strength.minimum);
        EXPECT_LE(results[i].damage, strength.maximum);
        EXPECT_EQ(gAiLogicData->simulatedDmg[1][0][1].maximum, 0);
        EXPECT_EQ(gLastHitByType[0], TYPE_GRASS);
    }
}

AI_SINGLE_BATTLE_TEST("Technicolor AI predicts the observed defensive pair each turn without changing it", s16 first; s16 second)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        memset(sTechnicolorSeen, 0, sizeof(sTechnicolorSeen));
        sDynamicAiFunc = ObserveTechnicolor;
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].first);
        HP_BAR(player, captureDamage: &results[i].second);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTechnicolorSeen[0][0]); EXPECT(sTechnicolorSeen[1][0]);
        EXPECT_GE(results[i].first, sTechnicolorDamage[0][0].minimum);
        EXPECT_LE(results[i].first, sTechnicolorDamage[0][0].maximum);
        EXPECT_GE(results[i].second, sTechnicolorDamage[1][0].minimum);
        EXPECT_LE(results[i].second, sTechnicolorDamage[1][0].maximum);
        for (u32 first = 0; first < 2; first++)
            for (u32 second = 0; second < 2; second++) {
                EXPECT_NE(sTechnicolorTypes[0][first], sTechnicolorTypes[1][second]);
                EXPECT_NE(sTechnicolorTypes[1][first], player->types[second]);
            }
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(0), ABILITY_TECHNICOLOR);
    }
}

AI_SINGLE_BATTLE_TEST("Technicolor AI avoids absorbed candidate types when forecasting Normal attacks", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_EARTH_EATER); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Moves(MOVE_STRENGTH); }
    } WHEN {
        memset(sTechnicolorSeen, 0, sizeof(sTechnicolorSeen));
        sDynamicAiFunc = ObserveTechnicolor;
        TURN { EXPECT_MOVE(opponent, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTechnicolorSeen[0][0]);
        EXPECT_GE(results[i].damage, sTechnicolorDamage[0][0].minimum);
        EXPECT_LE(results[i].damage, sTechnicolorDamage[0][0].maximum);
        EXPECT_EQ(gLastHitByType[0], TYPE_FIGHTING);
    }
}

AI_DOUBLE_BATTLE_TEST("Technicolor AI forecasts spread damage with a different resolved type for each target", s16 left; s16 right)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CHARIZARD) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        memset(sTechnicolorSeen, 0, sizeof(sTechnicolorSeen));
        sDynamicAiFunc = ObserveTechnicolor;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].left);
        HP_BAR(playerRight, captureDamage: &results[i].right);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTechnicolorSeen[0][0]); EXPECT(sTechnicolorSeen[0][2]);
        EXPECT_GE(results[i].left, sTechnicolorDamage[0][0].minimum);
        EXPECT_LE(results[i].left, sTechnicolorDamage[0][0].maximum);
        EXPECT_GE(results[i].right, sTechnicolorDamage[0][2].minimum);
        EXPECT_LE(results[i].right, sTechnicolorDamage[0][2].maximum);
        EXPECT_EQ(gLastHitByType[0], TYPE_ROCK);
        EXPECT_EQ(gLastHitByType[2], TYPE_GRASS);
    }
}

static bool32 sTechnicolorCandidateSeen[PARTY_SIZE];
static struct SimulatedDamage sTechnicolorIncoming[PARTY_SIZE], sTechnicolorOutgoing[PARTY_SIZE];
static uq4_12_t sTechnicolorCandidateEffect[PARTY_SIZE];
static enum Type sTechnicolorCandidateTypes[PARTY_SIZE][2], sTechnicolorHistory[PARTY_SIZE][2];
static void ObserveTechnicolorCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sTechnicolorCandidateSeen[slot] = TRUE;
    sTechnicolorIncoming[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][1];
    sTechnicolorOutgoing[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0];
    sTechnicolorCandidateEffect[slot] = gAiLogicData->effectiveness[context->battler][context->opposingBattler][0];
    sTechnicolorCandidateTypes[slot][0] = gBattleMons[context->battler].types[0];
    sTechnicolorCandidateTypes[slot][1] = gBattleMons[context->battler].types[1];
    const enum Type *history = gBattleStruct->technicolorTypes[GetBattlerTrainer(context->battler)][slot];
    sTechnicolorHistory[slot][0] = history[0];
    sTechnicolorHistory[slot][1] = history[1];
}

AI_SINGLE_BATTLE_TEST("Technicolor switch AI averages unknown entry types and restores candidate typing and history")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_REVELATION_DANCE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_JELLICENT) { Ability(ABILITY_CURSED_BODY); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SHADOW_SNEAK, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); Moves(move); }
        OPPONENT(SPECIES_KECLEON) { Ability(ABILITY_COLOR_CHANGE); HP(1000); MaxHP(1000); Speed(2); Moves(move); }
    } WHEN {
        memset(sTechnicolorCandidateSeen, 0, sizeof(sTechnicolorCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveTechnicolorCandidate;
        TURN { MOVE(player, MOVE_SHADOW_SNEAK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTechnicolorCandidateSeen[1]); EXPECT(sTechnicolorCandidateSeen[2]);
        EXPECT_LT(sTechnicolorIncoming[1].minimum, sTechnicolorIncoming[2].minimum);
        EXPECT_GT(sTechnicolorIncoming[1].maximum, sTechnicolorIncoming[2].maximum);
        EXPECT_GT(sTechnicolorIncoming[1].median, sTechnicolorIncoming[1].minimum);
        EXPECT_LT(sTechnicolorIncoming[1].median, sTechnicolorIncoming[1].maximum);
        if (move == MOVE_TACKLE) {
            EXPECT_GT(sTechnicolorOutgoing[1].minimum, 0);
            EXPECT_EQ(sTechnicolorCandidateEffect[1], UQ_4_12(2.0));
        } else {
            // The 17 eligible primary types total 17x effectiveness against
            // Water/Ghost. Both slot orders must have equal probability.
            EXPECT_EQ(sTechnicolorCandidateEffect[1], UQ_4_12(1.0));
            EXPECT_EQ(sTechnicolorOutgoing[1].minimum, 0);
            EXPECT_GT(sTechnicolorOutgoing[1].median, 0);
        }
        EXPECT_EQ(sTechnicolorOutgoing[2].maximum, 0);
        EXPECT_EQ(sTechnicolorCandidateTypes[1][0], TYPE_NORMAL);
        EXPECT_EQ(sTechnicolorCandidateTypes[1][1], TYPE_NORMAL);
        EXPECT_EQ(sTechnicolorCandidateTypes[2][0], TYPE_NORMAL);
        EXPECT_EQ(sTechnicolorCandidateTypes[2][1], TYPE_NORMAL);
        // Forecasting leaves each benched Kecleon's remembered pair untouched.
        EXPECT_EQ(sTechnicolorHistory[1][0], TYPE_NONE);
        EXPECT_EQ(sTechnicolorHistory[1][1], TYPE_NONE);
        EXPECT_EQ(sTechnicolorHistory[2][0], TYPE_NONE);
        EXPECT_EQ(sTechnicolorHistory[2][1], TYPE_NONE);
        EXPECT(!gAiLogicData->technicolorAveraging);
    }
}
