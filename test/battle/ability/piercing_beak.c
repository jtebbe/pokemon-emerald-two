#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Piercing Beak is the Apex ability of Spearow and Fearow and halves physical Defense", s16 damage)
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_SPEAROW; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SPEAROW; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_FEAROW; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_FEAROW; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_KEEN_EYE); Item(item); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(item == ITEM_NONE ? 100 : 200); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_PIERCING_BEAK);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_NONE ? ABILITY_KEEN_EYE : ABILITY_PIERCING_BEAK);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak affects attacks that use Defense including Psyshock Body Press and Foul Play", s16 damage)
{
    enum Move move = MOVE_NONE;
    enum Item item = ITEM_NONE;
    static const enum Move moves[] = { MOVE_PSYSHOCK, MOVE_BODY_PRESS, MOVE_FOUL_PLAY };
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = moves[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Attack(100); SpAttack(100); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Attack(100); Defense(item == ITEM_NONE ? 100 : 200); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++)
            EXPECT_EQ(results[j * 2].damage, results[j * 2 + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak does not halve Special Defense", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(200); SpDefense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak halves the total Defense after stages Eviolite and rounding", s16 damage)
{
    bool32 piercing;
    PARAMETRIZE { piercing = FALSE; }
    PARAMETRIZE { piercing = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(piercing ? ITEM_APEX_METEORITE : ITEM_NONE); Attack(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); Defense(piercing ? 103 : 115); Item(piercing ? ITEM_EVIOLITE : ITEM_NONE); }
    } WHEN {
        if (piercing) TURN { MOVE(opponent, MOVE_HARDEN); }
        else TURN {}
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(opponent->defense, piercing ? 103 : 115);
    } FINALLY {
        // 103 at +1 Defense -> 154; Eviolite -> 231; half of that -> 115.
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak includes Fur Coat in total Defense", s16 damage)
{
    bool32 piercing;
    PARAMETRIZE { piercing = FALSE; }
    PARAMETRIZE { piercing = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(piercing ? ITEM_APEX_METEORITE : ITEM_NONE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(piercing ? ABILITY_FUR_COAT : ABILITY_SHADOW_TAG); HP(3000); MaxHP(3000); Defense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak follows the physical Defense stat when Wonder Room swaps defenses", s16 damage)
{
    enum Move move = MOVE_NONE;
    enum Item item = ITEM_NONE;
    static const enum Move moves[] = { MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_PSYSHOCK };
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = moves[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(move == MOVE_WATER_GUN && item == ITEM_NONE ? 100 : 200); SpDefense(400); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WONDER_ROOM); }
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++)
            EXPECT_EQ(results[j * 2].damage, results[j * 2 + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak clamps halved Defense to one", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak does not change fixed damage", s16 damage)
{
    enum Item item = ITEM_NONE;
    enum Move move = MOVE_NONE;
    PARAMETRIZE { move = MOVE_DRAGON_RAGE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_DRAGON_RAGE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SEISMIC_TOSS; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SEISMIC_TOSS; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(200); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak increases critical hit ratio by one stage across generations")
{
    u32 config = GEN_2, passes = 1, trials = 8;
    PARAMETRIZE { config = GEN_1; passes = 25; trials = 32; }
    for (u32 gen = GEN_2; gen <= GEN_9; gen++)
        PARAMETRIZE { config = gen; passes = 1; trials = 8; }
    PASSES_RANDOMLY(passes, trials, RNG_CRITICAL_HIT);
    GIVEN {
        ASSUME(GetSpeciesBaseSpeed(SPECIES_FEAROW) == 100);
        WITH_CONFIG(B_CRIT_CHANCE, config);
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Piercing Beak stacks with a high critical hit ratio move")
{
    PASSES_RANDOMLY(1, 2, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Piercing Beak and Focus Energy guarantee critical hits unless prevented")
{
    enum Ability ability;
    bool32 luckyChant = FALSE;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; }
    PARAMETRIZE { ability = ABILITY_SHELL_ARMOR; }
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; luckyChant = TRUE; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); if (luckyChant) MOVE(opponent, MOVE_LUCKY_CHANT); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        if (ability == ABILITY_SHADOW_TAG && !luckyChant)
            MESSAGE("A critical hit!");
        else
            NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Piercing Beak prevents Attack drops from status and damaging moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_CHARM; }
    PARAMETRIZE { move = MOVE_FEATHER_DANCE; }
    PARAMETRIZE { move = MOVE_BREAKING_SWIPE; }
    PARAMETRIZE { move = MOVE_PLAY_ROUGH; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak prevents Intimidate")
{
    GIVEN {
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_SPEAROW) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INTIMIDATE);
        ABILITY_POPUP(opponent, ABILITY_PIERCING_BEAK);
        MESSAGE("The opposing Spearow's Attack was not lowered!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak does not protect other stats in a move that also lowers Attack")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NOBLE_ROAR); }
    } SCENE {
        MESSAGE("The opposing Fearow's Attack was not lowered!");
        MESSAGE("The opposing Fearow's Sp. Atk fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak permits self inflicted Attack drops like Hyper Cutter")
{
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUPERPOWER); }
    } SCENE {
        MESSAGE("Fearow's Attack fell!");
        MESSAGE("Fearow's Defense fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak Attack protection survives Mold Breaker and Neutralizing Gas")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { ability = ABILITY_NEUTRALIZING_GAS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_PIERCING_BEAK);
        MESSAGE("The opposing Fearow's Attack was not lowered!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak does not prevent burn damage reduction", s16 damage)
{
    bool32 burned;
    PARAMETRIZE { burned = FALSE; }
    PARAMETRIZE { burned = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Item(ITEM_APEX_METEORITE); Status1(burned ? STATUS1_BURN : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Beak does not amplify self inflicted confusion damage", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); Item(item); Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CONFUSE_RAY); MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_CONFUSION, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, opponent);
        MESSAGE("It hurt itself in its confusion!");
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
