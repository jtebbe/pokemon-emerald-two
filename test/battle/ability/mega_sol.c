#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mega Sol multiplies the power of Fire-type moves by 1.5x")
{
    s16 damage[2];

    GIVEN {
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_SKILL_SWAP); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], Q_4_12(1.5), damage[0]);
    }
}

SINGLE_BATTLE_TEST("Mega Sol halves the power of the user's Water-type moves")
{
    s16 damage[2];

    GIVEN {
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_SKILL_SWAP); }
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], Q_4_12(0.5), damage[0]);
    }
}

SINGLE_BATTLE_TEST("Weather Ball stays Fire-type under real weather if user has Mega Sol")
{
    u16 weatherMove;
    PARAMETRIZE { weatherMove = MOVE_RAIN_DANCE; }
    PARAMETRIZE { weatherMove = MOVE_SANDSTORM; }
    PARAMETRIZE { weatherMove = MOVE_SNOWSCAPE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WEATHER_BALL) == EFFECT_WEATHER_BALL);
        ASSUME(GetSpeciesType(SPECIES_BELDUM, 0) == TYPE_STEEL || GetSpeciesType(SPECIES_BELDUM, 1) == TYPE_STEEL);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_BELDUM);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { MOVE(opponent, weatherMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, weatherMove, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Mega Sol doesn't trigger the foe's Leaf Guard")
{
    u16 move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SUNNY_DAY; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WILL_O_WISP) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_WILL_O_WISP) == MOVE_EFFECT_BURN);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_LEAFEON) { Ability(ABILITY_LEAF_GUARD); }
    } WHEN {
        TURN { MOVE(player, move, gimmick: GIMMICK_MEGA); }
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (move == MOVE_CELEBRATE)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
            STATUS_ICON(opponent, STATUS1_BURN);
        }
        else
        {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
            ABILITY_POPUP(opponent, ABILITY_LEAF_GUARD);
            NOT STATUS_ICON(opponent, STATUS1_BURN);
        }
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Cloud Nine")
{
    s16 damage[2];
    u16 species;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_GOLDUCK;  ability = ABILITY_CLOUD_NINE; }
    PARAMETRIZE { species = SPECIES_RAYQUAZA; ability = ABILITY_AIR_LOCK; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_SKILL_SWAP); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], Q_4_12(1.5), damage[0]);
    }
}

SINGLE_BATTLE_TEST("Mega Sol: Solar Beam does not need a charging turn if user has Mega Sol")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SOLAR_BEAM) == EFFECT_SOLAR_BEAM);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SOLAR_BEAM, gimmick: GIMMICK_MEGA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Sandstorm's solar beam power reduction and Rock defense boost")
{
    s16 damage[2];

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SOLAR_BEAM) == EFFECT_SOLAR_BEAM);
        ASSUME(GetMoveType(MOVE_SOLAR_BEAM) == TYPE_GRASS);
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        ASSUME(GetSpeciesType(SPECIES_BASTIODON, 0) == TYPE_ROCK || GetSpeciesType(SPECIES_BASTIODON, 1) == TYPE_ROCK);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_BASTIODON) { Ability(ABILITY_SAND_STREAM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOLAR_BEAM, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_SKILL_SWAP); }
        TURN { MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SAND_STREAM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], Q_4_12(3.0), damage[0]);
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Snow's Ice-type Defense boost")
{
    s16 damage[2];

    GIVEN {
        ASSUME(IsBattleMovePhysical(MOVE_SCRATCH));
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        ASSUME(GetSpeciesType(SPECIES_VANILLUXE, 0) == TYPE_ICE || GetSpeciesType(SPECIES_VANILLUXE, 1) == TYPE_ICE);
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_MEGANIUMITE); }
        OPPONENT(SPECIES_VANILLUXE) { Ability(ABILITY_SNOW_WARNING); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SKILL_SWAP); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SNOW_WARNING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &damage[1]);
    } THEN {
        EXPECT_MUL_EQ(damage[1], Q_4_12(1.5), damage[0]);
    }
}
