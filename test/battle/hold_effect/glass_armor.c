#include "global.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_GLASS_ARMOR].holdEffect == HOLD_EFFECT_GLASS_ARMOR);
    ASSUME(GetItemPocket(ITEM_GLASS_ARMOR) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_GLASS_ARMOR) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_GLASS_ARMOR) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_GLASS_ARMOR) == 10000);
    ASSUME(GetItemFlingPower(ITEM_GLASS_ARMOR) == 100);
}

SINGLE_BATTLE_TEST("Glass Armor reduces only the first attacking move that hits the holder", s16 firstDamage, s16 secondDamage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_GLASS_ARMOR; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); Defense(100); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(150); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].firstDamage);
        if (item == ITEM_GLASS_ARMOR) {
            MESSAGE("The Glass Armor shattered into sharp spikes!");
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GLASS_ARMOR_SPIKES, opponent);
        }
        HP_BAR(player, captureDamage: &results[i].secondDamage);
    } FINALLY {
        EXPECT_LT(results[1].firstDamage, results[0].firstDamage);
        EXPECT_GE(results[1].firstDamage * 100, results[0].firstDamage * 65);
        EXPECT_LE(results[1].firstDamage * 100, results[0].firstDamage * 68);
        EXPECT_EQ(results[1].secondDamage, results[0].secondDamage);
    }
}

SINGLE_BATTLE_TEST("Glass Armor scatters one layer of Spikes on both sides and is consumed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_GLASS_ARMOR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The Glass Armor shattered into sharp spikes!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GLASS_ARMOR_SPIKES, opponent);
    } THEN {
        u32 playerSpikes = gSideTimers[B_SIDE_PLAYER].spikesAmount;
        u32 opponentSpikes = gSideTimers[B_SIDE_OPPONENT].spikesAmount;

        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(playerSpikes, 1);
        EXPECT_EQ(opponentSpikes, 1);
    }
}

SINGLE_BATTLE_TEST("Glass Armor scatters Spikes when held by the opponent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_GLASS_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The Glass Armor shattered into sharp spikes!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GLASS_ARMOR_SPIKES, player);
    } THEN {
        u32 playerSpikes = gSideTimers[B_SIDE_PLAYER].spikesAmount;
        u32 opponentSpikes = gSideTimers[B_SIDE_OPPONENT].spikesAmount;

        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(playerSpikes, 1);
        EXPECT_EQ(opponentSpikes, 1);
    }
}

SINGLE_BATTLE_TEST("Glass Armor does not shatter when a status move hits the holder")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_GLASS_ARMOR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        NOT MESSAGE("The Glass Armor shattered into sharp spikes!");
    } THEN {
        u32 playerSpikes = gSideTimers[B_SIDE_PLAYER].spikesAmount;
        u32 opponentSpikes = gSideTimers[B_SIDE_OPPONENT].spikesAmount;

        EXPECT_EQ(player->item, ITEM_GLASS_ARMOR);
        EXPECT_EQ(playerSpikes, 0);
        EXPECT_EQ(opponentSpikes, 0);
    }
}

SINGLE_BATTLE_TEST("Glass Armor does not reduce or shatter from a hit absorbed by Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Item(ITEM_GLASS_ARMOR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("The Glass Armor shattered into sharp spikes!");
    } THEN {
        u32 playerSpikes = gSideTimers[B_SIDE_PLAYER].spikesAmount;
        u32 opponentSpikes = gSideTimers[B_SIDE_OPPONENT].spikesAmount;

        EXPECT_EQ(player->item, ITEM_GLASS_ARMOR);
        EXPECT_EQ(playerSpikes, 0);
        EXPECT_EQ(opponentSpikes, 0);
    }
}
