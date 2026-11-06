#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sPeakVoltageDamage;
static bool32 sPeakVoltageObserved;
static enum BattleTerrain sPeakVoltageLiveTerrain;
static s32 sPeakVoltageScore;

static s32 ObservePeakVoltage(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && (move == MOVE_THUNDER_SHOCK || move == MOVE_REVELATION_DANCE || move == MOVE_THUNDER_WAVE || move == MOVE_DOUBLE_HIT)
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sPeakVoltageDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sPeakVoltageLiveTerrain = gFieldTimers.terrain;
        if (move == MOVE_THUNDER_WAVE || move == MOVE_DOUBLE_HIT)
            sPeakVoltageScore = score;
        sPeakVoltageObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Peak Voltage AI predicts the triggering move's terrain boost without changing the live field")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_THUNDER_SHOCK; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_REVELATION_DANCE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_THUNDER_SHOCK; item = ITEM_NONE; }
    s16 damage;
    GIVEN {
        WITH_CONFIG(B_TERRAIN_TYPE_BOOST, GEN_8);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_RAICHU) { Ability(ABILITY_STATIC); Item(item); SpAttack(300); Moves(move); }
    } WHEN {
        sPeakVoltageObserved = FALSE;
        sDynamicAiFunc = ObservePeakVoltage;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sPeakVoltageObserved);
        EXPECT_EQ(sPeakVoltageLiveTerrain, B_TERRAIN_NONE);
        EXPECT_GE(damage, sPeakVoltageDamage.minimum);
        EXPECT_LE(damage, sPeakVoltageDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_PEAK_VOLTAGE : ABILITY_STATIC);
    }
}

AI_SINGLE_BATTLE_TEST("Peak Voltage AI values setting terrain with Electric status moves", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Item(item); Moves(MOVE_THUNDER_WAVE, MOVE_THUNDER_SHOCK); }
    } WHEN {
        sPeakVoltageObserved = FALSE;
        sDynamicAiFunc = ObservePeakVoltage;
        TURN { EXPECT_MOVE(opponent, MOVE_THUNDER_WAVE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sPeakVoltageObserved);
        results[i].score = sPeakVoltageScore;
    } FINALLY {
        EXPECT_GT(results[1].score, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Peak Voltage AI avoids feeding an opponent five boosts but still uses a lethal Electric attack")
{
    u32 hp;
    enum Item item;
    PARAMETRIZE { hp = 3000; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { hp = 1; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { hp = 3000; item = ITEM_NONE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Item(item); HP(hp); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER_SHOCK, MOVE_WATER_GUN); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE && hp > 1) {
                SCORE_LT(opponent, MOVE_THUNDER_SHOCK, MOVE_WATER_GUN);
                EXPECT_MOVE(opponent, MOVE_WATER_GUN);
            } else if (hp == 1) {
                SCORE_EQ(opponent, MOVE_THUNDER_SHOCK, MOVE_WATER_GUN);
                SEND_OUT(player, 1);
            } else {
                SCORE_EQ_VAL(opponent, MOVE_THUNDER_SHOCK, 100);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Peak Voltage AI recognizes that a Substitute prevents the opponent's boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(1); Speed(1); Moves(MOVE_THUNDER_SHOCK, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
        TURN { SCORE_EQ_VAL(opponent, MOVE_THUNDER_SHOCK, 100); }
    }
}

AI_DOUBLE_BATTLE_TEST("Peak Voltage AI can deliberately give a surviving ally five boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SANDSHREW) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_SANDSHREW) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(1); Moves(MOVE_THUNDER_SHOCK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_THUNDER_SHOCK, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight);
        ABILITY_POPUP(opponentRight, ABILITY_PEAK_VOLTAGE);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(opponentRight->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}

AI_SINGLE_BATTLE_TEST("Peak Voltage AI predicts the defensive boosts between Electric multistrike hits")
{
    s16 first, second;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_GALVANIZE); Attack(300); Moves(MOVE_DOUBLE_HIT); }
    } WHEN {
        sPeakVoltageObserved = FALSE;
        sDynamicAiFunc = ObservePeakVoltage;
        TURN { EXPECT_MOVE(opponent, MOVE_DOUBLE_HIT); }
    } SCENE {
        HP_BAR(player, captureDamage: &first);
        HP_BAR(player, captureDamage: &second);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sPeakVoltageObserved);
        EXPECT_GE(first + second, sPeakVoltageDamage.minimum);
        EXPECT_LE(first + second, sPeakVoltageDamage.maximum);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2);
    }
}
