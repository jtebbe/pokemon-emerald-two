#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hidden Power is always a 60 BP special Psychic move")
{
    GIVEN {
        ASSUME(GetMovePower(MOVE_HIDDEN_POWER) == 60);
        ASSUME(GetMoveType(MOVE_HIDDEN_POWER) == TYPE_PSYCHIC);
        ASSUME(GetMoveCategory(MOVE_HIDDEN_POWER) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING) { Item(ITEM_PAYAPA_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_HIDDEN_POWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HIDDEN_POWER, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}
