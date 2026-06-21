// Auto-generated from src/data/bingo_mons.bingomons. Do not modify.

const struct TrainerMon gBingoMons[BINGO_MON_COUNT] =
{
#line 1 "src/data/bingo_mons.bingomons"
    [BINGO_MON_CALYREX_SHADOW_1] =
    {
#line 2
        .species = SPECIES_CALYREX_SHADOW,
        .gender = TRAINER_MON_RANDOM_GENDER,
        .heldItem = ITEM_LIFE_ORB,
#line 5
        .ev = TRAINER_PARTY_EVS(0, 0, 0, 252, 252, 4),
#line 7
        .iv = TRAINER_PARTY_IVS(31, 0, 31, 31, 31, 31),
#line 3
        .ability = ABILITY_AS_ONE_SHADOW_RIDER,
#line 9
        .lvl = 100,
        .ball = POKEBALL_COUNT,
#line 6
        .nature = NATURE_TIMID,
        .dynamaxLevel = MAX_DYNAMAX_LEVEL,
#line 4
        .teraType = TYPE_FAIRY,
#line 8
        .tags = MON_POOL_TAG_LEAD | MON_POOL_TAG_TERA,
        .moves = {
#line 9
            MOVE_NASTY_PLOT,
            MOVE_ASTRAL_BARRAGE,
            MOVE_PSYCHIC,
            MOVE_PROTECT,
        },
    },
};
