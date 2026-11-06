#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sLullingDamage;
static bool32 sLullingObserved;
static s32 sLullingScore;
static s32 ObserveLulling(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sLullingDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sLullingScore = score;
        sLullingObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI predicts double damage from sound and nonsound attacks against sleep and Comatose", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    u32 status = STATUS1_NONE;
    enum Ability ability = ABILITY_NONE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_HYPER_VOICE};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; status = STATUS1_NONE; ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_SLEEP; ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_NONE; ability = ABILITY_COMATOSE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(1); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); Speed(100); Moves(move); }
    } WHEN {
        sLullingObserved = FALSE;
        sDynamicAiFunc = ObserveLulling;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLullingObserved);
        EXPECT_GE(results[i].damage, sLullingDamage.minimum);
        EXPECT_LE(results[i].damage, sLullingDamage.maximum);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
            EXPECT_MUL_EQ(results[j * 3].damage, UQ_4_12(2.0), results[j * 3 + 1].damage);
            EXPECT_EQ(results[j * 3 + 1].damage, results[j * 3 + 2].damage);
        }
    }
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI knows sound priority and loses it against a sleeping opponent")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_SLEEP; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); Status1(status); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_ALLURING_VOICE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_ALLURING_VOICE); }
    } THEN {
        // Inspect the original state independently of the attack's random sleep roll.
        player->status1 = status;
        EXPECT_EQ(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_ALLURING_VOICE, MOVE_SCRATCH, CONSIDER_PRIORITY), status == STATUS1_NONE);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_ALLURING_VOICE, MOVE_SCRATCH, DONT_CONSIDER_PRIORITY));
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_LULLING_MELODIES);
    }
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI chooses priority sound damage to win before a faster opponent")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(100); Attack(4000); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Moves(MOVE_ALLURING_VOICE, MOVE_MOONBLAST); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_ALLURING_VOICE, MOVE_MOONBLAST); EXPECT_MOVE(opponent, MOVE_ALLURING_VOICE); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 1);
    }
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI avoids priority sound attacks blocked by Psychic Terrain or Dazzling")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PSYCHIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_DAZZLING; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Moves(MOVE_ALLURING_VOICE, MOVE_MOONBLAST); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_MOONBLAST, MOVE_ALLURING_VOICE); EXPECT_MOVE(opponent, MOVE_MOONBLAST); }
    }
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI values added sleep only against susceptible targets", s32 score)
{
    enum Ability ability = ABILITY_NONE;
    enum Item item = ITEM_NONE;
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_INSOMNIA; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Moves(MOVE_ALLURING_VOICE); }
    } WHEN {
        sLullingObserved = FALSE;
        sDynamicAiFunc = ObserveLulling;
        TURN { EXPECT_MOVE(opponent, MOVE_ALLURING_VOICE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLullingObserved);
        results[i].score = sLullingScore;
    } FINALLY {
        for (u32 j = 1; j < 5; j++) EXPECT_EQ(results[j].score + DECENT_EFFECT, results[0].score);
    }
}

AI_SINGLE_BATTLE_TEST("Lulling Melodies AI rewards sleep setup to enable its damage bonus")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Ability(ABILITY_CUTE_CHARM); Item(item); Moves(MOVE_SING, MOVE_SCRATCH); }
    } WHEN {
        TURN {}
    } THEN {
        s32 score = 0;
        player->status1 = STATUS1_NONE;
        IncreaseSleepScore(B_BATTLER_1, B_BATTLER_0, MOVE_SING, &score);
        EXPECT_EQ(score, item == ITEM_APEX_METEORITE ? DECENT_EFFECT + WEAK_EFFECT : DECENT_EFFECT);
    }
}

static s32 ObserveLullingDouble(u32 attacker, u32 defender, u32 move, s32 score)
{
    score = ObserveLulling(attacker, defender, move, score);
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0)
        return score + 20; // Keep the observed attack on the defender being compared.
    return score;
}

AI_DOUBLE_BATTLE_TEST("Lulling Melodies AI notices a sleeping opponent in the other slot and can attack through Psychic Terrain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PSYCHIC_SURGE); HP(3000); MaxHP(3000); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_ALLURING_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sLullingObserved = FALSE;
        sDynamicAiFunc = ObserveLullingDouble;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_ALLURING_VOICE, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sLullingObserved);
        EXPECT_GT(sLullingDamage.median, 0);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_ALLURING_VOICE, MOVE_SCRATCH, CONSIDER_PRIORITY));
    }
}
