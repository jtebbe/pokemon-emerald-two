#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Piercing Beak AI values physical Defense penetration without boosting special attacks")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(200); SpDefense(100); }
        OPPONENT(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Attack(100); SpAttack(100); Moves(MOVE_SCRATCH, MOVE_WATER_GUN); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE) {
                SCORE_GT(opponent, MOVE_SCRATCH, MOVE_WATER_GUN);
                EXPECT_MOVE(opponent, MOVE_SCRATCH);
            } else {
                SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_SCRATCH);
                EXPECT_MOVE(opponent, MOVE_WATER_GUN);
            }
        }
    }
}

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sPiercingPrediction;
static s32 sPiercingCritStage;
static bool32 sObservedPiercingPrediction;
static u32 sPiercingPredictionTurn;
static enum Move sPiercingPredictionMove;
static s32 ObservePiercingPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && move == sPiercingPredictionMove && gBattleResults.battleTurnCounter == sPiercingPredictionTurn
     && !gAiLogicData->aiPredictionInProgress)
    {
        struct DamageContext ctx = {0};
        ctx.battlerAtk = battlerAtk;
        ctx.battlerDef = battlerDef;
        ctx.move = ctx.chosenMove = move;
        ctx.aiCalc = TRUE;
        for (u32 battler = 0; battler < gBattlersCount; battler++) {
            ctx.abilities[battler] = gAiLogicData->abilities[battler];
            ctx.holdEffects[battler] = gAiLogicData->holdEffects[battler];
        }
        sPiercingCritStage = CalcCritChanceStage(&ctx);
        sPiercingPrediction = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sObservedPiercingPrediction = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Piercing Beak AI predicts physical special and Psyshock damage through defensive modifiers without changing stats")
{
    enum Move move;
    s16 damage;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_PSYSHOCK; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_FUR_COAT); Item(ITEM_EVIOLITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); Moves(move); }
    } WHEN {
        sObservedPiercingPrediction = FALSE;
        sPiercingPredictionTurn = 0;
        sPiercingPredictionMove = move;
        sDynamicAiFunc = ObservePiercingPrediction;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedPiercingPrediction);
        EXPECT_EQ(sPiercingCritStage, 1);
        EXPECT_GE(damage, sPiercingPrediction.minimum);
        EXPECT_LE(damage, sPiercingPrediction.maximum);
        EXPECT_EQ(player->defense, 100);
        EXPECT_EQ(player->spDefense, 100);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("Piercing Beak AI predicts guaranteed critical damage after Focus Energy")
{
    s16 damage;
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_FORCE_SETUP_FIRST_TURN | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); Moves(MOVE_FOCUS_ENERGY, MOVE_SCRATCH); }
    } WHEN {
        sObservedPiercingPrediction = FALSE;
        sPiercingPredictionTurn = 1;
        sPiercingPredictionMove = MOVE_SCRATCH;
        sDynamicAiFunc = ObservePiercingPrediction;
        TURN { EXPECT_MOVE(opponent, MOVE_FOCUS_ENERGY); }
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player, captureDamage: &damage);
        MESSAGE("A critical hit!");
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedPiercingPrediction);
        EXPECT_EQ(sPiercingCritStage, 3);
        EXPECT_GE(damage, sPiercingPrediction.minimum);
        EXPECT_LE(damage, sPiercingPrediction.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Piercing Beak AI avoids ineffective Attack drops even with Mold Breaker")
{
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Moves(MOVE_CELEBRATE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            if (item == ITEM_APEX_METEORITE) {
                SCORE_LT(opponent, MOVE_GROWL, MOVE_TAIL_WHIP);
                EXPECT_MOVE(opponent, MOVE_TAIL_WHIP);
            } else {
                SCORE_EQ(opponent, MOVE_GROWL, MOVE_TAIL_WHIP);
            }
        }
    } THEN {
        EXPECT_EQ(CanLowerStat(B_BATTLER_1, B_BATTLER_0, gAiLogicData, STAT_ATK), item == ITEM_NONE);
        EXPECT(CanLowerStat(B_BATTLER_1, B_BATTLER_0, gAiLogicData, STAT_DEF));
    }
}

AI_SINGLE_BATTLE_TEST("Piercing Beak AI does not cycle Intimidate against its Attack protection")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_INTIMIDATE); HP(1000); MaxHP(1000); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ZIGZAGOON) { HP(1000); MaxHP(1000); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
