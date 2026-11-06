#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Endless Luau is Bellossom's Apex ability only while holding its Meteorite")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(item); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_ENDLESS_LUAU, item == ITEM_APEX_METEORITE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE));
    }
}

SINGLE_BATTLE_TEST("Endless Luau copies an unknown dance immediately through its Dancer component")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUIVER_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NOT ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Endless Luau can copy damaging dances without knowing a dance move")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PETAL_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PETAL_DANCE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PETAL_DANCE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Endless Luau uses one known dance after normal actions on every turn without spending PP")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); MovesWithPP({MOVE_CELEBRATE, 5}, {MOVE_QUIVER_DANCE, 5}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->pp[0], 3);
        EXPECT_EQ(player->pp[1], 5);
    }
}

SINGLE_BATTLE_TEST("Endless Luau chooses equally among known dancing moves and excludes other moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SWORDS_DANCE; }
    PARAMETRIZE { move = MOVE_QUIVER_DANCE; }
    PASSES_RANDOMLY(1, 2, RNG_ENDLESS_LUAU);
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_SWORDS_DANCE, MOVE_SCRATCH, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Endless Luau does nothing at turn end if its user knows no dancing moves")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        NOT ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
    }
}

SINGLE_BATTLE_TEST("Endless Luau can call a known dance with zero PP")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); MovesWithPP({MOVE_CELEBRATE, 5}, {MOVE_QUIVER_DANCE, 0}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
    } THEN {
        EXPECT_EQ(player->pp[1], 0);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Endless Luau's automatic Petal Dance does not lock its user's next action")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); MovesWithPP({MOVE_SCRATCH, 5}, {MOVE_PETAL_DANCE, 5}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PETAL_DANCE, player);
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ((u32)player->volatiles.rampageTurns, 0);
        EXPECT_EQ(player->pp[0], 3);
        EXPECT_EQ(player->pp[1], 5);
    }
}

SINGLE_BATTLE_TEST("Endless Luau's automatic move preserves the user's ordinary move history")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_SCRATCH);
        EXPECT_EQ(gLastResultingMoves[B_BATTLER_0], MOVE_SCRATCH);
        EXPECT_EQ((u32)player->volatiles.usedMoves, 1);
    }
}

SINGLE_BATTLE_TEST("Endless Luau respects Taunt when its random dance is a status move")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_SCRATCH, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAUNT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Endless Luau respects Disable without rerolling a blocked dance")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_DISABLE; }
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (move == MOVE_DISABLE ? 2 : 4));
    }
}

SINGLE_BATTLE_TEST("Endless Luau cannot perform its dance while asleep")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_SLEEP); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
    } THEN {
        EXPECT(player->status1 & STATUS1_SLEEP);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Endless Luau cannot activate after residual damage knocks its user out")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); HP(1); Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player, hp: 0);
        MESSAGE("Bellossom fainted!");
        NOT ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
    }
}

SINGLE_BATTLE_TEST("Endless Luau can copy a dance and perform a different known dance in the same turn")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Endless Luau's automatic dance triggers a finite Dancer chain in allies and opponents")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_SWORDS_DANCE); }
        PLAYER(SPECIES_ORICORIO) { Ability(ABILITY_DANCER); }
        OPPONENT(SPECIES_ORICORIO) { Ability(ABILITY_DANCER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Endless Luau gives each holder one independent automatic move and lets them copy each other")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Endless Luau can KO an opponent at turn end and continue normally after a replacement")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); SpAttack(4000); Moves(MOVE_CELEBRATE, MOVE_FIERY_DANCE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_FLASH_FIRE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIERY_DANCE, player);
        HP_BAR(opponent, hp: 0);
        MESSAGE("The opposing Magikarp fainted!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
    }
}

DOUBLE_BATTLE_TEST("Endless Luau copies an ally's normal dance without needing a known dance of its own")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_DRAGON_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_DANCE, playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_DANCE, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Endless Luau can use Lunar Dance at turn end and heal its replacement")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_LUNAR_DANCE); }
        PLAYER(SPECIES_WYNAUT) { HP(1); Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENDLESS_LUAU);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LUNAR_DANCE, player);
        MESSAGE("Bellossom fainted!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
