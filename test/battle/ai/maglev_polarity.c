#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static EWRAM_DATA struct SimulatedDamage sMaglevDamage;
static EWRAM_DATA bool32 sMaglevObserved;
static EWRAM_DATA s32 sMaglevScore;
static EWRAM_DATA u32 sMaglevObservedTurn;
static s32 ObserveMaglev(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sMaglevObservedTurn && !gAiLogicData->aiPredictionInProgress)
    {
        sMaglevDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sMaglevScore = score;
        sMaglevObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI predicts double damage against Electric and Steel targets", s16 damage)
{
    enum Item item = ITEM_NONE;
    enum Species species = SPECIES_PIKACHU;
    enum Move move = MOVE_SCRATCH;
    const enum Species speciesList[] = { SPECIES_PIKACHU, SPECIES_REGISTEEL, SPECIES_MAGNEZONE, SPECIES_WOBBUFFET };
    const enum Move moves[] = { MOVE_SCRATCH, MOVE_WATER_GUN };
    for (u32 j = 0; j < ARRAY_COUNT(speciesList); j++) {
        for (u32 k = 0; k < ARRAY_COUNT(moves); k++) {
            PARAMETRIZE { species = speciesList[j]; move = moves[k]; item = ITEM_NONE; }
            PARAMETRIZE { species = speciesList[j]; move = moves[k]; item = ITEM_APEX_METEORITE; }
        }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_MAGNEZONE) { Item(item); Attack(200); SpAttack(200); Moves(move); }
    } WHEN {
        sMaglevObserved = FALSE;
        sMaglevObservedTurn = 0;
        sDynamicAiFunc = ObserveMaglev;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMaglevObserved);
        EXPECT_GE(results[i].damage, sMaglevDamage.minimum);
        EXPECT_LE(results[i].damage, sMaglevDamage.maximum);
    } FINALLY {
        for (u32 j = 0; j < 8; j++)
            EXPECT_MUL_EQ(results[j * 2].damage, j < 6 ? UQ_4_12(2.0) : UQ_4_12(1.0), results[j * 2 + 1].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI avoids Ground moves even with Mold Breaker without omniscience")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_EARTHQUAKE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_EARTHQUAKE, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_MAGLEV_POLARITY);
        EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][0].maximum, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI switches into its Ground immunity against Mold Breaker")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TINKATON) { Ability(ABILITY_MOLD_BREAKER); Speed(3); }
        OPPONENT(SPECIES_PONYTA) { Level(1); Item(ITEM_EJECT_PACK); Moves(MOVE_OVERHEAT); Speed(4); }
        OPPONENT(SPECIES_MAGNEZONE) { HP(1); Item(ITEM_APEX_METEORITE); Moves(MOVE_FLAMETHROWER); Speed(1); }
        OPPONENT(SPECIES_HYPNO) { Moves(MOVE_IRON_HEAD); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SLAP); EXPECT_SEND_OUT(opponent, 1); }
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI values recovery that lets it survive an otherwise fatal attack")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LIQUID_OOZE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Ability(ability); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(30); MaxHP(400); Level(60); Speed(100); Moves(MOVE_SEISMIC_TOSS, MOVE_SONIC_BOOM); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); SCORE_GT(opponent, MOVE_SEISMIC_TOSS, MOVE_SONIC_BOOM); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, damage: 60);
        ABILITY_POPUP(opponent, ABILITY_MAGLEV_POLARITY);
        HP_BAR(opponent, damage: -20);
        HP_BAR(opponent, damage: 40);
    } THEN {
        EXPECT_EQ(opponent->hp, 10);
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI does not reward recovery blocked by Heal Block", s32 score)
{
    bool32 blocked;
    PARAMETRIZE { blocked = FALSE; }
    PARAMETRIZE { blocked = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_PRANKSTER); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_DRAGON_RAGE, MOVE_HEAL_BLOCK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Level(60); Speed(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        sMaglevObserved = FALSE;
        sMaglevObservedTurn = 1;
        sDynamicAiFunc = ObserveMaglev;
        TURN { MOVE(player, blocked ? MOVE_HEAL_BLOCK : MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMaglevObserved);
        results[i].score = sMaglevScore;
    } FINALLY {
        EXPECT_EQ(results[0].score, results[1].score + DECENT_EFFECT);
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI predicts all hits of a multi-hit attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); Attack(300); Moves(MOVE_DOUBLE_HIT); }
    } WHEN {
        sMaglevObserved = FALSE;
        sMaglevObservedTurn = 0;
        sDynamicAiFunc = ObserveMaglev;
        TURN { EXPECT_MOVE(opponent, MOVE_DOUBLE_HIT); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMaglevObserved);
        u32 damage = player->maxHP - player->hp;
        EXPECT_GE(damage, sMaglevDamage.minimum);
        EXPECT_LE(damage, sMaglevDamage.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI recognizes that Gravity removes its Ground immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRAVITY); EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
        TURN { SCORE_GT(opponent, MOVE_EARTHQUAKE, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
    }
}

AI_SINGLE_BATTLE_TEST("Maglev Polarity AI rewards recovery only against qualifying types", s32 score)
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_PIKACHU; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ABILITY_NONE); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGNEZONE) { Item(ITEM_APEX_METEORITE); HP(30); MaxHP(400); Level(60); Speed(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        sMaglevObserved = FALSE;
        sMaglevObservedTurn = 0;
        sDynamicAiFunc = ObserveMaglev;
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sMaglevObserved);
        results[i].score = sMaglevScore;
    } FINALLY {
        EXPECT_EQ(results[1].score, results[0].score + DECENT_EFFECT);
    }
}
