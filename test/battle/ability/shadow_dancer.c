#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shadow Dancer doubles its holder's stat increases and decreases")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_TAIL_WHIP); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 4);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

DOUBLE_BATTLE_TEST("Shadow Dancer grants one dance copy to its ally and doubles stat changes only for its holder")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_DANCER_ORDER, GEN_9);
        PLAYER(SPECIES_MAROWAK_ALOLA) { Item(item); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(150); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_QUIVER_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, opponentLeft);
        if (item == ITEM_APEX_METEORITE)
        {
            ABILITY_POPUP(playerLeft, ABILITY_SHADOW_DANCER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerLeft);
            ABILITY_POPUP(playerLeft, ABILITY_SHADOW_DANCER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerRight);
        }
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerRight);
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE ? 2 : 0));
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE ? 1 : 0));
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_2), ABILITY_OWN_TEMPO);
    }
}

DOUBLE_BATTLE_TEST("Shadow Dancer's ally copies the holder's Soultorch Swing without spending PP")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLA) { Item(ITEM_APEX_METEORITE); MovesWithPP({MOVE_SOULTORCH_SWING, 5}); }
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_CELEBRATE, 5}); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_WATER_VEIL); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SOULTORCH_SWING, target: opponentLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOULTORCH_SWING, playerLeft, target: opponentLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SHADOW_DANCER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOULTORCH_SWING, playerRight, target: opponentLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
    } THEN {
        EXPECT_EQ(playerLeft->pp[0], 4);
        EXPECT_EQ(playerRight->pp[0], 4);
    }
}

DOUBLE_BATTLE_TEST("An ally loses the granted Dancer effect when Shadow Dancer switches out")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); MOVE(opponentLeft, MOVE_QUIVER_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, opponentLeft);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
