#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Sickening Stench belongs only to regular Grimer and Muk with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GRIMER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MUK; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRIMER_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MUK_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRIMER; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MUK; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        bool32 active = item == ITEM_APEX_METEORITE && (species == SPECIES_GRIMER || species == SPECIES_MUK);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SICKENING_STENCH, active);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - active);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - active);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench lowers both defenses by one or two according to each target's poison")
{
    enum Species species = SPECIES_GRIMER;
    u32 status = STATUS1_NONE;
    static const u32 statuses[] = {STATUS1_NONE, STATUS1_POISON, STATUS1_TOXIC_POISON, STATUS1_BURN};
    for (u32 j = 0; j < ARRAY_COUNT(statuses); j++)
    {
        PARAMETRIZE { species = SPECIES_GRIMER; status = statuses[j]; }
        PARAMETRIZE { species = SPECIES_MUK; status = statuses[j]; }
    }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } SCENE {
        ABILITY_POPUP(player, ABILITY_SICKENING_STENCH);
        if (status & (STATUS1_POISON | STATUS1_TOXIC_POISON)) {
            MESSAGE("The opposing Wobbuffet's Defense harshly fell!");
            MESSAGE("The opposing Wobbuffet's Sp. Def harshly fell!");
        } else {
            MESSAGE("The opposing Wobbuffet's Defense fell!");
            MESSAGE("The opposing Wobbuffet's Sp. Def fell!");
        }
    } THEN {
        u32 stages = status & (STATUS1_POISON | STATUS1_TOXIC_POISON) ? 2 : 1;
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - stages);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - stages);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Sickening Stench checks each opposing target independently and never affects its ally")
{
    bool32 oblivious;
    PARAMETRIZE { oblivious = FALSE; }
    PARAMETRIZE { oblivious = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(oblivious ? ABILITY_OBLIVIOUS : ABILITY_TELEPATHY); Status1(STATUS1_TOXIC_POISON); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SICKENING_STENCH);
        NOT ABILITY_POPUP(playerLeft, ABILITY_SICKENING_STENCH);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (oblivious ? 0 : 2));
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (oblivious ? 0 : 2));
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Oblivious blocks Sickening Stench even while poisoned and under old Intimidate rules")
{
    u32 gen, status;
    PARAMETRIZE { gen = GEN_3; status = STATUS1_NONE; }
    PARAMETRIZE { gen = GEN_3; status = STATUS1_POISON; }
    PARAMETRIZE { gen = GEN_9; status = STATUS1_NONE; }
    PARAMETRIZE { gen = GEN_9; status = STATUS1_TOXIC_POISON; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, gen);
        PLAYER(SPECIES_GRIMER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OBLIVIOUS); Status1(status); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench respects ordinary stat-loss protection but is not Intimidate or a sound")
{
    enum Ability ability;
    enum Item item = ITEM_NONE;
    bool32 blocksDef = FALSE, blocksSpDef = FALSE;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; blocksDef = TRUE; blocksSpDef = TRUE; }
    PARAMETRIZE { ability = ABILITY_WHITE_SMOKE; blocksDef = TRUE; blocksSpDef = TRUE; }
    PARAMETRIZE { ability = ABILITY_FULL_METAL_BODY; blocksDef = TRUE; blocksSpDef = TRUE; }
    PARAMETRIZE { ability = ABILITY_BIG_PECKS; blocksDef = TRUE; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; item = ITEM_CLEAR_AMULET; blocksDef = TRUE; blocksSpDef = TRUE; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { ability = ABILITY_SOUNDPROOF; }
    PARAMETRIZE { ability = ABILITY_GUARD_DOG; }
    PARAMETRIZE { ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { ability = ABILITY_SCRAPPY; }
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; }
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - !blocksDef);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - !blocksSpDef);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench uses standard Contrary Simple Defiant Competitive and Mirror Armor reactions")
{
    enum Ability ability = ABILITY_CONTRARY;
    bool32 poisoned = FALSE;
    static const enum Ability abilities[] = {ABILITY_CONTRARY, ABILITY_SIMPLE, ABILITY_DEFIANT, ABILITY_COMPETITIVE, ABILITY_MIRROR_ARMOR};
    for (u32 j = 0; j < ARRAY_COUNT(abilities); j++)
    {
        PARAMETRIZE { ability = abilities[j]; poisoned = FALSE; }
        PARAMETRIZE { ability = abilities[j]; poisoned = TRUE; }
    }
    GIVEN {
        PLAYER(SPECIES_GRIMER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Status1(poisoned ? STATUS1_POISON : STATUS1_NONE); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        s32 stages = poisoned ? 2 : 1;
        s32 change = ability == ABILITY_MIRROR_ARMOR ? 0 : ability == ABILITY_CONTRARY ? stages : -stages * (ability == ABILITY_SIMPLE ? 2 : 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + change);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + change);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_DEFIANT ? 4 : 0));
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (ability == ABILITY_COMPETITIVE ? 4 : 0));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (ability == ABILITY_MIRROR_ARMOR ? stages : 0));
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (ability == ABILITY_MIRROR_ARMOR ? stages : 0));
    }
}

SINGLE_BATTLE_TEST("Sickening Stench is blocked by Substitute or Mist when its holder switches in")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SUBSTITUTE; }
    PARAMETRIZE { protection = MOVE_MIST; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, protection); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Sickening Stench respects allied Flower Veil")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_FLABEBE) { Ability(ABILITY_FLOWER_VEIL); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench activates White Herb but not Rattled or Adrenaline Orb")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_WHITE_HERB; }
    PARAMETRIZE { item = ITEM_ADRENALINE_ORB; }
    GIVEN {
        PLAYER(SPECIES_GRIMER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_RATTLED); Item(item); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (item != ITEM_WHITE_HERB));
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (item != ITEM_WHITE_HERB));
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->item, item == ITEM_WHITE_HERB ? ITEM_NONE : item);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench checks poison on entry rather than continuously")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); }
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench reactivates on reentry and respects the minus-six stat cap")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MAGIC_GUARD); Status1(STATUS1_POISON); }
    } WHEN {
        TURN {}
        for (u32 entry = 0; entry < 3; entry++) {
            TURN { SWITCH(player, 1); }
            TURN { SWITCH(player, 0); }
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], MIN_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], MIN_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench does not reactivate just because the opponent switches")
{
    GIVEN {
        PLAYER(SPECIES_GRIMER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Sickening Stench survives Neutralizing Gas while suppressed Oblivious no longer protects")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OBLIVIOUS); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_SICKENING_STENCH);
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Sickening Stench respects Total Zen's Oblivious component")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_POISON); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_TOTAL_ZEN);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
