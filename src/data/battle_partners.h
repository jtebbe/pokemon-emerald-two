//
// DO NOT MODIFY THIS FILE! It is auto-generated from src/data/battle_partners.party
//
// If you want to modify this file set COMPETITIVE_PARTY_SYNTAX to FALSE
// in include/config/general.h and remove this notice.
// Use sed -i '/^#line/d' 'src/data/battle_partners.h' to remove #line markers.
//

#line 1 "src/data/battle_partners.party"

#line 1
    [DIFFICULTY_NORMAL][PARTNER_NONE] =
    {
#line 3
        .trainerClass = TRAINER_CLASS_PKMN_TRAINER_1,
#line 4
        .trainerPic = TRAINER_BACK_PIC_BRENDAN,
        .encounterMusic_gender =
#line 6
            TRAINER_ENCOUNTER_MUSIC_MALE,
        .partySize = 0,
        .party = (const struct TrainerMon[])
        {
        },
    },
#line 8
    [DIFFICULTY_NORMAL][PARTNER_STEVEN] =
    {
#line 9
        .trainerName = _("STEVEN"),
#line 10
        .trainerClass = TRAINER_CLASS_RIVAL,
#line 11
        .trainerPic = TRAINER_BACK_PIC_STEVEN,
        .encounterMusic_gender =
#line 13
            TRAINER_ENCOUNTER_MUSIC_MALE,
        .partySize = 3,
        .party = (const struct TrainerMon[])
        {
            {
#line 15
            .species = SPECIES_METAGROSS,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 15
            .heldItem = ITEM_ASSAULT_VEST,
#line 19
            .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
#line 21
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 16
            .ability = ABILITY_CLEAR_BODY,
#line 17
            .lvl = 70,
#line 20
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 18
            .teraType = TYPE_WATER,
            .moves = {
#line 21
                MOVE_HEAVY_SLAM,
                MOVE_PSYCHIC_FANGS,
                MOVE_HAMMER_ARM,
                MOVE_KNOCK_OFF,
            },
            },
            {
#line 26
            .species = SPECIES_ARCHALUDON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 26
            .heldItem = ITEM_POWER_HERB,
#line 29
            .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
#line 31
            .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
#line 27
            .ability = ABILITY_STAMINA,
#line 28
            .lvl = 70,
#line 30
            .nature = NATURE_MODEST,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 32
                MOVE_ELECTRO_SHOT,
                MOVE_PROTECT,
                MOVE_DRACO_METEOR,
                MOVE_BODY_PRESS,
            },
            },
            {
#line 37
            .species = SPECIES_AGGRON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 37
            .heldItem = ITEM_AIR_BALLOON,
#line 40
            .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
#line 42
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 38
            .ability = ABILITY_STURDY,
#line 39
            .lvl = 70,
#line 41
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 42
                MOVE_HEAVY_SLAM,
                MOVE_STONE_EDGE,
                MOVE_PROTECT,
                MOVE_ICE_PUNCH,
            },
            },
        },
    },
