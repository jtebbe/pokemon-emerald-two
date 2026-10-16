#include "global.h"
#include "test/battle.h"
#include "item_use.h"
#include "party_menu.h"

SINGLE_BATTLE_TEST("Ether restores the PP of one of a battler's moves by 10 ")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_ETHER].battleUsage == EFFECT_ITEM_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_ETHER].type == ITEM_USE_PARTY_MENU_MOVES);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, 0}, {MOVE_CONFUSION, 20}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_ETHER, partyIndex: 0, move: MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->pp[0], 10);
        EXPECT_EQ(player->pp[1], 20);
    }
}
SINGLE_BATTLE_TEST("Max Ether restores the PP of one of a battler's moves fully")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_MAX_ETHER].battleUsage == EFFECT_ITEM_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_MAX_ETHER].type == ITEM_USE_PARTY_MENU_MOVES);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, 0}, {MOVE_CONFUSION, 20}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_MAX_ETHER, partyIndex: 0, move: MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_SCRATCH));
        EXPECT_EQ(player->pp[1], 20);
    }
}

SINGLE_BATTLE_TEST("Elixir restores the PP of all of a battler's moves by 10")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_ELIXIR].battleUsage == EFFECT_ITEM_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_ELIXIR].type == ITEM_USE_PARTY_MENU);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, 0}, {MOVE_CONFUSION, 0}, {MOVE_SCRATCH, 0}, {MOVE_GROWL, 0}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_ELIXIR, partyIndex: 0); }
    } THEN {
        EXPECT_EQ(player->pp[0], 10);
        EXPECT_EQ(player->pp[1], 10);
        EXPECT_EQ(player->pp[2], 10);
        EXPECT_EQ(player->pp[3], 10);
    }
}

SINGLE_BATTLE_TEST("Max Elixir restores the PP of all of a battler's moves fully")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_MAX_ELIXIR].battleUsage == EFFECT_ITEM_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_MAX_ELIXIR].type == ITEM_USE_PARTY_MENU);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, 0}, {MOVE_CONFUSION, 0}, {MOVE_SCRATCH, 0}, {MOVE_GROWL, 0}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_MAX_ELIXIR, partyIndex: 0); }
    } THEN {
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_SCRATCH));
        EXPECT_EQ(player->pp[1], GetMovePP(MOVE_CONFUSION));
        EXPECT_EQ(player->pp[2], GetMovePP(MOVE_SCRATCH));
        EXPECT_EQ(player->pp[3], GetMovePP(MOVE_GROWL));
    }
}

SINGLE_BATTLE_TEST("PP items check active battle PP rather than always-full party storage")
{
    u32 depletedSlot;
    PARAMETRIZE { depletedSlot = 0; }
    PARAMETRIZE { depletedSlot = 1; }
    PARAMETRIZE { depletedSlot = 2; }
    PARAMETRIZE { depletedSlot = 3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_CONFUSION, MOVE_GROWL, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
        gPartyMenu.slotId = 0;
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
            player->pp[i] = GetMovePP(player->moves[i]);
        gPartyMenu.data1 = depletedSlot;
        EXPECT(CannotUseItemsInBattle(ITEM_ETHER, mon));
        EXPECT(CannotUseItemsInBattle(ITEM_MAX_ETHER, mon));
        EXPECT(CannotUseItemsInBattle(ITEM_ELIXIR, mon));
        player->pp[depletedSlot]--;
        EXPECT(!CannotUseItemsInBattle(ITEM_ETHER, mon));
        EXPECT(!CannotUseItemsInBattle(ITEM_MAX_ETHER, mon));
        EXPECT(!CannotUseItemsInBattle(ITEM_ELIXIR, mon));
        EXPECT(!CannotUseItemsInBattle(ITEM_MAX_ELIXIR, mon));
        gPartyMenu.data1 = (depletedSlot + 1) % MAX_MON_MOVES;
        EXPECT(CannotUseItemsInBattle(ITEM_ETHER, mon));
        // Benched Pokémon never retain depleted PP.
        EXPECT(CannotUseItemsInBattle(ITEM_ELIXIR, &gParties[B_TRAINER_PLAYER][1]));
    }
}

SINGLE_BATTLE_TEST("Ether caps restored PP at the PP-Max maximum")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, GetMovePP(MOVE_SCRATCH) - 2}); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_ETHER, partyIndex: 0, move: MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_SCRATCH));
        EXPECT_EQ(player->pp[1], 0);
    }
}
