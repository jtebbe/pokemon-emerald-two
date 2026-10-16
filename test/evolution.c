#include "global.h"
#include "move.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Former friendship evolutions require level five rather than evolving early")
{
    const struct { enum Species from; enum Species to; } evolutions[] = {
        {SPECIES_PICHU, SPECIES_PIKACHU},
        {SPECIES_CLEFFA, SPECIES_CLEFAIRY},
        {SPECIES_IGGLYBUFF, SPECIES_JIGGLYPUFF},
        {SPECIES_GOLBAT, SPECIES_CROBAT},
        {SPECIES_CHANSEY, SPECIES_BLISSEY},
        {SPECIES_TOGEPI, SPECIES_TOGETIC},
        {SPECIES_AZURILL, SPECIES_MARILL},
        {SPECIES_BUNEARY, SPECIES_LOPUNNY},
        {SPECIES_WOOBAT, SPECIES_SWOOBAT},
        {SPECIES_SWADLOON, SPECIES_LEAVANNY},
    };
    struct Pokemon mon;
    bool32 canStop;
    for (u32 i = 0; i < ARRAY_COUNT(evolutions); i++)
    {
        CreateMon(&mon, evolutions[i].from, 4, 0, OTID_STRUCT_PRESET(0));
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);
        CreateMon(&mon, evolutions[i].from, 5, 0, OTID_STRUCT_PRESET(0));
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), evolutions[i].to);
    }
}

TEST("Level five evolution still respects Everstone")
{
    struct Pokemon mon;
    bool32 canStop;
    enum Item item = ITEM_EVERSTONE;
    CreateMon(&mon, SPECIES_PICHU, 5, 0, OTID_STRUCT_PRESET(0));
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);
}

TEST("Eevee retains its time and Fairy move evolution conditions at level five")
{
    const struct Evolution *evolutions = GetSpeciesEvolutions(SPECIES_EEVEE);
    u32 count = 0;
    for (u32 i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
    {
        enum Species target = evolutions[i].targetSpecies;
        if (target != SPECIES_ESPEON && target != SPECIES_UMBREON && target != SPECIES_SYLVEON)
            continue;
        EXPECT_EQ(evolutions[i].method, EVO_LEVEL);
        EXPECT_EQ(evolutions[i].param, 5);
        EXPECT(evolutions[i].params != NULL);
        EXPECT_EQ(evolutions[i].params[0].condition,
                  target == SPECIES_SYLVEON ? IF_KNOWS_MOVE_TYPE : target == SPECIES_ESPEON ? IF_NOT_TIME : IF_TIME);
        EXPECT_EQ(evolutions[i].params[1].condition, CONDITIONS_END);
        count++;
    }
    EXPECT_EQ(count, 3);
}

TEST("Pika Papow and Veevee Volley have fixed power without friendship effects")
{
    EXPECT_EQ(GetMovePower(MOVE_PIKA_PAPOW), 100);
    EXPECT_EQ(GetMovePower(MOVE_VEEVEE_VOLLEY), 100);
    EXPECT_EQ(GetMoveEffect(MOVE_PIKA_PAPOW), EFFECT_HIT);
    EXPECT_EQ(GetMoveEffect(MOVE_VEEVEE_VOLLEY), EFFECT_HIT);
}
