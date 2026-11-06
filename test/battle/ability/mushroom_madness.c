#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Mushroom Madness is the Apex ability of Paras and Parasect")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_PARAS; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PARASECT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PARASECT; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_MUSHROOM_MADNESS, item == ITEM_APEX_METEORITE);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness redirects attacks before its first action and stays active every turn")
{
    GIVEN {
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN {
        for (u32 j = 0; j < 2; j++)
            TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); MOVE(opponentRight, MOVE_WATER_GUN, target: playerRight); }
    } SCENE {
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
            HP_BAR(playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponentRight);
            HP_BAR(playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].followmeTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness starts redirecting immediately when switched in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        SEND_IN_MESSAGE("Paras");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        HP_BAR(playerLeft);
        NOT HP_BAR(playerRight);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness stops redirecting as soon as its holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        HP_BAR(playerRight);
        NOT HP_BAR(playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness respects Grass typing, Overcoat, and Safety Goggles on the attacker")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    bool32 immune;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_NONE; immune = FALSE; }
    PARAMETRIZE { species = SPECIES_BULBASAUR; ability = ABILITY_NONE; item = ITEM_NONE; immune = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; immune = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; immune = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        if (immune) HP_BAR(playerRight);
        else HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->hp == playerLeft->maxHP, immune);
        EXPECT_EQ(playerRight->hp == playerRight->maxHP, !immune);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness respects Stalwart, Propeller Tail, and Snipe Shot but cannot be bypassed by Mold Breaker")
{
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { ability = ABILITY_STALWART; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_PROPELLER_TAIL; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_SNIPE_SHOT; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; move = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, move, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        if (ability == ABILITY_MOLD_BREAKER) HP_BAR(playerLeft);
        else HP_BAR(playerRight);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness does not redirect spread moves or its own side's attacks")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(3); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SURF); MOVE(playerRight, MOVE_SCRATCH, target: opponentRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, opponentLeft);
        HP_BAR(playerLeft);
        HP_BAR(playerRight);
        HP_BAR(opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness redirects status moves as well as attacks")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, opponentLeft);
        STATUS_ICON(playerLeft, paralysis: TRUE);
    } THEN {
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT(playerLeft->status1 & STATUS1_PARALYSIS);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness yields to an active Follow Me and resumes on the next turn")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_FOLLOW_ME); MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOLLOW_ME, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        HP_BAR(playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        HP_BAR(playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness chooses the first holder in turn order when both allies have it")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(49); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        HP_BAR(playerLeft);
        NOT HP_BAR(playerRight);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness ceases redirection after its holder faints")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); MOVE(opponentRight, MOVE_WATER_GUN, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        HP_BAR(playerLeft, hp: 0);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponentRight);
        HP_BAR(playerRight);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness doubles physical and special damage after any opposing attack deals HP damage", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    enum Move incoming = MOVE_CELEBRATE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    const enum Move incomingMoves[] = {MOVE_CELEBRATE, MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_SEISMIC_TOSS};
    for (u32 a = 0; a < ARRAY_COUNT(moves); a++)
        for (u32 b = 0; b < ARRAY_COUNT(incomingMoves); b++)
            PARAMETRIZE { move = moves[a]; incoming = incomingMoves[b]; }
    GIVEN {
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); Attack(300); SpAttack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(opponent, incoming); MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 a = 0; a < ARRAY_COUNT(moves); a++)
            for (u32 b = 1; b < ARRAY_COUNT(incomingMoves); b++)
                EXPECT_MUL_EQ(results[a * 4].damage, UQ_4_12(2.0), results[a * 4 + b].damage);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness does not boost an attack used before the opponent hits it", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, incoming); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness resets its damage bonus at the start of the next turn")
{
    s16 boosted, normal;
    GIVEN {
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); MOVE(player, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &boosted);
        HP_BAR(opponent, captureDamage: &normal);
    } THEN {
        EXPECT_MUL_EQ(normal, UQ_4_12(2.0), boosted);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness does not activate from misses, paralysis, or a zero damage False Swipe", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_THUNDER; }
    PARAMETRIZE { incoming = MOVE_FALSE_SWIPE; }
    PARAMETRIZE { incoming = MOVE_THUNDER_WAVE; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, incoming, hit: incoming != MOVE_THUNDER); MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[0].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness does not activate from damage absorbed by a Substitute", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(60); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(opponent, incoming); MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness does not activate from an ally's attack", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
    } WHEN {
        TURN { MOVE(playerRight, incoming, target: incoming == MOVE_SCRATCH ? playerLeft : playerRight); MOVE(playerLeft, MOVE_WATER_GUN, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness can boost damage against a foe other than the attacker", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(3); }
    } WHEN {
        TURN { MOVE(opponentLeft, incoming, target: incoming == MOVE_SCRATCH ? playerRight : opponentLeft); MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness keeps its bonus after healing back to full HP", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
    } WHEN {
        TURN { MOVE(opponentLeft, incoming, target: incoming == MOVE_SCRATCH ? playerRight : opponentLeft); MOVE(playerRight, MOVE_HEAL_PULSE, target: playerLeft); MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness stacks with Revenge's own power boost", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_PARASECT) { Ability(ABILITY_DAMP); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); MOVE(player, MOVE_REVENGE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness does not multiply fixed damage")
{
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); Level(60); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); MOVE(player, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(opponent, damage: 60);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness is not primed by recoil before an Instructed second attack")
{
    s16 first, second;
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(49); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft); MOVE(playerRight, MOVE_INSTRUCT, target: playerLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &first);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &second);
    } THEN {
        EXPECT_EQ(first, second);
    }
}

SINGLE_BATTLE_TEST("Mushroom Madness is not primed by poison damage on the previous turn", s16 damage)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_POISON; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness is neither stacked nor erased by further opposing or allied hits", s16 damage)
{
    u32 extraHit;
    PARAMETRIZE { extraHit = 0; }
    PARAMETRIZE { extraHit = 1; }
    PARAMETRIZE { extraHit = 2; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight);
            MOVE(opponentRight, extraHit == 2 ? MOVE_SCRATCH : MOVE_CELEBRATE, target: extraHit == 2 ? playerRight : opponentRight);
            MOVE(playerRight, extraHit == 1 ? MOVE_SCRATCH : MOVE_CELEBRATE, target: extraHit == 1 ? playerLeft : playerRight);
            MOVE(playerLeft, MOVE_WATER_GUN, target: opponentLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

DOUBLE_BATTLE_TEST("Mushroom Madness redirects random-target moves and both Dragon Darts hits")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_OUTRAGE; }
    PARAMETRIZE { move = MOVE_DRAGON_DARTS; }
    GIVEN {
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (move == MOVE_DRAGON_DARTS)
            TURN { MOVE(opponentLeft, move, target: playerRight); }
        else
            TURN { MOVE(opponentLeft, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        HP_BAR(playerLeft);
        if (move == MOVE_DRAGON_DARTS) HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}
