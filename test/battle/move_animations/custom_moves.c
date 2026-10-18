#include "global.h"
#include "test/battle.h"
#include "battle_anim.h"
#include "sprite.h"

SINGLE_BATTLE_TEST("Custom move animations finish and release their resources from either side")
{
    enum Move move = MOVE_NONE;
    bool32 opponentUsesMove = FALSE;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { move = MOVE_SERRATED_SLICE; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_DAZZLING_DASH; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_CORRODE; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_MIND_CRUSH; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_HAUNTING_SHRIEKS; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_VOLTAIC_RUSH; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_STORM_SACRIFICE; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_SUNBLOOM; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_INFERNAL_FISTS; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_FRIGID_CLAWS; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_CREEPING_CUTS; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_AURA_PULSE; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_FIERCE_RUSHDOWN; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_DARK_IMITATION; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_SIMPLIFY; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_MUD_BATH; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_GEMSHOT; opponentUsesMove = j; }
        PARAMETRIZE { move = MOVE_HARSH_ZEPHYR; opponentUsesMove = j; }
    }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); }
    } WHEN {
        TURN {
            if (opponentUsesMove) { MOVE(opponent, move); MOVE(player, move == MOVE_DARK_IMITATION ? MOVE_TACKLE : MOVE_CELEBRATE); }
            else { MOVE(player, move); MOVE(opponent, move == MOVE_DARK_IMITATION ? MOVE_TACKLE : MOVE_CELEBRATE); }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentUsesMove ? opponent : player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
        EXPECT(!gBattleStruct->hauntingShrieksBgActive);
    }
}

DOUBLE_BATTLE_TEST("Gemshot and Harsh Zephyr animations finish on both sides with one or two affected foes")
{
    enum Move move;
    bool32 opponentUsesMove, protect;
    for (u32 side = 0; side < 2; side++) {
        for (u32 blocked = 0; blocked < 2; blocked++) {
            PARAMETRIZE { move = MOVE_GEMSHOT; opponentUsesMove = side; protect = blocked; }
            PARAMETRIZE { move = MOVE_HARSH_ZEPHYR; opponentUsesMove = side; protect = blocked; }
        }
    }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); Speed(50); }
        PLAYER(SPECIES_WYNAUT) { HP(9999); MaxHP(9999); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); Speed(50); }
        OPPONENT(SPECIES_WYNAUT) { HP(9999); MaxHP(9999); Speed(100); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_TAILWIND);
            MOVE(opponentRight, MOVE_TAILWIND);
            if (opponentUsesMove) {
                MOVE(opponentLeft, move, target: playerLeft);
                if (protect) { MOVE(playerLeft, MOVE_PROTECT); }
            } else {
                MOVE(playerLeft, move, target: opponentLeft);
                if (protect) { MOVE(opponentLeft, MOVE_PROTECT); }
            }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentUsesMove ? opponentLeft : playerLeft);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks restores its dark background after a full volley or an early knockout")
{
    u32 hp;
    PARAMETRIZE { hp = 9999; }
    PARAMETRIZE { hp = 1; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GASTLY) { HP(0); }
        PLAYER(SPECIES_SHUPPET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); MaxHP(9999); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); if (hp == 1) { SEND_OUT(opponent, 1); } }
    } SCENE {
        for (u32 j = 0; j < (hp == 1 ? 1 : 3); j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
            HP_BAR(opponent);
        }
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HAUNTING_SHRIEKS_END, player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
        EXPECT(!gBattleStruct->hauntingShrieksBgActive);
    }
}
