#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObservedRiver;
static struct SimulatedDamage sRiverDamage;
static u32 sRiverSpAtk, sRiverSpeed;
static s32 ObserveRiverEsper(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == gBattleMons[attacker].moves[0]
     && !gAiLogicData->aiPredictionInProgress && gBattleResults.battleTurnCounter == 0)
    {
        sObservedRiver = TRUE;
        sRiverDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sRiverSpAtk = gBattleMons[attacker].statStages[STAT_SPATK];
        sRiverSpeed = gBattleMons[attacker].statStages[STAT_SPEED];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("River Esper AI predicts pre-attack boosts including Stored Power without leaking stages", s16 damage)
{
    enum Move move;
    bool32 rain;
    PARAMETRIZE { move = MOVE_WATER_GUN; rain = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; rain = TRUE; }
    PARAMETRIZE { move = MOVE_STORED_POWER; rain = FALSE; }
    PARAMETRIZE { move = MOVE_STORED_POWER; rain = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(rain ? SPECIES_POLITOED : SPECIES_WOBBUFFET) { Ability(rain ? ABILITY_DRIZZLE : ABILITY_NONE); HP(3000); MaxHP(3000); SpDefense(100); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); SpAttack(100); Speed(100); Moves(move); }
    } WHEN { sObservedRiver = FALSE; sDynamicAiFunc = ObserveRiverEsper; TURN { EXPECT_MOVE(opponent, move); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedRiver);
        EXPECT_EQ(sRiverSpAtk, DEFAULT_STAT_STAGE);
        EXPECT_EQ(sRiverSpeed, DEFAULT_STAT_STAGE);
        EXPECT_GE(results[i].damage, sRiverDamage.minimum);
        EXPECT_LE(results[i].damage, sRiverDamage.maximum);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + ((rain || move == MOVE_WATER_GUN) ? 2 : 0));
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + ((rain || move == MOVE_STORED_POWER) ? 2 : 0));
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_RIVER_ESPER);
    }
}

AI_SINGLE_BATTLE_TEST("River Esper AI recognizes the triggering Water move's increased damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { HP(3000); MaxHP(3000); SpDefense(100); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_WATER_GUN, MOVE_PSYCHIC); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_PSYCHIC); EXPECT_MOVE(opponent, MOVE_WATER_GUN); } }
}

AI_SINGLE_BATTLE_TEST("River Esper AI treats an immune Psychic move as useful Speed setup until Speed is capped")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_UMBREON) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Attack(1); Speed(60); Moves(MOVE_CONFUSION, MOVE_SCRATCH); }
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN { SCORE_GT(opponent, MOVE_CONFUSION, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_CONFUSION); }
        TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_CONFUSION); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN { EXPECT_EQ(opponent->statStages[STAT_SPEED], MAX_STAT_STAGE); EXPECT_LT(player->hp, 3000); }
}

AI_SINGLE_BATTLE_TEST("River Esper AI does not assume its before-attack Speed boost lets a slow move act first")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_POLITOED) { Ability(ABILITY_DRIZZLE); HP(1); Attack(4000); Speed(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); HP(1); Speed(60); Moves(MOVE_WATER_GUN, MOVE_AQUA_JET); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_AQUA_JET, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_AQUA_JET); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(opponent->hp, 1); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); }
}

AI_SINGLE_BATTLE_TEST("River Esper AI uses effective Revelation Dance and Weather Ball types", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_REVELATION_DANCE; }
    PARAMETRIZE { move = MOVE_WEATHER_BALL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_POLITOED) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(move); }
    } WHEN { sObservedRiver = FALSE; sDynamicAiFunc = ObserveRiverEsper; TURN { EXPECT_MOVE(opponent, move); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObservedRiver); EXPECT_GE(results[i].damage, sRiverDamage.minimum); EXPECT_LE(results[i].damage, sRiverDamage.maximum); EXPECT_EQ(sRiverSpeed, DEFAULT_STAT_STAGE); EXPECT_EQ(sRiverSpAtk, DEFAULT_STAT_STAGE); }
}

AI_SINGLE_BATTLE_TEST("River Esper AI recognizes an opposing Apex holder without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_GOLDUCK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { MOVE(player, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_RIVER_ESPER); }
}
