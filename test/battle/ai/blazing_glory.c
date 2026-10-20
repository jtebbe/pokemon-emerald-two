#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("AI learns Blazing Glory from its item reveal and forecasts its mandatory HP loss")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_BLAZING_GLORY);
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_0), 20);
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

AI_SINGLE_BATTLE_TEST("Blazing Glory AI values safe Protect boosts but rejects waiting when its HP cost is lethal")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Item(ITEM_APEX_METEORITE); HP(120); MaxHP(120); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        s32 boosted = ProtectChecks(B_BATTLER_1, B_BATTLER_0, MOVE_PROTECT, MOVE_SCRATCH);
        enum Ability savedAbility = gAiLogicData->abilities[B_BATTLER_1];
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_NONE;
        s32 ordinary = ProtectChecks(B_BATTLER_1, B_BATTLER_0, MOVE_PROTECT, MOVE_SCRATCH);
        gAiLogicData->abilities[B_BATTLER_1] = savedAbility;
        EXPECT_GT(boosted, ordinary);
        u32 hp = opponent->hp;
        opponent->hp = 20;
        EXPECT(BattlerWillFaintFromSecondaryDamage(B_BATTLER_1, savedAbility));
        EXPECT_LT(ProtectChecks(B_BATTLER_1, B_BATTLER_0, MOVE_PROTECT, MOVE_SCRATCH), 0);
        EXPECT(!ShouldRaiseAnyStat(B_BATTLER_1, B_BATTLER_0));
        opponent->hp = hp;
    }
}
