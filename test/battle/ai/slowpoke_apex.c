#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
// Observer storage belongs in EWRAM, leaving room for recursive AI calculations.
static EWRAM_DATA struct SimulatedDamage sSlowDamage;
static EWRAM_DATA u32 sSlowAccuracy;
static EWRAM_DATA bool32 sSlowSeen;
static EWRAM_DATA s32 sSlowEntryScore;
static s32 ObserveSlowpokeApex(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sSlowSeen = TRUE;
        sSlowDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sSlowAccuracy = gAiLogicData->moveAccuracy[attacker][defender][gAiThinkingStruct->movesetIndex];
        if (gAiLogicData->abilities[attacker] == ABILITY_VOODOO_STRENGTH)
            sSlowEntryScore = AI_GetVoodooStrengthEntryScore(attacker);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Spiced Dreams AI predicts boosted sleeping attacks and unboosted wake-up attacks", s16 damage)
{
    u32 sleep;
    PARAMETRIZE { sleep = 1; }
    PARAMETRIZE { sleep = 4; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Item(ITEM_COVERT_CLOAK); }
        OPPONENT(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_SLEEP_TURN(sleep)); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sSlowSeen = FALSE; sDynamicAiFunc = ObserveSlowpokeApex;
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE { HP_BAR(player, captureDamage: &results[i].damage); } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sSlowSeen);
        EXPECT_GE(results[i].damage, sSlowDamage.minimum);
        EXPECT_LE(results[i].damage, sSlowDamage.maximum);
        EXPECT(!IsBattlerIncapacitated(1, ABILITY_SPICED_DREAMS));
        EXPECT(HasMoveUsableWhileAsleep(1));
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Spiced Dreams AI uses normal attacks instead of needing Sleep Talk")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SLOWPOKE_GALAR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_WATER_GUN, MOVE_SLEEP_TALK); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_SPICED_DREAMS, FALSE)); }
    }
}

AI_SINGLE_BATTLE_TEST("Sea Herald AI predicts perfect accuracy for Water attacks and rain Weather Ball", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; }
    PARAMETRIZE { move = MOVE_WEATHER_BALL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); Moves(move); }
    } WHEN {
        sSlowSeen = FALSE; sDynamicAiFunc = ObserveSlowpokeApex;
        TURN { EXPECT_MOVE(opponent, move, hit: FALSE); }
    } SCENE { HP_BAR(player, captureDamage: &results[i].damage); } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sSlowSeen);
        EXPECT_EQ(sSlowAccuracy, 101);
        EXPECT_GE(results[i].damage, sSlowDamage.minimum);
        EXPECT_LE(results[i].damage, sSlowDamage.maximum);
        enum DamageCategory saved = gBattleStruct->dynamicMoveCategory;
        gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_PHYSICAL;
        EXPECT(AI_CanBypassProtect(1, 0, move));
        EXPECT_EQ((u32)gBattleStruct->dynamicMoveCategory, DAMAGE_CATEGORY_PHYSICAL);
        gBattleStruct->dynamicMoveCategory = saved;
    }
}

AI_DOUBLE_BATTLE_TEST("Sea Herald AI recognizes the ally aura through Substitute and Protect")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(400); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); Speed(2); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SUBSTITUTE); EXPECT_MOVE(opponentLeft, MOVE_WATER_GUN, hit: FALSE); }
    } THEN {
        EXPECT_EQ((u32)playerLeft->volatiles.substituteHP, 100);
        EXPECT_LT(playerLeft->hp, 300);
        EXPECT(AI_CanBypassProtect(1, 0, MOVE_WATER_GUN));
        EXPECT(!AI_CanBypassProtect(1, 0, MOVE_TACKLE));
        EXPECT(!IsSubstituteProtected(1, 0, gAiLogicData->abilities[1], MOVE_WATER_GUN));
    }
}

AI_SINGLE_BATTLE_TEST("Sea Herald AI recognizes that Protect cannot stop opposing Water moves in rain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SLOWKING) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); Moves(MOVE_PROTECT, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_WATER_GUN); SCORE_LT(opponent, MOVE_PROTECT, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

static EWRAM_DATA bool32 sSlowCandidateSeen[PARTY_SIZE];
static EWRAM_DATA struct SimulatedDamage sSlowOutgoing[PARTY_SIZE], sSlowIncoming[PARTY_SIZE];
static EWRAM_DATA u32 sSlowOutAccuracy[PARTY_SIZE], sSlowInAccuracy[PARTY_SIZE], sSlowStatus[PARTY_SIZE];
static EWRAM_DATA bool32 sSlowStagesRestored[PARTY_SIZE];
static void ObserveSlowCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sSlowCandidateSeen[slot] = TRUE;
    sSlowOutgoing[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0];
    sSlowIncoming[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][1];
    sSlowOutAccuracy[slot] = gAiLogicData->moveAccuracy[context->battler][context->opposingBattler][0];
    sSlowInAccuracy[slot] = gAiLogicData->moveAccuracy[context->opposingBattler][context->battler][1];
    sSlowStatus[slot] = gBattleMons[context->battler].status1;
    sSlowStagesRestored[slot] = TRUE;
    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        if (gBattleMons[context->battler].statStages[stat] != DEFAULT_STAT_STAGE)
            sSlowStagesRestored[slot] = FALSE;
}

AI_SINGLE_BATTLE_TEST("Spiced Dreams switch AI predicts entry sleep and its power boost without leaking status to other candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SLOWBRO_GALAR) { Item(ITEM_APEX_METEORITE); Speed(2); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SLOWBRO_GALAR) { Ability(ABILITY_OWN_TEMPO); Speed(2); Moves(MOVE_TACKLE); }
    } WHEN {
        memset(sSlowCandidateSeen, 0, sizeof(sSlowCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveSlowCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sSlowCandidateSeen[1]); EXPECT(sSlowCandidateSeen[2]);
        EXPECT(sSlowStatus[1] & STATUS1_SLEEP);
        EXPECT_EQ(sSlowStatus[2], STATUS1_NONE);
        EXPECT_GT(sSlowOutgoing[1].median, sSlowOutgoing[2].median);
    }
}

AI_SINGLE_BATTLE_TEST("Voodoo Strength switch AI averages damage Accuracy and Evasion outcomes and restores stages")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); Speed(2); Moves(MOVE_MEGA_KICK); }
        OPPONENT(SPECIES_SLOWKING_GALAR) { Ability(ABILITY_OWN_TEMPO); Speed(2); Moves(MOVE_MEGA_KICK); }
    } WHEN {
        memset(sSlowCandidateSeen, 0, sizeof(sSlowCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveSlowCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sSlowCandidateSeen[1]); EXPECT(sSlowCandidateSeen[2]);
        EXPECT_GT(sSlowOutgoing[1].median, sSlowOutgoing[2].median);
        EXPECT_GT(sSlowOutgoing[1].maximum, sSlowOutgoing[2].maximum);
        EXPECT_LT(sSlowIncoming[1].minimum, sSlowIncoming[2].minimum);
        EXPECT_EQ(sSlowIncoming[1].maximum, sSlowIncoming[2].maximum);
        EXPECT_EQ(sSlowOutAccuracy[1], (75 * 6 + 100) / 7);
        EXPECT_EQ(sSlowOutAccuracy[2], 75);
        EXPECT_EQ(sSlowInAccuracy[1], (100 * 6 + 60) / 7);
        EXPECT_EQ(sSlowInAccuracy[2], 100);
        EXPECT(sSlowStagesRestored[1]); EXPECT(sSlowStagesRestored[2]);
        EXPECT(!gAiLogicData->voodooStrengthAveraging);
    }
}

AI_DOUBLE_BATTLE_TEST("Voodoo Strength AI accounts for whether its ally benefits from the entry boosts")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sSlowSeen = FALSE; sDynamicAiFunc = ObserveSlowpokeApex;
        TURN {}
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sSlowSeen);
        if (ability == ABILITY_CONTRARY) EXPECT_EQ(sSlowEntryScore, 0);
        else EXPECT_GT(sSlowEntryScore, 0);
    }
}
