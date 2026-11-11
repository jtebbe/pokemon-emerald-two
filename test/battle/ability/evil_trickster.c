#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Evil Trickster gives every Gengar family member Prankster and Levitate with an Apex Meteorite")
{
    enum Species species = SPECIES_GASTLY;
    PARAMETRIZE { species = SPECIES_GASTLY; }
    PARAMETRIZE { species = SPECIES_HAUNTER; }
    PARAMETRIZE { species = SPECIES_GENGAR; }
    PARAMETRIZE { species = SPECIES_GENGAR_MEGA; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_WATER_GUN); } } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_EVIL_TRICKSTER);
        EXPECT(!IsBattlerGrounded(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)));
    }
}

SINGLE_BATTLE_TEST("Evil Trickster needs its Meteorite and does not grant priority to damaging attacks")
{
    enum Item item = ITEM_NONE;
    enum Move move = MOVE_GROWL;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_GROWL; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_LICK; }
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(item); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); MOVE(opponent, MOVE_WATER_GUN); } } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Evil Trickster keeps Prankster's generation-dependent Dark immunity")
{
    u32 gen = GEN_7;
    PARAMETRIZE { gen = GEN_6; }
    PARAMETRIZE { gen = GEN_7; }
    GIVEN {
        WITH_CONFIG(B_PRANKSTER_DARK_TYPES, gen);
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_UMBREON);
    } WHEN { TURN { MOVE(player, MOVE_GROWL); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (gen < GEN_7));
    }
}

DOUBLE_BATTLE_TEST("Evil Trickster's Prankster component can boost an allied Dark type")
{
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_UMBREON);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerLeft, MOVE_COACHING, target: playerRight); } } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Evil Trickster's status priority is blocked by anti-priority abilities")
{
    enum Ability ability = ABILITY_DAZZLING;
    PARAMETRIZE { ability = ABILITY_DAZZLING; }
    PARAMETRIZE { ability = ABILITY_QUEENLY_MAJESTY; }
    PARAMETRIZE { ability = ABILITY_ARMOR_TAIL; }
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN { TURN { MOVE(player, MOVE_LEER); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Evil Trickster's status priority respects Quick Guard and Psychic Terrain")
{
    enum Move protection = MOVE_QUICK_GUARD;
    PARAMETRIZE { protection = MOVE_QUICK_GUARD; }
    PARAMETRIZE { protection = MOVE_PSYCHIC_TERRAIN; }
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (protection == MOVE_PSYCHIC_TERRAIN) {
            TURN { MOVE(opponent, protection); }
            TURN { MOVE(player, MOVE_LEER); }
        } else {
            TURN { MOVE(opponent, protection); MOVE(player, MOVE_LEER); }
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Evil Trickster blocks Ground attacks and cannot be bypassed by Mold Breaker")
{
    enum Ability ability = ABILITY_TELEPATHY;
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN { TURN { MOVE(opponent, MOVE_EARTH_POWER); } } SCENE {
        ABILITY_POPUP(player, ABILITY_EVIL_TRICKSTER);
        MESSAGE("It doesn't affect Gengar…");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Evil Trickster is grounded by Gravity Smack Down and Thousand Arrows")
{
    enum Move grounding = MOVE_GRAVITY;
    PARAMETRIZE { grounding = MOVE_GRAVITY; }
    PARAMETRIZE { grounding = MOVE_SMACK_DOWN; }
    PARAMETRIZE { grounding = MOVE_THOUSAND_ARROWS; }
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, grounding); }
        TURN { MOVE(opponent, MOVE_EARTH_POWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT(IsBattlerGrounded(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)));
    }
}

SINGLE_BATTLE_TEST("Evil Trickster avoids grounded hazards without absorbing Toxic Spikes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_TOXIC_SPIKES));
    }
}

DOUBLE_BATTLE_TEST("Evil Trickster retains both components under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_GROWL); MOVE(opponentLeft, MOVE_EARTH_POWER, target: playerLeft); } } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_EVIL_TRICKSTER);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_EVIL_TRICKSTER);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}
