#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Conduction Zone belongs only to the Alolan Geodude line")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GEODUDE_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRAVELER_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GEODUDE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRAVELER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM_ALOLA; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_CONDUCTION_ZONE,
            item == ITEM_APEX_METEORITE && (species == SPECIES_GEODUDE_ALOLA || species == SPECIES_GRAVELER_ALOLA || species == SPECIES_GOLEM_ALOLA));
    }
}

SINGLE_BATTLE_TEST("Conduction Zone creates Electric Terrain on entry for the normal duration")
{
    GIVEN {
        PLAYER(SPECIES_GEODUDE_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CONDUCTION_ZONE);
        MESSAGE("An electric current ran across the battlefield!");
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gFieldTimers.terrainTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone respects Terrain Extender when its ability is supplied independently")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CONDUCTION_ZONE); Item(ITEM_TERRAIN_EXTENDER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gFieldTimers.terrainTimer, 7);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone sets Electric Terrain again on reentry and overwrites other terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone guarantees paralysis for physical and special Electric attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SPARK; }
    PARAMETRIZE { move = MOVE_THUNDERBOLT; }
    PARAMETRIZE { move = MOVE_SHOCK_WAVE; }
    PARAMETRIZE { move = MOVE_THUNDER_SHOCK; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (move == MOVE_SHOCK_WAVE) MOVE(player, move);
            else MOVE(player, move, secondaryEffect: FALSE);
        }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet is paralyzed, so it may be unable to move!");
        STATUS_ICON(opponent, paralysis: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone ignores whether the user or target is grounded")
{
    bool32 rise;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { rise = FALSE; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { rise = TRUE; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { rise = FALSE; ability = ABILITY_LEVITATE; item = ITEM_NONE; }
    PARAMETRIZE { rise = TRUE; ability = ABILITY_LEVITATE; item = ITEM_NONE; }
    PARAMETRIZE { rise = TRUE; ability = ABILITY_NONE; item = ITEM_AIR_BALLOON; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, rise ? MOVE_MAGNET_RISE : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone paralyzes Flying targets without changing their grounding")
{
    GIVEN {
        PLAYER(SPECIES_GEODUDE_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_TOGEKISS) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
        EXPECT(!IsBattlerGrounded(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)));
    }
}

SINGLE_BATTLE_TEST("Conduction Zone adds paralysis to successful targeted Electric status moves")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_SANDSLASH; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_EERIE_IMPULSE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone does not paralyze with self buffs or terrain moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CHARGE; }
    PARAMETRIZE { move = MOVE_ELECTRIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_MAGNETIC_FLUX; }
    PARAMETRIZE { move = MOVE_TACKLE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone cannot add paralysis after Electric Terrain is replaced or expires")
{
    bool32 expire;
    PARAMETRIZE { expire = FALSE; }
    PARAMETRIZE { expire = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (expire) {
            for (u32 turn = 0; turn < 5; turn++) TURN {}
        } else {
            TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_NE((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone respects status immunity abilities and Electric typing")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_LIMBER; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_COMATOSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { species = SPECIES_PIKACHU; ability = ABILITY_NONE; }
    GIVEN {
        WITH_CONFIG(B_PARALYZE_ELECTRIC, GEN_6);
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone never replaces an existing nonvolatile status")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone respects Shield Dust and Covert Cloak for damaging and status moves")
{
    enum Move move;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { move = MOVE_SHOCK_WAVE; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SHOCK_WAVE; ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { move = MOVE_EERIE_IMPULSE; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_EERIE_IMPULSE; ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        if (move == MOVE_EERIE_IMPULSE)
            EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone is blocked by Safeguard")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SAFEGUARD); }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone does not paralyze through a Substitute or Protect")
{
    bool32 substitute;
    PARAMETRIZE { substitute = FALSE; }
    PARAMETRIZE { substitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (substitute) TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SHOCK_WAVE); MOVE(opponent, substitute ? MOVE_CELEBRATE : MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone does not paralyze immune or absorbing targets")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_SANDSLASH; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_VOLT_ABSORB; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_LIGHTNING_ROD; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_MOTOR_DRIVE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone does not paralyze a target when the move misses or KOs it")
{
    bool32 knockout;
    PARAMETRIZE { knockout = FALSE; }
    PARAMETRIZE { knockout = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(knockout ? 1 : 3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (knockout) { MOVE(player, MOVE_SHOCK_WAVE); SEND_OUT(opponent, 1); }
            else MOVE(player, MOVE_THUNDER, WITH_RNG(RNG_ACCURACY, FALSE));
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone paralysis can be cured immediately by Cheri or Lum Berry")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_CHERI_BERRY; }
    PARAMETRIZE { item = ITEM_LUM_BERRY; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone preserves the Electric move's original secondary effects")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTROWEB); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone paralysis can be reflected by Synchronize")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CONDUCTION_ZONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SYNCHRONIZE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

DOUBLE_BATTLE_TEST("Conduction Zone paralyzes every eligible target of Discharge including its ally")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_LEVITATE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DISCHARGE, secondaryEffect: FALSE); }
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponentLeft->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponentRight->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone preserves self-targeted secondary effects of Electric attacks")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHARGE_BEAM, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Conduction Zone paralyzes on Electro Shot's attacking turn rather than its charging turn")
{
    bool32 release;
    PARAMETRIZE { release = FALSE; }
    PARAMETRIZE { release = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRO_SHOT); }
        if (release) TURN { SKIP_TURN(player); }
    } THEN {
        EXPECT_EQ(opponent->status1, release ? STATUS1_PARALYSIS : STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}
