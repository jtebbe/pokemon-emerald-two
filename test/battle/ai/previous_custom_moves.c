#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("Custom AI damage ranges match all six previous moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SERRATED_SLICE; }
    PARAMETRIZE { move = MOVE_DAZZLING_DASH; }
    PARAMETRIZE { move = MOVE_CORRODE; }
    PARAMETRIZE { move = MOVE_MIND_CRUSH; }
    PARAMETRIZE { move = MOVE_HAUNTING_SHRIEKS; }
    PARAMETRIZE { move = MOVE_VOLTAIC_RUSH; }
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(move == MOVE_CORRODE ? SPECIES_REGISTEEL : SPECIES_WOBBUFFET) {
            HP(3000); MaxHP(3000); Speed(100); Ability(ABILITY_LIGHT_METAL);
            Status1(move == MOVE_VOLTAIC_RUSH ? STATUS1_PARALYSIS : STATUS1_NONE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); SpAttack(100); SpDefense(250); Moves(move); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        // The turn-end prediction cache can already contain Corrode's new defensive typing/stages.
        struct BattlePokemon afterHit = *player;
        if (move == MOVE_CORRODE) {
            player->types[0] = TYPE_STEEL;
            player->types[1] = TYPE_STEEL;
            player->types[2] = TYPE_MYSTERY;
            player->statStages[STAT_DEF] = DEFAULT_STAT_STAGE;
            player->statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE;
        }
        struct AiCalcValues cv = { .move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain };
        struct SimulatedDamage expected = AI_CalcDamage(&cv, B_BATTLER_1, B_BATTLER_0);
        *player = afterHit;
        EXPECT_GE(damage, expected.minimum);
        EXPECT_LE(damage, expected.maximum);
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0], move == MOVE_DAZZLING_DASH ? 101 : 100);
    }
}

AI_SINGLE_BATTLE_TEST("Serrated Slice AI hazard credit requires Steel and a free Spikes layer")
{
    bool32 steel;
    PARAMETRIZE { steel = FALSE; }
    PARAMETRIZE { steel = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(steel ? SPECIES_REGISTEEL : SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SERRATED_SLICE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SERRATED_SLICE); }
    } THEN {
        for (u32 layers = 0; layers <= 3; layers++) {
            gSideTimers[B_SIDE_PLAYER].spikesAmount = layers;
            EXPECT_EQ(AI_ShouldSetUpHazards(B_BATTLER_1, B_BATTLER_0, MOVE_SERRATED_SLICE, gAiLogicData), steel && layers < 3);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Dazzling Dash AI values its Speed change with Contrary and Trick Room", s32 score)
{
    bool32 contrary, trickRoom;
    PARAMETRIZE { contrary = FALSE; trickRoom = FALSE; }
    PARAMETRIZE { contrary = TRUE; trickRoom = FALSE; }
    PARAMETRIZE { contrary = FALSE; trickRoom = TRUE; }
    PARAMETRIZE { contrary = TRUE; trickRoom = TRUE; }
    if (trickRoom)
        SetStartingStatus(STARTING_STATUS_TRICK_ROOM_TEMPORARY);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(contrary ? ABILITY_CONTRARY : ABILITY_SHADOW_TAG); Moves(MOVE_DAZZLING_DASH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_DAZZLING_DASH); }
    } THEN {
        results[i].score = gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
    } FINALLY {
        EXPECT_GT(results[0].score, results[1].score);
        EXPECT_GT(results[3].score, results[2].score);
    }
}

AI_SINGLE_BATTLE_TEST("Dazzling Dash AI respects critical-hit immunity", u32 damage)
{
    bool32 armor;
    PARAMETRIZE { armor = FALSE; }
    PARAMETRIZE { armor = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(50); Ability(armor ? ABILITY_BATTLE_ARMOR : ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_DAZZLING_DASH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_DAZZLING_DASH); }
    } THEN {
        results[i].damage = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].median;
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Corrode AI does not credit blocked defense drops", s32 score)
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_LIGHT_METAL; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_LIGHT_METAL; item = ITEM_CLEAR_AMULET; }
    PARAMETRIZE { ability = ABILITY_LIGHT_METAL; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_REGISTEEL) { HP(3000); MaxHP(3000); Ability(ability); Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CORRODE, MOVE_PSYCHIC); }
    } WHEN {
        TURN { SCORE_GT_VAL(opponent, MOVE_CORRODE, AI_SCORE_DEFAULT); }
    } THEN {
        results[i].score = gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
    } FINALLY {
        for (u32 j = 1; j < 5; j++)
            EXPECT_GT(results[0].score, results[j].score);
    }
}

AI_SINGLE_BATTLE_TEST("Corrode AI checks the actual type argument rather than mistaking it for Freeze-Dry", s32 score)
{
    enum Move attack;
    PARAMETRIZE { attack = MOVE_CORRODE; }
    PARAMETRIZE { attack = MOVE_FREEZE_DRY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SOAK, attack); }
    } WHEN {
        TURN { SCORE_GT_VAL(opponent, attack, 0); }
    } THEN {
        results[i].score = gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
    } FINALLY {
        EXPECT_GT(results[1].score, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Mind Crush AI values Sp. Def setup but not Sp. Atk setup")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_MIND_CRUSH, MOVE_AMNESIA, MOVE_NASTY_PLOT); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_AMNESIA, MOVE_NASTY_PLOT); }
    } THEN {
        EXPECT_EQ(IncreaseStatUpScore(B_BATTLER_1, B_BATTLER_0, STAT_SPATK, 2), NO_INCREASE);
        EXPECT_GT(IncreaseStatUpScore(B_BATTLER_1, B_BATTLER_0, STAT_SPDEF, 2), NO_INCREASE);
    }
}

AI_SINGLE_BATTLE_TEST("Haunting Shrieks AI sums the actual damage from different fainted teammates")
{
    s16 damage[3];
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_HAUNTING_SHRIEKS); }
        OPPONENT(SPECIES_GASTLY) { HP(0); SpAttack(300); }
        OPPONENT(SPECIES_RATTATA) { HP(0); SpAttack(200); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        for (u32 j = 0; j < 3; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, opponent);
            HP_BAR(player, captureDamage: &damage[j]);
        }
    } THEN {
        u32 total = damage[0] + damage[1] + damage[2];
        struct SimulatedDamage expected = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
        EXPECT_GE(total, expected.minimum);
        EXPECT_LE(total, expected.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Haunting Shrieks AI respects Soundproof and Ghost immunity")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SOUNDPROOF; }
    PARAMETRIZE { species = SPECIES_RATTATA; ability = ABILITY_RUN_AWAY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(species) { HP(3000); MaxHP(3000); Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HAUNTING_SHRIEKS, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_GASTLY) { HP(0); SpAttack(300); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].median, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Voltaic Rush AI recognizes paralysis doubling its damage", u32 damage)
{
    bool32 paralyzed;
    PARAMETRIZE { paralyzed = FALSE; }
    PARAMETRIZE { paralyzed = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Status1(paralyzed ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_VOLTAIC_RUSH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_VOLTAIC_RUSH); }
    } THEN {
        // An actual hit may have inflicted paralysis; compare the two original states explicitly.
        u32 status = player->status1;
        player->status1 = paralyzed ? STATUS1_PARALYSIS : STATUS1_NONE;
        struct AiCalcValues cv = { .move = MOVE_VOLTAIC_RUSH, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain };
        results[i].damage = AI_CalcDamage(&cv, B_BATTLER_1, B_BATTLER_0).median;
        player->status1 = status;
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

AI_DOUBLE_BATTLE_TEST("Previous custom move AI chooses an opposing KO target, not its ally")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SERRATED_SLICE; }
    PARAMETRIZE { move = MOVE_DAZZLING_DASH; }
    PARAMETRIZE { move = MOVE_CORRODE; }
    PARAMETRIZE { move = MOVE_MIND_CRUSH; }
    PARAMETRIZE { move = MOVE_HAUNTING_SHRIEKS; }
    PARAMETRIZE { move = MOVE_VOLTAIC_RUSH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(50); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(50); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, move, target: playerLeft); SEND_OUT(playerLeft, 2); }
    }
}

AI_SINGLE_BATTLE_TEST("Dazzling Dash AI prefers its perfect accuracy when both moves can KO")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(40); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Attack(200); Speed(200); Moves(MOVE_DAZZLING_DASH, MOVE_MEGA_KICK); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_DAZZLING_DASH); }
    }
}

AI_SINGLE_BATTLE_TEST("Corrode AI evaluation does not change real typing when it selects a different move")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_STEELIX) { HP(200); MaxHP(200); SpDefense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); SpAttack(200); Moves(MOVE_CORRODE, MOVE_SURF); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SURF); }
    } THEN {
        EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_0, TYPE_STEEL));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("Corrode AI recognizes Contrary and Mold Breaker", s32 score)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_REGISTEEL) { HP(3000); MaxHP(3000); Ability(ABILITY_CONTRARY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_CORRODE, MOVE_PSYCHIC); }
    } WHEN {
        TURN { SCORE_GT_VAL(opponent, MOVE_CORRODE, 0); }
    } THEN {
        results[i].score = gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
    } FINALLY {
        EXPECT_GT(results[1].score, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Haunting Shrieks AI sees a KO through Focus Sash with a fainted contributor")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(60); SpDefense(100); Item(ITEM_FOCUS_SASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); SpAttack(300); Moves(MOVE_HAUNTING_SHRIEKS, MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_GASTLY) { HP(0); SpAttack(1); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_HAUNTING_SHRIEKS); }
    }
}

AI_SINGLE_BATTLE_TEST("Mind Crush and Voltaic Rush AI avoid immune targets")
{
    enum Move move;
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_MIND_CRUSH; species = SPECIES_UMBREON; ability = ABILITY_SYNCHRONIZE; }
    PARAMETRIZE { move = MOVE_VOLTAIC_RUSH; species = SPECIES_SANDSHREW; ability = ABILITY_SAND_VEIL; }
    PARAMETRIZE { move = MOVE_VOLTAIC_RUSH; species = SPECIES_WOBBUFFET; ability = ABILITY_VOLT_ABSORB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(species) { HP(3000); MaxHP(3000); Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].median, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Voltaic Rush makes paralysis setup more valuable for its partner", s32 score)
{
    bool32 voltaic;
    PARAMETRIZE { voltaic = FALSE; }
    PARAMETRIZE { voltaic = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); Speed(50); }
        PLAYER(SPECIES_WOBBUFFET) { HP(9999); MaxHP(9999); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(voltaic ? MOVE_VOLTAIC_RUSH : MOVE_SPARK); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN, target: playerLeft); SKIP_TURN(opponentRight); }
    } THEN {
        u32 status = playerLeft->status1;
        playerLeft->status1 = STATUS1_NONE;
        results[i].score = 0;
        IncreaseParalyzeScore(B_BATTLER_1, B_BATTLER_0, MOVE_THUNDER_WAVE, &results[i].score);
        playerLeft->status1 = status;
    } FINALLY {
        EXPECT_GT(results[1].score, results[0].score);
    }
}
