#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
extern bool32 DoesBattlerKOItselfWithRecoil(enum BattlerId attacker, enum BattlerId defender, enum Move move);
static bool32 sObservedChase;
static struct SimulatedDamage sChaseDamage;
static bool32 sChaseRecoilKO, sChasePredictingMove;
static s32 ObserveChase(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == gBattleMons[attacker].moves[0]
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sObservedChase = TRUE;
        sChasePredictingMove = gAiLogicData->predictingMove;
        sChaseDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sChaseRecoilKO = DoesBattlerKOItselfWithRecoil(attacker, defender, move);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI classifies offensive and opponent-targeted status moves as Pursuit moves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN {
        EXPECT(AI_IsChaseMove(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(AI_IsChaseMove(B_BATTLER_1, MOVE_SURF));
        EXPECT(AI_IsChaseMove(B_BATTLER_1, MOVE_WILL_O_WISP));
        EXPECT(AI_IsChaseMove(B_BATTLER_1, MOVE_PURSUIT));
        EXPECT(!AI_IsChaseMove(B_BATTLER_1, MOVE_PROTECT));
        EXPECT(!AI_IsChaseMove(B_BATTLER_1, MOVE_SWORDS_DANCE));
        EXPECT(!AI_IsChaseMove(B_BATTLER_1, MOVE_SPIKES));
    }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI predicts interception damage while preserving Pursuit's own boost")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_PURSUIT; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Moves(move); }
    } WHEN { TURN { EXPECT_MOVE(opponent, move); } }
    THEN {
        bool32 savedPredicting = gAiLogicData->predictingSwitch;
        u32 savedSwitches = gAiLogicData->shouldSwitch;
        uq4_12_t effectiveness;
        gAiLogicData->predictingSwitch = FALSE;
        struct SimulatedDamage normal = AI_CalcDamageSaveBattlers(move, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        gAiLogicData->predictingSwitch = TRUE;
        gAiLogicData->shouldSwitch = 1u << B_BATTLER_0;
        struct SimulatedDamage chase = AI_CalcDamageSaveBattlers(move, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_MUL_EQ(normal.median, move == MOVE_PURSUIT ? Q_4_12(3.0) : Q_4_12(1.5), chase.median);
        EXPECT(AI_IsRelentlessChaseIntercept(B_BATTLER_1, B_BATTLER_0, move));
        gAiLogicData->predictingSwitch = savedPredicting;
        gAiLogicData->shouldSwitch = savedSwitches;
    }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI predicts fast pivot interception and its recoil protection", s16 damage)
{
    PARAMETRIZE {}
    PASSES_RANDOMLY(1, 1, RNG_AI_PREDICT_MOVE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Attack(1); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(800); Speed(1); Moves(MOVE_HEAD_SMASH); }
    } WHEN { sObservedChase = FALSE; sDynamicAiFunc = ObserveChase; TURN { MOVE(player, MOVE_U_TURN); EXPECT_MOVE(opponent, MOVE_HEAD_SMASH); SEND_OUT(player, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAD_SMASH, opponent); HP_BAR(player, captureDamage: &results[i].damage); SEND_IN_MESSAGE("Wynaut"); }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObservedChase);
        if (sChasePredictingMove)
        {
            EXPECT_GE(results[i].damage, sChaseDamage.minimum);
            EXPECT_LE(results[i].damage, sChaseDamage.maximum);
            EXPECT(!sChaseRecoilKO);
        }
        else
        {
            EXPECT_GT(results[i].damage, sChaseDamage.maximum);
            EXPECT(sChaseRecoilKO);
        }
        EXPECT_GT(opponent->hp, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI keeps scoring against the outgoing Pokemon when predicting an incoming mon")
{
    PASSES_RANDOMLY(1, 1, RNG_AI_PREDICT_SWITCH);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PREDICT_SWITCH | AI_FLAG_PREDICT_INCOMING_MON);
        PLAYER(SPECIES_GENGAR) { HP(1); MaxHP(3000); SpAttack(1); Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_CONKELDURR) { Attack(4000); HP(3000); MaxHP(3000); Moves(MOVE_HAMMER_ARM); }
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(500); Moves(MOVE_BITE, MOVE_BODY_SLAM); }
    } WHEN { TURN { SWITCH(player, 1); EXPECT_MOVE(opponent, MOVE_BITE); SEND_OUT(player, 1); } }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI does not apply the damage bonus without a switching or pivoting target", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SCRATCH); }
    } WHEN { sObservedChase = FALSE; sDynamicAiFunc = ObserveChase; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObservedChase); EXPECT_GE(results[i].damage, sChaseDamage.minimum); EXPECT_LE(results[i].damage, sChaseDamage.maximum); EXPECT(!AI_IsRelentlessChaseIntercept(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH)); }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI treats interception as acting before a manual switch without changing raw Speed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_SCRATCH); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN {
        bool32 savedPredicting = gAiLogicData->predictingSwitch;
        u32 savedSwitches = gAiLogicData->shouldSwitch;
        gAiLogicData->predictingSwitch = TRUE;
        gAiLogicData->shouldSwitch = 1u << B_BATTLER_0;
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_CELEBRATE, CONSIDER_PRIORITY));
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_CELEBRATE, DONT_CONSIDER_PRIORITY));
        gAiLogicData->predictingSwitch = savedPredicting;
        gAiLogicData->shouldSwitch = savedSwitches;
    }
}

AI_SINGLE_BATTLE_TEST("Relentless Chase AI recognizes an opposing Apex holder without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_CELEBRATE); } }
    THEN { EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_RELENTLESS_CHASE); }
}
