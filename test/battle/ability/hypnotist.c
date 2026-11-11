#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hypnotist bypasses damaging and status accuracy rolls and drops defenses only for status moves")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2400); MaxHP(2400); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, hit: FALSE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ABILITY_POPUP(player, ABILITY_HYPNOTIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Hypnotist applies its defense drops to each reached target of a spread status move")
{
    bool32 protect = FALSE;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DROWZEE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            if (protect) MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(playerLeft, MOVE_GROWL);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - !protect);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - !protect);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Hypnotist also lowers an ally's defenses when healing that ally")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerRight); }
    } THEN {
        EXPECT_EQ(playerRight->hp, 200);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Hypnotist respects normal stat-drop protection and Contrary")
{
    enum Ability ability = ABILITY_CLEAR_BODY;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_BIG_PECKS; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (ability == ABILITY_CONTRARY));
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + (ability == ABILITY_CONTRARY ? 1 : ability == ABILITY_BIG_PECKS ? -1 : 0));
    }
}

SINGLE_BATTLE_TEST("Hypnotist does not lower defenses with self or field status moves or a hit blocked by Substitute")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT); MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
