#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Mutual Origin adds 50% base power per active ally or opponent that knows the move", s16 damage)
{
    u32 shared;
    PARAMETRIZE { shared = 0; }
    PARAMETRIZE { shared = 1; }
    PARAMETRIZE { shared = 2; }
    PARAMETRIZE { shared = 3; }
    GIVEN {
        PLAYER(SPECIES_MEW) { Item(ITEM_APEX_METEORITE); Level(100); Attack(350); Speed(10); Moves(MOVE_POUND); }
        PLAYER(SPECIES_WYNAUT) { Speed(20); Moves(shared >= 1 ? MOVE_POUND : MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFAIRY) { Speed(30); Moves(MOVE_POUND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); Defense(100); Moves(shared >= 2 ? MOVE_POUND : MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(80); Moves(shared >= 3 ? MOVE_POUND : MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_POUND, target: opponentLeft, criticalHit: FALSE); }
    } SCENE {
        if (shared == 0)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        if (shared != 0)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
    } THEN {
        EXPECT_EQ(CountOtherBattlersKnowingMove(B_BATTLER_0, MOVE_POUND, FALSE), shared);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Mutual Origin grants priority to a shared status move")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GROWL, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
