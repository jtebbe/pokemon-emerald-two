#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObserved;
static s32 sScore;
static struct SimulatedDamage sDamage;
static s32 ObserveSickeningGreed(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sObserved = TRUE;
        sScore = score;
        sDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI values Pay Day's five stat boosts over immediate damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_PAY_DAY, MOVE_SLASH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_PAY_DAY, MOVE_SLASH); EXPECT_MOVE(opponent, MOVE_PAY_DAY); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_SICKENING_GREED); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI recognizes useful Pay Day boosts despite Normal immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTLY) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_PAY_DAY, MOVE_NIGHT_SLASH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_PAY_DAY, MOVE_NIGHT_SLASH); EXPECT_MOVE(opponent, MOVE_PAY_DAY); } }
    THEN { EXPECT_EQ(player->hp, 3000); for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI stops using immune Pay Day once all core stats are capped")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTLY) { HP(10000); MaxHP(10000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_PAY_DAY, MOVE_NIGHT_SLASH); }
    } WHEN {
        for (u32 turn = 0; turn < 6; turn++) TURN { EXPECT_MOVE(opponent, MOVE_PAY_DAY); }
        TURN { SCORE_GT(opponent, MOVE_NIGHT_SLASH, MOVE_PAY_DAY); EXPECT_MOVE(opponent, MOVE_NIGHT_SLASH); }
    } THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(opponent->statStages[stat], MAX_STAT_STAGE); EXPECT_LT(player->hp, 10000); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI adds knockout value while predicting unboosted attack damage", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); Item(item); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSickeningGreed; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; EXPECT_GT(sDamage.median, 1); }
    FINALLY { EXPECT_EQ(results[0].score + GOOD_EFFECT, results[1].score); EXPECT(IsMoxieTypeAbility(ABILITY_SICKENING_GREED)); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI prefers Pay Day when it earns both use and knockout boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_PAY_DAY, MOVE_SCRATCH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_PAY_DAY, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_PAY_DAY); SEND_OUT(player, 1); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE + 2); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI uses a priority knockout instead of dying before Pay Day")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Attack(4000); Speed(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Moves(MOVE_PAY_DAY, MOVE_QUICK_ATTACK); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_QUICK_ATTACK, MOVE_PAY_DAY); EXPECT_MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(opponent->hp, 1); for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI does not award knockout setup value when the battle ends", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); Item(item); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSickeningGreed; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score, results[1].score); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI's Pay Day damage prediction excludes its later stat boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Attack(300); Speed(100); Moves(MOVE_PAY_DAY); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSickeningGreed; TURN { EXPECT_MOVE(opponent, MOVE_PAY_DAY); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); EXPECT_GE(3000 - player->hp, sDamage.minimum); EXPECT_LE(3000 - player->hp, sDamage.maximum); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI does not value Pay Day boosts lost to Red Card", s32 score)
{
    enum Item attackerItem, defenderItem;
    PARAMETRIZE { attackerItem = ITEM_NONE; defenderItem = ITEM_NONE; }
    PARAMETRIZE { attackerItem = ITEM_APEX_METEORITE; defenderItem = ITEM_NONE; }
    PARAMETRIZE { attackerItem = ITEM_NONE; defenderItem = ITEM_RED_CARD; }
    PARAMETRIZE { attackerItem = ITEM_APEX_METEORITE; defenderItem = ITEM_RED_CARD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(defenderItem); HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); Item(attackerItem); Moves(MOVE_PAY_DAY); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSickeningGreed; TURN { EXPECT_MOVE(opponent, MOVE_PAY_DAY); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_GT(results[1].score, results[0].score); EXPECT_EQ(results[2].score, results[3].score); }
}

AI_SINGLE_BATTLE_TEST("Sickening Greed AI does not value boosts when Innards Out will knock its user out", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_INNARDS_OUT); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); Item(item); HP(1); Speed(100); Moves(MOVE_PAY_DAY); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveSickeningGreed; TURN { EXPECT_MOVE(opponent, MOVE_PAY_DAY); SEND_OUT(player, 1); EXPECT_SEND_OUT(opponent, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score, results[1].score); }
}
