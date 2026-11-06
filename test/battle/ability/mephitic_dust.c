#include "global.h"
#include "test/battle.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Mephitic Dust is the Apex ability of Venonat and Venomoth")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_VENONAT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VENOMOTH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VENONAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_VENOMOTH; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_MEPHITIC_DUST, item == ITEM_APEX_METEORITE); }
}

SINGLE_BATTLE_TEST("Mephitic Dust adds priority to physical special and status Poison moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_POISON_JAB; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; }
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_POISON_GAS; }
    PARAMETRIZE { move = MOVE_ACID_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN { EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_MEPHITIC_DUST, move), 1); }
}

SINGLE_BATTLE_TEST("Mephitic Dust does not increase priority of other move types")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_PSYBEAM; }
    PARAMETRIZE { move = MOVE_QUIVER_DANCE; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, secondaryEffect: FALSE); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust lowers priority of poisoned and badly poisoned opponents only")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_BURN; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} }
    SCENE {
        if (status & STATUS1_PSN_ANY) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
    } THEN { EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_CELEBRATE), status & STATUS1_PSN_ANY ? -1 : 0); }
}

SINGLE_BATTLE_TEST("Mephitic Dust changes priority rather than speed and preserves native priority brackets")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_ATTACK); }
        TURN { MOVE(player, MOVE_SLUDGE_BOMB, secondaryEffect: FALSE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_PROTECT), 3);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_ROAR), -7);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_PRANKSTER, MOVE_CELEBRATE), 0);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_TRIAGE, MOVE_RECOVER), 2);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust priority follows the move's dynamic Poison type")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_TOXIC_PLATE; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Ability(ABILITY_MEPHITIC_DUST); Item(item); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_JUDGMENT); } }
    SCENE {
        if (item == ITEM_TOXIC_PLATE) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_JUDGMENT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust Poison priority is blocked by Psychic Terrain and priority-blocking abilities")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PSYCHIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_DAZZLING; }
    PARAMETRIZE { ability = ABILITY_QUEENLY_MAJESTY; }
    PARAMETRIZE { ability = ABILITY_ARMOR_TAIL; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN { TURN { MOVE(player, MOVE_SLUDGE_BOMB); } }
    SCENE { NOT HP_BAR(opponent); }
    THEN { EXPECT_EQ(opponent->hp, opponent->maxHP); }
}

SINGLE_BATTLE_TEST("Mephitic Dust status priority does not inherit Prankster's Dark immunity")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_UMBREON) { Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_TOXIC); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, player);
        STATUS_ICON(opponent, badPoison: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

DOUBLE_BATTLE_TEST("Mephitic Dust penalizes both opponents regardless of their target but does not boost its partner's Poison moves")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_TOXIC_POISON); Speed(99); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight);
            MOVE(opponentRight, MOVE_SCRATCH, target: opponentLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentRight);
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_CELEBRATE), -1);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_3, ABILITY_NONE, MOVE_CELEBRATE), -1);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_2, ABILITY_NONE, MOVE_POISON_JAB), 0);
    }
}

DOUBLE_BATTLE_TEST("Mephitic Dust does not penalize its own poisoned allies")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(98); }
    } WHEN { TURN {} }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight); ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft); }
    THEN { EXPECT_EQ(GetBattleMovePriority(B_BATTLER_2, ABILITY_NONE, MOVE_CELEBRATE), 0); }
}

DOUBLE_BATTLE_TEST("Mephitic Dust's opponent priority penalty does not stack")
{
    GIVEN {
        PLAYER(SPECIES_VENONAT) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_TOXIC_POISON); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_CELEBRATE), -1);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_3, ABILITY_NONE, MOVE_CELEBRATE), -1);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust's two priority effects cancel against another Mephitic Dust holder")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SLUDGE_BOMB, secondaryEffect: FALSE); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponent); ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player); }
    THEN { EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_MEPHITIC_DUST, MOVE_SLUDGE_BOMB), 0); }
}

SINGLE_BATTLE_TEST("Mephitic Dust stops penalizing opponents after switching out")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        SEND_IN_MESSAGE("Wobbuffet");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN { EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_CELEBRATE), 0); }
}

DOUBLE_BATTLE_TEST("Mephitic Dust's aura ends when its last holder faints")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_SCRATCH, target: playerLeft); }
        TURN {}
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_NONE, MOVE_CELEBRATE), 0);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust stops penalizing cured opponents")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_REFRESH); } TURN {} }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFRESH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust priority outranks speed reversal in Trick Room")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(1); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_TRICK_ROOM); } TURN {} }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust respects speed and Trick Room within the increased priority bracket")
{
    bool32 trickRoom;
    PARAMETRIZE { trickRoom = FALSE; }
    PARAMETRIZE { trickRoom = TRUE; }
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        if (trickRoom) TURN { MOVE(player, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, MOVE_SLUDGE_BOMB, secondaryEffect: FALSE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        if (trickRoom) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, player);
        }
    }
}

SINGLE_BATTLE_TEST("Mephitic Dust preserves Grassy Glide's terrain priority")
{
    GIVEN {
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(player, MOVE_GRASSY_GLIDE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_GLIDE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
