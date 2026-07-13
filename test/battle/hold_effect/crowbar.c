#include "global.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_CROWBAR].holdEffect == HOLD_EFFECT_CROWBAR);
    ASSUME(GetItemPocket(ITEM_CROWBAR) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_CROWBAR) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_CROWBAR) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_CROWBAR) == 10000);
    ASSUME(GetItemFlingPower(ITEM_CROWBAR) == 120);
}

SINGLE_BATTLE_TEST("Crowbar ignores positive Defense stages when using a physical contact move", s16 damage)
{
    u16 item;
    bool32 boostDefense;
    PARAMETRIZE { item = ITEM_NONE; boostDefense = FALSE; }
    PARAMETRIZE { item = ITEM_CROWBAR; boostDefense = TRUE; }
    PARAMETRIZE { item = ITEM_NONE; boostDefense = TRUE; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(MoveMakesContact(MOVE_TACKLE));
        PLAYER(SPECIES_WOBBUFFET) { Attack(150); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); }
    } WHEN {
        if (boostDefense)
            TURN { MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        if (boostDefense)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_GT(results[1].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Crowbar ignores positive Sp. Defense stages when using a special contact move", s16 damage)
{
    u16 item;
    bool32 boostSpDefense;
    PARAMETRIZE { item = ITEM_NONE; boostSpDefense = FALSE; }
    PARAMETRIZE { item = ITEM_CROWBAR; boostSpDefense = TRUE; }
    PARAMETRIZE { item = ITEM_NONE; boostSpDefense = TRUE; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_DRAINING_KISS) == DAMAGE_CATEGORY_SPECIAL);
        ASSUME(MoveMakesContact(MOVE_DRAINING_KISS));
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(150); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(100); }
    } WHEN {
        if (boostSpDefense)
            TURN { MOVE(opponent, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_DRAINING_KISS, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        if (boostSpDefense)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AMNESIA, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_GT(results[1].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Crowbar does not ignore positive Defense stages for non-contact attacks", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_CROWBAR; }
    PARAMETRIZE { item = ITEM_NONE; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_EARTHQUAKE) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(!MoveMakesContact(MOVE_EARTHQUAKE));
        PLAYER(SPECIES_WOBBUFFET) { Attack(150); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Crowbar does not ignore negative defensive stages", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_CROWBAR; }
    PARAMETRIZE { item = ITEM_NONE; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(MoveMakesContact(MOVE_TACKLE));
        PLAYER(SPECIES_WOBBUFFET) { Attack(150); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAIL_WHIP); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
