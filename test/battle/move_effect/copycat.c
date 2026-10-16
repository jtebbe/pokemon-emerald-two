#include "global.h"
#include "test/battle.h"

TO_DO_BATTLE_TEST("Copycat causes the user to use the last move used in battle");
TO_DO_BATTLE_TEST("Copycat can call the user's last move used if it was the last move used in battle");
TO_DO_BATTLE_TEST("Copycat fails if no move has been made");
TO_DO_BATTLE_TEST("Copycat chooses a target at random if the copied move wasn't the user's");
TO_DO_BATTLE_TEST("Copycat can copy moves regardless if they failed or not"); //Has "X used move!"

// Gen 4
TO_DO_BATTLE_TEST("Copycat can only copy charging moves after it has been executed (Gen 4)");
TO_DO_BATTLE_TEST("Copycat can only copy recharging moves after it has been executed (Gen 4)");
TO_DO_BATTLE_TEST("Copycat cannot copy Bide's final turn (Gen 4)");
TO_DO_BATTLE_TEST("Copycat copies other calling moves instead of the move they called (Gen 4)");
TO_DO_BATTLE_TEST("Copycat copies moves called by other calling moves instead of the calling move if they are executed in a second turn (Gen 5+)"); //Eg. Dig

// Gen 5+
TO_DO_BATTLE_TEST("Copycat can copy charging moves in both the charging and the executing turn (Gen 5+)");
TO_DO_BATTLE_TEST("Copycat ignores the recharging turn of recharging moves (Gen 5+)");
TO_DO_BATTLE_TEST("Copycat can copy Bide on all turns");
TO_DO_BATTLE_TEST("Copycat copies moves called by other calling moves instead of the calling move (Gen 5+)");

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_COPYCAT) == EFFECT_COPYCAT);
}

SINGLE_BATTLE_TEST("Copycat deducts power points from itself, not the copied move")
{
    ASSUME(GetMovePP(MOVE_COPYCAT) == 32);
    ASSUME(GetMovePP(MOVE_POUND) == 56);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_COPYCAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POUND); MOVE(player, MOVE_COPYCAT); }
    } SCENE {
    } THEN {
        EXPECT_EQ(opponent->pp[0], GetMovePP(MOVE_POUND) - 1);
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_COPYCAT) - 1);
    }
}

DOUBLE_BATTLE_TEST("Copycat can copy a move that fainted its target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_COPYCAT, target: opponentRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentRight);
    }
}
