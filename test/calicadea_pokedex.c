#include "global.h"
#include "pokedex.h"
#include "pokemon.h"
#include "text.h"
#include "test/test.h"

#define CALICADEA_NATIONAL_ENTRY(name) NATIONAL_DEX_ ##name,
static const enum NationalDexOrder sCalicadeaNationalOrder[] = {
    FOREACH_SPECIES_IN_CALICADEA_DEX_ORDER(CALICADEA_NATIONAL_ENTRY)
};
#undef CALICADEA_NATIONAL_ENTRY

TEST("Calicadea Dex preserves the supplied 329-entry order without adding evolutions")
{
    EXPECT_EQ(CALICADEA_DEX_SIZE, 329);
    EXPECT_EQ(ARRAY_COUNT(sCalicadeaNationalOrder), 329);
    EXPECT_EQ(REGIONAL_DEX_COUNT, CALICADEA_DEX_COUNT);
    EXPECT_EQ(DEX_MODE_CALICADEA, 0); // Preserve the saved regional mode value.
    EXPECT_EQ(CALICADEA_DEX_SNIVY, 1);
    EXPECT_EQ(CALICADEA_DEX_SEEL, 40);
    EXPECT_EQ(CALICADEA_DEX_FLOETTE, 161);
    EXPECT_EQ(CALICADEA_DEX_DACHSBUN, 172);
    EXPECT_EQ(CALICADEA_DEX_NIDORAN_F, 218);
    EXPECT_EQ(CALICADEA_DEX_NIDORAN_M, 221);
    EXPECT_EQ(CALICADEA_DEX_BELDUM, 273);
    EXPECT_EQ(CALICADEA_DEX_BELBUOY, 276);
    EXPECT_EQ(CALICADEA_DEX_TUGTANG, 277);
    EXPECT_EQ(CALICADEA_DEX_METTLESHIP, 278);
    EXPECT_EQ(CALICADEA_DEX_SUBMAGROSS, 279);
    EXPECT_EQ(CALICADEA_DEX_DEOXYS, 329);
    EXPECT_EQ(NationalToRegionalOrder(NATIONAL_DEX_FLABEBE), 0);
    EXPECT_EQ(NationalToRegionalOrder(NATIONAL_DEX_FIDOUGH), 0);
    EXPECT_EQ(NationalToRegionalOrder(NATIONAL_DEX_DUBWOOL), 0);
}

TEST("Calicadea Dex mappings are unique and round-trip through species and national numbers")
{
    for (u32 i = 0; i < ARRAY_COUNT(sCalicadeaNationalOrder); i++) {
        enum NationalDexOrder national = sCalicadeaNationalOrder[i];
        enum Species species = NationalPokedexNumToSpecies(national);
        EXPECT(IsSpeciesEnabled(species));
        EXPECT_EQ(RegionalToNationalOrder(i + 1), national);
        EXPECT_EQ(CalicadeaToNationalOrder(i + 1), national);
        EXPECT_EQ(NationalToRegionalOrder(national), i + 1);
        EXPECT_EQ(NationalToCalicadeaOrder(national), i + 1);
        EXPECT_EQ(SpeciesToRegionalPokedexNum(species), i + 1);
        EXPECT_EQ(SpeciesToCalicadeaPokedexNum(species), i + 1);
        EXPECT(IsSpeciesInRegionalDex(species));
        EXPECT(IsSpeciesInCalicadeaDex(species));
        for (u32 j = 0; j < i; j++)
            EXPECT_NE(sCalicadeaNationalOrder[j], national);
    }
    EXPECT_EQ(RegionalToNationalOrder(0), NATIONAL_DEX_NONE);
    EXPECT_EQ(RegionalToNationalOrder(CALICADEA_DEX_COUNT), NATIONAL_DEX_NONE);
    EXPECT_EQ(NationalToRegionalOrder(NATIONAL_DEX_NONE), 0);
    EXPECT_EQ(CalicadeaToNationalOrder(0), NATIONAL_DEX_NONE);
    EXPECT_EQ(CalicadeaToNationalOrder(CALICADEA_DEX_COUNT), NATIONAL_DEX_NONE);
    EXPECT_EQ(SpeciesToCalicadeaPokedexNum(SPECIES_NONE), CALICADEA_DEX_NONE);
    EXPECT(!IsSpeciesInRegionalDex(SPECIES_NONE));
    EXPECT(!IsSpeciesInRegionalDex(SPECIES_EGG));
    EXPECT(!IsSpeciesInRegionalDex(SPECIES_TREECKO));
    EXPECT_EQ(SpeciesToRegionalPokedexNum(SPECIES_METAGROSS_MEGA), CALICADEA_DEX_METAGROSS);
}

TEST("Calicadea Dex seen and caught counts use the new regional roster")
{
    u8 savedSeen[NUM_DEX_FLAG_BYTES], savedCaught[NUM_DEX_FLAG_BYTES];
    memcpy(savedSeen, gSaveData.dexSeen, sizeof(savedSeen));
    memcpy(savedCaught, gSaveData.dexCaught, sizeof(savedCaught));
    memset(gSaveData.dexSeen, 0, sizeof(gSaveData.dexSeen));
    memset(gSaveData.dexCaught, 0, sizeof(gSaveData.dexCaught));

    GetSetPokedexFlag(NATIONAL_DEX_SNIVY, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_BELBUOY, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_SUBMAGROSS, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_TREECKO, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_SNIVY, FLAG_SET_CAUGHT);
    GetSetPokedexFlag(NATIONAL_DEX_BELBUOY, FLAG_SET_CAUGHT);
    GetSetPokedexFlag(NATIONAL_DEX_TREECKO, FLAG_SET_CAUGHT);
    u32 seen = GetRegionalPokedexCount(FLAG_GET_SEEN);
    u32 caught = GetRegionalPokedexCount(FLAG_GET_CAUGHT);
    u32 calicadeaSeen = GetCalicadeaPokedexCount(FLAG_GET_SEEN);
    u32 calicadeaCaught = GetCalicadeaPokedexCount(FLAG_GET_CAUGHT);

    memcpy(gSaveData.dexSeen, savedSeen, sizeof(savedSeen));
    memcpy(gSaveData.dexCaught, savedCaught, sizeof(savedCaught));
    EXPECT_EQ(seen, 3);
    EXPECT_EQ(caught, 2);
    EXPECT_EQ(calicadeaSeen, seen);
    EXPECT_EQ(calicadeaCaught, caught);
}

TEST("Calicadea Dex completion includes custom species and preserves the mythical exemption")
{
    u8 savedCaught[NUM_DEX_FLAG_BYTES];
    memcpy(savedCaught, gSaveData.dexCaught, sizeof(savedCaught));
    memset(gSaveData.dexCaught, 0, sizeof(gSaveData.dexCaught));
    for (u32 i = 0; i < ARRAY_COUNT(sCalicadeaNationalOrder); i++) {
        enum NationalDexOrder dex = sCalicadeaNationalOrder[i];
        if (dex != NATIONAL_DEX_DEOXYS && dex != NATIONAL_DEX_SUBMAGROSS)
            GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    }
    bool32 missingCustom = HasAllRegionalMons();
    GetSetPokedexFlag(NATIONAL_DEX_SUBMAGROSS, FLAG_SET_CAUGHT);
    bool32 complete = HasAllRegionalMons();
    bool32 calicadeaComplete = HasAllCalicadeaMons();
    memcpy(gSaveData.dexCaught, savedCaught, sizeof(savedCaught));

    EXPECT(!missingCustom);
    EXPECT(complete);
    EXPECT(calicadeaComplete);
}

TEST("Calicadea Dex text fits its sprite label and search-menu field")
{
    EXPECT_LE(GetStringWidth(FONT_SMALL_NARROWER, COMPOUND_STRING("Calicadea"), 0), 32);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, COMPOUND_STRING("Calicadea Dex"), 0), 90);
}
