#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mime Box sets Light Screen and Reflect for five turns on its own side")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_MIME_JR; }
    PARAMETRIZE { species = SPECIES_MR_MIME; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MIME_BOX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        MESSAGE("Light Screen made your side stronger against special moves!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        MESSAGE("Reflect made your side stronger against physical moves!");
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gSideStatuses[B_SIDE_PLAYER] & (SIDE_STATUS_LIGHTSCREEN | SIDE_STATUS_REFLECT), SIDE_STATUS_LIGHTSCREEN | SIDE_STATUS_REFLECT);
        EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT] & (SIDE_STATUS_LIGHTSCREEN | SIDE_STATUS_REFLECT), 0);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].lightscreenTimer, 4);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimer, 4);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].lightscreenTimerTotal, 5);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimerTotal, 5);
    }
}

SINGLE_BATTLE_TEST("Mime Box only sets missing screens and preserves existing timers")
{
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MR_MIME) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(player, MOVE_REFLECT); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        ABILITY_POPUP(player, ABILITY_MIME_BOX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimer, 3);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].lightscreenTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Mime Box can knock out the attacker with blocked contact")
{
    GIVEN {
        PLAYER(SPECIES_MR_MIME) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1); MaxHP(240); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Mr. Mime protected itself!");
        ABILITY_POPUP(player, ABILITY_MIME_BOX);
        HP_BAR(opponent, hp: 0);
        MESSAGE("The opposing Makuhita fainted!");
    }
}

SINGLE_BATTLE_TEST("Mime Box deals one sixth max HP when protection blocks contact")
{
    enum Move protection, attack;
    enum Ability attackerAbility;
    enum Item item;
    u32 damage;
    PARAMETRIZE { protection = MOVE_PROTECT; attack = MOVE_TACKLE; attackerAbility = ABILITY_GUTS; item = ITEM_NONE; damage = 40; }
    PARAMETRIZE { protection = MOVE_PROTECT; attack = MOVE_SWIFT; attackerAbility = ABILITY_GUTS; item = ITEM_NONE; damage = 0; }
    PARAMETRIZE { protection = MOVE_CELEBRATE; attack = MOVE_TACKLE; attackerAbility = ABILITY_GUTS; item = ITEM_NONE; damage = 0; }
    PARAMETRIZE { protection = MOVE_PROTECT; attack = MOVE_TACKLE; attackerAbility = ABILITY_GUTS; item = ITEM_PROTECTIVE_PADS; damage = 0; }
    PARAMETRIZE { protection = MOVE_PROTECT; attack = MOVE_TACKLE; attackerAbility = ABILITY_MAGIC_GUARD; item = ITEM_NONE; damage = 0; }
    PARAMETRIZE { protection = MOVE_PROTECT; attack = MOVE_HYPER_DRILL; attackerAbility = ABILITY_GUTS; item = ITEM_NONE; damage = 0; }
    PARAMETRIZE { protection = MOVE_SPIKY_SHIELD; attack = MOVE_TACKLE; attackerAbility = ABILITY_GUTS; item = ITEM_NONE; damage = 70; }
    GIVEN {
        PLAYER(SPECIES_MR_MIME) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA) { Ability(attackerAbility); Item(item); HP(240); MaxHP(240); }
    } WHEN {
        TURN { MOVE(player, protection); MOVE(opponent, attack); }
    } THEN {
        EXPECT_EQ(opponent->hp, 240 - damage);
    }
}

DOUBLE_BATTLE_TEST("Mime Box retaliates when an ally's Wide Guard blocks a spread contact move")
{
    GIVEN {
        PLAYER(SPECIES_MIME_JR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_MAKUHITA);
        OPPONENT(SPECIES_MAKUHITA) { HP(240); MaxHP(240); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_WIDE_GUARD); MOVE(opponentLeft, MOVE_BRUTAL_SWING); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(opponentLeft->hp, 200);
    }
}
