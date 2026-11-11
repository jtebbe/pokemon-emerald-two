#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Great Minds deals five quarter-strength hits for one PP", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_EXEGGCUTE) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); MovesWithPP({MOVE_TACKLE, 5}); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(500); MaxHP(500); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        s16 hit;
        for (u32 i = 0; i < (apex ? 5 : 1); i++)
        {
            HP_BAR(opponent, captureDamage: &hit);
            results[apex].damage += hit;
        }
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->pp[0], 4);
    } FINALLY {
        EXPECT_EQ(results[1].damage, 5 * (results[0].damage / 4));
    }
}

SINGLE_BATTLE_TEST("Great Minds leaves existing multihit moves and status moves alone")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BULLET_SEED, MOVE_GROWTH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWTH); }
        TURN { MOVE(player, MOVE_BULLET_SEED, WITH_RNG(RNG_HITS, 2)); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Great Minds continues a spread move against foes that survive")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SWIFT); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponentLeft);
        for (u32 i = 0; i < 5; i++)
            HP_BAR(opponentRight);
        NOT HP_BAR(opponentRight);
    }
}

SINGLE_BATTLE_TEST("Great Minds applies to a Future Sight attack after the setter switches out")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_FUTURE_SIGHT); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE {
        for (u32 i = 0; i < 5; i++)
            HP_BAR(opponent);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Great Minds lets Explosion finish its strikes before the user faints")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_EXPLOSION); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_EXPLOSION); }
    } SCENE {
        for (u32 i = 0; i < 5; i++)
            HP_BAR(opponent);
        MESSAGE("Exeggutor fainted!");
    }
}
