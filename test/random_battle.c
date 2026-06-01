#include "global.h"
#include "battle.h"
#include "data.h"
#include "item.h"
#include "pokemon.h"
#include "random_battle.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/pokeball.h"
#include "constants/species.h"
#include "constants/trainers.h"

static const u8 sTestTrainerName[] = _("RANDY");

static struct RandomBattleOptions RandomBattleTestOptions(u8 numPlayerMons, u8 numOpponentMons)
{
    return (struct RandomBattleOptions)
    {
        .numPlayerMons = numPlayerMons,
        .numOpponentMons = numOpponentMons,
        .trainerPic = TRAINER_PIC_HIKER,
        .itemsOff = TRUE,
        .teraTypesOff = TRUE,
        .zCrystalsOff = TRUE,
        .megaStonesOff = TRUE,
    };
}

static void RandomBattleTestMakePlayerParty(enum Species species)
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, 50, 0, OTID_STRUCT_PRESET(0));
    CalculatePlayerPartyCount();
}

static u8 RandomBattleTestCountPlayerSpecies(enum Species species)
{
    u8 count = 0;

    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == species)
            count++;
    }

    return count;
}

static bool32 RandomBattleTestPlayerHasSpecies(enum Species species)
{
    return RandomBattleTestCountPlayerSpecies(species) != 0;
}

static bool32 RandomBattleTestMoveIsPreselected(enum Move move)
{
    static const enum Move sPreselectedMoves[] =
    {
        MOVE_PROTECT,
        MOVE_SWORDS_DANCE,
        MOVE_NASTY_PLOT,
        MOVE_CALM_MIND,
        MOVE_DRAGON_DANCE,
        MOVE_STEALTH_ROCK,
        MOVE_THUNDER_WAVE,
        MOVE_WILL_O_WISP,
        MOVE_RECOVER,
        MOVE_ROOST,
        MOVE_EARTHQUAKE,
        MOVE_FLAMETHROWER,
        MOVE_SURF,
        MOVE_THUNDERBOLT,
        MOVE_ICE_BEAM,
        MOVE_PSYCHIC,
        MOVE_SHADOW_BALL,
        MOVE_SLUDGE_BOMB,
        MOVE_DARK_PULSE,
        MOVE_DRAGON_PULSE,
        MOVE_MOONBLAST,
        MOVE_FLASH_CANNON,
        MOVE_STONE_EDGE,
        MOVE_CLOSE_COMBAT,
        MOVE_ENERGY_BALL,
    };

    for (u32 i = 0; i < ARRAY_COUNT(sPreselectedMoves); i++)
    {
        if (sPreselectedMoves[i] == move)
            return TRUE;
    }

    return FALSE;
}

static bool32 RandomBattleTestItemIsPreselected(enum Item item)
{
    static const enum Item sPreselectedItems[] =
    {
        ITEM_CHOICE_BAND,
        ITEM_CHOICE_SCARF,
        ITEM_CHOICE_SPECS,
        ITEM_LEFTOVERS,
        ITEM_LIFE_ORB,
        ITEM_SITRUS_BERRY,
        ITEM_ASSAULT_VEST,
        ITEM_FOCUS_SASH,
        ITEM_EXPERT_BELT,
        ITEM_LUM_BERRY,
        ITEM_ROCKY_HELMET,
        ITEM_BLACK_SLUDGE,
    };

    for (u32 i = 0; i < ARRAY_COUNT(sPreselectedItems); i++)
    {
        if (sPreselectedItems[i] == item)
            return TRUE;
    }

    return FALSE;
}

static u8 RandomBattleTestCountPlayerTypes(void)
{
    bool8 seenTypes[NUMBER_OF_MON_TYPES] = {0};
    u8 count = 0;

    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES);

        for (u32 j = 0; j < ARRAY_COUNT(gSpeciesInfo[species].types); j++)
        {
            enum Type type = gSpeciesInfo[species].types[j];

            if (type < NUMBER_OF_MON_TYPES && type != TYPE_NONE && !seenTypes[type])
            {
                seenTypes[type] = TRUE;
                count++;
            }
        }
    }

    return count;
}

TEST("True random battle rejects invalid party counts")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(0, 1);

    EXPECT(!RandomBattle_GenerateTrueBattle(&options));

    options = RandomBattleTestOptions(1, 7);
    EXPECT(!RandomBattle_GenerateTrueBattle(&options));
}

TEST("True random battle replaces and restores the player's party")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(2, 1);

    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 2);

    RandomBattle_RestorePlayerParty();
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
}

TEST("True random battle creates max-IV no-EV neutral-nature Pokemon")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);

    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IVS), TRAINER_PARTY_IVS(31, 31, 31, 31, 31, 31));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 0);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_HARDY);

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle honors items_off")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(3, 1);

    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM), ITEM_NONE);

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle supports pre-evolutions")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    enum Species species;

    options.preEvos = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    species = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES);
    EXPECT(!RandomBattle_IsFullyEvolvedSpecies(species));

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle can use preselected moves")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    const struct TrainerMon *party;

    options.preselectedMoves = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    party = RandomBattle_GetTrainerStruct()->party;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        EXPECT(RandomBattleTestMoveIsPreselected(party[0].moves[i]));

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle can use preselected items with item clause")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(3, 1);
    enum Item items[PARTY_SIZE] = {0};

    options.itemsOff = FALSE;
    options.itemsPreselected = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        enum Item item = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM);

        EXPECT(RandomBattleTestItemIsPreselected(item));
        for (u32 j = 0; j < i; j++)
            EXPECT_NE(item, items[j]);
        items[i] = item;
    }

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle can assign z-crystals")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    enum Item item;

    options.itemsOff = FALSE;
    options.itemsPreselected = TRUE;
    options.zCrystalsOff = FALSE;
    options.megaStonesOff = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    item = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM);
    EXPECT_EQ(GetItemHoldEffect(item), HOLD_EFFECT_Z_CRYSTAL);

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle can assign mega stones")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    enum Item item;

    options.itemsOff = FALSE;
    options.zCrystalsOff = TRUE;
    options.megaStonesOff = FALSE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    item = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM);
    EXPECT_EQ(GetItemHoldEffect(item), HOLD_EFFECT_MEGA_STONE);

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle can disable tera types")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    const struct TrainerMon *party;
    enum Type teraType;

    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    party = RandomBattle_GetTrainerStruct()->party;
    teraType = party[0].teraType;
    EXPECT_EQ(teraType, TYPE_NONE);

    RandomBattle_RestorePlayerParty();
}

TEST("True random battle generates tera types by default")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    const struct TrainerMon *party;
    enum Type teraType;

    options.teraTypesOff = FALSE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GenerateTrueBattle(&options));
    party = RandomBattle_GetTrainerStruct()->party;
    teraType = party[0].teraType;
    EXPECT_NE(teraType, TYPE_NONE);

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle rejects invalid party counts")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(7, 1);

    EXPECT(!RandomBattle_GeneratePoolBattle(&options));
}

TEST("Pool random battle honors no_legendaries")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(4, 1);

    options.preEvos = TRUE;
    options.noSpeciesClause = TRUE;
    options.noItemClause = TRUE;
    options.noTypeDiversity = TRUE;
    options.noLegendaries = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    EXPECT(!RandomBattleTestPlayerHasSpecies(SPECIES_MEW));

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle honors species clause")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(3, 1);

    options.preEvos = TRUE;
    options.noItemClause = TRUE;
    options.noTypeDiversity = TRUE;
    options.noLegendaries = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    EXPECT_EQ(RandomBattleTestCountPlayerSpecies(SPECIES_CHARIZARD), 1);

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle can disable species clause")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(2, 1);

    options.preEvos = TRUE;
    options.noSpeciesClause = TRUE;
    options.noItemClause = TRUE;
    options.noTypeDiversity = TRUE;
    options.noLegendaries = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    EXPECT_EQ(RandomBattleTestCountPlayerSpecies(SPECIES_CHARIZARD), 2);

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle honors item clause")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(3, 1);
    enum Item items[PARTY_SIZE] = {0};

    options.preEvos = TRUE;
    options.noSpeciesClause = TRUE;
    options.noTypeDiversity = TRUE;
    options.noLegendaries = TRUE;
    options.itemsOff = FALSE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    for (u32 i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
    {
        enum Item item = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM);

        for (u32 j = 0; j < i; j++)
            EXPECT_NE(item, items[j]);
        items[i] = item;
    }

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle can include pre-evolutions")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(6, 1);

    options.preEvos = TRUE;
    options.noSpeciesClause = TRUE;
    options.noItemClause = TRUE;
    options.noTypeDiversity = TRUE;
    options.noLegendaries = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    EXPECT(RandomBattleTestPlayerHasSpecies(SPECIES_PIKACHU));

    RandomBattle_RestorePlayerParty();
}

TEST("Pool random battle maximizes type diversity")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(4, 1);

    options.preEvos = TRUE;
    options.noItemClause = TRUE;
    options.noLegendaries = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    EXPECT_GE(RandomBattleTestCountPlayerTypes(), 4);

    RandomBattle_RestorePlayerParty();
}

TEST("Random battle stores supplied trainer metadata")
{
    struct RandomBattleOptions options = RandomBattleTestOptions(1, 1);
    const struct Trainer *trainer;

    options.trainerPic = TRAINER_PIC_BEAUTY;
    options.trainerName = sTestTrainerName;
    options.isDouble = TRUE;
    options.preEvos = TRUE;
    options.noItemClause = TRUE;
    RandomBattleTestMakePlayerParty(SPECIES_WOBBUFFET);

    EXPECT(RandomBattle_GeneratePoolBattle(&options));
    trainer = RandomBattle_GetTrainerStruct();
    enum TrainerBattleType battleType = trainer->battleType;
    EXPECT_EQ(trainer->trainerPic, TRAINER_PIC_BEAUTY);
    EXPECT_EQ(battleType, TRAINER_BATTLE_TYPE_DOUBLES);
    EXPECT(StringCompare(trainer->trainerName, sTestTrainerName) == 0);

    RandomBattle_RestorePlayerParty();
}
