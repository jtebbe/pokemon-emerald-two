#include "global.h"
#include "battle_util.h"
#include "item_use.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_THESAURUS].holdEffect == HOLD_EFFECT_THESAURUS);
    ASSUME(GetItemPocket(ITEM_THESAURUS) == POCKET_BATTLE_ITEMS);
    ASSUME(GetItemType(ITEM_THESAURUS) == ITEM_USE_BAG_MENU);
    ASSUME(GetItemFieldFunc(ITEM_THESAURUS) == ItemUseOutOfBattle_CannotUse);
    ASSUME(GetItemSellPrice(ITEM_THESAURUS) == 10000);
    ASSUME(GetItemFlingPower(ITEM_THESAURUS) == 80);
}

SINGLE_BATTLE_TEST("Thesaurus replaces a physical move with another move of the same type and category")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_THESAURUS, MOVE_SCRATCH)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Scratch!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_TACKLE) - 1);
    }
}

SINGLE_BATTLE_TEST("Thesaurus replaces a special move with another move of the same type and category")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_THUNDERBOLT, MOVE_THUNDER_SHOCK));
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT, WITH_RNG(RNG_THESAURUS, MOVE_THUNDER_SHOCK)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Thunder Shock!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
    }
}

SINGLE_BATTLE_TEST("Thesaurus replaces a status move and recalculates its target")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_LEER, MOVE_RECOVER));
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(200); HP(100); Item(ITEM_THESAURUS); Moves(MOVE_LEER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEER, WITH_RNG(RNG_THESAURUS, MOVE_RECOVER)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Recover!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Thesaurus preserves the selected target when replacing a single-target move with another single-target move")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_WATER_GUN, MOVE_HYDRO_PUMP));
        ASSUME(GetMoveTarget(MOVE_WATER_GUN) == MOVE_TARGET_SELECTED);
        ASSUME(GetMoveTarget(MOVE_HYDRO_PUMP) == MOVE_TARGET_SELECTED);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight, WITH_RNG(RNG_THESAURUS, MOVE_HYDRO_PUMP)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Hydro Pump!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, playerLeft, target: opponentRight);
        NOT HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Thesaurus recalculates targeting when replacing a single-target move with a spread move")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_WATER_GUN, MOVE_MUDDY_WATER));
        ASSUME(GetMoveTarget(MOVE_WATER_GUN) == MOVE_TARGET_SELECTED);
        ASSUME(GetMoveTarget(MOVE_MUDDY_WATER) == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_WATER_GUN); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight, WITH_RNG(RNG_THESAURUS, MOVE_MUDDY_WATER)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Muddy Water!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUDDY_WATER, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Thesaurus chooses a random opposing target when replacing a spread move with a single-target move")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_MUDDY_WATER, MOVE_HYDRO_PUMP));
        ASSUME(GetMoveTarget(MOVE_MUDDY_WATER) == MOVE_TARGET_BOTH);
        ASSUME(GetMoveTarget(MOVE_HYDRO_PUMP) == MOVE_TARGET_SELECTED);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_MUDDY_WATER); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_MUDDY_WATER, WITH_RNG(RNG_THESAURUS, MOVE_HYDRO_PUMP)); }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        MESSAGE("Wobbuffet used Hydro Pump!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, playerLeft);
    } THEN {
        u32 battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u32 target = gBattleStruct->moveTarget[battler];

        EXPECT_EQ(target == GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)
               || target == GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT), TRUE);
        EXPECT_EQ((opponentLeft->hp < opponentLeft->maxHP) + (opponentRight->hp < opponentRight->maxHP), 1);
    }
}

SINGLE_BATTLE_TEST("Thesaurus works when held by an opposing Pokemon")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_THESAURUS); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_THESAURUS, MOVE_SCRATCH)); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    } THEN {
        u32 battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        EXPECT_EQ(gBattleStruct->thesaurusMove[battler], MOVE_SCRATCH);
        EXPECT_EQ(gLastMoves[battler], MOVE_TACKLE);
        EXPECT_EQ(gLastResultingMoves[battler], MOVE_SCRATCH);
    }
}

SINGLE_BATTLE_TEST("Thesaurus is suppressed by Klutz")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Item(ITEM_THESAURUS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Thesaurus does not randomize Trick")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_TRICK); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); }
    } SCENE {
        NOT MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_THESAURUS);
    }
}

SINGLE_BATTLE_TEST("Thesaurus does not randomize Switcheroo")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_SWITCHEROO); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWITCHEROO); }
    } SCENE {
        NOT MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWITCHEROO, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_THESAURUS);
    }
}

SINGLE_BATTLE_TEST("Thesaurus does not randomize Fling")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(MOVE_FLING); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        NOT MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

AI_SINGLE_BATTLE_TEST("AI tries to give Thesaurus to an opponent with Trick or Switcheroo")
{
    u32 move;

    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not try to give Thesaurus to a Good as Gold opponent")
{
    u32 move;

    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_GHOLDENGO) { Ability(ABILITY_GOOD_AS_GOLD); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_THESAURUS); Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    }
}

AI_SINGLE_BATTLE_TEST("AI does not try to give Thesaurus to a Dark type with Prankster")
{
    u32 move;

    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }

    GIVEN {
        WITH_CONFIG(CONFIG_PRANKSTER_DARK_TYPES, GEN_7);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_UMBREON);
        OPPONENT(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); Item(ITEM_THESAURUS); Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    }
}

SINGLE_BATTLE_TEST("Thesaurus holders are Encored into the selected move rather than the fetched move")
{
    GIVEN {
        ASSUME(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_THESAURUS); Moves(MOVE_TACKLE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_ENCORE, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_THESAURUS, MOVE_SCRATCH));
            MOVE(opponent, MOVE_ENCORE);
        }
        TURN {
            MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_THESAURUS, MOVE_POUND));
            MOVE(opponent, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, opponent);
        MESSAGE("Wobbuffet is checking its thesaurus!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, player);
    } THEN {
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].encoredMove, MOVE_TACKLE);
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_TACKLE) - 2);
        EXPECT_EQ(player->pp[1], GetMovePP(MOVE_SPLASH));
    }
}

SINGLE_BATTLE_TEST("Thesaurus candidate filtering excludes invalid move identities and categories")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_TACKLE), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_THUNDER_SHOCK), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_GROWL), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_LEER, MOVE_METRONOME), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_BREAKNECK_BLITZ), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_MAX_STRIKE), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TACKLE, MOVE_G_MAX_REPLENISH), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_TRICK, MOVE_LEER), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_SWITCHEROO, MOVE_LEER), FALSE);
        EXPECT_EQ(IsMoveValidForThesaurus(MOVE_FLING, MOVE_TACKLE), FALSE);
    }
}
