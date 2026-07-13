#include "global.h"
#include "battle_util.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_STRANGE_AMULET].holdEffect == HOLD_EFFECT_STRANGE_AMULET);
    ASSUME(GetItemPocket(ITEM_STRANGE_AMULET) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_STRANGE_AMULET) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_STRANGE_AMULET) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_STRANGE_AMULET) == 10000);
    ASSUME(GetItemFlingPower(ITEM_STRANGE_AMULET) == 40);
}

SINGLE_BATTLE_TEST("Strange Amulet makes physical moves deal special damage", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_STRANGE_AMULET; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); SpAttack(50); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(50); SpDefense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet makes special moves deal physical damage", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_STRANGE_AMULET; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); SpAttack(50); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(50); SpDefense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet uses the swapped category for battle move display helpers")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
    } THEN {
        u32 battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerBattleMoveCategory(battler, MOVE_TACKLE), DAMAGE_CATEGORY_SPECIAL);
        EXPECT_EQ(GetBattlerBattleMoveCategory(battler, MOVE_WATER_GUN), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_TACKLE), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_WATER_GUN), DAMAGE_CATEGORY_SPECIAL);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet makes Superpower lower Sp. Atk and Sp. Def instead of Attack and Defense")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SUPERPOWER) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_SUPERPOWER, MOVE_EFFECT_ATK_DEF_DOWN));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPERPOWER); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet makes Draco Meteor lower Attack instead of Sp. Atk")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_DRACO_METEOR) == DAMAGE_CATEGORY_SPECIAL);
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_DRACO_METEOR, MOVE_EFFECT_SP_ATK_MINUS_2));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRACO_METEOR); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet makes Meteor Mash raise Sp. Atk instead of Attack")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_METEOR_MASH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_METEOR_MASH, MOVE_EFFECT_ATK_PLUS_1));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_METEOR_MASH, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet makes Fiery Dance raise Attack instead of Sp. Atk")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_FIERY_DANCE) == DAMAGE_CATEGORY_SPECIAL);
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_FIERY_DANCE, MOVE_EFFECT_SP_ATK_PLUS_1));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FIERY_DANCE, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Strange Amulet does not swap positive stat changes from status moves")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SWORDS_DANCE) == DAMAGE_CATEGORY_STATUS);
        ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_ATTACK_UP_2);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_STRANGE_AMULET); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
