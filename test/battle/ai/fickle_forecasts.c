#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
// Keep observer buffers out of IWRAM so recursive AI forecasts retain stack room.
static EWRAM_DATA struct SimulatedDamage sFickleDamage[MAX_BATTLERS_COUNT];
static EWRAM_DATA bool32 sFickleSeen[MAX_BATTLERS_COUNT];
static EWRAM_DATA u32 sFickleWeather;
static EWRAM_DATA enum Species sFickleForm;
static s32 ObserveFickleDamage(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && gBattleResults.battleTurnCounter == 0
     && !gAiLogicData->aiPredictionInProgress)
    {
        sFickleSeen[defender] = TRUE;
        sFickleDamage[defender] = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sFickleWeather = gBattleWeather;
        sFickleForm = gBattleMons[attacker].species;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Fickle Forecasts AI predicts Weather Ball using the actual entry weather and form", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); SpAttack(100); Moves(MOVE_WEATHER_BALL); }
    } WHEN {
        memset(sFickleSeen, 0, sizeof(sFickleSeen));
        sDynamicAiFunc = ObserveFickleDamage;
        TURN { EXPECT_MOVE(opponent, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFickleSeen[0]);
        EXPECT_GE(results[i].damage, sFickleDamage[0].minimum);
        EXPECT_LE(results[i].damage, sFickleDamage[0].maximum);
        EXPECT_NE(sFickleForm, SPECIES_CASTFORM_NORMAL);
        EXPECT_NE(sFickleWeather, gBattleWeather);
    }
}

AI_DOUBLE_BATTLE_TEST("Fickle Forecasts AI predicts both Weather Ball targets including spread damage", s16 left; s16 right)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CHARIZARD) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        memset(sFickleSeen, 0, sizeof(sFickleSeen));
        sDynamicAiFunc = ObserveFickleDamage;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].left);
        HP_BAR(playerRight, captureDamage: &results[i].right);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFickleSeen[0]); EXPECT(sFickleSeen[2]);
        EXPECT_GE(results[i].left, sFickleDamage[0].minimum);
        EXPECT_LE(results[i].left, sFickleDamage[0].maximum);
        EXPECT_GE(results[i].right, sFickleDamage[2].minimum);
        EXPECT_LE(results[i].right, sFickleDamage[2].maximum);
        EXPECT_EQ(AI_GetBattlerMoveTargetType(1, MOVE_WEATHER_BALL), TARGET_BOTH);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("Fickle Forecasts AI recognizes Weather Ball when deciding whether Wide Guard is useful")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); Moves(MOVE_WEATHER_BALL); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_WIDE_GUARD, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); }
        TURN { MOVE(playerLeft, MOVE_WEATHER_BALL, target: opponentLeft); SCORE_GT(opponentLeft, MOVE_WIDE_GUARD, MOVE_CELEBRATE, target: playerLeft); EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(0), ABILITY_FICKLE_FORECASTS);
    }
}

AI_DOUBLE_BATTLE_TEST("Fickle Forecasts AI only predicts Storm Drain immunity for the absorbing Weather Ball target", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        // Drought acts first; the default highest legal weather roll then
        // chooses rain for Castform, exercising Water-type spread targeting.
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); Speed(100); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_GASTRODON) { Ability(ABILITY_STORM_DRAIN); Speed(50); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_CELEBRATE); }
    } WHEN {
        memset(sFickleSeen, 0, sizeof(sFickleSeen));
        sDynamicAiFunc = ObserveFickleDamage;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
        NOT HP_BAR(playerRight);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sFickleSeen[0]); EXPECT(sFickleSeen[2]);
        EXPECT_EQ(sFickleWeather, B_WEATHER_RAIN_NORMAL);
        EXPECT_GT(sFickleDamage[0].minimum, 0);
        EXPECT_GE(results[i].damage, sFickleDamage[0].minimum);
        EXPECT_LE(results[i].damage, sFickleDamage[0].maximum);
        EXPECT_EQ(sFickleDamage[2].maximum, 0);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}

static EWRAM_DATA bool32 sFickleCandidateSeen[PARTY_SIZE];
static EWRAM_DATA struct SimulatedDamage sFickleIncoming[PARTY_SIZE], sFickleOutgoing[PARTY_SIZE];
static EWRAM_DATA uq4_12_t sFickleIncomingEffect[PARTY_SIZE], sFickleOutgoingEffect[PARTY_SIZE];
static EWRAM_DATA u32 sFickleCandidateWeather[PARTY_SIZE];
static EWRAM_DATA enum Species sFickleCandidateForm[PARTY_SIZE];
static void ObserveFickleCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sFickleCandidateSeen[slot] = TRUE;
    sFickleIncoming[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][1];
    sFickleOutgoing[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0];
    sFickleIncomingEffect[slot] = gAiLogicData->effectiveness[context->opposingBattler][context->battler][1];
    sFickleOutgoingEffect[slot] = gAiLogicData->effectiveness[context->battler][context->opposingBattler][0];
    sFickleCandidateWeather[slot] = gBattleWeather;
    sFickleCandidateForm[slot] = gBattleMons[context->battler].species;
}

AI_SINGLE_BATTLE_TEST("Fickle Forecasts switch AI averages legal entry weather and restores the field and candidate forms")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SCIZOR) { Ability(rain ? ABILITY_DRIZZLE : ABILITY_SWARM); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); SpDefense(100); SpAttack(100); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); HP(1000); MaxHP(1000); Speed(2); SpDefense(100); SpAttack(100); Moves(MOVE_WEATHER_BALL); }
    } WHEN {
        memset(sFickleCandidateSeen, 0, sizeof(sFickleCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveFickleCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sFickleCandidateSeen[1]); EXPECT(sFickleCandidateSeen[2]);
        // Ice/Water/Fire are 0.5x/1x/4x against Scizor. Existing rain excludes
        // Water from the draw rather than being treated as the next weather.
        EXPECT_EQ(sFickleOutgoingEffect[1], rain ? UQ_4_12(2.25) : (UQ_4_12(0.5) + UQ_4_12(1.0) + UQ_4_12(4.0)) / 3);
        EXPECT_EQ(sFickleIncomingEffect[1], rain ? UQ_4_12(1.25) : UQ_4_12(1.0));
        EXPECT_LT(sFickleIncoming[1].minimum, sFickleIncoming[1].median);
        EXPECT_GT(sFickleIncoming[1].maximum, sFickleIncoming[1].median);
        EXPECT_GT(sFickleOutgoing[1].median, 0);
        for (u32 slot = 1; slot <= 2; slot++) {
            EXPECT_EQ(sFickleCandidateWeather[slot], rain ? B_WEATHER_RAIN_NORMAL : B_WEATHER_NONE);
            EXPECT_EQ(sFickleCandidateForm[slot], SPECIES_CASTFORM_NORMAL);
        }
        EXPECT(!gAiLogicData->fickleForecastsAveraging);
    }
}

AI_SINGLE_BATTLE_TEST("Fickle Forecasts switch AI respects Cloud Nine when forecasting weather forms and Weather Ball")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SCIZOR) { Ability(ABILITY_CLOUD_NINE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); SpDefense(100); SpAttack(100); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); HP(1000); MaxHP(1000); Speed(2); SpDefense(100); SpAttack(100); Moves(MOVE_WEATHER_BALL); }
    } WHEN {
        memset(sFickleCandidateSeen, 0, sizeof(sFickleCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveFickleCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sFickleCandidateSeen[1]); EXPECT(sFickleCandidateSeen[2]);
        EXPECT_EQ(sFickleOutgoing[1].median, sFickleOutgoing[2].median);
        EXPECT_EQ(sFickleIncoming[1].median, sFickleIncoming[2].median);
        EXPECT_EQ(sFickleOutgoingEffect[1], UQ_4_12(0.5));
        EXPECT_EQ(sFickleIncomingEffect[1], UQ_4_12(1.0));
        EXPECT_EQ(sFickleCandidateWeather[1], B_WEATHER_NONE);
        EXPECT_EQ(sFickleCandidateForm[1], SPECIES_CASTFORM_NORMAL);
    }
}

AI_SINGLE_BATTLE_TEST("Fickle Forecasts switch AI predicts the existing locked weather and matching form")
{
    bool32 primal;
    PARAMETRIZE { primal = FALSE; }
    PARAMETRIZE { primal = TRUE; }
    ResetStartingStatuses();
    if (!primal) SetStartingStatus(STARTING_STATUS_WEATHER_SUN);
    GIVEN {
        WITH_CONFIG(B_OVERWORLD_WEATHER_OVERRIDE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SCIZOR) { Ability(primal ? ABILITY_DESOLATE_LAND : ABILITY_SWARM); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_CASTFORM_NORMAL) { Ability(ABILITY_FORECAST); HP(1000); MaxHP(1000); Speed(2); Moves(MOVE_WEATHER_BALL); }
    } WHEN {
        memset(sFickleCandidateSeen, 0, sizeof(sFickleCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveFickleCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sFickleCandidateSeen[1]);
        EXPECT_EQ(sFickleOutgoingEffect[1], UQ_4_12(4.0));
        EXPECT_EQ(sFickleIncomingEffect[1], UQ_4_12(0.5));
        EXPECT_EQ(sFickleCandidateWeather[1], primal ? B_WEATHER_SUN_PRIMAL : B_WEATHER_SUN_NORMAL);
        EXPECT_EQ(sFickleCandidateForm[1], SPECIES_CASTFORM_NORMAL);
        ResetStartingStatuses();
    }
}
