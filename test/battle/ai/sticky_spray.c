#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Sticky Spray AI avoids immune attacks and status moves even against Mold Breaker")
{
    enum Move move = MOVE_BUG_BUZZ;
    enum Ability ability = ABILITY_NONE;
    for (u32 i = 0; i < 2; i++)
    {
        PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = i ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_SLUDGE_BOMB; ability = i ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_TOXIC; ability = i ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
        PARAMETRIZE { move = MOVE_STRING_SHOT; ability = i ? ABILITY_MOLD_BREAKER : ABILITY_NONE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(move, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, move, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_STICKY_SPRAY);
        EXPECT_EQ(gAiLogicData->effectiveness[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], UQ_4_12(0.0));
        EXPECT_GT(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_1].maximum, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Sticky Spray AI avoids redundant Sticky Web after its entry ability")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_KAKUNA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_STICKY_WEB, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_STICKY_WEB, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("Sticky Spray AI can still set field hazards against an immune holder")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TOXIC_SPIKES; }
    PARAMETRIZE { move = MOVE_STICKY_WEB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    } THEN {
        EXPECT_NE(gAiLogicData->effectiveness[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], UQ_4_12(0.0));
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, move == MOVE_STICKY_WEB ? HAZARDS_STICKY_WEB : HAZARDS_TOXIC_SPIKES));
    }
}

AI_SINGLE_BATTLE_TEST("Sticky Spray switch AI recognizes a bench Apex immunity even against Mold Breaker")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TINKATON) { Ability(ability); Speed(3); Moves(move); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_METAPOD) { HP(1); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_HYPNO) { Speed(1); Moves(MOVE_IRON_HEAD); }
    } WHEN {
        TURN { MOVE(player, move); EXPECT_SEND_OUT(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("Sticky Spray smart switching seeks a Bug or Poison immunity even against Mold Breaker")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_PICKUP; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; ability = ABILITY_PICKUP; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ability); Moves(move, MOVE_STRENGTH); }
        OPPONENT(SPECIES_ZIGZAGOON) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_METAPOD) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, move); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, move); EXPECT_SWITCH(opponent, 1); }
    }
}
