#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ancient Helix converts Normal moves into Rock moves only with the Apex Meteorite")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_OMANYTE) { Item(item); }
        OPPONENT(SPECIES_DRIFBLIM);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        if (item == ITEM_APEX_METEORITE)
        {
            HP_BAR(opponent);
            MESSAGE("It's super effective!");
        }
        else
        {
            MESSAGE("It doesn't affect the opposing Drifblim…");
            NOT HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Ancient Helix gives only converted moves a fixed 1.3x base power boost")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SWIFT; }
    PARAMETRIZE { move = MOVE_ANCIENT_POWER; }
    GIVEN {
        WITH_CONFIG(B_ATE_MULTIPLIER, GEN_9);
        PLAYER(SPECIES_OMASTAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(gBattleMovePower, GetMovePower(move) * (move == MOVE_SWIFT ? 130 : 100) / 100);
    }
}

SINGLE_BATTLE_TEST("Ancient Helix skips recharge even for moves it does not convert")
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_HYPER_BEAM; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_HYPER_BEAM; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_METEOR_ASSAULT; }
    GIVEN {
        PLAYER(SPECIES_OMASTAR) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, hit: TRUE); }
        TURN {
            if (item == ITEM_APEX_METEORITE)
                MOVE(player, move, hit: TRUE);
            else
                SKIP_TURN(player);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (item == ITEM_APEX_METEORITE)
        {
            NOT MESSAGE("Omastar must recharge!");
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            HP_BAR(opponent);
        }
        else
        {
            MESSAGE("Omastar must recharge!");
        }
    } THEN {
        if (item == ITEM_APEX_METEORITE)
            EXPECT_EQ((u32)player->volatiles.rechargeTimer, 0);
    }
}
