#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Blazing Rush boosts Speed before burning slower foes for both regular horse species")
{
    enum Species species;
    bool32 opposing;
    PARAMETRIZE { species = SPECIES_PONYTA; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_RAPIDASH; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_PONYTA; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_RAPIDASH; opposing = TRUE; }
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : species) { Item(opposing ? ITEM_NONE : ITEM_APEX_METEORITE); Speed(opposing ? 120 : 100); }
        OPPONENT(opposing ? species : SPECIES_WOBBUFFET) { Item(opposing ? ITEM_APEX_METEORITE : ITEM_NONE); Speed(opposing ? 100 : 120); }
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_BLAZING_RUSH);
        if (opposing) {
            if (species == SPECIES_PONYTA) MESSAGE("The opposing Ponyta's Speed rose!");
            else MESSAGE("The opposing Rapidash's Speed rose!");
        } else {
            if (species == SPECIES_PONYTA) MESSAGE("Ponyta's Speed rose!");
            else MESSAGE("Rapidash's Speed rose!");
        }
        STATUS_ICON(opposing ? player : opponent, burn: TRUE);
        NOT ABILITY_POPUP(opposing ? opponent : player, ABILITY_BLAZING_RUSH);
    } THEN {
        enum BattlerId holder = opposing ? B_BATTLER_1 : B_BATTLER_0;
        enum BattlerId foe = opposing ? B_BATTLER_0 : B_BATTLER_1;
        EXPECT_EQ(GetBattlerAbility(holder), ABILITY_BLAZING_RUSH);
        EXPECT_EQ(gBattleMons[holder].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMons[foe].status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush requires strictly greater effective Speed and ignores Trick Room order")
{
    u32 speed;
    PARAMETRIZE { speed = 90; }
    PARAMETRIZE { speed = 149; }
    PARAMETRIZE { speed = 150; }
    PARAMETRIZE { speed = 151; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speed); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, speed < 150 ? STATUS1_BURN : STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Blazing Rush checks each foe independently and never burns its ally")
{
    u32 speed;
    PARAMETRIZE { speed = 149; }
    PARAMETRIZE { speed = 150; }
    PARAMETRIZE { speed = 151; }
    GIVEN {
        PLAYER(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(120); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speed); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->status1, STATUS1_BURN);
        EXPECT_EQ(opponentRight->status1, speed < 150 ? STATUS1_BURN : STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush respects burn and status immunity")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_CHARMANDER; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_WATER_VEIL; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_WATER_BUBBLE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_THERMAL_EXCHANGE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_COMATOSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; }
    GIVEN {
        PLAYER(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(species) { Ability(ability); Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush does not replace existing nonvolatile status")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_BURN; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush respects Safeguard and grounded Misty Terrain but bypasses Substitute")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_MISTY_TERRAIN; }
    PARAMETRIZE { protection = MOVE_SUBSTITUTE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponent, protection); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, protection == MOVE_SUBSTITUTE ? STATUS1_BURN : STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush compares Speed modifiers such as Choice Scarf and Tailwind")
{
    bool32 tailwind;
    PARAMETRIZE { tailwind = FALSE; }
    PARAMETRIZE { tailwind = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Item(tailwind ? ITEM_NONE : ITEM_CHOICE_SCARF); }
    } WHEN {
        if (tailwind) TURN { MOVE(opponent, MOVE_TAILWIND); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush activates again on reentry and curing berries consume normally")
{
    enum Item berry;
    PARAMETRIZE { berry = ITEM_RAWST_BERRY; }
    PARAMETRIZE { berry = ITEM_LUM_BERRY; }
    GIVEN {
        PLAYER(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Item(berry); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLAZING_RUSH);
        STATUS_ICON(opponent, burn: TRUE);
        STATUS_ICON(opponent, burn: FALSE);
        ABILITY_POPUP(player, ABILITY_BLAZING_RUSH);
        STATUS_ICON(opponent, burn: TRUE);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush still burns at the Speed stage cap")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush resolves Sticky Web before its entry Speed boost")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(120); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush cannot be suppressed by Neutralizing Gas and its boost can be copied")
{
    bool32 gas;
    PARAMETRIZE { gas = FALSE; }
    PARAMETRIZE { gas = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(gas ? ABILITY_NEUTRALIZING_GAS : ABILITY_OPPORTUNIST); Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + !gas);
        EXPECT_EQ(opponent->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Blazing Rush requires an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_PONYTA) { Ability(ABILITY_RUN_AWAY); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
