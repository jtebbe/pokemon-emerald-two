#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Species and forms default to no Apex ability unless explicitly assigned")
{
    for (enum Species species = SPECIES_NONE; species < NUM_SPECIES; species++)
    {
        enum Ability expected = ABILITY_NONE;
        if (species == SPECIES_BULBASAUR || species == SPECIES_IVYSAUR || species == SPECIES_VENUSAUR || species == SPECIES_VENUSAUR_MEGA)
            expected = ABILITY_ROOTBOUND;
        else if (species == SPECIES_CHARMANDER || species == SPECIES_CHARMELEON || species == SPECIES_CHARIZARD
              || species == SPECIES_CHARIZARD_MEGA_X || species == SPECIES_CHARIZARD_MEGA_Y)
            expected = ABILITY_BLAZING_GLORY;
        else if (species == SPECIES_DIALGA || species == SPECIES_DIALGA_ORIGIN)
            expected = ABILITY_TIME_DOMINUS;
        else if (species == SPECIES_CELEBI)
            expected = ABILITY_ILEX_WHIMSY;
        else if (species == SPECIES_GIRATINA_ALTERED || species == SPECIES_GIRATINA_ORIGIN)
            expected = ABILITY_DISTORTION;
        else if (species == SPECIES_PALKIA || species == SPECIES_PALKIA_ORIGIN)
            expected = ABILITY_SPACE_DOMINUS;
        else if (species == SPECIES_ARCEUS_NORMAL || (species >= SPECIES_ARCEUS_FIGHTING && species <= SPECIES_ARCEUS_FAIRY))
            expected = ABILITY_OMNIPOTENCE;
        EXPECT_EQ(gSpeciesInfo[species].apexAbility, expected);
        if (species == SPECIES_NONE || IsSpeciesEnabled(species))
            EXPECT_EQ(GetSpeciesApexAbility(species), expected);
    }
}

TEST("Apex abilities replace normal slots only while the Meteorite is held")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_BULBASAUR; }
    PARAMETRIZE { species = SPECIES_IVYSAUR; }
    PARAMETRIZE { species = SPECIES_VENUSAUR; }
    PARAMETRIZE { species = SPECIES_VENUSAUR_MEGA; }
    PARAMETRIZE { species = SPECIES_CHARMANDER; }
    PARAMETRIZE { species = SPECIES_CHARMELEON; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; }
    PARAMETRIZE { species = SPECIES_CHARIZARD_MEGA_X; }
    PARAMETRIZE { species = SPECIES_CHARIZARD_MEGA_Y; }
    PARAMETRIZE { species = SPECIES_DIALGA; }
    PARAMETRIZE { species = SPECIES_DIALGA_ORIGIN; }
    PARAMETRIZE { species = SPECIES_CELEBI; }
    PARAMETRIZE { species = SPECIES_GIRATINA_ALTERED; }
    PARAMETRIZE { species = SPECIES_GIRATINA_ORIGIN; }
    PARAMETRIZE { species = SPECIES_PALKIA; }
    PARAMETRIZE { species = SPECIES_PALKIA_ORIGIN; }
    PARAMETRIZE { species = SPECIES_ARCEUS_NORMAL; }
    PARAMETRIZE { species = SPECIES_ARCEUS_GHOST; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    struct Pokemon mon;
    CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 slot = GetMonData(&mon, MON_DATA_ABILITY_NUM);
    EXPECT_EQ(GetMonAbility(&mon), GetSpeciesAbility(species, slot));
    u32 item = ITEM_APEX_METEORITE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(GetMonAbility(&mon), GetSpeciesApexAbility(species));
    item = ITEM_NONE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(GetMonAbility(&mon), GetSpeciesAbility(species, slot));
}

TEST("Apex Meteorite is a protected nonconsumable held item using Meteorite art")
{
    EXPECT_EQ(GetItemImportance(ITEM_APEX_METEORITE), 1);
    EXPECT_EQ(GetItemPrice(ITEM_APEX_METEORITE), 0);
    EXPECT_EQ(GetItemPocket(ITEM_APEX_METEORITE), POCKET_ITEMS);
    EXPECT_EQ(GetItemHoldEffect(ITEM_APEX_METEORITE), HOLD_EFFECT_APEX_METEORITE);
    EXPECT_EQ((u32)gItemsInfo[ITEM_APEX_METEORITE].notConsumed, TRUE);
    EXPECT_EQ((u32)gItemsInfo[ITEM_APEX_METEORITE].flingPower, 0);
    EXPECT_EQ(gItemsInfo[ITEM_APEX_METEORITE].iconPic, gItemsInfo[ITEM_METEORITE].iconPic);
    EXPECT_EQ(gItemsInfo[ITEM_APEX_METEORITE].iconPalette, gItemsInfo[ITEM_METEORITE].iconPalette);
}
