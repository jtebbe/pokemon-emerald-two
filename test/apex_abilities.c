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
        switch (species)
        {
        case SPECIES_VOLTORB:
        case SPECIES_ELECTRODE:
            expected = ABILITY_I_CHOOSE_YOU;
            break;
        case SPECIES_VOLTORB_HISUI:
        case SPECIES_ELECTRODE_HISUI:
            expected = ABILITY_AMPED_BLOOM;
            break;
        case SPECIES_EXEGGCUTE:
        case SPECIES_EXEGGUTOR:
            expected = ABILITY_GREAT_MINDS;
            break;
        case SPECIES_EXEGGUTOR_ALOLA:
            expected = ABILITY_GRAVITY_FALLS;
            break;
        case SPECIES_CUBONE:
        case SPECIES_MAROWAK:
            expected = ABILITY_BONE_COLLECTOR;
            break;
        case SPECIES_MAROWAK_ALOLA:
            expected = ABILITY_SHADOW_DANCER;
            break;
        case SPECIES_TYROGUE:
            expected = ABILITY_TRAINING_PUPIL;
            break;
        case SPECIES_HITMONLEE:
            expected = ABILITY_KICKING_MASTER;
            break;
        case SPECIES_HITMONCHAN:
            expected = ABILITY_PUNCHING_MASTER;
            break;
        case SPECIES_HITMONTOP:
            expected = ABILITY_BALANCE_MASTER;
            break;
        case SPECIES_LICKITUNG:
        case SPECIES_LICKILICKY:
            expected = ABILITY_TONGUE_LASH;
            break;
        case SPECIES_KOFFING:
        case SPECIES_WEEZING:
            expected = ABILITY_NOXIOUS_CLOUD;
            break;
        case SPECIES_WEEZING_GALAR:
            expected = ABILITY_AIR_PURIFIER;
            break;
        case SPECIES_RHYHORN:
        case SPECIES_RHYDON:
        case SPECIES_RHYPERIOR:
            expected = ABILITY_ROCKY_CARAPACE;
            break;
        case SPECIES_HAPPINY:
        case SPECIES_CHANSEY:
        case SPECIES_BLISSEY:
            expected = ABILITY_PURE_BLISS;
            break;
        case SPECIES_TANGELA:
        case SPECIES_TANGROWTH:
            expected = ABILITY_TANGLEVINES;
            break;
        case SPECIES_KANGASKHAN:
        case SPECIES_KANGASKHAN_MEGA:
            expected = ABILITY_DOUBLE_TEAMING;
            break;
        case SPECIES_HORSEA:
        case SPECIES_SEADRA:
        case SPECIES_KINGDRA:
            expected = ABILITY_TIDAL_RULER;
            break;
        case SPECIES_GOLDEEN:
        case SPECIES_SEAKING:
            expected = ABILITY_PIERCING_HORN;
            break;
        case SPECIES_STARYU:
        case SPECIES_STARMIE:
        case SPECIES_STARMIE_MEGA:
            expected = ABILITY_UNKNOWN_ORIGIN;
            break;
        case SPECIES_MIME_JR:
        case SPECIES_MR_MIME:
            expected = ABILITY_MIME_BOX;
            break;
        case SPECIES_MR_MIME_GALAR:
        case SPECIES_MR_RIME:
            expected = ABILITY_YAOI_ON_ICE;
            break;
        case SPECIES_SCYTHER:
        case SPECIES_SCIZOR:
        case SPECIES_SCIZOR_MEGA:
        case SPECIES_KLEAVOR:
            expected = ABILITY_DECAPITATOR;
            break;
        case SPECIES_JYNX:
            expected = ABILITY_SNOW_SIREN;
            break;
        case SPECIES_ELEKID:
        case SPECIES_ELECTABUZZ:
        case SPECIES_ELECTIVIRE:
            expected = ABILITY_FULLY_CHARGED;
            break;
        case SPECIES_MAGBY:
        case SPECIES_MAGMAR:
        case SPECIES_MAGMORTAR:
            expected = ABILITY_SCORCHING_BLAST;
            break;
        case SPECIES_PINSIR:
        case SPECIES_PINSIR_MEGA:
            expected = ABILITY_SPIKY_GRIP;
            break;
        case SPECIES_TAUROS:
        case SPECIES_TAUROS_PALDEA_COMBAT:
            expected = ABILITY_FURIOUS_CHARGE;
            break;
        case SPECIES_TAUROS_PALDEA_BLAZE:
            expected = ABILITY_BLAZE_HERITAGE;
            break;
        case SPECIES_TAUROS_PALDEA_AQUA:
            expected = ABILITY_AQUA_HERITAGE;
            break;
        case SPECIES_MAGIKARP:
            expected = ABILITY_SACRIFICIAL_CARP;
            break;
        case SPECIES_GYARADOS:
        case SPECIES_GYARADOS_MEGA:
            expected = ABILITY_MINDLESS_RAMPAGE;
            break;
        case SPECIES_LAPRAS:
        case SPECIES_LAPRAS_MEGA:
            expected = ABILITY_SEA_GUARDIAN;
            break;
        case SPECIES_DITTO:
            expected = ABILITY_TRUE_ADAPTATION;
            break;
        case SPECIES_PORYGON:
        case SPECIES_PORYGON2:
        case SPECIES_PORYGON_Z:
            expected = ABILITY_ENIGMA_CODE;
            break;
        case SPECIES_OMANYTE:
        case SPECIES_OMASTAR:
            expected = ABILITY_ANCIENT_HELIX;
            break;
        case SPECIES_KABUTO:
        case SPECIES_KABUTOPS:
            expected = ABILITY_HONED_HUNTER;
            break;
        case SPECIES_AERODACTYL:
        case SPECIES_AERODACTYL_MEGA:
            expected = ABILITY_AERIAL_TERROR;
            break;
        case SPECIES_SNORLAX:
        case SPECIES_SNORLAX_MEGA:
            expected = ABILITY_HANGER_ATTACK;
            break;
        case SPECIES_ARTICUNO:
            expected = ABILITY_ICEBORN;
            break;
        case SPECIES_ARTICUNO_GALAR:
            expected = ABILITY_FRIGID_STARE;
            break;
        case SPECIES_ZAPDOS:
            expected = ABILITY_SHOCKBORN;
            break;
        case SPECIES_ZAPDOS_GALAR:
            expected = ABILITY_RAPID_RUNNER;
            break;
        case SPECIES_MOLTRES:
            expected = ABILITY_FLAMEBORN;
            break;
        case SPECIES_MOLTRES_GALAR:
            expected = ABILITY_BLACKENED_WINGS;
            break;
        case SPECIES_DRATINI:
        case SPECIES_DRAGONAIR:
        case SPECIES_DRAGONITE:
        case SPECIES_DRAGONITE_MEGA:
            expected = ABILITY_GENTLE_SOUL;
            break;
        case SPECIES_MEWTWO:
        case SPECIES_MEWTWO_MEGA_X:
        case SPECIES_MEWTWO_MEGA_Y:
            expected = ABILITY_OVERWHELM;
            break;
        case SPECIES_MEW:
            expected = ABILITY_MUTUAL_ORIGIN;
            break;
        case SPECIES_EEVEE:
            expected = ABILITY_MULTIVOLVE;
            break;
        case SPECIES_JOLTEON:
            expected = ABILITY_HIGH_VOLTAGE;
            break;
        case SPECIES_VAPOREON:
            expected = ABILITY_AQUATIC_FORM;
            break;
        case SPECIES_FLAREON:
            expected = ABILITY_INNER_FURNACE;
            break;
        case SPECIES_LEAFEON:
            expected = ABILITY_PHOTOSYNTHETIC;
            break;
        case SPECIES_GLACEON:
            expected = ABILITY_FROSTED_FUR;
            break;
        case SPECIES_ESPEON:
            expected = ABILITY_PREDICTIVE_MIND;
            break;
        case SPECIES_UMBREON:
            expected = ABILITY_UMBRAL_THIRST;
            break;
        case SPECIES_SYLVEON:
            expected = ABILITY_RAINBOW_SKIES;
            break;
        case SPECIES_DROWZEE:
        case SPECIES_HYPNO:
            expected = ABILITY_HYPNOTIST;
            break;
        case SPECIES_KRABBY:
        case SPECIES_KINGLER:
        case SPECIES_KINGLER_MEGA:
            expected = ABILITY_BIG_MEATY_CLAWS;
            break;
        case SPECIES_ONIX:
        case SPECIES_STEELIX:
        case SPECIES_STEELIX_MEGA:
            expected = ABILITY_WEIGHTED_ASSAULT;
            break;
        case SPECIES_GASTLY:
        case SPECIES_HAUNTER:
        case SPECIES_GENGAR:
        case SPECIES_GENGAR_MEGA:
            expected = ABILITY_EVIL_TRICKSTER;
            break;
        case SPECIES_SHELLDER:
        case SPECIES_CLOYSTER:
            expected = ABILITY_SHELL_BURST;
            break;
        case SPECIES_GRIMER_ALOLA:
        case SPECIES_MUK_ALOLA:
            expected = ABILITY_CHEMICAL_ODDITY;
            break;
        case SPECIES_GRIMER:
        case SPECIES_MUK:
            expected = ABILITY_SICKENING_STENCH;
            break;
        case SPECIES_SEEL:
        case SPECIES_DEWGONG:
            expected = ABILITY_ADAPTIVE_BUILD;
            break;
        case SPECIES_DODUO:
        case SPECIES_DODRIO:
            expected = ABILITY_ROADRUNNIN;
            break;
        case SPECIES_FARFETCHD:
            expected = ABILITY_LEEK_MASTER;
            break;
        case SPECIES_FARFETCHD_GALAR:
        case SPECIES_SIRFETCHD:
            expected = ABILITY_NOBLESSE_OBLIGE;
            break;
        case SPECIES_MAGNEMITE:
        case SPECIES_MAGNETON:
        case SPECIES_MAGNEZONE:
            expected = ABILITY_MAGLEV_POLARITY;
            break;
        case SPECIES_SLOWPOKE_GALAR:
        case SPECIES_SLOWBRO_GALAR:
            expected = ABILITY_SPICED_DREAMS;
            break;
        case SPECIES_SLOWKING:
            expected = ABILITY_SEA_HERALD;
            break;
        case SPECIES_SLOWKING_GALAR:
            expected = ABILITY_VOODOO_STRENGTH;
            break;
        case SPECIES_CASTFORM_NORMAL:
        case SPECIES_CASTFORM_SUNNY:
        case SPECIES_CASTFORM_RAINY:
        case SPECIES_CASTFORM_SNOWY:
            expected = ABILITY_FICKLE_FORECASTS;
            break;
        case SPECIES_KECLEON:
            expected = ABILITY_TECHNICOLOR;
            break;
        case SPECIES_FROSLASS:
        case SPECIES_FROSLASS_MEGA:
            expected = ABILITY_FROSTFELT_HATRED;
            break;
        case SPECIES_SLOWPOKE:
        case SPECIES_SLOWBRO:
        case SPECIES_SLOWBRO_MEGA:
            expected = ABILITY_TOTAL_ZEN;
            break;
        case SPECIES_PONYTA:
        case SPECIES_RAPIDASH:
            expected = ABILITY_BLAZING_RUSH;
            break;
        case SPECIES_PONYTA_GALAR:
        case SPECIES_RAPIDASH_GALAR:
            expected = ABILITY_RAINBOW_RIDER;
            break;
        case SPECIES_SQUIRTLE:
        case SPECIES_WARTORTLE:
        case SPECIES_BLASTOISE:
        case SPECIES_BLASTOISE_MEGA:
            expected = ABILITY_SHELL_INVERSION;
            break;
        case SPECIES_CATERPIE:
        case SPECIES_METAPOD:
        case SPECIES_WEEDLE:
        case SPECIES_KAKUNA:
            expected = ABILITY_STICKY_SPRAY;
            break;
        case SPECIES_BUTTERFREE:
        case SPECIES_BUTTERFREE_MEGA:
            expected = ABILITY_PSISIGHT;
            break;
        case SPECIES_BEEDRILL:
        case SPECIES_BEEDRILL_MEGA:
            expected = ABILITY_VICIOUS_STINGS;
            break;
        case SPECIES_PIDGEY:
        case SPECIES_PIDGEOTTO:
        case SPECIES_PIDGEOT:
        case SPECIES_PIDGEOT_MEGA:
            expected = ABILITY_MACH_FLIGHT;
            break;
        case SPECIES_RATTATA:
        case SPECIES_RATICATE:
            expected = ABILITY_GNASHING_TEETH;
            break;
        case SPECIES_RATTATA_ALOLA:
        case SPECIES_RATICATE_ALOLA:
        case SPECIES_RATICATE_ALOLA_TOTEM:
            expected = ABILITY_INFINITE_STASH;
            break;
        case SPECIES_SPEAROW:
        case SPECIES_FEAROW:
            expected = ABILITY_PIERCING_BEAK;
            break;
        case SPECIES_ARBOK:
            expected = ABILITY_SERPENTS_RETORT;
            break;
        case SPECIES_PICHU:
        case SPECIES_PICHU_SPIKY_EARED:
        case SPECIES_PIKACHU:
        case SPECIES_PIKACHU_COSPLAY:
        case SPECIES_PIKACHU_ROCK_STAR:
        case SPECIES_PIKACHU_BELLE:
        case SPECIES_PIKACHU_POP_STAR:
        case SPECIES_PIKACHU_PHD:
        case SPECIES_PIKACHU_LIBRE:
        case SPECIES_PIKACHU_ORIGINAL:
        case SPECIES_PIKACHU_HOENN:
        case SPECIES_PIKACHU_SINNOH:
        case SPECIES_PIKACHU_UNOVA:
        case SPECIES_PIKACHU_KALOS:
        case SPECIES_PIKACHU_ALOLA:
        case SPECIES_PIKACHU_PARTNER:
        case SPECIES_PIKACHU_WORLD:
        case SPECIES_PIKACHU_STARTER:
        case SPECIES_RAICHU:
        case SPECIES_RAICHU_MEGA_X:
        case SPECIES_RAICHU_MEGA_Y:
            expected = ABILITY_PEAK_VOLTAGE;
            break;
        case SPECIES_SANDSHREW:
        case SPECIES_SANDSLASH:
            expected = ABILITY_DUST_TO_DUST;
            break;
        case SPECIES_SANDSHREW_ALOLA:
        case SPECIES_SANDSLASH_ALOLA:
            expected = ABILITY_FROZEN_SHRAPNEL;
            break;
        case SPECIES_NIDORAN_F:
        case SPECIES_NIDORINA:
        case SPECIES_NIDOQUEEN:
            expected = ABILITY_EARTHEN_MOTHER;
            break;
        case SPECIES_NIDORAN_M:
        case SPECIES_NIDORINO:
        case SPECIES_NIDOKING:
            expected = ABILITY_BATTLE_KING;
            break;
        case SPECIES_CLEFFA:
        case SPECIES_CLEFAIRY:
        case SPECIES_CLEFABLE:
        case SPECIES_CLEFABLE_MEGA:
            expected = ABILITY_MOON_ATTUNED;
            break;
        case SPECIES_VULPIX:
        case SPECIES_NINETALES:
            expected = ABILITY_NINE_TAIL_LEGEND;
            break;
        case SPECIES_VULPIX_ALOLA:
        case SPECIES_NINETALES_ALOLA:
            expected = ABILITY_AURORAL_HERALD;
            break;
        case SPECIES_IGGLYBUFF:
        case SPECIES_JIGGLYPUFF:
        case SPECIES_WIGGLYTUFF:
            expected = ABILITY_LULLING_MELODIES;
            break;
        case SPECIES_POLITOED:
            expected = ABILITY_DROPS_OF_GOD;
            break;
        case SPECIES_TENTACOOL:
        case SPECIES_TENTACRUEL:
            expected = ABILITY_ABYSSAL_EMBRACE;
            break;
        case SPECIES_GEODUDE:
        case SPECIES_GRAVELER:
        case SPECIES_GOLEM:
            expected = ABILITY_UNBROKEN_STONE;
            break;
        case SPECIES_GEODUDE_ALOLA:
        case SPECIES_GRAVELER_ALOLA:
        case SPECIES_GOLEM_ALOLA:
            expected = ABILITY_CONDUCTION_ZONE;
            break;
        case SPECIES_BELLSPROUT:
        case SPECIES_WEEPINBELL:
        case SPECIES_VICTREEBEL:
        case SPECIES_VICTREEBEL_MEGA:
            expected = ABILITY_SUCCULENT_TRAP;
            break;
        case SPECIES_MACHOP:
        case SPECIES_MACHOKE:
        case SPECIES_MACHAMP:
        case SPECIES_MACHAMP_MEGA:
            expected = ABILITY_TRUE_POWER;
            break;
        case SPECIES_ABRA:
        case SPECIES_KADABRA:
        case SPECIES_ALAKAZAM:
        case SPECIES_ALAKAZAM_MEGA:
            expected = ABILITY_MENTAL_MASTER;
            break;
        case SPECIES_POLIWAG:
        case SPECIES_POLIWHIRL:
        case SPECIES_POLIWRATH:
            expected = ABILITY_HYPNOTIC_PATTERN;
            break;
        case SPECIES_GROWLITHE:
        case SPECIES_ARCANINE:
            expected = ABILITY_GUARDIANS_GROWL;
            break;
        case SPECIES_GROWLITHE_HISUI:
        case SPECIES_ARCANINE_HISUI:
            expected = ABILITY_RELENTLESS_CHASE;
            break;
        case SPECIES_PSYDUCK:
        case SPECIES_GOLDUCK:
            expected = ABILITY_RIVER_ESPER;
            break;
        case SPECIES_MANKEY:
        case SPECIES_PRIMEAPE:
        case SPECIES_ANNIHILAPE:
            expected = ABILITY_FISTS_OF_FURY;
            break;
        case SPECIES_MEOWTH_ALOLA:
        case SPECIES_PERSIAN_ALOLA:
            expected = ABILITY_SADISTIC_STREAK;
            break;
        case SPECIES_MEOWTH_GALAR:
        case SPECIES_PERRSERKER:
            expected = ABILITY_SCRAP_BRAWLER;
            break;
        case SPECIES_MEOWTH:
        case SPECIES_PERSIAN:
            expected = ABILITY_SICKENING_GREED;
            break;
        case SPECIES_DIGLETT_ALOLA:
        case SPECIES_DUGTRIO_ALOLA:
            expected = ABILITY_GLORIOUS_MANE;
            break;
        case SPECIES_DIGLETT:
        case SPECIES_DUGTRIO:
            expected = ABILITY_TUNNEL_TROUBLE;
            break;
        case SPECIES_VENONAT:
        case SPECIES_VENOMOTH:
            expected = ABILITY_MEPHITIC_DUST;
            break;
        case SPECIES_PARAS:
        case SPECIES_PARASECT:
            expected = ABILITY_MUSHROOM_MADNESS;
            break;
        case SPECIES_BELLOSSOM:
            expected = ABILITY_ENDLESS_LUAU;
            break;
        case SPECIES_ODDISH:
        case SPECIES_GLOOM:
        case SPECIES_VILEPLUME:
            expected = ABILITY_SPORE_CLOUD;
            break;
        case SPECIES_ZUBAT:
        case SPECIES_GOLBAT:
        case SPECIES_CROBAT:
            expected = ABILITY_VAMPIRISM;
            break;
        case SPECIES_BELDUM:
        case SPECIES_METANG:
        case SPECIES_METAGROSS:
            expected = ABILITY_QUANTUM_THOUGHT;
            break;
        default:
            break;
        }
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
    PARAMETRIZE { species = SPECIES_SQUIRTLE; }
    PARAMETRIZE { species = SPECIES_CATERPIE; }
    PARAMETRIZE { species = SPECIES_BUTTERFREE; }
    PARAMETRIZE { species = SPECIES_BEEDRILL; }
    PARAMETRIZE { species = SPECIES_PIDGEY; }
    PARAMETRIZE { species = SPECIES_RATTATA; }
    PARAMETRIZE { species = SPECIES_RATTATA_ALOLA; }
    PARAMETRIZE { species = SPECIES_SPEAROW; }
    PARAMETRIZE { species = SPECIES_ARBOK; }
    PARAMETRIZE { species = SPECIES_PICHU; }
    PARAMETRIZE { species = SPECIES_SANDSHREW; }
    PARAMETRIZE { species = SPECIES_SANDSHREW_ALOLA; }
    PARAMETRIZE { species = SPECIES_NIDORAN_F; }
    PARAMETRIZE { species = SPECIES_NIDORINA; }
    PARAMETRIZE { species = SPECIES_NIDOQUEEN; }
    PARAMETRIZE { species = SPECIES_NIDORAN_M; }
    PARAMETRIZE { species = SPECIES_NIDORINO; }
    PARAMETRIZE { species = SPECIES_NIDOKING; }
    PARAMETRIZE { species = SPECIES_CLEFFA; }
    PARAMETRIZE { species = SPECIES_CLEFAIRY; }
    PARAMETRIZE { species = SPECIES_CLEFABLE; }
    PARAMETRIZE { species = SPECIES_CLEFABLE_MEGA; }
    PARAMETRIZE { species = SPECIES_IGGLYBUFF; }
    PARAMETRIZE { species = SPECIES_JIGGLYPUFF; }
    PARAMETRIZE { species = SPECIES_WIGGLYTUFF; }
    PARAMETRIZE { species = SPECIES_VULPIX_ALOLA; }
    PARAMETRIZE { species = SPECIES_NINETALES_ALOLA; }
    PARAMETRIZE { species = SPECIES_VULPIX; }
    PARAMETRIZE { species = SPECIES_NINETALES; }
    PARAMETRIZE { species = SPECIES_BELDUM; }
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
