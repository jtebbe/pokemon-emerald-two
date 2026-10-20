#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_ai_util.h"
#include "sprite.h"

SINGLE_BATTLE_TEST("Ilex Whimsy reveals the Apex Meteorite before shielding either side")
{
    bool32 opposing;
    PARAMETRIZE { opposing = FALSE; }
    PARAMETRIZE { opposing = TRUE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : SPECIES_CELEBI) { Item(opposing ? ITEM_NONE : ITEM_APEX_METEORITE); }
        OPPONENT(opposing ? SPECIES_CELEBI : SPECIES_WOBBUFFET) { Item(opposing ? ITEM_APEX_METEORITE : ITEM_NONE); }
    } WHEN {
        TURN {}
    } SCENE {
        ITEM_POPUP(opposing ? opponent : player, ITEM_APEX_METEORITE);
        if (opposing)
            MESSAGE("The opposing Celebi is unleashing its potential!");
        else
            MESSAGE("Celebi is unleashing its potential!");
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_ILEX_WHIMSY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LUCKY_CHANT, opposing ? opponent : player);
        if (opposing)
            MESSAGE("The opposing Celebi is shielding its side!");
        else
            MESSAGE("Celebi is shielding its side!");
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        enum BattleSide side = opposing ? B_SIDE_OPPONENT : B_SIDE_PLAYER;
        enum BattlerId battler = opposing ? B_BATTLER_1 : B_BATTLER_0;
        u32 protections = SIDE_STATUS_MIST | SIDE_STATUS_SAFEGUARD | SIDE_STATUS_LUCKY_CHANT;
        EXPECT_EQ(gSideStatuses[side] & protections, protections);
        EXPECT_EQ(gSideStatuses[side ^ 1] & protections, 0);
        EXPECT_EQ(gSideTimers[side].mistTimer, 4);
        EXPECT_EQ(gSideTimers[side].safeguardTimer, 4);
        EXPECT_EQ(gSideTimers[side].luckyChantTimer, 4);
        EXPECT_EQ(gAiPartyData->mons[side][0].heldEffect, HOLD_EFFECT_APEX_METEORITE);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(battler), ABILITY_ILEX_WHIMSY);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

SINGLE_BATTLE_TEST("Celebi retains Natural Cure without an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        NONE_OF {
            MESSAGE("Celebi is unleashing its potential!");
            ABILITY_POPUP(player, ABILITY_ILEX_WHIMSY);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_NATURAL_CURE);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 0);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].safeguardTimer, 0);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].luckyChantTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Ilex Whimsy's three protections expire after five turns")
{
    u32 turns = 1;
    for (u32 i = 1; i <= 5; i++)
        PARAMETRIZE { turns = i; }
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 i = 0; i < turns; i++)
            TURN {}
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 5 - turns);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].safeguardTimer, 5 - turns);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].luckyChantTimer, 5 - turns);
    }
}

SINGLE_BATTLE_TEST("Ilex Whimsy blocks status stat drops and guaranteed critical hits")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { move = MOVE_SPORE; }
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_CONFIDE; }
    PARAMETRIZE { move = MOVE_FROST_BREATH; }
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Ilex Whimsy leaves Infiltrator's normal Mist and Safeguard bypass intact")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CROBAT) { Ability(ABILITY_INFILTRATOR); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Apex reveal is once per party Pokemon but Ilex Whimsy can activate again")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
        TURN { SWITCH(player, 0); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Celebi is unleashing its potential!");
        MESSAGE("Celebi is shielding its side!");
        MESSAGE("Dialga is unleashing its potential!");
        NOT MESSAGE("Celebi is unleashing its potential!");
        MESSAGE("Celebi is shielding its side!");
        NOT MESSAGE("Dialga is unleashing its potential!");
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 3);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].safeguardTimer, 3);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].luckyChantTimer, 3);
        EXPECT(gBattleStruct->partyState[B_TRAINER_PLAYER][0].apexRevealed);
        EXPECT(gBattleStruct->partyState[B_TRAINER_PLAYER][1].apexRevealed);
    }
}

SINGLE_BATTLE_TEST("Ilex Whimsy preserves Time Dominus's longer active protections")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(player, MOVE_MIST); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 253);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].safeguardTimer, 4);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].luckyChantTimer, 4);
    }
}

DOUBLE_BATTLE_TEST("Apex reveal is separate for simultaneous holders and Ilex Whimsy shields its ally")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Speed(90); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(70); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_GROWL); MOVE(opponentRight, MOVE_WILL_O_WISP, target: playerRight); }
    } SCENE {
        MESSAGE("Celebi is unleashing its potential!");
        MESSAGE("Dialga is unleashing its potential!");
        MESSAGE("Celebi is shielding its side!");
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("An unassigned Apex ability still reveals its Meteorite to the AI")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("Wobbuffet is unleashing its potential!");
    } THEN {
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].heldEffect, HOLD_EFFECT_APEX_METEORITE);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_NONE);
    }
}
