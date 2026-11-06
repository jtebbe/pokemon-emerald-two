#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sMushroomDamage;
static bool32 sMushroomObserved;
static u32 sMushroomTurn;
static s32 ObserveMushroom(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sMushroomTurn && !gAiLogicData->aiPredictionInProgress)
    {
        sMushroomObserved = TRUE;
        sMushroomDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Mushroom Madness AI predicts physical and special retaliation only after an earlier damaging attack", s16 damage)
{
    enum Move move = MOVE_SCRATCH, incoming = MOVE_CELEBRATE;
    u32 ownSpeed = 1, foeSpeed = 100;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; incoming = MOVE_CELEBRATE; ownSpeed = 1; foeSpeed = 100; }
        PARAMETRIZE { move = moves[j]; incoming = MOVE_SCRATCH; ownSpeed = 1; foeSpeed = 100; }
        PARAMETRIZE { move = moves[j]; incoming = MOVE_SCRATCH; ownSpeed = 100; foeSpeed = 1; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(foeSpeed); Moves(incoming); }
        OPPONENT(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Attack(300); SpAttack(300); Speed(ownSpeed); Moves(move); }
    } WHEN {
        sMushroomObserved = FALSE;
        sMushroomTurn = 0;
        sDynamicAiFunc = ObserveMushroom;
        TURN { MOVE(player, incoming); EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMushroomObserved);
        EXPECT_GE(results[i].damage, sMushroomDamage.minimum);
        EXPECT_LE(results[i].damage, sMushroomDamage.maximum);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
            EXPECT_MUL_EQ(results[j * 3].damage, UQ_4_12(2.0), results[j * 3 + 1].damage);
            EXPECT_EQ(results[j * 3].damage, results[j * 3 + 2].damage);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Mushroom Madness AI chooses a boosted slower attack over unboosted priority")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Attack(300); SpAttack(300); Speed(1); Moves(MOVE_QUICK_ATTACK, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_QUICK_ATTACK); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("Mushroom Madness AI does not forecast retaliation from zero damage False Swipe", s16 damage)
{
    enum Move incoming;
    PARAMETRIZE { incoming = MOVE_CELEBRATE; }
    PARAMETRIZE { incoming = MOVE_FALSE_SWIPE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(incoming); }
        OPPONENT(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sMushroomObserved = FALSE;
        sMushroomTurn = 0;
        sDynamicAiFunc = ObserveMushroom;
        TURN { MOVE(player, incoming); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMushroomObserved);
        EXPECT_GE(results[i].damage, sMushroomDamage.minimum);
        EXPECT_LE(results[i].damage, sMushroomDamage.maximum);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

AI_DOUBLE_BATTLE_TEST("Mushroom Madness AI targets the redirector unless it can bypass powder redirection")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    bool32 immune;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_NONE; immune = FALSE; }
    PARAMETRIZE { species = SPECIES_BULBASAUR; ability = ABILITY_NONE; item = ITEM_NONE; immune = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; immune = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; immune = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_STALWART; item = ITEM_NONE; immune = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); }
        PLAYER(SPECIES_ONIX) { HP(1); MaxHP(3000); SpDefense(100); }
        OPPONENT(species) { Ability(ability); Item(item); SpAttack(300); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN, target: immune ? playerRight : playerLeft); }
    } SCENE {
        if (immune) HP_BAR(playerRight);
        else HP_BAR(playerLeft);
    } THEN {
        if (!immune) EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][MOVESLOT_0].median, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Mushroom Madness AI redirects its status-move targeting as well")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerLeft); }
    } THEN {
        EXPECT(playerLeft->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

AI_DOUBLE_BATTLE_TEST("Mushroom Madness AI avoids spending an action on redundant Rage Powder")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); Moves(MOVE_RAGE_POWDER, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_GT(opponentLeft, MOVE_WATER_GUN, MOVE_RAGE_POWDER, target: playerLeft); EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("Mushroom Madness AI handles two opposing holders without recursive damage forecasts")
{
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PARAS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sMushroomObserved = FALSE;
        sMushroomTurn = 0;
        sDynamicAiFunc = ObserveMushroom;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMushroomObserved);
        EXPECT_GE(damage, sMushroomDamage.minimum);
        EXPECT_LE(damage, sMushroomDamage.maximum);
        EXPECT(!gAiLogicData->aiCalcInProgress);
    }
}

static s32 ObserveSubstitutedMushroom(u32 attacker, u32 defender, u32 move, s32 score)
{
    score = ObserveMushroom(attacker, defender, move, score);
    if (gBattleResults.battleTurnCounter == 0)
        return score + (move == MOVE_SUBSTITUTE ? 100 : -100);
    return score;
}

AI_SINGLE_BATTLE_TEST("Mushroom Madness AI does not forecast its bonus when a Substitute takes the incoming hit")
{
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_SUBSTITUTE, MOVE_WATER_GUN); }
    } WHEN {
        sMushroomObserved = FALSE;
        sMushroomTurn = 1;
        sDynamicAiFunc = ObserveSubstitutedMushroom;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        SUB_HIT(opponent);
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMushroomObserved);
        EXPECT_GE(damage, sMushroomDamage.minimum);
        EXPECT_LE(damage, sMushroomDamage.maximum);
    }
}

AI_DOUBLE_BATTLE_TEST("Mushroom Madness AI knows powder redirection takes precedence over Storm Drain")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_OVERCOAT; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PARASECT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_STORM_DRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (ability == ABILITY_OVERCOAT));
        if (ability == ABILITY_NONE) {
            EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
            EXPECT_GT(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].median, 0);
        } else {
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Mushroom Madness AI recognizes the redirector in either opposing position")
{
    bool32 right;
    PARAMETRIZE { right = FALSE; }
    PARAMETRIZE { right = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(right ? SPECIES_ONIX : SPECIES_PARASECT) { Item(right ? ITEM_NONE : ITEM_APEX_METEORITE); HP(right ? 1 : 3000); MaxHP(3000); }
        PLAYER(right ? SPECIES_PARASECT : SPECIES_ONIX) { Item(right ? ITEM_APEX_METEORITE : ITEM_NONE); HP(right ? 3000 : 1); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN, target: right ? playerRight : playerLeft); }
    } SCENE {
        if (right) HP_BAR(playerRight);
        else HP_BAR(playerLeft);
    }
}
