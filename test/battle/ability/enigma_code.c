#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Enigma Code reverses move order for the opening turn and boosts Speed when it ends")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_PORYGON; }
    PARAMETRIZE { species = SPECIES_PORYGON2; }
    PARAMETRIZE { species = SPECIES_PORYGON_Z; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENIGMA_CODE);
        MESSAGE("The dimensions were twisted!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        MESSAGE("The twisted dimensions returned to normal!");
        ABILITY_POPUP(player, ABILITY_ENIGMA_CODE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Enigma Code sets Trick Room for the remainder of the switch-in turn")
{
    GIVEN {
        WITH_CONFIG(B_RECALC_TURN_AFTER_ACTIONS, GEN_8);
        PLAYER(SPECIES_WOBBUFFET) { Speed(70); }
        PLAYER(SPECIES_WYNAUT) { Speed(20); }
        PLAYER(SPECIES_PORYGON2) { Item(ITEM_APEX_METEORITE); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN {
        TURN {}
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_ENIGMA_CODE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("The twisted dimensions returned to normal!");
        ABILITY_POPUP(playerLeft, ABILITY_ENIGMA_CODE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
    }
}

SINGLE_BATTLE_TEST("Enigma Code leaves an active Trick Room and its timer alone on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_PORYGON2) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ENIGMA_CODE);
            MESSAGE("The twisted dimensions returned to normal!");
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 3);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Enigma Code boosts every active holder when Trick Room is canceled or expires")
{
    GIVEN {
        PLAYER(SPECIES_PORYGON) { Item(ITEM_APEX_METEORITE); Speed(20); }
        OPPONENT(SPECIES_PORYGON_Z) { Item(ITEM_APEX_METEORITE); Speed(50); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, opponent);
        MESSAGE("The twisted dimensions returned to normal!");
        ABILITY_POPUP(player, ABILITY_ENIGMA_CODE);
        ABILITY_POPUP(opponent, ABILITY_ENIGMA_CODE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, opponent);
        MESSAGE("The twisted dimensions returned to normal!");
        ABILITY_POPUP(player, ABILITY_ENIGMA_CODE);
        ABILITY_POPUP(opponent, ABILITY_ENIGMA_CODE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
    }
}
