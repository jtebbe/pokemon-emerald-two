#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sZoneObserved;
static s32 sZoneScore;
static struct SimulatedDamage sZoneDamage;
static s32 ObserveConductionZone(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == gBattleMons[attacker].moves[0]
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sZoneObserved = TRUE;
        sZoneScore = score;
        sZoneDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Conduction Zone AI values guaranteed paralysis and respects its protections", s32 score)
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_LIMBER; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CHERI_BERRY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SHOCK_WAVE); }
    } WHEN {
        sZoneObserved = FALSE; sDynamicAiFunc = ObserveConductionZone;
        TURN { EXPECT_MOVE(opponent, MOVE_SHOCK_WAVE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sZoneObserved);
        results[i].score = sZoneScore;
    } FINALLY {
        for (u32 blocked = 1; blocked < 5; blocked++)
            EXPECT_GT(results[0].score, results[blocked].score);
    }
}

AI_SINGLE_BATTLE_TEST("Conduction Zone AI forecasts paralysis even when both battlers are airborne", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEVITATE); HP(3000); MaxHP(3000); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CONDUCTION_ZONE); Item(ITEM_AIR_BALLOON); Speed(100); Moves(MOVE_SHOCK_WAVE); }
    } WHEN {
        sZoneObserved = FALSE; sDynamicAiFunc = ObserveConductionZone;
        TURN { EXPECT_MOVE(opponent, MOVE_SHOCK_WAVE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sZoneObserved);
        EXPECT_GE(results[i].damage, sZoneDamage.minimum);
        EXPECT_LE(results[i].damage, sZoneDamage.maximum);
        EXPECT_EQ(player->status1, STATUS1_PARALYSIS);
        EXPECT(!AI_IsBattlerGrounded(B_BATTLER_0));
        EXPECT(!AI_IsBattlerGrounded(B_BATTLER_1));
    }
}

AI_SINGLE_BATTLE_TEST("Conduction Zone AI stops valuing its extra paralysis after terrain is overwritten")
{
    GIVEN {
        ASSUME(B_TERRAIN_TYPE_BOOST >= GEN_8);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CHERI_BERRY); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE, MOVE_GRASSY_TERRAIN); }
        OPPONENT(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SHOCK_WAVE, MOVE_POWER_GEM); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); EXPECT_MOVE(opponent, MOVE_SHOCK_WAVE); }
        TURN { EXPECT_MOVE(opponent, MOVE_POWER_GEM); }
    } THEN {
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_GRASSY);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

static bool32 sZoneCandidateSeen[PARTY_SIZE];
static u32 sZoneCandidateTerrain[PARTY_SIZE], sZoneCandidateDamage[PARTY_SIZE];
static void ObserveConductionCandidate(struct SwitchAiContext *context)
{
    if (gBattlerPartyIndexes[context->battler] != 0 || context->partyMonIndex == 0)
        return;
    enum PartySlot slot = context->partyMonIndex;
    sZoneCandidateSeen[slot] = TRUE;
    sZoneCandidateTerrain[slot] = AI_GetSwitchinTerrain(context->battler);
    sZoneCandidateDamage[slot] = gAiLogicData->simulatedDmg[context->battler][context->opposingBattler][0].median;
}

AI_SINGLE_BATTLE_TEST("Conduction Zone AI forecasts Electric Terrain on entry without leaking it to other candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); SpAttack(100); Speed(2); Moves(MOVE_SHOCK_WAVE); }
        OPPONENT(SPECIES_GOLEM_ALOLA) { HP(1000); MaxHP(1000); SpAttack(100); Speed(2); Moves(MOVE_SHOCK_WAVE); }
    } WHEN {
        sZoneCandidateSeen[1] = sZoneCandidateSeen[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveConductionCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sZoneCandidateSeen[1]);
        EXPECT(sZoneCandidateSeen[2]);
        EXPECT_EQ(sZoneCandidateTerrain[1], B_TERRAIN_ELECTRIC);
        EXPECT_EQ(sZoneCandidateTerrain[2], B_TERRAIN_NONE);
        if (B_TERRAIN_TYPE_BOOST >= GEN_8)
            EXPECT_MUL_EQ(sZoneCandidateDamage[2], Q_4_12(1.3), sZoneCandidateDamage[1]);
        else
            EXPECT_MUL_EQ(sZoneCandidateDamage[2], Q_4_12(1.5), sZoneCandidateDamage[1]);
    }
}

AI_SINGLE_BATTLE_TEST("Conduction Zone AI values paralysis on targeted Electric status moves", s32 score)
{
    bool32 cloak;
    PARAMETRIZE { cloak = FALSE; }
    PARAMETRIZE { cloak = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(cloak ? ITEM_COVERT_CLOAK : ITEM_NONE); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_GOLEM_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_EERIE_IMPULSE); }
    } WHEN {
        sZoneObserved = FALSE; sDynamicAiFunc = ObserveConductionZone;
        TURN { MOVE(player, MOVE_PSYCHIC); EXPECT_MOVE(opponent, MOVE_EERIE_IMPULSE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sZoneObserved);
        results[i].score = sZoneScore;
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->status1, cloak ? STATUS1_NONE : STATUS1_PARALYSIS);
    } FINALLY {
        EXPECT_GT(results[0].score, results[1].score);
    }
}
