#include "global.h"
#include "test/battle.h"
#include "battle_anim.h"
#include "sprite.h"

SINGLE_BATTLE_TEST("Shared ordinary move animations still load and release their resources")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_ROCK_WRECKER; }
    PARAMETRIZE { move = MOVE_ETERNABEAM; }
    PARAMETRIZE { move = MOVE_EXPANDING_FORCE; }
    PARAMETRIZE { move = MOVE_METEOR_BEAM; }
    PARAMETRIZE { move = MOVE_RISING_VOLTAGE; }
    PARAMETRIZE { move = MOVE_WICKED_BLOW; }
    PARAMETRIZE { move = MOVE_ASTRAL_BARRAGE; }
    PARAMETRIZE { move = MOVE_MAGICAL_TORQUE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}
