#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static EWRAM_DATA bool32 sShellSeen[2];
static EWRAM_DATA u32 sShellIncoming[2];
static EWRAM_DATA s32 sShellScore[2];

static s32 ObserveShellBurst(u32 attacker, u32 defender, u32 move, s32 score)
{
    u32 turn = gBattleResults.battleTurnCounter;
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && turn < 2 && !gAiLogicData->aiPredictionInProgress)
    {
        sShellSeen[turn] = TRUE;
        sShellIncoming[turn] = gAiLogicData->simulatedDmg[defender][attacker][MOVESLOT_0].median;
        sShellScore[turn] = BattlerBenefitsFromAbilityScore(attacker, ABILITY_SHELL_BURST, gAiLogicData);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Shell Burst AI predicts zero physical damage until the shield is spent without consuming it in forecasts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        memset(sShellSeen, 0, sizeof(sShellSeen));
        sDynamicAiFunc = ObserveShellBurst;
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sShellSeen[0]); EXPECT(sShellSeen[1]);
        EXPECT_EQ(sShellIncoming[0], 0);
        EXPECT_GT(sShellIncoming[1], 0);
        EXPECT_GT(sShellScore[0], 0);
        EXPECT_EQ(sShellScore[1], 0);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

AI_SINGLE_BATTLE_TEST("Shell Burst AI chooses a special attack rather than feeding an opponent's intact shield")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(400); SpAttack(10); Moves(MOVE_SCRATCH, MOVE_WATER_GUN); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); } } THEN {
        EXPECT(!gBattleStruct->battlerState[B_BATTLER_0].shellBurstUsed);
        EXPECT_LT(player->hp, player->maxHP);
    }
}

static EWRAM_DATA bool32 sShellCandidateSeen[PARTY_SIZE], sShellCandidateApex[PARTY_SIZE];
static EWRAM_DATA u32 sShellCandidateDamage[PARTY_SIZE];

static void ObserveShellCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sShellCandidateSeen[slot] = TRUE;
    sShellCandidateApex[slot] = gAiLogicData->abilities[context->battler] == ABILITY_SHELL_BURST;
    sShellCandidateDamage[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][MOVESLOT_0].median;
}

AI_SINGLE_BATTLE_TEST("Shell Burst AI treats a replacement as fresh and forecasts recurring damage without permanent immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Attack(400); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_SHELLDER) { Item(ITEM_APEX_METEORITE); HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CLOYSTER) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_CLOYSTER) { Ability(ABILITY_SHELL_ARMOR); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_SCRATCH); }
    } WHEN {
        memset(sShellCandidateSeen, 0, sizeof(sShellCandidateSeen));
        gDynamicAiMonChoiceFunc = ObserveShellCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sShellCandidateSeen[1]); EXPECT(sShellCandidateSeen[2]);
        EXPECT(sShellCandidateApex[1]);
        EXPECT(!sShellCandidateApex[2]);
        EXPECT_GT(sShellCandidateDamage[1], 0);
        EXPECT_GT(sShellCandidateDamage[2], 0);
        if (GetBattlerAbility(B_BATTLER_1) == ABILITY_SHELL_BURST)
            EXPECT(!gBattleStruct->battlerState[B_BATTLER_1].shellBurstUsed);
    }
}
