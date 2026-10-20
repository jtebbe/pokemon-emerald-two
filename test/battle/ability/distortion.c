#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "main.h"

SINGLE_BATTLE_TEST("Distortion activates only while either Giratina form holds an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GIRATINA_ALTERED; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_GIRATINA_ALTERED; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GIRATINA_ORIGIN; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_GIRATINA_ORIGIN; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(IsDistortionActive(), item == ITEM_APEX_METEORITE);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), item == ITEM_APEX_METEORITE ? DAMAGE_CATEGORY_SPECIAL : DAMAGE_CATEGORY_PHYSICAL);
    }
}

SINGLE_BATTLE_TEST("Distortion restores type-based categories rather than inverting move categories")
{
    enum Move move;
    enum DamageCategory category;
    PARAMETRIZE { move = MOVE_HYPER_VOICE; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_AURA_SPHERE; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_AIR_SLASH; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_POWER_GEM; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_SHADOW_BALL; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_FLASH_CANNON; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_FIRE_PUNCH; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_WATERFALL; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_LEAF_BLADE; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_THUNDER_PUNCH; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_ICE_PUNCH; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_PSYCHO_CUT; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_DRAGON_CLAW; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_BITE; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_PLAY_ROUGH; category = DAMAGE_CATEGORY_SPECIAL; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; category = DAMAGE_CATEGORY_PHYSICAL; }
    PARAMETRIZE { move = MOVE_FLAMETHROWER; category = DAMAGE_CATEGORY_SPECIAL; }
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattleMoveCategory(move), category);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_SWORDS_DANCE), DAMAGE_CATEGORY_STATUS);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_THUNDER_WAVE), DAMAGE_CATEGORY_STATUS);
    }
}

DOUBLE_BATTLE_TEST("Distortion changes the offensive stat used for both physical and special moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_SHADOW_BALL; }
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(move == MOVE_BITE ? 40 : 240); SpAttack(move == MOVE_BITE ? 240 : 40); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(playerRight, move, target: opponentLeft, criticalHit: FALSE, secondaryEffect: FALSE); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { MOVE(playerRight, move, target: opponentLeft, criticalHit: FALSE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &damage[0]);
        HP_BAR(opponentLeft, captureDamage: &damage[1]);
    } THEN {
        EXPECT_GT(damage[0], damage[1] * 3);
        EXPECT(!IsDistortionActive());
    }
}

DOUBLE_BATTLE_TEST("Distortion is global and multiple holders do not cancel each other")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_GIRATINA_ORIGIN) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN {}
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT(IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_SHADOW_BALL), DAMAGE_CATEGORY_PHYSICAL);
    }
}

DOUBLE_BATTLE_TEST("Distortion ends immediately after the last holder faints")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); HP(1); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_ICE_BEAM, target: playerLeft); SEND_OUT(playerLeft, 2); }
    } SCENE {
        MESSAGE("Giratina fainted!");
    } THEN {
        EXPECT(!IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_SHADOW_BALL), DAMAGE_CATEGORY_SPECIAL);
    }
}

DOUBLE_BATTLE_TEST("Distortion preserves the resolved category of a spread hit that KOs its holder")
{
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); HP(1); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(50); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_ASTRAL_BARRAGE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_COUNTER, target: opponentLeft);
            SEND_OUT(playerLeft, 2);
        }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &damage[0]);
        HP_BAR(opponentLeft, captureDamage: &damage[1]);
    } THEN {
        EXPECT_EQ(damage[1], damage[0] * 2);
        EXPECT(!IsDistortionActive());
    }
}

SINGLE_BATTLE_TEST("Distortion keeps stat-adaptive attacks type-based and leaves status moves alone")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); Attack(200); SpAttack(50); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        SetDynamicMoveCategory(B_BATTLER_0, B_BATTLER_1, MOVE_PHOTON_GEYSER);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_PHOTON_GEYSER), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_CELEBRATE), DAMAGE_CATEGORY_STATUS);
        gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;
    }
}

SINGLE_BATTLE_TEST("Distortion uses a type-changing move's resolved type without changing contact flags")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        SetDynamicMoveCategory(B_BATTLER_0, B_BATTLER_1, MOVE_WEATHER_BALL);
        gBattleStruct->dynamicMoveType = TYPE_FIRE;
        EXPECT_EQ(GetBattleMoveCategory(MOVE_WEATHER_BALL), DAMAGE_CATEGORY_SPECIAL);
        gBattleStruct->dynamicMoveType = TYPE_ROCK;
        EXPECT_EQ(GetBattleMoveCategory(MOVE_WEATHER_BALL), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_FLAMETHROWER), DAMAGE_CATEGORY_SPECIAL);
        EXPECT(MoveMakesContact(MOVE_BITE));
        EXPECT(!MoveMakesContact(MOVE_SHADOW_BALL));
        gBattleStruct->dynamicMoveType = TYPE_NONE;
        gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;
    }
}

SINGLE_BATTLE_TEST("Distortion remains active through Neutralizing Gas Magic Room and Gastro Acid")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGIC_ROOM); MOVE(opponent, MOVE_GASTRO_ACID); }
    } THEN {
        EXPECT(IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_DISTORTION);
    }
}

SINGLE_BATTLE_TEST("Distortion's type-based categories drive burn and frostbite damage penalties")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_FROSTBITE; }
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_SHADOW_BALL };
        ctx.abilities[B_BATTLER_0] = ABILITY_DISTORTION;
        EXPECT_EQ(GetBurnOrFrostBiteModifier(&ctx), status == STATUS1_BURN ? UQ_4_12(0.5) : UQ_4_12(1.0));
        ctx.move = MOVE_BITE;
        EXPECT_EQ(GetBurnOrFrostBiteModifier(&ctx), status == STATUS1_FROSTBITE ? UQ_4_12(0.5) : UQ_4_12(1.0));
    }
}

SINGLE_BATTLE_TEST("Distortion cannot change categories outside battle or from an off-field holder")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        gBattleStruct->battlerState[B_BATTLER_0].notOnField = TRUE;
        EXPECT(!IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_PHYSICAL);
        gBattleStruct->battlerState[B_BATTLER_0].notOnField = FALSE;
        bool32 inBattle = gMain.inBattle;
        gMain.inBattle = FALSE;
        EXPECT(!IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_PHYSICAL);
        gMain.inBattle = inBattle;
    }
}
