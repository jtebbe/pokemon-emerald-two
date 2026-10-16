#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_HEAL_BLOCK) == EFFECT_HEAL_BLOCK);
    ASSUME(IsHealingMove(MOVE_RECOVER));
}

SINGLE_BATTLE_TEST("Heal Block prevents both executing and selecting healing moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_RECOVER, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_HEAL_BLOCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(player, MOVE_RECOVER, allowed: FALSE); MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Heal Block permits healing again after five turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE, MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_HEAL_BLOCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_HEAL_BLOCK); }
        for (u32 i = 0; i < 4; i++)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        MESSAGE("Wobbuffet is no longer prevented from healing!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
    } THEN {
        EXPECT_GT(player->hp, 1);
    }
}
