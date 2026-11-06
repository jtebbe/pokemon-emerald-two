#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Moon-Attuned belongs to the Cleffa line including Mega Clefable")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_CLEFFA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CLEFAIRY; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CLEFABLE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CLEFABLE_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_CLEFFA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); HP(1); MaxHP(1000); Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        if (species != SPECIES_WOBBUFFET && item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
            STATUS_ICON(player, none: TRUE);
            HP_BAR(player, damage: -250);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        }
    } THEN {
        bool32 active = species != SPECIES_WOBBUFFET && item == ITEM_APEX_METEORITE;
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_MOON_ATTUNED, active);
        EXPECT_EQ(player->status1, active ? STATUS1_NONE : STATUS1_PARALYSIS);
        EXPECT_EQ(player->hp, active ? 251 : 1);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned cures and heals both active allies using each recipient's max HP")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_WOBBUFFET) { HP(10); MaxHP(800); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED);
        STATUS_ICON(playerLeft, none: TRUE);
        HP_BAR(playerLeft, damage: -100);
        STATUS_ICON(playerRight, none: TRUE);
        HP_BAR(playerRight, damage: -200);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 101);
        EXPECT_EQ(playerRight->hp, 210);
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(opponentLeft->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponentRight->status1, STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned cures every major status including frostbite independently of the freeze configuration")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_SLEEP; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_FREEZE; }
    PARAMETRIZE { status = STATUS1_FROSTBITE; }
    GIVEN {
        WITH_CONFIG(B_USE_FROSTBITE, FALSE);
        PLAYER(SPECIES_CLEFAIRY) { Item(ITEM_APEX_METEORITE); HP(75); MaxHP(100); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        STATUS_ICON(player, none: TRUE);
        HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned can cure at full HP or heal without status and skips an unnecessary activation")
{
    u32 hp, status;
    PARAMETRIZE { hp = 100; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { hp = 75; status = STATUS1_NONE; }
    PARAMETRIZE { hp = 100; status = STATUS1_NONE; }
    GIVEN {
        PLAYER(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(100); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        if (hp < 100 || status) ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        else NOT ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        if (status) STATUS_ICON(player, none: TRUE);
        if (hp < 100) HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned activates on each switch-in for the holder and its current partner")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Status1(STATUS1_BURN); }
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED);
        HP_BAR(playerLeft, damage: -100);
        HP_BAR(playerRight, damage: -100);
        ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED);
        HP_BAR(playerLeft, damage: -100);
        HP_BAR(playerRight, damage: -100);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 201);
        EXPECT_EQ(playerRight->hp, 201);
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned cures an ally under Heal Block while only healing eligible recipients")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(800); Status1(STATUS1_PARALYSIS); }
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HEAL_BLOCK, target: playerRight); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED);
        HP_BAR(playerLeft, damage: -100);
        STATUS_ICON(playerRight, none: TRUE);
        NOT HP_BAR(playerRight);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 101);
        EXPECT_EQ(playerRight->hp, 500);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned entry healing ignores its ally's Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SUBSTITUTE); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        HP_BAR(playerRight, damage: 100);
        ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED);
        HP_BAR(playerRight, damage: -100);
    } THEN {
        EXPECT_EQ(playerRight->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned heals after entry damage and cures Toxic Spikes poisoning")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        HP_BAR(player, damage: 100);
        STATUS_ICON(player, poison: TRUE);
        ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        STATUS_ICON(player, none: TRUE);
        HP_BAR(player, damage: -200);
    } THEN {
        EXPECT_EQ(player->hp, 500);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned cannot heal either ally if the holder faints to entry hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(800); }
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SPIKES); }
        TURN { SWITCH(playerLeft, 2); SEND_OUT(playerLeft, 0); }
    } SCENE {
        HP_BAR(playerLeft, damage: 100);
        NONE_OF { ABILITY_POPUP(playerLeft, ABILITY_MOON_ATTUNED); HP_BAR(playerRight); }
    } THEN {
        EXPECT_EQ(playerRight->hp, 500);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned entry healing has a minimum of one HP")
{
    GIVEN {
        PLAYER(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(3); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        HP_BAR(player, damage: -1);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned boosts Blood Moon, Moonblast and Moongeist Beam but not other attacks", s16 damage)
{
    enum Move move = MOVE_BLOOD_MOON;
    enum Item item = ITEM_NONE;
    const enum Move attacks[] = {MOVE_BLOOD_MOON, MOVE_MOONBLAST, MOVE_MOONGEIST_BEAM, MOVE_SHADOW_BALL};
    for (u32 j = 0; j < ARRAY_COUNT(attacks); j++) {
        PARAMETRIZE { move = attacks[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = attacks[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); Item(item); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < 6; j += 2)
            EXPECT_MUL_EQ(results[j].damage, UQ_4_12(1.5), results[j + 1].damage);
        EXPECT_EQ(results[6].damage, results[7].damage);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned boosts an ally's Moon moves once even with two holders", s16 damage)
{
    u32 holders;
    PARAMETRIZE { holders = 0; }
    PARAMETRIZE { holders = 1; }
    PARAMETRIZE { holders = 2; }
    GIVEN {
        PLAYER(SPECIES_CLEFFA) { Item(holders >= 1 ? ITEM_APEX_METEORITE : ITEM_NONE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); Item(holders == 2 ? ITEM_APEX_METEORITE : ITEM_NONE); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_MOONBLAST, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned does not strengthen the opposing side's moves", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(300); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CLEFFA) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_MOONBLAST, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned's boost ends when its last holder leaves or faints")
{
    bool32 withdraw;
    PARAMETRIZE { withdraw = FALSE; }
    PARAMETRIZE { withdraw = TRUE; }
    s16 boosted, unboosted;
    GIVEN {
        PLAYER(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(40); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); SpAttack(300); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(40); Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); Speed(100); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_MOONBLAST, target: opponentRight); }
        if (withdraw)
            TURN { SWITCH(playerLeft, 2); MOVE(playerRight, MOVE_MOONBLAST, target: opponentRight); }
        else
            TURN { MOVE(opponentLeft, MOVE_SEISMIC_TOSS, target: playerLeft); MOVE(playerRight, MOVE_MOONBLAST, target: opponentRight); SEND_OUT(playerLeft, 2); }
    } SCENE {
        HP_BAR(opponentRight, captureDamage: &boosted);
        HP_BAR(opponentRight, captureDamage: &unboosted);
    } THEN {
        EXPECT_MUL_EQ(unboosted, UQ_4_12(1.5), boosted);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned increases Moonlight's weather-dependent healing by 50 percent")
{
    enum Move weather;
    PARAMETRIZE { weather = MOVE_CELEBRATE; }
    PARAMETRIZE { weather = MOVE_SUNNY_DAY; }
    PARAMETRIZE { weather = MOVE_RAIN_DANCE; }
    GIVEN {
        WITH_CONFIG(B_TIME_OF_DAY_HEALING_MOVES, GEN_9);
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Defense(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(9999); Speed(200); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FALSE_SWIPE); }
        TURN { MOVE(opponent, weather); MOVE(player, MOVE_MOONLIGHT); }
    } SCENE {
        HP_BAR(player, damage: 799);
        HP_BAR(player, damage: weather == MOVE_SUNNY_DAY ? -799 : weather == MOVE_RAIN_DANCE ? -300 : -600);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned also increases an ally's Moonlight healing")
{
    GIVEN {
        PLAYER(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Defense(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(9999); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_FALSE_SWIPE, target: playerRight); }
        TURN { MOVE(playerRight, MOVE_MOONLIGHT); }
    } SCENE {
        HP_BAR(playerRight, damage: 799);
        HP_BAR(playerRight, damage: -600);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned increases Lunar Blessing to three eighths HP for both recipients and retains its status cure")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); Item(item); HP(800); MaxHP(800); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Speed(5); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(199); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SUPER_FANG, target: playerLeft); MOVE(opponentRight, MOVE_SUPER_FANG, target: playerRight); }
        TURN { MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerRight); MOVE(opponentRight, MOVE_WILL_O_WISP, target: playerLeft); MOVE(playerLeft, MOVE_LUNAR_BLESSING); }
    } SCENE {
        HP_BAR(playerLeft, damage: 400);
        HP_BAR(playerRight, damage: 400);
        HP_BAR(playerLeft, damage: item == ITEM_APEX_METEORITE ? -300 : -200);
        HP_BAR(playerRight, damage: item == ITEM_APEX_METEORITE ? -300 : -200);
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(playerLeft->hp, item == ITEM_APEX_METEORITE ? 700 : 600);
        EXPECT_EQ(playerRight->hp, item == ITEM_APEX_METEORITE ? 700 : 600);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned does not boost unrelated recovery moves")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Defense(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(9999); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FALSE_SWIPE); }
        TURN { MOVE(player, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(player, damage: 799);
        HP_BAR(player, damage: -400);
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned's stronger Moonlight still respects Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Defense(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(9999); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FALSE_SWIPE); }
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(player, MOVE_MOONLIGHT, allowed: FALSE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: 799);
        NOT HP_BAR(player);
    }
}

DOUBLE_BATTLE_TEST("Moon-Attuned leaves Lunar Dance's full HP, PP and status restoration capped at their maxima")
{
    GIVEN {
        WITH_CONFIG(B_HEALING_WISH_SWITCH, GEN_7);
        PLAYER(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_CRESSELIA);
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Status1(STATUS1_POISON); MovesWithPP({MOVE_SCRATCH, 0}); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_LUNAR_DANCE); SEND_OUT(playerRight, 2); }
    } SCENE {
        HP_BAR(playerRight, hp: 0);
        HP_BAR(playerRight, hp: 100);
        STATUS_ICON(playerRight, none: TRUE);
    } THEN {
        EXPECT_EQ(playerRight->hp, 100);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->pp[0], GetMovePP(MOVE_SCRATCH));
    }
}

SINGLE_BATTLE_TEST("Moon-Attuned still cures and heals under Neutralizing Gas and cannot be suppressed")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Status1(STATUS1_PARALYSIS); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GASTRO_ACID); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOON_ATTUNED);
        HP_BAR(player, damage: -100);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->hp, 101);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_MOON_ATTUNED);
    }
}
