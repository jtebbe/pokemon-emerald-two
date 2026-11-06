#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Frostfelt Hatred belongs to Froslass and Mega Froslass with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_FROSLASS; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_FROSLASS_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_FROSLASS; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SNORUNT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GLALIE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GLALIE_MEGA; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_FROSTFELT_HATRED,
            item == ITEM_APEX_METEORITE && (species == SPECIES_FROSLASS || species == SPECIES_FROSLASS_MEGA));
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred halves the selected move's remaining PP rounded up before or after its action")
{
    u32 pp = 1;
    bool32 first = TRUE;
    for (u32 n = 1; n <= 5; n++) {
        PARAMETRIZE { pp = n; first = TRUE; }
        PARAMETRIZE { pp = n; first = FALSE; }
    }
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(first ? 100 : 1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(first ? 1 : 100); HP(3000); MaxHP(3000); MovesWithPP({MOVE_WATER_GUN, pp}, {MOVE_TACKLE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        if (!first) HP_BAR(player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        MESSAGE("The opposing Wobbuffet's Water Gun\nwas weakened!");
        if (first) HP_BAR(player);
    } THEN {
        EXPECT_EQ(opponent->pp[0], first ? (pp + 1) / 2 - 1 : pp / 2);
        EXPECT_EQ(opponent->pp[1], 20);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred halves physical special and fixed damage only for the rest of that turn", s16 before; s16 weak; s16 after)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_DRAGON_RAGE; }
    PARAMETRIZE { move = MOVE_NIGHT_SHADE; }
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Attack(100); SpAttack(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move, secondaryEffect: FALSE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, move, secondaryEffect: FALSE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].before);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        HP_BAR(player, captureDamage: &results[i].weak);
        HP_BAR(player, captureDamage: &results[i].after);
    } THEN {
        EXPECT_EQ(results[i].before / 2, results[i].weak);
        EXPECT_EQ(results[i].before, results[i].after);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred drains the chosen slot rather than the last move used")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); MovesWithPP({MOVE_CELEBRATE, 20}, {MOVE_WATER_GUN, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(opponent->pp[0], 19);
        EXPECT_EQ(opponent->pp[1], 9);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred drains PP per hit without stacking its damage reduction", s16 before; s16 weak)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); MovesWithPP({MOVE_DRAGON_RAGE, 10}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_ICICLE_SPEAR, WITH_RNG(RNG_HITS, 3)); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[0].before);
        for (u32 hit = 0; hit < 3; hit++) {
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        }
        HP_BAR(player, captureDamage: &results[0].weak);
    } THEN {
        // 10 - 1 = 9; three hits leave 5, 3, 2; the selected action spends one.
        EXPECT_EQ(opponent->pp[0], 1);
        EXPECT_EQ(results[0].before, 40);
        EXPECT_EQ(results[0].weak, 20);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred can drain a selected status move")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->pp[0], 9);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred is not blocked by Shield Dust Covert Cloak Magic Guard or Neutralizing Gas")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_NEUTRALIZING_GAS; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->pp[0], 9);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred does not trigger on misses immunity or status moves")
{
    enum Move move;
    enum Species species;
    bool32 hit;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; species = SPECIES_WOBBUFFET; hit = FALSE; }
    PARAMETRIZE { move = MOVE_TACKLE; species = SPECIES_GENGAR; hit = TRUE; }
    PARAMETRIZE { move = MOVE_GROWL; species = SPECIES_WOBBUFFET; hit = TRUE; }
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(species) { Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
    } WHEN {
        TURN { MOVE(player, move, hit: hit); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
    } THEN {
        EXPECT_EQ(opponent->pp[0], 19);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred does not trigger through Protect")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_PROTECT, 10}); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
    } THEN {
        EXPECT_EQ(opponent->pp[0], 9);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred does not trigger through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); MovesWithPP({MOVE_SUBSTITUTE, 10}, {MOVE_CELEBRATE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
    } THEN {
        EXPECT_EQ(opponent->pp[1], 19);
    }
}

DOUBLE_BATTLE_TEST("Frostfelt Hatred applies independently to both spread targets")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); HP(1000); MaxHP(1000); MovesWithPP({MOVE_DRAGON_RAGE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); HP(1000); MaxHP(1000); MovesWithPP({MOVE_DRAGON_RAGE, 10}); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_POWDER_SNOW, secondaryEffect: FALSE); MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerRight); MOVE(opponentRight, MOVE_DRAGON_RAGE, target: playerLeft); }
    } THEN {
        EXPECT_EQ(opponentLeft->pp[0], 4);
        EXPECT_EQ(opponentRight->pp[0], 4);
        EXPECT_EQ(playerLeft->hp, 980);
        EXPECT_EQ(playerRight->hp, 980);
    }
}

DOUBLE_BATTLE_TEST("Frostfelt Hatred never drains or weakens an ally hit by Surf")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(playerRight->pp[0], 19);
        EXPECT_EQ(gProtectStructs[B_BATTLER_2].frostfeltHatredMove, MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred's damage penalty persists after its holder pivots out")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); MovesWithPP({MOVE_DRAGON_RAGE, 10}); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        HP_BAR(player, damage: 20);
    } THEN {
        EXPECT_EQ(opponent->pp[0], 4);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred clears on switching and respects the engine's PP restoration")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); SWITCH(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        NOT ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
    } THEN {
        EXPECT_EQ(opponent->pp[0], GetMovePP(MOVE_CELEBRATE));
        EXPECT_EQ(GetMonData(&OPPONENT_PARTY[1], MON_DATA_PP1), GetMovePP(MOVE_CELEBRATE));
        EXPECT_EQ(gProtectStructs[B_BATTLER_1].frostfeltHatredMove, MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred does not drain a stale move slot when the target uses Struggle")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); MovesWithPP({MOVE_TACKLE, 0}, {MOVE_STRUGGLE, 1}); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_STRUGGLE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
    } THEN {
        EXPECT_EQ(opponent->pp[0], 0);
        EXPECT_EQ(gProtectStructs[B_BATTLER_1].frostfeltHatredMove, MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Frostfelt Hatred still drains the chosen PP when its hit knocks out the target")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); MovesWithPP({MOVE_CELEBRATE, 20}); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(opponent, 1); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_FROSTFELT_HATRED);
        MESSAGE("The opposing Wobbuffet lost 10 PP from Celebrate!");
        MESSAGE("The opposing Wobbuffet's Celebrate\nwas weakened!");
        MESSAGE("The opposing Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(opponent->pp[0], 20);
        EXPECT_EQ(gProtectStructs[B_BATTLER_1].frostfeltHatredMove, MOVE_NONE);
    }
}
