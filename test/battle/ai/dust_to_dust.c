#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sDustDamage;
static bool32 sDustObserved;
static u32 sDustTurn;
static u32 sDustLiveWeather;

static s32 ObserveDustToDust(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sDustTurn
     && !gAiLogicData->aiPredictionInProgress)
    {
        sDustDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sDustLiveWeather = gBattleWeather;
        sDustObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI predicts Rock STAB and does not apply sand before its own triggering attack")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_ROCK_THROW; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_POWER_GEM; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_MUD_SHOT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_POWER_GEM; item = ITEM_NONE; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_RHYHORN) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Item(item); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sDustObserved = FALSE;
        sDustTurn = 0;
        sDynamicAiFunc = ObserveDustToDust;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDustObserved);
        EXPECT_EQ(sDustLiveWeather, B_WEATHER_NONE);
        EXPECT_GE(damage, sDustDamage.minimum);
        EXPECT_LE(damage, sDustDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_DUST_TO_DUST : ABILITY_SAND_VEIL);
    }
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI predicts the virtual Rock Special Defense bonus in existing sand")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Item(item); HP(3000); MaxHP(3000); SpDefense(100); Speed(100); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); SpAttack(300); Speed(1); Moves(MOVE_SWIFT); }
    } WHEN {
        sDustObserved = FALSE;
        sDustTurn = 1;
        sDynamicAiFunc = ObserveDustToDust;
        TURN { MOVE(player, MOVE_SANDSTORM); }
        TURN { EXPECT_MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player);
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDustObserved);
        EXPECT(sDustLiveWeather & B_WEATHER_SANDSTORM);
        EXPECT_GE(damage, sDustDamage.minimum);
        EXPECT_LE(damage, sDustDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI predicts sand from a faster opponent before its own special attack")
{
    s16 damage;
    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); Speed(100); Moves(MOVE_MUD_SHOT); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); SpAttack(300); Speed(1); Moves(MOVE_SWIFT); }
    } WHEN {
        sDustObserved = FALSE;
        sDustTurn = 0;
        sDynamicAiFunc = ObserveDustToDust;
        TURN { MOVE(player, MOVE_MUD_SHOT); EXPECT_MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDustObserved);
        EXPECT_EQ(sDustLiveWeather, B_WEATHER_NONE);
        EXPECT_GE(damage, sDustDamage.minimum);
        EXPECT_LE(damage, sDustDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI does not predict sand from a slower opponent before its own attack")
{
    s16 damage;
    PASSES_RANDOMLY(PREDICT_MOVE_CHANCE, 100, RNG_AI_PREDICT_MOVE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); Speed(1); Moves(MOVE_MUD_SHOT); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); SpAttack(300); Speed(100); Moves(MOVE_SWIFT); }
    } WHEN {
        sDustObserved = FALSE;
        sDustTurn = 0;
        sDynamicAiFunc = ObserveDustToDust;
        TURN { MOVE(player, MOVE_MUD_SHOT); EXPECT_MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sDustObserved);
        EXPECT_EQ(sDustLiveWeather, B_WEATHER_NONE);
        EXPECT_GE(damage, sDustDamage.minimum);
        EXPECT_LE(damage, sDustDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI values free sand with a Ground status move")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SANDSLASH) { Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Item(item); Moves(MOVE_SAND_ATTACK, MOVE_SMOKESCREEN); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE) {
                SCORE_GT(opponent, MOVE_SAND_ATTACK, MOVE_SMOKESCREEN);
                EXPECT_MOVE(opponent, MOVE_SAND_ATTACK);
            } else {
                SCORE_EQ(opponent, MOVE_SAND_ATTACK, MOVE_SMOKESCREEN);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Dust to Dust AI does not reward sand setup in existing sand or unchangeable weather")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    ResetStartingStatuses();
    if (variant == 1) SetStartingStatus(STARTING_STATUS_WEATHER_SANDSTORM);
    if (variant == 2) SetStartingStatus(STARTING_STATUS_WEATHER_SUN);
    GIVEN {
        WITH_CONFIG(B_OVERWORLD_WEATHER_OVERRIDE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_GROUDON) { Ability(variant == 0 ? ABILITY_DESOLATE_LAND : ABILITY_NONE); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SAND_ATTACK, MOVE_SMOKESCREEN); }
    } WHEN {
        TURN { SCORE_EQ(opponent, MOVE_SAND_ATTACK, MOVE_SMOKESCREEN); }
    }
}
