#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sMoonDamage;
static bool32 sMoonObserved;
static s32 ObserveMoonDamage(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sMoonDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sMoonObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Moon-Attuned AI predicts each listed damaging Moon move", s16 damage)
{
    enum Move move = MOVE_BLOOD_MOON;
    enum Item item = ITEM_NONE;
    const enum Move attacks[] = {MOVE_BLOOD_MOON, MOVE_MOONBLAST, MOVE_MOONGEIST_BEAM};
    for (u32 j = 0; j < ARRAY_COUNT(attacks); j++) {
        PARAMETRIZE { move = attacks[j]; item = ITEM_NONE; }
        PARAMETRIZE { move = attacks[j]; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); Item(item); SpAttack(300); Moves(move); }
    } WHEN {
        sMoonObserved = FALSE;
        sDynamicAiFunc = ObserveMoonDamage;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMoonObserved);
        EXPECT_GE(results[i].damage, sMoonDamage.minimum);
        EXPECT_LE(results[i].damage, sMoonDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_MOON_ATTUNED : ABILITY_CUTE_CHARM);
    } FINALLY {
        for (u32 j = 0; j < 6; j += 2)
            EXPECT_MUL_EQ(results[j].damage, UQ_4_12(1.5), results[j + 1].damage);
    }
}

static s32 ObserveAllyMoonDamage(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_3 && battlerDef == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sMoonDamage = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sMoonObserved = TRUE;
        return score + 20; // Compare the prediction with damage to this target.
    }
    return score;
}

AI_DOUBLE_BATTLE_TEST("Moon-Attuned AI predicts a partner's aura without strengthening unrelated moves", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_MOONBLAST; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_MOONBLAST; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SHADOW_BALL; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SHADOW_BALL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_CLEFFA) { Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(300); Moves(move); }
    } WHEN {
        sMoonObserved = FALSE;
        sDynamicAiFunc = ObserveAllyMoonDamage;
        TURN { EXPECT_MOVE(opponentRight, move, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMoonObserved);
        EXPECT_GE(results[i].damage, sMoonDamage.minimum);
        EXPECT_LE(results[i].damage, sMoonDamage.maximum);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

AI_DOUBLE_BATTLE_TEST("Moon-Attuned AI chooses stronger Moonlight healing when ordinary recovery cannot prevent a KO")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); Attack(4000); Speed(1); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CLEFFA) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(1); MaxHP(800); Defense(100); Speed(200); Moves(MOVE_MOONLIGHT, MOVE_RECOVER); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentRight);
            SCORE_GT(opponentRight, MOVE_MOONLIGHT, MOVE_RECOVER, target: playerLeft);
            EXPECT_MOVE(opponentRight, MOVE_MOONLIGHT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
    }
}

static bool32 sMoonCandidateObserved;
static u32 sMoonCandidateStatus;
static void ObserveMoonCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1)
    {
        sMoonCandidateObserved = TRUE;
        sMoonCandidateStatus = gBattleMons[context->battler].status1;
    }
}

AI_SINGLE_BATTLE_TEST("Moon-Attuned switch AI predicts the entry status cure")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TINKATON) { Speed(3); Moves(MOVE_DRAGON_PULSE); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_CLEFABLE) { HP(1); MaxHP(800); Status1(STATUS1_FROSTBITE); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_MOONBLAST); }
        OPPONENT(SPECIES_HYPNO) { Speed(1); Moves(MOVE_IRON_HEAD); }
    } WHEN {
        sMoonCandidateObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveMoonCandidate;
        TURN { MOVE(player, MOVE_DRAGON_PULSE); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sMoonCandidateObserved);
        EXPECT_EQ(sMoonCandidateStatus, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->hp, 201);
    }
}

AI_SINGLE_BATTLE_TEST("Moon-Attuned switch AI chooses a candidate that survives because of its entry heal")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(80); MaxHP(800); Speed(3); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); HP(1); MaxHP(800); Defense(100); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); HP(1); MaxHP(800); Defense(100); Speed(1); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_GT(opponent->hp, 1);
    }
}

AI_SINGLE_BATTLE_TEST("Moon-Attuned switch AI does not use its entry healing to survive fatal hazards")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); Moves(MOVE_SPIKES, MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(40); Speed(4); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); HP(100); MaxHP(800); Speed(1); Item(ITEM_HEAVY_DUTY_BOOTS); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_CUTE_CHARM); HP(100); MaxHP(800); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_SEND_OUT(opponent, 1); }
    }
}

static bool32 sMoonBenchObserved;
static void ObserveMoonBench(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 2)
        sMoonBenchObserved = TRUE;
}

AI_DOUBLE_BATTLE_TEST("Moon-Attuned switch AI does not leak simulated healing and status cures onto the current partner")
{
    GIVEN {
        WITH_CONFIG(B_BURN_DAMAGE, GEN_9);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TINKATON) { Speed(3); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(800); Status1(STATUS1_BURN); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CLEFABLE) { HP(1); MaxHP(800); Speed(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_MOONBLAST); }
        OPPONENT(SPECIES_METAGROSS) { Speed(1); Moves(MOVE_IRON_HEAD); }
    } WHEN {
        sMoonBenchObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveMoonBench;
        TURN { MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentLeft); EXPECT_SEND_OUT(opponentLeft, 3); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sMoonBenchObserved);
        EXPECT_EQ(opponentRight->status1, STATUS1_BURN);
        EXPECT_EQ(opponentRight->hp, 350);
    }
}
