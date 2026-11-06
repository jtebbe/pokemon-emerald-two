#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObserved;
static s32 sScore;
static struct SimulatedDamage sDamage;
static s32 ObserveSadisticStreak(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sObserved = TRUE;
        sScore = score;
        sDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Sadistic Streak AI predicts Merciless and respects critical-hit immunity", s16 damage)
{
    enum Ability ability;
    u32 status;
    PARAMETRIZE { ability = ABILITY_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; status = STATUS1_POISON; }
    PARAMETRIZE { ability = ABILITY_NONE; status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; status = STATUS1_POISON; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSadisticStreak; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); if (status && ability != ABILITY_BATTLE_ARMOR) MESSAGE("A critical hit!"); else NOT MESSAGE("A critical hit!"); }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); EXPECT_GE(results[i].damage, sDamage.minimum); EXPECT_LE(results[i].damage, sDamage.maximum); EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_SADISTIC_STREAK); }
}

AI_SINGLE_BATTLE_TEST("Sadistic Streak AI values a knockout's three boosts without boosting the initial damage", s32 score, u32 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN_ALOLA) { Ability(ABILITY_FUR_COAT); Item(item); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSadisticStreak; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; results[i].damage = sDamage.median; }
    FINALLY { EXPECT_EQ(results[0].score + GOOD_EFFECT, results[1].score); EXPECT_EQ(results[0].damage, results[1].damage); EXPECT(IsMoxieTypeAbility(ABILITY_SADISTIC_STREAK)); }
}

AI_SINGLE_BATTLE_TEST("Sadistic Streak AI adds no future setup value to the battle's final knockout", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN_ALOLA) { Ability(ABILITY_FUR_COAT); Item(item); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSadisticStreak; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score, results[1].score); }
}

AI_SINGLE_BATTLE_TEST("Sadistic Streak AI takes a priority knockout instead of fainting before attacking")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Attack(4000); Speed(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Moves(MOVE_SCRATCH, MOVE_QUICK_ATTACK); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_QUICK_ATTACK, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(opponent->hp, 1); EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); }
}

AI_SINGLE_BATTLE_TEST("Sadistic Streak AI does not value a knockout boost that fatal Innards Out prevents", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_INNARDS_OUT); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN_ALOLA) { Ability(ABILITY_FUR_COAT); Item(item); HP(1); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSadisticStreak; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); EXPECT_SEND_OUT(opponent, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score, results[1].score); }
}
