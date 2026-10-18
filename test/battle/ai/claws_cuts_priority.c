#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("New attacking moves have accurate AI damage ranges")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FRIGID_CLAWS; }
    PARAMETRIZE { move = MOVE_CREEPING_CUTS; }
    PARAMETRIZE { move = MOVE_AURA_PULSE; }
    PARAMETRIZE { move = MOVE_FIERCE_RUSHDOWN; }
    s16 damage, secondHit;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        if (move == MOVE_CREEPING_CUTS)
            HP_BAR(player, captureDamage: &secondHit);
    } THEN {
        s32 total = damage;
        if (move == MOVE_CREEPING_CUTS)
            total += secondHit;
        EXPECT_GE(total, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].minimum);
        EXPECT_LE(total, gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum);
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], move == MOVE_AURA_PULSE ? 101 : 100);
    }
}

AI_SINGLE_BATTLE_TEST("AI recognizes conditional priority blocking by Psychic Terrain")
{
    bool32 damaged;
    PARAMETRIZE { damaged = FALSE; }
    PARAMETRIZE { damaged = TRUE; }
    SetStartingStatus(STARTING_STATUS_PSYCHIC_TERRAIN);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(damaged ? 1000 : 2000); MaxHP(2000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FIERCE_RUSHDOWN, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, damaged ? MOVE_TACKLE : MOVE_FIERCE_RUSHDOWN); }
    }
}

AI_SINGLE_BATTLE_TEST("AI speed predictions evaluate conditional priority against the simulated target")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GARDEVOIR) { Speed(200); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        player->hp = 1500;
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_FIERCE_RUSHDOWN, MOVE_TACKLE, CONSIDER_PRIORITY));
        player->hp = 1501;
        EXPECT(AI_IsSlower(B_BATTLER_1, B_BATTLER_0, MOVE_FIERCE_RUSHDOWN, MOVE_TACKLE, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_DARK_IMITATION, MOVE_TACKLE, CONSIDER_PRIORITY));
        player->hp = 3000;
        EXPECT(AI_IsSlower(B_BATTLER_1, B_BATTLER_0, MOVE_DARK_IMITATION, MOVE_TACKLE, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SIMPLIFY, MOVE_TACKLE, CONSIDER_PRIORITY));
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids Dark Imitation without a safe move to copy")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DARK_IMITATION, MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("Dark Imitation AI damage uses the predicted move without Me First's boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        bool32 predicting = gAiLogicData->predictingMove;
        enum Move prediction = gAiLogicData->predictedMove[B_BATTLER_0];
        gAiLogicData->predictingMove = TRUE;
        gAiLogicData->predictedMove[B_BATTLER_0] = MOVE_TACKLE;
        struct AiCalcValues cv = { .move = MOVE_TACKLE, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain };
        struct SimulatedDamage original = AI_CalcDamage(&cv, B_BATTLER_1, B_BATTLER_0);
        cv.move = MOVE_DARK_IMITATION;
        struct SimulatedDamage copied = AI_CalcDamage(&cv, B_BATTLER_1, B_BATTLER_0);
        EXPECT_EQ(original.minimum, copied.minimum);
        EXPECT_EQ(original.maximum, copied.maximum);
        gAiLogicData->predictedMove[B_BATTLER_0] = MOVE_DARK_IMITATION;
        copied = AI_CalcDamage(&cv, B_BATTLER_1, B_BATTLER_0);
        EXPECT_EQ(copied.maximum, 0);
        gAiLogicData->predictingMove = predicting;
        gAiLogicData->predictedMove[B_BATTLER_0] = prediction;
    }
}

AI_SINGLE_BATTLE_TEST("AI values Dark Imitation above Splash once it knows a usable move")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_DARK_IMITATION, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); SCORE_EQ(opponent, MOVE_DARK_IMITATION, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_DARK_IMITATION); }
    }
}

AI_SINGLE_BATTLE_TEST("Simplify AI avoids redundant or Tera targets")
{
    bool32 tera;
    PARAMETRIZE { tera = FALSE; }
    PARAMETRIZE { tera = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); TeraType(TYPE_FIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SIMPLIFY, MOVE_WATER_GUN); }
    } WHEN {
        if (tera)
            TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_TERA); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("Simplify AI values removing Ghost immunity for Normal attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTLY) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SIMPLIFY, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SIMPLIFY); }
    }
}
