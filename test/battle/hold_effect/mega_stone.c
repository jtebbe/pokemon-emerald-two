#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Necrozmite cannot be knocked off fused Necrozma before or after Mega Evolution")
{
    enum Species species;
    bool32 mega;
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; mega = FALSE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; mega = FALSE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; mega = TRUE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; mega = TRUE; }
    GIVEN {
        PLAYER(species) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE, gimmick: mega ? GIMMICK_MEGA : GIMMICK_NONE);
            MOVE(opponent, MOVE_KNOCK_OFF);
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NECROZMITE);
    }
}

SINGLE_BATTLE_TEST("Necrozmite can be knocked off a Pokemon that cannot use it")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_NECROZMA; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    GIVEN {
        PLAYER(species) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}
