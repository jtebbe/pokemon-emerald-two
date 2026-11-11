#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"

AI_SINGLE_BATTLE_TEST("Evil Trickster AI recognizes status priority and Ground immunity with grounding overrides")
{
    bool32 gravity = FALSE;
    PARAMETRIZE { gravity = FALSE; }
    PARAMETRIZE { gravity = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_EARTH_POWER, MOVE_WATER_GUN, MOVE_GRAVITY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, gravity ? MOVE_GRAVITY : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EARTH_POWER); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_EVIL_TRICKSTER);
        EXPECT_EQ(AI_IsBattlerGrounded(B_BATTLER_1), gravity);
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_EVIL_TRICKSTER, MOVE_GROWL, B_BATTLER_0, gAiLogicData->abilities), 1);
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_EVIL_TRICKSTER, MOVE_LICK, B_BATTLER_0, gAiLogicData->abilities), 0);
        if (gravity) EXPECT_GT(gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][MOVESLOT_0].median, 0);
        else EXPECT_EQ(gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][MOVESLOT_0].median, 0);
        EXPECT_GT(gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][MOVESLOT_1].median, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Evil Trickster AI avoids feeding a status move into Dark-type Prankster immunity")
{
    GIVEN {
        WITH_CONFIG(B_PRANKSTER_DARK_TYPES, GEN_7);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_UMBREON) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CONFUSE_RAY, MOVE_LICK); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_LICK); } }
}

AI_SINGLE_BATTLE_TEST("AI chooses an attack that can hit Evil Trickster instead of a Ground move")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTH_POWER, MOVE_WATER_GUN); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); } }
}

static EWRAM_DATA bool32 sTricksterSeen[PARTY_SIZE];
static EWRAM_DATA u32 sTricksterDamage[PARTY_SIZE];

static void ObserveTricksterCandidate(struct SwitchAiContext *context)
{
    u32 slot = context->partyMonIndex;
    if (gBattlerPartyIndexes[context->battler] != 0 || slot == 0 || slot > 2)
        return;
    sTricksterSeen[slot] = TRUE;
    sTricksterDamage[slot] = gAiLogicData->simulatedDmg[context->opposingBattler][context->battler][MOVESLOT_0].median;
}

AI_SINGLE_BATTLE_TEST("Evil Trickster AI predicts an Apex switch-in's Ground immunity without leaking it to ordinary Gengar")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(400); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_EARTH_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GENGAR) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_LICK); }
        OPPONENT(SPECIES_GENGAR) { Ability(ABILITY_CURSED_BODY); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_LICK); }
    } WHEN {
        memset(sTricksterSeen, 0, sizeof(sTricksterSeen));
        gDynamicAiMonChoiceFunc = ObserveTricksterCandidate;
        TURN { MOVE(player, MOVE_EARTH_POWER); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTricksterSeen[1]); EXPECT(sTricksterSeen[2]);
        EXPECT_EQ(sTricksterDamage[1], 0);
        EXPECT_GT(sTricksterDamage[2], 0);
    }
}
