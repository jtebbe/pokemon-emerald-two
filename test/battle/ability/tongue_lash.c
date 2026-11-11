#include "global.h"
#include "battle_main.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Tongue Lash follows contact with a Normal Lick without needing to know it")
{
    bool32 contact;
    PARAMETRIZE { contact = TRUE; }
    PARAMETRIZE { contact = FALSE; }
    enum Move move = contact ? MOVE_TACKLE : MOVE_SWIFT;
    GIVEN {
        PLAYER(SPECIES_LICKITUNG) { Item(ITEM_APEX_METEORITE); MovesWithPP({move, 5}); }
        OPPONENT(SPECIES_EEVEE) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (contact)
        {
            ABILITY_POPUP(player, ABILITY_TONGUE_LASH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LICK, player);
            HP_BAR(opponent);
        }
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_LICK, player);
    } THEN {
        EXPECT_EQ(player->pp[0], 4);
        EXPECT_EQ(GetMoveType(MOVE_LICK), TYPE_GHOST);
    }
}

SINGLE_BATTLE_TEST("Tongue Lash retaliates once after a multi-hit contact move and flinching stops it")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DOUBLE_KICK; }
    PARAMETRIZE { move = MOVE_FAKE_OUT; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_LICKILICKY) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player);
        if (move == MOVE_DOUBLE_KICK)
        {
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_TONGUE_LASH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LICK, player);
            HP_BAR(opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
        NOT ABILITY_POPUP(player, ABILITY_TONGUE_LASH);
    }
}

SINGLE_BATTLE_TEST("Tongue Lash subtracts two priority brackets from a paralyzed opponent's move")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_LICKILICKY) { Item(item); Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_ATTACK, WITH_RNG(RNG_PARALYSIS, FALSE)); MOVE(player, MOVE_SWIFT); }
    } SCENE {
        if (item == ITEM_APEX_METEORITE)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LICK, player);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        }
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, ABILITY_SHADOW_TAG, MOVE_QUICK_ATTACK, B_BATTLER_0),
            item == ITEM_APEX_METEORITE ? -1 : 1);
    }
}
