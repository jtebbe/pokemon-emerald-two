#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Gnashing Teeth AI values Strong Jaw the second bite and its Defense drop")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_SNORLAX) { Ability(ABILITY_THICK_FAT); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_RATICATE) { Ability(ABILITY_RUN_AWAY); Item(item); Attack(200); Moves(MOVE_BITE, MOVE_SLASH); }
    } WHEN {
        TURN {
            if (item == ITEM_APEX_METEORITE) {
                SCORE_GT(opponent, MOVE_BITE, MOVE_SLASH); EXPECT_MOVE(opponent, MOVE_BITE);
            } else {
                SCORE_GT(opponent, MOVE_SLASH, MOVE_BITE); EXPECT_MOVE(opponent, MOVE_SLASH);
            }
        }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), item == ITEM_APEX_METEORITE ? ABILITY_GNASHING_TEETH : ABILITY_RUN_AWAY);
    }
}

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sGnashingPrediction;
static s32 sGnashingDefenseAtSelection;
static bool32 sObservedGnashingPrediction;

static s32 ObserveGnashingPrediction(u32 battlerAtk, u32 battlerDef, u32 move, s32 score)
{
    if (battlerAtk == B_BATTLER_1 && battlerDef == B_BATTLER_0 && move == MOVE_BITE
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sGnashingPrediction = gAiLogicData->simulatedDmg[battlerAtk][battlerDef][gAiThinkingStruct->movesetIndex];
        sGnashingDefenseAtSelection = gBattleMons[battlerDef].statStages[STAT_DEF];
        sObservedGnashingPrediction = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Gnashing Teeth AI predicts both hits with intervening Defense changes and protections without leaking stages")
{
    enum Ability ability;
    enum Item item;
    s16 damage[2];
    PARAMETRIZE { ability = ABILITY_THICK_FAT; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_THICK_FAT; item = ITEM_CLEAR_AMULET; }
    PARAMETRIZE { ability = ABILITY_THICK_FAT; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_THICK_FAT; item = ITEM_WHITE_HERB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE); }
    } WHEN {
        sObservedGnashingPrediction = FALSE;
        sDynamicAiFunc = ObserveGnashingPrediction;
        TURN { EXPECT_MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage[0]);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedGnashingPrediction);
        EXPECT_EQ(sGnashingDefenseAtSelection, DEFAULT_STAT_STAGE);
        EXPECT_GE(damage[0] + damage[1], sGnashingPrediction.minimum);
        EXPECT_LE(damage[0] + damage[1], sGnashingPrediction.maximum);
        s32 expected = DEFAULT_STAT_STAGE;
        if (ability == ABILITY_CONTRARY)
            expected += 2;
        else if (ability == ABILITY_SIMPLE)
            expected -= 4;
        else if (ability == ABILITY_THICK_FAT && item == ITEM_NONE)
            expected -= 2;
        EXPECT_EQ(player->statStages[STAT_DEF], expected);
    }
}

AI_SINGLE_BATTLE_TEST("Gnashing Teeth AI knows Focus Sash and Sturdy cannot endure both bites")
{
    bool32 sturdy;
    PARAMETRIZE { sturdy = FALSE; }
    PARAMETRIZE { sturdy = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { Ability(sturdy ? ABILITY_STURDY : ABILITY_THICK_FAT); Item(sturdy ? ITEM_NONE : ITEM_FOCUS_SASH); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_BITE); }
    } THEN {
        u32 hp = player->hp;
        player->hp = player->maxHP;
        bool32 enduresBite = CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_BITE);
        bool32 enduresTackle = CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE);
        player->hp = hp;
        EXPECT(!enduresBite);
        EXPECT(enduresTackle);
    }
}

AI_SINGLE_BATTLE_TEST("Gnashing Teeth AI rewards Defense drops and penalizes Contrary", s32 score)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_THICK_FAT; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PSYCHIC_FANGS); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_PSYCHIC_FANGS); }
    } THEN {
        results[i].score = gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
    } FINALLY {
        EXPECT_GT(results[0].score, results[1].score);
    }
}

static struct SimulatedDamage sGnashingCandidateDamage;
static s32 sGnashingCandidateDefense;
static bool32 sObservedGnashingCandidate;

static void ObserveGnashingCandidate(struct SwitchAiContext *context)
{
    if (gBattleMons[context->battler].species == SPECIES_RATICATE)
    {
        sGnashingCandidateDamage = gAiLogicData->simulatedDmg[context->battler][B_BATTLER_0][MOVESLOT_0];
        sGnashingCandidateDefense = gBattleMons[B_BATTLER_0].statStages[STAT_DEF];
        sObservedGnashingCandidate = TRUE;
    }
}

AI_SINGLE_BATTLE_TEST("Gnashing Teeth switch AI predicts biting damage and leaves the opponent's real Defense intact")
{
    s16 damage[2];
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SNORLAX) { Speed(100); HP(3000); MaxHP(3000); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Speed(60); Moves(MOVE_BITE); }
    } WHEN {
        sObservedGnashingCandidate = FALSE;
        gDynamicAiMonChoiceFunc = ObserveGnashingCandidate;
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_SEND_OUT(opponent, 1); }
        TURN { EXPECT_MOVE(opponent, MOVE_BITE); }
    } SCENE {
        MESSAGE("The opposing Raticate used Bite!");
        HP_BAR(player, captureDamage: &damage[0]);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sObservedGnashingCandidate);
        EXPECT_EQ(sGnashingCandidateDefense, DEFAULT_STAT_STAGE);
        EXPECT_GE(damage[0] + damage[1], sGnashingCandidateDamage.minimum);
        EXPECT_LE(damage[0] + damage[1], sGnashingCandidateDamage.maximum);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}
