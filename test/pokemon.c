#include "global.h"
#include "battle.h"
#include "egg_hatch.h"
#include "event_data.h"
#include "new_game.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "string_util.h"
#include "strings.h"
#include "test/overworld_script.h"
#include "test/test.h"
#include "constants/characters.h"
#include "constants/daycare.h"
#include "constants/move_relearner.h"

TEST("Pokemon stats always include the fixed maximum contribution")
{
    struct Pokemon mon;
    enum Species species = SPECIES_WOBBUFFET;
    u32 personality = GetMonPersonality(species, MON_GENDER_RANDOM, NATURE_HARDY, RANDOM_UNOWN_LETTER);
    CreateMonWithStats(&mon, species, 100, personality, OTID_STRUCT_PLAYER_ID);

    // Level 100, neutral nature, and no effort investment.
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), 2 * GetSpeciesBaseHP(species) + 141);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK), 2 * GetSpeciesBaseAttack(species) + 36);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF), 2 * GetSpeciesBaseDefense(species) + 36);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED), 2 * GetSpeciesBaseSpeed(species) + 36);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK), 2 * GetSpeciesBaseSpAttack(species) + 36);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF), 2 * GetSpeciesBaseSpDefense(species) + 36);
}

TEST("Nature flavor preferences remain correct for berries")
{
    static const enum Stat flavorStats[FLAVOR_COUNT] =
    {
        [FLAVOR_SPICY] = STAT_ATK,
        [FLAVOR_DRY] = STAT_SPATK,
        [FLAVOR_SWEET] = STAT_SPEED,
        [FLAVOR_BITTER] = STAT_SPDEF,
        [FLAVOR_SOUR] = STAT_DEF,
    };

    for (u32 nature = 0; nature < NUM_NATURES; nature++)
    {
        u32 personality = GetMonPersonality(SPECIES_WOBBUFFET, MON_GENDER_RANDOM, nature, RANDOM_UNOWN_LETTER);
        for (u32 flavor = 0; flavor < FLAVOR_COUNT; flavor++)
        {
            s32 expected = ((s32)ModifyStatByNature(nature, 100, flavorStats[flavor]) - 100) / 10;
            EXPECT_EQ(GetFlavorRelationByPersonality(personality, flavor), expected);
        }
    }
}

TEST("Summary has only info skills and battle move pages")
{
    EXPECT_EQ(PSS_PAGE_INFO, 0);
    EXPECT_EQ(PSS_PAGE_SKILLS, 1);
    EXPECT_EQ(PSS_PAGE_BATTLE_MOVES, 2);
    EXPECT_EQ(PSS_PAGE_COUNT, 3);
}

TEST("Move PP always uses PP Max values")
{
    EXPECT_EQ(GetMovePP(MOVE_SCRATCH), 56);
    EXPECT_EQ(GetMovePP(MOVE_WATER_GUN), 40);
    EXPECT_EQ(GetMovePP(MOVE_SLEEP_TALK), 16);
    EXPECT_EQ(GetMovePP(MOVE_BEHEMOTH_BLADE), 8);
}

TEST("Nature independent from Hidden Nature")
{
    u32 i, j, nature = 0, hiddenNature = 0;
    struct Pokemon mon;
    for (i = 0; i < NUM_NATURES; i++)
    {
        for (j = 0; j < NUM_NATURES; j++)
        {
            PARAMETRIZE { nature = i; hiddenNature = j; }
        }
    }
    enum Species species = SPECIES_WOBBUFFET;
    u32 personality = GetMonPersonality(species, MON_GENDER_RANDOM, nature, RANDOM_UNOWN_LETTER);
    CreateMon(&mon, species, 100, personality, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &hiddenNature);
    EXPECT_EQ(GetNature(&mon), nature);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), hiddenNature);
}

TEST("Terastallization type defaults to primary or secondary type")
{
    u32 i;
    enum Type teraType;
    struct Pokemon mon;
    for (i = 0; i < 128; i++) PARAMETRIZE {}
    CreateRandomMon(&mon, SPECIES_PIDGEY, 100);
    teraType = GetMonData(&mon, MON_DATA_TERA_TYPE);
    EXPECT(teraType == GetSpeciesType(SPECIES_PIDGEY, 0)
        || teraType == GetSpeciesType(SPECIES_PIDGEY, 1));
}

TEST("Terastallization type can be set to any type except TYPE_NONE")
{
    u32 i;
    enum Type teraType;
    struct Pokemon mon;
    for (i = 1; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE { teraType = i; }
    }
    CreateRandomMon(&mon, SPECIES_WOBBUFFET, 100);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);
    EXPECT_EQ(teraType, GetMonData(&mon, MON_DATA_TERA_TYPE));
}

TEST("Terastallization type is reset to the default types when setting Tera Type back to TYPE_NONE")
{
    u32 i;
    enum Type teraType, typeNone;
    struct Pokemon mon;
    for (i = 1; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE { teraType = i; typeNone = TYPE_NONE; }
    }
    CreateRandomMon(&mon, SPECIES_PIDGEY, 100);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &teraType);
    EXPECT_EQ(teraType, GetMonData(&mon, MON_DATA_TERA_TYPE));
    if (typeNone == TYPE_NONE)
        typeNone = GetTeraTypeFromPersonality(&mon);
    SetMonData(&mon, MON_DATA_TERA_TYPE, &typeNone);
    typeNone = GetMonData(&mon, MON_DATA_TERA_TYPE);
    EXPECT(typeNone == GetSpeciesType(SPECIES_PIDGEY, 0)
        || typeNone == GetSpeciesType(SPECIES_PIDGEY, 1));
}

TEST("Shininess independent from PID and OTID")
{
    u32 pid, otId, data;
    bool32 isShiny;
    struct Pokemon mon;
    PARAMETRIZE { pid = 0; otId = 0; }
    CreateMon(&mon, SPECIES_WOBBUFFET, 100, pid, OTID_STRUCT_PRESET(otId));
    isShiny = IsMonShiny(&mon);
    data = !isShiny;
    SetMonData(&mon, MON_DATA_IS_SHINY, &data);
    EXPECT_EQ(pid, GetMonData(&mon, MON_DATA_PERSONALITY));
    EXPECT_EQ(otId, GetMonData(&mon, MON_DATA_OT_ID));
    EXPECT_EQ(!isShiny, GetMonData(&mon, MON_DATA_IS_SHINY));
}

TEST("Shininess set on an Egg persists after hatching")
{
    u32 personality = SHINY_ODDS;
    u32 trainerId = 0;
    bool32 isShiny = TRUE;
    bool8 isEgg = TRUE;

    SetTrainerId(trainerId, gSaveData.playerTrainerId);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_TOGEPI, EGG_HATCH_LEVEL, personality, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &isEgg);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY, &isShiny);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);

    gSpecialVar_0x8004 = 0;
    ScriptHatchMon();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
}

TEST("Status1 round-trips through BoxPokemon")
{
    u32 status1;
    struct Pokemon mon1, mon2;
    PARAMETRIZE { status1 = STATUS1_NONE; }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(1); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(2); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(3); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(4); }
    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(5); }
    PARAMETRIZE { status1 = STATUS1_POISON; }
    PARAMETRIZE { status1 = STATUS1_BURN; }
    PARAMETRIZE { status1 = STATUS1_FREEZE; }
    PARAMETRIZE { status1 = STATUS1_PARALYSIS; }
    PARAMETRIZE { status1 = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status1 = STATUS1_FROSTBITE; }
    CreateRandomMon(&mon1, SPECIES_WOBBUFFET, 100);
    SetMonData(&mon1, MON_DATA_STATUS, &status1);
    BoxMonToMon(&mon1.box, &mon2);
    EXPECT_EQ(GetMonData(&mon2, MON_DATA_STATUS), status1);
}

TEST("givemon [simple]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
}

TEST("givemon respects FORM_CHANGE_ITEM_HOLD")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_ARCEUS_NORMAL, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_ARCEUS_GRASS, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_ARCEUS_ELECTRIC, 100, item=ITEM_ZAP_PLATE;
        givemon SPECIES_GIRATINA_ORIGIN, 100, item=ITEM_POTION;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_SPECIES), SPECIES_ARCEUS_ELECTRIC);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][3], MON_DATA_SPECIES), SPECIES_GIRATINA_ALTERED);
}

TEST("givemon [moves]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_NONE, move4=MOVE_NONE;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), GetMovePP(MOVE_SCRATCH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP2), GetMovePP(MOVE_SPLASH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP4), 0);
}

TEST("givemon [moves (default)]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_PYUKUMUKU, 100, move1=MOVE_DEFAULT, move2=MOVE_DEFAULT, move3=MOVE_DEFAULT;
    );

    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_PYUKUMUKU);
    u32 learnsetLength;
    for (learnsetLength = 0; learnset[learnsetLength].move != LEVEL_UP_MOVE_END; learnsetLength++)
    {
        ; // we just want to get length of the learnset array
    }
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PYUKUMUKU);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), learnset[learnsetLength - 4].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), learnset[learnsetLength - 3].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), learnset[learnsetLength - 2].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), learnset[learnsetLength - 1].move);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), GetMovePP(learnset[learnsetLength - 4].move));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP2), GetMovePP(learnset[learnsetLength - 3].move));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), GetMovePP(learnset[learnsetLength - 2].move));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP4), GetMovePP(learnset[learnsetLength - 1].move));
}

TEST("givemon [all]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, item=ITEM_LEFTOVERS, ball=BALL_MASTER, nature=NATURE_BOLD, abilityNum=2, gender=MON_MALE, hpEv=1, atkEv=2, defEv=3, speedEv=4, spAtkEv=5, spDefEv=6, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_CELEBRATE, move4=MOVE_EXPLOSION, shinyMode=SHINY_MODE_ALWAYS, teraType=TYPE_FIRE;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKEBALL), BALL_MASTER);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_BOLD);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), GetSpeciesAbility(SPECIES_WOBBUFFET, 2));
    EXPECT_EQ(GetMonGender(&gParties[B_TRAINER_PLAYER][0]), MON_MALE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 3);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 5);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 6);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_CELEBRATE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_EXPLOSION);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), GetMovePP(MOVE_SCRATCH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP2), GetMovePP(MOVE_SPLASH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), GetMovePP(MOVE_CELEBRATE));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP4), GetMovePP(MOVE_EXPLOSION));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_TERA_TYPE), TYPE_FIRE);
}

TEST("givemon [egg]: properties are preserved after hatching")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, item=ITEM_LEFTOVERS, ball=BALL_MASTER, nature=NATURE_BOLD, abilityNum=2, gender=MON_MALE, hpEv=1, atkEv=2, defEv=3, speedEv=4, spAtkEv=5, spDefEv=6, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_CELEBRATE, move4=MOVE_EXPLOSION, shinyMode=SHINY_MODE_ALWAYS, teraType=TYPE_FIRE, isEgg=TRUE;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG), TRUE);

    gSpecialVar_0x8004 = 0;
    ScriptHatchMon();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKEBALL), BALL_MASTER);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_BOLD);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), GetSpeciesAbility(SPECIES_WOBBUFFET, 2));
    EXPECT_EQ(GetMonGender(&gParties[B_TRAINER_PLAYER][0]), MON_MALE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 3);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 5);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 6);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_CELEBRATE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_EXPLOSION);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), GetMovePP(MOVE_SCRATCH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP2), GetMovePP(MOVE_SPLASH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), GetMovePP(MOVE_CELEBRATE));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP4), GetMovePP(MOVE_EXPLOSION));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_TERA_TYPE), TYPE_FIRE);

    //Let's test hatching specific properties too
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES_OR_EGG), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LANGUAGE), GAME_LANGUAGE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MET_LEVEL), 0);
}

TEST("givemon [vars]")
{
    ZeroPlayerPartyMons();

    VarSet(VAR_TEMP_C, SPECIES_WOBBUFFET);
    VarSet(VAR_TEMP_D, 100);
    VarSet(VAR_0x8000, ITEM_LEFTOVERS);
    VarSet(VAR_0x8001, BALL_MASTER);
    VarSet(VAR_0x8002, NATURE_BOLD);
    VarSet(VAR_0x8003, 2);
    VarSet(VAR_0x8004, MON_MALE);
    VarSet(VAR_0x8005, 1);
    VarSet(VAR_0x8006, 2);
    VarSet(VAR_0x8007, 3);
    VarSet(VAR_0x8008, 4);
    VarSet(VAR_0x8009, 5);
    VarSet(VAR_0x800A, 6);
    VarSet(VAR_TEMP_5, MOVE_SCRATCH);
    VarSet(VAR_TEMP_6, MOVE_SPLASH);
    VarSet(VAR_TEMP_7, MOVE_CELEBRATE);
    VarSet(VAR_TEMP_8, MOVE_EXPLOSION);
    VarSet(VAR_TEMP_9, SHINY_MODE_ALWAYS);
    VarSet(VAR_TEMP_B, TYPE_FIRE);

    RUN_OVERWORLD_SCRIPT(
        givemon VAR_TEMP_C, VAR_TEMP_D, item=VAR_0x8000, ball=VAR_0x8001, nature=VAR_0x8002, abilityNum=VAR_0x8003, gender=VAR_0x8004, hpEv=VAR_0x8005, atkEv=VAR_0x8006, defEv=VAR_0x8007, speedEv=VAR_0x8008, spAtkEv=VAR_0x8009, spDefEv=VAR_0x800A, move1=VAR_TEMP_5, move2=VAR_TEMP_6, move3=VAR_TEMP_7, move4=VAR_TEMP_8, shinyMode=VAR_TEMP_9, teraType=VAR_TEMP_B;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKEBALL), BALL_MASTER);
    EXPECT_EQ(GetNature(&gParties[B_TRAINER_PLAYER][0]), NATURE_BOLD);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), GetSpeciesAbility(SPECIES_WOBBUFFET, 2));
    EXPECT_EQ(GetMonGender(&gParties[B_TRAINER_PLAYER][0]), MON_MALE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_EV), 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF_EV), 3);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_EV), 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_EV), 5);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF_EV), 6);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_SCRATCH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE2), MOVE_SPLASH);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_CELEBRATE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE4), MOVE_EXPLOSION);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), GetMovePP(MOVE_SCRATCH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP2), GetMovePP(MOVE_SPLASH));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), GetMovePP(MOVE_CELEBRATE));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP4), GetMovePP(MOVE_EXPLOSION));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_TERA_TYPE), TYPE_FIRE);
}

TEST("checkteratype/setteratype work")
{
    CreateRandomMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 100);

    RUN_OVERWORLD_SCRIPT(
        checkteratype 0;
    );
    EXPECT(VarGet(VAR_RESULT) == TYPE_PSYCHIC);

    RUN_OVERWORLD_SCRIPT(
        setteratype TYPE_FIRE, 0;
        checkteratype 0;
    );
    EXPECT(VarGet(VAR_RESULT) == TYPE_FIRE);
}

TEST("createmon [simple]")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        createmon 1, 0, SPECIES_WOBBUFFET, 100;
        createmon 1, 1, SPECIES_WYNAUT, 10;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_WOBBUFFET);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_SPECIES), SPECIES_WYNAUT);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 10);
}

TEST("Pokémon level up learnsets fit within MAX_RELEARNER_MOVES")
{
    u32 j, count;
    enum Species species = SPECIES_NONE;
    const struct LevelUpMove *learnset;

    for(j = 0; j < SPECIES_EGG; j++)
    {
        if (IsSpeciesEnabled(j))
        {
            PARAMETRIZE { species = j; }
        }
    }

    learnset = GetSpeciesLevelUpLearnset(species);
    count = 0;
    for (j = 0; learnset[j].move != LEVEL_UP_MOVE_END; j++)
        count++;
    EXPECT_LT(count, MAX_RELEARNER_MOVES - 1); // - 1 because at least one move is already known
}

TEST("Optimised GetMonData")
{
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 5, Random32(), OTID_STRUCT_PRESET(0x12345678));
    u32 exp = 0x123456;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP, &exp);
    struct Benchmark optimised,
        vanilla = (struct Benchmark) { .ticks = 137 }; // From prior testing
    u32 expGet = 0;
    BENCHMARK(&optimised) { expGet = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP); }
    EXPECT_EQ(exp, expGet);
    EXPECT_FASTER(optimised, vanilla);
}

TEST("Optimised SetMonData")
{
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 5, Random32(), OTID_STRUCT_PRESET(0x12345678));
    u32 exp = 0x123456;
    struct Benchmark optimised,
        vanilla = (struct Benchmark) { .ticks = 205 }; // From prior testing
    BENCHMARK(&optimised) { SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP, &exp); }
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP), exp);
    EXPECT_FASTER(optimised, vanilla);
}

//Sanity check for a CalculateMonStats refactor (could be deleted or improved)
TEST("CalculateMonStats")
{
    ZeroPlayerPartyMons();

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100, item=ITEM_LEFTOVERS, ball=BALL_MASTER, nature=NATURE_BOLD, abilityNum=2, gender=MON_MALE, hpEv=1, atkEv=2, defEv=3, speedEv=4, spAtkEv=5, spDefEv=6, move1=MOVE_SCRATCH, move2=MOVE_SPLASH, move3=MOVE_CELEBRATE, move4=MOVE_EXPLOSION, shinyMode=SHINY_MODE_ALWAYS, teraType=TYPE_FIRE;
    );

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP), 521);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK), 91);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF), 167);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED), 103);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK), 103);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF), 153);

}

TEST("BoxPokemon uses one direct data layout")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_TORCHIC, 20, Random32(), OTID_STRUCT_PRESET(0x12345678));
    enum Item item = ITEM_ORAN_BERRY;
    enum Move move = MOVE_TACKLE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    SetMonData(&mon, MON_DATA_MOVE1, &move);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_TORCHIC);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_TACKLE);
}

TEST("Egg cycles persist independently of removed friendship data")
{
    struct Pokemon mon = {0};
    u8 eggCycles = 20;

    SetMonData(&mon, MON_DATA_EGG_CYCLES, &eggCycles);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_EGG_CYCLES), 20);
}

TEST("BoxPokemon cleanup preserves the streamed save layout")
{
    EXPECT_EQ(sizeof(struct BoxPokemon), 72);
    EXPECT_EQ(sizeof(struct Pokemon), 92);
    EXPECT_EQ(offsetof(struct BoxPokemon, otName), 20);
    EXPECT_EQ(offsetof(struct BoxPokemon, data), 32);
}

TEST("Species and egg state come from direct data regardless of old header bits")
{
    struct Pokemon mon;
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 headerBits;
    PARAMETRIZE { headerBits = 0; }
    PARAMETRIZE { headerBits = 7; }
    CreateMon(&mon, SPECIES_TORCHIC, 20, 123, OTID_STRUCT_PRESET(1234));
    SetMonData(&mon, MON_DATA_NICKNAME, COMPOUND_STRING("TESTMON"));
    // Preserve the form-change counter in bits 3-5 and vary only retired bits.
    ((u8 *)&mon.box)[offsetof(struct BoxPokemon, otName) - 1] = (3 << 3) | headerBits;
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DAYS_SINCE_FORM_CHANGE), 3);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_TORCHIC);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_EGG), FALSE);
    GetMonData(&mon, MON_DATA_NICKNAME, nickname);
    EXPECT_EQ(StringCompare(nickname, COMPOUND_STRING("TESTMON")), 0);

    struct BoxPokemon stored;
    struct Pokemon restored;
    SaveMonToBox(&stored, &mon);
    BoxMonToMon(&stored, &restored);
    EXPECT_EQ(memcmp(&stored, &restored.box, sizeof(stored)), 0);

    u8 isEgg = TRUE;
    SetMonData(&mon, MON_DATA_IS_EGG, &isEgg);
    EXPECT_EQ(mon.box.data.isEgg, TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_TORCHIC);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_EGG);
    GetMonData(&mon, MON_DATA_NICKNAME, nickname);
    EXPECT_EQ(StringCompare(nickname, gText_EggNickname), 0);

    isEgg = FALSE;
    SetMonData(&mon, MON_DATA_IS_EGG, &isEgg);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_EGG), FALSE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_TORCHIC);
    GetMonData(&mon, MON_DATA_NICKNAME, nickname);
    EXPECT_EQ(StringCompare(nickname, COMPOUND_STRING("TESTMON")), 0);
    mon.box.data.species = SPECIES_NONE;
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_NONE);
}

TEST("Retired header property IDs cannot change Pokemon data")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_TORCHIC, 20, 123, OTID_STRUCT_PRESET(1234));
    struct BoxPokemon before = mon.box;
    const u8 value = 1;
    const enum MonData fields[] = {MON_DATA_UNUSED_4, MON_DATA_UNUSED_5, MON_DATA_UNUSED_6};
    for (u32 i = 0; i < ARRAY_COUNT(fields); i++)
    {
        SetMonData(&mon, fields[i], &value);
        EXPECT_EQ(GetMonData(&mon, fields[i]), 0);
        EXPECT_EQ(memcmp(&mon.box, &before, sizeof(before)), 0);
    }
}
