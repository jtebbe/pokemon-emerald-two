#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_ai_util.h"
#include "sprite.h"

SINGLE_BATTLE_TEST("Rootbound raises both defenses and applies normal Ingrain on entry for the entire Bulbasaur line")
{
    enum Species species;
    bool32 opposing;
    PARAMETRIZE { species = SPECIES_BULBASAUR; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_IVYSAUR; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_VENUSAUR; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_VENUSAUR_MEGA; opposing = FALSE; }
    PARAMETRIZE { species = SPECIES_BULBASAUR; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_IVYSAUR; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_VENUSAUR; opposing = TRUE; }
    PARAMETRIZE { species = SPECIES_VENUSAUR_MEGA; opposing = TRUE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : species) { Item(opposing ? ITEM_NONE : ITEM_APEX_METEORITE); HP(128); MaxHP(256); }
        OPPONENT(opposing ? species : SPECIES_WOBBUFFET) { Item(opposing ? ITEM_APEX_METEORITE : ITEM_NONE); HP(128); MaxHP(256); }
    } WHEN {
        TURN {}
    } SCENE {
        ITEM_POPUP(opposing ? opponent : player, ITEM_APEX_METEORITE);
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_ROOTBOUND);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opposing ? opponent : player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, opposing ? opponent : player);
        HP_BAR(opposing ? opponent : player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        enum BattlerId battler = opposing ? B_BATTLER_1 : B_BATTLER_0;
        EXPECT_EQ(GetBattlerAbility(battler), ABILITY_ROOTBOUND);
        EXPECT_EQ(gBattleMons[battler].statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMons[battler].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(gBattleMons[battler].volatiles.root);
        EXPECT_EQ(gBattleMons[battler].hp, 144);
        EXPECT(!CanBattlerEscape(battler));
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(battler), ABILITY_ROOTBOUND);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

SINGLE_BATTLE_TEST("Rootbound does not activate without an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { HP(128); MaxHP(256); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN {}
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROOTBOUND);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_OVERGROW);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT(!player->volatiles.root);
        EXPECT_EQ(player->hp, 128);
    }
}

SINGLE_BATTLE_TEST("Rootbound doubles Growth to two stages normally and four stages in sun with normal stage caps")
{
    bool32 sunny;
    u32 uses;
    PARAMETRIZE { sunny = FALSE; uses = 1; }
    PARAMETRIZE { sunny = FALSE; uses = 2; }
    PARAMETRIZE { sunny = FALSE; uses = 3; }
    PARAMETRIZE { sunny = TRUE; uses = 1; }
    PARAMETRIZE { sunny = TRUE; uses = 2; }
    PARAMETRIZE { sunny = TRUE; uses = 3; }
    GIVEN {
        WITH_CONFIG(B_GROWTH_STAT_RAISE, GEN_5);
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (sunny)
            TURN { MOVE(player, MOVE_SUNNY_DAY); }
        for (u32 i = 0; i < uses; i++)
            TURN { MOVE(player, MOVE_GROWTH); }
    } THEN {
        u32 expected = min(MAX_STAT_STAGE, DEFAULT_STAT_STAGE + uses * (sunny ? 4 : 2));
        EXPECT_EQ(player->statStages[STAT_ATK], expected);
        EXPECT_EQ(player->statStages[STAT_SPATK], expected);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Rootbound does not double other stat boosts or the opponent's Growth")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WORK_UP); MOVE(opponent, MOVE_GROWTH); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Cloud Nine suppresses Rootbound's sunny Growth bonus but not its doubling")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GOLDUCK) { Ability(ABILITY_CLOUD_NINE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_GROWTH); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Rootbound's Ingrain blocks phazing and redundant Ingrain still fails")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_IVYSAUR);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_INGRAIN); MOVE(opponent, MOVE_ROAR); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        MESSAGE("Bulbasaur planted its roots!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_BULBASAUR);
        EXPECT(player->volatiles.root);
    }
}

SINGLE_BATTLE_TEST("Rootbound's Ingrain healing obeys Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); HP(128); MaxHP(256); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
    } THEN {
        EXPECT_EQ(player->hp, 128);
        EXPECT(player->volatiles.root);
    }
}

SINGLE_BATTLE_TEST("Rootbound activates on each entry even with Baton Passed roots and defensive boosts")
{
    GIVEN {
        PLAYER(SPECIES_BULBASAUR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 0); }
    } SCENE {
        MESSAGE("Bulbasaur is unleashing its potential!");
        ABILITY_POPUP(player, ABILITY_ROOTBOUND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
        NOT MESSAGE("Bulbasaur is unleashing its potential!");
        ABILITY_POPUP(player, ABILITY_ROOTBOUND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT(player->volatiles.root);
    }
}
