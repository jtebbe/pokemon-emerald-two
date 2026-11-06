#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static bool32 sObserved, sEndures;
static s32 sScore;
static struct SimulatedDamage sDamage;
static s32 ObserveScrapBrawler(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && move == gBattleMons[attacker].moves[0]
     && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        sObserved = TRUE;
        sEndures = CanEndureHit(attacker, defender, move);
        sScore = score;
        sDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI prefers contact damage plus immediate hazards over a stronger non-contact attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_SWIFT); EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN { EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_SCRAP_BRAWLER); }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI respects Boots and Magic Guard when forecasting immediate hazards")
{
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { item = ITEM_HEAVY_DUTY_BOOTS; ability = ABILITY_NONE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_MAGIC_GUARD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Ability(ability); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_SWIFT, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SWIFT); } }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI includes new hazard damage in its predicted range", s16 attackDamage, s16 hazardDamage)
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_TAILLOW; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { HP(800); MaxHP(800); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(1); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    SCENE {
        HP_BAR(player, captureDamage: &results[i].attackDamage);
        ABILITY_POPUP(opponent, ABILITY_SCRAP_BRAWLER);
        if (species == SPECIES_WOBBUFFET) HP_BAR(player, captureDamage: &results[i].hazardDamage);
    } THEN {
        sDynamicAiFunc = NULL; EXPECT(sObserved);
        if (species == SPECIES_WOBBUFFET) {
            EXPECT_EQ(results[i].hazardDamage, 100);
            EXPECT_GE(results[i].attackDamage + results[i].hazardDamage, sDamage.minimum);
            EXPECT_LE(results[i].attackDamage + results[i].hazardDamage, sDamage.maximum);
        } else {
            EXPECT_GT(sDamage.median, results[i].attackDamage);
            EXPECT_GT(sDamage.maximum, sDamage.minimum + 150);
        }
        // Prediction must leave the actual field with just the real attack's roll.
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_PLAYER], 1);
    }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI predicts Tough Claws without adding hazards for immune targets", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HEAVY_DUTY_BOOTS); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); EXPECT_GE(results[i].damage, sDamage.minimum); EXPECT_LE(results[i].damage, sDamage.maximum); }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI values future hazard setup even when immediate activation is blocked", s32 score)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HEAVY_DUTY_BOOTS); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_PERRSERKER) { Ability(ABILITY_TOUGH_CLAWS); Item(item); Speed(100); Moves(MOVE_SWIFT); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { EXPECT_MOVE(opponent, MOVE_SWIFT); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); results[i].score = sScore; }
    FINALLY { EXPECT_EQ(results[0].score + DECENT_EFFECT, results[1].score); }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI does not predict hazard damage after type-immune attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTLY) { HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_SCRATCH, MOVE_METAL_CLAW); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_METAL_CLAW, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_METAL_CLAW); } }
    THEN { EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0].median, 0); }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI recognizes a knockout caused by immediate hazard damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(800); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_HEAVY_DUTY_BOOTS); Speed(1); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(1); SpAttack(1); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_SWIFT); EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); EXPECT_GE(sDamage.minimum, 50); }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI predicts hazard growth and repeated activation for multihit moves", s16 hit1, s16 hazard1, s16 hit2, s16 hazard2)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(1); Speed(100); Moves(MOVE_DOUBLE_HIT); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { EXPECT_MOVE(opponent, MOVE_DOUBLE_HIT); } }
    SCENE {
        HP_BAR(player, captureDamage: &results[i].hit1);
        ABILITY_POPUP(opponent, ABILITY_SCRAP_BRAWLER);
        HP_BAR(player, captureDamage: &results[i].hazard1);
        HP_BAR(player, captureDamage: &results[i].hit2);
        // Repeated Stealth Rock selections need not display another popup.
    } THEN {
        sDynamicAiFunc = NULL; EXPECT(sObserved);
        EXPECT_GT(sDamage.minimum, 150);
        EXPECT_GT(sDamage.maximum, sDamage.minimum + 75);
        EXPECT_GE(800 - player->hp, sDamage.minimum);
        EXPECT_LE(800 - player->hp, sDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Scrap Brawler AI knows contact hazards bypass Focus Sash and Sturdy")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_STURDY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_FOCUS_SASH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(100); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_HEAVY_DUTY_BOOTS); Speed(1); }
        OPPONENT(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(4000); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved = FALSE; sDynamicAiFunc = ObserveScrapBrawler; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved); EXPECT(!sEndures); }
}
