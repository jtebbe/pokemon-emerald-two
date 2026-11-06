#include "global.h"
#include "test/battle.h"
#include "battle_gimmick.h"
#include "battle_terastal.h"

AI_SINGLE_BATTLE_TEST("Apex Meteorite AI cannot Tera even when Tera would secure a KO")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PIKACHU; item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_TERA);
        PLAYER(SPECIES_WOBBUFFET) { HP(58); Defense(152); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(species) { Item(item); Attack(102); Speed(1); Moves(MOVE_SEED_BOMB); TeraType(TYPE_GRASS); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponent, MOVE_SEED_BOMB, gimmick: item == ITEM_APEX_METEORITE ? GIMMICK_NONE : GIMMICK_TERA);
            if (item == ITEM_NONE) SEND_OUT(player, 1);
        }
    } THEN {
        EXPECT_EQ(GetActiveGimmick(B_BATTLER_1), item == ITEM_APEX_METEORITE ? GIMMICK_NONE : GIMMICK_TERA);
        if (item == ITEM_APEX_METEORITE)
            EXPECT(!CanTerastallize(B_BATTLER_1));
    }
}
