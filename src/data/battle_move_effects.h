#include "battle.h"
#include "battle_scripts.h"
#include "constants/battle_move_effects.h"

const struct BattleMoveEffect gBattleMoveEffects[NUM_BATTLE_MOVE_EFFECTS] =
{
    [EFFECT_PLACEHOLDER] =
    {
    },

    [EFFECT_HIT] =
    {
    },

    [EFFECT_STAT_CHANGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_DEFENSE_CURL] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MINIMIZE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_AUTOTOMIZE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_GEOMANCY] =
    {
        .encourageEncore = TRUE,
        .twoTurnEffect = TRUE,
    },

    [EFFECT_NON_VOLATILE_STATUS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TERRAIN_BOOST] =
    {
    },

    [EFFECT_DREAM_EATER] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MIRROR_MOVE] =
    {
    },

    [EFFECT_HAZE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_BIDE] =
    {
    },

    [EFFECT_ROAR] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_CONVERSION] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_RESTORE_HP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LIGHT_SCREEN] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_REST] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_OHKO] =
    {
    },

    [EFFECT_FUSION_COMBO] =
    {
    },

    [EFFECT_FIXED_PERCENT_DAMAGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FIXED_HP_DAMAGE] =
    {
    },

    [EFFECT_HEAL_BLOCK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_RECOIL_IF_MISS] =
    {
    },

    [EFFECT_MIST] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FOCUS_ENERGY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_CONFUSE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TRANSFORM] =
    {
    },

    [EFFECT_REFLECT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TWO_TURNS_ATTACK] =
    {
        .twoTurnEffect = TRUE,
    },

    [EFFECT_SUBSTITUTE] =
    {
    },

    [EFFECT_MIMIC] =
    {
    },

    [EFFECT_METRONOME] =
    {
    },

    [EFFECT_LEECH_SEED] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_DO_NOTHING] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HOLD_HANDS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_CELEBRATE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HAPPY_HOUR] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_DISABLE] =
    {
    },

    [EFFECT_LEVEL_DAMAGE] =
    {
    },

    [EFFECT_PSYWAVE] =
    {
    },

    [EFFECT_REFLECT_DAMAGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_ENCORE] =
    {
    },

    [EFFECT_PAIN_SPLIT] =
    {
    },

    [EFFECT_SNORE] =
    {
    },

    [EFFECT_CONVERSION_2] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LOCK_ON] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SKETCH] =
    {
    },

    [EFFECT_SLEEP_TALK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_DESTINY_BOND] =
    {
    },

    [EFFECT_FLAIL] =
    {
    },

    [EFFECT_SPITE] =
    {
    },

    [EFFECT_FALSE_SWIPE] =
    {
    },

    [EFFECT_HEAL_BELL] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TRIPLE_KICK] =
    {
    },

    [EFFECT_MEAN_LOOK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_NIGHTMARE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_CURSE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HEALING_WISH] =
    {
    },

    [EFFECT_LUNAR_DANCE] =
    {
    },

    [EFFECT_PROTECT] =
    {
        .encourageEncore = TRUE,
        .usesProtectCounter = TRUE,
    },

    [EFFECT_SPIKES] =
    {
    },

    [EFFECT_FORESIGHT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_PERISH_SONG] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_ENDURE] =
    {
        .encourageEncore = TRUE,
        .usesProtectCounter = TRUE,
    },

    [EFFECT_ROLLOUT] =
    {
    },

    [EFFECT_SWAGGER] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FURY_CUTTER] =
    {
    },

    [EFFECT_ATTRACT] =
    {
        .encourageEncore = TRUE,
    },


    [EFFECT_PRESENT] =
    {
    },


    [EFFECT_SAFEGUARD] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MAGNITUDE] =
    {
    },

    [EFFECT_BATON_PASS] =
    {
    },

    [EFFECT_PURSUIT] =
    {
    },

    [EFFECT_CAPTIVATE] =
    {
    },

    [EFFECT_MORNING_SUN] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SYNTHESIS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MOONLIGHT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HIDDEN_POWER] =
    {
    },

    [EFFECT_WEATHER] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FELL_STINGER] =
    {
    },

    [EFFECT_STAT_CHANGE_HALF_HP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_BELLY_DRUM] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_PSYCH_UP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_EARTHQUAKE] =
    {
    },

    [EFFECT_FUTURE_SIGHT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SOLAR_BEAM] =
    {
        .twoTurnEffect = TRUE,
    },

    [EFFECT_TELEPORT] =
    {
    },

    [EFFECT_BEAT_UP] =
    {
    },

    [EFFECT_SEMI_INVULNERABLE] =
    {
        .twoTurnEffect = TRUE,
        .semiInvulnerableEffect = TRUE,
    },

    [EFFECT_FIRST_TURN_ONLY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_STOCKPILE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SPIT_UP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SWALLOW] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_OVERWRITE_ABILITY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TORMENT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MEMENTO] =
    {
    },

    [EFFECT_FACADE] =
    {
    },

    [EFFECT_FOCUS_PUNCH] =
    {
    },

    [EFFECT_DOUBLE_POWER_ON_ARG_STATUS] =
    {
    },

    [EFFECT_FOLLOW_ME] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_NATURE_POWER] =
    {
    },

    [EFFECT_CHARGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TAUNT] =
    {
    },

    [EFFECT_HELPING_HAND] =
    {
    },

    [EFFECT_TRICK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_ROLE_PLAY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_WISH] =
    {
    },

    [EFFECT_ASSIST] =
    {
    },

    [EFFECT_INGRAIN] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MAGIC_COAT] =
    {
    },

    [EFFECT_RECYCLE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_REVENGE] =
    {
    },

    [EFFECT_YAWN] =
    {
    },

    [EFFECT_KNOCK_OFF] =
    {
    },

    [EFFECT_STEAL_ITEM] =
    {
    },

    [EFFECT_ENDEAVOR] =
    {
    },

    [EFFECT_POWER_BASED_ON_USER_HP] =
    {
    },

    [EFFECT_SKILL_SWAP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_IMPRISON] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_REFRESH] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_GRUDGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SNATCH] =
    {
    },

    [EFFECT_LOW_KICK] =
    {
    },

    [EFFECT_HIT_ESCAPE] =
    {
    },

    [EFFECT_MUD_SPORT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_WEATHER_BALL] =
    {
    },

    [EFFECT_WATER_SPORT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_CAMOUFLAGE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_PLEDGE] =
    {
    },

    [EFFECT_FLING] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_NATURAL_GIFT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_POWER_BASED_ON_TARGET_HP] =
    {
    },

    [EFFECT_ASSURANCE] =
    {
    },

    [EFFECT_TRUMP_CARD] =
    {
    },

    [EFFECT_ACROBATICS] =
    {
    },

    [EFFECT_HEAT_CRASH] =
    {
    },

    [EFFECT_PUNISHMENT] =
    {
    },

    [EFFECT_STORED_POWER] =
    {
    },

    [EFFECT_ELECTRO_BALL] =
    {
    },

    [EFFECT_GYRO_BALL] =
    {
    },

    [EFFECT_ECHOED_VOICE] =
    {
    },

    [EFFECT_PAYBACK] =
    {
    },

    [EFFECT_ROUND] =
    {
    },

    [EFFECT_BRINE] =
    {
    },

    [EFFECT_RETALIATE] =
    {
    },

    [EFFECT_FOUL_PLAY] =
    {
    },

    [EFFECT_PSYSHOCK] =
    {
    },

    [EFFECT_ROOST] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_GRAVITY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_MIRACLE_EYE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TAILWIND] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_EMBARGO] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_AQUA_RING] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TRICK_ROOM] =
    {
    },

    [EFFECT_WONDER_ROOM] =
    {
    },

    [EFFECT_MAGIC_ROOM] =
    {
    },

    [EFFECT_MAGNET_RISE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TOXIC_SPIKES] =
    {
    },

    [EFFECT_GASTRO_ACID] =
    {
    },

    [EFFECT_STEALTH_ROCK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TELEKINESIS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_POWER_SWAP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_GUARD_SWAP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HEART_SWAP] =
    {
    },

    [EFFECT_POWER_SPLIT] =
    {
    },

    [EFFECT_GUARD_SPLIT] =
    {
    },

    [EFFECT_STICKY_WEB] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LUCKY_CHANT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SUCKER_PUNCH] =
    {
    },

    [EFFECT_ENTRAINMENT] =
    {
    },

    [EFFECT_HEAL_PULSE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_QUASH] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_ION_DELUGE] =
    {
    },

    [EFFECT_SUPER_EFFECTIVE_ON_ARG] =
    {
    },

    [EFFECT_TOPSY_TURVY] =
    {
    },

    [EFFECT_TERRAIN] =
    {
    },

    [EFFECT_TWO_TYPED_MOVE] =
    {
    },

    [EFFECT_ME_FIRST] =
    {
    },

    [EFFECT_ELECTRIFY] =
    {
    },

    [EFFECT_REFLECT_TYPE] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_OVERWRITE_TYPE] =
    {
    },

    [EFFECT_GROWTH] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LAST_RESORT] =
    {
    },

    [EFFECT_STAT_CHANGE_ON_STATUS] =
    {
    },

    [EFFECT_TOXIC_THREAD] =
    {
    },

    [EFFECT_HIT_SWITCH_TARGET] =
    {
    },

    [EFFECT_FINAL_GAMBIT] =
    {
    },

    [EFFECT_CHANGE_TYPE_ON_ITEM] =
    {
    },

    [EFFECT_COPYCAT] =
    {
    },

    [EFFECT_DEFOG] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_HIT_ENEMY_HEAL_ALLY] =
    {
    },

    [EFFECT_SYNCHRONOISE] =
    {
    },

    [EFFECT_PSYCHO_SHIFT] =
    {
    },

    [EFFECT_POWER_TRICK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_AFTER_YOU] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_BESTOW] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_ROTOTILLER] =
    {
    },

    [EFFECT_FLOWER_SHIELD] =
    {
    },

    [EFFECT_SPEED_SWAP] =
    {
    },

    [EFFECT_REVELATION_DANCE] =
    {
    },

    [EFFECT_AURORA_VEIL] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_THIRD_TYPE] =
    {
    },

    [EFFECT_ACUPRESSURE] =
    {
    },

    [EFFECT_POWDER] =
    {
    },

    [EFFECT_BELCH] =
    {
    },

    [EFFECT_PARTING_SHOT] =
    {
    },

    [EFFECT_MAT_BLOCK] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_STOMPING_TANTRUM] =
    {
    },

    [EFFECT_INSTRUCT] =
    {
    },

    [EFFECT_LASER_FOCUS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_STAT_CHANGE_MAGNETIC] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_STRENGTH_SAP] =
    {
    },

    [EFFECT_PURIFY] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FAIL_IF_NOT_ARG_TYPE] =
    {
    },

    [EFFECT_SHORE_UP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_FAIRY_LOCK] =
    {
    },

    [EFFECT_ALLY_SWITCH] =
    {
    },

    [EFFECT_BODY_PRESS] =
    {
    },

    [EFFECT_JUNGLE_HEALING] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LASH_OUT] =
    {
    },

    [EFFECT_GRASSY_GLIDE] =
    {
    },

    [EFFECT_SNIPE_SHOT] =
    {
    },

    [EFFECT_STRUGGLE] =
    {
    },

    [EFFECT_STUFF_CHEEKS] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_GRAV_APPLE] =
    {
    },

    [EFFECT_HYPERSPACE_FURY] =
    {
    },

    [EFFECT_AURA_WHEEL] =
    {
    },

    [EFFECT_PHOTON_GEYSER] =
    {
    },

    [EFFECT_TERRAIN_PULSE] =
    {
    },

    [EFFECT_NO_RETREAT] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TAR_SHOT] =
    {
    },

    [EFFECT_POLTERGEIST] =
    {
    },

    [EFFECT_OCTOLOCK] =
    {
    },

    [EFFECT_CLANGOROUS_SOUL] =
    {
    },

    [EFFECT_BOLT_BEAK] =
    {
    },

    [EFFECT_SKY_DROP] =
    {
        .twoTurnEffect = TRUE,
        .semiInvulnerableEffect = TRUE,
    },

    [EFFECT_BEAK_BLAST] =
    {
    },

    [EFFECT_COURT_CHANGE] =
    {
    },

    [EFFECT_MAX_HP_50_RECOIL] =
    {
    },

    [EFFECT_CHLOROBLAST] =
    {
    },


    [EFFECT_DARK_VOID] =
    {
    },

    [EFFECT_TEATIME] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_SHELL_TRAP] =
    {
    },

    [EFFECT_HYDRO_STEAM] =
    {
    },

    [EFFECT_REVIVAL_BLESSING] =
    {
    },

    [EFFECT_TAKE_HEART] =
    {
    },

    [EFFECT_COLLISION_COURSE] =
    {
    },

    [EFFECT_CORROSIVE_GAS] =
    {
    },

    [EFFECT_POPULATION_BOMB] =
    {
    },

    [EFFECT_WEATHER_AND_SWITCH] =
    {
    },

    [EFFECT_RAGING_BULL] =
    {
    },

    [EFFECT_RAGE_FIST] =
    {
    },

    [EFFECT_DOODLE] =
    {
    },

    [EFFECT_IVY_CUDGEL] =
    {
    },

    [EFFECT_FICKLE_BEAM] =
    {
    },

    [EFFECT_SHED_TAIL] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_UPPER_HAND] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_DRAGON_CHEER] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_LAST_RESPECTS] =
    {
    },

    [EFFECT_TIDY_UP] =
    {
        .encourageEncore = TRUE,
    },

    [EFFECT_TERA_BLAST] =
    {
    },

    [EFFECT_TERA_STARSTORM] =
    {
    },

    [EFFECT_SHELL_SIDE_ARM] =
    {
    },

    [EFFECT_RAPID_SPIN] =
    {
    },

    [EFFECT_RECOIL] =
    {
    },

    [EFFECT_SMACK_DOWN] =
    {
    },

    [EFFECT_LIFE_DEW] =
    {
    },

    [EFFECT_ICE_SPINNER] =
    {
    },

    [EFFECT_STEEL_ROLLER] =
    {
    },

    [EFFECT_STONE_AXE] =
    {
    },

    [EFFECT_CEASELESS_EDGE] =
    {
    },

    [EFFECT_SPECIES_POWER_OVERRIDE] =
    {
    },

    [EFFECT_SCALE_SHOT] =
    {
    },

    [EFFECT_DAZZLING_DASH] = {},
    [EFFECT_HAUNTING_SHRIEKS] = {},
    [EFFECT_STORM_SACRIFICE] = { .encourageEncore = TRUE },
    [EFFECT_SUNBLOOM] = {},
    [EFFECT_DARK_IMITATION] = {},
    [EFFECT_MUD_BATH] = { .encourageEncore = TRUE },

    [EFFECT_SECRET_POWER] =
    {
    },
};
