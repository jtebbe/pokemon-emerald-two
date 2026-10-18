#include "global.h"
#include "test/battle.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Frigid Claws inflicts frostbite, not freeze, with a 20 percent chance")
{
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(IsSlicingMove(MOVE_FRIGID_CLAWS));
        ASSUME(MoveMakesContact(MOVE_FRIGID_CLAWS));
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FRIGID_CLAWS); }
    } SCENE {
        HP_BAR(opponent);
        STATUS_ICON(opponent, frostbite: TRUE);
    }
}

SINGLE_BATTLE_TEST("Frigid Claws frostbite respects Ice immunity, Shield Dust, Covert Cloak and Substitute")
{
    u32 protection = 0;
    for (u32 j = 0; j < 4; j++)
        PARAMETRIZE { protection = j; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(protection == 0 ? SPECIES_GLALIE : SPECIES_WOBBUFFET) {
            Speed(100); HP(3000); MaxHP(3000);
            Ability(protection == 1 ? ABILITY_SHIELD_DUST : ABILITY_INNER_FOCUS);
            Item(protection == 2 ? ITEM_COVERT_CLOAK : ITEM_NONE);
        }
    } WHEN {
        TURN { MOVE(opponent, protection == 3 ? MOVE_SUBSTITUTE : MOVE_CELEBRATE); MOVE(player, MOVE_FRIGID_CLAWS, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Creeping Cuts hits twice and overrides only Fairy's Bug matchup", s16 damage)
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_CLEFAIRY; }
    PARAMETRIZE { species = SPECIES_TOGEKISS; }
    GIVEN {
        ASSUME(GetMoveStrikeCount(MOVE_CREEPING_CUTS) == 2);
        ASSUME(IsSlicingMove(MOVE_CREEPING_CUTS));
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_CREEPING_CUTS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CREEPING_CUTS, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CREEPING_CUTS, player);
        HP_BAR(opponent);
        MESSAGE("The Pokémon was hit 2 times!");
    } FINALLY {
        // Psychic and Fairy are both weak. Flying still halves the Fairy matchup.
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Creeping Cuts stops when its first hit knocks out the target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_CREEPING_CUTS); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CREEPING_CUTS, player);
        HP_BAR(opponent, hp: 0);
        MESSAGE("The opposing Wobbuffet fainted!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CREEPING_CUTS, player);
    }
}

SINGLE_BATTLE_TEST("Aura Pulse bypasses accuracy and evasion changes but not Protect")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_AURA_PULSE) == 0);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SAND_ATTACK); MOVE(opponent, MOVE_SAND_ATTACK); }
        TURN { MOVE(player, MOVE_AURA_PULSE); MOVE(opponent, protect ? MOVE_PROTECT : MOVE_DOUBLE_TEAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_ATTACK, player);
        if (protect) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
            NOT HP_BAR(opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AURA_PULSE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Fierce Rushdown gains priority at half HP including odd max HP")
{
    u32 hp;
    PARAMETRIZE { hp = 499; }
    PARAMETRIZE { hp = 500; }
    PARAMETRIZE { hp = 501; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(hp); MaxHP(1001); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIERCE_RUSHDOWN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (hp <= 500) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIERCE_RUSHDOWN, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIERCE_RUSHDOWN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Conditional priority is blocked by Psychic Terrain only when active")
{
    enum Move move;
    bool32 active;
    for (u32 j = 0; j < 2; j++) {
        PARAMETRIZE { move = MOVE_FIERCE_RUSHDOWN; active = j; }
        PARAMETRIZE { move = MOVE_DARK_IMITATION; active = j; }
        PARAMETRIZE { move = MOVE_SIMPLIFY; active = j; }
    }
    SetStartingStatus(STARTING_STATUS_PSYCHIC_TERRAIN);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(move == MOVE_SIMPLIFY && active ? SPECIES_GARDEVOIR : SPECIES_WOBBUFFET) {
            Speed(100); HP(active && move != MOVE_SIMPLIFY ? 500 : 1000); MaxHP(1000);
        }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (active) {
            if (move == MOVE_SIMPLIFY) {
                MESSAGE("The opposing Gardevoir is protected by the Psychic Terrain!");
            } else {
                MESSAGE("The opposing Wobbuffet is protected by the Psychic Terrain!");
            }
            NOT HP_BAR(opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, move, player);
        }
    }
}

DOUBLE_BATTLE_TEST("Conditional priority uses the selected target, not the other foe")
{
    u32 target;
    PARAMETRIZE { target = B_BATTLER_1; }
    PARAMETRIZE { target = B_BATTLER_3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FIERCE_RUSHDOWN, target: target == B_BATTLER_1 ? opponentLeft : opponentRight); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        if (target == B_BATTLER_1) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIERCE_RUSHDOWN, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FIERCE_RUSHDOWN, playerLeft);
        }
    }
}

SINGLE_BATTLE_TEST("Dark Imitation copies this turn's attack before or after the target acts without a damage boost", s16 damage)
{
    enum Move move;
    u32 speed;
    PARAMETRIZE { move = MOVE_TACKLE; speed = 200; }
    PARAMETRIZE { move = MOVE_DARK_IMITATION; speed = 200; }
    PARAMETRIZE { move = MOVE_DARK_IMITATION; speed = 50; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(player->moves[0], move);
        EXPECT_EQ(player->pp[0], GetMovePP(move) - 1);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Dark Imitation gains priority against damaged but not full-health targets")
{
    bool32 damaged;
    PARAMETRIZE { damaged = FALSE; }
    PARAMETRIZE { damaged = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(damaged ? 999 : 1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_IMITATION); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (damaged) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_IMITATION, player);
            HP_BAR(opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_IMITATION, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Dark Imitation can copy self-targeting status moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); HP(200); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(200); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_IMITATION); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_IMITATION, player);
        HP_BAR(player, hp: 700);
        HP_BAR(opponent, hp: 700);
    }
}

SINGLE_BATTLE_TEST("Dark Imitation retains its own priority when copying a priority attack")
{
    SetStartingStatus(STARTING_STATUS_PSYCHIC_TERRAIN);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_IMITATION); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        MESSAGE("Wobbuffet is protected by the Psychic Terrain!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_IMITATION, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Dark Imitation checks its own accuracy before copying a never-miss move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DARK_IMITATION, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_TEAM); }
        TURN { MOVE(player, MOVE_DARK_IMITATION, hit: FALSE); MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        MESSAGE("Wobbuffet used Dark Imitation!");
        MESSAGE("The opposing Wobbuffet avoided the attack!");
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_DARK_IMITATION) - 1);
    }
}

SINGLE_BATTLE_TEST("Dark Imitation fails safely against recursive callers or a switching target")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DARK_IMITATION; }
    PARAMETRIZE { move = MOVE_METRONOME; }
    PARAMETRIZE { move = MOVE_COPYCAT; }
    PARAMETRIZE { move = MOVE_NONE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_IMITATION); if (move == MOVE_NONE) { SWITCH(opponent, 1); } else { MOVE(opponent, move); } }
    } SCENE {
        MESSAGE("Wobbuffet used Dark Imitation!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Simplify turns all target types into pure Normal and counts distinct types for priority")
{
    bool32 dual;
    PARAMETRIZE { dual = FALSE; }
    PARAMETRIZE { dual = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(dual ? SPECIES_GARDEVOIR : SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SIMPLIFY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (dual) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SIMPLIFY, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SIMPLIFY, player);
        }
    } THEN {
        EXPECT_EQ(opponent->types[0], TYPE_NORMAL);
        EXPECT_EQ(opponent->types[1], TYPE_NORMAL);
        EXPECT_EQ(opponent->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Simplify removes an added third type even from a Normal target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_FORESTS_CURSE); }
        TURN { MOVE(player, MOVE_SIMPLIFY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FORESTS_CURSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SIMPLIFY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->types[2], TYPE_MYSTERY);
        EXPECT_EQ(opponent->types[0], TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Simplify fails on Terastallized or already pure Normal targets")
{
    bool32 tera;
    PARAMETRIZE { tera = FALSE; }
    PARAMETRIZE { tera = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); TeraType(TYPE_FIRE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SIMPLIFY); MOVE(opponent, MOVE_CELEBRATE, gimmick: tera ? GIMMICK_TERA : GIMMICK_NONE); }
    } SCENE {
        MESSAGE("Wobbuffet used Simplify!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conditional priority stacks with Prankster and never overrides Quash")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GARDEVOIR) { HP(500); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_PRANKSTER, MOVE_SIMPLIFY, B_BATTLER_1), 2);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_PRANKSTER, MOVE_DARK_IMITATION, B_BATTLER_1), 2);
        gProtectStructs[B_BATTLER_0].quash = TRUE;
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_PRANKSTER, MOVE_SIMPLIFY, B_BATTLER_1), -8);
        gProtectStructs[B_BATTLER_0].quash = FALSE;
    }
}
