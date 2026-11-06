#include "global.h"
#include "test/battle.h"
#include "battle_main.h"
#include "battle_util.h"
#include "move.h"

SINGLE_BATTLE_TEST("Succulent Trap is the Apex ability of the Bellsprout line")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_BELLSPROUT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WEEPINBELL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VICTREEBEL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VICTREEBEL_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VICTREEBEL; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SUCCULENT_TRAP, item == ITEM_APEX_METEORITE);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap lowers both foes' evasion but does not poison a newly negative stage or affect allies")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SUCCULENT_TRAP);
        MESSAGE("The opposing Wobbuffet's evasiveness fell!");
        MESSAGE("The opposing Wobbuffet's evasiveness fell!");
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap activates on every entry and normally poisons an already negative foe")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SUCCULENT_TRAP);
        MESSAGE("The opposing Wobbuffet's evasiveness fell!");
        ABILITY_POPUP(player, ABILITY_SUCCULENT_TRAP);
        MESSAGE("The opposing Wobbuffet's evasiveness fell!");
        MESSAGE("The opposing Wobbuffet was poisoned!");
        STATUS_ICON(opponent, poison: TRUE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT(!(opponent->status1 & STATUS1_TOXIC_POISON));
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap checks each foe's pre-entry evasion independently for poison")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DEFOG, target: opponentLeft); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponentRight->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 1);
        EXPECT(opponentLeft->status1 & STATUS1_POISON);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap respects stat-drop immunity abilities and Clear Amulet")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_WHITE_SMOKE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_FULL_METAL_BODY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_GLORIOUS_MANE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap applies Contrary Simple Defiant Competitive and Mirror Armor normally")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], ability == ABILITY_CONTRARY ? DEFAULT_STAT_STAGE + 1
                  : ability == ABILITY_SIMPLE ? DEFAULT_STAT_STAGE - 2
                  : ability == ABILITY_MIRROR_ARMOR ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_EVASION], ability == ABILITY_MIRROR_ARMOR ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], ability == ABILITY_DEFIANT ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], ability == ABILITY_COMPETITIVE ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap can poison at minimum evasion even though evasion cannot fall further")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        for (u32 turn = 0; turn < (B_UPDATED_MOVE_DATA >= GEN_6 ? 3 : 6); turn++) TURN { MOVE(player, MOVE_SWEET_SCENT); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], MIN_STAT_STAGE);
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap can poison already negative foes even when its evasion drop is blocked")
{
    bool32 amulet;
    PARAMETRIZE { amulet = FALSE; }
    PARAMETRIZE { amulet = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CLEAR_BODY); Item(ITEM_CLEAR_AMULET); }
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWEET_SCENT); }
        TURN { MOVE(player, amulet ? MOVE_TRICK : MOVE_SKILL_SWAP); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - (B_UPDATED_MOVE_DATA >= GEN_6 ? 2 : 1));
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap respects poison typing and status immunity abilities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_MAGNEMITE; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_EKANS; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_IMMUNITY; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_COMATOSE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFOG); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap respects poison protections without preventing its evasion drop")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_MISTY_TERRAIN; }
    PARAMETRIZE { protection = MOVE_ELECTRIC_TERRAIN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFOG); MOVE(opponent, protection); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(!!(opponent->status1 & STATUS1_POISON), protection == MOVE_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap poison is not blocked by Grass Overcoat Shield Dust or Safety Goggles")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { species = SPECIES_CHIKORITA; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFOG); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap never replaces existing status conditions")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT(opponent->status1 & status);
        EXPECT(!(opponent->status1 & STATUS1_POISON));
    }
}

SINGLE_BATTLE_TEST("Succulent Trap's poison can be cured immediately by Pecha and Lum Berries")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_PECHA_BERRY; }
    PARAMETRIZE { item = ITEM_LUM_BERRY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFOG); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        STATUS_ICON(opponent, poison: TRUE);
        STATUS_ICON(opponent, poison: FALSE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap slows incoming damaging attacks below the holder's evasion stage")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, move, B_BATTLER_0), -1);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap applies an additional priority drop to normally or badly poisoned attackers")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_EXTREME_SPEED); }
    } SCENE {
        if (status == STATUS1_NONE) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EXTREME_SPEED, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EXTREME_SPEED, opponent);
        }
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_EXTREME_SPEED, B_BATTLER_0), status ? 0 : 1);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap leaves targeted status moves and field moves at their usual priority")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap requires lower relative evasion even when the attacker is poisoned")
{
    bool32 higher;
    PARAMETRIZE { higher = FALSE; }
    PARAMETRIZE { higher = TRUE; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); }
        if (higher) TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_TEAM, opponent);
        if (higher) ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_TEAM, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0), 0);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap compares evasion stages rather than checking whether the attacker is negative")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DEFOG); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap slows attacks aimed at its holder without shielding its partner")
{
    bool32 targetTrap;
    PARAMETRIZE { targetTrap = FALSE; }
    PARAMETRIZE { targetTrap = TRUE; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_TACKLE, target: targetTrap ? playerLeft : playerRight); }
    } SCENE {
        if (targetTrap) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        }
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap also slows friendly attacks aimed at the holder when evasion is lower")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(98); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DEFOG, target: playerRight); }
        TURN { MOVE(playerRight, MOVE_TACKLE, target: playerLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEFOG, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap slows an entire spread attack once even when it hits two holders")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(2); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponentLeft);
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_POISON);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_EARTHQUAKE, B_BATTLER_2), -2);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap preserves native negative priorities and leaves Quash intact")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {}
    } THEN {
        gProtectStructs[B_BATTLER_1].quash = TRUE;
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0), -8);
        gProtectStructs[B_BATTLER_1].quash = FALSE;
        // Native negative move priorities are lowered by the same amount.
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_COUNTER, B_BATTLER_0), GetMovePriority(MOVE_COUNTER) - 2);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap and Mephitic Dust priority penalties stack")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0), -3);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_2), -1);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap's priority reduction lets attacks bypass priority protection only against its holder")
{
    enum Ability protection;
    bool32 targetTrap;
    PARAMETRIZE { protection = ABILITY_DAZZLING; targetTrap = FALSE; }
    PARAMETRIZE { protection = ABILITY_DAZZLING; targetTrap = TRUE; }
    PARAMETRIZE { protection = ABILITY_PSYCHIC_SURGE; targetTrap = FALSE; }
    PARAMETRIZE { protection = ABILITY_PSYCHIC_SURGE; targetTrap = TRUE; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(protection); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: targetTrap ? playerLeft : playerRight); }
    } THEN {
        if (targetTrap) EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        else EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}

SINGLE_BATTLE_TEST("Succulent Trap's entry drop can be cured by White Herb and then gives no priority penalty")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0), 0);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap slows attacks redirected to its holder by Follow Me or Rage Powder")
{
    enum Move redirect;
    PARAMETRIZE { redirect = MOVE_FOLLOW_ME; }
    PARAMETRIZE { redirect = MOVE_RAGE_POWDER; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(playerLeft, redirect);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, redirect, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
        HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap does not slow attacks redirected away from it by Mushroom Madness")
{
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); Speed(50); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
        HP_BAR(playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Succulent Trap follows Storm Drain and Lightning Rod redirection and respects Stalwart")
{
    enum Ability redirectAbility, attackerAbility;
    enum Move move;
    PARAMETRIZE { redirectAbility = ABILITY_STORM_DRAIN; move = MOVE_WATER_GUN; attackerAbility = ABILITY_NONE; }
    PARAMETRIZE { redirectAbility = ABILITY_LIGHTNING_ROD; move = MOVE_THUNDER_SHOCK; attackerAbility = ABILITY_NONE; }
    PARAMETRIZE { redirectAbility = ABILITY_STORM_DRAIN; move = MOVE_WATER_GUN; attackerAbility = ABILITY_STALWART; }
    PARAMETRIZE { redirectAbility = ABILITY_LIGHTNING_ROD; move = MOVE_THUNDER_SHOCK; attackerAbility = ABILITY_STALWART; }
    GIVEN {
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(redirectAbility); Speed(50); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(attackerAbility); Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            MOVE(opponentLeft, move, target: playerLeft);
        }
    } SCENE {
        if (attackerAbility == ABILITY_STALWART) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
            ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
            HP_BAR(playerLeft);
        } else {
            ABILITY_POPUP(playerRight, redirectAbility);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        }
    } THEN {
        if (attackerAbility == ABILITY_NONE)
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}
