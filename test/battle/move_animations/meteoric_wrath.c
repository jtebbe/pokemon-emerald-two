#include "global.h"
#include "test/battle.h"
#include "battle_anim.h"
#include "sprite.h"

SINGLE_BATTLE_TEST("Meteoric Wrath impact animation releases resources from either side after switching")
{
    bool32 opponentUsesMove;
    PARAMETRIZE { opponentUsesMove = FALSE; }
    PARAMETRIZE { opponentUsesMove = TRUE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN {
            if (opponentUsesMove) { MOVE(opponent, MOVE_METEORIC_WRATH); }
            else { MOVE(player, MOVE_METEORIC_WRATH); }
        }
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
        TURN { SEND_OUT(player, 0); SEND_OUT(opponent, 0); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, opponentUsesMove ? opponent : player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

DOUBLE_BATTLE_TEST("Meteoric Wrath impact animation finishes with four active battlers")
{
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_METEORIC_WRATH); }
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, opponentRight);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}
