#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

static s32 sSporeEntryScore;
static bool32 sSporeCandidateObserved;
static bool32 sSporeCandidateLeaked;
static void ObserveSporeCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1 && gBattlerPartyIndexes[context->battler] == 0)
    {
        sSporeCandidateObserved = TRUE;
        sSporeEntryScore = AI_GetSporeCloudEntryScore(context->battler);
        // Candidate evaluation must never actually inflict a random status.
        if (gBattleMons[context->opposingBattler].status1 != STATUS1_NONE)
            sSporeCandidateLeaked = TRUE;
    }
}

AI_SINGLE_BATTLE_TEST("Spore Cloud AI favors a winning switch-in with useful entry statuses but not a protected opponent")
{
    enum Ability ability;
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; }
    PARAMETRIZE { species = SPECIES_BULBASAUR; ability = ABILITY_NONE; item = ITEM_NONE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); Item(item); HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VILEPLUME) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_VILEPLUME) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sSporeCandidateObserved = FALSE;
        sSporeCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveSporeCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, species == SPECIES_BULBASAUR || (ability == ABILITY_NONE && item == ITEM_NONE) ? 1 : 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sSporeCandidateObserved);
        EXPECT(!sSporeCandidateLeaked);
        if (ability == ABILITY_NONE && item == ITEM_NONE && species == SPECIES_WOBBUFFET) EXPECT_GT(sSporeEntryScore, 0);
        else EXPECT_EQ(sSporeEntryScore, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Spore Cloud AI averages only viable outcomes and avoids helping Poison Heal", s32 score)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_POISON_HEAL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Ability(ability); HP(120); MaxHP(1000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { Ability(ABILITY_ELECTRIC_SURGE); HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VILEPLUME) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_VILEPLUME) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sSporeCandidateObserved = FALSE;
        sSporeCandidateLeaked = FALSE;
        gDynamicAiMonChoiceFunc = ObserveSporeCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, ability == ABILITY_NONE ? 1 : 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sSporeCandidateObserved);
        EXPECT(!sSporeCandidateLeaked);
        results[i].score = sSporeEntryScore;
    } FINALLY {
        EXPECT_GT(results[0].score, 0);
        EXPECT_LT(results[1].score, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Spore Cloud AI does not favor reapplying a status to an already statused foe")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); HP(120); MaxHP(1000); Attack(100); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VILEPLUME) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_VILEPLUME) { HP(400); MaxHP(400); Defense(100); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 2); }
    }
}

AI_SINGLE_BATTLE_TEST("Spore Cloud AI recognizes the opposing Apex ability without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_GLOOM) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SNORE, MOVE_WATER_GUN); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_SPORE_CLOUD);
    }
}
