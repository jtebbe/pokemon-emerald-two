#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_field_statuses.h"

extern AiScoreFunc sDynamicAiFunc;
static EWRAM_DATA struct SimulatedDamage sAdaptiveDamage;
static EWRAM_DATA bool32 sAdaptiveSeen, sAdaptiveIncoming;

static s32 ObserveAdaptiveBuild(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 1 && !gAiLogicData->aiPredictionInProgress)
    {
        sAdaptiveSeen = TRUE;
        sAdaptiveDamage = sAdaptiveIncoming
            ? gAiLogicData->simulatedDmg[defender][attacker][MOVESLOT_0]
            : gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Adaptive Build AI forecasts offensive stats defensive stats and Thick Fat in each weather")
{
    static const enum Move weathers[] = {MOVE_CELEBRATE, MOVE_RAIN_DANCE, MOVE_SNOWSCAPE};
    static const enum Move moves[] = {MOVE_TACKLE, MOVE_WATER_GUN, MOVE_INCINERATE, MOVE_ICE_SPINNER};
    enum Move weather = MOVE_CELEBRATE, move = MOVE_TACKLE;
    bool32 incoming = FALSE;
    for (u32 w = 0; w < ARRAY_COUNT(weathers); w++)
        for (u32 m = 0; m < ARRAY_COUNT(moves); m++)
        {
            PARAMETRIZE { weather = weathers[w]; move = moves[m]; incoming = FALSE; }
            PARAMETRIZE { weather = weathers[w]; move = moves[m]; incoming = TRUE; }
        }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(100);
            Attack(100); SpAttack(100); Defense(100); SpDefense(100); Moves(move, weather);
        }
        OPPONENT(SPECIES_DEWGONG) {
            Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1);
            Attack(100); SpAttack(100); Defense(100); SpDefense(100); Moves(move);
        }
    } WHEN {
        TURN { MOVE(player, weather); }
        sAdaptiveSeen = FALSE;
        sAdaptiveIncoming = incoming;
        sDynamicAiFunc = ObserveAdaptiveBuild;
        TURN { MOVE(player, move, criticalHit: FALSE, secondaryEffect: FALSE); EXPECT_MOVE(opponent, move); }
    } SCENE {
        // The first turn includes an AI attack before the observation is installed.
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        if (incoming) {
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            HP_BAR(opponent, captureDamage: &damage);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, move, opponent);
            HP_BAR(player, captureDamage: &damage);
        }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sAdaptiveSeen);
        EXPECT_GE(damage, sAdaptiveDamage.minimum);
        EXPECT_LE(damage, sAdaptiveDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_ADAPTIVE_BUILD);
    }
}

static EWRAM_DATA bool32 sAdaptiveSnow, sAdaptiveRain;
static s32 ObserveAdaptiveWeather(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && !gAiLogicData->aiPredictionInProgress)
    {
        sAdaptiveSeen = TRUE;
        sAdaptiveSnow = ShouldSetWeather(attacker, B_WEATHER_SNOW);
        sAdaptiveRain = ShouldSetWeather(attacker, B_WEATHER_RAIN);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Adaptive Build AI values both snow and rain even with only a Normal attack")
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SEEL) { Ability(ABILITY_THICK_FAT); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Moves(MOVE_TACKLE); }
    } WHEN {
        sAdaptiveSeen = FALSE;
        sDynamicAiFunc = ObserveAdaptiveWeather;
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sAdaptiveSeen);
        EXPECT_EQ(sAdaptiveSnow, apex);
        EXPECT_EQ(sAdaptiveRain, apex);
    }
}
