#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sTruePowerDamage;
static bool32 sTruePowerObserved;
static s32 ObserveTruePowerDamage(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sTruePowerObserved = TRUE;
        sTruePowerDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("True Power AI predicts stacked physical modifiers and the Guts burn exception", s16 damage)
{
    enum Item item;
    u32 status, attack;
    PARAMETRIZE { item = ITEM_NONE; status = STATUS1_NONE; attack = 200; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_NONE; attack = 100; }
    PARAMETRIZE { item = ITEM_NONE; status = STATUS1_NONE; attack = 300; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_BURN; attack = 100; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_POISON; attack = 100; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_MACHAMP) { Item(item); Ability(ABILITY_NONE); Attack(attack); Status1(status); Moves(MOVE_TACKLE); }
    } WHEN {
        sTruePowerObserved = FALSE;
        sDynamicAiFunc = ObserveTruePowerDamage;
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTruePowerObserved);
        EXPECT_GE(results[i].damage, sTruePowerDamage.minimum);
        EXPECT_LE(results[i].damage, sTruePowerDamage.maximum);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[2].damage, results[4].damage);
    }
}

AI_SINGLE_BATTLE_TEST("True Power AI does not boost special attacks", s16 damage)
{
    enum Item item;
    u32 status;
    PARAMETRIZE { item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_BURN; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_MACHAMP) { Item(item); Ability(ABILITY_NONE); SpAttack(100); Status1(status); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sTruePowerObserved = FALSE;
        sDynamicAiFunc = ObserveTruePowerDamage;
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sTruePowerObserved);
        EXPECT_GE(results[i].damage, sTruePowerDamage.minimum);
        EXPECT_LE(results[i].damage, sTruePowerDamage.maximum);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

AI_SINGLE_BATTLE_TEST("True Power AI recognizes beneficial burn and poison only with physical attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_TRUE_POWER);
        EXPECT_EQ(ShouldBurn(B_BATTLER_1, B_BATTLER_0, ABILITY_TRUE_POWER), move != MOVE_TACKLE);
        EXPECT_EQ(ShouldPoison(B_BATTLER_1, B_BATTLER_0), move != MOVE_TACKLE);
        EXPECT_EQ(ShouldBurn(B_BATTLER_0, B_BATTLER_0, ABILITY_TRUE_POWER), move == MOVE_TACKLE);
    }
}

AI_SINGLE_BATTLE_TEST("True Power AI knows it cannot flinch the target even with Mold Breaker")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT(!ShouldTryToFlinch(B_BATTLER_1, B_BATTLER_0, ability, ABILITY_TRUE_POWER, MOVE_FAKE_OUT));
        EXPECT(!IsFlinchGuaranteed(B_BATTLER_1, B_BATTLER_0, MOVE_FAKE_OUT));
    }
}

static bool32 sTruePowerEntryObserved;
static u32 sTruePowerEntryAttack, sTruePowerEntrySpAttack;
static void ObserveTruePowerEntry(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 1 && gBattlerPartyIndexes[context->battler] == 0)
    {
        sTruePowerEntryObserved = TRUE;
        sTruePowerEntryAttack = gBattleMons[context->opposingBattler].statStages[STAT_ATK];
        sTruePowerEntrySpAttack = gBattleMons[context->opposingBattler].statStages[STAT_SPATK];
    }
}

AI_SINGLE_BATTLE_TEST("True Power AI respects Inner Focus when forecasting Intimidate and Guardian's Growl")
{
    u32 generation;
    enum Item item;
    PARAMETRIZE { generation = GEN_7; item = ITEM_NONE; }
    PARAMETRIZE { generation = GEN_8; item = ITEM_NONE; }
    PARAMETRIZE { generation = GEN_7; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { generation = GEN_8; item = ITEM_APEX_METEORITE; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, generation);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_MACHAMP) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Item(item); Ability(ABILITY_INTIMIDATE); HP(1000); MaxHP(1000); Speed(200); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sTruePowerEntryObserved = FALSE;
        gDynamicAiMonChoiceFunc = ObserveTruePowerEntry;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTruePowerEntryObserved);
        EXPECT_EQ(sTruePowerEntryAttack, generation >= GEN_8 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(sTruePowerEntrySpAttack, item == ITEM_APEX_METEORITE && generation < GEN_8 ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
        if (item == ITEM_NONE && generation >= GEN_8)
            EXPECT_EQ(BattlerBenefitsFromAbilityScore(B_BATTLER_1, ABILITY_INTIMIDATE, gAiLogicData), NO_INCREASE);
    }
}
