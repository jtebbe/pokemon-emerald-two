#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Noblesse Oblige is the Apex ability of Galarian Farfetch'd and Sirfetch'd")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_FARFETCHD_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SIRFETCHD; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SIRFETCHD; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_FARFETCHD; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(0) == ABILITY_NOBLESSE_OBLIGE, species != SPECIES_FARFETCHD && item == ITEM_APEX_METEORITE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige raises only its ally's Defense and Sp Def on entry")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_NOBLESSE_OBLIGE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++) {
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentLeft->statStages[stat], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->statStages[stat], DEFAULT_STAT_STAGE);
        }
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige's entry boost respects Simple and Contrary and bypasses an ally's Substitute")
{
    enum Ability ability;
    s32 change;
    PARAMETRIZE { ability = ABILITY_NONE; change = 1; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; change = 2; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; change = -1; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; change = 1; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        PLAYER(SPECIES_FARFETCHD_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SUBSTITUTE); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_NOBLESSE_OBLIGE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + change);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + change);
        EXPECT(playerRight->volatiles.substitute);
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige triggers once per entry and can trigger again after switching")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
        TURN {}
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige skips capped stats independently and does not announce when both are capped")
{
    bool32 both;
    PARAMETRIZE { both = FALSE; }
    PARAMETRIZE { both = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 j = 0; j < 6; j++) TURN { MOVE(playerRight, both ? MOVE_COSMIC_POWER : MOVE_IRON_DEFENSE); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        if (both) NOT ABILITY_POPUP(playerLeft, ABILITY_NOBLESSE_OBLIGE);
        else ABILITY_POPUP(playerLeft, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_DEF], MAX_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], both ? MAX_STAT_STAGE : DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige gives exactly two critical stages across generations")
{
    u32 gen = GEN_9, denominator = 2;
    for (u32 j = GEN_1; j <= GEN_9; j++)
        PARAMETRIZE { gen = j; denominator = j >= GEN_2 && j <= GEN_5 ? 4 : 2; }
    PASSES_RANDOMLY(1, denominator, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); } } SCENE { MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Noblesse Oblige stacks its two critical stages with high-crit moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SLASH; }
    PARAMETRIZE { move = MOVE_AIR_CUTTER; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_FARFETCHD_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
        ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige lowers both defenses only after a damaging physical or special critical hit")
{
    bool32 crit;
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; crit = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; crit = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; crit = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; crit = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: crit); } } SCENE {
        HP_BAR(opponent);
        if (crit) {
            MESSAGE("A critical hit!");
            ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (crit ? 2 : 0));
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (crit ? 2 : 0));
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige waits for the whole multi-hit move and lowers each defense once")
{
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_HIT, criticalHit: TRUE); } } SCENE {
        HP_BAR(opponent, captureDamage: &damage[0]);
        HP_BAR(opponent, captureDamage: &damage[1]);
        ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(damage[0], damage[1]);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige handles each critical recipient of a spread move independently")
{
    bool32 protected;
    PARAMETRIZE { protected = FALSE; }
    PARAMETRIZE { protected = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(protected ? ABILITY_BATTLE_ARMOR : ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_SWIFT, criticalHit: TRUE); } } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        ABILITY_POPUP(playerLeft, ABILITY_NOBLESSE_OBLIGE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        if (!protected) ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (protected ? 0 : 2));
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (protected ? 0 : 2));
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige's defense drops respect stat protection and Contrary but ignore secondary-effect protection")
{
    enum Ability ability;
    enum Item item = ITEM_NONE;
    s32 change = -2;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; change = 0; }
    PARAMETRIZE { ability = ABILITY_WHITE_SMOKE; item = ITEM_NONE; change = 0; }
    PARAMETRIZE { ability = ABILITY_FULL_METAL_BODY; item = ITEM_NONE; change = 0; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; change = 0; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; item = ITEM_NONE; change = 2; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; item = ITEM_NONE; change = -4; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; change = -2; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; change = -2; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SLASH, criticalHit: TRUE); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + change);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + change);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige is reflected by Mirror Armor")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MIRROR_ARMOR); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SLASH); } } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige cannot lower defenses through Mist or critical protection")
{
    enum Move setup;
    PARAMETRIZE { setup = MOVE_MIST; }
    PARAMETRIZE { setup = MOVE_LUCKY_CHANT; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, setup); MOVE(player, MOVE_SLASH); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige does not lower defenses from critical hits to a Substitute, including breaking it")
{
    u32 defense;
    PARAMETRIZE { defense = 1; }
    PARAMETRIZE { defense = 1000; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Defense(defense); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SLASH); } } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige applies its drops when a critical sound move bypasses Substitute")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_HYPER_VOICE, criticalHit: TRUE); } } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT(opponent->volatiles.substitute);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige does not trigger on status moves, fixed damage, misses, immunity, or Protect")
{
    enum Move move;
    enum Species species = SPECIES_WOBBUFFET;
    bool32 protect = FALSE, hit = TRUE;
    PARAMETRIZE { move = MOVE_SEISMIC_TOSS; species = SPECIES_WOBBUFFET; protect = FALSE; hit = TRUE; }
    PARAMETRIZE { move = MOVE_CELEBRATE; species = SPECIES_WOBBUFFET; protect = FALSE; hit = TRUE; }
    PARAMETRIZE { move = MOVE_THUNDER; species = SPECIES_WOBBUFFET; protect = FALSE; hit = FALSE; }
    PARAMETRIZE { move = MOVE_SLASH; species = SPECIES_GASTLY; protect = FALSE; hit = TRUE; }
    PARAMETRIZE { move = MOVE_SLASH; species = SPECIES_WOBBUFFET; protect = TRUE; hit = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, move, hit: hit, criticalHit: TRUE); } } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige clears its pending drops before the next attack")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: TRUE); }
        TURN { MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige still applies a landed critical hit's drops if recoil KOs its user")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_EDGE, criticalHit: TRUE); SEND_OUT(player, 1); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige does not lower a replacement's defenses after KOing its target")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { MOVE(player, MOVE_SLASH); SEND_OUT(opponent, 1); } } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_NOBLESSE_OBLIGE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Noblesse Oblige's two critical stages apply only to its holder")
{
    PASSES_RANDOMLY(1, 24, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft); } } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige stacks with a move's original defense-lowering effect")
{
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_CRUNCH, criticalHit: TRUE, secondaryEffect: TRUE); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 3);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Noblesse Oblige's stat drops trigger Defiant and Competitive")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_DEFIANT; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; }
    GIVEN {
        PLAYER(SPECIES_SIRFETCHD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SLASH); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_GT(opponent->statStages[ability == ABILITY_DEFIANT ? STAT_ATK : STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
