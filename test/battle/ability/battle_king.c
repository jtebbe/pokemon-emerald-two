#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Battle King is exclusive to the male Nidoran line with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_NIDORAN_M; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORINO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDOKING; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORAN_M; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_NIDORAN_F; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDORINA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NIDOQUEEN; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH, criticalHit: TRUE); }
    } SCENE {
        HP_BAR(player);
        if (item == ITEM_APEX_METEORITE && (species == SPECIES_NIDORAN_M || species == SPECIES_NIDORINO || species == SPECIES_NIDOKING))
            NOT MESSAGE("A critical hit!");
        else
            MESSAGE("A critical hit!");
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_BATTLE_KING,
                  item == ITEM_APEX_METEORITE && (species == SPECIES_NIDORAN_M || species == SPECIES_NIDORINO || species == SPECIES_NIDOKING));
    }
}

SINGLE_BATTLE_TEST("Battle King stacks Technician and Sheer Force to 1.95x using the original base power", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    enum Ability ability = ABILITY_NONE;
    const enum Move attacks[] = {MOVE_SCRATCH, MOVE_STRENGTH, MOVE_FORCE_PALM, MOVE_WATER_PULSE, MOVE_BODY_SLAM, MOVE_EARTH_POWER};
    for (u32 j = 0; j < ARRAY_COUNT(attacks); j++) {
        PARAMETRIZE { move = attacks[j]; ability = ABILITY_NONE; }
        PARAMETRIZE { move = attacks[j]; ability = ABILITY_TECHNICIAN; }
        PARAMETRIZE { move = attacks[j]; ability = ABILITY_SHEER_FORCE; }
        PARAMETRIZE { move = attacks[j]; ability = ABILITY_BATTLE_KING; }
    }
    GIVEN {
        ASSUME(GetMovePower(MOVE_FORCE_PALM) == 60);
        ASSUME(GetMovePower(MOVE_WATER_PULSE) == 60);
        PLAYER(SPECIES_NIDOKING) { Ability(ability); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Scratch: Technician only. Strength: neither component.
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[1].damage, results[3].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
        EXPECT_EQ(results[4].damage, results[6].damage);
        EXPECT_EQ(results[4].damage, results[7].damage);
        // Both 60 BP moves must retain Technician even though Sheer Force alone raises them to 78 BP.
        for (u32 j = 8; j < 16; j += 4) {
            EXPECT_MUL_EQ(results[j].damage, UQ_4_12(1.95), results[j + 3].damage);
            EXPECT_MUL_EQ(results[j + 1].damage, UQ_4_12(1.3), results[j + 3].damage);
            EXPECT_MUL_EQ(results[j + 2].damage, UQ_4_12(1.5), results[j + 3].damage);
        }
        // Body Slam and Earth Power: Sheer Force only, with no Technician boost above 60 BP.
        for (u32 j = 16; j < 24; j += 4) {
            EXPECT_EQ(results[j].damage, results[j + 1].damage);
            EXPECT_EQ(results[j + 2].damage, results[j + 3].damage);
            EXPECT_MUL_EQ(results[j].damage, UQ_4_12(1.3), results[j + 3].damage);
        }
    }
}

SINGLE_BATTLE_TEST("Battle King removes guaranteed secondary effects on the user and target")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_POWER_UP_PUNCH; }
    PARAMETRIZE { move = MOVE_ACID_SPRAY; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Battle King removes status and flinch secondary effects")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THUNDER_SHOCK; }
    PARAMETRIZE { move = MOVE_BITE; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Battle King retains primary effects and self-inflicted stat drops")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUPERPOWER); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Battle King keeps draining effects on moves without a Sheer Force boost")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_GT(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Battle King checks Technician after Weather Ball's weather power change", s16 damage)
{
    enum Ability ability;
    bool32 rain;
    PARAMETRIZE { rain = FALSE; ability = ABILITY_NONE; }
    PARAMETRIZE { rain = FALSE; ability = ABILITY_BATTLE_KING; }
    PARAMETRIZE { rain = TRUE; ability = ABILITY_NONE; }
    PARAMETRIZE { rain = TRUE; ability = ABILITY_BATTLE_KING; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (rain) TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Battle King does not boost fixed damage")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
    }
}

SINGLE_BATTLE_TEST("Battle King boosts each eligible strike of a multihit move", s16 damage[2])
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_BATTLE_KING; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ability); Attack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage[0]);
        HP_BAR(opponent, captureDamage: &results[i].damage[1]);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage[0], UQ_4_12(1.5), results[1].damage[0]);
        EXPECT_MUL_EQ(results[0].damage[1], UQ_4_12(1.5), results[1].damage[1]);
    }
}

SINGLE_BATTLE_TEST("Battle King blocks forced and guaranteed criticals even against Mold Breaker and Quantum Thought")
{
    enum Move move = MOVE_SCRATCH;
    enum Ability ability = ABILITY_NONE;
    u32 gen = GEN_9;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { gen = j ? GEN_9 : GEN_1; move = MOVE_SCRATCH; ability = ABILITY_NONE; }
        PARAMETRIZE { gen = j ? GEN_9 : GEN_1; move = MOVE_WICKED_BLOW; ability = ABILITY_MOLD_BREAKER; }
        PARAMETRIZE { gen = j ? GEN_9 : GEN_1; move = MOVE_SCRATCH; ability = ABILITY_QUANTUM_THOUGHT; }
    }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, move, criticalHit: TRUE); }
    } SCENE {
        HP_BAR(player);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Battle King cannot be suppressed by Neutralizing Gas or Gastro Acid")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GASTRO_ACID); }
        TURN { MOVE(opponent, MOVE_WICKED_BLOW); }
    } SCENE {
        MESSAGE("But it failed!");
        HP_BAR(player);
        NOT MESSAGE("A critical hit!");
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_BATTLE_KING);
    }
}

SINGLE_BATTLE_TEST("Battle King skips Life Orb and Shell Bell effects only on Sheer Force moves")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_BODY_SLAM; item = ITEM_LIFE_ORB; }
    PARAMETRIZE { move = MOVE_STRENGTH; item = ITEM_LIFE_ORB; }
    PARAMETRIZE { move = MOVE_BODY_SLAM; item = ITEM_SHELL_BELL; }
    PARAMETRIZE { move = MOVE_STRENGTH; item = ITEM_SHELL_BELL; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ABILITY_BATTLE_KING); Item(item); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent);
        if (move == MOVE_BODY_SLAM)
            NOT HP_BAR(player);
        else if (item == ITEM_LIFE_ORB)
            HP_BAR(player, damage: 100);
        else
            HP_BAR(player);
    } THEN {
        if (move == MOVE_BODY_SLAM) EXPECT_EQ(player->hp, 500);
    }
}
