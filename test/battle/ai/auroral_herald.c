#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sHeraldDamage;
static bool32 sHeraldObserved;
static s32 sHeraldScore;
static s32 ObserveHerald(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sHeraldDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sHeraldScore = score;
        sHeraldObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Auroral Herald AI predicts the Veil damage boost", s16 damage)
{
    enum Item item;
    enum Move move = MOVE_SCRATCH;
    const enum Move attacks[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(attacks); j++) {
        PARAMETRIZE { item = ITEM_NONE; move = attacks[j]; }
        PARAMETRIZE { item = ITEM_APEX_METEORITE; move = attacks[j]; }
    }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sHeraldObserved = FALSE;
        sDynamicAiFunc = ObserveHerald;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHeraldObserved);
        EXPECT_GE(results[i].damage, sHeraldDamage.minimum);
        EXPECT_LE(results[i].damage, sHeraldDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_AURORAL_HERALD : ABILITY_SNOW_CLOAK);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

static s32 ObserveHeraldPartner(u32 attacker, u32 defender, u32 move, s32 score)
{
    score = ObserveHerald(attacker, defender, move, score);
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0)
        return score + 20; // Keep predicted and actual damage on the same target.
    return score;
}

AI_DOUBLE_BATTLE_TEST("Auroral Herald AI predicts a partner's damage aura", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sHeraldObserved = FALSE;
        sDynamicAiFunc = ObserveHeraldPartner;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHeraldObserved);
        EXPECT_GE(results[i].damage, sHeraldDamage.minimum);
        EXPECT_LE(results[i].damage, sHeraldDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

AI_DOUBLE_BATTLE_TEST("Auroral Herald AI values frostbite only against susceptible targets", s32 score)
{
    enum Ability ability = ABILITY_SNOW_WARNING;
    enum Item item = ITEM_NONE;
    enum Species species = SPECIES_WOBBUFFET;
    PARAMETRIZE { ability = ABILITY_SNOW_WARNING; item = ITEM_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { ability = ABILITY_MAGMA_ARMOR; item = ITEM_NONE; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { ability = ABILITY_SNOW_WARNING; item = ITEM_COVERT_CLOAK; species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { ability = ABILITY_SNOW_WARNING; item = ITEM_NONE; species = SPECIES_ABOMASNOW; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        sHeraldObserved = FALSE;
        sDynamicAiFunc = ObserveHeraldPartner;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sHeraldObserved);
        results[i].score = sHeraldScore;
    } FINALLY {
        for (u32 j = 1; j < 5; j++) EXPECT_EQ(results[j].score + DECENT_EFFECT, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Auroral Herald AI does not waste a turn setting an already active Aurora Veil")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_AURORA_VEIL, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_AURORA_VEIL); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

static bool32 sHeraldCandidateObserved;
static u32 sHeraldCandidateTaken, sHeraldCandidateDealt;
static bool32 sHeraldCandidateLeaked;
static void ObserveHeraldCandidate(struct SwitchAiContext *context)
{
    if (gBattlerPartyIndexes[context->battler] != 0) return;
    sHeraldCandidateLeaked |= (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_AURORA_VEIL) != 0;
    if (context->partyMonIndex == 1)
    {
        sHeraldCandidateObserved = TRUE;
        sHeraldCandidateTaken = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][0].median;
        sHeraldCandidateDealt = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0].median;
    }
}

AI_SINGLE_BATTLE_TEST("Auroral Herald switch AI predicts entry Veil for offense and defense without leaking screens", u32 dealt, u32 taken)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(150); MaxHP(800); Attack(1500); Defense(100); Speed(3); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); HP(250); MaxHP(800); Attack(300); Defense(100); Speed(2); Moves(MOVE_STRENGTH); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); HP(250); MaxHP(800); Attack(300); Defense(100); Speed(2); Moves(MOVE_STRENGTH); }
    } WHEN {
        sHeraldCandidateObserved = FALSE;
        sHeraldCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveHeraldCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, item == ITEM_APEX_METEORITE ? 1 : 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sHeraldCandidateObserved);
        EXPECT(!sHeraldCandidateLeaked);
        results[i].dealt = sHeraldCandidateDealt;
        results[i].taken = sHeraldCandidateTaken;
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_AURORA_VEIL), item == ITEM_APEX_METEORITE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].dealt, UQ_4_12(1.3), results[1].dealt);
        EXPECT_MUL_EQ(results[1].taken, UQ_4_12(2.0), results[0].taken);
    }
}

AI_SINGLE_BATTLE_TEST("Auroral Herald switch AI cannot use entry Veil to survive fatal hazards")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(80); MaxHP(800); Attack(100); Speed(3); Moves(MOVE_STEALTH_ROCK, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(4); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(ITEM_HEAVY_DUTY_BOOTS); HP(100); MaxHP(800); Defense(100); Speed(1); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(ITEM_APEX_METEORITE); HP(100); MaxHP(800); Defense(100); Speed(1); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_AURORA_VEIL));
    }
}
