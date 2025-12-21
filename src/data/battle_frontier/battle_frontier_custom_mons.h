/*
*
*
*
*
*
* The following mons are used for the battle factory but are also available as general frontier mons.
*
*
*
*
*
*/

const struct TrainerMon gCustomBattleFrontierMons[NUM_CUSTOM_FRONTIER_GENERIC_MONS] = {
    [CUSTOM_FRONTIER_MON_VENUSAUR_GENERIC_1] = {
        .species = SPECIES_VENUSAUR,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(224, 0, 0, 32, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_PROTECT, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_VENUSAUR_GENERIC_2] = {
        .species = SPECIES_VENUSAUR,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_INGRAIN, MOVE_TOXIC, MOVE_PROTECT, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_CHARIZARD_GENERIC_1] = {
        .species = SPECIES_CHARIZARD,
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HURRICANE, MOVE_FIRE_BLAST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CHARIZARD_GENERIC_2] = {
        .species = SPECIES_CHARIZARD,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HEAT_WAVE, MOVE_PROTECT, MOVE_WILL_O_WISP}
    },
    [CUSTOM_FRONTIER_MON_BLASTOISE_GENERIC_1] = {
        .species = SPECIES_BLASTOISE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_HEAVY_ARMOR,
        .ev = TRAINER_PARTY_EVS(252, 76, 180, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_FAKE_OUT, MOVE_SEISMIC_TOSS, MOVE_DRAGON_TAIL, MOVE_LIQUIDATION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BLASTOISE_GENERIC_2] = {
        .species = SPECIES_BLASTOISE,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_TORRENT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_BUTTERFREE_GENERIC_1] = {
        .species = SPECIES_BUTTERFREE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_COMPOUND_EYES,
        .ev = TRAINER_PARTY_EVS(136, 0, 0, 120, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_SLEEP_POWDER, MOVE_BUG_BUZZ, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_BUTTERFREE_GENERIC_2] = {
        .species = SPECIES_BUTTERFREE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_COMPOUND_EYES,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_RAGE_POWDER, MOVE_TAILWIND, MOVE_PSYCHIC}
    },
    [CUSTOM_FRONTIER_MON_BEEDRILL_GENERIC_1] = {
        .species = SPECIES_BEEDRILL,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SNIPER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FIRST_IMPRESSION, MOVE_POISON_JAB, MOVE_TAILWIND, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BEEDRILL_GENERIC_2] = {
        .species = SPECIES_BEEDRILL,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SNIPER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRILL_RUN, MOVE_U_TURN, MOVE_POISON_JAB, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PIDGEOT_GENERIC_1] = {
        .species = SPECIES_PIDGEOT,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_NO_GUARD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HURRICANE, MOVE_HYPER_BEAM, MOVE_HEAT_WAVE, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_PIDGEOT_GENERIC_2] = {
        .species = SPECIES_PIDGEOT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_NO_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HURRICANE, MOVE_TAILWIND, MOVE_DETECT, MOVE_HEAT_WAVE}
    },
    [CUSTOM_FRONTIER_MON_RATICATE_GENERIC_1] = {
        .species = SPECIES_RATICATE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HIT_AND_RUN, MOVE_FOLLOW_ME, MOVE_ENCORE, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RATICATE_GENERIC_2] = {
        .species = SPECIES_RATICATE,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOLLOW_ME, MOVE_SUPER_FANG, MOVE_THUNDER_WAVE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_RATICATE_ALOLA_GENERIC_1] = {
        .species = SPECIES_RATICATE_ALOLA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(252, 4, 252, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_FOLLOW_ME, MOVE_ENCORE, MOVE_SUPER_FANG, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RATICATE_ALOLA_GENERIC_2] = {
        .species = SPECIES_RATICATE_ALOLA,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_ENCORE, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FEAROW_GENERIC_1] = {
        .species = SPECIES_FEAROW,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SNIPER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRILL_PECK, MOVE_DRILL_RUN, MOVE_DOUBLE_EDGE, MOVE_THROAT_CHOP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FEAROW_GENERIC_2] = {
        .species = SPECIES_FEAROW,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_DRILL_PECK, MOVE_PROTECT, MOVE_GROWL},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARBOK_GENERIC_1] = {
        .species = SPECIES_ARBOK,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_COIL, MOVE_SUCKER_PUNCH, MOVE_GLARE, MOVE_CAUSTIC_SPIT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARBOK_GENERIC_2] = {
        .species = SPECIES_ARBOK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 4, 252, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TRAILBLAZE, MOVE_SUCKER_PUNCH, MOVE_THROAT_CHOP, MOVE_CAUSTIC_SPIT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAICHU_GENERIC_1] = {
        .species = SPECIES_RAICHU,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .moves = {MOVE_FAKE_OUT, MOVE_NUZZLE, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAICHU_GENERIC_2] = {
        .species = SPECIES_RAICHU,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_NASTY_PLOT, MOVE_GRASS_KNOT, MOVE_THUNDERBOLT}
    },
    [CUSTOM_FRONTIER_MON_RAICHU_ALOLA_GENERIC_1] = {
        .species = SPECIES_RAICHU_ALOLA,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SURGE_SURFER,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PSYSHOCK, MOVE_THUNDERBOLT, MOVE_GRASS_KNOT, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_RAICHU_ALOLA_GENERIC_2] = {
        .species = SPECIES_RAICHU_ALOLA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SURGE_SURFER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_NUZZLE, MOVE_SEISMIC_TOSS, MOVE_VOLT_SWITCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SANDSLASH_GENERIC_1] = {
        .species = SPECIES_SANDSLASH,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SAND_SPIT,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAPID_SPIN, MOVE_EARTHQUAKE, MOVE_GUNK_SHOT, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SANDSLASH_GENERIC_2] = {
        .species = SPECIES_SANDSLASH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SAND_SPIT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_RAPID_SPIN, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SANDSLASH_ALOLA_GENERIC_1] = {
        .species = SPECIES_SANDSLASH_ALOLA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SNOW_BLOW,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RAPID_SPIN, MOVE_SWORDS_DANCE, MOVE_ICICLE_CRASH, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SANDSLASH_ALOLA_GENERIC_2] = {
        .species = SPECIES_SANDSLASH_ALOLA,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_SNOW_BLOW,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RAPID_SPIN, MOVE_SWORDS_DANCE, MOVE_ICICLE_CRASH, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_NIDOQUEEN_GENERIC_1] = {
        .species = SPECIES_NIDOQUEEN,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_NIDOQUEEN_GENERIC_2] = {
        .species = SPECIES_NIDOQUEEN,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_POISON_POINT,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_FISSURE, MOVE_HELPING_HAND, MOVE_TOXIC}
    },
    [CUSTOM_FRONTIER_MON_NIDOKING_GENERIC_1] = {
        .species = SPECIES_NIDOKING,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICE_BEAM, MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_NIDOKING_GENERIC_2] = {
        .species = SPECIES_NIDOKING,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_SLIDE, MOVE_POISON_JAB, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CLEFABLE_GENERIC_1] = {
        .species = SPECIES_CLEFABLE,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_MAGIC_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MOONBLAST, MOVE_ICE_BEAM, MOVE_FLAMETHROWER, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CLEFABLE_GENERIC_2] = {
        .species = SPECIES_CLEFABLE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_UNAWARE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOLLOW_ME, MOVE_LIFE_DEW, MOVE_MOONBLAST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_NINETALES_GENERIC_1] = {
        .species = SPECIES_NINETALES,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DROUGHT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_PROTECT, MOVE_WEATHER_BALL, MOVE_SOLAR_BEAM}
    },
    [CUSTOM_FRONTIER_MON_NINETALES_GENERIC_2] = {
        .species = SPECIES_NINETALES,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_DROUGHT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ENCORE, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_WEATHER_BALL}
    },
    [CUSTOM_FRONTIER_MON_NINETALES_ALOLA_GENERIC_1] = {
        .species = SPECIES_NINETALES_ALOLA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AURORA_VEIL, MOVE_BLIZZARD, MOVE_MOONBLAST, MOVE_FREEZE_DRY}
    },
    [CUSTOM_FRONTIER_MON_NINETALES_ALOLA_GENERIC_2] = {
        .species = SPECIES_NINETALES_ALOLA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AURORA_VEIL, MOVE_ENCORE, MOVE_PROTECT, MOVE_BLIZZARD}
    },
    [CUSTOM_FRONTIER_MON_WIGGLYTUFF_GENERIC_1] = {
        .species = SPECIES_WIGGLYTUFF,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOLLOW_ME, MOVE_ALLURING_VOICE, MOVE_DISABLE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_WIGGLYTUFF_GENERIC_2] = {
        .species = SPECIES_WIGGLYTUFF,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MINIMIZE, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE, MOVE_FLAMETHROWER}
    },
    [CUSTOM_FRONTIER_MON_VILEPLUME_GENERIC_1] = {
        .species = SPECIES_VILEPLUME,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_EFFECT_SPORE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_INGRAIN, MOVE_STRENGTH_SAP, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_VILEPLUME_GENERIC_2] = {
        .species = SPECIES_VILEPLUME,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_LINGERING_AROMA,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLEEP_POWDER, MOVE_PROTECT, MOVE_SLUDGE_BOMB, MOVE_GIGA_DRAIN}
    },
    [CUSTOM_FRONTIER_MON_PARASECT_GENERIC_1] = {
        .species = SPECIES_PARASECT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ANALYTIC,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_X_SCISSOR, MOVE_SEED_BOMB, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PARASECT_GENERIC_2] = {
        .species = SPECIES_PARASECT,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_DRY_SKIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_RAGE_POWDER, MOVE_SLEEP_POWDER, MOVE_X_SCISSOR, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VENOMOTH_GENERIC_1] = {
        .species = SPECIES_VENOMOTH,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_SHIELD_DUST,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_TAILWIND, MOVE_SLEEP_POWDER, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_VENOMOTH_GENERIC_2] = {
        .species = SPECIES_VENOMOTH,
        .heldItem = ITEM_BRIGHT_POWDER,
        .ability = ABILITY_MULTISCALE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_QUIVER_DANCE, MOVE_SLUDGE_BOMB, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_DUGTRIO_GENERIC_1] = {
        .species = SPECIES_DUGTRIO,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_ARENA_TRAP,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SUCKER_PUNCH, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_THROAT_CHOP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DUGTRIO_GENERIC_2] = {
        .species = SPECIES_DUGTRIO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ARENA_TRAP,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_SUCKER_PUNCH, MOVE_STEALTH_ROCK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DUGTRIO_ALOLA_GENERIC_1] = {
        .species = SPECIES_DUGTRIO_ALOLA,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_TANGLING_HAIR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SUCKER_PUNCH, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DUGTRIO_ALOLA_GENERIC_2] = {
        .species = SPECIES_DUGTRIO_ALOLA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TANGLING_HAIR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_STEALTH_ROCK, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PERSIAN_GENERIC_1] = {
        .species = SPECIES_PERSIAN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_NORMALIZE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_WORK_UP, MOVE_FAKE_OUT, MOVE_GUNK_SHOT, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PERSIAN_GENERIC_2] = {
        .species = SPECIES_PERSIAN,
        .heldItem = ITEM_SILK_SCARF,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_DOUBLE_EDGE, MOVE_TAUNT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PERSIAN_ALOLA_GENERIC_1] = {
        .species = SPECIES_PERSIAN_ALOLA,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_FUR_COAT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_DARK_PULSE, MOVE_THUNDERBOLT, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_PERSIAN_ALOLA_GENERIC_2] = {
        .species = SPECIES_PERSIAN_ALOLA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MERCILESS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_DARK_PULSE, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .nature = NATURE_TIMID
    },
    [CUSTOM_FRONTIER_MON_GOLDUCK_GENERIC_1] = {
        .species = SPECIES_GOLDUCK,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_GOLDUCK_GENERIC_2] = {
        .species = SPECIES_GOLDUCK,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_CLOUD_NINE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPNOSIS, MOVE_ENCORE, MOVE_SCALD, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ARCANINE_GENERIC_1] = {
        .species = SPECIES_ARCANINE,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_GENTLE,
        .moves = {MOVE_WILL_O_WISP, MOVE_FLARE_BLITZ, MOVE_PROTECT, MOVE_SNARL},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARCANINE_GENERIC_2] = {
        .species = SPECIES_ARCANINE,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FLARE_BLITZ, MOVE_PLAY_ROUGH, MOVE_EXTREME_SPEED, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARCANINE_HISUI_GENERIC_1] = {
        .species = SPECIES_ARCANINE_HISUI,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_WILL_O_WISP, MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_FLARE_BLITZ},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARCANINE_HISUI_GENERIC_2] = {
        .species = SPECIES_ARCANINE_HISUI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FLARE_BLITZ, MOVE_HEAD_SMASH, MOVE_WILD_CHARGE, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_POLIWRATH_GENERIC_1] = {
        .species = SPECIES_POLIWRATH,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(140, 252, 0, 116, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_LIQUIDATION, MOVE_CLOSE_COMBAT, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_POLIWRATH_GENERIC_2] = {
        .species = SPECIES_POLIWRATH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_WATER_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BULK_UP, MOVE_DRAIN_PUNCH, MOVE_LIQUIDATION, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ALAKAZAM_GENERIC_1] = {
        .species = SPECIES_ALAKAZAM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_PSYCHIC, MOVE_GRASS_KNOT, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_ALAKAZAM_GENERIC_2] = {
        .species = SPECIES_ALAKAZAM,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_MAGIC_GUARD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_PSYCHIC, MOVE_FOCUS_BLAST, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_MACHAMP_GENERIC_1] = {
        .species = SPECIES_MACHAMP,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_NO_GUARD,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_DYNAMIC_PUNCH, MOVE_STONE_EDGE, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MACHAMP_GENERIC_2] = {
        .species = SPECIES_MACHAMP,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VICTREEBEL_GENERIC_1] = {
        .species = SPECIES_VICTREEBEL,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_STRENGTH_SAP, MOVE_RAGE_POWDER, MOVE_TOXIC, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_VICTREEBEL_GENERIC_2] = {
        .species = SPECIES_VICTREEBEL,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SUNNY_DAY, MOVE_POWER_WHIP, MOVE_POISON_JAB, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TENTACRUEL_GENERIC_1] = {
        .species = SPECIES_TENTACRUEL,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_AQUA_RING, MOVE_TOXIC_SPIKES, MOVE_TOXIC, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TENTACRUEL_GENERIC_2] = {
        .species = SPECIES_TENTACRUEL,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(80, 0, 0, 176, 252, 0),
        .nature = NATURE_MODEST,
        .moves = {MOVE_RAPID_SPIN, MOVE_SLUDGE_BOMB, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOLEM_GENERIC_1] = {
        .species = SPECIES_GOLEM,
        .heldItem = ITEM_CUSTAP_BERRY,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_EXPLOSION, MOVE_STEALTH_ROCK, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOLEM_GENERIC_2] = {
        .species = SPECIES_GOLEM,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_POLISH, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOLEM_ALOLA_GENERIC_1] = {
        .species = SPECIES_GOLEM_ALOLA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_GALVANIZE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_EXPLOSION, MOVE_ROCK_SLIDE, MOVE_GYRO_BALL, MOVE_HIGH_HORSEPOWER}
    },
    [CUSTOM_FRONTIER_MON_GOLEM_ALOLA_GENERIC_2] = {
        .species = SPECIES_GOLEM_ALOLA,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_POLISH, MOVE_SUPERCELL_SLAM, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAPIDASH_GENERIC_1] = {
        .species = SPECIES_RAPIDASH,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FLARE_BLITZ, MOVE_HIGH_HORSEPOWER, MOVE_MEGAHORN, MOVE_WILD_CHARGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAPIDASH_GENERIC_2] = {
        .species = SPECIES_RAPIDASH,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_FLARE_BLITZ, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SLOWBRO_GENERIC_1] = {
        .species = SPECIES_SLOWBRO,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_SCALD, MOVE_PSYSHOCK, MOVE_SLACK_OFF}
    },
    [CUSTOM_FRONTIER_MON_SLOWBRO_GENERIC_2] = {
        .species = SPECIES_SLOWBRO,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_RELAXED,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_FOUL_PLAY, MOVE_SLACK_OFF, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_SLOWBRO_GALAR_GENERIC_1] = {
        .species = SPECIES_SLOWBRO_GALAR,
        .heldItem = ITEM_QUICK_CLAW,
        .ability = ABILITY_QUICK_DRAW,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_NASTY_PLOT, MOVE_SHELL_SIDE_ARM, MOVE_PSYSHOCK, MOVE_GRASS_KNOT},
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SLOWBRO_GALAR_GENERIC_2] = {
        .species = SPECIES_SLOWBRO_GALAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_PSYSHOCK, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_FARFETCHD_GENERIC_1] = {
        .species = SPECIES_FARFETCHD,
        .heldItem = ITEM_LEEK,
        .ability = ABILITY_STICK_HOARDER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BRAVE_BIRD, MOVE_TAILWIND, MOVE_PROTECT, MOVE_LEAF_BLADE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FARFETCHD_GENERIC_2] = {
        .species = SPECIES_FARFETCHD,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BRAVE_BIRD, MOVE_DOUBLE_EDGE, MOVE_LEAF_BLADE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DODRIO_GENERIC_1] = {
        .species = SPECIES_DODRIO,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_RECKLESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BRAVE_BIRD, MOVE_DRILL_RUN, MOVE_KNOCK_OFF, MOVE_DOUBLE_EDGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DODRIO_GENERIC_2] = {
        .species = SPECIES_DODRIO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_RECKLESS,
        .moves = {MOVE_SWORDS_DANCE, MOVE_BRAVE_BIRD, MOVE_DRILL_RUN, MOVE_PROTECT},
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .nature = NATURE_JOLLY
    },
    [CUSTOM_FRONTIER_MON_DEWGONG_GENERIC_1] = {
        .species = SPECIES_DEWGONG,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .moves = {MOVE_FLIP_TURN, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DEWGONG_GENERIC_2] = {
        .species = SPECIES_DEWGONG,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AQUA_RING, MOVE_ENCORE, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_MUK_GENERIC_1] = {
        .species = SPECIES_MUK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_POISON_TOUCH,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_KNOCK_OFF, MOVE_GUNK_SHOT, MOVE_SHADOW_SNEAK, MOVE_FIRE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MUK_GENERIC_2] = {
        .species = SPECIES_MUK,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_STICKY_HOLD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_ACID_ARMOR, MOVE_TOXIC, MOVE_PROTECT, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MUK_ALOLA_GENERIC_1] = {
        .species = SPECIES_MUK_ALOLA,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_POWER_OF_ALCHEMY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_MINIMIZE, MOVE_KNOCK_OFF, MOVE_POISON_JAB, MOVE_ACID_ARMOR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MUK_ALOLA_GENERIC_2] = {
        .species = SPECIES_MUK_ALOLA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_POISON_TOUCH,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_KNOCK_OFF, MOVE_GUNK_SHOT, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CLOYSTER_GENERIC_1] = {
        .species = SPECIES_CLOYSTER,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_SKILL_LINK,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHELL_SMASH, MOVE_ICICLE_SPEAR, MOVE_ROCK_BLAST, MOVE_DRILL_RUN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CLOYSTER_GENERIC_2] = {
        .species = SPECIES_CLOYSTER,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHELL_ARMOR,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SPIKES, MOVE_EXPLOSION, MOVE_ICE_SHARD, MOVE_LIQUIDATION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GENGAR_GENERIC_1] = {
        .species = SPECIES_GENGAR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_SLUDGE_BOMB, MOVE_SHADOW_BALL, MOVE_THUNDERBOLT}
    },
    [CUSTOM_FRONTIER_MON_GENGAR_GENERIC_2] = {
        .species = SPECIES_GENGAR,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_HYPNO_GENERIC_1] = {
        .species = SPECIES_HYPNO,
        .heldItem = ITEM_MENTAL_HERB,
        .ability = ABILITY_HYPNOTIST,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPNOSIS, MOVE_PSYSHOCK, MOVE_FOUL_PLAY, MOVE_REFLECT}
    },
    [CUSTOM_FRONTIER_MON_HYPNO_GENERIC_2] = {
        .species = SPECIES_HYPNO,
        .heldItem = ITEM_MENTAL_HERB,
        .ability = ABILITY_HYPNOTIST,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPNOSIS, MOVE_DREAM_EATER, MOVE_PROTECT, MOVE_FOUL_PLAY}
    },
    [CUSTOM_FRONTIER_MON_KINGLER_GENERIC_1] = {
        .species = SPECIES_KINGLER,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAZOR_SHELL, MOVE_NIGHT_SLASH, MOVE_X_SCISSOR, MOVE_AGILITY},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KINGLER_GENERIC_2] = {
        .species = SPECIES_KINGLER,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RAZOR_SHELL, MOVE_NIGHT_SLASH, MOVE_X_SCISSOR, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ELECTRODE_GENERIC_1] = {
        .species = SPECIES_ELECTRODE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ELECTROWEB, MOVE_VOLT_SWITCH, MOVE_FOUL_PLAY, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ELECTRODE_GENERIC_2] = {
        .species = SPECIES_ELECTRODE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 0, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LIGHT_SCREEN, MOVE_REFLECT, MOVE_FOUL_PLAY, MOVE_VOLT_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_ELECTRODE_HISUI_GENERIC_1] = {
        .species = SPECIES_ELECTRODE_HISUI,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LEAF_STORM, MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_CHLOROBLAST}
    },
    [CUSTOM_FRONTIER_MON_ELECTRODE_HISUI_GENERIC_2] = {
        .species = SPECIES_ELECTRODE_HISUI,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_VOLT_SWITCH, MOVE_ELECTROWEB, MOVE_LEAF_STORM}
    },
    [CUSTOM_FRONTIER_MON_EXEGGUTOR_GENERIC_1] = {
        .species = SPECIES_EXEGGUTOR,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HARVEST,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYSHOCK, MOVE_CALM_MIND, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_EXEGGUTOR_GENERIC_2] = {
        .species = SPECIES_EXEGGUTOR,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HARVEST,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_SLUDGE_BOMB, MOVE_PSYSHOCK, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_EXEGGUTOR_ALOLA_GENERIC_1] = {
        .species = SPECIES_EXEGGUTOR_ALOLA,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HARVEST,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_SWORDS_DANCE, MOVE_TRICK_ROOM, MOVE_DRAGON_HAMMER, MOVE_SEED_BOMB}
    },
    [CUSTOM_FRONTIER_MON_EXEGGUTOR_ALOLA_GENERIC_2] = {
        .species = SPECIES_EXEGGUTOR_ALOLA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_OWN_TEMPO,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_DRACO_METEOR, MOVE_ENERGY_BALL, MOVE_FLAMETHROWER, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_MAROWAK_GENERIC_1] = {
        .species = SPECIES_MAROWAK,
        .heldItem = ITEM_THICK_CLUB,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHADOW_CLAW, MOVE_SHADOW_SNEAK, MOVE_BONEMERANG, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAROWAK_GENERIC_2] = {
        .species = SPECIES_MAROWAK,
        .heldItem = ITEM_THICK_CLUB,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SHADOW_SNEAK, MOVE_SHADOW_CLAW, MOVE_BONEMERANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAROWAK_ALOLA_GENERIC_1] = {
        .species = SPECIES_MAROWAK_ALOLA,
        .heldItem = ITEM_THICK_CLUB,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHADOW_BONE, MOVE_FLARE_BLITZ, MOVE_PROTECT, MOVE_BONEMERANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAROWAK_ALOLA_GENERIC_2] = {
        .species = SPECIES_MAROWAK_ALOLA,
        .heldItem = ITEM_THICK_CLUB,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHADOW_BONE, MOVE_FLARE_BLITZ, MOVE_PROTECT, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HITMONLEE_GENERIC_1] = {
        .species = SPECIES_HITMONLEE,
        .heldItem = ITEM_NORMAL_GEM,
        .ability = ABILITY_UNBURDEN,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HITMONLEE_GENERIC_2] = {
        .species = SPECIES_HITMONLEE,
        .heldItem = ITEM_LIECHI_BERRY,
        .ability = ABILITY_UNBURDEN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ENDURE, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HITMONCHAN_GENERIC_1] = {
        .species = SPECIES_HITMONCHAN,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_ONE_TWO,
        .ev = TRAINER_PARTY_EVS(184, 252, 0, 68, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HITMONCHAN_GENERIC_2] = {
        .species = SPECIES_HITMONCHAN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ONE_TWO,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAPID_SPIN, MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WEEZING_GENERIC_1] = {
        .species = SPECIES_WEEZING,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_SLUDGE_BOMB, MOVE_PROTECT, MOVE_TOXIC_SPIKES}
    },
    [CUSTOM_FRONTIER_MON_WEEZING_GENERIC_2] = {
        .species = SPECIES_WEEZING,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_NEUTRALIZING_GAS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_PROTECT, MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER}
    },
    [CUSTOM_FRONTIER_MON_WEEZING_GALAR_GENERIC_1] = {
        .species = SPECIES_WEEZING_GALAR,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_NEUTRALIZING_GAS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_STRANGE_STEAM, MOVE_SLUDGE_BOMB, MOVE_PROTECT, MOVE_WILL_O_WISP}
    },
    [CUSTOM_FRONTIER_MON_WEEZING_GALAR_GENERIC_2] = {
        .species = SPECIES_WEEZING_GALAR,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_PROTECT, MOVE_SLUDGE_BOMB, MOVE_STRANGE_STEAM}
    },
    [CUSTOM_FRONTIER_MON_KANGASKHAN_GENERIC_1] = {
        .species = SPECIES_KANGASKHAN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_SEISMIC_TOSS, MOVE_POWER_UP_PUNCH, MOVE_DRAIN_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KANGASKHAN_GENERIC_2] = {
        .species = SPECIES_KANGASKHAN,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_SEISMIC_TOSS, MOVE_ROAR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEAKING_GENERIC_1] = {
        .species = SPECIES_SEAKING,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_WATERFALL, MOVE_MEGAHORN, MOVE_DRILL_RUN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEAKING_GENERIC_2] = {
        .species = SPECIES_SEAKING,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_RAIN_DANCE, MOVE_WATERFALL, MOVE_MEGAHORN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STARMIE_GENERIC_1] = {
        .species = SPECIES_STARMIE,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_STARMIE_GENERIC_2] = {
        .species = SPECIES_STARMIE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_RAPID_SPIN, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_PSYSHOCK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MR_MIME_GENERIC_1] = {
        .species = SPECIES_MR_MIME,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_MAGIC_SHOW,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_TRICK, MOVE_FAKE_OUT, MOVE_PSYSHOCK, MOVE_DAZZLING_GLEAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MR_MIME_GENERIC_2] = {
        .species = SPECIES_MR_MIME,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_PSYCHIC_SURGE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_EXPANDING_FORCE, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_JYNX_GENERIC_1] = {
        .species = SPECIES_JYNX,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ICE_BEAM, MOVE_PSYCHIC, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_JYNX_GENERIC_2] = {
        .species = SPECIES_JYNX,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(0, 4, 0, 252, 252, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_FAKE_OUT, MOVE_LOVELY_KISS, MOVE_ICE_BEAM, MOVE_PSYSHOCK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PINSIR_GENERIC_1] = {
        .species = SPECIES_PINSIR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_HYPER_CUTTER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_X_SCISSOR, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PINSIR_GENERIC_2] = {
        .species = SPECIES_PINSIR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_HYPER_CUTTER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_X_SCISSOR, MOVE_KNOCK_OFF, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_GENERIC_1] = {
        .species = SPECIES_TAUROS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_DOUBLE_EDGE, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_GENERIC_2] = {
        .species = SPECIES_TAUROS,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .moves = {MOVE_BODY_SLAM, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .nature = NATURE_JOLLY
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_COMBAT_GENERIC_1] = {
        .species = SPECIES_TAUROS_PALDEA_COMBAT,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_CUD_CHEW,
        .ev = TRAINER_PARTY_EVS(112, 252, 0, 144, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BULK_UP, MOVE_TRAILBLAZE, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_COMBAT_GENERIC_2] = {
        .species = SPECIES_TAUROS_PALDEA_COMBAT,
        .heldItem = ITEM_CLEAR_AMULET,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_PROTECT, MOVE_CLOSE_COMBAT, MOVE_WILD_CHARGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_AQUA_GENERIC_1] = {
        .species = SPECIES_TAUROS_PALDEA_AQUA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_WAVE_CRASH, MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_AQUA_GENERIC_2] = {
        .species = SPECIES_TAUROS_PALDEA_AQUA,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BULK_UP, MOVE_TRAILBLAZE, MOVE_CLOSE_COMBAT, MOVE_LIQUIDATION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_BLAZE_GENERIC_1] = {
        .species = SPECIES_TAUROS_PALDEA_BLAZE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FLARE_BLITZ, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TAUROS_PALDEA_BLAZE_GENERIC_2] = {
        .species = SPECIES_TAUROS_PALDEA_BLAZE,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BULK_UP, MOVE_TRAILBLAZE, MOVE_RAGING_BULL, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GYARADOS_GENERIC_1] = {
        .species = SPECIES_GYARADOS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_THUNDER_WAVE, MOVE_PROTECT, MOVE_WATERFALL, MOVE_HELPING_HAND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GYARADOS_GENERIC_2] = {
        .species = SPECIES_GYARADOS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MOXIE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DRAGON_DANCE, MOVE_WATERFALL, MOVE_GALE_TAIL, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LAPRAS_GENERIC_1] = {
        .species = SPECIES_LAPRAS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE, MOVE_FREEZE_DRY}
    },
    [CUSTOM_FRONTIER_MON_LAPRAS_GENERIC_2] = {
        .species = SPECIES_LAPRAS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AQUA_RING, MOVE_PROTECT, MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP}
    },
    [CUSTOM_FRONTIER_MON_VAPOREON_GENERIC_1] = {
        .species = SPECIES_VAPOREON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_WATER_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WISH, MOVE_PROTECT, MOVE_CALM_MIND, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_VAPOREON_GENERIC_2] = {
        .species = SPECIES_VAPOREON,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_WATER_ABSORB,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_JOLTEON_GENERIC_1] = {
        .species = SPECIES_JOLTEON,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_ALLURING_VOICE, MOVE_SHADOW_BALL, MOVE_VOLT_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_JOLTEON_GENERIC_2] = {
        .species = SPECIES_JOLTEON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUBSTITUTE, MOVE_CALM_MIND, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_FLAREON_GENERIC_1] = {
        .species = SPECIES_FLAREON,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(4, 252, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_FLARE_BLITZ, MOVE_GIGA_IMPACT, MOVE_SUPERPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FLAREON_GENERIC_2] = {
        .species = SPECIES_FLAREON,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_FLASH_FIRE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FLARE_BLITZ, MOVE_SUPERPOWER, MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_OMASTAR_GENERIC_1] = {
        .species = SPECIES_OMASTAR,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_WEAK_ARMOR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_ICE_BEAM, MOVE_EARTH_POWER, MOVE_HYDRO_PUMP}
    },
    [CUSTOM_FRONTIER_MON_OMASTAR_GENERIC_2] = {
        .species = SPECIES_OMASTAR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_KABUTOPS_GENERIC_1] = {
        .species = SPECIES_KABUTOPS,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_LIQUIDATION, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KABUTOPS_GENERIC_2] = {
        .species = SPECIES_KABUTOPS,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_STONE_AXE, MOVE_RAZOR_SHELL, MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AERODACTYL_GENERIC_1] = {
        .species = SPECIES_AERODACTYL,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_UNNERVE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_ROCK_SLIDE, MOVE_SKY_ATTACK, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AERODACTYL_GENERIC_2] = {
        .species = SPECIES_AERODACTYL,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_UNNERVE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_SKY_DROP, MOVE_PSYCHIC_FANGS, MOVE_STONE_EDGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SNORLAX_GENERIC_1] = {
        .species = SPECIES_SNORLAX,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BELLY_DRUM, MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SNORLAX_GENERIC_2] = {
        .species = SPECIES_SNORLAX,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_IMMUNITY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CURSE, MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARTICUNO_GENERIC_1] = {
        .species = SPECIES_ARTICUNO,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_FREEZE_DRY, MOVE_PROTECT, MOVE_HURRICANE}
    },
    [CUSTOM_FRONTIER_MON_ARTICUNO_GENERIC_2] = {
        .species = SPECIES_ARTICUNO,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BLIZZARD, MOVE_HURRICANE, MOVE_ICE_BEAM, MOVE_FREEZE_DRY}
    },
    [CUSTOM_FRONTIER_MON_ARTICUNO_GALAR_GENERIC_1] = {
        .species = SPECIES_ARTICUNO_GALAR,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_FREEZING_GLARE, MOVE_HURRICANE, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_ARTICUNO_GALAR_GENERIC_2] = {
        .species = SPECIES_ARTICUNO_GALAR,
        .heldItem = ITEM_ADRENALINE_ORB,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_FREEZING_GLARE, MOVE_HURRICANE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ZAPDOS_GENERIC_1] = {
        .species = SPECIES_ZAPDOS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_THUNDER_WAVE, MOVE_HELPING_HAND, MOVE_THUNDERBOLT}
    },
    [CUSTOM_FRONTIER_MON_ZAPDOS_GENERIC_2] = {
        .species = SPECIES_ZAPDOS,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_HURRICANE, MOVE_ANCIENT_POWER}
    },
    [CUSTOM_FRONTIER_MON_ZAPDOS_GALAR_GENERIC_1] = {
        .species = SPECIES_ZAPDOS_GALAR,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BRAVE_BIRD, MOVE_CLOSE_COMBAT, MOVE_U_TURN, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZAPDOS_GALAR_GENERIC_2] = {
        .species = SPECIES_ZAPDOS_GALAR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_BRAVE_BIRD, MOVE_CLOSE_COMBAT, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MOLTRES_GENERIC_1] = {
        .species = SPECIES_MOLTRES,
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_DEFOG, MOVE_TAILWIND, MOVE_WILL_O_WISP, MOVE_BRAVE_BIRD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MOLTRES_GENERIC_2] = {
        .species = SPECIES_MOLTRES,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_WILL_O_WISP, MOVE_FLARE_BLITZ, MOVE_BRAVE_BIRD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MOLTRES_GALAR_GENERIC_1] = {
        .species = SPECIES_MOLTRES_GALAR,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_NASTY_PLOT, MOVE_FIERY_WRATH, MOVE_HURRICANE}
    },
    [CUSTOM_FRONTIER_MON_MOLTRES_GALAR_GENERIC_2] = {
        .species = SPECIES_MOLTRES_GALAR,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AGILITY, MOVE_PROTECT, MOVE_FIERY_WRATH, MOVE_HURRICANE}
    },
    [CUSTOM_FRONTIER_MON_DRAGONITE_GENERIC_1] = {
        .species = SPECIES_DRAGONITE,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_SCALE_SHOT, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DRAGONITE_GENERIC_2] = {
        .species = SPECIES_DRAGONITE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_DUAL_WINGBEAT, MOVE_ICE_SPINNER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MEW_GENERIC_1] = {
        .species = SPECIES_MEW,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SYNCHRONIZE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_COACHING, MOVE_FAKE_OUT, MOVE_SEISMIC_TOSS, MOVE_TRICK_ROOM}
    },
    [CUSTOM_FRONTIER_MON_MEW_GENERIC_2] = {
        .species = SPECIES_MEW,
        .heldItem = ITEM_BRIGHT_POWDER,
        .ability = ABILITY_SYNCHRONIZE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_SWORDS_DANCE, MOVE_PSYCHIC_FANGS, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DITTO_GENERIC_1] = {
        .species = SPECIES_DITTO,
        .heldItem = ITEM_METAL_POWDER,
        .ability = ABILITY_IMPOSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 30, 31, 31, 31, 31),
        .moves = {MOVE_TRANSFORM}
    },
    [CUSTOM_FRONTIER_MON_DITTO_GENERIC_2] = {
        .species = SPECIES_DITTO,
        .heldItem = ITEM_QUICK_POWDER,
        .ability = ABILITY_IMPOSTER,
        .ev = TRAINER_PARTY_EVS(248, 8, 252, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .iv = TRAINER_PARTY_IVS(31, 30, 31, 31, 31, 31),
        .moves = {MOVE_TRANSFORM}
    },
    [CUSTOM_FRONTIER_MON_RAPIDASH_GALAR_GENERIC_1] = {
        .species = SPECIES_RAPIDASH_GALAR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_MISTY_SURGE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_PLAY_ROUGH, MOVE_MEGAHORN, MOVE_THROAT_CHOP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAPIDASH_GALAR_GENERIC_2] = {
        .species = SPECIES_RAPIDASH_GALAR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_RUN_AWAY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_PLAY_ROUGH, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RHYDON_GENERIC_1] = {
        .species = SPECIES_RHYDON,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_FIRE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RHYDON_GENERIC_2] = {
        .species = SPECIES_RHYDON,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_POLISH, MOVE_PROTECT, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCYTHER_GENERIC_1] = {
        .species = SPECIES_SCYTHER,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_SWORDS_DANCE, MOVE_DUAL_WINGBEAT, MOVE_BUG_BITE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCYTHER_GENERIC_2] = {
        .species = SPECIES_SCYTHER,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_BUG_BITE, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MEGANIUM_GENERIC_1] = {
        .species = SPECIES_MEGANIUM,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_FRIEND_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_INGRAIN, MOVE_LEECH_SEED, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_MEGANIUM_GENERIC_2] = {
        .species = SPECIES_MEGANIUM,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_FRIEND_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_DRAGON_TAIL, MOVE_SEED_BOMB, MOVE_KNOCK_OFF, MOVE_IRON_TAIL},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TYPHLOSION_GENERIC_1] = {
        .species = SPECIES_TYPHLOSION,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ERUPTION, MOVE_EXTRASENSORY, MOVE_FOCUS_BLAST, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_TYPHLOSION_GENERIC_2] = {
        .species = SPECIES_TYPHLOSION,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ERUPTION, MOVE_SHADOW_BALL, MOVE_FOCUS_BLAST, MOVE_EXTRASENSORY}
    },
    [CUSTOM_FRONTIER_MON_TYPHLOSION_HISUI_GENERIC_1] = {
        .species = SPECIES_TYPHLOSION_HISUI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ERUPTION, MOVE_SHADOW_BALL, MOVE_EXTRASENSORY, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_TYPHLOSION_HISUI_GENERIC_2] = {
        .species = SPECIES_TYPHLOSION_HISUI,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_BERSERK,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOCUS_BLAST, MOVE_ERUPTION, MOVE_SHADOW_BALL, MOVE_EXTRASENSORY}
    },
    [CUSTOM_FRONTIER_MON_FERALIGATR_GENERIC_1] = {
        .species = SPECIES_FERALIGATR,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_LIQUIDATION, MOVE_ICE_PUNCH, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FERALIGATR_GENERIC_2] = {
        .species = SPECIES_FERALIGATR,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DRAGON_DANCE, MOVE_LIQUIDATION, MOVE_ICE_FANG, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FURRET_GENERIC_1] = {
        .species = SPECIES_FURRET,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_FUR_COAT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRICK, MOVE_DOUBLE_EDGE, MOVE_U_TURN, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FURRET_GENERIC_2] = {
        .species = SPECIES_FURRET,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FRISK,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TIDY_UP, MOVE_DOUBLE_EDGE, MOVE_PLAY_ROUGH, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_NOCTOWL_GENERIC_1] = {
        .species = SPECIES_NOCTOWL,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_MINDS_EYE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AGILITY, MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_MOONBLAST}
    },
    [CUSTOM_FRONTIER_MON_NOCTOWL_GENERIC_2] = {
        .species = SPECIES_NOCTOWL,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MINDS_EYE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HURRICANE, MOVE_HYPER_VOICE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_LEDIAN_GENERIC_1] = {
        .species = SPECIES_LEDIAN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHIELD_DUST,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HEROIC_RISE, MOVE_BATON_PASS, MOVE_TAILWIND, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_LEDIAN_GENERIC_2] = {
        .species = SPECIES_LEDIAN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_IRON_FIST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_HEROIC_RISE, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARIADOS_GENERIC_1] = {
        .species = SPECIES_ARIADOS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MERCILESS,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TOXIC_THREAD, MOVE_KNOCK_OFF, MOVE_POISON_JAB, MOVE_LEECH_LIFE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARIADOS_GENERIC_2] = {
        .species = SPECIES_ARIADOS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MERCILESS,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_STICKY_WEB, MOVE_TOXIC_THREAD, MOVE_KNOCK_OFF, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CROBAT_GENERIC_1] = {
        .species = SPECIES_CROBAT,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_U_TURN, MOVE_BRAVE_BIRD, MOVE_GUNK_SHOT, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CROBAT_GENERIC_2] = {
        .species = SPECIES_CROBAT,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROOST, MOVE_TAILWIND, MOVE_TOXIC, MOVE_SKY_ATTACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LANTURN_GENERIC_1] = {
        .species = SPECIES_LANTURN,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_THUNDER_WAVE, MOVE_SCALD, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_LANTURN_GENERIC_2] = {
        .species = SPECIES_LANTURN,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 4, 0),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_SCALD, MOVE_THUNDERBOLT, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_XATU_GENERIC_1] = {
        .species = SPECIES_XATU,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_TEN_STEPS_AHEAD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_COSMIC_POWER, MOVE_STORED_POWER, MOVE_LIGHT_SCREEN, MOVE_REFLECT}
    },
    [CUSTOM_FRONTIER_MON_XATU_GENERIC_2] = {
        .species = SPECIES_XATU,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_FOUL_PLAY, MOVE_ROOST, MOVE_ALLY_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_AMPHAROS_GENERIC_1] = {
        .species = SPECIES_AMPHAROS,
        .heldItem = ITEM_POWER_HERB,
        .ability = ABILITY_GALVANIZE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ELECTRO_SHOT, MOVE_METEOR_BEAM, MOVE_HYPER_VOICE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_AMPHAROS_GENERIC_2] = {
        .species = SPECIES_AMPHAROS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_GALVANIZE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPER_BEAM, MOVE_HYPER_VOICE, MOVE_FOCUS_BLAST, MOVE_POWER_GEM}
    },
    [CUSTOM_FRONTIER_MON_BELLOSSOM_GENERIC_1] = {
        .species = SPECIES_BELLOSSOM,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_FLOWER_VEIL,
        .ev = TRAINER_PARTY_EVS(0, 0, 4, 252, 252, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_QUIVER_DANCE, MOVE_GIGA_DRAIN, MOVE_MOONBLAST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_BELLOSSOM_GENERIC_2] = {
        .species = SPECIES_BELLOSSOM,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_HEALER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLEEP_POWDER, MOVE_HELPING_HAND, MOVE_STRENGTH_SAP, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_AZUMARILL_GENERIC_1] = {
        .species = SPECIES_AZUMARILL,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_HUGE_POWER,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PLAY_ROUGH, MOVE_LIQUIDATION, MOVE_AQUA_JET, MOVE_ICE_SPINNER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AZUMARILL_GENERIC_2] = {
        .species = SPECIES_AZUMARILL,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HUGE_POWER,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PLAY_ROUGH, MOVE_AQUA_JET, MOVE_LIQUIDATION, MOVE_BELLY_DRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SUDOWOODO_GENERIC_1] = {
        .species = SPECIES_SUDOWOODO,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_RECKLESS,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HEAD_SMASH, MOVE_SUCKER_PUNCH, MOVE_HIGH_HORSEPOWER, MOVE_BRICK_BREAK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SUDOWOODO_GENERIC_2] = {
        .species = SPECIES_SUDOWOODO,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HEAD_SMASH, MOVE_SUCKER_PUNCH, MOVE_HIGH_HORSEPOWER, MOVE_WOOD_HAMMER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_POLITOED_GENERIC_1] = {
        .species = SPECIES_POLITOED,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_DRIZZLE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WEATHER_BALL, MOVE_ICE_BEAM, MOVE_EARTH_POWER, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_POLITOED_GENERIC_2] = {
        .species = SPECIES_POLITOED,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_DRIZZLE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ENCORE, MOVE_WEATHER_BALL, MOVE_HELPING_HAND, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_JUMPLUFF_GENERIC_1] = {
        .species = SPECIES_JUMPLUFF,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_TAILWIND, MOVE_ENCORE, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_JUMPLUFF_GENERIC_2] = {
        .species = SPECIES_JUMPLUFF,
        .heldItem = ITEM_FLYING_GEM,
        .ability = ABILITY_WIND_RIDER,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_SWORDS_DANCE, MOVE_ACROBATICS, MOVE_SEED_BOMB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SUNFLORA_GENERIC_1] = {
        .species = SPECIES_SUNFLORA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_WEATHER_BALL, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_SUNFLORA_GENERIC_2] = {
        .species = SPECIES_SUNFLORA,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SOLAR_POWER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_LEAF_STORM, MOVE_SLUDGE_BOMB, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_QUAGSIRE_GENERIC_1] = {
        .species = SPECIES_QUAGSIRE,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_WATER_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_RECOVER, MOVE_PROTECT, MOVE_BODY_PRESS}
    },
    [CUSTOM_FRONTIER_MON_QUAGSIRE_GENERIC_2] = {
        .species = SPECIES_QUAGSIRE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_DAMP,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_AVALANCHE, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ESPEON_GENERIC_1] = {
        .species = SPECIES_ESPEON,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PSYCHIC, MOVE_ALLURING_VOICE, MOVE_GRASS_KNOT, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_ESPEON_GENERIC_2] = {
        .species = SPECIES_ESPEON,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_PSYSHOCK, MOVE_ALLURING_VOICE, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_UMBREON_GENERIC_1] = {
        .species = SPECIES_UMBREON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_FOUL_PLAY, MOVE_WISH, MOVE_TOXIC}
    },
    [CUSTOM_FRONTIER_MON_UMBREON_GENERIC_2] = {
        .species = SPECIES_UMBREON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SYNCHRONIZE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_REST, MOVE_SLEEP_TALK, MOVE_FOUL_PLAY}
    },
    [CUSTOM_FRONTIER_MON_SLOWKING_GENERIC_1] = {
        .species = SPECIES_SLOWKING,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_NASTY_PLOT, MOVE_HYDRO_PUMP, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_SLOWKING_GENERIC_2] = {
        .species = SPECIES_SLOWKING,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PSYSHOCK, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_SLOWKING_GALAR_GENERIC_1] = {
        .species = SPECIES_SLOWKING_GALAR,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_NASTY_PLOT, MOVE_SLUDGE_BOMB, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_SLOWKING_GALAR_GENERIC_2] = {
        .species = SPECIES_SLOWKING_GALAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLUDGE_BOMB, MOVE_PSYSHOCK, MOVE_FLAMETHROWER, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_UNOWN_GENERIC_1] = {
        .species = SPECIES_UNOWN,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_PHONETIC_MAGIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 4, 252, 252, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 30),
        .moves = {MOVE_HIDDEN_POWER}
    },
    [CUSTOM_FRONTIER_MON_UNOWN_GENERIC_2] = {
        .species = SPECIES_UNOWN,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_PHONETIC_MAGIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 4, 252, 252, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(30, 31, 31, 31, 31, 30),
        .moves = {MOVE_HIDDEN_POWER}
    },
    [CUSTOM_FRONTIER_MON_WOBBUFFET_GENERIC_1] = {
        .species = SPECIES_WOBBUFFET,
        .heldItem = ITEM_COLBUR_BERRY,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_DESTINY_BOND, MOVE_ENCORE}
    },
    [CUSTOM_FRONTIER_MON_WOBBUFFET_GENERIC_2] = {
        .species = SPECIES_WOBBUFFET,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_CHARM, MOVE_TICKLE}
    },
    [CUSTOM_FRONTIER_MON_FORRETRESS_GENERIC_1] = {
        .species = SPECIES_FORRETRESS,
        .heldItem = ITEM_MENTAL_HERB,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SHELL_SMASH, MOVE_IRON_HEAD, MOVE_LUNGE, MOVE_EXPLOSION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FORRETRESS_GENERIC_2] = {
        .species = SPECIES_FORRETRESS,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_ROCK_POLISH, MOVE_PAYBACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STEELIX_GENERIC_1] = {
        .species = SPECIES_STEELIX,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_BODY_PRESS, MOVE_IRON_HEAD, MOVE_HIGH_HORSEPOWER, MOVE_DRAGON_TAIL}
    },
    [CUSTOM_FRONTIER_MON_STEELIX_GENERIC_2] = {
        .species = SPECIES_STEELIX,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_GYRO_BALL, MOVE_BODY_PRESS, MOVE_PSYCHIC_FANGS}
    },
    [CUSTOM_FRONTIER_MON_GRANBULL_GENERIC_1] = {
        .species = SPECIES_GRANBULL,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STRONG_JAW,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ICE_FANG, MOVE_PLAY_ROUGH, MOVE_THUNDER_FANG, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GRANBULL_GENERIC_2] = {
        .species = SPECIES_GRANBULL,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 252, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_THUNDER_WAVE, MOVE_PLAY_ROUGH, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_QWILFISH_GENERIC_1] = {
        .species = SPECIES_QWILFISH,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 252, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_TOXIC, MOVE_PROTECT, MOVE_SUBSTITUTE, MOVE_LIQUIDATION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_QWILFISH_GENERIC_2] = {
        .species = SPECIES_QWILFISH,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SCALE_SHOT, MOVE_SWORDS_DANCE, MOVE_LIQUIDATION, MOVE_GUNK_SHOT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_QWILFISH_HISUI_GENERIC_1] = {
        .species = SPECIES_QWILFISH_HISUI,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_GUNK_SHOT, MOVE_CRUNCH, MOVE_PROTECT, MOVE_TOXIC_SPIKES},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_QWILFISH_HISUI_GENERIC_2] = {
        .species = SPECIES_QWILFISH_HISUI,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_TOXIC, MOVE_PAIN_SPLIT, MOVE_PROTECT, MOVE_BARB_BARRAGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCIZOR_GENERIC_1] = {
        .species = SPECIES_SCIZOR,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_BULLET_PUNCH, MOVE_BUG_BITE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCIZOR_GENERIC_2] = {
        .species = SPECIES_SCIZOR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DUAL_WINGBEAT, MOVE_BUG_BITE, MOVE_BULLET_PUNCH, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHUCKLE_GENERIC_1] = {
        .species = SPECIES_SHUCKLE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_CONTRARY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_BODY_PRESS, MOVE_INFESTATION, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SHUCKLE_GENERIC_2] = {
        .species = SPECIES_SHUCKLE,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_STICKY_WEB, MOVE_TOXIC}
    },
    [CUSTOM_FRONTIER_MON_HERACROSS_GENERIC_1] = {
        .species = SPECIES_HERACROSS,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_TRAILBLAZE, MOVE_FACADE, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HERACROSS_GENERIC_2] = {
        .species = SPECIES_HERACROSS,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_MOXIE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_MEGAHORN, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAGCARGO_GENERIC_1] = {
        .species = SPECIES_MAGCARGO,
        .heldItem = ITEM_PASSHO_BERRY,
        .ability = ABILITY_EARTH_EATER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_MOLTEN_BURST, MOVE_POWER_GEM, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_MAGCARGO_GENERIC_2] = {
        .species = SPECIES_MAGCARGO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_MOLTEN_BURST, MOVE_EARTH_POWER, MOVE_HEAT_WAVE}
    },
    [CUSTOM_FRONTIER_MON_CORSOLA_GENERIC_1] = {
        .species = SPECIES_CORSOLA,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_ALGAE_ADDICT,
        .ev = TRAINER_PARTY_EVS(252, 0, 124, 0, 0, 132),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WHIRLPOOL, MOVE_RECOVER, MOVE_SUBSTITUTE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CORSOLA_GENERIC_2] = {
        .species = SPECIES_CORSOLA,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_ALGAE_ADDICT,
        .ev = TRAINER_PARTY_EVS(252, 0, 116, 0, 0, 140),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AQUA_RING, MOVE_WHIRLPOOL, MOVE_PROTECT, MOVE_LIFE_DEW}
    },
    [CUSTOM_FRONTIER_MON_OCTILLERY_GENERIC_1] = {
        .species = SPECIES_OCTILLERY,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_OCTAZOOKA, MOVE_ICE_BEAM, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_OCTILLERY_GENERIC_2] = {
        .species = SPECIES_OCTILLERY,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_OCTAZOOKA, MOVE_REST, MOVE_SLEEP_TALK, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_DELIBIRD_GENERIC_1] = {
        .species = SPECIES_DELIBIRD,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FRIEND_GUARD,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_FREEZE_DRY, MOVE_FRIGID_TOUCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DELIBIRD_GENERIC_2] = {
        .species = SPECIES_DELIBIRD,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_FRIEND_GUARD,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 252, 0, 0),
        .nature = NATURE_TIMID,
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_FRIGID_TOUCH, MOVE_DESTINY_BOND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MANTINE_GENERIC_1] = {
        .species = SPECIES_MANTINE,
        .heldItem = ITEM_WACAN_BERRY,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_SCALD, MOVE_AIR_SLASH, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_MANTINE_GENERIC_2] = {
        .species = SPECIES_MANTINE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AQUA_RING, MOVE_PROTECT, MOVE_SCALD, MOVE_TAILWIND}
    },
    [CUSTOM_FRONTIER_MON_SKARMORY_GENERIC_1] = {
        .species = SPECIES_SKARMORY,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 24, 0, 232),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_ROOST, MOVE_TAILWIND}
    },
    [CUSTOM_FRONTIER_MON_SKARMORY_GENERIC_2] = {
        .species = SPECIES_SKARMORY,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_WEAK_ARMOR,
        .ev = TRAINER_PARTY_EVS(232, 252, 0, 24, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_TAILWIND, MOVE_SKY_ATTACK, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HOUNDOOM_GENERIC_1] = {
        .species = SPECIES_HOUNDOOM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_SLUDGE_BOMB, MOVE_DARK_PULSE, MOVE_HEAT_WAVE}
    },
    [CUSTOM_FRONTIER_MON_HOUNDOOM_GENERIC_2] = {
        .species = SPECIES_HOUNDOOM,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_EARLY_BIRD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 252, 0, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_PROTECT, MOVE_HEAT_WAVE, MOVE_DESTINY_BOND}
    },
    [CUSTOM_FRONTIER_MON_KINGDRA_GENERIC_1] = {
        .species = SPECIES_KINGDRA,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAIN_DANCE, MOVE_WEATHER_BALL, MOVE_DRACO_METEOR, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_KINGDRA_GENERIC_2] = {
        .species = SPECIES_KINGDRA,
        .heldItem = ITEM_SCOPE_LENS,
        .ability = ABILITY_SNIPER,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAIN_DANCE, MOVE_PROTECT, MOVE_WEATHER_BALL, MOVE_DRAGON_PULSE}
    },
    [CUSTOM_FRONTIER_MON_DONPHAN_GENERIC_1] = {
        .species = SPECIES_DONPHAN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAPID_SPIN, MOVE_ICE_SPINNER, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DONPHAN_GENERIC_2] = {
        .species = SPECIES_DONPHAN,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(248, 8, 252, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_STEALTH_ROCK, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SMEARGLE_GENERIC_1] = {
        .species = SPECIES_SMEARGLE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_BURNING_BULWARK, MOVE_SPORE, MOVE_SKILL_SWAP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SMEARGLE_GENERIC_2] = {
        .species = SPECIES_SMEARGLE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 0, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_BURNING_BULWARK, MOVE_SKILL_SWAP, MOVE_STICKY_WEB}
    },
    [CUSTOM_FRONTIER_MON_HITMONTOP_GENERIC_1] = {
        .species = SPECIES_HITMONTOP,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_WIDE_GUARD, MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_COACHING},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HITMONTOP_GENERIC_2] = {
        .species = SPECIES_HITMONTOP,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_TRIPLE_AXEL, MOVE_AERIAL_ACE, MOVE_FEINT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MILTANK_GENERIC_1] = {
        .species = SPECIES_MILTANK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BODY_SLAM, MOVE_TRAILBLAZE, MOVE_HIGH_HORSEPOWER, MOVE_PLAY_ROUGH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MILTANK_GENERIC_2] = {
        .species = SPECIES_MILTANK,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_WORK_UP, MOVE_MILK_DRINK, MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BLISSEY_GENERIC_1] = {
        .species = SPECIES_BLISSEY,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_NATURAL_CURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SEISMIC_TOSS, MOVE_THUNDER_WAVE, MOVE_SOFT_BOILED, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_BLISSEY_GENERIC_2] = {
        .species = SPECIES_BLISSEY,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_NATURAL_CURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDER_WAVE, MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_SEISMIC_TOSS}
    },
    [CUSTOM_FRONTIER_MON_RAIKOU_GENERIC_1] = {
        .species = SPECIES_RAIKOU,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAIN_DANCE, MOVE_ELECTRO_SHOT, MOVE_WEATHER_BALL, MOVE_EXTRASENSORY}
    },
    [CUSTOM_FRONTIER_MON_RAIKOU_GENERIC_2] = {
        .species = SPECIES_RAIKOU,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_AURA_SPHERE, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_ENTEI_GENERIC_1] = {
        .species = SPECIES_ENTEI,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_INNER_FOCUS,
        .moves = {MOVE_SACRED_FIRE, MOVE_EXTREME_SPEED, MOVE_IRON_HEAD, MOVE_STOMPING_TANTRUM},
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .nature = NATURE_ADAMANT
    },
    [CUSTOM_FRONTIER_MON_ENTEI_GENERIC_2] = {
        .species = SPECIES_ENTEI,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .moves = {MOVE_WILL_O_WISP, MOVE_SACRED_FIRE, MOVE_ROAR, MOVE_SNARL},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SUICUNE_GENERIC_1] = {
        .species = SPECIES_SUICUNE,
        .heldItem = ITEM_MENTAL_HERB,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_CALM_MIND}
    },
    [CUSTOM_FRONTIER_MON_SUICUNE_GENERIC_2] = {
        .species = SPECIES_SUICUNE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_REST, MOVE_SLEEP_TALK, MOVE_CALM_MIND, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_TYRANITAR_GENERIC_1] = {
        .species = SPECIES_TYRANITAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SAND_STREAM,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TYRANITAR_GENERIC_2] = {
        .species = SPECIES_TYRANITAR,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_SAND_STREAM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DRAGON_DANCE, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CELEBI_GENERIC_1] = {
        .species = SPECIES_CELEBI,
        .heldItem = ITEM_ROOM_SERVICE,
        .ability = ABILITY_ILEX_WHIMSY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_CALM_MIND, MOVE_ENERGY_BALL, MOVE_MOONBLAST}
    },
    [CUSTOM_FRONTIER_MON_CELEBI_GENERIC_2] = {
        .species = SPECIES_CELEBI,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_ILEX_WHIMSY,
        .ev = TRAINER_PARTY_EVS(0, 0, 4, 252, 252, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MOONBLAST, MOVE_TRICK, MOVE_LEAF_STORM, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_SCEPTILE_GENERIC_1] = {
        .species = SPECIES_SCEPTILE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_UNBURDEN,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_SHED_TAIL, MOVE_PROTECT, MOVE_FAKE_OUT, MOVE_GRASS_KNOT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCEPTILE_GENERIC_2] = {
        .species = SPECIES_SCEPTILE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SWIFT_SLICES,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LEAF_BLADE, MOVE_PSYCHO_CUT, MOVE_NIGHT_SLASH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BLAZIKEN_GENERIC_1] = {
        .species = SPECIES_BLAZIKEN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_PROTECT, MOVE_FLARE_BLITZ, MOVE_HIGH_JUMP_KICK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BLAZIKEN_GENERIC_2] = {
        .species = SPECIES_BLAZIKEN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_BATON_PASS, MOVE_SWORDS_DANCE, MOVE_VACUUM_WAVE}
    },
    [CUSTOM_FRONTIER_MON_SWAMPERT_GENERIC_1] = {
        .species = SPECIES_SWAMPERT,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_ROCK_SLIDE, MOVE_AVALANCHE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SWAMPERT_GENERIC_2] = {
        .species = SPECIES_SWAMPERT,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_WIDE_GUARD, MOVE_KNOCK_OFF, MOVE_ICE_PUNCH, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MIGHTYENA_GENERIC_1] = {
        .species = SPECIES_MIGHTYENA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PLAY_ROUGH, MOVE_CRUNCH, MOVE_SUCKER_PUNCH, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MIGHTYENA_GENERIC_2] = {
        .species = SPECIES_MIGHTYENA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STRONG_JAW,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PSYCHIC_FANGS, MOVE_CRUNCH, MOVE_ICE_FANG, MOVE_THUNDER_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LINOONE_GENERIC_1] = {
        .species = SPECIES_LINOONE,
        .heldItem = ITEM_IAPAPA_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(4, 252, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BELLY_DRUM, MOVE_EXTREME_SPEED, MOVE_SHADOW_CLAW, MOVE_STOMPING_TANTRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LINOONE_GENERIC_2] = {
        .species = SPECIES_LINOONE,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_FUR_COAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_CHARM, MOVE_FOUL_PLAY},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BEAUTIFLY_GENERIC_1] = {
        .species = SPECIES_BEAUTIFLY,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MULTISCALE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_BUG_BUZZ, MOVE_HURRICANE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_BEAUTIFLY_GENERIC_2] = {
        .species = SPECIES_BEAUTIFLY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SIMPLE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BUG_BUZZ, MOVE_HURRICANE, MOVE_PSYCHIC, MOVE_QUIVER_DANCE}
    },
    [CUSTOM_FRONTIER_MON_DUSTOX_GENERIC_1] = {
        .species = SPECIES_DUSTOX,
        .heldItem = ITEM_ENIGMA_BERRY,
        .ability = ABILITY_SHIELD_DUST,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_STRUGGLE_BUG, MOVE_TOXIC, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_DUSTOX_GENERIC_2] = {
        .species = SPECIES_DUSTOX,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_SHIELD_DUST,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_PROTECT, MOVE_VENOSHOCK, MOVE_ROOST}
    },
    [CUSTOM_FRONTIER_MON_LUDICOLO_GENERIC_1] = {
        .species = SPECIES_LUDICOLO,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_RAIN_DISH,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_SCALD, MOVE_ICY_WIND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUDICOLO_GENERIC_2] = {
        .species = SPECIES_LUDICOLO,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAIN_DANCE, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_SHIFTRY_GENERIC_1] = {
        .species = SPECIES_SHIFTRY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_WIND_RIDER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_TAILWIND, MOVE_KNOCK_OFF, MOVE_LEAF_BLADE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHIFTRY_GENERIC_2] = {
        .species = SPECIES_SHIFTRY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_WIND_RIDER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_KNOCK_OFF, MOVE_LEAF_BLADE, MOVE_EXPLOSION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SWELLOW_GENERIC_1] = {
        .species = SPECIES_SWELLOW,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_BRAVE_BIRD, MOVE_FACADE, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SWELLOW_GENERIC_2] = {
        .species = SPECIES_SWELLOW,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BOOMBURST, MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_HYPER_BEAM}
    },
    [CUSTOM_FRONTIER_MON_PELIPPER_GENERIC_1] = {
        .species = SPECIES_PELIPPER,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_DRIZZLE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .moves = {MOVE_WEATHER_BALL, MOVE_HURRICANE, MOVE_U_TURN, MOVE_ICE_BEAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PELIPPER_GENERIC_2] = {
        .species = SPECIES_PELIPPER,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DRIZZLE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_WIDE_GUARD, MOVE_HURRICANE, MOVE_WEATHER_BALL}
    },
    [CUSTOM_FRONTIER_MON_GARDEVOIR_GENERIC_1] = {
        .species = SPECIES_GARDEVOIR,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_TRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_PSYSHOCK, MOVE_MOONBLAST, MOVE_MYSTICAL_FIRE}
    },
    [CUSTOM_FRONTIER_MON_GARDEVOIR_GENERIC_2] = {
        .species = SPECIES_GARDEVOIR,
        .heldItem = ITEM_IRON_BALL,
        .ability = ABILITY_TRACE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_MOONBLAST, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_MASQUERAIN_GENERIC_1] = {
        .species = SPECIES_MASQUERAIN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_STICKY_WEB, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_MASQUERAIN_GENERIC_2] = {
        .species = SPECIES_MASQUERAIN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HURRICANE, MOVE_BUG_BUZZ, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_BRELOOM_GENERIC_1] = {
        .species = SPECIES_BRELOOM,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_POISON_HEAL,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_FLING, MOVE_DRAIN_PUNCH, MOVE_MACH_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BRELOOM_GENERIC_2] = {
        .species = SPECIES_BRELOOM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_MACH_PUNCH, MOVE_ROCK_TOMB, MOVE_BULLET_SEED, MOVE_SPORE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SLAKING_GENERIC_1] = {
        .species = SPECIES_SLAKING,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_TRUANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_KNOCK_OFF, MOVE_GUNK_SHOT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SLAKING_GENERIC_2] = {
        .species = SPECIES_SLAKING,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_TRUANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_KNOCK_OFF, MOVE_GUNK_SHOT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_NINJASK_GENERIC_1] = {
        .species = SPECIES_NINJASK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_X_SCISSOR, MOVE_ACROBATICS, MOVE_NIGHT_SLASH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_NINJASK_GENERIC_2] = {
        .species = SPECIES_NINJASK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ACROBATICS, MOVE_LEECH_LIFE, MOVE_SWORDS_DANCE, MOVE_NIGHT_SLASH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHEDINJA_GENERIC_1] = {
        .species = SPECIES_SHEDINJA,
        .heldItem = ITEM_LUM_BERRY,
        .ability = ABILITY_WONDER_GUARD,
        .ev = TRAINER_PARTY_EVS(0, 252, 4, 252, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SHADOW_SNEAK, MOVE_X_SCISSOR, MOVE_POLTERGEIST},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHEDINJA_GENERIC_2] = {
        .species = SPECIES_SHEDINJA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_WONDER_GUARD,
        .ev = TRAINER_PARTY_EVS(0, 252, 4, 252, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_PROTECT, MOVE_POLTERGEIST, MOVE_X_SCISSOR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EXPLOUD_GENERIC_1] = {
        .species = SPECIES_EXPLOUD,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_PUNK_ROCK,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BOOMBURST, MOVE_ICE_BEAM, MOVE_FIRE_BLAST, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_EXPLOUD_GENERIC_2] = {
        .species = SPECIES_EXPLOUD,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOCUS_BLAST, MOVE_BOOMBURST, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP}
    },
    [CUSTOM_FRONTIER_MON_HARIYAMA_GENERIC_1] = {
        .species = SPECIES_HARIYAMA,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_CLOSE_COMBAT, MOVE_PROTECT, MOVE_BULLET_PUNCH, MOVE_KNOCK_OFF}
    },
    [CUSTOM_FRONTIER_MON_HARIYAMA_GENERIC_2] = {
        .species = SPECIES_HARIYAMA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_BULLET_PUNCH, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DELCATTY_GENERIC_1] = {
        .species = SPECIES_DELCATTY,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_NORMALIZE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_ICE_BEAM, MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_HELPING_HAND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DELCATTY_GENERIC_2] = {
        .species = SPECIES_DELCATTY,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_NORMALIZE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPER_BEAM, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_SABLEYE_GENERIC_1] = {
        .species = SPECIES_SABLEYE,
        .heldItem = ITEM_ROSELI_BERRY,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_PARTING_SHOT, MOVE_FAKE_OUT, MOVE_ENCORE, MOVE_FOUL_PLAY},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SABLEYE_GENERIC_2] = {
        .species = SPECIES_SABLEYE,
        .heldItem = ITEM_LAGGING_TAIL,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(248, 0, 252, 0, 0, 8),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_TRICK, MOVE_FAKE_OUT, MOVE_FOUL_PLAY, MOVE_PARTING_SHOT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAWILE_GENERIC_1] = {
        .species = SPECIES_MAWILE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOUL_PLAY, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_STEALTH_ROCK}
    },
    [CUSTOM_FRONTIER_MON_MAWILE_GENERIC_2] = {
        .species = SPECIES_MAWILE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_HYPER_CUTTER,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SUCKER_PUNCH, MOVE_PLAY_ROUGH, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AGGRON_GENERIC_1] = {
        .species = SPECIES_AGGRON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .moves = {MOVE_HEAD_SMASH, MOVE_HIGH_HORSEPOWER, MOVE_HEAVY_SLAM, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
        .nature = NATURE_ADAMANT
    },
    [CUSTOM_FRONTIER_MON_AGGRON_GENERIC_2] = {
        .species = SPECIES_AGGRON,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_HEAVY_METAL,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_PROTECT, MOVE_HEAVY_SLAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MEDICHAM_GENERIC_1] = {
        .species = SPECIES_MEDICHAM,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_PURE_POWER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BULLET_PUNCH, MOVE_CLOSE_COMBAT, MOVE_ZEN_HEADBUTT, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MEDICHAM_GENERIC_2] = {
        .species = SPECIES_MEDICHAM,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_PURE_POWER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_ZEN_HEADBUTT, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MANECTRIC_GENERIC_1] = {
        .species = SPECIES_MANECTRIC,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_THUNDERBOLT, MOVE_OVERHEAT, MOVE_SNARL}
    },
    [CUSTOM_FRONTIER_MON_MANECTRIC_GENERIC_2] = {
        .species = SPECIES_MANECTRIC,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_MINUS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_HYPER_VOICE, MOVE_OVERHEAT}
    },
    [CUSTOM_FRONTIER_MON_PLUSLE_GENERIC_1] = {
        .species = SPECIES_PLUSLE,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_GRASS_KNOT, MOVE_THUNDERBOLT, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_PLUSLE_GENERIC_2] = {
        .species = SPECIES_PLUSLE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PLUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .moves = {MOVE_ENCORE, MOVE_THUNDERBOLT, MOVE_HELPING_HAND, MOVE_NUZZLE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MINUN_GENERIC_1] = {
        .species = SPECIES_MINUN,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_MINUS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_THUNDERBOLT, MOVE_ALLURING_VOICE, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_MINUN_GENERIC_2] = {
        .species = SPECIES_MINUN,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MINUS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_ENCORE, MOVE_THUNDERBOLT, MOVE_HELPING_HAND, MOVE_NUZZLE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VOLBEAT_GENERIC_1] = {
        .species = SPECIES_VOLBEAT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_SEISMIC_TOSS, MOVE_LIGHT_SCREEN}
    },
    [CUSTOM_FRONTIER_MON_VOLBEAT_GENERIC_2] = {
        .species = SPECIES_VOLBEAT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_TAIL_GLOW, MOVE_BATON_PASS, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_ILLUMISE_GENERIC_1] = {
        .species = SPECIES_ILLUMISE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_CHARM, MOVE_SEISMIC_TOSS, MOVE_LIGHT_SCREEN}
    },
    [CUSTOM_FRONTIER_MON_ILLUMISE_GENERIC_2] = {
        .species = SPECIES_ILLUMISE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_BUG_BUZZ, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_SWALOT_GENERIC_1] = {
        .species = SPECIES_SWALOT,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_STICKY_HOLD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_PROTECT, MOVE_VENOSHOCK, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_SWALOT_GENERIC_2] = {
        .species = SPECIES_SWALOT,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_STICKY_HOLD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ACID_ARMOR, MOVE_BODY_PRESS, MOVE_CLEAR_SMOG, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SHARPEDO_GENERIC_1] = {
        .species = SPECIES_SHARPEDO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_CRUNCH, MOVE_LIQUIDATION, MOVE_SWORDS_DANCE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHARPEDO_GENERIC_2] = {
        .species = SPECIES_SHARPEDO,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_LIQUIDATION, MOVE_CRUNCH, MOVE_PSYCHIC_FANGS},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WAILORD_GENERIC_1] = {
        .species = SPECIES_WAILORD,
        .heldItem = ITEM_BRIGHT_POWDER,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MAELSTROM, MOVE_ICE_BEAM, MOVE_PROTECT, MOVE_DOUBLE_TEAM}
    },
    [CUSTOM_FRONTIER_MON_WAILORD_GENERIC_2] = {
        .species = SPECIES_WAILORD,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MAELSTROM, MOVE_ICY_WIND, MOVE_ICE_BEAM, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CAMERUPT_GENERIC_1] = {
        .species = SPECIES_CAMERUPT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SIMPLE,
        .ev = TRAINER_PARTY_EVS(160, 0, 0, 96, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ROCK_POLISH, MOVE_FIRE_BLAST, MOVE_EARTH_POWER, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_CAMERUPT_GENERIC_2] = {
        .species = SPECIES_CAMERUPT,
        .heldItem = ITEM_PASSHO_BERRY,
        .ability = ABILITY_MAGMA_ARMOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_WILL_O_WISP, MOVE_HOWL, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TORKOAL_GENERIC_1] = {
        .species = SPECIES_TORKOAL,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_DROUGHT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SOLAR_BEAM, MOVE_FIRE_BLAST, MOVE_SLUDGE_BOMB, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_TORKOAL_GENERIC_2] = {
        .species = SPECIES_TORKOAL,
        .heldItem = ITEM_SHUCA_BERRY,
        .ability = ABILITY_DROUGHT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_PROTECT, MOVE_FIRE_BLAST, MOVE_FISSURE}
    },
    [CUSTOM_FRONTIER_MON_GRUMPIG_GENERIC_1] = {
        .species = SPECIES_GRUMPIG,
        .heldItem = ITEM_COLBUR_BERRY,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_PSYSHOCK, MOVE_EARTH_POWER, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_GRUMPIG_GENERIC_2] = {
        .species = SPECIES_GRUMPIG,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_OWN_TEMPO,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PSYSHOCK, MOVE_DAZZLING_GLEAM, MOVE_FLASH, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_SPINDA_GENERIC_1] = {
        .species = SPECIES_SPINDA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_RAGEBAITER,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_IMPISH,
        .moves = {MOVE_FAKE_OUT, MOVE_SEISMIC_TOSS, MOVE_SUCKER_PUNCH, MOVE_ICY_WIND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SPINDA_GENERIC_2] = {
        .species = SPECIES_SPINDA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_RAGEBAITER,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_BODY_SLAM, MOVE_SUCKER_PUNCH, MOVE_DIZZY_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FLYGON_GENERIC_1] = {
        .species = SPECIES_FLYGON,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_SANDSWORN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRAGON_DANCE, MOVE_GROUND_SLAM, MOVE_DRAGON_CLAW, MOVE_FIRST_IMPRESSION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FLYGON_GENERIC_2] = {
        .species = SPECIES_FLYGON,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SANDSWORN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_GROUND_SLAM, MOVE_DRAGON_CLAW, MOVE_FIRST_IMPRESSION, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CACTURNE_GENERIC_1] = {
        .species = SPECIES_CACTURNE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_WATER_ABSORB,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SUCKER_PUNCH, MOVE_SEED_BOMB, MOVE_SPIKY_SHIELD},
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
    },
    [CUSTOM_FRONTIER_MON_CACTURNE_GENERIC_2] = {
        .species = SPECIES_CACTURNE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SAND_RUSH,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SANDSTORM, MOVE_KNOCK_OFF, MOVE_SEED_BOMB, MOVE_SPIKY_SHIELD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ALTARIA_GENERIC_1] = {
        .species = SPECIES_ALTARIA,
        .heldItem = ITEM_YACHE_BERRY,
        .ability = ABILITY_CLOUD_NINE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_SKY_ATTACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ALTARIA_GENERIC_2] = {
        .species = SPECIES_ALTARIA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_FLUFFY,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_SKY_ATTACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZANGOOSE_GENERIC_1] = {
        .species = SPECIES_ZANGOOSE,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_TOXIC_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_CLOSE_COMBAT, MOVE_FACADE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZANGOOSE_GENERIC_2] = {
        .species = SPECIES_ZANGOOSE,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_TOXIC_BOOST,
        .moves = {MOVE_QUICK_ATTACK, MOVE_FACADE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEVIPER_GENERIC_1] = {
        .species = SPECIES_SEVIPER,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INFILTRATOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TRAILBLAZE, MOVE_GUNK_SHOT, MOVE_KNOCK_OFF, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEVIPER_GENERIC_2] = {
        .species = SPECIES_SEVIPER,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_INFILTRATOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SUCKER_PUNCH, MOVE_GUNK_SHOT, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUNATONE_GENERIC_1] = {
        .species = SPECIES_LUNATONE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_HYPNOSIS, MOVE_TRICK_ROOM, MOVE_METEOR_BEAM, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_LUNATONE_GENERIC_2] = {
        .species = SPECIES_LUNATONE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_METEOR_BEAM, MOVE_HYPNOSIS, MOVE_MOONBLAST}
    },
    [CUSTOM_FRONTIER_MON_SOLROCK_GENERIC_1] = {
        .species = SPECIES_SOLROCK,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ROCK_SLIDE, MOVE_ZEN_HEADBUTT, MOVE_HYPNOSIS}
    },
    [CUSTOM_FRONTIER_MON_SOLROCK_GENERIC_2] = {
        .species = SPECIES_SOLROCK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_POLISH, MOVE_HYPNOSIS, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WHISCASH_GENERIC_1] = {
        .species = SPECIES_WHISCASH,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRAGON_DANCE, MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WHISCASH_GENERIC_2] = {
        .species = SPECIES_WHISCASH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_EARTH_EATER,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HELPING_HAND, MOVE_ICY_WIND, MOVE_FISSURE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CRAWDAUNT_GENERIC_1] = {
        .species = SPECIES_CRAWDAUNT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_HYPER_CUTTER,
        .ev = TRAINER_PARTY_EVS(40, 252, 0, 216, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_CRABHAMMER, MOVE_KNOCK_OFF, MOVE_AQUA_JET},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRAWDAUNT_GENERIC_2] = {
        .species = SPECIES_CRAWDAUNT,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(36, 252, 0, 216, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_LIQUIDATION, MOVE_KNOCK_OFF, MOVE_AQUA_JET},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CLAYDOL_GENERIC_1] = {
        .species = SPECIES_CLAYDOL,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_COSMIC_POWER, MOVE_BODY_PRESS, MOVE_STORED_POWER, MOVE_TRICK_ROOM}
    },
    [CUSTOM_FRONTIER_MON_CLAYDOL_GENERIC_2] = {
        .species = SPECIES_CLAYDOL,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_GRAVITATION,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ROCK_SLIDE, MOVE_ALLY_SWITCH, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CRADILY_GENERIC_1] = {
        .species = SPECIES_CRADILY,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_INGRAIN, MOVE_SYNTHESIS, MOVE_TOXIC, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRADILY_GENERIC_2] = {
        .species = SPECIES_CRADILY,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_METEOR_BEAM, MOVE_GRASS_KNOT, MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_ARMALDO_GENERIC_1] = {
        .species = SPECIES_ARMALDO,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_ROCK_SLIDE, MOVE_X_SCISSOR, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARMALDO_GENERIC_2] = {
        .species = SPECIES_ARMALDO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_BATTLE_ARMOR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_POLISH, MOVE_X_SCISSOR, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MILOTIC_GENERIC_1] = {
        .species = SPECIES_MILOTIC,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HELPING_HAND, MOVE_PROTECT, MOVE_SCALD, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_MILOTIC_GENERIC_2] = {
        .species = SPECIES_MILOTIC,
        .heldItem = ITEM_ADRENALINE_ORB,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_ALLURING_VOICE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CASTFORM_GENERIC_1] = {
        .species = SPECIES_CASTFORM,
        .heldItem = ITEM_DAMP_ROCK,
        .ability = ABILITY_FORECAST,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_WEATHER_BALL, MOVE_PROTECT, MOVE_HURRICANE}
    },
    [CUSTOM_FRONTIER_MON_CASTFORM_GENERIC_2] = {
        .species = SPECIES_CASTFORM,
        .heldItem = ITEM_HEAT_ROCK,
        .ability = ABILITY_FORECAST,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_TAILWIND, MOVE_WEATHER_BALL, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_KECLEON_GENERIC_1] = {
        .species = SPECIES_KECLEON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_PROTEAN,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_SEISMIC_TOSS, MOVE_ICY_WIND, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KECLEON_GENERIC_2] = {
        .species = SPECIES_KECLEON,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PROTEAN,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_DOUBLE_EDGE, MOVE_KNOCK_OFF, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_BANETTE_GENERIC_1] = {
        .species = SPECIES_BANETTE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_DOUBLE_EDGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BANETTE_GENERIC_2] = {
        .species = SPECIES_BANETTE,
        .heldItem = ITEM_COLBUR_BERRY,
        .ability = ABILITY_FLUFFY,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_WILL_O_WISP, MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_POLTERGEIST},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TROPIUS_GENERIC_1] = {
        .species = SPECIES_TROPIUS,
        .heldItem = ITEM_ENIGMA_BERRY,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_WIDE_GUARD, MOVE_GRASS_KNOT, MOVE_HURRICANE}
    },
    [CUSTOM_FRONTIER_MON_TROPIUS_GENERIC_2] = {
        .species = SPECIES_TROPIUS,
        .heldItem = ITEM_STRANGE_SHIELD,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_ROAR, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_CHIMECHO_GENERIC_1] = {
        .species = SPECIES_CHIMECHO,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_ATONAL_BELLS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYCHIC_NOISE, MOVE_SNARL, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_CHIMECHO_GENERIC_2] = {
        .species = SPECIES_CHIMECHO,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_ATONAL_BELLS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PERISH_SONG, MOVE_PROTECT, MOVE_PSYCHIC_NOISE, MOVE_ALLY_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_DUSCLOPS_GENERIC_1] = {
        .species = SPECIES_DUSCLOPS,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_FRISK,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_TRICK_ROOM, MOVE_DESTINY_BOND, MOVE_POLTERGEIST}
    },
    [CUSTOM_FRONTIER_MON_DUSCLOPS_GENERIC_2] = {
        .species = SPECIES_DUSCLOPS,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_ALLY_SWITCH, MOVE_WILL_O_WISP, MOVE_POLTERGEIST, MOVE_SEISMIC_TOSS},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ABSOL_GENERIC_1] = {
        .species = SPECIES_ABSOL,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_CLOSE_COMBAT, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ABSOL_GENERIC_2] = {
        .species = SPECIES_ABSOL,
        .heldItem = ITEM_SCOPE_LENS,
        .ability = ABILITY_SUPER_LUCK,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT, MOVE_SHADOW_CLAW, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GLALIE_GENERIC_1] = {
        .species = SPECIES_GLALIE,
        .heldItem = ITEM_CHESTO_BERRY,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_FREEZE_DRY, MOVE_REST, MOVE_SLEEP_TALK}
    },
    [CUSTOM_FRONTIER_MON_GLALIE_GENERIC_2] = {
        .species = SPECIES_GLALIE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .moves = {MOVE_FOUL_PLAY, MOVE_PROTECT, MOVE_ICE_SHARD, MOVE_ICY_WIND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WALREIN_GENERIC_1] = {
        .species = SPECIES_WALREIN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ICY_WIND, MOVE_SHEER_COLD}
    },
    [CUSTOM_FRONTIER_MON_WALREIN_GENERIC_2] = {
        .species = SPECIES_WALREIN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_OBLIVIOUS,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ICICLE_SPEAR, MOVE_LIQUIDATION, MOVE_IRON_HEAD, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HUNTAIL_GENERIC_1] = {
        .species = SPECIES_HUNTAIL,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_STRONG_JAW,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHELL_SMASH, MOVE_CRUNCH, MOVE_FISHIOUS_REND, MOVE_PSYCHIC_FANGS},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HUNTAIL_GENERIC_2] = {
        .species = SPECIES_HUNTAIL,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_STRONG_JAW,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CRUNCH, MOVE_PSYCHIC_FANGS, MOVE_LIQUIDATION, MOVE_ICE_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOREBYSS_GENERIC_1] = {
        .species = SPECIES_GOREBYSS,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHELL_SMASH, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PSYCHIC}
    },
    [CUSTOM_FRONTIER_MON_GOREBYSS_GENERIC_2] = {
        .species = SPECIES_GOREBYSS,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_SHADOW_BALL, MOVE_PSYCHIC}
    },
    [CUSTOM_FRONTIER_MON_RELICANTH_GENERIC_1] = {
        .species = SPECIES_RELICANTH,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_ROCK_HEAD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_POLISH, MOVE_HEAD_SMASH, MOVE_LIQUIDATION, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RELICANTH_GENERIC_2] = {
        .species = SPECIES_RELICANTH,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_LIQUIDATION, MOVE_ROCK_SLIDE, MOVE_ZEN_HEADBUTT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUVDISC_GENERIC_1] = {
        .species = SPECIES_LUVDISC,
        .heldItem = ITEM_HEART_SCALE,
        .ability = ABILITY_SOUL_HEART,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_MOONBLAST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_LUVDISC_GENERIC_2] = {
        .species = SPECIES_LUVDISC,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SOUL_HEART,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_MOONBLAST, MOVE_BLIZZARD}
    },
    [CUSTOM_FRONTIER_MON_SALAMENCE_GENERIC_1] = {
        .species = SPECIES_SALAMENCE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HEAT_WAVE, MOVE_DRACO_METEOR, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SALAMENCE_GENERIC_2] = {
        .species = SPECIES_SALAMENCE,
        .heldItem = ITEM_YACHE_BERRY,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DUAL_WINGBEAT, MOVE_IRON_HEAD, MOVE_DRAGON_CLAW},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_METAGROSS_GENERIC_1] = {
        .species = SPECIES_METAGROSS,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(140, 252, 0, 116, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHIFT_GEAR, MOVE_HEAVY_SLAM, MOVE_PSYCHIC_FANGS, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_METAGROSS_GENERIC_2] = {
        .species = SPECIES_METAGROSS,
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_HEAVY_SLAM, MOVE_PSYCHIC_FANGS, MOVE_STOMPING_TANTRUM, MOVE_TRICK_ROOM}
    },
    [CUSTOM_FRONTIER_MON_REGIROCK_GENERIC_1] = {
        .species = SPECIES_REGIROCK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ARCHAIC_SHARDS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_BODY_PRESS, MOVE_ROCK_SLIDE, MOVE_HEAVY_SLAM, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_REGIROCK_GENERIC_2] = {
        .species = SPECIES_REGIROCK,
        .heldItem = ITEM_CHESTO_BERRY,
        .ability = ABILITY_ARCHAIC_SHARDS,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_THUNDER_WAVE, MOVE_REST}
    },
    [CUSTOM_FRONTIER_MON_REGICE_GENERIC_1] = {
        .species = SPECIES_REGICE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ABSOLUTE_ZERO,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOCUS_BLAST, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_REGICE_GENERIC_2] = {
        .species = SPECIES_REGICE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_ABSOLUTE_ZERO,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LOCK_ON, MOVE_SHEER_COLD, MOVE_PROTECT, MOVE_THUNDER_WAVE}
    },
    [CUSTOM_FRONTIER_MON_REGISTEEL_GENERIC_1] = {
        .species = SPECIES_REGISTEEL,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_METALLICIZE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_STOMPING_TANTRUM, MOVE_CURSE, MOVE_EXPLOSION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_REGISTEEL_GENERIC_2] = {
        .species = SPECIES_REGISTEEL,
        .heldItem = ITEM_CHESTO_BERRY,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_THUNDER_WAVE, MOVE_REST}
    },
    [CUSTOM_FRONTIER_MON_LATIAS_GENERIC_1] = {
        .species = SPECIES_LATIAS,
        .heldItem = ITEM_SOUL_DEW,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_PSYSHOCK, MOVE_DRAGON_PULSE}
    },
    [CUSTOM_FRONTIER_MON_LATIAS_GENERIC_2] = {
        .species = SPECIES_LATIAS,
        .heldItem = ITEM_SOUL_DEW,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_ICE_BEAM, MOVE_MIST_BALL, MOVE_DRACO_METEOR}
    },
    [CUSTOM_FRONTIER_MON_LATIOS_GENERIC_1] = {
        .species = SPECIES_LATIOS,
        .heldItem = ITEM_SOUL_DEW,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_AURA_SPHERE, MOVE_DRACO_METEOR, MOVE_LUSTER_PURGE}
    },
    [CUSTOM_FRONTIER_MON_LATIOS_GENERIC_2] = {
        .species = SPECIES_LATIOS,
        .heldItem = ITEM_SOUL_DEW,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_DRACO_METEOR, MOVE_LUSTER_PURGE, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_JIRACHI_GENERIC_1] = {
        .species = SPECIES_JIRACHI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_IRON_HEAD, MOVE_TRICK, MOVE_ZEN_HEADBUTT, MOVE_BODY_SLAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_JIRACHI_GENERIC_2] = {
        .species = SPECIES_JIRACHI,
        .heldItem = ITEM_COLBUR_BERRY,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_PROTECT, MOVE_DOOM_DESIRE, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_GENERIC_1] = {
        .species = SPECIES_DEOXYS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 4, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_PSYCHO_BOOST, MOVE_SUPERPOWER, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_GENERIC_2] = {
        .species = SPECIES_DEOXYS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 4, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_PSYCHO_BOOST, MOVE_EXTREME_SPEED, MOVE_SUPERPOWER, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_ATTACK_GENERIC_1] = {
        .species = SPECIES_DEOXYS_ATTACK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 4, 0, 252, 252, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_PSYCHO_BOOST, MOVE_ICE_BEAM, MOVE_SUPERPOWER, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_ATTACK_GENERIC_2] = {
        .species = SPECIES_DEOXYS_ATTACK,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 4, 0),
        .nature = NATURE_HASTY,
        .moves = {MOVE_PROTECT, MOVE_PSYCHO_BOOST, MOVE_KNOCK_OFF, MOVE_SUPERPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_DEFENSE_GENERIC_1] = {
        .species = SPECIES_DEOXYS_DEFENSE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_COSMIC_POWER, MOVE_STORED_POWER, MOVE_NIGHT_SHADE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_DEFENSE_GENERIC_2] = {
        .species = SPECIES_DEOXYS_DEFENSE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_COSMIC_POWER, MOVE_STORED_POWER, MOVE_NIGHT_SHADE}
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_SPEED_GENERIC_1] = {
        .species = SPECIES_DEOXYS_SPEED,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(192, 0, 252, 60, 4, 0),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICY_WIND, MOVE_TAUNT, MOVE_PSYCHO_BOOST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_DEOXYS_SPEED_GENERIC_2] = {
        .species = SPECIES_DEOXYS_SPEED,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(192, 252, 4, 60, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TRICK, MOVE_SUPERPOWER, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PORYGON_2_GENERIC_1] = {
        .species = SPECIES_PORYGON2,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_DOWNLOAD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_TRI_ATTACK, MOVE_RECOVER, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_PORYGON_2_GENERIC_2] = {
        .species = SPECIES_PORYGON2,
        .heldItem = ITEM_EVIOLITE,
        .ability = ABILITY_DOWNLOAD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_RECOVER, MOVE_THUNDERBOLT, MOVE_TRI_ATTACK}
    },
    [CUSTOM_FRONTIER_MON_TORTERRA_GENERIC_1] = {
        .species = SPECIES_TORTERRA,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_SHELL_ARMOR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHELL_SMASH, MOVE_HIGH_HORSEPOWER, MOVE_BULLET_SEED, MOVE_ROCK_BLAST},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TORTERRA_GENERIC_2] = {
        .species = SPECIES_TORTERRA,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STAMINA,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_PROTECT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_INFERNAPE_GENERIC_1] = {
        .species = SPECIES_INFERNAPE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_OVERHEAT, MOVE_AURA_SPHERE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_INFERNAPE_GENERIC_2] = {
        .species = SPECIES_INFERNAPE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_CLOSE_COMBAT, MOVE_FLARE_BLITZ},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EMPOLEON_GENERIC_1] = {
        .species = SPECIES_EMPOLEON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_QUEENLY_MAJESTY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WHIRLPOOL, MOVE_ROOST, MOVE_PROTECT, MOVE_AQUA_RING}
    },
    [CUSTOM_FRONTIER_MON_EMPOLEON_GENERIC_2] = {
        .species = SPECIES_EMPOLEON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FLASH_CANNON, MOVE_HYDRO_PUMP, MOVE_GRASS_KNOT, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_STARAPTOR_GENERIC_1] = {
        .species = SPECIES_STARAPTOR,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FINAL_GAMBIT, MOVE_BRAVE_BIRD, MOVE_DOUBLE_EDGE, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STARAPTOR_GENERIC_2] = {
        .species = SPECIES_STARAPTOR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_RECKLESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_BRAVE_BIRD, MOVE_U_TURN, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BIBAREL_GENERIC_1] = {
        .species = SPECIES_BIBAREL,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MOODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 120, 0, 0, 136),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_PROTECT, MOVE_CURSE, MOVE_LIQUIDATION, MOVE_LAST_RESORT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BIBAREL_GENERIC_2] = {
        .species = SPECIES_BIBAREL,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_SIMPLE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_AQUA_JET, MOVE_PROTECT, MOVE_QUICK_ATTACK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KRICKETUNE_GENERIC_1] = {
        .species = SPECIES_KRICKETUNE,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_CONCERTMASTER,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_STICKY_WEB, MOVE_LUNGE, MOVE_LEECH_LIFE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KRICKETUNE_GENERIC_2] = {
        .species = SPECIES_KRICKETUNE,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_CONCERTMASTER,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_LEECH_LIFE, MOVE_LUNGE, MOVE_KNOCK_OFF, MOVE_NIGHT_SLASH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUXRAY_GENERIC_1] = {
        .species = SPECIES_LUXRAY,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_SUPERCELL_SLAM, MOVE_PLAY_ROUGH, MOVE_THROAT_CHOP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUXRAY_GENERIC_2] = {
        .species = SPECIES_LUXRAY,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_PROTECT, MOVE_SUPERCELL_SLAM, MOVE_FACADE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ROSERADE_GENERIC_1] = {
        .species = SPECIES_ROSERADE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_POISON_POINT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLEEP_POWDER, MOVE_PROTECT, MOVE_SLUDGE_BOMB, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_ROSERADE_GENERIC_2] = {
        .species = SPECIES_ROSERADE,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_POISON_POINT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LEAF_STORM, MOVE_DAZZLING_GLEAM, MOVE_SLUDGE_BOMB, MOVE_WEATHER_BALL}
    },
    [CUSTOM_FRONTIER_MON_RAMPARDOS_GENERIC_1] = {
        .species = SPECIES_RAMPARDOS,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_POLISH, MOVE_ROCK_SLIDE, MOVE_FIRE_PUNCH, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RAMPARDOS_GENERIC_2] = {
        .species = SPECIES_RAMPARDOS,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_HEAD_SMASH, MOVE_ZEN_HEADBUTT, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BASTIODON_GENERIC_1] = {
        .species = SPECIES_BASTIODON,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_WIDE_GUARD, MOVE_METAL_BURST}
    },
    [CUSTOM_FRONTIER_MON_BASTIODON_GENERIC_2] = {
        .species = SPECIES_BASTIODON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STOUT_SHIELD,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_FOUL_PLAY, MOVE_BODY_PRESS, MOVE_METAL_BURST, MOVE_BULLDOZE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_PLANT_GENERIC_1] = {
        .species = SPECIES_WORMADAM,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ALLY_SWITCH, MOVE_BUG_BUZZ, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_PLANT_GENERIC_2] = {
        .species = SPECIES_WORMADAM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ANTICIPATION,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ENDEAVOR, MOVE_ALLY_SWITCH, MOVE_STRUGGLE_BUG}
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_SANDY_GENERIC_1] = {
        .species = SPECIES_WORMADAM_SANDY,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_RELAXED,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ALLY_SWITCH, MOVE_FISSURE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_SANDY_GENERIC_2] = {
        .species = SPECIES_WORMADAM_SANDY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_RELAXED,
        .moves = {MOVE_TRICK_ROOM, MOVE_BUG_BITE, MOVE_ENDEAVOR, MOVE_ALLY_SWITCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_TRASH_GENERIC_1] = {
        .species = SPECIES_WORMADAM_TRASH,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_GYRO_BALL, MOVE_PROTECT, MOVE_ALLY_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_WORMADAM_TRASH_GENERIC_2] = {
        .species = SPECIES_WORMADAM_TRASH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_MOTHIM_GENERIC_1] = {
        .species = SPECIES_MOTHIM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_TAILWIND, MOVE_BUG_BUZZ, MOVE_ELECTROWEB}
    },
    [CUSTOM_FRONTIER_MON_MOTHIM_GENERIC_2] = {
        .species = SPECIES_MOTHIM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_PSYCHIC, MOVE_BUG_BUZZ, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_VESPIQUEN_GENERIC_1] = {
        .species = SPECIES_VESPIQUEN,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_QUEENLY_MAJESTY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ATTACK_ORDER, MOVE_ROOST, MOVE_DUAL_WINGBEAT}
    },
    [CUSTOM_FRONTIER_MON_VESPIQUEN_GENERIC_2] = {
        .species = SPECIES_VESPIQUEN,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_QUEENLY_MAJESTY,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_DEFEND_ORDER, MOVE_HELPING_HAND, MOVE_ROOST}
    },
    [CUSTOM_FRONTIER_MON_PACHIRISU_GENERIC_1] = {
        .species = SPECIES_PACHIRISU,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FOLLOW_ME, MOVE_PROTECT, MOVE_NUZZLE, MOVE_ENDEAVOR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PACHIRISU_GENERIC_2] = {
        .species = SPECIES_PACHIRISU,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_GENTLE,
        .moves = {MOVE_SUPER_FANG, MOVE_NUZZLE, MOVE_ENDEAVOR, MOVE_GRASS_KNOT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FLOATZEL_GENERIC_1] = {
        .species = SPECIES_FLOATZEL,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_WAVE_CRASH, MOVE_FLIP_TURN, MOVE_ICE_SPINNER, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FLOATZEL_GENERIC_2] = {
        .species = SPECIES_FLOATZEL,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_RAIN_DANCE, MOVE_LIQUIDATION, MOVE_ICE_SPINNER, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CHERRIM_GENERIC_1] = {
        .species = SPECIES_CHERRIM,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_FLOWER_GIFT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_HELPING_HAND, MOVE_PROTECT, MOVE_GIGA_DRAIN}
    },
    [CUSTOM_FRONTIER_MON_CHERRIM_GENERIC_2] = {
        .species = SPECIES_CHERRIM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FLOWER_GIFT,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_PROTECT, MOVE_WEATHER_BALL, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_GASTRODON_GENERIC_1] = {
        .species = SPECIES_GASTRODON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_SCALD, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_GASTRODON_GENERIC_2] = {
        .species = SPECIES_GASTRODON_EAST,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_RECOVER}
    },
    [CUSTOM_FRONTIER_MON_AMBIPOM_GENERIC_1] = {
        .species = SPECIES_AMBIPOM,
        .heldItem = ITEM_SILK_SCARF,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_U_TURN, MOVE_DOUBLE_EDGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AMBIPOM_GENERIC_2] = {
        .species = SPECIES_AMBIPOM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_LOW_KICK, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DRIFBLIM_GENERIC_1] = {
        .species = SPECIES_DRIFBLIM,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_FLARE_BOOST,
        .ev = TRAINER_PARTY_EVS(220, 0, 0, 36, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_SHADOW_BALL, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_DRIFBLIM_GENERIC_2] = {
        .species = SPECIES_DRIFBLIM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_UNBURDEN,
        .ev = TRAINER_PARTY_EVS(220, 0, 0, 36, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_STRENGTH_SAP, MOVE_CALM_MIND, MOVE_SHADOW_BALL, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_LOPUNNY_GENERIC_1] = {
        .species = SPECIES_LOPUNNY,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_KLUTZ,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWITCHEROO, MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_PLAY_ROUGH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LOPUNNY_GENERIC_2] = {
        .species = SPECIES_LOPUNNY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LIMBER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_SWORDS_DANCE, MOVE_TRIPLE_AXEL, MOVE_GIGA_IMPACT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MISMAGIUS_GENERIC_1] = {
        .species = SPECIES_MISMAGIUS,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_MYSTICAL_FIRE, MOVE_POWER_GEM}
    },
    [CUSTOM_FRONTIER_MON_MISMAGIUS_GENERIC_2] = {
        .species = SPECIES_MISMAGIUS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_POWER_GEM}
    },
    [CUSTOM_FRONTIER_MON_HONCHKROW_GENERIC_1] = {
        .species = SPECIES_HONCHKROW,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MOXIE,
        .ev = TRAINER_PARTY_EVS(148, 252, 0, 108, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_DRILL_PECK, MOVE_LASH_OUT, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HONCHKROW_GENERIC_2] = {
        .species = SPECIES_HONCHKROW,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MOXIE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 248, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_SUCKER_PUNCH, MOVE_U_TURN, MOVE_BRAVE_BIRD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PURUGLY_GENERIC_1] = {
        .species = SPECIES_PURUGLY,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PLAY_ROUGH, MOVE_KNOCK_OFF, MOVE_BODY_SLAM, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_PURUGLY_GENERIC_2] = {
        .species = SPECIES_PURUGLY,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_THICK_FAT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_BODY_SLAM, MOVE_FOUL_PLAY, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SKUNTANK_GENERIC_1] = {
        .species = SPECIES_SKUNTANK,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_AFTERMATH,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SUCKER_PUNCH, MOVE_GUNK_SHOT, MOVE_KNOCK_OFF, MOVE_EXPLOSION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SKUNTANK_GENERIC_2] = {
        .species = SPECIES_SKUNTANK,
        .heldItem = ITEM_BLACK_SLUDGE,
        .ability = ABILITY_AFTERMATH,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 0, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOUL_PLAY, MOVE_HELPING_HAND, MOVE_PROTECT, MOVE_TAUNT}
    },
    [CUSTOM_FRONTIER_MON_BRONZONG_GENERIC_1] = {
        .species = SPECIES_BRONZONG,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_GYRO_BALL, MOVE_HYPNOSIS, MOVE_ALLY_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_BRONZONG_GENERIC_2] = {
        .species = SPECIES_BRONZONG,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_HYPNOSIS, MOVE_PSYCHIC_NOISE, MOVE_REFLECT}
    },
    [CUSTOM_FRONTIER_MON_CHATOT_GENERIC_1] = {
        .species = SPECIES_CHATOT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_KEEN_EYE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CHATTER, MOVE_BOOMBURST, MOVE_PROTECT, MOVE_TAILWIND}
    },
    [CUSTOM_FRONTIER_MON_CHATOT_GENERIC_2] = {
        .species = SPECIES_CHATOT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_KEEN_EYE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_BOOMBURST, MOVE_CHATTER, MOVE_ROOST}
    },
    [CUSTOM_FRONTIER_MON_SPIRITOMB_GENERIC_1] = {
        .species = SPECIES_SPIRITOMB,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_POLTERGEIST, MOVE_SUCKER_PUNCH, MOVE_WILL_O_WISP, MOVE_PAIN_SPLIT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SPIRITOMB_GENERIC_2] = {
        .species = SPECIES_SPIRITOMB,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_POLTERGEIST, MOVE_SUCKER_PUNCH, MOVE_ROCK_TOMB, MOVE_SHADOW_SNEAK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GARCHOMP_GENERIC_1] = {
        .species = SPECIES_GARCHOMP,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_ROUGH_SKIN,
        .ev = TRAINER_PARTY_EVS(116, 252, 0, 140, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SCALE_SHOT, MOVE_STOMPING_TANTRUM, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GARCHOMP_GENERIC_2] = {
        .species = SPECIES_GARCHOMP,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_ROUGH_SKIN,
        .ev = TRAINER_PARTY_EVS(192, 252, 0, 60, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_SLIDE, MOVE_SCALE_SHOT, MOVE_PROTECT, MOVE_DRAGON_CLAW},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUCARIO_GENERIC_1] = {
        .species = SPECIES_LUCARIO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_PROTECT, MOVE_METEOR_MASH, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LUCARIO_GENERIC_2] = {
        .species = SPECIES_LUCARIO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MARTIAL_EXPERT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_AURA_SPHERE, MOVE_FLASH_CANNON, MOVE_DRAGON_PULSE}
    },
    [CUSTOM_FRONTIER_MON_HIPPOWDON_GENERIC_1] = {
        .species = SPECIES_HIPPOWDON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SAND_STREAM,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FISSURE, MOVE_PROTECT, MOVE_SLACK_OFF, MOVE_YAWN}
    },
    [CUSTOM_FRONTIER_MON_HIPPOWDON_GENERIC_2] = {
        .species = SPECIES_HIPPOWDON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SAND_STREAM,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_HEAVY_SLAM, MOVE_ICE_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TOXICROAK_GENERIC_1] = {
        .species = SPECIES_TOXICROAK,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_ANTICIPATION,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_GUNK_SHOT, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TOXICROAK_GENERIC_2] = {
        .species = SPECIES_TOXICROAK,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_ANTICIPATION,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SUCKER_PUNCH, MOVE_GUNK_SHOT, MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CARNIVINE_GENERIC_1] = {
        .species = SPECIES_CARNIVINE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_HUGE_POWER,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_FLY_TRAP, MOVE_POWER_WHIP, MOVE_SUCKER_PUNCH, MOVE_ICE_FANG}
    },
    [CUSTOM_FRONTIER_MON_CARNIVINE_GENERIC_2] = {
        .species = SPECIES_CARNIVINE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_FLY_TRAP, MOVE_KNOCK_OFF, MOVE_ICE_FANG}
    },
    [CUSTOM_FRONTIER_MON_LUMINEON_GENERIC_1] = {
        .species = SPECIES_LUMINEON,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_DAZZLING,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_ENCORE, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_LUMINEON_GENERIC_2] = {
        .species = SPECIES_LUMINEON,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AQUA_RING, MOVE_WHIRLPOOL, MOVE_PROTECT, MOVE_TAILWIND}
    },
    [CUSTOM_FRONTIER_MON_ABOMASNOW_GENERIC_1] = {
        .species = SPECIES_ABOMASNOW,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AURORA_VEIL, MOVE_BLIZZARD, MOVE_GIGA_DRAIN, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ABOMASNOW_GENERIC_2] = {
        .species = SPECIES_ABOMASNOW,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AURORA_VEIL, MOVE_ROCK_SLIDE, MOVE_WOOD_HAMMER, MOVE_ICE_SHARD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WEAVILE_GENERIC_1] = {
        .species = SPECIES_WEAVILE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PICKPOCKET,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_FAKE_OUT, MOVE_TRIPLE_AXEL, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WEAVILE_GENERIC_2] = {
        .species = SPECIES_WEAVILE,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_PRESSURE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRIPLE_AXEL, MOVE_KNOCK_OFF, MOVE_U_TURN, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAGNEZONE_GENERIC_1] = {
        .species = SPECIES_MAGNEZONE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ANALYTIC,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_FLASH_CANNON, MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_BODY_PRESS}
    },
    [CUSTOM_FRONTIER_MON_MAGNEZONE_GENERIC_2] = {
        .species = SPECIES_MAGNEZONE,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_ANALYTIC,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_PROTECT, MOVE_VOLT_SWITCH, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_LICKILICKY_GENERIC_1] = {
        .species = SPECIES_LICKILICKY,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_OWN_TEMPO,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_EXPLOSION, MOVE_GYRO_BALL, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE}
    },
    [CUSTOM_FRONTIER_MON_LICKILICKY_GENERIC_2] = {
        .species = SPECIES_LICKILICKY,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_OWN_TEMPO,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_EXPLOSION, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RHYPERIOR_GENERIC_1] = {
        .species = SPECIES_RHYPERIOR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_POLISH, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_HEAVY_SLAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_RHYPERIOR_GENERIC_2] = {
        .species = SPECIES_RHYPERIOR,
        .heldItem = ITEM_PASSHO_BERRY,
        .ability = ABILITY_SOLID_ROCK,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_ICE_PUNCH}
    },
    [CUSTOM_FRONTIER_MON_TANGROWTH_GENERIC_1] = {
        .species = SPECIES_TANGROWTH,
        .heldItem = ITEM_ENIGMA_BERRY,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_PROTECT, MOVE_ENERGY_BALL, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_TANGROWTH_GENERIC_2] = {
        .species = SPECIES_TANGROWTH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_INGRAIN, MOVE_SLEEP_POWDER, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_ELECTIVIRE_GENERIC_1] = {
        .species = SPECIES_ELECTIVIRE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_SUPERCELL_SLAM, MOVE_TRAILBLAZE, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ELECTIVIRE_GENERIC_2] = {
        .species = SPECIES_ELECTIVIRE,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SUPERCELL_SLAM, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MAGMORTAR_GENERIC_1] = {
        .species = SPECIES_MAGMORTAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FIRE_BLAST, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_MAGMORTAR_GENERIC_2] = {
        .species = SPECIES_MAGMORTAR,
        .heldItem = ITEM_AIR_BALLOON,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FOLLOW_ME, MOVE_WILL_O_WISP, MOVE_PROTECT, MOVE_FIRE_BLAST}
    },
    [CUSTOM_FRONTIER_MON_TOGEKISS_GENERIC_1] = {
        .species = SPECIES_TOGEKISS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_MOONBLAST, MOVE_AIR_SLASH, MOVE_HEAT_WAVE}
    },
    [CUSTOM_FRONTIER_MON_TOGEKISS_GENERIC_2] = {
        .species = SPECIES_TOGEKISS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_FOLLOW_ME, MOVE_PROTECT, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_YANMEGA_GENERIC_1] = {
        .species = SPECIES_YANMEGA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_AIR_SLASH, MOVE_BUG_BUZZ, MOVE_PSYCHIC_NOISE}
    },
    [CUSTOM_FRONTIER_MON_YANMEGA_GENERIC_2] = {
        .species = SPECIES_YANMEGA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_TAILWIND, MOVE_AIR_SLASH, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_LEAFEON_GENERIC_1] = {
        .species = SPECIES_LEAFEON,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_LEAF_BLADE, MOVE_KNOCK_OFF, MOVE_X_SCISSOR, MOVE_GIGA_IMPACT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LEAFEON_GENERIC_2] = {
        .species = SPECIES_LEAFEON,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SUNNY_DAY, MOVE_PROTECT, MOVE_SOLAR_BLADE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GLACEON_GENERIC_1] = {
        .species = SPECIES_GLACEON,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AURORA_VEIL, MOVE_FREEZE_DRY, MOVE_BLIZZARD, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_GLACEON_GENERIC_2] = {
        .species = SPECIES_GLACEON,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BLIZZARD, MOVE_FREEZE_DRY, MOVE_ALLURING_VOICE, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_GLISCOR_GENERIC_1] = {
        .species = SPECIES_GLISCOR,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_POISON_HEAL,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_DUAL_WINGBEAT, MOVE_PROTECT, MOVE_TAILWIND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GLISCOR_GENERIC_2] = {
        .species = SPECIES_GLISCOR,
        .heldItem = ITEM_TOXIC_ORB,
        .ability = ABILITY_POISON_HEAL,
        .ev = TRAINER_PARTY_EVS(252, 0, 184, 72, 0, 0),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_TOXIC, MOVE_ROOST, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_PORYGON_Z_GENERIC_1] = {
        .species = SPECIES_PORYGON_Z,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_DOWNLOAD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BOOMBURST, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_PORYGON_Z_GENERIC_2] = {
        .species = SPECIES_PORYGON_Z,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_DOWNLOAD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BOOMBURST, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_GALLADE_GENERIC_1] = {
        .species = SPECIES_GALLADE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT, MOVE_SACRED_SWORD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GALLADE_GENERIC_2] = {
        .species = SPECIES_GALLADE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_BRAVE,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT, MOVE_SACRED_SWORD}
    },
    [CUSTOM_FRONTIER_MON_PROBOPASS_GENERIC_1] = {
        .species = SPECIES_PROBOPASS,
        .heldItem = ITEM_CHESTO_BERRY,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_REST}
    },
    [CUSTOM_FRONTIER_MON_PROBOPASS_GENERIC_2] = {
        .species = SPECIES_PROBOPASS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_REST, MOVE_SLEEP_TALK}
    },
    [CUSTOM_FRONTIER_MON_DUSKNOIR_GENERIC_1] = {
        .species = SPECIES_DUSKNOIR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SOUL_STRIKES,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PHANTOM_FORCE, MOVE_LEECH_LIFE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DUSKNOIR_GENERIC_2] = {
        .species = SPECIES_DUSKNOIR,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SOUL_STRIKES,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_PHANTOM_FORCE, MOVE_PROTECT, MOVE_WILL_O_WISP, MOVE_HELPING_HAND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FROSLASS_GENERIC_1] = {
        .species = SPECIES_FROSLASS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SNOW_CLOAK,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_WILL_O_WISP, MOVE_POLTERGEIST, MOVE_TRIPLE_AXEL, MOVE_DESTINY_BOND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_FROSLASS_GENERIC_2] = {
        .species = SPECIES_FROSLASS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SNOW_CLOAK,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FRIGID_TOUCH, MOVE_SHADOW_BALL, MOVE_ICE_BEAM, MOVE_DESTINY_BOND}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_GENERIC_1] = {
        .species = SPECIES_ROTOM,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_SHADOW_BALL, MOVE_VOLT_SWITCH, MOVE_ELECTROWEB}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_GENERIC_2] = {
        .species = SPECIES_ROTOM,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ELECTROWEB, MOVE_WILL_O_WISP, MOVE_VOLT_SWITCH, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_HEAT_GENERIC_1] = {
        .species = SPECIES_ROTOM_HEAT,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_OVERHEAT, MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_HEAT_GENERIC_2] = {
        .species = SPECIES_ROTOM_HEAT,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_OVERHEAT, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_WASH_GENERIC_1] = {
        .species = SPECIES_ROTOM_WASH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_THUNDERBOLT, MOVE_HYDRO_PUMP, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_WASH_GENERIC_2] = {
        .species = SPECIES_ROTOM_WASH,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_HYDRO_PUMP, MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_FROST_GENERIC_1] = {
        .species = SPECIES_ROTOM_FROST,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BLIZZARD, MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_FROST_GENERIC_2] = {
        .species = SPECIES_ROTOM_FROST,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_WILL_O_WISP, MOVE_ELECTROWEB, MOVE_BLIZZARD}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_FAN_GENERIC_1] = {
        .species = SPECIES_ROTOM_FAN,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AIR_SLASH, MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_FAN_GENERIC_2] = {
        .species = SPECIES_ROTOM_FAN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 8, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_VOLT_SWITCH, MOVE_AIR_SLASH, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_MOW_GENERIC_1] = {
        .species = SPECIES_ROTOM_MOW,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LEAF_STORM, MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_ROTOM_MOW_GENERIC_2] = {
        .species = SPECIES_ROTOM_MOW,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_LEAF_STORM, MOVE_TRICK}
    },
    [CUSTOM_FRONTIER_MON_UXIE_GENERIC_1] = {
        .species = SPECIES_UXIE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MIND_ETERNAL,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDER_WAVE, MOVE_PSYCHIC_NOISE, MOVE_ENCORE, MOVE_REFLECT}
    },
    [CUSTOM_FRONTIER_MON_UXIE_GENERIC_2] = {
        .species = SPECIES_UXIE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MIND_ETERNAL,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_PROTECT, MOVE_BATON_PASS}
    },
    [CUSTOM_FRONTIER_MON_MESPRIT_GENERIC_1] = {
        .species = SPECIES_MESPRIT,
        .heldItem = ITEM_SCOPE_LENS,
        .ability = ABILITY_HEART_ETERNAL,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_REST, MOVE_SLEEP_TALK}
    },
    [CUSTOM_FRONTIER_MON_MESPRIT_GENERIC_2] = {
        .species = SPECIES_MESPRIT,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_PSYSHOCK, MOVE_ENERGY_BALL, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_AZELF_GENERIC_1] = {
        .species = SPECIES_AZELF,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_BODY_ETERNAL,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PSYSHOCK, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_AZELF_GENERIC_2] = {
        .species = SPECIES_AZELF,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_BODY_ETERNAL,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_PSYSHOCK, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_HEATRAN_GENERIC_1] = {
        .species = SPECIES_HEATRAN,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_EARTH_EATER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_MAGMA_STORM, MOVE_PROTECT, MOVE_WILL_O_WISP, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_HEATRAN_GENERIC_2] = {
        .species = SPECIES_HEATRAN,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_EARTH_EATER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_MAGMA_STORM, MOVE_DRAGON_PULSE}
    },
    [CUSTOM_FRONTIER_MON_REGIGIGAS_GENERIC_1] = {
        .species = SPECIES_REGIGIGAS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SLOW_START,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BODY_SLAM, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_REGIGIGAS_GENERIC_2] = {
        .species = SPECIES_REGIGIGAS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SLOW_START,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SUBSTITUTE, MOVE_PROTECT, MOVE_BODY_PRESS, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRESSELIA_GENERIC_1] = {
        .species = SPECIES_CRESSELIA,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_ALLY_SWITCH, MOVE_LUNAR_BLESSING, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_CRESSELIA_GENERIC_2] = {
        .species = SPECIES_CRESSELIA,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_PROTECT, MOVE_MOONBLAST}
    },
    [CUSTOM_FRONTIER_MON_PHIONE_GENERIC_1] = {
        .species = SPECIES_PHIONE,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_HYDRATION,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ACID_ARMOR, MOVE_TAKE_HEART, MOVE_SCALD, MOVE_AQUA_RING}
    },
    [CUSTOM_FRONTIER_MON_PHIONE_GENERIC_2] = {
        .species = SPECIES_PHIONE,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HYDRATION,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAKE_HEART, MOVE_ALLURING_VOICE, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP}
    },
    [CUSTOM_FRONTIER_MON_MANAPHY_GENERIC_1] = {
        .species = SPECIES_MANAPHY,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_HYDRATION,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_MANAPHY_GENERIC_2] = {
        .species = SPECIES_MANAPHY,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_HYDRATION,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAIL_GLOW, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_SHAYMIN_GENERIC_1] = {
        .species = SPECIES_SHAYMIN,
        .heldItem = ITEM_ENIGMA_BERRY,
        .ability = ABILITY_NATURAL_CURE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_ENERGY_BALL, MOVE_EARTH_POWER, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SHAYMIN_GENERIC_2] = {
        .species = SPECIES_SHAYMIN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_NATURAL_CURE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_TAILWIND, MOVE_PLAY_ROUGH, MOVE_SEED_BOMB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SHAYMIN_SKY_GENERIC_1] = {
        .species = SPECIES_SHAYMIN_SKY,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AIR_SLASH, MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_PSYCHIC}
    },
    [CUSTOM_FRONTIER_MON_SHAYMIN_SKY_GENERIC_2] = {
        .species = SPECIES_SHAYMIN_SKY,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_AIR_SLASH, MOVE_SEED_FLARE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_VICTINI_GENERIC_1] = {
        .species = SPECIES_VICTINI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_VICTORY_STAR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PLAY_ROUGH, MOVE_V_CREATE, MOVE_BOLT_STRIKE, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VICTINI_GENERIC_2] = {
        .species = SPECIES_VICTINI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_VICTORY_STAR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FINAL_GAMBIT, MOVE_MOONBLAST, MOVE_SEARING_SHOT, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_SERPERIOR_GENERIC_1] = {
        .species = SPECIES_SERPERIOR,
        .heldItem = ITEM_BRIGHT_POWDER,
        .ability = ABILITY_CONTRARY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_GLARE, MOVE_LEAF_STORM, MOVE_PROTECT, MOVE_SYNTHESIS}
    },
    [CUSTOM_FRONTIER_MON_SERPERIOR_GENERIC_2] = {
        .species = SPECIES_SERPERIOR,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_CONTRARY,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_LEAF_STORM, MOVE_DRAGON_PULSE, MOVE_GLARE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_EMBOAR_GENERIC_1] = {
        .species = SPECIES_EMBOAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FIRE_PUNCH, MOVE_HIGH_HORSEPOWER, MOVE_IRON_HEAD, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EMBOAR_GENERIC_2] = {
        .species = SPECIES_EMBOAR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FLARE_BLITZ, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAMUROTT_GENERIC_1] = {
        .species = SPECIES_SAMUROTT,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RAZOR_SHELL, MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT, MOVE_FLIP_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAMUROTT_GENERIC_2] = {
        .species = SPECIES_SAMUROTT,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SHELL_ARMOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ICY_WIND, MOVE_ICE_BEAM, MOVE_HYDRO_PUMP, MOVE_VACUUM_WAVE}
    },
    [CUSTOM_FRONTIER_MON_SAMUROTT_HISUI_GENERIC_1] = {
        .species = SPECIES_SAMUROTT_HISUI,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_CEASELESS_EDGE, MOVE_RAZOR_SHELL, MOVE_X_SCISSOR, MOVE_FLIP_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAMUROTT_HISUI_GENERIC_2] = {
        .species = SPECIES_SAMUROTT_HISUI,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHARPNESS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FLIP_TURN, MOVE_CEASELESS_EDGE, MOVE_RAZOR_SHELL, MOVE_X_SCISSOR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WATCHOG_GENERIC_1] = {
        .species = SPECIES_WATCHOG,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_ENDEAVOR, MOVE_PROTECT, MOVE_SUPER_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WATCHOG_GENERIC_2] = {
        .species = SPECIES_WATCHOG,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SUPER_FANG, MOVE_ENDEAVOR, MOVE_FAKE_OUT, MOVE_LOW_KICK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STOUTLAND_GENERIC_1] = {
        .species = SPECIES_STOUTLAND,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_PLAY_ROUGH, MOVE_IRON_HEAD, MOVE_WILD_CHARGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STOUTLAND_GENERIC_2] = {
        .species = SPECIES_STOUTLAND,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_HOWL, MOVE_THUNDER_WAVE, MOVE_HEADBUTT, MOVE_HELPING_HAND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LIEPARD_GENERIC_1] = {
        .species = SPECIES_LIEPARD,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_FAKE_OUT, MOVE_ENCORE, MOVE_THUNDER_WAVE, MOVE_FOUL_PLAY},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LIEPARD_GENERIC_2] = {
        .species = SPECIES_LIEPARD,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_UNBURDEN,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_DARK_PULSE, MOVE_GRASS_KNOT, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SIMISAGE_GENERIC_1] = {
        .species = SPECIES_SIMISAGE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_OVERGROW,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_LEAF_STORM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SIMISAGE_GENERIC_2] = {
        .species = SPECIES_SIMISAGE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_OVERGROW,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_LEAF_STORM, MOVE_ALLURING_VOICE, MOVE_FOCUS_BLAST}
    },
    [CUSTOM_FRONTIER_MON_SIMISEAR_GENERIC_1] = {
        .species = SPECIES_SIMISEAR,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_BLAZE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_FAKE_OUT, MOVE_PROTECT, MOVE_WILL_O_WISP, MOVE_FIRE_BLAST},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SIMISEAR_GENERIC_2] = {
        .species = SPECIES_SIMISEAR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_BLAZE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_ALLURING_VOICE, MOVE_FOCUS_BLAST, MOVE_FIRE_BLAST}
    },
    [CUSTOM_FRONTIER_MON_SIMIPOUR_GENERIC_1] = {
        .species = SPECIES_SIMIPOUR,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_TORRENT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_FAKE_OUT, MOVE_HYDRO_PUMP, MOVE_PROTECT, MOVE_TAUNT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SIMIPOUR_GENERIC_2] = {
        .species = SPECIES_SIMIPOUR,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TORRENT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_HYDRO_PUMP, MOVE_FOCUS_BLAST, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_MUSHARNA_GENERIC_1] = {
        .species = SPECIES_MUSHARNA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_COMATOSE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_CALM_MIND, MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_MUSHARNA_GENERIC_2] = {
        .species = SPECIES_MUSHARNA,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_COMATOSE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYSHOCK, MOVE_ENERGY_BALL, MOVE_DAZZLING_GLEAM}
    },
    [CUSTOM_FRONTIER_MON_UNFEZANT_GENERIC_1] = {
        .species = SPECIES_UNFEZANT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SUPER_LUCK,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_PROTECT, MOVE_GIGA_IMPACT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_UNFEZANT_GENERIC_2] = {
        .species = SPECIES_UNFEZANT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SUPER_LUCK,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAILWIND, MOVE_BRAVE_BIRD, MOVE_GIGA_IMPACT, MOVE_DETECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZEBSTRIKA_GENERIC_1] = {
        .species = SPECIES_ZEBSTRIKA,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_VOLT_SWITCH, MOVE_SUPERCELL_SLAM, MOVE_LOW_KICK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZEBSTRIKA_GENERIC_2] = {
        .species = SPECIES_ZEBSTRIKA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LIGHTNING_ROD,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_PROTECT, MOVE_SUPERCELL_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_THUNDER_WAVE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GIGALITH_GENERIC_1] = {
        .species = SPECIES_GIGALITH,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SAND_STREAM,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_RELAXED,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_BODY_PRESS, MOVE_EXPLOSION, MOVE_ROCK_SLIDE, MOVE_HEAVY_SLAM}
    },
    [CUSTOM_FRONTIER_MON_GIGALITH_GENERIC_2] = {
        .species = SPECIES_GIGALITH,
        .heldItem = ITEM_SHUCA_BERRY,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_WIDE_GUARD, MOVE_ROCK_SLIDE, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SWOOBAT_GENERIC_1] = {
        .species = SPECIES_SWOOBAT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SIMPLE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_TAILWIND, MOVE_AIR_SLASH, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_SWOOBAT_GENERIC_2] = {
        .species = SPECIES_SWOOBAT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_UNAWARE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 252, 4, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_AIR_SLASH, MOVE_CHARM}
    },
    [CUSTOM_FRONTIER_MON_EXCADRILL_GENERIC_1] = {
        .species = SPECIES_EXCADRILL,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_MOLD_BREAKER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EXCADRILL_GENERIC_2] = {
        .species = SPECIES_EXCADRILL,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SAND_RUSH,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_IRON_HEAD, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AUDINO_GENERIC_1] = {
        .species = SPECIES_AUDINO,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_HEALER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_BODY_SLAM, MOVE_HEAL_PULSE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AUDINO_GENERIC_2] = {
        .species = SPECIES_AUDINO,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_HEALER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_AFTER_YOU, MOVE_HELPING_HAND, MOVE_HYPER_VOICE}
    },
    [CUSTOM_FRONTIER_MON_CONKELDURR_GENERIC_1] = {
        .species = SPECIES_CONKELDURR,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FIRE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_ICE_PUNCH, MOVE_MACH_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CONKELDURR_GENERIC_2] = {
        .species = SPECIES_CONKELDURR,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_MACH_PUNCH, MOVE_FACADE, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEISMITOAD_GENERIC_1] = {
        .species = SPECIES_SEISMITOAD,
        .heldItem = ITEM_RINDO_BERRY,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_STOMPING_TANTRUM, MOVE_LIQUIDATION, MOVE_KNOCK_OFF, MOVE_RAIN_DANCE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SEISMITOAD_GENERIC_2] = {
        .species = SPECIES_SEISMITOAD,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_WATER_ABSORB,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_KNOCK_OFF, MOVE_LIQUIDATION, MOVE_STOMPING_TANTRUM, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_THROH_GENERIC_1] = {
        .species = SPECIES_THROH,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_THROH_GENERIC_2] = {
        .species = SPECIES_THROH,
        .heldItem = ITEM_FLAME_ORB,
        .ability = ABILITY_GUTS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_STORM_THROW, MOVE_THUNDER_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAWK_GENERIC_1] = {
        .species = SPECIES_SAWK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAWK_GENERIC_2] = {
        .species = SPECIES_SAWK,
        .heldItem = ITEM_LIECHI_BERRY,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TRAILBLAZE, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LEAVANNY_GENERIC_1] = {
        .species = SPECIES_LEAVANNY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_AGILITY, MOVE_LEAF_BLADE, MOVE_LUNGE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LEAVANNY_GENERIC_2] = {
        .species = SPECIES_LEAVANNY,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_U_TURN, MOVE_LEAF_BLADE, MOVE_POISON_JAB, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCOLIPEDE_GENERIC_1] = {
        .species = SPECIES_SCOLIPEDE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_PROTECT, MOVE_POISON_JAB, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCOLIPEDE_GENERIC_2] = {
        .species = SPECIES_SCOLIPEDE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SPEED_BOOST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_ROCK_SLIDE, MOVE_MEGAHORN, MOVE_THROAT_CHOP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_WHIMSICOTT_GENERIC_1] = {
        .species = SPECIES_WHIMSICOTT,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 4, 0),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_MOONBLAST, MOVE_HELPING_HAND}
    },
    [CUSTOM_FRONTIER_MON_WHIMSICOTT_GENERIC_2] = {
        .species = SPECIES_WHIMSICOTT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 4, 0),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_TAILWIND, MOVE_ENCORE, MOVE_MOONBLAST}
    },
    [CUSTOM_FRONTIER_MON_LILLIGANT_GENERIC_1] = {
        .species = SPECIES_LILLIGANT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SUNNY_DAY, MOVE_SOLAR_BEAM, MOVE_ALLURING_VOICE, MOVE_WEATHER_BALL}
    },
    [CUSTOM_FRONTIER_MON_LILLIGANT_GENERIC_2] = {
        .species = SPECIES_LILLIGANT,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_CHLOROPHYLL,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_QUIVER_DANCE, MOVE_PROTECT, MOVE_GIGA_DRAIN, MOVE_ALLURING_VOICE}
    },
    [CUSTOM_FRONTIER_MON_LILLIGANT_HISUI_GENERIC_1] = {
        .species = SPECIES_LILLIGANT_HISUI,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_HUSTLE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_VICTORY_DANCE, MOVE_CLOSE_COMBAT, MOVE_TRIPLE_AXEL, MOVE_LEAF_BLADE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LILLIGANT_HISUI_GENERIC_2] = {
        .species = SPECIES_LILLIGANT_HISUI,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_HUSTLE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_VICTORY_DANCE, MOVE_POISON_JAB, MOVE_TRIPLE_AXEL, MOVE_CLOSE_COMBAT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BASCULIN_GENERIC_1] = {
        .species = SPECIES_BASCULIN_RED_STRIPED,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_WAVE_CRASH, MOVE_FLIP_TURN, MOVE_ZEN_HEADBUTT, MOVE_ICE_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BASCULIN_GENERIC_2] = {
        .species = SPECIES_BASCULIN_BLUE_STRIPED,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_WAVE_CRASH, MOVE_FLIP_TURN, MOVE_ICE_FANG, MOVE_SUPERPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KROOKODILE_GENERIC_1] = {
        .species = SPECIES_KROOKODILE,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_KNOCK_OFF, MOVE_GUNK_SHOT, MOVE_HIGH_HORSEPOWER, MOVE_FIRE_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KROOKODILE_GENERIC_2] = {
        .species = SPECIES_KROOKODILE,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SCALE_SHOT, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DARMANITAN_GENERIC_1] = {
        .species = SPECIES_DARMANITAN,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FLARE_BLITZ, MOVE_ROCK_SLIDE, MOVE_IRON_HEAD, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DARMANITAN_GENERIC_2] = {
        .species = SPECIES_DARMANITAN,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_U_TURN, MOVE_FLARE_BLITZ, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DARMANITAN_GALAR_GENERIC_1] = {
        .species = SPECIES_DARMANITAN_GALAR,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_GORILLA_TACTICS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ICICLE_CRASH, MOVE_U_TURN, MOVE_FLARE_BLITZ, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DARMANITAN_GALAR_GENERIC_2] = {
        .species = SPECIES_DARMANITAN_GALAR,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_GORILLA_TACTICS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ICICLE_CRASH, MOVE_ROCK_SLIDE, MOVE_FLARE_BLITZ, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MARACTUS_GENERIC_1] = {
        .species = SPECIES_MARACTUS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_HEATPROOF,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_SALSA_SHAKES, MOVE_HYPER_VOICE, MOVE_GIGA_DRAIN, MOVE_SPIKY_SHIELD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MARACTUS_GENERIC_2] = {
        .species = SPECIES_MARACTUS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STORM_DRAIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_SALSA_SHAKES, MOVE_SPIKY_SHIELD, MOVE_ENERGY_BALL, MOVE_HYPER_VOICE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRUSTLE_GENERIC_1] = {
        .species = SPECIES_CRUSTLE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_WEAK_ARMOR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SHELL_SMASH, MOVE_PROTECT, MOVE_ROCK_SLIDE, MOVE_X_SCISSOR},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRUSTLE_GENERIC_2] = {
        .species = SPECIES_CRUSTLE,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_X_SCISSOR, MOVE_KNOCK_OFF, MOVE_POISON_JAB},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCRAFTY_GENERIC_1] = {
        .species = SPECIES_SCRAFTY,
        .heldItem = ITEM_ROSELI_BERRY,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_PARTING_SHOT, MOVE_KNOCK_OFF, MOVE_LOW_KICK},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SCRAFTY_GENERIC_2] = {
        .species = SPECIES_SCRAFTY,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SIGILYPH_GENERIC_1] = {
        .species = SPECIES_SIGILYPH,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 252, 8, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_LIGHT_SCREEN, MOVE_REFLECT, MOVE_PSYSHOCK}
    },
    [CUSTOM_FRONTIER_MON_SIGILYPH_GENERIC_2] = {
        .species = SPECIES_SIGILYPH,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_WONDER_SKIN,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_COSMIC_POWER, MOVE_STORED_POWER, MOVE_ROOST, MOVE_TAILWIND}
    },
    [CUSTOM_FRONTIER_MON_COFAGRIGUS_GENERIC_1] = {
        .species = SPECIES_COFAGRIGUS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MUMMY,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_NASTY_PLOT, MOVE_FLASH_CANNON, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_COFAGRIGUS_GENERIC_2] = {
        .species = SPECIES_COFAGRIGUS,
        .heldItem = ITEM_MENTAL_HERB,
        .ability = ABILITY_MUMMY,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_CARRACOSTA_GENERIC_1] = {
        .species = SPECIES_CARRACOSTA,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_SOLID_ROCK,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_WIDE_GUARD, MOVE_ICY_WIND}
    },
    [CUSTOM_FRONTIER_MON_CARRACOSTA_GENERIC_2] = {
        .species = SPECIES_CARRACOSTA,
        .heldItem = ITEM_WHITE_HERB,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SHELL_SMASH, MOVE_LIQUIDATION, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARCHEOPS_GENERIC_1] = {
        .species = SPECIES_ARCHEOPS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_DEFEATIST,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_ROCK_SLIDE, MOVE_ENDEAVOR, MOVE_ALLY_SWITCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ARCHEOPS_GENERIC_2] = {
        .species = SPECIES_ARCHEOPS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DEFEATIST,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_ROCK_SLIDE, MOVE_DUAL_WINGBEAT, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GARBODOR_GENERIC_1] = {
        .species = SPECIES_GARBODOR,
        .heldItem = ITEM_ABILITY_SHIELD,
        .ability = ABILITY_CORROSIVE_ODOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_GUNK_SHOT, MOVE_DRAIN_PUNCH, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GARBODOR_GENERIC_2] = {
        .species = SPECIES_GARBODOR,
        .heldItem = ITEM_ABILITY_SHIELD,
        .ability = ABILITY_CORROSIVE_ODOR,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_AUTOTOMIZE, MOVE_GUNK_SHOT, MOVE_KNOCK_OFF, MOVE_STOMPING_TANTRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ZOROARK_GENERIC_1] = {
        .species = SPECIES_ZOROARK,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_ILLUSION,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_DARK_PULSE, MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER}
    },
    [CUSTOM_FRONTIER_MON_ZOROARK_GENERIC_2] = {
        .species = SPECIES_ZOROARK,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ILLUSION,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_DARK_PULSE, MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_NASTY_PLOT}
    },
    [CUSTOM_FRONTIER_MON_ZOROARK_HISUI_GENERIC_1] = {
        .species = SPECIES_ZOROARK_HISUI,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_ILLUSION,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_HYPER_VOICE, MOVE_SHADOW_BALL, MOVE_FLAMETHROWER}
    },
    [CUSTOM_FRONTIER_MON_ZOROARK_HISUI_GENERIC_2] = {
        .species = SPECIES_ZOROARK_HISUI,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_ILLUSION,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_HYPER_VOICE, MOVE_PSYCHIC},
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
    },
    [CUSTOM_FRONTIER_MON_CINCCINO_GENERIC_1] = {
        .species = SPECIES_CINCCINO,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_TECHNICIAN,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TAIL_SLAP, MOVE_BULLET_SEED, MOVE_TRIPLE_AXEL, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CINCCINO_GENERIC_2] = {
        .species = SPECIES_CINCCINO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SKILL_LINK,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_TIDY_UP, MOVE_TAIL_SLAP, MOVE_BULLET_SEED, MOVE_TRIPLE_AXEL},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOTHITELLE_GENERIC_1] = {
        .species = SPECIES_GOTHITELLE,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .moves = {MOVE_FAKE_OUT, MOVE_PSYSHOCK, MOVE_FOCUS_BLAST, MOVE_THUNDERBOLT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOTHITELLE_GENERIC_2] = {
        .species = SPECIES_GOTHITELLE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYSHOCK, MOVE_FOCUS_BLAST, MOVE_THUNDERBOLT}
    },
    [CUSTOM_FRONTIER_MON_REUNICLUS_GENERIC_1] = {
        .species = SPECIES_REUNICLUS,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_MAGIC_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYSHOCK, MOVE_FOCUS_BLAST, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_REUNICLUS_GENERIC_2] = {
        .species = SPECIES_REUNICLUS,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_FLASH_CANNON, MOVE_PSYSHOCK, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_SWANNA_GENERIC_1] = {
        .species = SPECIES_SWANNA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_STORMDANCER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HURRICANE, MOVE_WEATHER_BALL, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_SWANNA_GENERIC_2] = {
        .species = SPECIES_SWANNA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_STORMDANCER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HURRICANE, MOVE_WEATHER_BALL, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_VANILLUXE_GENERIC_1] = {
        .species = SPECIES_VANILLUXE,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SNOW_WARNING,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BLIZZARD, MOVE_FLASH_CANNON, MOVE_WEATHER_BALL, MOVE_ICY_WIND}
    },
    [CUSTOM_FRONTIER_MON_VANILLUXE_GENERIC_2] = {
        .species = SPECIES_VANILLUXE,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_SNOW_WARNING,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_AURORA_VEIL, MOVE_BLIZZARD, MOVE_WEATHER_BALL, MOVE_FREEZE_DRY},
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
    },
    [CUSTOM_FRONTIER_MON_SAWSBUCK_GENERIC_1] = {
        .species = SPECIES_SAWSBUCK,
        .heldItem = ITEM_CLEAR_AMULET,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HEADBUTT, MOVE_HORN_LEECH, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_SAWSBUCK_GENERIC_2] = {
        .species = SPECIES_SAWSBUCK,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HEADBUTT, MOVE_HORN_LEECH, MOVE_JUMP_KICK, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EMOLGA_GENERIC_1] = {
        .species = SPECIES_EMOLGA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 252, 0, 0),
        .nature = NATURE_TIMID,
        .moves = {MOVE_NUZZLE, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EMOLGA_GENERIC_2] = {
        .species = SPECIES_EMOLGA,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_STATIC,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_VOLT_SWITCH, MOVE_AIR_SLASH, MOVE_THUNDERBOLT, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_ESCAVALIER_GENERIC_1] = {
        .species = SPECIES_ESCAVALIER,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_PROTECT, MOVE_FIRST_IMPRESSION, MOVE_IRON_HEAD, MOVE_DRILL_RUN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ESCAVALIER_GENERIC_2] = {
        .species = SPECIES_ESCAVALIER,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_SWARM,
        .ev = TRAINER_PARTY_EVS(248, 252, 0, 0, 0, 8),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FIRST_IMPRESSION, MOVE_IRON_HEAD, MOVE_CLOSE_COMBAT, MOVE_DRILL_RUN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_AMOONGUSS_GENERIC_1] = {
        .species = SPECIES_AMOONGUSS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_EFFECT_SPORE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SLUDGE_BOMB, MOVE_RAGE_POWDER, MOVE_SPORE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_AMOONGUSS_GENERIC_2] = {
        .species = SPECIES_AMOONGUSS,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_EFFECT_SPORE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_SPORE, MOVE_POLLEN_PUFF, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_JELLICENT_GENERIC_1] = {
        .species = SPECIES_JELLICENT,
        .heldItem = ITEM_COLBUR_BERRY,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_DESTINY_BOND, MOVE_MUDDY_WATER, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_JELLICENT_GENERIC_2] = {
        .species = SPECIES_JELLICENT,
        .heldItem = ITEM_SITRUS_BERRY,
        .ability = ABILITY_GOOEY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_WILL_O_WISP, MOVE_STRENGTH_SAP, MOVE_SHADOW_BALL, MOVE_ICE_BEAM}
    },
    [CUSTOM_FRONTIER_MON_ALOMOMOLA_GENERIC_1] = {
        .species = SPECIES_ALOMOMOLA,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_KNOCK_OFF, MOVE_FLIP_TURN, MOVE_PLAY_ROUGH, MOVE_LIQUIDATION},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_ALOMOMOLA_GENERIC_2] = {
        .species = SPECIES_ALOMOMOLA,
        .heldItem = ITEM_ROCKY_HELMET,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .moves = {MOVE_WISH, MOVE_SCALD, MOVE_FLIP_TURN, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GALVANTULA_GENERIC_1] = {
        .species = SPECIES_GALVANTULA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_COMPOUND_EYES,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDER, MOVE_ELECTROWEB, MOVE_ENERGY_BALL, MOVE_BUG_BUZZ}
    },
    [CUSTOM_FRONTIER_MON_GALVANTULA_GENERIC_2] = {
        .species = SPECIES_GALVANTULA,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_COMPOUND_EYES,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ELECTROWEB, MOVE_VOLT_SWITCH, MOVE_BUG_BUZZ, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_FERROTHORN_GENERIC_1] = {
        .species = SPECIES_FERROTHORN,
        .heldItem = ITEM_OCCA_BERRY,
        .ability = ABILITY_IRON_BARBS,
        .ev = TRAINER_PARTY_EVS(248, 8, 0, 0, 0, 252),
        .nature = NATURE_SASSY,
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 0, 31, 31),
        .moves = {MOVE_BODY_PRESS, MOVE_GYRO_BALL, MOVE_IRON_DEFENSE, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_FERROTHORN_GENERIC_2] = {
        .species = SPECIES_FERROTHORN,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_IRON_BARBS,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TOXIC, MOVE_PROTECT, MOVE_LEECH_SEED, MOVE_BODY_PRESS}
    },
    [CUSTOM_FRONTIER_MON_KLINKLANG_GENERIC_1] = {
        .species = SPECIES_KLINKLANG,
        .heldItem = ITEM_BURGLARY_KIT,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHIFT_GEAR, MOVE_HIGH_HORSEPOWER, MOVE_WILD_CHARGE, MOVE_GEAR_GRIND},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KLINKLANG_GENERIC_2] = {
        .species = SPECIES_KLINKLANG,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_CLEAR_BODY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SHIFT_GEAR, MOVE_GEAR_GRIND, MOVE_HIGH_HORSEPOWER, MOVE_WILD_CHARGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EELEKTROSS_GENERIC_1] = {
        .species = SPECIES_EELEKTROSS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SUPERCELL_SLAM, MOVE_DRAGON_TAIL, MOVE_FIRE_PUNCH, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_EELEKTROSS_GENERIC_2] = {
        .species = SPECIES_EELEKTROSS,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_COIL, MOVE_SUPERCELL_SLAM, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BEHEEYEM_GENERIC_1] = {
        .species = SPECIES_BEHEEYEM,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_ANALYTIC,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_PSYSHOCK, MOVE_DARK_PULSE, MOVE_FLASH_CANNON, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_BEHEEYEM_GENERIC_2] = {
        .species = SPECIES_BEHEEYEM,
        .heldItem = ITEM_POWER_HERB,
        .ability = ABILITY_ANALYTIC,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 0, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_METEOR_BEAM, MOVE_PSYSHOCK, MOVE_FLASH_CANNON}
    },
    [CUSTOM_FRONTIER_MON_CHANDELURE_GENERIC_1] = {
        .species = SPECIES_CHANDELURE,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK, MOVE_SHADOW_BALL, MOVE_OVERHEAT, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_CHANDELURE_GENERIC_2] = {
        .species = SPECIES_CHANDELURE,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHADOW_TAG,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_PROTECT, MOVE_SHADOW_BALL, MOVE_OVERHEAT, MOVE_WILL_O_WISP}
    },
    [CUSTOM_FRONTIER_MON_HAXORUS_GENERIC_1] = {
        .species = SPECIES_HAXORUS,
        .heldItem = ITEM_LOADED_DICE,
        .ability = ABILITY_MOLD_BREAKER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SCALE_SHOT, MOVE_IRON_HEAD, MOVE_STOMPING_TANTRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HAXORUS_GENERIC_2] = {
        .species = SPECIES_HAXORUS,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_MOLD_BREAKER,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SCALE_SHOT, MOVE_IRON_HEAD, MOVE_CLOSE_COMBAT, MOVE_STOMPING_TANTRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BEARTIC_GENERIC_1] = {
        .species = SPECIES_BEARTIC,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_SLUSH_RUSH,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SNOWSCAPE, MOVE_ICICLE_CRASH, MOVE_CLOSE_COMBAT, MOVE_PLAY_ROUGH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BEARTIC_GENERIC_2] = {
        .species = SPECIES_BEARTIC,
        .heldItem = ITEM_EXPERT_BELT,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RAIN_DANCE, MOVE_LIQUIDATION, MOVE_ICICLE_CRASH, MOVE_PLAY_ROUGH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_CRYOGONAL_GENERIC_1] = {
        .species = SPECIES_CRYOGONAL,
        .heldItem = ITEM_CHOPLE_BERRY,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 252, 0, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_SHEER_COLD, MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_HAZE}
    },
    [CUSTOM_FRONTIER_MON_CRYOGONAL_GENERIC_2] = {
        .species = SPECIES_CRYOGONAL,
        .heldItem = ITEM_CHARTI_BERRY,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(248, 0, 8, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ACID_ARMOR, MOVE_RECOVER, MOVE_SHEER_COLD, MOVE_REFLECT}
    },
    [CUSTOM_FRONTIER_MON_ACCELGOR_GENERIC_1] = {
        .species = SPECIES_ACCELGOR,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_STICKY_HOLD,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BUG_BUZZ, MOVE_DARK_PULSE, MOVE_FOCUS_BLAST, MOVE_ENERGY_BALL}
    },
    [CUSTOM_FRONTIER_MON_ACCELGOR_GENERIC_2] = {
        .species = SPECIES_ACCELGOR,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_STICKY_HOLD,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 252, 0, 0),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_FINAL_GAMBIT}
    },
    [CUSTOM_FRONTIER_MON_STUNFISK_GENERIC_1] = {
        .species = SPECIES_STUNFISK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_THUNDERBOLT, MOVE_SLUDGE_BOMB, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_STUNFISK_GENERIC_2] = {
        .species = SPECIES_STUNFISK,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_SCALD}
    },
    [CUSTOM_FRONTIER_MON_STUNFISK_GALAR_GENERIC_1] = {
        .species = SPECIES_STUNFISK_GALAR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_MIMICRY,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_SNAP_TRAP, MOVE_STOMPING_TANTRUM, MOVE_ROCK_SLIDE, MOVE_CRUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_STUNFISK_GALAR_GENERIC_2] = {
        .species = SPECIES_STUNFISK_GALAR,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_MIMICRY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CAREFUL,
        .moves = {MOVE_FISSURE, MOVE_THUNDER_WAVE, MOVE_PROTECT, MOVE_SNAP_TRAP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MIENSHAO_GENERIC_1] = {
        .species = SPECIES_MIENSHAO,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_TRIPLE_AXEL, MOVE_KNOCK_OFF},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MIENSHAO_GENERIC_2] = {
        .species = SPECIES_MIENSHAO,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_INNER_FOCUS,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DRUDDIGON_GENERIC_1] = {
        .species = SPECIES_DRUDDIGON,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_ROUGH_SKIN,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_FAKE_OUT, MOVE_FOLLOW_ME, MOVE_ROCK_SLIDE, MOVE_DRACONIC_STRIKE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DRUDDIGON_GENERIC_2] = {
        .species = SPECIES_DRUDDIGON,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_ROUGH_SKIN,
        .ev = TRAINER_PARTY_EVS(252, 252, 4, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_DRACONIC_STRIKE, MOVE_DUAL_WINGBEAT, MOVE_FIRE_PUNCH, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOLURK_GENERIC_1] = {
        .species = SPECIES_GOLURK,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_NO_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_POLTERGEIST, MOVE_ICE_PUNCH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_GOLURK_GENERIC_2] = {
        .species = SPECIES_GOLURK,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_NO_GUARD,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_ROCK_SLIDE, MOVE_DYNAMIC_PUNCH, MOVE_HIGH_HORSEPOWER, MOVE_POLTERGEIST},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BOUFFALANT_GENERIC_1] = {
        .species = SPECIES_BOUFFALANT,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_RECKLESS,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HEAD_CHARGE, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BOUFFALANT_GENERIC_2] = {
        .species = SPECIES_BOUFFALANT,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_SAP_SIPPER,
        .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_HEAD_CHARGE, MOVE_HIGH_HORSEPOWER, MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BRAVIARY_GENERIC_1] = {
        .species = SPECIES_BRAVIARY,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_TAILWIND, MOVE_CLOSE_COMBAT, MOVE_GIGA_IMPACT, MOVE_BRAVE_BIRD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BRAVIARY_GENERIC_2] = {
        .species = SPECIES_BRAVIARY,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BODY_SLAM, MOVE_BRAVE_BIRD, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BRAVIARY_HISUI_GENERIC_1] = {
        .species = SPECIES_BRAVIARY_HISUI,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_KEEN_EYE,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 0, 252, 8),
        .nature = NATURE_QUIET,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYSHOCK, MOVE_HURRICANE, MOVE_HEAT_WAVE}
    },
    [CUSTOM_FRONTIER_MON_BRAVIARY_HISUI_GENERIC_2] = {
        .species = SPECIES_BRAVIARY_HISUI,
        .heldItem = ITEM_SCOPE_LENS,
        .ability = ABILITY_TINTED_LENS,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_ESPER_WING, MOVE_TAILWIND, MOVE_PROTECT, MOVE_AIR_SLASH}
    },
    [CUSTOM_FRONTIER_MON_MANDIBUZZ_GENERIC_1] = {
        .species = SPECIES_MANDIBUZZ,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(252, 0, 252, 0, 0, 4),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_TOXIC, MOVE_ROOST, MOVE_FOUL_PLAY}
    },
    [CUSTOM_FRONTIER_MON_MANDIBUZZ_GENERIC_2] = {
        .species = SPECIES_MANDIBUZZ,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_OVERCOAT,
        .ev = TRAINER_PARTY_EVS(248, 0, 252, 0, 0, 8),
        .nature = NATURE_BOLD,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_DOUBLE_TEAM, MOVE_ROOST, MOVE_FOUL_PLAY}
    },
    [CUSTOM_FRONTIER_MON_HEATMOR_GENERIC_1] = {
        .species = SPECIES_HEATMOR,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_WHITE_SMOKE,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 252, 0),
        .nature = NATURE_MODEST,
        .moves = {MOVE_FIRE_LASH, MOVE_FOCUS_BLAST, MOVE_GIGA_DRAIN, MOVE_SCORCHING_SANDS},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HEATMOR_GENERIC_2] = {
        .species = SPECIES_HEATMOR,
        .heldItem = ITEM_STARF_BERRY,
        .ability = ABILITY_GLUTTONY,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .moves = {MOVE_RECYCLE, MOVE_PROTECT, MOVE_FIRE_LASH, MOVE_WILL_O_WISP},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DURANT_GENERIC_1] = {
        .species = SPECIES_DURANT,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_HUSTLE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_IRON_HEAD, MOVE_ROCK_SLIDE, MOVE_X_SCISSOR, MOVE_SUPERPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_DURANT_GENERIC_2] = {
        .species = SPECIES_DURANT,
        .heldItem = ITEM_CHOICE_BAND,
        .ability = ABILITY_HUSTLE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_ROCK_SLIDE, MOVE_X_SCISSOR, MOVE_IRON_HEAD, MOVE_SUPERPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_HYDREIGON_GENERIC_1] = {
        .species = SPECIES_HYDREIGON,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_PROTECT, MOVE_SNARL, MOVE_DRACO_METEOR}
    },
    [CUSTOM_FRONTIER_MON_HYDREIGON_GENERIC_2] = {
        .species = SPECIES_HYDREIGON,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_LEVITATE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_DRACO_METEOR, MOVE_DARK_PULSE, MOVE_FIRE_BLAST, MOVE_EARTH_POWER}
    },
    [CUSTOM_FRONTIER_MON_VOLCARONA_GENERIC_1] = {
        .species = SPECIES_VOLCARONA,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(248, 0, 0, 52, 0, 208),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_RAGE_POWDER, MOVE_TAILWIND, MOVE_FIERY_DANCE, MOVE_WILL_O_WISP}
    },
    [CUSTOM_FRONTIER_MON_VOLCARONA_GENERIC_2] = {
        .species = SPECIES_VOLCARONA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_FLAME_BODY,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_QUIVER_DANCE, MOVE_FIERY_DANCE, MOVE_BUG_BUZZ, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_COBALION_GENERIC_1] = {
        .species = SPECIES_COBALION,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STAMINA,
        .ev = TRAINER_PARTY_EVS(252, 4, 0, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_THUNDER_WAVE, MOVE_IRON_HEAD},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_COBALION_GENERIC_2] = {
        .species = SPECIES_COBALION,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_STAMINA,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_AURA_SPHERE, MOVE_FLASH_CANNON, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_TERRAKION_GENERIC_1] = {
        .species = SPECIES_TERRAKION,
        .heldItem = ITEM_SALAC_BERRY,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_ROCK_SLIDE, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TERRAKION_GENERIC_2] = {
        .species = SPECIES_TERRAKION,
        .heldItem = ITEM_ASSAULT_VEST,
        .ability = ABILITY_STURDY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_ZEN_HEADBUTT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VIRIZION_GENERIC_1] = {
        .species = SPECIES_VIRIZION,
        .heldItem = ITEM_COBA_BERRY,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LEAF_BLADE, MOVE_CLOSE_COMBAT, MOVE_MEGAHORN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_VIRIZION_GENERIC_2] = {
        .species = SPECIES_VIRIZION,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SCRAPPY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_CLOSE_COMBAT, MOVE_LEAF_BLADE, MOVE_PROTECT},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_TORNADUS_GENERIC_1] = {
        .species = SPECIES_TORNADUS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HEAT_WAVE, MOVE_BLEAKWIND_STORM, MOVE_SLUDGE_BOMB}
    },
    [CUSTOM_FRONTIER_MON_TORNADUS_GENERIC_2] = {
        .species = SPECIES_TORNADUS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 4, 0, 0, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_FOUL_PLAY, MOVE_TAUNT, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_TORNADUS_THERIAN_GENERIC_1] = {
        .species = SPECIES_TORNADUS_THERIAN,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_TAILWIND, MOVE_HEAT_WAVE, MOVE_BLEAKWIND_STORM, MOVE_GRASS_KNOT}
    },
    [CUSTOM_FRONTIER_MON_TORNADUS_THERIAN_GENERIC_2] = {
        .species = SPECIES_TORNADUS_THERIAN,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_REGENERATOR,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_BLEAKWIND_STORM, MOVE_HEAT_WAVE, MOVE_GRASS_KNOT, MOVE_PSYCHIC}
    },
    [CUSTOM_FRONTIER_MON_THUNDURUS_GENERIC_1] = {
        .species = SPECIES_THUNDURUS,
        .heldItem = ITEM_COVERT_CLOAK,
        .ability = ABILITY_PRANKSTER,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 4, 252),
        .nature = NATURE_CALM,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDER_WAVE, MOVE_TAUNT, MOVE_WILDBOLT_STORM, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_THUNDURUS_GENERIC_2] = {
        .species = SPECIES_THUNDURUS,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_DEFIANT,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_WILDBOLT_STORM, MOVE_SLUDGE_BOMB, MOVE_DARK_PULSE, MOVE_U_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_THUNDURUS_THERIAN_GENERIC_1] = {
        .species = SPECIES_THUNDURUS_THERIAN,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_MODEST,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_VOLT_SWITCH, MOVE_THUNDERBOLT, MOVE_SLUDGE_BOMB, MOVE_DARK_PULSE}
    },
    [CUSTOM_FRONTIER_MON_THUNDURUS_THERIAN_GENERIC_2] = {
        .species = SPECIES_THUNDURUS_THERIAN,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_VOLT_ABSORB,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_SLUDGE_BOMB, MOVE_DARK_PULSE}
    },
    [CUSTOM_FRONTIER_MON_LANDORUS_GENERIC_1] = {
        .species = SPECIES_LANDORUS,
        .heldItem = ITEM_LIFE_ORB,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_NASTY_PLOT, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_LANDORUS_GENERIC_2] = {
        .species = SPECIES_LANDORUS,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SHEER_FORCE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_NASTY_PLOT, MOVE_SLUDGE_BOMB, MOVE_EARTH_POWER, MOVE_EXTRASENSORY}
    },
    [CUSTOM_FRONTIER_MON_LANDORUS_THERIAN_GENERIC_1] = {
        .species = SPECIES_LANDORUS_THERIAN,
        .heldItem = ITEM_YACHE_BERRY,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_BULK_UP, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_STOMPING_TANTRUM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_LANDORUS_THERIAN_GENERIC_2] = {
        .species = SPECIES_LANDORUS_THERIAN,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_INTIMIDATE,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_U_TURN, MOVE_STOMPING_TANTRUM, MOVE_ROCK_SLIDE, MOVE_STONE_EDGE},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_KELDEO_GENERIC_1] = {
        .species = SPECIES_KELDEO,
        .heldItem = ITEM_LEFTOVERS,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_CALM_MIND, MOVE_HYDRO_PUMP, MOVE_SECRET_SWORD, MOVE_PROTECT}
    },
    [CUSTOM_FRONTIER_MON_KELDEO_GENERIC_2] = {
        .species = SPECIES_KELDEO,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_COMPETITIVE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_SECRET_SWORD, MOVE_FLIP_TURN},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MELOETTA_GENERIC_1] = {
        .species = SPECIES_MELOETTA,
        .heldItem = ITEM_FOCUS_SASH,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 252, 4, 252, 0, 0),
        .nature = NATURE_JOLLY,
        .moves = {MOVE_RELIC_SONG, MOVE_CLOSE_COMBAT, MOVE_TRIPLE_AXEL, MOVE_PLAY_ROUGH},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_MELOETTA_GENERIC_2] = {
        .species = SPECIES_MELOETTA,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SERENE_GRACE,
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
        .nature = NATURE_TIMID,
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
        .moves = {MOVE_HYPER_VOICE, MOVE_PSYSHOCK, MOVE_ALLURING_VOICE, MOVE_SHADOW_BALL}
    },
    [CUSTOM_FRONTIER_MON_BASCULEGION_GENERIC_1] = {
        .species = SPECIES_BASCULEGION,
        .heldItem = ITEM_CHOICE_SCARF,
        .ability = ABILITY_ADAPTABILITY,
        .ev = TRAINER_PARTY_EVS(0, 252, 0, 252, 0, 4),
        .nature = NATURE_ADAMANT,
        .moves = {MOVE_WAVE_CRASH, MOVE_PHANTOM_FORCE, MOVE_FLIP_TURN, MOVE_ICE_FANG},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
    [CUSTOM_FRONTIER_MON_BASCULEGION_GENERIC_2] = {
        .species = SPECIES_BASCULEGION_F,
        .heldItem = ITEM_CHOICE_SPECS,
        .ability = ABILITY_SWIFT_SWIM,
        .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
        .nature = NATURE_MODEST,
        .moves = {MOVE_SHADOW_BALL, MOVE_HYDRO_PUMP, MOVE_FLIP_TURN, MOVE_ICE_BEAM},
        .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31)
    },
};