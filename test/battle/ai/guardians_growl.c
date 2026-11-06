#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

static bool32 sObservedGrowl;
static u32 sGrowlStages[NUM_BATTLE_STATS];
static u32 sGrowlSpeed;
static bool32 sGrowlNextCandidate, sGrowlCandidateLeaked;
static void ObserveGrowlCandidate(struct SwitchAiContext *ctx)
{
    if (ctx->partyMonIndex == 2 && gBattlerPartyIndexes[ctx->battler] == 0)
    {
        sGrowlNextCandidate = TRUE;
        if (gBattleMons[ctx->opposingBattler].statStages[STAT_ATK] != DEFAULT_STAT_STAGE
         || gBattleMons[ctx->opposingBattler].statStages[STAT_SPATK] != DEFAULT_STAT_STAGE
         || gBattleMons[ctx->opposingBattler].statStages[STAT_SPEED] != DEFAULT_STAT_STAGE
         || gAiLogicData->speedStats[ctx->opposingBattler] != 100)
            sGrowlCandidateLeaked = TRUE;
    }
    if (ctx->partyMonIndex != 1 || gBattlerPartyIndexes[ctx->battler] != 0 || gBattleMons[ctx->battler].species != SPECIES_ARCANINE)
        return;
    sObservedGrowl = TRUE;
    sGrowlSpeed = gAiLogicData->speedStats[ctx->opposingBattler];
    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        sGrowlStages[stat] = gBattleMons[ctx->opposingBattler].statStages[stat];
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI simulates both entry drops when evaluating a switch-in")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Attack(100); SpAttack(100); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_FLASH_FIRE); Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN { sObservedGrowl = FALSE; gDynamicAiMonChoiceFunc = ObserveGrowlCandidate; TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); } }
    THEN { gDynamicAiMonChoiceFunc = NULL; EXPECT(sObservedGrowl); EXPECT_EQ(sGrowlStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(sGrowlStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1); }
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI respects Soundproof Intimidate immunities and Clear Amulet in switch predictions")
{
    enum Ability ability;
    enum Item item = ITEM_NONE;
    PARAMETRIZE { ability = ABILITY_SOUNDPROOF; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN { sObservedGrowl = FALSE; gDynamicAiMonChoiceFunc = ObserveGrowlCandidate; TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); } }
    THEN { gDynamicAiMonChoiceFunc = NULL; EXPECT(sObservedGrowl); EXPECT_EQ(sGrowlStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT_EQ(sGrowlStages[STAT_SPATK], DEFAULT_STAT_STAGE); }
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI simulates Contrary Defiant Competitive Guard Dog and Simple reactions")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; }
    PARAMETRIZE { ability = ABILITY_GUARD_DOG; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN { sObservedGrowl = FALSE; gDynamicAiMonChoiceFunc = ObserveGrowlCandidate; TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); } }
    THEN {
        gDynamicAiMonChoiceFunc = NULL; EXPECT(sObservedGrowl);
        EXPECT_EQ(sGrowlStages[STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_DEFIANT ? 3 : ability == ABILITY_CONTRARY || ability == ABILITY_GUARD_DOG ? 1 : ability == ABILITY_SIMPLE ? -2 : -1));
        EXPECT_EQ(sGrowlStages[STAT_SPATK], DEFAULT_STAT_STAGE + (ability == ABILITY_COMPETITIVE ? 3 : ability == ABILITY_CONTRARY ? 1 : ability == ABILITY_GUARD_DOG ? 0 : ability == ABILITY_SIMPLE ? -2 : -1));
    }
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI includes Rattled and Adrenaline Orb Speed reactions")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_RATTLED; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_ADRENALINE_ORB; }
    PARAMETRIZE { ability = ABILITY_RATTLED; item = ITEM_ADRENALINE_ORB; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; item = ITEM_ADRENALINE_ORB; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; item = ITEM_ADRENALINE_ORB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN { sObservedGrowl = FALSE; gDynamicAiMonChoiceFunc = ObserveGrowlCandidate; TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); } }
    THEN {
        gDynamicAiMonChoiceFunc = NULL; EXPECT(sObservedGrowl);
        u32 stages = ability == ABILITY_CONTRARY ? DEFAULT_STAT_STAGE - 1 : ability == ABILITY_SIMPLE ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE + (ability == ABILITY_RATTLED) + (item == ITEM_ADRENALINE_ORB);
        EXPECT_EQ(sGrowlStages[STAT_SPEED], stages);
        EXPECT_EQ(sGrowlSpeed, 100 * gStatStageRatios[stages][0] / gStatStageRatios[stages][1]);
    }
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI recognizes an opposing Apex holder without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_GUARDIANS_GROWL); }
}

AI_SINGLE_BATTLE_TEST("Guardian's Growl AI restores opponent stages and Speed before evaluating another candidate")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_RATTLED); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_FLASH_FIRE); HP(400); MaxHP(400); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sObservedGrowl = sGrowlNextCandidate = sGrowlCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveGrowlCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sObservedGrowl); EXPECT(sGrowlNextCandidate); EXPECT(!sGrowlCandidateLeaked);
        EXPECT_EQ(sGrowlSpeed, 150);
    }
}
