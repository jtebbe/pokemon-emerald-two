#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Big Meaty Claws applies Sharpness and refreshes a three-turn bleed")
{
    s16 firstHit, secondHit;
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(2400); MaxHP(2400); }
    } WHEN {
        TURN { MOVE(player, MOVE_NIGHT_SLASH); }
        TURN {}
        TURN { MOVE(player, MOVE_NIGHT_SLASH); }
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SLASH, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        MESSAGE("The opposing Wobbuffet started bleeding!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SLASH, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_EQ(firstHit, secondHit);
        EXPECT_EQ(opponent->hp, 2400 - firstHit - secondHit - 5 * 300);
        EXPECT_EQ((u32)opponent->volatiles.bleedTurns, 0);
    }
}

SINGLE_BATTLE_TEST("Big Meaty Claws boosts slicing damage like Sharpness even against Ghost types", s16 damage)
{
    bool32 apex = FALSE;
    PARAMETRIZE { apex = TRUE; }
    PARAMETRIZE { apex = FALSE; }
    GIVEN {
        if (apex)
            PLAYER(SPECIES_KRABBY) { Item(ITEM_APEX_METEORITE); }
        else
            PLAYER(SPECIES_KRABBY) { Ability(ABILITY_SHARPNESS); }
        OPPONENT(SPECIES_GENGAR) { Ability(ABILITY_BATTLE_ARMOR); HP(2400); MaxHP(2400); }
    } WHEN {
        TURN { MOVE(player, MOVE_NIGHT_SLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(opponent->hp, 2400 - results[i].damage);
        EXPECT_EQ((u32)opponent->volatiles.bleedTurns, 0);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Big Meaty Claws keeps Hyper Cutter's Attack protection")
{
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Big Meaty Claws cannot bleed a target through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_KRABBY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(2400); MaxHP(2400); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_NIGHT_SLASH); }
    } SCENE {
        SUB_HIT(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, 1800);
        EXPECT_EQ((u32)opponent->volatiles.bleedTurns, 0);
    }
}
