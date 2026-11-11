#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Fully Charged takes neutral Electric damage and gains Speed after being hit")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_ELECTIVIRE) { Ability(ABILITY_VITAL_SPIRIT); Item(item); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SHOCK_WAVE); }
    } SCENE {
        HP_BAR(player);
        if (item == ITEM_APEX_METEORITE) {
            NOT MESSAGE("It's not very effective…");
            ABILITY_POPUP(player, ABILITY_FULLY_CHARGED);
        } else {
            MESSAGE("It's not very effective…");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (item == ITEM_APEX_METEORITE));
    }
}

SINGLE_BATTLE_TEST("Fully Charged doubles damage from the holder's Electric moves", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_ELECTABUZZ) { Item(item); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0), criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage * 2);
    }
}

SINGLE_BATTLE_TEST("Fully Charged guarantees critical hits against paralysis with any attack type")
{
    u32 status;
    enum Ability ability;
    PARAMETRIZE { status = STATUS1_NONE; ability = ABILITY_GUTS; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; ability = ABILITY_GUTS; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; ability = ABILITY_BATTLE_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_ELEKID) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA) { Ability(ability); Status1(status); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_POUND, criticalHit: FALSE); }
    } SCENE {
        if (status == STATUS1_PARALYSIS && ability != ABILITY_BATTLE_ARMOR) {
            MESSAGE("A critical hit!");
        } else {
            NOT MESSAGE("A critical hit!");
        }
    }
}
