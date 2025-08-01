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
#line 18
            .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
#line 20
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 16
            .ability = ABILITY_CLEAR_BODY,
#line 20
            .lvl = 100,
#line 19
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 17
            .teraType = TYPE_WATER,
            .moves = {
#line 20
                MOVE_HEAVY_SLAM,
                MOVE_PSYCHIC_FANGS,
                MOVE_HAMMER_ARM,
                MOVE_KNOCK_OFF,
            },
            },
            {
#line 25
            .species = SPECIES_ARCHALUDON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 25
            .heldItem = ITEM_POWER_HERB,
#line 27
            .ev = TRAINER_PARTY_EVS(252, 0, 0, 0, 252, 4),
#line 29
            .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
#line 26
            .ability = ABILITY_STAMINA,
#line 30
            .lvl = 100,
#line 28
            .nature = NATURE_MODEST,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 30
                MOVE_ELECTRO_SHOT,
                MOVE_PROTECT,
                MOVE_DRACO_METEOR,
                MOVE_BODY_PRESS,
            },
            },
            {
#line 35
            .species = SPECIES_AGGRON,
            .gender = TRAINER_MON_RANDOM_GENDER,
#line 35
            .heldItem = ITEM_AIR_BALLOON,
#line 37
            .ev = TRAINER_PARTY_EVS(252, 252, 0, 0, 0, 4),
#line 39
            .iv = TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31),
#line 36
            .ability = ABILITY_STURDY,
#line 39
            .lvl = 100,
#line 38
            .nature = NATURE_ADAMANT,
            .dynamaxLevel = MAX_DYNAMAX_LEVEL,
            .moves = {
#line 39
                MOVE_HEAVY_SLAM,
                MOVE_STONE_EDGE,
                MOVE_PROTECT,
                MOVE_ICE_PUNCH,
            },
            },
        },
    },
