#include "global.h"
#include "test/battle.h"
#include "battle_gimmick.h"
#include "battle_terastal.h"

SINGLE_BATTLE_TEST("Apex Meteorite blocks Terastallization even for species without an Apex ability")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_PIKACHU; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_TERA); }
    } SCENE {
        if (item == ITEM_APEX_METEORITE) {
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_TERA_CHARGE, player);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    } THEN {
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_0), item == ITEM_APEX_METEORITE ? GIMMICK_NONE : GIMMICK_TERA);
        if (item == ITEM_APEX_METEORITE)
            EXPECT(!CanTerastallize(B_BATTLER_0));
    }
}

SINGLE_BATTLE_TEST("Apex Meteorite still blocks Tera under Klutz, Magic Room and Embargo")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(variant == 0 ? ABILITY_KLUTZ : ABILITY_NONE); Item(ITEM_APEX_METEORITE); TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (variant == 1) TURN { MOVE(opponent, MOVE_MAGIC_ROOM); }
        if (variant == 2) TURN { MOVE(opponent, MOVE_EMBARGO); }
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_TERA); }
    } THEN {
        EXPECT(!CanTerastallize(B_BATTLER_0));
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_0), GIMMICK_NONE);
    }
}

SINGLE_BATTLE_TEST("Apex Meteorite does not consume a teammate's opportunity to Terastallize")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_TERA); }
    } SCENE {
        MESSAGE("Wobbuffet terastallized into the Water type!");
    } THEN {
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_0), GIMMICK_TERA);
    }
}
