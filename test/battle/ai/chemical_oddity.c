#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

static EWRAM_DATA bool32 sChemicalSeen[PARTY_SIZE];
static EWRAM_DATA u8 sChemicalStages[PARTY_SIZE][NUM_BATTLE_STATS];
static EWRAM_DATA u32 sChemicalSpeed[PARTY_SIZE];

static void ObserveChemicalCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 3)
        return;
    sChemicalSeen[slot] = TRUE;
    memcpy(sChemicalStages[slot], gBattleMons[context->battler].statStages, sizeof(sChemicalStages[slot]));
    sChemicalSpeed[slot] = gAiLogicData->speedStats[context->battler];
}

AI_SINGLE_BATTLE_TEST("Chemical Oddity AI uses average entry boosts without leaking them into another candidate")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_GRIMER_ALOLA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
        OPPONENT(SPECIES_MUK_ALOLA) { Ability(ABILITY_POISON_TOUCH); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN {
        memset(sChemicalSeen, 0, sizeof(sChemicalSeen));
        gDynamicAiMonChoiceFunc = ObserveChemicalCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sChemicalSeen[1]); EXPECT(sChemicalSeen[2]);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        {
            EXPECT_EQ(sChemicalStages[1][stat], DEFAULT_STAT_STAGE + (stat < NUM_STATS));
            EXPECT_EQ(sChemicalStages[2][stat], DEFAULT_STAT_STAGE);
        }
        EXPECT_EQ(sChemicalSpeed[1], 150);
        EXPECT_EQ(sChemicalSpeed[2], 100);
    }
}

AI_DOUBLE_BATTLE_TEST("Chemical Oddity AI applies its estimate once for both opposing battlers")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(90); Moves(MOVE_QUICK_ATTACK); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); Speed(80); }
        OPPONENT(SPECIES_GRIMER_ALOLA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
        OPPONENT(SPECIES_MUK_ALOLA) { Ability(ABILITY_POISON_TOUCH); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_SCRATCH, MOVE_SWIFT); }
    } WHEN {
        memset(sChemicalSeen, 0, sizeof(sChemicalSeen));
        gDynamicAiMonChoiceFunc = ObserveChemicalCandidate;
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sChemicalSeen[2]); EXPECT(sChemicalSeen[3]);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        {
            EXPECT_EQ(sChemicalStages[2][stat], DEFAULT_STAT_STAGE + (stat < NUM_STATS));
            EXPECT_EQ(sChemicalStages[3][stat], DEFAULT_STAT_STAGE);
        }
        EXPECT_EQ(sChemicalSpeed[2], 150);
        EXPECT_EQ(sChemicalSpeed[3], 100);
    }
}

AI_SINGLE_BATTLE_TEST("Chemical Oddity AI values entry boosts only when a core stat can rise")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); }
    } WHEN { TURN {} } THEN {
        EXPECT_GT(BattlerBenefitsFromAbilityScore(B_BATTLER_1, ABILITY_CHEMICAL_ODDITY, gAiLogicData), 0);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            opponent->statStages[stat] = MAX_STAT_STAGE;
        EXPECT_EQ(BattlerBenefitsFromAbilityScore(B_BATTLER_1, ABILITY_CHEMICAL_ODDITY, gAiLogicData), 0);
    }
}
