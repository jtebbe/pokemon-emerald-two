#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Auroral Herald belongs only to Alolan Vulpix and Ninetales")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_VULPIX_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VULPIX; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES_ALOLA; item = ITEM_NONE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); }
    } WHEN {
        TURN {}
    } THEN {
        bool32 herald = item == ITEM_APEX_METEORITE && (species == SPECIES_VULPIX_ALOLA || species == SPECIES_NINETALES_ALOLA);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_AURORAL_HERALD, herald);
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL), herald);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald sets a five-turn Veil in hail or snow after weather abilities regardless of Speed")
{
    u32 gen = GEN_9, speed = 1;
    PARAMETRIZE { gen = GEN_8; speed = 1; }
    PARAMETRIZE { gen = GEN_8; speed = 1000; }
    PARAMETRIZE { gen = GEN_9; speed = 1; }
    PARAMETRIZE { gen = GEN_9; speed = 1000; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, gen);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(speed); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); Speed(100); }
    } WHEN {
        TURN {}
    } SCENE {
        ITEM_POPUP(player, ITEM_APEX_METEORITE);
        ABILITY_POPUP(opponent, ABILITY_SNOW_WARNING);
        ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
        MESSAGE("Aurora Veil made your side stronger against physical and special moves!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].auroraVeilTimer, 4);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].auroraVeilTimerTotal, 5);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].auroraVeilTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald cannot set Veil without effective hail or snow")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_DRIZZLE; }
    PARAMETRIZE { ability = ABILITY_DROUGHT; }
    PARAMETRIZE { ability = ABILITY_SAND_STREAM; }
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL));
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].auroraVeilTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald cannot set Veil while Cloud Nine or Air Lock suppresses icy weather")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CLOUD_NINE; }
    PARAMETRIZE { ability = ABILITY_AIR_LOCK; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(playerLeft, ABILITY_AURORAL_HERALD);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL));
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not refresh an existing Veil and can set it again on reentry after expiration")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_VULPIX_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); Item(ITEM_ICY_ROCK); }
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
        TURN {}
        TURN {}
        TURN { SWITCH(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
        MESSAGE("Your side's Aurora Veil wore off!");
        ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].auroraVeilTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not automatically set Veil when weather changes after its entry")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SNOWSCAPE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL));
    }
}

SINGLE_BATTLE_TEST("Auroral Herald strengthens both physical and special attacks by 1.3x", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    enum Item item = ITEM_NONE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = moves[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald strengthens its current teammate and does not stack with a second holder", s16 damage)
{
    bool32 partnerHerald, attackerHerald;
    PARAMETRIZE { partnerHerald = FALSE; attackerHerald = FALSE; }
    PARAMETRIZE { partnerHerald = TRUE; attackerHerald = FALSE; }
    PARAMETRIZE { partnerHerald = TRUE; attackerHerald = TRUE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_VULPIX_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(partnerHerald ? ITEM_APEX_METEORITE : ITEM_NONE); }
        PLAYER(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(attackerHerald ? ITEM_APEX_METEORITE : ITEM_NONE); Attack(300); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not boost attacks without Veil", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not multiply fixed damage")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald gives attacks a twenty percent frostbite chance even with freezing configured")
{
    PASSES_RANDOMLY(1, 5, RNG_AURORAL_HERALD);
    GIVEN {
        WITH_CONFIG(B_USE_FROSTBITE, FALSE);
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
        MESSAGE("The opposing Wobbuffet got frostbite!");
        STATUS_ICON(opponent, frostbite: TRUE);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald gives teammates noncontact attacks a frostbite chance and credits the holder")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_AURORAL_HERALD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_AURORAL_HERALD);
        STATUS_ICON(opponentLeft, frostbite: TRUE);
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_FROSTBITE);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald respects frostbite immunities existing status Shield Dust and Covert Cloak")
{
    enum Species species = SPECIES_WOBBUFFET;
    enum Ability ability = ABILITY_NONE;
    enum Item item = ITEM_NONE;
    u32 status = STATUS1_NONE;
    PARAMETRIZE { species = SPECIES_GLACEON; ability = ABILITY_NONE; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_MAGMA_ARMOR; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_COMATOSE; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; status = STATUS1_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_NONE; status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); Status1(status); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNOWSCAPE); }
        TURN { MOVE(player, MOVE_AURORA_VEIL); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not inflict frostbite through misses Protect immunity or Substitute")
{
    enum Move defense;
    PARAMETRIZE { defense = MOVE_PROTECT; }
    PARAMETRIZE { defense = MOVE_SUBSTITUTE; }
    PARAMETRIZE { defense = MOVE_TRICK_OR_TREAT; }
    PARAMETRIZE { defense = MOVE_CELEBRATE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
    } WHEN {
        if (defense == MOVE_SUBSTITUTE) TURN { MOVE(opponent, defense); }
        if (defense == MOVE_TRICK_OR_TREAT) TURN { MOVE(player, defense); }
        TURN {
            if (defense == MOVE_PROTECT) MOVE(opponent, defense);
            MOVE(player, defense == MOVE_CELEBRATE ? MOVE_THUNDER : MOVE_SCRATCH, hit: defense != MOVE_CELEBRATE, WITH_RNG(RNG_AURORAL_HERALD, TRUE));
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald stops buffing a teammate when the holder switches out or faints", s16 damage)
{
    bool32 faint;
    s16 first;
    PARAMETRIZE { faint = FALSE; }
    PARAMETRIZE { faint = TRUE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(300); Speed(200); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft, criticalHit: FALSE); }
        if (faint) TURN { MOVE(opponentRight, MOVE_SCRATCH, target: playerLeft); SEND_OUT(playerLeft, 2); }
        else TURN { SWITCH(playerLeft, 2); }
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &first);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL);
        EXPECT_MUL_EQ(results[i].damage, UQ_4_12(1.3), first);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald's buffs end immediately when Aurora Veil is broken", s16 damage)
{
    bool32 broken;
    PARAMETRIZE { broken = FALSE; }
    PARAMETRIZE { broken = TRUE; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Attack(300); }
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { if (broken) MOVE(opponent, MOVE_BRICK_BREAK); }
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.3), results[0].damage);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald frostbite remains a twenty percent ability effect with Serene Grace or Sheer Force")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SERENE_GRACE; }
    PARAMETRIZE { ability = ABILITY_SHEER_FORCE; }
    PASSES_RANDOMLY(1, 5, RNG_AURORAL_HERALD);
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(ITEM_LIFE_ORB); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_WATER_PULSE, target: opponentLeft); }
    } SCENE {
        STATUS_ICON(opponentLeft, frostbite: TRUE);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald can frostbite both opponents with a spread attack")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_HYPER_VOICE, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_FROSTBITE);
        EXPECT_EQ(opponentRight->status1, STATUS1_FROSTBITE);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald checks each hit of a multistrike attack")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); Item(ITEM_LUM_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_HIT, player);
        STATUS_ICON(opponent, frostbite: TRUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        STATUS_ICON(opponent, none: TRUE);
        STATUS_ICON(opponent, frostbite: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_FROSTBITE);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald respects Safeguard Misty Terrain and sunlight while Veil remains active")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_MISTY_TERRAIN; }
    PARAMETRIZE { protection = MOVE_SUNNY_DAY; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, protection); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Auroral Herald uses its teammate's Mold Breaker and Infiltrator for frostbite protections")
{
    enum Ability attackerAbility, defenderAbility;
    enum Move protection;
    PARAMETRIZE { attackerAbility = ABILITY_MOLD_BREAKER; defenderAbility = ABILITY_SHIELD_DUST; protection = MOVE_CELEBRATE; }
    PARAMETRIZE { attackerAbility = ABILITY_MOLD_BREAKER; defenderAbility = ABILITY_MAGMA_ARMOR; protection = MOVE_CELEBRATE; }
    PARAMETRIZE { attackerAbility = ABILITY_INFILTRATOR; defenderAbility = ABILITY_NONE; protection = MOVE_SAFEGUARD; }
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(attackerAbility); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(defenderAbility); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SNOWSCAPE); }
        TURN { MOVE(playerLeft, MOVE_AURORA_VEIL); MOVE(opponentLeft, protection); }
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); }
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_FROSTBITE);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald does not frostbite a target knocked out by the attack")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SNOW_WARNING); HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_AURORAL_HERALD, TRUE)); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        NOT STATUS_ICON(opponent, frostbite: TRUE);
    }
}

SINGLE_BATTLE_TEST("Auroral Herald cannot set Aurora Veil if entry hazards knock it out")
{
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_9);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_NINETALES_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(800); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ABOMASNOW) { Ability(ABILITY_SNOW_WARNING); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); SEND_OUT(player, 2); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AURORAL_HERALD);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_AURORA_VEIL));
    }
}
