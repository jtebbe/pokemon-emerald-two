#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Mud Bath replaces only the secondary type and heals one third of max HP")
{
    enum Species species;
    u32 maxHp;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; maxHp = 300; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; maxHp = 301; }
    GIVEN {
        PLAYER(species) { HP(50); MaxHP(maxHp); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_BATH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_BATH, player);
        HP_BAR(player, hp: 150);
    } THEN {
        EXPECT_EQ(player->types[0], species == SPECIES_WOBBUFFET ? TYPE_PSYCHIC : TYPE_FIRE);
        EXPECT_EQ(player->types[1], TYPE_GROUND);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Mud Bath succeeds at full HP and caps healing at maximum HP")
{
    u32 hp;
    PARAMETRIZE { hp = 250; }
    PARAMETRIZE { hp = 300; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_BATH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_BATH, player);
        if (hp < 300)
            HP_BAR(player, hp: 300);
        else
            NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->types[1], TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("Mud Bath cures non-volatile status while healing")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_FROSTBITE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(300); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_BATH, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        STATUS_ICON(player, none: TRUE);
        HP_BAR(player, hp: 150);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Mud Bath called by Sleep Talk cures sleep")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(300); Status1(STATUS1_SLEEP); Moves(MOVE_SLEEP_TALK, MOVE_MUD_BATH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SLEEP_TALK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_BATH, player);
        STATUS_ICON(player, none: TRUE);
        HP_BAR(player, hp: 150);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Mud Bath fails atomically if Ground-typed or Terastallized")
{
    bool32 ground;
    PARAMETRIZE { ground = FALSE; }
    PARAMETRIZE { ground = TRUE; }
    GIVEN {
        PLAYER(ground ? SPECIES_SANDSHREW : SPECIES_WOBBUFFET) { HP(50); MaxHP(300); Status1(STATUS1_BURN); TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_BATH, gimmick: ground ? GIMMICK_NONE : GIMMICK_TERA); }
    } SCENE {
        MESSAGE("But it failed!");
        NOT STATUS_ICON(player, none: TRUE);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_BURN);
        EXPECT_EQ(player->types[1], ground ? TYPE_GROUND : TYPE_PSYCHIC);
        EXPECT_LT(player->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Mud Bath is blocked by Heal Block without changing types or curing status")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(50); MaxHP(300); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_MUD_BATH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_BATH, player);
    } THEN {
        EXPECT_EQ(player->types[1], TYPE_PSYCHIC);
        EXPECT_EQ(player->status1, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Mud Bath's type change persists until switching out")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_BATH); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_FIRE);
        EXPECT_EQ(player->types[1], TYPE_FLYING);
    }
}

SINGLE_BATTLE_TEST("Mud Bath preserves an added third type")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FORESTS_CURSE); }
        TURN { MOVE(player, MOVE_MUD_BATH); }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_FIRE);
        EXPECT_EQ(player->types[1], TYPE_GROUND);
        EXPECT_EQ(player->types[2], TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Gemshot hits twice only with Stealth Rock on the opposing side")
{
    u32 hazards;
    PARAMETRIZE { hazards = 0; }
    PARAMETRIZE { hazards = 1; }
    PARAMETRIZE { hazards = 2; }
    ResetStartingStatuses();
    if (hazards == 1)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_OPPONENT);
    if (hazards == 2)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_GEMSHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
        HP_BAR(opponent);
        if (hazards == 1) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
            HP_BAR(opponent);
            MESSAGE("The Pokémon was hit 2 times!");
        } else {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
        }
    }
}

DOUBLE_BATTLE_TEST("Gemshot hits both foes once or twice and never hits its ally")
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    ResetStartingStatuses();
    if (rocks)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_OPPONENT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_GEMSHOT); }
    } SCENE {
        for (u32 j = 0; j < (rocks ? 2 : 1); j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, playerLeft);
            HP_BAR(opponentLeft);
            HP_BAR(opponentRight);
        }
        NOT HP_BAR(playerRight);
    }
}

SINGLE_BATTLE_TEST("Gemshot receives Tera's weak-move power floor only when it hits once")
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    ResetStartingStatuses();
    if (rocks)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_OPPONENT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(100); SpAttack(300); TeraType(TYPE_ROCK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_GEMSHOT, gimmick: GIMMICK_TERA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gBattleMovePower, rocks ? 45 : 60);
    }
}

SINGLE_BATTLE_TEST("Gemshot sees Stealth Rock being set or removed earlier that turn")
{
    bool32 remove;
    PARAMETRIZE { remove = FALSE; }
    PARAMETRIZE { remove = TRUE; }
    ResetStartingStatuses();
    if (remove)
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_OPPONENT);
    else
        SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, remove ? MOVE_RAPID_SPIN : MOVE_COURT_CHANGE); MOVE(player, MOVE_GEMSHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
        HP_BAR(opponent);
        if (remove) {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GEMSHOT, player);
            HP_BAR(opponent);
        }
    }
}

DOUBLE_BATTLE_TEST("Harsh Zephyr hits only its selected foe without Tailwind and both foes with Tailwind")
{
    bool32 tailwind;
    PARAMETRIZE { tailwind = FALSE; }
    PARAMETRIZE { tailwind = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (tailwind) { MOVE(playerRight, MOVE_TAILWIND); }
            MOVE(playerLeft, MOVE_HARSH_ZEPHYR, target: opponentRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HARSH_ZEPHYR, playerLeft);
        if (tailwind) {
            HP_BAR(opponentLeft);
        }
        HP_BAR(opponentRight);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], tailwind ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}

DOUBLE_BATTLE_TEST("The opponent's Tailwind does not spread Harsh Zephyr")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(200); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_TAILWIND); MOVE(playerLeft, MOVE_HARSH_ZEPHYR, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Harsh Zephyr's guaranteed Speed drop respects Contrary, Clear Body and Covert Cloak")
{
    u32 protection;
    PARAMETRIZE { protection = 0; }
    PARAMETRIZE { protection = 1; }
    PARAMETRIZE { protection = 2; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(protection == 0 ? ABILITY_CONTRARY : protection == 1 ? ABILITY_CLEAR_BODY : ABILITY_SHADOW_TAG);
            Item(protection == 2 ? ITEM_COVERT_CLOAK : ITEM_NONE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_HARSH_ZEPHYR); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (protection == 0));
    }
}
