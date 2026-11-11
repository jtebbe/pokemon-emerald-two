#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Scorching Blast bypasses Fire resistances, hit immunities and burn prevention")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_FLASH_FIRE; }
    PARAMETRIZE { ability = ABILITY_WELL_BAKED_BODY; }
    PARAMETRIZE { ability = ABILITY_WONDER_GUARD; }
    PARAMETRIZE { ability = ABILITY_WATER_VEIL; }
    GIVEN {
        PLAYER(SPECIES_MAGMAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PONYTA) { Ability(ability); HP(1000); MaxHP(1000); Status1(STATUS1_PARALYSIS); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        NOT MESSAGE("It's not very effective…");
        ABILITY_POPUP(player, ABILITY_SCORCHING_BLAST);
        STATUS_ICON(opponent, burn: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Scorching Blast can burn with Will-O-Wisp and that burn persists through switching")
{
    GIVEN {
        PLAYER(SPECIES_MAGBY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PONYTA) { Ability(ABILITY_WATER_VEIL); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Scorching Blast cannot burn through Protect or a Substitute that blocks the attack")
{
    enum Move guard;
    PARAMETRIZE { guard = MOVE_PROTECT; }
    PARAMETRIZE { guard = MOVE_SUBSTITUTE; }
    GIVEN {
        PLAYER(SPECIES_MAGMORTAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, guard); MOVE(player, MOVE_FLAMETHROWER); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Scorching Blast also gives Mega Launcher to non-Fire pulse moves")
{
    GIVEN {
        PLAYER(SPECIES_MAGMORTAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_PULSE, secondaryEffect: FALSE); }
    } THEN {
        EXPECT_EQ(gBattleMovePower, 90);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
