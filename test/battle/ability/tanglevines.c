#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Tanglevines lowers Speed, traps and seeds a contact attacker")
{
    GIVEN {
        PLAYER(SPECIES_TANGELA) { Item(ITEM_APEX_METEORITE); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA) { HP(160); MaxHP(160); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TANGLEVINES);
        MESSAGE("The opposing Makuhita can no longer escape!");
        MESSAGE("The opposing Makuhita was seeded!");
        MESSAGE("The opposing Makuhita's Speed fell!");
        HP_BAR(opponent, damage: 20);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
        EXPECT_EQ((u32)opponent->volatiles.battlerPreventingEscape, B_BATTLER_0);
        EXPECT_EQ((u32)opponent->volatiles.leechSeed, LEECHSEEDED_BY(B_BATTLER_0));
    }
}

SINGLE_BATTLE_TEST("Tanglevines applies each effect independently of the other effects' immunities")
{
    enum Species species;
    bool32 slower, trapped, seeded;
    PARAMETRIZE { species = SPECIES_BELDUM; slower = FALSE; trapped = TRUE; seeded = TRUE; }
    PARAMETRIZE { species = SPECIES_TREECKO; slower = TRUE; trapped = TRUE; seeded = FALSE; }
    PARAMETRIZE { species = SPECIES_GASTLY; slower = TRUE; trapped = FALSE; seeded = TRUE; }
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        PLAYER(SPECIES_TANGROWTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TANGLEVINES);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - slower);
        EXPECT_EQ((bool32)opponent->volatiles.escapePrevention, trapped);
        EXPECT_EQ((bool32)(opponent->volatiles.leechSeed != 0), seeded);
    }
}

SINGLE_BATTLE_TEST("Tanglevines needs an Apex Meteorite and a hit that triggers contact effects")
{
    enum Item holderItem, attackerItem;
    enum Move move;
    PARAMETRIZE { holderItem = ITEM_NONE; attackerItem = ITEM_NONE; move = MOVE_SCRATCH; }
    PARAMETRIZE { holderItem = ITEM_APEX_METEORITE; attackerItem = ITEM_PROTECTIVE_PADS; move = MOVE_SCRATCH; }
    PARAMETRIZE { holderItem = ITEM_APEX_METEORITE; attackerItem = ITEM_NONE; move = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_TANGELA) { Item(holderItem); }
        OPPONENT(SPECIES_MAKUHITA) { Item(attackerItem); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TANGLEVINES);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(!opponent->volatiles.escapePrevention);
        EXPECT(!opponent->volatiles.leechSeed);
    }
}

SINGLE_BATTLE_TEST("Tanglevines trapping ends when the holder switches out, while Leech Seed stays")
{
    GIVEN {
        PLAYER(SPECIES_TANGELA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_MAKUHITA) { HP(160); MaxHP(160); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT(CanBattlerEscape(B_BATTLER_1));
        EXPECT_EQ((u32)opponent->volatiles.leechSeed, LEECHSEEDED_BY(B_BATTLER_0));
        EXPECT_EQ(opponent->hp, 120);
    }
}

AI_SINGLE_BATTLE_TEST("The AI avoids making contact with Tanglevines when it has another attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_TANGELA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_MAKUHITA) { Attack(100); SpAttack(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_SCRATCH, MOVE_SWIFT); EXPECT_MOVE(opponent, MOVE_SWIFT); }
    }
}
