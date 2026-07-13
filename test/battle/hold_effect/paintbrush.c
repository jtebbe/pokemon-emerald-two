#include "global.h"
#include "battle_util.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_PAINTBRUSH].holdEffect == HOLD_EFFECT_PAINTBRUSH);
    ASSUME(GetItemPocket(ITEM_PAINTBRUSH) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_PAINTBRUSH) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_PAINTBRUSH) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_PAINTBRUSH) == 10000);
    ASSUME(GetItemFlingPower(ITEM_PAINTBRUSH) == 20);
}

SINGLE_BATTLE_TEST("Paintbrush gives a single-type holder the move's type as its secondary type")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 1) == TYPE_PSYCHIC);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PAINTBRUSH); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        MESSAGE("Wobbuffet painted on the Water type!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_PSYCHIC);
        EXPECT_EQ(player->types[1], TYPE_WATER);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush overwrites a dual-type holder's secondary type")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_LUDICOLO, 0) == TYPE_WATER);
        ASSUME(GetSpeciesType(SPECIES_LUDICOLO, 1) == TYPE_GRASS);
        ASSUME(GetMoveType(MOVE_ICE_BEAM) == TYPE_ICE);
        PLAYER(SPECIES_LUDICOLO) { Item(ITEM_PAINTBRUSH); Moves(MOVE_ICE_BEAM); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_BEAM); }
    } SCENE {
        MESSAGE("Ludicolo painted over its Grass type with the Ice type!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_WATER);
        EXPECT_EQ(player->types[1], TYPE_ICE);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush does not trigger when the move already matches one of the holder's types")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_LUDICOLO, 0) == TYPE_WATER);
        ASSUME(GetSpeciesType(SPECIES_LUDICOLO, 1) == TYPE_GRASS);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_LUDICOLO) { Item(ITEM_PAINTBRUSH); Moves(MOVE_WATER_GUN, MOVE_ICE_BEAM); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF {
            MESSAGE("Ludicolo painted on the Water type!");
            MESSAGE("Ludicolo painted over its Grass type with the Water type!");
        }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_WATER);
        EXPECT_EQ(player->types[1], TYPE_GRASS);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush only triggers once per switch-in")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 1) == TYPE_PSYCHIC);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PAINTBRUSH); Moves(MOVE_WATER_GUN, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("Wobbuffet painted on the Water type!");
        NOT MESSAGE("Wobbuffet painted over its Water type with the Fire type!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_PSYCHIC);
        EXPECT_EQ(player->types[1], TYPE_WATER);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush reverts and can trigger again after switching out")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 1) == TYPE_PSYCHIC);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PAINTBRUSH); Moves(MOVE_WATER_GUN, MOVE_EMBER); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_ZIGZAGOON);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("Wobbuffet painted on the Water type!");
        MESSAGE("Wobbuffet painted on the Fire type!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_PSYCHIC);
        EXPECT_EQ(player->types[1], TYPE_FIRE);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush runs after Protean and does not spend itself on Protean's type change")
{
    GIVEN {
        WITH_CONFIG(CONFIG_PROTEAN_LIBERO, GEN_9);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_PROTEAN); Item(ITEM_PAINTBRUSH); Moves(MOVE_WATER_GUN, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PROTEAN);
        MESSAGE("Kecleon transformed into the Water type!");
        MESSAGE("Kecleon painted on the Fire type!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_WATER);
        EXPECT_EQ(player->types[1], TYPE_FIRE);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedProteanLibero);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}

SINGLE_BATTLE_TEST("Paintbrush does not activate on the same move as Protean")
{
    GIVEN {
        WITH_CONFIG(CONFIG_PROTEAN_LIBERO, GEN_9);
        ASSUME(GetMoveType(MOVE_BITE) == TYPE_DARK);
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_PROTEAN); Item(ITEM_PAINTBRUSH); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PROTEAN);
        MESSAGE("Kecleon transformed into the Dark type!");
        NONE_OF {
            MESSAGE("Kecleon painted on the Dark type!");
            MESSAGE("Kecleon painted over its Dark type with the Dark type!");
        }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_DARK);
        EXPECT_EQ(player->types[1], TYPE_DARK);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].usedProteanLibero);
        EXPECT(!gDisableStructs[B_POSITION_PLAYER_LEFT].usedPaintbrush);
    }
}
