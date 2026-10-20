#include "global.h"
#include "daycare.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"

TEST("Belbuoy family species have unique names, IDs and Steel/Water typing")
{
    enum Species species;
    enum NationalDexOrder dexNum;
    const u8 *name;
    PARAMETRIZE { species = SPECIES_BELBUOY; dexNum = NATIONAL_DEX_BELBUOY; name = COMPOUND_STRING("Belbuoy"); }
    PARAMETRIZE { species = SPECIES_TUGTANG; dexNum = NATIONAL_DEX_TUGTANG; name = COMPOUND_STRING("Tugtang"); }
    PARAMETRIZE { species = SPECIES_METTLESHIP; dexNum = NATIONAL_DEX_METTLESHIP; name = COMPOUND_STRING("Mettleship"); }
    PARAMETRIZE { species = SPECIES_SUBMAGROSS; dexNum = NATIONAL_DEX_SUBMAGROSS; name = COMPOUND_STRING("Submagross"); }

    EXPECT(IsSpeciesEnabled(species));
    EXPECT_GT(species, SPECIES_CUSTOM_START);
    EXPECT_EQ(GetSpeciesType(species, 0), TYPE_STEEL);
    EXPECT_EQ(GetSpeciesType(species, 1), TYPE_WATER);
    EXPECT_EQ(StringCompare(GetSpeciesName(species), name), 0);
    EXPECT_EQ(SpeciesToNationalPokedexNum(species), dexNum);
    EXPECT_EQ(NationalPokedexNumToSpecies(dexNum), species);
    EXPECT_EQ(gSpeciesInfo[species].formSpeciesIdTable, NULL);
    EXPECT_EQ(gSpeciesInfo[species].formChangeTable, NULL);
}

TEST("Belbuoy family placeholders match the Beldum family stats, resources and learnsets")
{
    enum Species species, original;
    PARAMETRIZE { species = SPECIES_BELBUOY; original = SPECIES_BELDUM; }
    PARAMETRIZE { species = SPECIES_TUGTANG; original = SPECIES_METANG; }
    PARAMETRIZE { species = SPECIES_METTLESHIP; original = SPECIES_METAGROSS; }
    PARAMETRIZE { species = SPECIES_SUBMAGROSS; original = SPECIES_METAGROSS; }

    for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetSpeciesBaseStat(species, stat), GetSpeciesBaseStat(original, stat));
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        EXPECT_EQ(GetSpeciesAbility(species, slot), GetSpeciesAbility(original, slot));
    EXPECT_EQ(gSpeciesInfo[species].frontPic, gSpeciesInfo[original].frontPic);
    EXPECT_EQ(gSpeciesInfo[species].backPic, gSpeciesInfo[original].backPic);
    EXPECT_EQ(gSpeciesInfo[species].palette, gSpeciesInfo[original].palette);
    EXPECT_EQ(gSpeciesInfo[species].shinyPalette, gSpeciesInfo[original].shinyPalette);
    EXPECT_EQ(gSpeciesInfo[species].iconSprite, gSpeciesInfo[original].iconSprite);
    EXPECT_EQ((u32)gSpeciesInfo[species].cryId, (u32)gSpeciesInfo[original].cryId);

    const struct LevelUpMove *moves = GetSpeciesLevelUpLearnset(species);
    const struct LevelUpMove *originalMoves = GetSpeciesLevelUpLearnset(original);
    for (u32 j = 0; ; j++) {
        EXPECT_EQ(moves[j].move, originalMoves[j].move);
        EXPECT_EQ(moves[j].level, originalMoves[j].level);
        if (moves[j].move == MOVE_UNAVAILABLE)
            break;
    }

    const u16 *teachables = GetSpeciesTeachableLearnset(species);
    const u16 *originalTeachables = GetSpeciesTeachableLearnset(original);
    for (u32 j = 0; ; j++) {
        EXPECT_EQ(teachables[j], originalTeachables[j]);
        if (teachables[j] == MOVE_UNAVAILABLE)
            break;
    }
}

TEST("Belbuoy evolves into Tugtang and Tugtang branches by level or Water Stone")
{
    const struct Evolution *evolutions = GetSpeciesEvolutions(SPECIES_BELBUOY);
    EXPECT(evolutions != NULL);
    EXPECT_EQ(evolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(evolutions[0].param, 20);
    EXPECT_EQ(evolutions[0].targetSpecies, SPECIES_TUGTANG);
    EXPECT_EQ(evolutions[1].method, EVOLUTIONS_END);

    evolutions = GetSpeciesEvolutions(SPECIES_TUGTANG);
    EXPECT(evolutions != NULL);
    EXPECT_EQ(evolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(evolutions[0].param, 45);
    EXPECT_EQ(evolutions[0].targetSpecies, SPECIES_METTLESHIP);
    EXPECT_EQ(evolutions[1].method, EVO_ITEM);
    EXPECT_EQ(evolutions[1].param, ITEM_WATER_STONE);
    EXPECT_EQ(evolutions[1].targetSpecies, SPECIES_SUBMAGROSS);
    EXPECT_EQ(evolutions[2].method, EVOLUTIONS_END);
    EXPECT_EQ(GetSpeciesEvolutions(SPECIES_METTLESHIP), NULL);
    EXPECT_EQ(GetSpeciesEvolutions(SPECIES_SUBMAGROSS), NULL);
}

TEST("Belbuoy family breeding produces Belbuoy with an empty placeholder egg learnset")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_BELBUOY; }
    PARAMETRIZE { species = SPECIES_TUGTANG; }
    PARAMETRIZE { species = SPECIES_METTLESHIP; }
    PARAMETRIZE { species = SPECIES_SUBMAGROSS; }

    EXPECT_EQ(GetEggSpecies(species), SPECIES_BELBUOY);
    EXPECT_EQ(GetSpeciesEggMoves(SPECIES_BELBUOY)[0], MOVE_UNAVAILABLE);
}
