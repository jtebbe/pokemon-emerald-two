#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

AI_SINGLE_BATTLE_TEST("Distortion AI chooses damage using type-based categories rather than inverted categories")
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_GIRATINA_ALTERED) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Attack(40); SpAttack(240); Moves(MOVE_BITE, MOVE_SHADOW_BALL); }
    } WHEN {
        if (apex)
            TURN { SCORE_GT(opponent, MOVE_BITE, MOVE_SHADOW_BALL); EXPECT_MOVE(opponent, MOVE_BITE); }
        else
            TURN { SCORE_GT(opponent, MOVE_SHADOW_BALL, MOVE_BITE); EXPECT_MOVE(opponent, MOVE_SHADOW_BALL); }
    }
}

AI_SINGLE_BATTLE_TEST("AI damage prediction includes an opposing Distortion holder without omniscience")
{
    s16 damage;
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_LINOONE) { Attack(240); SpAttack(40); Moves(MOVE_SHADOW_BALL, MOVE_BITE); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_SHADOW_BALL, MOVE_BITE); EXPECT_MOVE(opponent, MOVE_SHADOW_BALL); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage estimate = AI_CalcDamageSaveBattlers(MOVE_SHADOW_BALL, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_GE(damage, estimate.minimum);
        EXPECT_LE(damage, estimate.maximum);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_DISTORTION);
    }
}

AI_SINGLE_BATTLE_TEST("AI screens and stat category helpers follow Distortion's type-based categories")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
        OPPONENT(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); Moves(MOVE_REFLECT, MOVE_LIGHT_SCREEN); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_LIGHT_SCREEN, MOVE_REFLECT); EXPECT_MOVE(opponent, MOVE_LIGHT_SCREEN); }
    } THEN {
        EXPECT(HasMoveWithCategory(B_BATTLER_0, DAMAGE_CATEGORY_SPECIAL));
        EXPECT(!HasMoveWithCategory(B_BATTLER_0, DAMAGE_CATEGORY_PHYSICAL));
    }
}

AI_DOUBLE_BATTLE_TEST("Distortion damage caching is discarded before the next simulation after a holder faints")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_LINOONE) { Attack(240); SpAttack(40); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage withAura = AI_CalcDamageSaveBattlers(MOVE_BITE, B_BATTLER_2, B_BATTLER_1, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT(!gAiLogicData->distortionCached);
        u32 hp = playerLeft->hp;
        playerLeft->hp = 0;
        struct SimulatedDamage withoutAura = AI_CalcDamageSaveBattlers(MOVE_BITE, B_BATTLER_2, B_BATTLER_1, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT(!gAiLogicData->distortionCached);
        EXPECT_GT(withoutAura.minimum, withAura.maximum * 3);
        EXPECT(!IsDistortionActive());
        playerLeft->hp = hp;
        EXPECT(IsDistortionActive());
    }
}

AI_SINGLE_BATTLE_TEST("AI switch-in simulation does not leave a benched Distortion ability active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_GIRATINA_ALTERED) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SHADOW_BALL); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_OPTIONAL);
        EXPECT(!IsDistortionActive());
        EXPECT_EQ(GetBattleMoveCategory(MOVE_BITE), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT_EQ(GetBattleMoveCategory(MOVE_SHADOW_BALL), DAMAGE_CATEGORY_SPECIAL);
    }
}
