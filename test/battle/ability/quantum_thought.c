#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Quantum Thought activates with an Apex Meteorite for Beldum, Metang and Metagross")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Item item;
    for (u32 i = 0; i < 3; i++)
    {
        PARAMETRIZE { species = SPECIES_BELDUM + i; item = ITEM_APEX_METEORITE; }
        PARAMETRIZE { species = SPECIES_BELDUM + i; item = ITEM_NONE; }
    }
    GIVEN {
        PLAYER(species) { Item(item); Ability(ABILITY_CLEAR_BODY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        if (item == ITEM_APEX_METEORITE)
            MESSAGE("A critical hit!");
        else
            NOT MESSAGE("A critical hit!");
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_QUANTUM_THOUGHT);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_APEX_METEORITE ? ABILITY_QUANTUM_THOUGHT : ABILITY_CLEAR_BODY);
    }
}

SINGLE_BATTLE_TEST("Quantum Thought guarantees physical and special critical hits with every generation's crit rules")
{
    u32 gen = GEN_9;
    enum Move move = MOVE_NONE;
    for (u32 i = GEN_1; i <= GEN_9; i++)
    {
        PARAMETRIZE { gen = i; move = MOVE_TACKLE; }
        PARAMETRIZE { gen = i; move = MOVE_WATER_GUN; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Quantum Thought bypasses accuracy drops, evasion boosts and evasion items")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_BRIGHT_POWDER; }
    PARAMETRIZE { item = ITEM_LAX_INCENSE; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); MOVE(opponent, MOVE_DOUBLE_TEAM); }
        TURN { MOVE(opponent, MOVE_SAND_ATTACK); MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    } THEN {
        struct BattleCalcValues cv = {
            .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_HYDRO_PUMP,
            .abilities[B_BATTLER_0] = ABILITY_QUANTUM_THOUGHT,
        };
        player->statStages[STAT_ACC] = MIN_STAT_STAGE;
        opponent->statStages[STAT_EVASION] = MAX_STAT_STAGE;
        EXPECT(CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        opponent->hp = 0;
        EXPECT(!CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
    }
}

SINGLE_BATTLE_TEST("Quantum Thought does not improve status moves or incoming attacks")
{
    bool32 incoming;
    PARAMETRIZE { incoming = FALSE; }
    PARAMETRIZE { incoming = TRUE; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (incoming)
            TURN { MOVE(opponent, MOVE_HYDRO_PUMP, hit: FALSE); }
        else
            TURN { MOVE(player, MOVE_HYPNOSIS, hit: FALSE); }
    } SCENE {
        if (incoming)
            NOT HP_BAR(player);
        else
            NOT STATUS_ICON(opponent, STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Quantum Thought respects Battle Armor, Shell Armor and Lucky Chant")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Ability ability = ABILITY_NONE;
    bool32 chant = FALSE;
    u32 gen = GEN_9;
    for (u32 i = GEN_1; i <= GEN_9; i++)
    {
        PARAMETRIZE { gen = i; species = SPECIES_ARMALDO; ability = ABILITY_BATTLE_ARMOR; chant = FALSE; }
        PARAMETRIZE { gen = i; species = SPECIES_KINGLER; ability = ABILITY_SHELL_ARMOR; chant = FALSE; }
        PARAMETRIZE { gen = i; species = SPECIES_WOBBUFFET; ability = ABILITY_TELEPATHY; chant = TRUE; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(species) { Ability(ability); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (chant)
                MOVE(opponent, MOVE_LUCKY_CHANT);
            MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE);
        }
    } SCENE {
        HP_BAR(opponent);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Quantum Thought respects semi-invulnerability")
{
    enum Move move = MOVE_NONE;
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_DIG; }
    PARAMETRIZE { move = MOVE_DIVE; }
    PARAMETRIZE { move = MOVE_PHANTOM_FORCE; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Quantum Thought respects Protect and type or ability immunity")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Move move = MOVE_NONE;
    bool32 protect;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; move = MOVE_TACKLE; protect = TRUE; }
    PARAMETRIZE { species = SPECIES_GASTLY; move = MOVE_TACKLE; protect = FALSE; }
    PARAMETRIZE { species = SPECIES_VAPOREON; move = MOVE_HYDRO_PUMP; protect = FALSE; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(species == SPECIES_VAPOREON ? ABILITY_WATER_ABSORB : ABILITY_LEVITATE); }
    } WHEN {
        TURN {
            if (protect)
                MOVE(opponent, MOVE_PROTECT);
            MOVE(player, move);
        }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Quantum Thought guarantees each hit of a multi-hit attack")
{
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRIPLE_AXEL, hit: FALSE, criticalHit: FALSE); }
    } SCENE {
        for (u32 i = 0; i < 3; i++)
        {
            HP_BAR(opponent);
            MESSAGE("A critical hit!");
        }
        MESSAGE("The Pokémon was hit 3 times!");
    }
}

DOUBLE_BATTLE_TEST("Quantum Thought calculates crit protection separately for each spread target")
{
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KINGLER) { Ability(ABILITY_SHELL_ARMOR); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        NOT MESSAGE("A critical hit on the opposing Kingler!");
        MESSAGE("A critical hit on the opposing Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Quantum Thought guarantees OHKO accuracy while preserving level and Sturdy restrictions")
{
    enum Ability ability = ABILITY_NONE;
    u32 level;
    PARAMETRIZE { ability = ABILITY_LIGHT_METAL; level = 50; }
    PARAMETRIZE { ability = ABILITY_LIGHT_METAL; level = 51; }
    PARAMETRIZE { ability = ABILITY_STURDY; level = 50; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); Level(50); }
        OPPONENT(SPECIES_REGISTEEL) { Level(level); Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_FISSURE, hit: FALSE); }
    } SCENE {
        if (level == 50 && ability != ABILITY_STURDY)
            HP_BAR(opponent);
        else
            NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, level == 50 && ability != ABILITY_STURDY ? 0 : opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Quantum Thought respects Ability Shield when a move would bypass critical-hit immunity")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_ABILITY_SHIELD; }
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_KINGLER) { Ability(ABILITY_SHELL_ARMOR); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNSTEEL_STRIKE, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        if (item == ITEM_ABILITY_SHIELD)
            NOT MESSAGE("A critical hit!");
        else
            MESSAGE("A critical hit!");
    }
}
