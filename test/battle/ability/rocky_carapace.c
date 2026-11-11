#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Rocky Carapace blocks damaging moves above 2x type effectiveness")
{
    enum Move move;
    enum Item item;
    bool32 blocked;
    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_APEX_METEORITE; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_NONE; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; item = ITEM_APEX_METEORITE; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_FLYING_PRESS; item = ITEM_APEX_METEORITE; blocked = FALSE; }
    GIVEN {
        PLAYER(SPECIES_RHYPERIOR) { Ability(ABILITY_LIGHTNING_ROD); Item(item); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move, secondaryEffect: FALSE); }
    } SCENE {
        if (blocked) {
            ABILITY_POPUP(player, ABILITY_ROCKY_CARAPACE);
            NOT HP_BAR(player);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_ROCKY_CARAPACE);
            HP_BAR(player);
            if (move == MOVE_METAL_CLAW)
                MESSAGE("It's super effective!");
        }
    }
}

SINGLE_BATTLE_TEST("Rocky Carapace checks combined effectiveness for two-type moves")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Ability(ABILITY_ROCKY_CARAPACE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SOULTORCH_SWING); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROCKY_CARAPACE);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(CalcPartyMonTypeEffectivenessMultiplier(MOVE_SOULTORCH_SWING, SPECIES_FROSLASS, ABILITY_ROCKY_CARAPACE), UQ_4_12(0.0));
    }
}

DOUBLE_BATTLE_TEST("Rocky Carapace clears its side before hazards affect either entrant")
{
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_SPIKES_PLAYER_L3);
    SetStartingStatus(STARTING_STATUS_TOXIC_SPIKES_PLAYER_L2);
    SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    SetStartingStatus(STARTING_STATUS_STICKY_WEB_PLAYER);
    SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_OPPONENT);
    GIVEN {
        PLAYER(SPECIES_RHYDON) { Item(ITEM_APEX_METEORITE); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_ROCKY_CARAPACE);
        MESSAGE("Rhydon crushed the hazards underfoot!");
    } THEN {
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_PLAYER));
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].spikesAmount, 0);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, 0);
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
