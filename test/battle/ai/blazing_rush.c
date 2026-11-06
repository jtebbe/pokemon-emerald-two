#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

static bool32 sRushSeen[PARTY_SIZE];
static u32 sRushSpeed[PARTY_SIZE], sRushStage[PARTY_SIZE], sRushStatus[PARTY_SIZE], sRushFoeSpeed[PARTY_SIZE];
static struct SimulatedDamage sRushIncoming[PARTY_SIZE];
static void ObserveRushCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sRushSeen[slot] = TRUE;
    sRushSpeed[slot] = gAiLogicData->speedStats[context->battler];
    sRushStage[slot] = gBattleMons[context->battler].statStages[STAT_SPEED];
    sRushStatus[slot] = gBattleMons[context->opposingBattler].status1;
    sRushFoeSpeed[slot] = gAiLogicData->speedStats[context->opposingBattler];
    sRushIncoming[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][0];
}

AI_SINGLE_BATTLE_TEST("Blazing Rush AI predicts boosted Speed burns and reduced physical damage without candidate leakage")
{
    u32 speed;
    PARAMETRIZE { speed = 120; }
    PARAMETRIZE { speed = 149; }
    PARAMETRIZE { speed = 150; }
    PARAMETRIZE { speed = 151; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(speed); Attack(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Defense(100); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_RAPIDASH) { Ability(ABILITY_RUN_AWAY); HP(1000); MaxHP(1000); Speed(100); Defense(100); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sRushSeen[1] = sRushSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRushCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRushSeen[1]); EXPECT(sRushSeen[2]);
        EXPECT_EQ(sRushSpeed[1], 150);
        EXPECT_EQ(sRushSpeed[2], 100);
        EXPECT_EQ(sRushStage[1], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(sRushStatus[1], speed < 150 ? STATUS1_BURN : STATUS1_NONE);
        EXPECT_EQ(sRushStatus[2], STATUS1_NONE);
        if (speed < 150)
            EXPECT_MUL_EQ(sRushIncoming[2].median, UQ_4_12(0.5), sRushIncoming[1].median);
        else
            EXPECT_EQ(sRushIncoming[1].median, sRushIncoming[2].median);
    }
}

AI_SINGLE_BATTLE_TEST("Blazing Rush AI respects burn immunity curing berries Guts and Quick Feet in entry forecasts")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_WATER_VEIL; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_THERMAL_EXCHANGE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_RAWST_BERRY; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_LUM_BERRY; }
    PARAMETRIZE { ability = ABILITY_GUTS; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_QUICK_FEET; item = ITEM_NONE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(1000); MaxHP(1000); Speed(100); Attack(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PONYTA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Defense(100); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_PONYTA) { Ability(ABILITY_RUN_AWAY); HP(1000); MaxHP(1000); Speed(100); Defense(100); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sRushSeen[1] = sRushSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRushCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRushSeen[1]); EXPECT(sRushSeen[2]);
        EXPECT_EQ(sRushStatus[1], ability == ABILITY_GUTS || ability == ABILITY_QUICK_FEET ? STATUS1_BURN : STATUS1_NONE);
        EXPECT_EQ(sRushStatus[2], STATUS1_NONE);
        EXPECT_EQ(sRushFoeSpeed[1], ability == ABILITY_QUICK_FEET ? 150 : 100);
        EXPECT_EQ(sRushFoeSpeed[2], 100);
        if (ability == ABILITY_GUTS)
            EXPECT_MUL_EQ(sRushIncoming[2].median, UQ_4_12(1.5), sRushIncoming[1].median);
    }
}

AI_SINGLE_BATTLE_TEST("Blazing Rush AI predicts Sticky Web before its entry boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(120); Moves(MOVE_STICKY_WEB, MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RAPIDASH) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        sRushSeen[1] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveRushCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sRushSeen[1]);
        EXPECT_EQ(sRushStage[1], DEFAULT_STAT_STAGE);
        EXPECT_EQ(sRushSpeed[1], 100);
        EXPECT_EQ(sRushStatus[1], STATUS1_NONE);
    }
}
