#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

SINGLE_BATTLE_TEST("Unknown Origin sets Gravity with its animation and message on entry")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_STARYU; }
    PARAMETRIZE { species = SPECIES_STARMIE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_UNKNOWN_ORIGIN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        MESSAGE("Gravity intensified!");
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Unknown Origin does not refresh existing Gravity on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_STARMIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(player, MOVE_GRAVITY); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Gravity intensified!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UNKNOWN_ORIGIN);
            MESSAGE("Gravity intensified!");
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Unknown Origin doubles positive and negative stat changes through Simple")
{
    GIVEN {
        PLAYER(SPECIES_STARMIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Unknown Origin neutralizes the Dark immunity while preserving the other type's matchup")
{
    enum Species foe;
    enum Item item;
    PARAMETRIZE { foe = SPECIES_UMBREON; item = ITEM_NONE; }
    PARAMETRIZE { foe = SPECIES_UMBREON; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { foe = SPECIES_BISHARP; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { foe = SPECIES_SCRAFTY; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_STARMIE) { Item(item); }
        OPPONENT(foe) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYBEAM, secondaryEffect: FALSE); }
    } SCENE {
        if (item == ITEM_NONE) {
            NOT HP_BAR(opponent);
        } else {
            HP_BAR(opponent);
            if (foe == SPECIES_BISHARP) {
                MESSAGE("It's not very effective…");
            } else if (foe == SPECIES_SCRAFTY) {
                MESSAGE("It's super effective!");
            } else {
                NONE_OF {
                    MESSAGE("It's not very effective…");
                    MESSAGE("It's super effective!");
                }
            }
        }
    }
}

AI_SINGLE_BATTLE_TEST("Unknown Origin AI can use Psychic moves directly against Dark types")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_UMBREON) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_STARMIE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PSYBEAM, MOVE_MIRACLE_EYE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_PSYBEAM, MOVE_MIRACLE_EYE); EXPECT_MOVE(opponent, MOVE_PSYBEAM); }
    } THEN {
        EXPECT_EQ(gAiLogicData->effectiveness[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], UQ_4_12(1.0));
        EXPECT_EQ(AI_GetAdjustedStatStage(B_BATTLER_1, MOVE_CALM_MIND, 1), 2);
    }
}
