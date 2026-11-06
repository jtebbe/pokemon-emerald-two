#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static EWRAM_DATA struct SimulatedDamage sDuckDamage;
static EWRAM_DATA bool32 sDuckSeen;
static EWRAM_DATA s32 sDuckCritScore;
static EWRAM_DATA enum Move sDuckObservedMove;
static s32 ObserveDuck(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == sDuckObservedMove
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sDuckSeen = TRUE;
        sDuckDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        if (gAiLogicData->abilities[attacker] == ABILITY_NOBLESSE_OBLIGE)
            sDuckCritScore = AI_GetNoblesseObligeCritScore(attacker, defender, move);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Leek Master AI predicts Huge Power's physical boost without boosting special attacks", s16 damage)
{
    enum Item item;
    enum Move move = MOVE_SCRATCH;
    u32 attack = 100;
    PARAMETRIZE { item = ITEM_NONE; attack = 200; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; attack = 100; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_NONE; attack = 100; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; attack = 100; move = MOVE_WATER_GUN; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_FARFETCHD) { Item(item); Ability(ABILITY_KEEN_EYE); Attack(attack); SpAttack(100); Moves(move); }
    } WHEN {
        sDuckSeen = FALSE; sDuckObservedMove = move; sDynamicAiFunc = ObserveDuck;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE { HP_BAR(player, captureDamage: &results[i].damage); } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDuckSeen);
        EXPECT_GE(results[i].damage, sDuckDamage.minimum);
        EXPECT_LE(results[i].damage, sDuckDamage.maximum);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Farfetch'd Apex AI predicts guaranteed critical damage and critical protection")
{
    enum Species species = SPECIES_FARFETCHD;
    enum Move move = MOVE_SCRATCH;
    enum Ability protection = ABILITY_NONE;
    const enum Ability protections[] = { ABILITY_NONE, ABILITY_BATTLE_ARMOR, ABILITY_SHELL_ARMOR, ABILITY_BATTLE_KING };
    for (u32 j = 0; j < ARRAY_COUNT(protections); j++) {
        PARAMETRIZE { species = SPECIES_FARFETCHD; move = MOVE_SCRATCH; protection = protections[j]; }
        PARAMETRIZE { species = SPECIES_FARFETCHD; move = MOVE_WATER_GUN; protection = protections[j]; }
        PARAMETRIZE { species = SPECIES_SIRFETCHD; move = MOVE_SLASH; protection = protections[j]; }
        PARAMETRIZE { species = SPECIES_FARFETCHD_GALAR; move = MOVE_AIR_CUTTER; protection = protections[j]; }
    }
    s16 damage;
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(protection); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(species) { Item(ITEM_APEX_METEORITE); Moves(move); }
    } WHEN {
        sDuckSeen = FALSE; sDuckObservedMove = move; sDynamicAiFunc = ObserveDuck;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        if (protection == ABILITY_NONE) MESSAGE("A critical hit!");
        else NOT MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDuckSeen);
        EXPECT_GE(damage, sDuckDamage.minimum);
        EXPECT_LE(damage, sDuckDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(1), species == SPECIES_FARFETCHD ? ABILITY_LEEK_MASTER : ABILITY_NOBLESSE_OBLIGE);
    }
}

AI_SINGLE_BATTLE_TEST("Leek Master AI avoids redundant Focus Energy and Laser Focus")
{
    enum Move setup;
    PARAMETRIZE { setup = MOVE_FOCUS_ENERGY; }
    PARAMETRIZE { setup = MOVE_LASER_FOCUS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(setup, MOVE_SLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); EXPECT_MOVE(opponent, MOVE_SLASH); }
        TURN { SCORE_LT(opponent, setup, MOVE_SLASH); EXPECT_MOVE(opponent, MOVE_SLASH); }
    }
}

AI_SINGLE_BATTLE_TEST("Leek Master AI still accounts for move accuracy")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_FARFETCHD) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_HYDRO_PUMP); } } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[1][0][MOVESLOT_0], GetMoveAccuracy(MOVE_HYDRO_PUMP));
    }
}

AI_SINGLE_BATTLE_TEST("Noblesse Oblige AI values defense drops and accounts for protection and harmful reactions")
{
    enum Ability ability;
    enum Item item = ITEM_NONE;
    s32 sign = 0;
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_NONE; sign = 1; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; sign = 1; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; sign = 1; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; sign = 0; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; sign = 0; }
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; item = ITEM_NONE; sign = 0; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; item = ITEM_NONE; sign = -1; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; item = ITEM_NONE; sign = -1; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; item = ITEM_NONE; sign = -1; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; item = ITEM_NONE; sign = -1; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SLASH, MOVE_WATER_GUN); }
    } WHEN {
        sDuckSeen = FALSE; sDuckObservedMove = MOVE_SLASH; sDynamicAiFunc = ObserveDuck;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDuckSeen);
        if (sign > 0) EXPECT_GT(sDuckCritScore, 0);
        else if (sign < 0) EXPECT_LT(sDuckCritScore, 0);
        else EXPECT_EQ(sDuckCritScore, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Noblesse Oblige AI weights its stat-drop reward by the move's critical chance", s32 score)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SLASH; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Moves(move, MOVE_WATER_GUN); }
    } WHEN {
        sDuckSeen = FALSE; sDuckObservedMove = move; sDynamicAiFunc = ObserveDuck;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDuckSeen);
        results[i].score = sDuckCritScore;
    } FINALLY {
        EXPECT_GT(results[0].score, 0);
        EXPECT_EQ(results[1].score, results[0].score * 2);
    }
}

AI_SINGLE_BATTLE_TEST("Noblesse Oblige AI does not reward stat drops from fixed damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        sDuckSeen = FALSE; sDuckObservedMove = MOVE_SEISMIC_TOSS; sDynamicAiFunc = ObserveDuck;
        TURN { EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDuckSeen);
        EXPECT_EQ(sDuckCritScore, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Noblesse Oblige AI evaluates its ally's entry boosts including Contrary and stat caps")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} } THEN {
        s32 score = AI_GetNoblesseObligeEntryScore(B_BATTLER_1);
        if (ability == ABILITY_CONTRARY) EXPECT_LT(score, 0);
        else EXPECT_GT(score, 0);
        opponentRight->statStages[STAT_DEF] = opponentRight->statStages[STAT_SPDEF] = ability == ABILITY_CONTRARY ? MIN_STAT_STAGE : MAX_STAT_STAGE;
        EXPECT_EQ(AI_GetNoblesseObligeEntryScore(B_BATTLER_1), 0);
    }
}

static EWRAM_DATA bool32 sDuckCandidateSeen[PARTY_SIZE];
static EWRAM_DATA s32 sDuckCandidateEntry[PARTY_SIZE];
static void ObserveDuckCandidate(struct SwitchAiContext *context)
{
    if (context->battler != B_BATTLER_1 || context->partyMonIndex < 2)
        return;
    u32 slot = context->partyMonIndex;
    sDuckCandidateSeen[slot] = TRUE;
    sDuckCandidateEntry[slot] = gAiLogicData->abilities[context->battler] == ABILITY_NOBLESSE_OBLIGE
        ? AI_GetNoblesseObligeEntryScore(context->battler) : 0;
}

AI_DOUBLE_BATTLE_TEST("Noblesse Oblige switch AI evaluates its ally benefit without leaking simulated boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_QUICK_ATTACK); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Speed(2); Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_SIRFETCHD) { Speed(2); Moves(MOVE_SLASH); }
    } WHEN {
        memset(sDuckCandidateSeen, 0, sizeof(sDuckCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveDuckCandidate;
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); EXPECT_SEND_OUT(opponentLeft, 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sDuckCandidateSeen[2]); EXPECT(sDuckCandidateSeen[3]);
        EXPECT_GT(sDuckCandidateEntry[2], 0);
        EXPECT_EQ(sDuckCandidateEntry[3], 0);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
