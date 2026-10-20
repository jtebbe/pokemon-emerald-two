#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Space Dominus makes damaging attacks bypass accuracy and evasion")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_PALKIA; }
    PARAMETRIZE { species = SPECIES_PALKIA_ORIGIN; }
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYDRO_PUMP); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        struct BattleCalcValues cv = {0};
        cv.battlerAtk = B_BATTLER_0;
        cv.battlerDef = B_BATTLER_1;
        cv.abilities[B_BATTLER_0] = ABILITY_SPACE_DOMINUS;
        cv.move = MOVE_HYDRO_PUMP;
        player->statStages[STAT_ACC] = MIN_STAT_STAGE;
        opponent->statStages[STAT_EVASION] = MAX_STAT_STAGE;
        EXPECT(CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        cv.move = MOVE_HYPNOSIS;
        EXPECT(!CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        cv.battlerAtk = B_BATTLER_1;
        cv.battlerDef = B_BATTLER_0;
        cv.move = MOVE_HYDRO_PUMP;
        EXPECT(!CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
    }
}

SINGLE_BATTLE_TEST("Space Dominus does not change status move accuracy")
{
    PASSES_RANDOMLY(GetMoveAccuracy(MOVE_HYPNOSIS), 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); }
    } SCENE {
        STATUS_ICON(opponent, STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Space Dominus does not hit through semi-invulnerability")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DIG); MOVE(player, MOVE_HYDRO_PUMP); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Space Dominus bypasses shields with intrinsically high-crit moves without contact penalties")
{
    enum Move shield;
    PARAMETRIZE { shield = MOVE_PROTECT; }
    PARAMETRIZE { shield = MOVE_SPIKY_SHIELD; }
    PARAMETRIZE { shield = MOVE_KINGS_SHIELD; }
    PARAMETRIZE { shield = MOVE_BANEFUL_BUNKER; }
    PARAMETRIZE { shield = MOVE_BURNING_BULWARK; }
    PARAMETRIZE { shield = MOVE_OBSTRUCT; }
    PARAMETRIZE { shield = MOVE_SILK_TRAP; }
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, shield); MOVE(player, MOVE_SLASH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Space Dominus does not let ordinary moves bypass Protect after Focus Energy")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Space Dominus and Omnipotence bypass Wide Guard for qualifying spread moves")
{
    enum Species species;
    enum Move move;
    PARAMETRIZE { species = SPECIES_PALKIA; move = MOVE_RAZOR_LEAF; }
    PARAMETRIZE { species = SPECIES_ARCEUS_NORMAL; move = MOVE_SURF; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WIDE_GUARD); MOVE(playerLeft, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}

SINGLE_BATTLE_TEST("Omnipotence floors type-chart immunities and resistances but preserves weaknesses")
{
    enum Species species;
    enum Move move;
    uq4_12_t expected;
    PARAMETRIZE { species = SPECIES_GENGAR; move = MOVE_SCRATCH; expected = UQ_4_12(1.0); }
    PARAMETRIZE { species = SPECIES_TYRANITAR; move = MOVE_SCRATCH; expected = UQ_4_12(1.0); }
    PARAMETRIZE { species = SPECIES_MAGNEZONE; move = MOVE_WING_ATTACK; expected = UQ_4_12(1.0); }
    PARAMETRIZE { species = SPECIES_PIDGEOT; move = MOVE_EARTH_POWER; expected = UQ_4_12(1.0); }
    PARAMETRIZE { species = SPECIES_CHARIZARD; move = MOVE_POWER_GEM; expected = UQ_4_12(4.0); }
    PARAMETRIZE { species = SPECIES_PARASECT; move = MOVE_FLYING_PRESS; expected = UQ_4_12(2.0); }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        struct DamageContext ctx = {0};
        ctx.battlerAtk = B_BATTLER_0;
        ctx.battlerDef = B_BATTLER_1;
        ctx.abilities[B_BATTLER_0] = ABILITY_OMNIPOTENCE;
        ctx.abilities[B_BATTLER_1] = GetBattlerAbility(B_BATTLER_1);
        ctx.move = ctx.chosenMove = move;
        ctx.moveType = GetMoveType(move);
        EXPECT_EQ(CalcTypeEffectivenessMultiplier(&ctx), expected);
    }
}

SINGLE_BATTLE_TEST("Omnipotence preserves Levitate Wonder Guard and Water Absorb immunities")
{
    enum Species species;
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { species = SPECIES_WEEZING; ability = ABILITY_LEVITATE; move = MOVE_EARTH_POWER; }
    PARAMETRIZE { species = SPECIES_SHEDINJA; ability = ABILITY_WONDER_GUARD; move = MOVE_SCRATCH; }
    PARAMETRIZE { species = SPECIES_VAPOREON; ability = ABILITY_WATER_ABSORB; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Omnipotence preserves Tera Shell's ability-based resistance")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SHADOW_BALL; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_TERAPAGOS_TERASTAL) { Ability(ABILITY_TERA_SHELL); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's not very effective…");
    } THEN {
        struct DamageContext ctx = {0};
        ctx.battlerAtk = B_BATTLER_0;
        ctx.battlerDef = B_BATTLER_1;
        ctx.abilities[B_BATTLER_0] = ABILITY_OMNIPOTENCE;
        ctx.abilities[B_BATTLER_1] = ABILITY_TERA_SHELL;
        ctx.move = ctx.chosenMove = move;
        ctx.moveType = GetMoveType(move);
        u32 hp = opponent->hp;
        opponent->hp = opponent->maxHP;
        EXPECT_EQ(CalcTypeEffectivenessMultiplier(&ctx), UQ_4_12(0.5));
        opponent->hp = hp;
    }
}

SINGLE_BATTLE_TEST("Omnipotence preserves Air Balloon and Magnet Rise Ground immunity")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_AIR_BALLOON; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); }
    } WHEN {
        if (item == ITEM_NONE)
            TURN { MOVE(opponent, MOVE_MAGNET_RISE); }
        TURN { MOVE(player, MOVE_EARTH_POWER); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Omnipotence bypasses Protect only for non-positive effective priority")
{
    enum Move move;
    bool32 blocked;
    PARAMETRIZE { move = MOVE_SCRATCH; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_DRAGON_TAIL; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_EXTREME_SPEED; blocked = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, move); }
    } SCENE {
        if (blocked)
            NOT HP_BAR(opponent);
        else
            HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp == opponent->maxHP, blocked);
    }
}

SINGLE_BATTLE_TEST("Omnipotence recognizes Grassy Glide's terrain-boosted priority")
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GRASSY_GLIDE); }
    } SCENE {
        if (terrain)
            NOT HP_BAR(opponent);
        else
            HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp == opponent->maxHP, terrain);
    }
}

SINGLE_BATTLE_TEST("Omnipotence lets non-priority status moves bypass Protect")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_THUNDER_WAVE, hit: TRUE); }
    } SCENE {
        STATUS_ICON(opponent, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Omnipotence does not remove status move type immunities or Synchronoise's prerequisite")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { move = MOVE_SYNCHRONOISE; }
    GIVEN {
        PLAYER(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SANDSHREW);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        NONE_OF {
            HP_BAR(opponent);
            STATUS_ICON(opponent, STATUS1_PARALYSIS);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
