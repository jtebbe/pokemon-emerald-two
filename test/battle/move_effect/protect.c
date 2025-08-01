#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_PROTECT) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_DETECT) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_KINGS_SHIELD) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_SILK_TRAP) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_SPIKY_SHIELD) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_WIDE_GUARD) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_QUICK_GUARD) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_CRAFTY_SHIELD) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_BANEFUL_BUNKER) == EFFECT_PROTECT);
    ASSUME(GetMoveEffect(MOVE_BURNING_BULWARK) == EFFECT_PROTECT);
    ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(MoveMakesContact(MOVE_SCRATCH));
    ASSUME(GetMoveCategory(MOVE_LEER) == DAMAGE_CATEGORY_STATUS);
    ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
    ASSUME(!(MoveMakesContact(MOVE_WATER_GUN)));
}

SINGLE_BATTLE_TEST("Protect: Protect, Detect, Spiky Shield, Baneful Bunker and Burning Bulwark protect from all moves")
{
    u32 j;
    static const u16 protectMoves[] = {
        MOVE_PROTECT,
        MOVE_DETECT,
        MOVE_SPIKY_SHIELD,
        MOVE_BANEFUL_BUNKER,
        MOVE_BURNING_BULWARK,
    };
    u16 protectMove = MOVE_NONE;
    u16 usedMove = MOVE_NONE;

    for (j = 0; j < ARRAY_COUNT(protectMoves); j++)
    {
        PARAMETRIZE { protectMove = protectMoves[j]; usedMove = MOVE_SCRATCH; }
        PARAMETRIZE { protectMove = protectMoves[j]; usedMove = MOVE_LEER; }
        PARAMETRIZE { protectMove = protectMoves[j]; usedMove = MOVE_WATER_GUN; }
    }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, protectMove); MOVE(player, usedMove); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, protectMove, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
        MESSAGE("The opposing Wobbuffet protected itself!");
        if (usedMove == MOVE_LEER) {
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        } else {
            NOT HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Protect: King's Shield, Silk Trap and Obstruct protect from damaging moves and lower stats on contact")
{
    u32 j;
    static const u16 protectMoves[][3] =
    {   // Move             Stat      Stages
        {MOVE_KINGS_SHIELD, STAT_ATK,   (B_KINGS_SHIELD_LOWER_ATK >= GEN_8) ? 1 : 2},
        {MOVE_SILK_TRAP,    STAT_SPEED, 1},
        {MOVE_OBSTRUCT,     STAT_DEF,   2},
    };
    u16 protectMove = MOVE_NONE;
    u16 usedMove = MOVE_NONE;
    u16 statId = 0, lowersBy = 0;

    for (j = 0; j < ARRAY_COUNT(protectMoves); j++)
    {
        PARAMETRIZE { usedMove = MOVE_SCRATCH; protectMove = protectMoves[j][0]; statId = protectMoves[j][1]; lowersBy = protectMoves[j][2]; }
        PARAMETRIZE { usedMove = MOVE_LEER; protectMove = protectMoves[j][0]; statId = 0; lowersBy = 0; }
        PARAMETRIZE { usedMove = MOVE_WATER_GUN; protectMove = protectMoves[j][0]; statId = 0; lowersBy = 0; }
    }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, protectMove); MOVE(player, usedMove); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, protectMove, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        if (usedMove == MOVE_LEER) {
            ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
            NOT MESSAGE("The opposing Wobbuffet protected itself!");
        } else {
            NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
            MESSAGE("The opposing Wobbuffet protected itself!");
            if (usedMove == MOVE_SCRATCH) {
                NOT HP_BAR(opponent);
                ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
                if (statId == STAT_ATK) {
                    #if B_KINGS_SHIELD_LOWER_ATK >= GEN_8
                    MESSAGE("Wobbuffet's Attack fell!");
                    #else
                    MESSAGE("Wobbuffet's Attack harshly fell!");
                    #endif
                } else if (statId == STAT_SPEED) {
                    MESSAGE("Wobbuffet's Speed fell!");
                } else if (statId == STAT_DEF) {
                    if (lowersBy == 2) {
                        MESSAGE("Wobbuffet's Defense harshly fell!");
                    }
                }
            } else {
                NONE_OF {
                    HP_BAR(opponent);
                    ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
                }
            }
        }
    } THEN {
        if (usedMove == MOVE_SCRATCH) {
            EXPECT_EQ(player->statStages[statId], DEFAULT_STAT_STAGE - lowersBy);
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Spiky Shield does 1/8 dmg of max hp of attackers making contact and may faint them")
{
    u16 usedMove = MOVE_NONE;
    u16 hp = 400, maxHp = 400;

    PARAMETRIZE { usedMove = MOVE_SCRATCH; hp = 1; }
    PARAMETRIZE { usedMove = MOVE_SCRATCH; }
    PARAMETRIZE { usedMove = MOVE_LEER; }
    PARAMETRIZE { usedMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(maxHp); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (hp == 1) {
            TURN { MOVE(opponent, MOVE_SPIKY_SHIELD); MOVE(player, usedMove); SEND_OUT(player, 1); }
        } else {
            TURN { MOVE(opponent, MOVE_SPIKY_SHIELD); MOVE(player, usedMove); }
        }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKY_SHIELD, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT HP_BAR(opponent);
        if (usedMove == MOVE_SCRATCH) {
            HP_BAR(player, maxHp / 8);
            if (hp == 1) {
                MESSAGE("Wobbuffet fainted!");
                SEND_IN_MESSAGE("Wobbuffet");
            }
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Baneful Bunker poisons pokemon for moves making contact")
{
    u16 usedMove = MOVE_NONE;

    PARAMETRIZE {usedMove = MOVE_SCRATCH; }
    PARAMETRIZE {usedMove = MOVE_LEER; }
    PARAMETRIZE {usedMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BANEFUL_BUNKER); MOVE(player, usedMove); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BANEFUL_BUNKER, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
        MESSAGE("The opposing Wobbuffet protected itself!");
        if (usedMove == MOVE_SCRATCH) {
            NOT HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_POISON);
        } else {
            NONE_OF {
                HP_BAR(opponent);
                STATUS_ICON(player, STATUS1_POISON);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Baneful Bunker can't poison pokemon if they are already statused")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WILL_O_WISP); }
        TURN { MOVE(opponent, MOVE_BANEFUL_BUNKER); MOVE(player, MOVE_SCRATCH); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, opponent);
        STATUS_ICON(player, STATUS1_BURN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BANEFUL_BUNKER, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_POISON);
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Burning Bulwark burns pokemon for moves making contact")
{
    u16 usedMove = MOVE_NONE;

    PARAMETRIZE {usedMove = MOVE_SCRATCH; }
    PARAMETRIZE {usedMove = MOVE_LEER; }
    PARAMETRIZE {usedMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BURNING_BULWARK); MOVE(player, usedMove); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_BULWARK, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, usedMove, player);
        MESSAGE("The opposing Wobbuffet protected itself!");
        if (usedMove == MOVE_SCRATCH) {
            NOT HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_BURN);
        } else {
            NONE_OF {
                HP_BAR(opponent);
                STATUS_ICON(player, STATUS1_BURN);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Burning Bulwark can't burn pokemon if they are already statused")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(opponent, MOVE_BURNING_BULWARK); MOVE(player, MOVE_SCRATCH); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, opponent);
        STATUS_ICON(player, STATUS1_TOXIC_POISON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_BULWARK, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_BURN);
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Recoil damage is not applied if target was protected")
{
    u32 j, k;
    static const u16 protectMoves[] = { MOVE_PROTECT, MOVE_DETECT, MOVE_KINGS_SHIELD, MOVE_BANEFUL_BUNKER, MOVE_SILK_TRAP, MOVE_OBSTRUCT, MOVE_SPIKY_SHIELD };
    static const u16 recoilMoves[] = { MOVE_VOLT_TACKLE, MOVE_HEAD_SMASH, MOVE_TAKE_DOWN, MOVE_DOUBLE_EDGE };
    u16 protectMove = MOVE_NONE;
    u16 recoilMove = MOVE_NONE;

    for (j = 0; j < ARRAY_COUNT(protectMoves); j++)
    {
        for (k = 0; k < ARRAY_COUNT(recoilMoves); k++)
        {
            PARAMETRIZE { protectMove = protectMoves[j]; recoilMove = recoilMoves[k]; }
        }
    }


    GIVEN {
        ASSUME(GetMoveRecoil(MOVE_VOLT_TACKLE) > 0);
        ASSUME(GetMoveRecoil(MOVE_HEAD_SMASH) > 0);
        ASSUME(GetMoveRecoil(MOVE_TAKE_DOWN) > 0);
        ASSUME(GetMoveRecoil(MOVE_DOUBLE_EDGE) > 0);
        PLAYER(SPECIES_RAPIDASH);
        OPPONENT(SPECIES_BEAUTIFLY);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(opponent, protectMove); MOVE(player, recoilMove); }
        TURN {}
    } SCENE {
        // 1st turn
        MESSAGE("The opposing Beautifly used Scratch!");
        MESSAGE("Rapidash used Scratch!");
        // 2nd turn
        ANIMATION(ANIM_TYPE_MOVE, protectMove, opponent);
        MESSAGE("The opposing Beautifly protected itself!");
        // MESSAGE("Rapidash used recoilMove!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, recoilMove, player);
            MESSAGE("Rapidash was damaged by the recoil!");
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Multi-hit moves don't hit a protected target and fail only once")
{
    u16 move = MOVE_NONE;

    PARAMETRIZE { move = MOVE_PROTECT; }
    PARAMETRIZE { move = MOVE_DETECT; }
    PARAMETRIZE { move = MOVE_KINGS_SHIELD; }
    PARAMETRIZE { move = MOVE_BANEFUL_BUNKER; }
    PARAMETRIZE { move = MOVE_SILK_TRAP; }
    PARAMETRIZE { move = MOVE_OBSTRUCT; }
    PARAMETRIZE { move = MOVE_SPIKY_SHIELD; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ARM_THRUST) == EFFECT_MULTI_HIT);
        PLAYER(SPECIES_RAPIDASH);
        OPPONENT(SPECIES_BEAUTIFLY);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_ARM_THRUST); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        MESSAGE("The opposing Beautifly protected itself!");
        MESSAGE("Rapidash used Arm Thrust!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_ARM_THRUST, player);
        MESSAGE("The opposing Beautifly protected itself!");
        // Each effect happens only once.
        if (move == MOVE_KINGS_SHIELD || move == MOVE_SILK_TRAP || move == MOVE_OBSTRUCT) {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        } else if (move == MOVE_SPIKY_SHIELD) {
            HP_BAR(player);
        } else if (move == MOVE_BANEFUL_BUNKER) {
            STATUS_ICON(player, STATUS1_POISON);
        }
        NONE_OF {
            if (move == MOVE_KINGS_SHIELD || move == MOVE_SILK_TRAP || move == MOVE_OBSTRUCT) {
                ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            } else if (move == MOVE_SPIKY_SHIELD) {
                HP_BAR(player);
            }
            MESSAGE("The Pokémon was hit 2 time(s)!");
            MESSAGE("The Pokémon was hit 3 time(s)!");
            MESSAGE("The Pokémon was hit 4 time(s)!");
            MESSAGE("The Pokémon was hit 5 time(s)!");
        }
    }
}

DOUBLE_BATTLE_TEST("Protect: Wide Guard protects self and ally from multi-target moves")
{
    u16 move = MOVE_NONE;

    PARAMETRIZE { move = MOVE_SCRATCH; }      // Single target
    PARAMETRIZE { move = MOVE_SURF; }        // All targets
    PARAMETRIZE { move = MOVE_HYPER_VOICE; } // 2 foes

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_SCRATCH) == MOVE_TARGET_SELECTED);
        ASSUME(GetMoveTarget(MOVE_SURF) == MOVE_TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveTarget(MOVE_HYPER_VOICE) == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WIDE_GUARD); MOVE(playerLeft, move, target: opponentLeft); }
        TURN {}
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Wide Guard!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WIDE_GUARD, opponentLeft);
        if (move == MOVE_SCRATCH) {
            MESSAGE("Wobbuffet used Scratch!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
            HP_BAR(opponentLeft);
        } else if (move == MOVE_HYPER_VOICE) {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, playerLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentRight);
        } else { // Surf
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentLeft);
            HP_BAR(playerRight);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentRight);
        }
    }
}

DOUBLE_BATTLE_TEST("Protect: Wide Guard can not fail on consecutive turns")
{
    u8 turns;

    PASSES_RANDOMLY(2, 2);
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_HYPER_VOICE) == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_WIDE_GUARD); MOVE(playerLeft, MOVE_HYPER_VOICE, target: opponentLeft); }
        TURN { MOVE(opponentLeft, MOVE_WIDE_GUARD); MOVE(playerLeft, MOVE_HYPER_VOICE, target: opponentLeft); }
        TURN {}
    } SCENE {
        for (turns = 0; turns < 2; turns++) {
            MESSAGE("The opposing Wobbuffet used Wide Guard!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WIDE_GUARD, opponentLeft);
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, playerLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentRight);
        }
    }
}

DOUBLE_BATTLE_TEST("Protect: Quick Guard protects self and ally from priority moves")
{
    u16 move = MOVE_NONE;
    struct BattlePokemon *targetOpponent = NULL;

    PARAMETRIZE { move = MOVE_SCRATCH; targetOpponent = opponentLeft; }
    PARAMETRIZE { move = MOVE_SCRATCH; targetOpponent = opponentRight; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; targetOpponent = opponentLeft; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; targetOpponent = opponentRight; }

    GIVEN {
        ASSUME(GetMovePriority(MOVE_SCRATCH) == 0);
        ASSUME(GetMovePriority(MOVE_QUICK_ATTACK) == 1);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_QUICK_GUARD); MOVE(playerLeft, move, target:targetOpponent); }
        TURN {}
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Quick Guard!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_GUARD, opponentLeft);
        if (move == MOVE_SCRATCH) {
            MESSAGE("Wobbuffet used Scratch!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
            HP_BAR(targetOpponent);
        } else if (move == MOVE_QUICK_ATTACK) {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(targetOpponent);
        }
    }
}

DOUBLE_BATTLE_TEST("Protect: Quick Guard can not fail on consecutive turns")
{
    u8 turns;

    PASSES_RANDOMLY(2, 2);
    GIVEN {
        ASSUME(GetMovePriority(MOVE_QUICK_ATTACK) == 1);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_QUICK_GUARD); MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight); }
        TURN { MOVE(opponentLeft, MOVE_QUICK_GUARD); MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight); }
    } SCENE {
        for (turns = 0; turns < 2; turns++) {
            MESSAGE("The opposing Wobbuffet used Quick Guard!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_GUARD, opponentLeft);
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT HP_BAR(opponentRight);
        }
    }
}

DOUBLE_BATTLE_TEST("Protect: Crafty Shield protects self and ally from status moves")
{
    u16 move = MOVE_NONE;
    struct BattlePokemon *targetOpponent = NULL;

    PARAMETRIZE { move = MOVE_HYPER_VOICE; }
    PARAMETRIZE { move = MOVE_LEER; }
    PARAMETRIZE { move = MOVE_SCRATCH; targetOpponent = opponentLeft; }
    PARAMETRIZE { move = MOVE_SCRATCH; targetOpponent = opponentRight; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_LEER) == MOVE_TARGET_BOTH);
        ASSUME(GetMoveTarget(MOVE_HYPER_VOICE) == MOVE_TARGET_BOTH);
        ASSUME(GetMoveCategory(MOVE_HYPER_VOICE) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CRAFTY_SHIELD); (move == MOVE_SCRATCH) ? MOVE(playerLeft, move, target:targetOpponent) : MOVE(playerLeft, move); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRAFTY_SHIELD, opponentLeft);
        if (move == MOVE_LEER) {
            MESSAGE("Wobbuffet used Leer!");
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
            MESSAGE("The opposing Wobbuffet protected itself!");
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
        } else {
            if (move == MOVE_HYPER_VOICE || targetOpponent == opponentLeft) {
                NOT MESSAGE("The opposing Wobbuffet protected itself!");
                HP_BAR(opponentLeft);
            } else if (move == MOVE_HYPER_VOICE || targetOpponent == opponentRight) {
                NOT MESSAGE("The opposing Wobbuffet protected itself!");
                HP_BAR(opponentRight);
            }
        }
    }
}

SINGLE_BATTLE_TEST("Protect: Protect does not block Confide or Decorate")
{
    u32 move;
    PARAMETRIZE { move = MOVE_CONFIDE; }
    PARAMETRIZE { move = MOVE_DECORATE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_CONFIDE) == EFFECT_SPECIAL_ATTACK_DOWN);
        ASSUME(MoveIgnoresProtect(MOVE_CONFIDE));
        ASSUME(GetMoveEffect(MOVE_DECORATE) == EFFECT_DECORATE);
        ASSUME(MoveIgnoresProtect(MOVE_DECORATE));
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        NOT MESSAGE("The opposing Wobbuffet protected itself!");
    }
}

DOUBLE_BATTLE_TEST("Crafty Shield protects self and ally from Confide and Decorate")
{
    u32 move;
    PARAMETRIZE { move = MOVE_CONFIDE; }
    PARAMETRIZE { move = MOVE_DECORATE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_CONFIDE) == EFFECT_SPECIAL_ATTACK_DOWN);
        ASSUME(MoveIgnoresProtect(MOVE_CONFIDE));
        ASSUME(GetMoveEffect(MOVE_DECORATE) == EFFECT_DECORATE);
        ASSUME(MoveIgnoresProtect(MOVE_DECORATE));
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CRAFTY_SHIELD); MOVE(playerLeft, move, target: opponentLeft); MOVE(playerRight, move, target: opponentRight); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, move, playerRight);
        MESSAGE("The opposing Wynaut protected itself!");
    }
}

DOUBLE_BATTLE_TEST("Crafty Shield does not protect against moves that target all battlers")
{
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_TANGELA].types[0] == TYPE_GRASS);
        ASSUME(gSpeciesInfo[SPECIES_TANGROWTH].types[0] == TYPE_GRASS);
        ASSUME(gSpeciesInfo[SPECIES_SUNKERN].types[0] == TYPE_GRASS);
        ASSUME(gSpeciesInfo[SPECIES_SUNFLORA].types[0] == TYPE_GRASS);
        PLAYER(SPECIES_TANGELA);
        PLAYER(SPECIES_TANGROWTH);
        OPPONENT(SPECIES_SUNKERN);
        OPPONENT(SPECIES_SUNFLORA);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CRAFTY_SHIELD); MOVE(opponentRight, MOVE_CELEBRATE); MOVE(playerLeft, MOVE_FLOWER_SHIELD); MOVE(playerRight, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Tangela used Flower Shield!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        MESSAGE("Tangela's Defense sharply rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        MESSAGE("The opposing Sunkern's Defense sharply rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
        MESSAGE("Tangrowth's Defense sharply rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
        MESSAGE("The opposing Sunflora's Defense sharply rose!");
    }
}

SINGLE_BATTLE_TEST("Protect: Quick Guard, Wide Guard and Crafty Shield don't reduce Max Move demage", s16 damage)
{
    s16 dmg[2];
    u32 move;

    PARAMETRIZE { move = MOVE_WIDE_GUARD; }
    PARAMETRIZE { move = MOVE_QUICK_GUARD; }
    PARAMETRIZE { move = MOVE_CRAFTY_SHIELD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, gimmick: GIMMICK_DYNAMAX); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &dmg[0]);
        HP_BAR(opponent, captureDamage: &dmg[1]);
    } FINALLY {
        EXPECT_EQ(dmg[0], dmg[1]);
    }
}

SINGLE_BATTLE_TEST("Protect: Quick Guard, Wide Guard and Crafty Shield don't reduce Z-Move demage", s16 damage)
{
    bool32 protected;
    u32 move;

    PARAMETRIZE { protected = TRUE; move = MOVE_WIDE_GUARD; }
    PARAMETRIZE { protected = FALSE; move = MOVE_WIDE_GUARD; }

    PARAMETRIZE { protected = TRUE; move = MOVE_QUICK_GUARD; }
    PARAMETRIZE { protected = FALSE; move = MOVE_QUICK_GUARD; }

    PARAMETRIZE { protected = TRUE; move = MOVE_CRAFTY_SHIELD; }
    PARAMETRIZE { protected = FALSE; move = MOVE_CRAFTY_SHIELD; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_SCRATCH) == TYPE_NORMAL);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_NORMALIUM_Z); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (protected)
            TURN { MOVE(player, MOVE_SCRATCH, gimmick: GIMMICK_Z_MOVE); MOVE(opponent, move); }
        else
            TURN { MOVE(player, MOVE_SCRATCH, gimmick: GIMMICK_Z_MOVE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Protect: Protective Pads protects from secondary effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PROTECTIVE_PADS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BURNING_BULWARK); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_BULWARK, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
            STATUS_ICON(player, STATUS1_BURN);
        }
    }
}
