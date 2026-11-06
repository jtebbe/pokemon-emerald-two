#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Fists of Fury belongs to Mankey Primeape and Annihilape with an Apex Meteorite")
{
    enum Species species = SPECIES_MANKEY;
    enum Item item = ITEM_NONE;
    PARAMETRIZE { species = SPECIES_MANKEY; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PRIMEAPE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ANNIHILAPE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ANNIHILAPE; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Ability(ABILITY_VITAL_SPIRIT); Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_FISTS_OF_FURY); EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_APEX_METEORITE ? ABILITY_FISTS_OF_FURY : ABILITY_VITAL_SPIRIT); }
}

SINGLE_BATTLE_TEST("Fists of Fury boosts punching damage by fifty percent rather than Iron Fist's twenty percent", s16 damage)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_BULLET_PUNCH; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_BULLET_PUNCH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Ability(ABILITY_VITAL_SPIRIT); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); EXPECT_EQ(results[2].damage, results[3].damage); }
}

SINGLE_BATTLE_TEST("Fists of Fury raises Speed only after punching damage and secondary effects resolve")
{
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_POWER_UP_PUNCH); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_UP_PUNCH, player);
        HP_BAR(opponent);
        MESSAGE("Mankey's Attack rose!");
        ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY);
        MESSAGE("Mankey's Speed rose!");
    } THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Fists of Fury bypasses accuracy drops evasion boosts and evasion items")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_BRIGHT_POWDER; }
    PARAMETRIZE { item = ITEM_LAX_INCENSE; }
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_MEGA_PUNCH, hit: FALSE); }
        TURN { MOVE(opponent, MOVE_SAND_ATTACK); MOVE(player, MOVE_MEGA_PUNCH, hit: FALSE); }
    } SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        struct BattleCalcValues cv = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_MEGA_PUNCH, .abilities[B_BATTLER_0] = ABILITY_FISTS_OF_FURY };
        EXPECT(CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        opponent->hp = 0;
        EXPECT(!CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
    }
}

SINGLE_BATTLE_TEST("Fists of Fury does not improve accuracy for non-punching moves or incoming punches")
{
    bool32 incoming;
    PARAMETRIZE { incoming = FALSE; }
    PARAMETRIZE { incoming = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { if (incoming) TURN { MOVE(opponent, MOVE_DYNAMIC_PUNCH, hit: FALSE); } else TURN { MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); } }
    SCENE { NONE_OF { HP_BAR(player); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Fists of Fury cannot hit semi-invulnerable targets but still raises Speed after use")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_DIG; }
    PARAMETRIZE { move = MOVE_DIVE; }
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(opponent, move); MOVE(player, MOVE_MEGA_PUNCH); } }
    SCENE { NOT HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Fists of Fury respects Protect and type immunity while still boosting Speed after use")
{
    bool32 ghost;
    PARAMETRIZE { ghost = FALSE; }
    PARAMETRIZE { ghost = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(ghost ? SPECIES_GASTLY : SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_MEGA_PUNCH); if (!ghost) MOVE(opponent, MOVE_PROTECT); } }
    SCENE { NOT HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Fists of Fury does not boost Speed if sleep paralysis flinching or Focus Punch interruption prevents use")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    PARAMETRIZE { block = 3; }
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); Status1(block == 0 ? STATUS1_SLEEP : block == 1 ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, block == 3 ? MOVE_FOCUS_PUNCH : MOVE_MEGA_PUNCH, WITH_RNG(RNG_PARALYSIS, TRUE)); if (block == 2) MOVE(opponent, MOVE_FAKE_OUT); if (block == 3) MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Fists of Fury boosts Speed once after all hits of Double Iron Bash")
{
    GIVEN {
        ASSUME(IsPunchingMove(MOVE_DOUBLE_IRON_BASH));
        PLAYER(SPECIES_ANNIHILAPE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_IRON_BASH, secondaryEffect: FALSE); } }
    SCENE { HP_BAR(opponent); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Fists of Fury repeatedly boosts Speed up to the cap and then stops showing its popup")
{
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); }
    } WHEN { for (u32 turn = 0; turn < 7; turn++) TURN { MOVE(player, MOVE_BULLET_PUNCH); } }
    SCENE { for (u32 turn = 0; turn < 6; turn++) { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); } HP_BAR(opponent); NOT ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], MAX_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Fists of Fury works through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Item(ITEM_APEX_METEORITE); Speed(1); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_MEGA_PUNCH); } }
    SCENE { SUB_HIT(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Fists of Fury boosts punching moves called by Metronome")
{
    GIVEN { PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_METRONOME, WITH_RNG(RNG_METRONOME, MOVE_MEGA_PUNCH)); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

DOUBLE_BATTLE_TEST("Fists of Fury boosts each punching move used again through Instruct")
{
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_ORANGURU) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_MEGA_PUNCH, target: opponentLeft); MOVE(playerRight, MOVE_INSTRUCT, target: playerLeft); } }
    SCENE { HP_BAR(opponentLeft); ABILITY_POPUP(playerLeft, ABILITY_FISTS_OF_FURY); HP_BAR(opponentLeft); ABILITY_POPUP(playerLeft, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("Fists of Fury cannot boost an attacker that faints to Rocky Helmet")
{
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Item(ITEM_APEX_METEORITE); HP(1); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); HP(3000); MaxHP(3000); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_MEGA_PUNCH); SEND_OUT(player, 1); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
}

SINGLE_BATTLE_TEST("Fists of Fury raises Speed after Drain Punch's healing")
{
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DRAIN_PUNCH); } }
    SCENE { HP_BAR(opponent); HP_BAR(player); ABILITY_POPUP(player, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

DOUBLE_BATTLE_TEST("Fists of Fury still boosts after using a punch against an ally")
{
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { MOVE(playerLeft, MOVE_MEGA_PUNCH, target: playerRight); } }
    SCENE { HP_BAR(playerRight); ABILITY_POPUP(playerLeft, ABILITY_FISTS_OF_FURY); }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}
