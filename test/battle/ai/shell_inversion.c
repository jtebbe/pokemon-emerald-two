#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Shell Inversion AI predicts boosted physical and special damage, weaker defenses and increased Speed")
{
    enum Move move;
    s16 dealt, taken;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SQUIRTLE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(80); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); HP(3000); MaxHP(3000); Speed(100); Moves(move); }
    } WHEN {
        TURN { MOVE(player, move); EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &dealt);
        HP_BAR(player, captureDamage: &taken);
    } THEN {
        struct SimulatedDamage attack = gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][MOVESLOT_0];
        struct SimulatedDamage defense = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
        EXPECT_GE(dealt, attack.minimum);
        EXPECT_LE(dealt, attack.maximum);
        EXPECT_GE(taken, defense.minimum);
        EXPECT_LE(taken, defense.maximum);
        EXPECT_EQ(gAiLogicData->speedStats[B_BATTLER_0], 120);
        EXPECT(AI_IsFaster(B_BATTLER_0, B_BATTLER_1, move, move, DONT_CONSIDER_PRIORITY));
    }
}

AI_SINGLE_BATTLE_TEST("Shell Inversion AI knows flinch immunity from the item reveal even against Mold Breaker")
{
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SERENE_GRACE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_SERENE_GRACE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SQUIRTLE) { Item(item); Speed(10); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_FAKE_OUT); }
    } THEN {
        enum Ability defAbility = AI_DecideKnownAbilityForTurn(B_BATTLER_0);
        if (item == ITEM_APEX_METEORITE)
            EXPECT_EQ(defAbility, ABILITY_SHELL_INVERSION);
        else
            EXPECT_NE(defAbility, ABILITY_SHELL_INVERSION);
        EXPECT_EQ(IsFlinchGuaranteed(B_BATTLER_1, B_BATTLER_0, MOVE_FAKE_OUT), item != ITEM_APEX_METEORITE);
        EXPECT_EQ(ShouldTryToFlinch(B_BATTLER_1, B_BATTLER_0, ability, defAbility, MOVE_FAKE_OUT), item != ITEM_APEX_METEORITE);
    }
}

AI_SINGLE_BATTLE_TEST("Shell Inversion AI does not reward Fake Out as a guaranteed flinch")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_BLASTOISE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_FAKE_OUT, MOVE_STRENGTH); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_FAKE_OUT, MOVE_STRENGTH); EXPECT_MOVE(opponent, MOVE_STRENGTH); }
    }
}

static bool32 sObservedShellCandidate;
static u8 sShellCandidateStages[NUM_BATTLE_STATS];
static u32 sShellCandidateSpeed;

static void ObserveShellInversionCandidate(struct SwitchAiContext *context)
{
    enum BattlerId battler = context->battler;
    if (gBattleMons[battler].species != SPECIES_WARTORTLE)
        return;

    sObservedShellCandidate = TRUE;
    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        sShellCandidateStages[stat] = gBattleMons[battler].statStages[stat];
    sShellCandidateSpeed = gAiLogicData->speedStats[battler];
}

AI_DOUBLE_BATTLE_TEST("Shell Inversion switch-in AI applies entry stages and Sticky Web once without changing its live ally")
{
    bool32 web;
    enum Item item;
    PARAMETRIZE { web = FALSE; item = ITEM_NONE; }
    PARAMETRIZE { web = FALSE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { web = TRUE; item = ITEM_APEX_METEORITE; }
    ResetStartingStatuses();
    if (web)
        SetStartingStatus(STARTING_STATUS_STICKY_WEB_OPPONENT);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(100); Moves(MOVE_DRAGON_RAGE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); HP(1); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WARTORTLE) { Item(item); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        // Observe the actual replacement calculation on the battle engine's stack.
        sObservedShellCandidate = FALSE;
        gDynamicAiMonChoiceFunc = ObserveShellInversionCandidate;
        TURN { MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft); EXPECT_SEND_OUT(opponentLeft, 2); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sObservedShellCandidate);
        u32 boost = item == ITEM_APEX_METEORITE;
        EXPECT_EQ(sShellCandidateStages[STAT_ATK], DEFAULT_STAT_STAGE + boost);
        EXPECT_EQ(sShellCandidateStages[STAT_SPATK], DEFAULT_STAT_STAGE + boost);
        EXPECT_EQ(sShellCandidateStages[STAT_SPEED], DEFAULT_STAT_STAGE + boost - web);
        EXPECT_EQ(sShellCandidateStages[STAT_DEF], DEFAULT_STAT_STAGE - boost);
        EXPECT_EQ(sShellCandidateStages[STAT_SPDEF], DEFAULT_STAT_STAGE - boost);
        EXPECT_EQ(sShellCandidateSpeed, boost && !web ? 150 : 100);
        EXPECT_EQ(opponentLeft->species, SPECIES_WARTORTLE);
        EXPECT_EQ(opponentRight->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - web);
    }
}
