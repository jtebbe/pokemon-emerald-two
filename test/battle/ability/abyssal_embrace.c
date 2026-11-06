#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Abyssal Embrace is the Apex ability of Tentacool and Tentacruel")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_TENTACOOL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_TENTACRUEL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_TENTACRUEL; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_ABYSSAL_EMBRACE, item == ITEM_APEX_METEORITE);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, item == ITEM_APEX_METEORITE ? B_BATTLER_0 + 1 : 0);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace traps on entry and damages its target every turn")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ABYSSAL_EMBRACE);
        MESSAGE("The opposing Wobbuffet was caught by Abyssal Embrace!");
        MESSAGE("The opposing Wobbuffet is hurt by Abyssal Embrace!");
        HP_BAR(opponent, damage: 16);
        MESSAGE("The opposing Wobbuffet is hurt by Abyssal Embrace!");
        HP_BAR(opponent, damage: 16);
    } THEN {
        EXPECT_EQ(opponent->hp, 224);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace doubles its damage strictly below half HP")
{
    u32 hp, damage;
    PARAMETRIZE { hp = 256; damage = 16; }
    PARAMETRIZE { hp = 129; damage = 16; }
    PARAMETRIZE { hp = 128; damage = 16; }
    PARAMETRIZE { hp = 127; damage = 32; }
    PARAMETRIZE { hp = 65; damage = 32; }
    GIVEN {
        PLAYER(SPECIES_TENTACOOL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); MaxHP(256); }
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(opponent, damage: damage);
    } THEN {
        EXPECT_EQ(opponent->hp, hp - damage);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace checks current HP again each turn")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(136); MaxHP(256); }
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        HP_BAR(opponent, damage: 16);
        HP_BAR(opponent, damage: 32);
    } THEN {
        EXPECT_EQ(opponent->hp, 88);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace rounds down with a minimum of one damage")
{
    u32 hp, maxHP, damage;
    PARAMETRIZE { hp = 15; maxHP = 15; damage = 1; }
    PARAMETRIZE { hp = 9; maxHP = 19; damage = 2; }
    PARAMETRIZE { hp = 17; maxHP = 33; damage = 2; }
    PARAMETRIZE { hp = 16; maxHP = 33; damage = 4; }
    GIVEN {
        PLAYER(SPECIES_TENTACOOL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); MaxHP(maxHP); }
    } WHEN {
        TURN {}
    } SCENE {
        HP_BAR(opponent, damage: damage);
    } THEN {
        EXPECT_EQ(opponent->hp, hp - damage);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace does not expire after binding move durations")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1024); MaxHP(1024); }
    } WHEN {
        for (u32 turn = 0; turn < 6; turn++)
            TURN {}
    } THEN {
        EXPECT_EQ(opponent->hp, 640);
        EXPECT(opponent->volatiles.abyssalEmbrace);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
    }
}

DOUBLE_BATTLE_TEST("Abyssal Embrace affects only the foe opposite its holder")
{
    bool32 right;
    PARAMETRIZE { right = FALSE; }
    PARAMETRIZE { right = TRUE; }
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(right ? ITEM_NONE : ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_TENTACRUEL) { Item(right ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->hp, right ? 256 : 240);
        EXPECT_EQ(opponentRight->hp, right ? 240 : 256);
        EXPECT_EQ((u32)opponentLeft->volatiles.abyssalEmbrace, right ? 0 : B_BATTLER_0 + 1);
        EXPECT_EQ((u32)opponentRight->volatiles.abyssalEmbrace, right ? B_BATTLER_2 + 1 : 0);
        EXPECT_EQ((u32)playerLeft->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ((u32)playerRight->volatiles.abyssalEmbrace, 0);
    }
}

DOUBLE_BATTLE_TEST("Abyssal Embrace holders each trap their own opposite foe")
{
    GIVEN {
        PLAYER(SPECIES_TENTACOOL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 240);
        EXPECT_EQ(opponentRight->hp, 240);
        EXPECT_EQ((u32)opponentLeft->volatiles.abyssalEmbrace, B_BATTLER_0 + 1);
        EXPECT_EQ((u32)opponentRight->volatiles.abyssalEmbrace, B_BATTLER_2 + 1);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace allows pivot moves and is not passed to the replacement")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    PARAMETRIZE { move = MOVE_TELEPORT; }
    GIVEN {
        WITH_CONFIG(B_TELEPORT_BEHAVIOR, GEN_8);
        WITH_CONFIG(B_BATON_PASS_TRAPPING, GEN_4);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, move); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(opponent->hp, 1000);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace releases targets when its holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_HP), 240);
        EXPECT_EQ(opponent->hp, 256);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace releases its target when its holder faints")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1); Level(1); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 256);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace releases its target when its holder pivots including Baton Pass")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    GIVEN {
        WITH_CONFIG(B_BATON_PASS_TRAPPING, GEN_4);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace does not retrap a target that returns without a new entry")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_U_TURN); SEND_OUT(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN {
        EXPECT_EQ(opponent->hp, 256);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace captures the current foe on every entry")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ(opponent->hp, 240);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, B_BATTLER_0 + 1);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace allows Ghost Shed Shell and Run Away escape")
{
    enum Species species;
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_GASTLY; item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_SHED_SHELL; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; ability = ABILITY_RUN_AWAY; }
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        WITH_CONFIG(B_RUN_AWAY, GEN_CHAMPIONS);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Item(item); Ability(ability); HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_HP), 240);
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ(opponent->hp, 256);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace damage is prevented by Magic Guard without removing its trapping")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MAGIC_GUARD); HP(256); MaxHP(256); }
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ABYSSAL_EMBRACE);
        NONE_OF { HP_BAR(opponent); }
    } THEN {
        EXPECT_EQ(opponent->hp, 256);
        EXPECT(opponent->volatiles.abyssalEmbrace);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace damages through Protect and semi-invulnerable moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_PROTECT; }
    PARAMETRIZE { move = MOVE_DIG; }
    PARAMETRIZE { move = MOVE_FLY; }
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(opponent, damage: 16);
    } THEN {
        EXPECT_EQ(opponent->hp, 240);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace traps through an existing Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(512); MaxHP(512); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 352);
        EXPECT(opponent->volatiles.substitute);
        EXPECT(opponent->volatiles.abyssalEmbrace);
    }
}

DOUBLE_BATTLE_TEST("Abyssal Embrace release does not clear another Pokemon's Mean Look")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_MEAN_LOOK, target: opponentLeft); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT_EQ((u32)opponentLeft->volatiles.abyssalEmbrace, 0);
        EXPECT(opponentLeft->volatiles.escapePrevention);
        EXPECT_EQ((u32)opponentLeft->volatiles.battlerPreventingEscape, B_BATTLER_2);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
        EXPECT_EQ(opponentLeft->hp, 240);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace and binding move damage stack independently")
{
    GIVEN {
        WITH_CONFIG(B_BINDING_DAMAGE, GEN_5);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(player, MOVE_WRAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WRAP, player);
        HP_BAR(opponent);
        HP_BAR(opponent, damage: 16);
        MESSAGE("The opposing Wobbuffet is hurt by Abyssal Embrace!");
        HP_BAR(opponent, damage: 16);
    } THEN {
        EXPECT(opponent->volatiles.wrapped);
        EXPECT(opponent->volatiles.abyssalEmbrace);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace is not released by Rapid Spin")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAPID_SPIN); }
    } THEN {
        EXPECT_EQ(opponent->hp, 240);
        EXPECT(opponent->volatiles.abyssalEmbrace);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace allows forced switches without trapping the replacement")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR); }
    } SCENE {
        MESSAGE("The opposing Wynaut was dragged out!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(opponent->hp, 256);
    }
}

SINGLE_BATTLE_TEST("Abyssal Embrace can KO its target without trapping the replacement")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(256); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); }
    } WHEN {
        TURN { SEND_OUT(opponent, 1); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet is hurt by Abyssal Embrace!");
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(opponent->hp, 256);
    }
}
