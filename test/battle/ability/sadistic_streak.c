#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Sadistic Streak is exclusive to Alolan Meowth and Persian holding an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_MEOWTH_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERSIAN_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERSIAN_ALOLA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MEOWTH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MEOWTH_GALAR; item = ITEM_APEX_METEORITE; }
    GIVEN { PLAYER(species) { Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SADISTIC_STREAK, item == ITEM_APEX_METEORITE && (species == SPECIES_MEOWTH_ALOLA || species == SPECIES_PERSIAN_ALOLA)); }
}

SINGLE_BATTLE_TEST("Sadistic Streak includes Merciless for physical and special attacks against either poison status")
{
    enum Move move = MOVE_SCRATCH;
    u32 status = STATUS1_POISON, gen = GEN_9;
    for (u32 n = GEN_1; n <= GEN_9; n++) {
        PARAMETRIZE { gen = n; move = MOVE_SCRATCH; status = STATUS1_POISON; }
        PARAMETRIZE { gen = n; move = MOVE_SWIFT; status = STATUS1_TOXIC_POISON; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Status1(status); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Sadistic Streak respects Battle Armor Shell Armor and Lucky Chant")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; }
    PARAMETRIZE { ability = ABILITY_SHELL_ARMOR; }
    PARAMETRIZE { ability = ABILITY_NONE; }
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); Status1(STATUS1_POISON); Speed(100); }
    } WHEN { TURN { MOVE(opponent, ability == ABILITY_NONE ? MOVE_LUCKY_CHANT : MOVE_CELEBRATE); MOVE(player, MOVE_SCRATCH); } }
    SCENE { NOT MESSAGE("A critical hit!"); HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Sadistic Streak does not force critical hits against unpoisoned targets")
{
    PASSES_RANDOMLY(23, 24, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_MEOWTH_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); } }
    SCENE { NOT MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Sadistic Streak raises only Attack Special Attack and Speed after a direct knockout")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    PARAMETRIZE { move = MOVE_DRAGON_RAGE; }
    GIVEN {
        PLAYER(SPECIES_MEOWTH_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, move); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SADISTIC_STREAK); }
    THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sadistic Streak does not inherit Sickening Greed's Pay Day activation")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_PAY_DAY); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SADISTIC_STREAK); }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE); }
}

DOUBLE_BATTLE_TEST("Sadistic Streak counts each knockout from a spread attack")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); Speed(1); }
        OPPONENT(SPECIES_SNORLAX) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_SWIFT); SEND_OUT(opponentLeft, 2); SEND_OUT(opponentRight, 3); } }
    THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sadistic Streak rewards one multihit knockout once")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Level(50); Attack(450); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(100); MaxHP(100); Defense(100); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_HIT, criticalHit: FALSE); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SADISTIC_STREAK); }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sadistic Streak raises uncapped stats when Attack is already capped")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        for (u32 i = 0; i < 3; i++) TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(opponent, 1); }
    } THEN { EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sadistic Streak does not reward poison residual knockouts")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(800); Status1(STATUS1_POISON); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_CELEBRATE); SEND_OUT(opponent, 1); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SADISTIC_STREAK); }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sadistic Streak cannot boost an attacker that faints to contact recoil")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); SEND_OUT(opponent, 1); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SADISTIC_STREAK); }
}

DOUBLE_BATTLE_TEST("Sadistic Streak does not reward its ally's knockout")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft); SEND_OUT(opponentLeft, 2); } }
    SCENE { NOT ABILITY_POPUP(playerLeft, ABILITY_SADISTIC_STREAK); }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}
