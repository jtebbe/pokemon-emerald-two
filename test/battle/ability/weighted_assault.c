#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Weighted Assault uses all seven base Attack tiers with the holder's level and nature", s16 damage)
{
    // Reference Attack stats are for level 50 and an Adamant nature.
    static const struct
    {
        enum Species species;
        enum Item item;
        u16 attack;
    } cases[] = {
        {SPECIES_SNORLAX, ITEM_NONE, 88},             // Heavier, base 60
        {SPECIES_DRAGONITE, ITEM_NONE, 88},           // Equal weight, base 60
        {SPECIES_REGISTEEL, ITEM_NONE, 110},          // 5 kg lead, base 80
        {SPECIES_SUICUNE, ITEM_NONE, 132},            // 23 kg lead, base 100
        {SPECIES_AGGRON, ITEM_FLOAT_STONE, 132},      // 30 kg lead after Float Stone, base 100
        {SPECIES_FARIGIRAF, ITEM_NONE, 154},          // 50 kg lead, base 120
        {SPECIES_LICKILICKY, ITEM_NONE, 176},         // 70 kg lead, base 140
        {SPECIES_NOSEPASS, ITEM_NONE, 198},           // 113 kg lead, base 160
        {SPECIES_KROOKODILE, ITEM_NONE, 220},         // 113.7 kg lead, base 180
    };
    u32 testCase = 0;
    bool32 apex = FALSE;
    for (u32 j = 0; j < ARRAY_COUNT(cases); j++)
    {
        PARAMETRIZE { testCase = j; apex = TRUE; }
        PARAMETRIZE { testCase = j; apex = FALSE; }
    }
    GIVEN {
        if (apex)
            PLAYER(SPECIES_ONIX) { Item(ITEM_APEX_METEORITE); Level(50); Nature(NATURE_ADAMANT); }
        else
            PLAYER(SPECIES_ONIX) { Attack(cases[testCase].attack); Level(50); Nature(NATURE_ADAMANT); }
        OPPONENT(cases[testCase].species) { Item(cases[testCase].item); Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(cases); j++)
            EXPECT_EQ(results[2 * j].damage, results[2 * j + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Weighted Assault recalculates Steelix's tier after each Autotomize")
{
    s16 before, afterOne, afterTwo;
    GIVEN {
        PLAYER(SPECIES_STEELIX) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_DRAGONITE) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_AUTOTOMIZE); }
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_AUTOTOMIZE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &before);
        HP_BAR(opponent, captureDamage: &afterOne);
        HP_BAR(opponent, captureDamage: &afterTwo);
    } THEN {
        EXPECT_GT(before, afterOne);
        EXPECT_GT(afterOne, afterTwo);
    }
}
