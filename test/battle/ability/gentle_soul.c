#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Gentle Soul protects its ally and heals each battler using their own max HP", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_DRAGONITE) { Item(item); HP(100); MaxHP(160); }
        PLAYER(SPECIES_WOBBUFFET) { HP(200); MaxHP(320); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); SpAttack(220); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WATER_GUN, target: playerRight); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &results[i].damage);
        if (item == ITEM_APEX_METEORITE)
        {
            HP_BAR(playerLeft, damage: -10);
            HP_BAR(playerRight, damage: -20);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, item == ITEM_APEX_METEORITE ? 110 : 100);
        EXPECT_EQ(playerRight->hp, 200 - results[i].damage + (item == ITEM_APEX_METEORITE ? 20 : 0));
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Gentle Soul doubles the chance of secondary effects")
{
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_DRATINI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}
