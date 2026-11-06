#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
extern bool32 ShouldStayInToUseMove(struct SwitchAiContext *switchContext);
static bool32 sObserved;
static s32 sScore;
static enum AIPivot sPivot;
static s32 ObserveTunnelTrouble(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sObserved = TRUE;
        sScore = score;
        sPivot = ShouldPivot(attacker, defender, move);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI identifies Ground attacks and status moves as pivots but excludes Dig")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_TUNNEL_TROUBLE);
        EXPECT(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_EARTH_POWER));
        EXPECT(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_EARTHQUAKE));
        EXPECT(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_SPIKES));
        EXPECT(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_SHORE_UP));
        EXPECT(!AI_IsSwitchOutMove(B_BATTLER_1, MOVE_DIG));
        EXPECT(!AI_IsSwitchOutMove(B_BATTLER_1, MOVE_ROCK_SLIDE));
        EXPECT(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_U_TURN));
    }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI pivots after attacking or using Ground status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_EARTH_POWER; }
    PARAMETRIZE { move = MOVE_SPIKES; }
    PARAMETRIZE { move = MOVE_SHORE_UP; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); HP(50); MaxHP(100); Moves(move); }
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_METEOR_MASH); }
    } WHEN { TURN { EXPECT_MOVE(opponent, move); EXPECT_SEND_OUT(opponent, 1); } }
    SCENE { ABILITY_POPUP(opponent, ABILITY_TUNNEL_TROUBLE); MESSAGE(AI_TRAINER_NAME " sent out Metagross!"); }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI applies the same pivot scoring as U-turn", s32 score, enum AIPivot pivot)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DUGTRIO) { Ability(ABILITY_SAND_VEIL); Item(item); Moves(MOVE_EARTH_POWER); }
        OPPONENT(SPECIES_MAGIKARP) { Level(1); Moves(MOVE_SPLASH); }
    } WHEN {
        sObserved = FALSE;
        sDynamicAiFunc = ObserveTunnelTrouble;
        TURN { EXPECT_MOVE(opponent, MOVE_EARTH_POWER); if (item == ITEM_APEX_METEORITE) EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved);
        results[i].score = sScore;
        results[i].pivot = sPivot;
    } FINALLY {
        s32 adjustment = results[1].pivot == DONT_PIVOT ? -5 : results[1].pivot == SHOULD_PIVOT ? BEST_EFFECT : 0;
        EXPECT_EQ(results[1].score, results[0].score + adjustment);
    }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI does not add a pivot penalty when no replacement is available", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DUGTRIO) { Ability(ABILITY_SAND_VEIL); Item(item); Moves(MOVE_EARTH_POWER); }
    } WHEN {
        sObserved = FALSE;
        sDynamicAiFunc = ObserveTunnelTrouble;
        TURN { EXPECT_MOVE(opponent, MOVE_EARTH_POWER); }
    } THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score, results[1].score); }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI uses the move's dynamic type")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_EARTH_PLATE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUGTRIO) { Ability(ABILITY_TUNNEL_TROUBLE); Item(item); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(AI_IsSwitchOutMove(B_BATTLER_1, MOVE_JUDGMENT), item == ITEM_EARTH_PLATE); }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI can keep a highly scored Ground pivot instead of hard switching")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_EARTH_POWER, MOVE_DIG); }
    } WHEN { TURN {} }
    THEN {
        struct SwitchAiContext ctx = {0};
        ctx.battler = B_BATTLER_1;
        ctx.opposingBattler = B_BATTLER_0;
        gBattleMons[B_BATTLER_1].moves[0] = MOVE_CELEBRATE;
        gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][0] = 0;
        gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][1] = AI_GOOD_SCORE_THRESHOLD + 1;
        gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][2] = AI_GOOD_SCORE_THRESHOLD + 1;
        EXPECT(ShouldStayInToUseMove(&ctx));
        gAiBattleData->finalScore[B_BATTLER_1][B_BATTLER_0][1] = 0;
        EXPECT(!ShouldStayInToUseMove(&ctx));
    }
}

AI_SINGLE_BATTLE_TEST("Tunnel Trouble AI avoids an immune Ground pivot when a damaging alternative exists")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIDGEY) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Moves(MOVE_EARTH_POWER, MOVE_ROCK_SLIDE); }
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_METEOR_MASH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_ROCK_SLIDE, MOVE_EARTH_POWER); EXPECT_MOVE(opponent, MOVE_ROCK_SLIDE); } }
}
