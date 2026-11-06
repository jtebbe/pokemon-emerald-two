#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Guardian's Growl belongs to regular Growlithe and Arcanine with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GROWLITHE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GROWLITHE_HISUI; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE_HISUI; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Ability(ABILITY_FLASH_FIRE); Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_GUARDIANS_GROWL, item == ITEM_APEX_METEORITE && (species == SPECIES_GROWLITHE || species == SPECIES_ARCANINE)); }
}

DOUBLE_BATTLE_TEST("Guardian's Growl lowers both opponents' Attack and Special Attack but not its partner's")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE) { Ability(ABILITY_INTIMIDATE); Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} }
    SCENE { ABILITY_POPUP(playerLeft, ABILITY_GUARDIANS_GROWL); MESSAGE("The opposing Wobbuffet's Attack fell!"); MESSAGE("The opposing Wobbuffet's Sp. Atk fell!"); MESSAGE("The opposing Wynaut's Attack fell!"); MESSAGE("The opposing Wynaut's Sp. Atk fell!"); }
    THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Guardian's Growl respects Intimidate immunity abilities and Clear Amulet")
{
    enum Ability ability;
    enum Item item = ITEM_NONE;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_WHITE_SMOKE; }
    PARAMETRIZE { ability = ABILITY_FULL_METAL_BODY; }
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { ability = ABILITY_SCRAPPY; }
    PARAMETRIZE { ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { ability = ABILITY_OBLIVIOUS; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_9);
        PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Guardian's Growl follows generation-specific Intimidate protection")
{
    u32 gen;
    PARAMETRIZE { gen = GEN_7; }
    PARAMETRIZE { gen = GEN_8; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, gen);
        PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INNER_FOCUS); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (gen < GEN_8)); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - (gen < GEN_8)); }
}

DOUBLE_BATTLE_TEST("Guardian's Growl is blocked independently by Soundproof without consuming Adrenaline Orb")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SOUNDPROOF); Item(ITEM_ADRENALINE_ORB); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} }
    SCENE { ABILITY_POPUP(playerLeft, ABILITY_GUARDIANS_GROWL); ABILITY_POPUP(opponentLeft, ABILITY_SOUNDPROOF); }
    THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->item, ITEM_ADRENALINE_ORB);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Guardian's Growl is blocked by Substitute and Mist")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SUBSTITUTE; }
    PARAMETRIZE { move = MOVE_MIST; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(opponent, move); } TURN { SWITCH(player, 1); } }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

DOUBLE_BATTLE_TEST("Guardian's Growl respects allied Flower Veil protection")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_FLABEBE) { Ability(ABILITY_FLOWER_VEIL); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1); }
}

DOUBLE_BATTLE_TEST("Guardian's Growl activates each opponent's Adrenaline Orb once")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ADRENALINE_ORB); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_ADRENALINE_ORB); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponentLeft->item, ITEM_NONE); EXPECT_EQ(opponentRight->item, ITEM_NONE); }
}

DOUBLE_BATTLE_TEST("Guardian's Growl activates each Rattled opponent once per entry")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_9);
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_RATTLED); }
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_RATTLED); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Guardian's Growl triggers Defiant Competitive Contrary and Guard Dog as Intimidate does")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_DEFIANT; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    PARAMETRIZE { ability = ABILITY_GUARD_DOG; }
    GIVEN { PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); } }
    WHEN { TURN {} }
    THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_DEFIANT ? 3 : ability == ABILITY_CONTRARY || ability == ABILITY_GUARD_DOG ? 1 : -1));
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (ability == ABILITY_COMPETITIVE ? 3 : ability == ABILITY_CONTRARY ? 1 : ability == ABILITY_GUARD_DOG ? 0 : -1));
    }
}

DOUBLE_BATTLE_TEST("Guardian's Growl preserves its source when Guard Dog reacts before the other opponent")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_OKIDOGI) { Ability(ABILITY_GUARD_DOG); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Guardian's Growl can reflect both drops through Mirror Armor")
{
    GIVEN { PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_CORVIKNIGHT) { Ability(ABILITY_MIRROR_ARMOR); } }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Guardian's Growl activates again when switching back in")
{
    GIVEN {
        PLAYER(SPECIES_GROWLITHE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_RATTLED); }
    } WHEN { TURN { SWITCH(player, 1); } TURN { SWITCH(player, 0); } }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("Guardian's Growl's drops are restored together by White Herb")
{
    GIVEN { PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); } }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->item, ITEM_NONE); }
}
