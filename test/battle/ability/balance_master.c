#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Balance Master boosts contact damage and bypasses Protect", s16 damage)
{
    enum Item item;
    bool32 protect;
    PARAMETRIZE { item = ITEM_NONE;           protect = FALSE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_HITMONTOP) { Ability(ABILITY_INTIMIDATE); Item(item); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Defense(100); }
    } WHEN {
        TURN {
            if (protect)
                MOVE(opponent, MOVE_PROTECT);
            MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0));
        }
    } SCENE {
        if (protect)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, uq4_12_multiply_by_int_half_down(UQ_4_12(1.2), results[0].damage));
    }
}

SINGLE_BATTLE_TEST("Balance Master counters blocked contact with a random known contact move without spending PP")
{
    enum Move attack, counter;
    u32 choice;
    PARAMETRIZE { attack = MOVE_TACKLE; counter = MOVE_TACKLE;  choice = 0; }
    PARAMETRIZE { attack = MOVE_TACKLE; counter = MOVE_SCRATCH; choice = 1; }
    PARAMETRIZE { attack = MOVE_SWIFT;  counter = MOVE_NONE;    choice = 0; }
    GIVEN {
        PLAYER(SPECIES_HITMONTOP) {
            Item(ITEM_APEX_METEORITE);
            MovesWithPP({MOVE_PROTECT, 5}, {MOVE_TACKLE, 0}, {MOVE_SCRATCH, 5}, {MOVE_SWIFT, 5});
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, attack, WITH_RNG(RNG_BALANCE_MASTER_MOVE, choice)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        if (counter != MOVE_NONE)
        {
            ABILITY_POPUP(player, ABILITY_BALANCE_MASTER);
            ANIMATION(ANIM_TYPE_MOVE, counter, player);
            HP_BAR(opponent);
        }
        NOT ABILITY_POPUP(player, ABILITY_BALANCE_MASTER);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->pp[0], 4);
        EXPECT_EQ(player->pp[1], 0);
        EXPECT_EQ(player->pp[2], 5);
        EXPECT_EQ(player->pp[3], 5);
    }
}

DOUBLE_BATTLE_TEST("Balance Master aims a normally random-target counter at the actual attacker")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_THRASH));
        PLAYER(SPECIES_HITMONTOP) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PROTECT, MOVE_THRASH); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(opponentRight, MOVE_TACKLE, target: playerLeft, WITH_RNG(RNG_RANDOM_TARGET, 0)); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_BALANCE_MASTER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THRASH, playerLeft, target: opponentRight);
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentLeft);
    }
}
