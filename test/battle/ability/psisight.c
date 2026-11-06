#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Psisight boosts both special stats for a selected Psychic attack or status move")
{
    enum Species species;
    enum Move move;
    PARAMETRIZE { species = SPECIES_BUTTERFREE; move = MOVE_CONFUSION; }
    PARAMETRIZE { species = SPECIES_BUTTERFREE_MEGA; move = MOVE_CONFUSION; }
    PARAMETRIZE { species = SPECIES_BUTTERFREE; move = MOVE_AGILITY; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSISIGHT);
        MESSAGE("Butterfree's Sp. Atk rose!");
        MESSAGE("Butterfree's Sp. Def rose!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_PSISIGHT);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->types[0], TYPE_BUG);
        EXPECT_EQ(player->types[1], TYPE_FLYING);
    }
}

SINGLE_BATTLE_TEST("Psisight activates before Fake Out even when the selected move cannot execute")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_FAKE_OUT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSISIGHT);
        MESSAGE("Butterfree's Sp. Atk rose!");
        MESSAGE("Butterfree's Sp. Def rose!");
        MESSAGE("The opposing Wobbuffet used Fake Out!");
        MESSAGE("Butterfree flinched and couldn't move!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Psisight activates once each Psychic turn and respects stat caps")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 turn = 0; turn < 8; turn++)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Psisight needs a selected Psychic move and an Apex Meteorite")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_CONFUSION; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_GUST; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SLEEP_POWDER; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_COMPOUND_EYES); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PSISIGHT);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Psisight does not boost when switching away despite a previous Psychic selection")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSISIGHT);
        SEND_IN_MESSAGE("Wobbuffet");
        NOT ABILITY_POPUP(player, ABILITY_PSISIGHT);
        SEND_IN_MESSAGE("Butterfree");
        NOT ABILITY_POPUP(player, ABILITY_PSISIGHT);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Psisight includes Compound Eyes accuracy for attacks and status moves")
{
    enum Move move;
    u32 accuracy;
    PARAMETRIZE { move = MOVE_THUNDER; accuracy = 91; }
    PARAMETRIZE { move = MOVE_SLEEP_POWDER; accuracy = 97; }
    PASSES_RANDOMLY(accuracy, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Psisight accuracy does not alter OHKO accuracy")
{
    PASSES_RANDOMLY(30, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FISSURE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Psisight grants Psychic STAB without doubling existing STAB", s16 damage)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_ZEN_HEADBUTT; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_ZEN_HEADBUTT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_BUG_BITE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_BUG_BITE; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_COMPOUND_EYES); Item(item); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Psisight checks dynamic Psychic type after Terastallizing")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_PSISIGHT); TeraType(TYPE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_BLAST, gimmick: GIMMICK_TERA); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSISIGHT);
        MESSAGE("Butterfree's Sp. Atk rose!");
        MESSAGE("Butterfree's Sp. Def rose!");
        MESSAGE("Butterfree used Tera Blast!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Psisight Psychic STAB is retained with a different Tera type and enhanced by Psychic Tera", s16 damage)
{
    enum Type teraType;
    enum Gimmick gimmick;
    PARAMETRIZE { teraType = TYPE_NORMAL; gimmick = GIMMICK_NONE; }
    PARAMETRIZE { teraType = TYPE_NORMAL; gimmick = GIMMICK_TERA; }
    PARAMETRIZE { teraType = TYPE_PSYCHIC; gimmick = GIMMICK_TERA; }
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_PSISIGHT); TeraType(teraType); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, MOVE_ZEN_HEADBUTT, gimmick: gimmick); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0 / 3.0), results[2].damage);
    }
}

DOUBLE_BATTLE_TEST("Psisight and Beak Blast share the setup phase and activate in Speed order")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CONFUSION, target: opponentLeft); MOVE(playerRight, MOVE_BEAK_BLAST, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_PSISIGHT);
        MESSAGE("Butterfree's Sp. Def rose!");
        MESSAGE("Wobbuffet started heating up its beak!");
        MESSAGE("Butterfree used Confusion!");
        MESSAGE("Wobbuffet used Beak Blast!");
    }
}

SINGLE_BATTLE_TEST("Psisight remains active under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_PSISIGHT);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
