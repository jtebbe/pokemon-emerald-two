#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Mental Master AI prefers guaranteed high-power attacks and recognizes the Apex ability")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP, MOVE_SURF); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_HYDRO_PUMP, MOVE_SURF); EXPECT_MOVE(opponent, MOVE_HYDRO_PUMP); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_MENTAL_MASTER);
        EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], 100);
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master AI knows status moves cannot miss")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYPNOSIS); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_HYPNOSIS); }
    } THEN {
        EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], 100);
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master AI avoids priority attacks against the holder even with Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_EXTREME_SPEED, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_EXTREME_SPEED); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_EXTREME_SPEED, gAiLogicData));
        EXPECT(!Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, gAiLogicData));
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master AI avoids Prankster status moves against its immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); Moves(MOVE_THUNDER_WAVE, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_THUNDER_WAVE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_DOUBLE_BATTLE_TEST("Mental Master AI distinguishes the holder's priority immunity from its unprotected partner")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EXTREME_SPEED); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EXTREME_SPEED, target: playerRight); }
    } THEN {
        EXPECT(Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_EXTREME_SPEED, gAiLogicData));
        EXPECT(!Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_2, MOVE_EXTREME_SPEED, gAiLogicData));
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master gives the AI knowledge of all revealed moves items and abilities without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_WATER_ABSORB); Item(ITEM_LEFTOVERS); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_DESTINY_BOND); }
        OPPONENT(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP, MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SCORE_GT(opponent, MOVE_PSYCHIC, MOVE_HYDRO_PUMP); EXPECT_MOVE(opponent, MOVE_PSYCHIC); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_WATER_ABSORB);
        EXPECT_EQ(AI_DecideHoldEffectForTurn(B_BATTLER_0), HOLD_EFFECT_LEFTOVERS);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].item, ITEM_LEFTOVERS);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_0][slot], player->moves[slot]);
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master AI remembers unused moves and exact revealed items")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].moves[1], MOVE_THUNDERBOLT);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].ability, ABILITY_SHADOW_TAG);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].item, ITEM_ORAN_BERRY);
    }
}

AI_SINGLE_BATTLE_TEST("Mental Master AI permits opposing priority weather moves that target the field")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); Moves(MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
    } THEN {
        EXPECT(!Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_RAIN_DANCE, gAiLogicData));
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
    }
}
