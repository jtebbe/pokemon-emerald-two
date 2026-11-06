#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

static bool32 sDropsObserved[PARTY_SIZE];
static bool32 sDropsCanWin[PARTY_SIZE];
static u32 sDropsWeather[PARTY_SIZE];
static struct SimulatedDamage sDropsDamage[PARTY_SIZE];
static void ObserveDropsCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex > 0 && context->partyMonIndex < 3 && gBattlerPartyIndexes[context->battler] == 0)
    {
        u32 slot = context->partyMonIndex;
        sDropsObserved[slot] = TRUE;
        sDropsCanWin[slot] = context->canBattlerWin1v1;
        sDropsWeather[slot] = AI_GetSwitchinWeather(context->battler);
        sDropsDamage[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][MOVESLOT_0];
    }
}

AI_SINGLE_BATTLE_TEST("Drops of God AI predicts entry rain and boosted Water damage without changing global weather")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Attack(20); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Speed(200); SpAttack(300); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_POLITOED) { Ability(ABILITY_WATER_ABSORB); HP(800); MaxHP(800); Speed(200); SpAttack(300); Moves(MOVE_WATER_GUN); }
    } WHEN {
        sDropsObserved[1] = sDropsObserved[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveDropsCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sDropsObserved[1]);
        EXPECT(sDropsObserved[2]);
        EXPECT(sDropsWeather[1] & B_WEATHER_RAIN_NORMAL);
        EXPECT_EQ(sDropsWeather[2], B_WEATHER_NONE);
        EXPECT_MUL_EQ(sDropsDamage[2].median, UQ_4_12(1.5), sDropsDamage[1].median);
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Drops of God AI accounts for recurring healing when choosing a winning switch-in")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(240); MaxHP(240); Speed(100); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(160); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_POLITOED) { Ability(ABILITY_WATER_ABSORB); HP(100); MaxHP(160); Speed(200); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        sDropsObserved[1] = sDropsObserved[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveDropsCandidate;
        TURN { MOVE(player, MOVE_SONIC_BOOM); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sDropsObserved[1]);
        EXPECT(sDropsObserved[2]);
        EXPECT(sDropsCanWin[1]);
        EXPECT(!sDropsCanWin[2]);
    }
}

AI_SINGLE_BATTLE_TEST("Drops of God AI does not predict rain healing through weather suppression")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CLOUD_NINE); HP(240); MaxHP(240); Speed(100); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(160); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_POLITOED) { Ability(ABILITY_WATER_ABSORB); HP(100); MaxHP(160); Speed(200); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        sDropsObserved[1] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveDropsCandidate;
        TURN { MOVE(player, MOVE_SONIC_BOOM); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sDropsObserved[1]);
        EXPECT_EQ(sDropsWeather[1], B_WEATHER_NONE);
        EXPECT(!sDropsCanWin[1]);
    }
}

AI_SINGLE_BATTLE_TEST("Drops of God AI values rain and avoids voluntarily replacing it with sun")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SUNNY_DAY, MOVE_WATER_GUN); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_SUNNY_DAY); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_DROPS_OF_GOD);
    }
}

AI_SINGLE_BATTLE_TEST("Drops of God AI recognizes opposing Apex users without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_DROPS_OF_GOD);
    }
}
