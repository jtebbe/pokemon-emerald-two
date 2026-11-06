#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Dust to Dust belongs only to regular Sandshrew and Sandslash holding an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_SANDSHREW; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSLASH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSHREW_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSLASH_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSHREW; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SANDSLASH; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_SAND_VEIL); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_SHOT, player);
        HP_BAR(opponent);
        if ((species == SPECIES_SANDSHREW || species == SPECIES_SANDSLASH) && item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
            MESSAGE("A sandstorm kicked up!");
        } else {
            NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_DUST_TO_DUST,
                  (species == SPECIES_SANDSHREW || species == SPECIES_SANDSLASH) && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust starts sand after Ground and Rock attacks and status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_MUD_SHOT; }
    PARAMETRIZE { move = MOVE_POWER_GEM; }
    PARAMETRIZE { move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (!IsBattleMoveStatus(move)) HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust starts sand even after a miss, Protect, or Ground immunity")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(variant == 2 ? SPECIES_TAILLOW : SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (variant == 1) MOVE(opponent, MOVE_PROTECT);
            MOVE(player, variant == 0 ? MOVE_ROCK_SLIDE : MOVE_MUD_SHOT, hit: variant != 0);
        }
    } SCENE {
        NOT HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust does not start sand on entry, for other types, or if flinched or asleep")
{
    u32 variant;
    PARAMETRIZE { variant = 0; }
    PARAMETRIZE { variant = 1; }
    PARAMETRIZE { variant = 2; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (variant == 2) TURN { MOVE(opponent, MOVE_SPORE); }
        TURN {
            if (variant == 1) MOVE(opponent, MOVE_FAKE_OUT);
            MOVE(player, variant == 0 ? MOVE_SCRATCH : MOVE_MUD_SHOT);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
            MESSAGE("A sandstorm kicked up!");
        }
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust waits until the attacking turn of Dig")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DIG); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        MESSAGE("Sandshrew burrowed its way under the ground!");
        NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIG, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
    }
}

SINGLE_BATTLE_TEST("Dust to Dust triggers once after the last hit of a multistrike move")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BONE_RUSH, WITH_RNG(RNG_HITS, 3)); }
    } SCENE {
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
        NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
    }
}

DOUBLE_BATTLE_TEST("Dust to Dust waits until all targets of a spread attack have taken damage")
{
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); }
    } SCENE {
        HP_BAR(opponentLeft);
        NOT ABILITY_POPUP(playerLeft, ABILITY_DUST_TO_DUST);
        HP_BAR(opponentRight);
        ABILITY_POPUP(playerLeft, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
    }
}

SINGLE_BATTLE_TEST("Dust to Dust creates five turns of sand without refreshing existing sand")
{
    GIVEN {
        WITH_CONFIG(B_ABILITY_WEATHER, GEN_9);
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
        MESSAGE("A sandstorm kicked up!");
        for (u32 turn = 0; turn < 4; turn++)
            MESSAGE("The sandstorm is raging.");
        MESSAGE("The sandstorm subsided.");
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust uses the effective move type after Electrify")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIFY); MOVE(player, MOVE_MUD_SHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_SHOT, player);
        NOT ABILITY_POPUP(player, ABILITY_DUST_TO_DUST);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust Rock STAB applies to physical and special attacks without stacking Ground STAB", s16 damage)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_ROCK_THROW; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_ROCK_THROW; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_POWER_GEM; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_POWER_GEM; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_MUD_SHOT; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_MUD_SHOT; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Item(item); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust gains the sand Special Defense bonus without changing physical Defense", s16 damage)
{
    enum Move move = MOVE_NONE;
    u32 variant = 0;
    for (u32 m = 0; m < 3; m++)
        for (u32 v = 0; v < 4; v++)
            PARAMETRIZE { move = m == 0 ? MOVE_SWIFT : m == 1 ? MOVE_SCRATCH : MOVE_PSYSHOCK; variant = v; }
    GIVEN {
        WITH_CONFIG(B_SANDSTORM_SPDEF_BOOST, GEN_9);
        PLAYER(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Item(variant & 1 ? ITEM_APEX_METEORITE : ITEM_NONE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ABILITY_SAND_VEIL); Attack(300); SpAttack(300); }
    } WHEN {
        if (variant & 2) TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(1.5), results[0].damage);
        for (u32 j = 5; j < 8; j++) EXPECT_EQ(results[4].damage, results[j].damage);
        for (u32 j = 9; j < 12; j++) EXPECT_EQ(results[8].damage, results[j].damage);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust does not stack the sand Special Defense bonus on an actual Rock type", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_DUST_TO_DUST; }
    GIVEN {
        PLAYER(SPECIES_RHYHORN) { Ability(ability); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_SANDSLASH) { SpAttack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN { MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust does not grant Rock typing, weaknesses, or resistances", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_BRICK_BREAK; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_BRICK_BREAK; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Ability(ABILITY_SAND_VEIL); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_0, TYPE_GROUND));
        EXPECT(!IS_BATTLER_OF_TYPE(B_BATTLER_0, TYPE_ROCK));
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust sand Special Defense bonus is suppressed by Cloud Nine and Air Lock", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_CLOUD_NINE; }
    PARAMETRIZE { ability = ABILITY_AIR_LOCK; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_SANDSLASH) { Ability(ability); SpAttack(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN { MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Dust to Dust respects primal weather and overworld weather restrictions")
{
    bool32 primal;
    PARAMETRIZE { primal = FALSE; }
    PARAMETRIZE { primal = TRUE; }
    ResetStartingStatuses();
    if (!primal) SetStartingStatus(STARTING_STATUS_WEATHER_SUN);
    GIVEN {
        WITH_CONFIG(B_OVERWORLD_WEATHER_OVERRIDE, GEN_9);
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GROUDON) { Ability(primal ? ABILITY_DESOLATE_LAND : ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SHOT); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SANDSTORM));
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}
