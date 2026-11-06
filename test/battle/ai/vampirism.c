#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sVampirismDamage;
static bool32 sVampirismObserved;
static s32 sVampirismScore;
static u32 sVampirismObservedTurn;
static s32 ObserveVampirism(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0
     && gBattleResults.battleTurnCounter == sVampirismObservedTurn && !gAiLogicData->aiPredictionInProgress)
    {
        sVampirismDamage = gAiLogicData->simulatedDmg[attacker][defender][gAiThinkingStruct->movesetIndex];
        sVampirismScore = score;
        sVampirismObserved = TRUE;
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Vampirism AI predicts its damage bonus against injured targets", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    u32 hp = 3000;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; hp = 3000; }
        PARAMETRIZE { move = moves[j]; hp = 2999; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); Moves(move); }
    } WHEN {
        sVampirismObserved = FALSE;
        sVampirismObservedTurn = 0;
        sDynamicAiFunc = ObserveVampirism;
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sVampirismObserved);
        EXPECT_GE(results[i].damage, sVampirismDamage.minimum);
        EXPECT_LE(results[i].damage, sVampirismDamage.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_VAMPIRISM);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.3), results[3].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Vampirism AI freely chooses a powerful recoil move at one HP")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DOUBLE_EDGE; }
    PARAMETRIZE { move = MOVE_CHLOROBLAST; }
    PARAMETRIZE { move = MOVE_STEEL_BEAM; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Moves(move, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_GT(opponent, move, MOVE_TACKLE); EXPECT_MOVE(opponent, move); }
    } THEN {
        EXPECT(!AI_IsDamagedByRecoil(B_BATTLER_1));
        EXPECT_GT(opponent->hp, 1);
    }
}

AI_SINGLE_BATTLE_TEST("Vampirism AI values healing that lets it survive an otherwise fatal attack")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LIQUID_OOZE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(30); MaxHP(400); Level(60); Speed(100); Moves(MOVE_SEISMIC_TOSS, MOVE_SONIC_BOOM); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); SCORE_GT(opponent, MOVE_SEISMIC_TOSS, MOVE_SONIC_BOOM); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, damage: 60);
        ABILITY_POPUP(opponent, ABILITY_VAMPIRISM);
        HP_BAR(opponent, damage: -30);
        HP_BAR(opponent, damage: 40);
    } THEN {
        EXPECT_EQ(opponent->hp, 20);
    }
}

AI_SINGLE_BATTLE_TEST("Vampirism AI does not reward recovery blocked by Heal Block", s32 score)
{
    bool32 blocked;
    PARAMETRIZE { blocked = FALSE; }
    PARAMETRIZE { blocked = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_DRAGON_RAGE, MOVE_HEAL_BLOCK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Level(60); Speed(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        sVampirismObserved = FALSE;
        sVampirismObservedTurn = 1;
        sDynamicAiFunc = ObserveVampirism;
        TURN { MOVE(player, blocked ? MOVE_HEAL_BLOCK : MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); EXPECT_MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sVampirismObserved);
        results[i].score = sVampirismScore;
    } FINALLY {
        EXPECT_EQ(results[0].score, results[1].score + DECENT_EFFECT);
    }
}

AI_SINGLE_BATTLE_TEST("Vampirism AI recognizes the opposing Apex ability without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_GOLBAT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_VAMPIRISM);
    }
}

AI_SINGLE_BATTLE_TEST("Vampirism AI predicts the wounded bonus on later hits without leaking simulated HP changes")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DOUBLE_HIT; }
    PARAMETRIZE { move = MOVE_BULLET_SEED; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_CROBAT) { Item(ITEM_APEX_METEORITE); Attack(300); Moves(move); }
    } WHEN {
        sVampirismObserved = FALSE;
        sVampirismObservedTurn = 0;
        sDynamicAiFunc = ObserveVampirism;
        TURN { EXPECT_MOVE(opponent, move); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sVampirismObserved);
        u32 damage = player->maxHP - player->hp;
        EXPECT_GE(damage, sVampirismDamage.minimum);
        EXPECT_LE(damage, sVampirismDamage.maximum);
    }
}
