#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sStoneObserved, sStoneHazards, sStoneBoostFlag;
static u32 sStoneObserveTurn;
static struct SimulatedDamage sStoneDamage;
static uq4_12_t sStoneEffectiveness;
static s32 ObserveUnbrokenStone(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && move == gBattleMons[attacker].moves[0]
     && !gAiLogicData->aiPredictionInProgress && gBattleResults.battleTurnCounter == sStoneObserveTurn)
    {
        sStoneObserved = TRUE;
        sStoneDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sStoneEffectiveness = gAiLogicData->effectiveness[attacker][defender][gAiThinkingStruct->movesetIndex];
        sStoneHazards = IsHazardOnSide(GetBattlerSide(attacker), HAZARDS_STEALTH_ROCK);
        sStoneBoostFlag = gProtectStructs[attacker].unbrokenStoneBoosted;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Unbroken Stone AI predicts neutral damage against both double and quadruple weaknesses", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(move); }
    } WHEN {
        sStoneObserved = FALSE; sStoneObserveTurn = 0; sDynamicAiFunc = ObserveUnbrokenStone;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sStoneObserved);
        EXPECT_EQ(sStoneEffectiveness, UQ_4_12(1.0));
        EXPECT_GE(results[i].damage, sStoneDamage.minimum);
        EXPECT_LE(results[i].damage, sStoneDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Unbroken Stone AI forecasts doubled Rock damage without consuming the real hazard", s16 damage)
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE, MOVE_STEALTH_ROCK); }
        OPPONENT(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_ROCK_THROW); }
    } WHEN {
        sStoneObserved = FALSE; sStoneObserveTurn = 1; sDynamicAiFunc = ObserveUnbrokenStone;
        TURN { MOVE(player, rocks ? MOVE_STEALTH_ROCK : MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_ROCK_THROW); }
        TURN { EXPECT_MOVE(opponent, MOVE_ROCK_THROW); }
    } SCENE {
        HP_BAR(player);
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sStoneObserved);
        EXPECT_EQ(sStoneHazards, rocks);
        EXPECT(!sStoneBoostFlag);
        EXPECT_GE(results[i].damage, sStoneDamage.minimum);
        EXPECT_LE(results[i].damage, sStoneDamage.maximum);
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Unbroken Stone AI avoids unnecessary contact hazards and respects contact protection")
{
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_PROTECTIVE_PADS; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_LONG_REACH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Ability(ability); Moves(MOVE_VINE_WHIP, MOVE_LEAFAGE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            if (item == ITEM_NONE && ability == ABILITY_NONE) {
                SCORE_LT(opponent, MOVE_VINE_WHIP, MOVE_LEAFAGE);
                EXPECT_MOVE(opponent, MOVE_LEAFAGE);
            } else {
                SCORE_GT(opponent, MOVE_VINE_WHIP, MOVE_LEAFAGE);
                EXPECT_MOVE(opponent, MOVE_VINE_WHIP);
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Unbroken Stone AI leaves rocks intact when selecting a non-Rock move")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_WONDER_GUARD); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE, MOVE_STEALTH_ROCK); }
        OPPONENT(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_ROCK_THROW, MOVE_NIGHT_SHADE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); EXPECT_MOVE(opponent, MOVE_NIGHT_SHADE); }
        TURN { EXPECT_MOVE(opponent, MOVE_NIGHT_SHADE); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    }
}
