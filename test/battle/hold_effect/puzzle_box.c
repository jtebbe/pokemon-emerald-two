#include "global.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_PUZZLE_BOX].holdEffect == HOLD_EFFECT_PUZZLE_BOX);
    ASSUME(GetItemPocket(ITEM_PUZZLE_BOX) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_PUZZLE_BOX) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_PUZZLE_BOX) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_PUZZLE_BOX) == 10000);
    ASSUME(GetItemFlingPower(ITEM_PUZZLE_BOX) == 80);
}

SINGLE_BATTLE_TEST("Puzzle Box swaps holder's Attack and Sp. Attack for physical damage calculation", s16 damage)
{
    u16 item;
    u16 attack;
    u16 spAttack;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; attack = 50; spAttack = 200; }
    PARAMETRIZE { item = ITEM_NONE; attack = 200; spAttack = 50; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(attack); SpAttack(spAttack); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box swaps holder's Attack and Sp. Attack for special damage calculation", s16 damage)
{
    u16 item;
    u16 attack;
    u16 spAttack;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; attack = 50; spAttack = 200; }
    PARAMETRIZE { item = ITEM_NONE; attack = 200; spAttack = 50; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(attack); SpAttack(spAttack); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box swaps holder's Defense and Sp. Defense against physical damage", s16 damage)
{
    u16 item;
    u16 defense;
    u16 spDefense;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; defense = 50; spDefense = 200; }
    PARAMETRIZE { item = ITEM_NONE; defense = 200; spDefense = 50; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Defense(defense); SpDefense(spDefense); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(150); SpAttack(150); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box swaps holder's Defense and Sp. Defense against special damage", s16 damage)
{
    u16 item;
    u16 defense;
    u16 spDefense;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; defense = 50; spDefense = 200; }
    PARAMETRIZE { item = ITEM_NONE; defense = 200; spDefense = 50; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Defense(defense); SpDefense(spDefense); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(150); SpAttack(150); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box makes Body Press use the holder's swapped defensive stat", s16 damage)
{
    u16 item;
    u16 defense;
    u16 spDefense;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; defense = 50; spDefense = 200; }
    PARAMETRIZE { item = ITEM_NONE; defense = 200; spDefense = 50; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_BODY_PRESS) == EFFECT_BODY_PRESS);
        PLAYER(SPECIES_WOBBUFFET) { Defense(defense); SpDefense(spDefense); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_BODY_PRESS, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box makes Foul Play use the target's swapped offensive stat", s16 damage)
{
    u16 item;
    u16 attack;
    u16 spAttack;
    PARAMETRIZE { item = ITEM_PUZZLE_BOX; attack = 50; spAttack = 200; }
    PARAMETRIZE { item = ITEM_NONE; attack = 200; spAttack = 50; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FOUL_PLAY) == EFFECT_FOUL_PLAY);
        PLAYER(SPECIES_WOBBUFFET) { Attack(10); SpAttack(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(attack); SpAttack(spAttack); Item(item); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_PLAY, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Puzzle Box does not modify the holder's stored battle stats")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(50); Defense(60); SpAttack(200); SpDefense(210); Item(ITEM_PUZZLE_BOX); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->attack, 50);
        EXPECT_EQ(player->defense, 60);
        EXPECT_EQ(player->spAttack, 200);
        EXPECT_EQ(player->spDefense, 210);
    }
}
