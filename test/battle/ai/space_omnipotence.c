#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Space Dominus AI treats Hydro Pump as accurate and prefers its higher damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYDRO_PUMP, MOVE_SURF); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_HYDRO_PUMP, MOVE_SURF); EXPECT_MOVE(opponent, MOVE_HYDRO_PUMP); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_SPACE_DOMINUS);
        EXPECT_GT(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], 100);
        struct BattleCalcValues cv = {0};
        cv.battlerAtk = B_BATTLER_1;
        cv.battlerDef = B_BATTLER_0;
        cv.abilities[B_BATTLER_1] = ABILITY_SPACE_DOMINUS;
        cv.move = MOVE_HYDRO_PUMP;
        EXPECT(CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        EXPECT(AI_CanBypassProtect(B_BATTLER_1, B_BATTLER_0, MOVE_SPACIAL_REND));
        EXPECT(!AI_CanBypassProtect(B_BATTLER_1, B_BATTLER_0, MOVE_HYDRO_PUMP));
    }
}

AI_SINGLE_BATTLE_TEST("AI recognizes Space Dominus from the item reveal and discourages ineffective Protect")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_PALKIA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SPACIAL_REND); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_PROTECT, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPACIAL_REND); }
        TURN { MOVE(player, MOVE_SPACIAL_REND); SCORE_LT(opponent, MOVE_PROTECT, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_SPACE_DOMINUS);
        EXPECT_LT(ProtectChecks(B_BATTLER_1, B_BATTLER_0, MOVE_PROTECT, MOVE_SPACIAL_REND), 0);
    }
}

AI_SINGLE_BATTLE_TEST("Omnipotence AI predicts neutral damage through Ghost immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GENGAR) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); Moves(MOVE_STRENGTH, MOVE_VINE_WHIP); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_STRENGTH, MOVE_VINE_WHIP); EXPECT_MOVE(opponent, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(player);
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage damage = AI_CalcDamageSaveBattlers(MOVE_STRENGTH, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_EQ(effectiveness, UQ_4_12(1.0));
        EXPECT_GT(damage.minimum, 0);
        EXPECT(AI_CanBypassProtect(B_BATTLER_1, B_BATTLER_0, MOVE_STRENGTH));
        EXPECT(!AI_CanBypassProtect(B_BATTLER_1, B_BATTLER_0, MOVE_QUICK_ATTACK));
    }
}

AI_SINGLE_BATTLE_TEST("Omnipotence AI still avoids attacks blocked by immunity abilities")
{
    enum Species species;
    enum Ability ability;
    enum Move blockedMove;
    PARAMETRIZE { species = SPECIES_WEEZING; ability = ABILITY_LEVITATE; blockedMove = MOVE_EARTH_POWER; }
    PARAMETRIZE { species = SPECIES_VAPOREON; ability = ABILITY_WATER_ABSORB; blockedMove = MOVE_SURF; }
    PARAMETRIZE { species = SPECIES_SHEDINJA; ability = ABILITY_WONDER_GUARD; blockedMove = MOVE_STRENGTH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ARCEUS_NORMAL) { Item(ITEM_APEX_METEORITE); Moves(blockedMove, MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { SCORE_LT(opponent, blockedMove, MOVE_FLAMETHROWER); EXPECT_MOVE(opponent, MOVE_FLAMETHROWER); }
    }
}
