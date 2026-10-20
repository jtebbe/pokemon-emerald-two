#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "battle_ai_switch.h"

AI_SINGLE_BATTLE_TEST("Apex AI identifies Time Dominus and scores Geomancy without a charge turn")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_GEOMANCY, MOVE_SPLASH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_GEOMANCY, MOVE_SPLASH); EXPECT_MOVE(opponent, MOVE_GEOMANCY); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_TIME_DOMINUS);
        EXPECT(!IsTwoTurnNotSemiInvulnerableMove(B_BATTLER_1, MOVE_GEOMANCY));
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

AI_SINGLE_BATTLE_TEST("Apex AI evaluates charge attacks as immediate attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_METEOR_BEAM, MOVE_POWER_GEM); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_METEOR_BEAM, MOVE_POWER_GEM); EXPECT_MOVE(opponent, MOVE_METEOR_BEAM); }
    } SCENE {
        HP_BAR(player);
    } THEN {
        EXPECT(!IsTwoTurnNotSemiInvulnerableMove(B_BATTLER_1, MOVE_METEOR_BEAM));
        EXPECT_EQ(AI_GetMoldBreakerSanitizedAbility(B_BATTLER_0, ABILITY_MOLD_BREAKER, ABILITY_TIME_DOMINUS, HOLD_EFFECT_APEX_METEORITE, MOVE_SUNSTEEL_STRIKE), ABILITY_TIME_DOMINUS);
    }
}

AI_SINGLE_BATTLE_TEST("Ilex Whimsy AI does not waste turns reapplying its active protections")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_MIST, MOVE_SAFEGUARD, MOVE_LUCKY_CHANT, MOVE_SCRATCH); }
    } WHEN {
        TURN {
            SCORE_LT(opponent, MOVE_MIST, MOVE_SCRATCH);
            SCORE_LT(opponent, MOVE_SAFEGUARD, MOVE_SCRATCH);
            SCORE_LT(opponent, MOVE_LUCKY_CHANT, MOVE_SCRATCH);
            EXPECT_MOVE(opponent, MOVE_SCRATCH);
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids status moves and stat drops blocked by Ilex Whimsy")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC, MOVE_THUNDER_WAVE, MOVE_GROWL, MOVE_SCRATCH); }
    } WHEN {
        TURN {
            SCORE_LT(opponent, MOVE_TOXIC, MOVE_SCRATCH);
            SCORE_LT(opponent, MOVE_THUNDER_WAVE, MOVE_SCRATCH);
            SCORE_LT(opponent, MOVE_GROWL, MOVE_SCRATCH);
            EXPECT_MOVE(opponent, MOVE_SCRATCH);
        }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_ILEX_WHIMSY);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].heldEffect, HOLD_EFFECT_APEX_METEORITE);
    }
}

AI_SINGLE_BATTLE_TEST("AI damage prediction accounts for Ilex Whimsy blocking guaranteed critical hits")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WICKED_BLOW); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WICKED_BLOW); }
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage protected = AI_CalcDamageSaveBattlers(MOVE_WICKED_BLOW, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        u32 savedSideStatus = gSideStatuses[B_SIDE_PLAYER];
        gSideStatuses[B_SIDE_PLAYER] &= ~SIDE_STATUS_LUCKY_CHANT;
        struct SimulatedDamage unprotected = AI_CalcDamageSaveBattlers(MOVE_WICKED_BLOW, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        gSideStatuses[B_SIDE_PLAYER] = savedSideStatus;
        EXPECT_LT(protected.maximum, unprotected.minimum);
    }
}

AI_SINGLE_BATTLE_TEST("Apex reveal identifies Time Dominus without omniscient AI flags")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_TIME_DOMINUS);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_PLAYER][0].heldEffect, HOLD_EFFECT_APEX_METEORITE);
    }
}

AI_SINGLE_BATTLE_TEST("Time Dominus AI uses recharge moves on consecutive turns")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HYPER_BEAM, MOVE_SWIFT); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_HYPER_BEAM, MOVE_SWIFT); EXPECT_MOVE(opponent, MOVE_HYPER_BEAM); }
        TURN { EXPECT_MOVE(opponent, MOVE_HYPER_BEAM); }
    } SCENE {
        HP_BAR(player);
        HP_BAR(player);
    }
}

AI_SINGLE_BATTLE_TEST("Ilex Whimsy switch-in prediction leaves the real side's timers and flags unchanged")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WICKED_BLOW); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_CELEBI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        u32 sideStatus = gSideStatuses[B_SIDE_OPPONENT];
        struct SideTimer timers = gSideTimers[B_SIDE_OPPONENT];
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_OPTIONAL);
        EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT], sideStatus);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].mistTimer, timers.mistTimer);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].safeguardTimer, timers.safeguardTimer);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].luckyChantTimer, timers.luckyChantTimer);
    }
}
