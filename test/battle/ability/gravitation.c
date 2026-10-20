#include "global.h"
#include "test/battle.h"
#include "battle_anim.h"
#include "battle_util.h"
#include "sprite.h"

TEST("Gravitation is available as Solrock and Lunatone's hidden ability")
{
    EXPECT_EQ(GetSpeciesAbility(SPECIES_SOLROCK, 2), ABILITY_GRAVITATION);
    EXPECT_EQ(GetSpeciesAbility(SPECIES_LUNATONE, 2), ABILITY_GRAVITATION);
}

SINGLE_BATTLE_TEST("Gravitation sets Gravity with its animation and message on either side")
{
    enum Species species = SPECIES_SOLROCK;
    bool32 opposing = FALSE;
    for (u32 side = 0; side < 2; side++) {
        PARAMETRIZE { species = SPECIES_SOLROCK; opposing = side; }
        PARAMETRIZE { species = SPECIES_LUNATONE; opposing = side; }
    }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : species) { Ability(opposing ? ABILITY_SHADOW_TAG : ABILITY_GRAVITATION); }
        OPPONENT(opposing ? species : SPECIES_WOBBUFFET) { Ability(opposing ? ABILITY_GRAVITATION : ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_GRAVITATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opposing ? opponent : player);
        MESSAGE("Gravity intensified!");
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

SINGLE_BATTLE_TEST("Gravitation lasts exactly five turns without refreshing itself")
{
    u32 turns = 1;
    for (u32 i = 1; i <= 5; i++) {
        PARAMETRIZE { turns = i; }
    }
    GIVEN {
        PLAYER(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 i = 0; i < turns; i++)
            TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAVITATION);
        MESSAGE("Gravity intensified!");
        NOT ABILITY_POPUP(player, ABILITY_GRAVITATION);
        if (turns == 5)
            MESSAGE("Gravity returned to normal!");
    } THEN {
        EXPECT_EQ(gFieldTimers.gravityTimer, 5 - turns);
        EXPECT_EQ(!!(gFieldStatuses & STATUS_FIELD_GRAVITY), turns < 5);
    }
}

SINGLE_BATTLE_TEST("Gravitation does nothing when switching into existing Gravity")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_LUNATONE) { Ability(ABILITY_GRAVITATION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GRAVITY); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Gravity intensified!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_GRAVITATION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
            MESSAGE("Gravity intensified!");
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 3);
    }
}

DOUBLE_BATTLE_TEST("Gravitation only activates once when two setters enter together")
{
    GIVEN {
        PLAYER(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(75); }
        OPPONENT(SPECIES_LUNATONE) { Ability(ABILITY_GRAVITATION); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(25); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_GRAVITATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, playerLeft);
        MESSAGE("Gravity intensified!");
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_GRAVITATION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponentLeft);
            MESSAGE("Gravity intensified!");
        }
    } THEN {
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravitation can set Gravity again after it expires and the user reenters")
{
    GIVEN {
        PLAYER(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAVITATION);
        MESSAGE("Gravity intensified!");
        MESSAGE("Gravity returned to normal!");
        ABILITY_POPUP(player, ABILITY_GRAVITATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Gravity intensified!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravitation grounds Flying and Levitate battlers for Ground attacks")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_PIDGEY; ability = ABILITY_KEEN_EYE; }
    PARAMETRIZE { species = SPECIES_FLYGON; ability = ABILITY_LEVITATE; }
    GIVEN {
        PLAYER(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTH_POWER); }
    } SCENE {
        MESSAGE("Gravity intensified!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(IsBattlerGrounded(B_BATTLER_1, ability, HOLD_EFFECT_NONE));
    }
}

SINGLE_BATTLE_TEST("Gravitation cancels airborne effects when its user switches in")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_MAGNET_RISE; }
    PARAMETRIZE { move = MOVE_TELEKINESIS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            if (move == MOVE_TELEKINESIS) { MOVE(player, move); }
            else { MOVE(opponent, move); }
        }
        TURN { SWITCH(player, 1); if (move == MOVE_FLY) { SKIP_TURN(opponent); } }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, move == MOVE_TELEKINESIS ? player : opponent);
        ABILITY_POPUP(player, ABILITY_GRAVITATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Gravity intensified!");
        MESSAGE("The opposing Wobbuffet fell from the sky due to the gravity!");
    } THEN {
        EXPECT_EQ((u32)opponent->volatiles.semiInvulnerable, STATE_NONE);
        EXPECT_EQ((u32)opponent->volatiles.magnetRiseTimer, 0);
        EXPECT_EQ((u32)opponent->volatiles.telekinesis, FALSE);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas suppresses Gravitation")
{
    GIVEN {
        PLAYER(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
        OPPONENT(SPECIES_SOLROCK) { Ability(ABILITY_GRAVITATION); }
    } WHEN {
        TURN {}
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_GRAVITATION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
            MESSAGE("Gravity intensified!");
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRAVITY));
        EXPECT_EQ(gFieldTimers.gravityTimer, 0);
    }
}
