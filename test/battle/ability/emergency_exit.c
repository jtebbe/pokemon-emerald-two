#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Emergency Exit switches out when taking 50% max-hp damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GOLISOPOD) { Ability(ABILITY_EMERGENCY_EXIT); MaxHP(263); HP(60); };
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPER_FANG); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_EMERGENCY_EXIT);
    }
}
