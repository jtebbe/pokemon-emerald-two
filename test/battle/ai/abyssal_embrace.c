#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Abyssal Embrace AI recognizes trapping and residual damage with escape and damage immunities")
{
    enum Species species;
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_GASTLY; item = ITEM_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_SHED_SHELL; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; ability = ABILITY_RUN_AWAY; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; ability = ABILITY_MAGIC_GUARD; }
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        WITH_CONFIG(B_RUN_AWAY, GEN_CHAMPIONS);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); HP(256); MaxHP(256); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(IsBattlerTrapped(B_BATTLER_0, B_BATTLER_1), species != SPECIES_GASTLY && item != ITEM_SHED_SHELL && ability != ABILITY_RUN_AWAY);
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), ability == ABILITY_MAGIC_GUARD ? 0 : 16);
        EXPECT(IsBattlerDamagedByStatus(B_BATTLER_1));
    }
}

AI_SINGLE_BATTLE_TEST("Abyssal Embrace AI predicts the exact half HP boundary and one damage minimum")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        opponent->hp = 128;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 16);
        opponent->hp = 127;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 32);
        opponent->maxHP = opponent->hp = 15;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 1);
        opponent->volatiles.abyssalEmbrace = 0;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 0);
    }
}

AI_SINGLE_BATTLE_TEST("Abyssal Embrace AI can escape with pivot moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(256); MaxHP(256); Moves(move); }
        OPPONENT(SPECIES_WYNAUT) { HP(256); MaxHP(256); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(opponent->hp, 256);
    }
}

static bool32 sEmbraceObserved[PARTY_SIZE];
static u32 sEmbraceTargets[PARTY_SIZE], sEmbraceOtherTargets[PARTY_SIZE], sEmbraceDamage[PARTY_SIZE];
static bool32 sEmbraceTrapped[PARTY_SIZE];
static void ObserveEmbraceCandidate(struct SwitchAiContext *context)
{
    if (gBattlerPartyIndexes[context->battler] != 0)
        return;
    enum PartySlot slot = context->partyMonIndex;
    enum BattlerId target = GetOppositeBattler(context->battler);
    sEmbraceObserved[slot] = TRUE;
    sEmbraceTargets[slot] = gBattleMons[target].volatiles.abyssalEmbrace;
    sEmbraceOtherTargets[slot] = gBattleMons[GetPartnerBattler(target)].volatiles.abyssalEmbrace;
    sEmbraceDamage[slot] = GetBattlerSecondaryDamage(target);
    sEmbraceTrapped[slot] = IsBattlerTrapped(context->battler, target);
}

AI_SINGLE_BATTLE_TEST("Abyssal Embrace AI forecasts entry damage and isolates switch candidates")
{
    bool32 magicGuard;
    PARAMETRIZE { magicGuard = FALSE; }
    PARAMETRIZE { magicGuard = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(magicGuard ? ABILITY_MAGIC_GUARD : ABILITY_NONE); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_QUICK_ATTACK); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TENTACRUEL) { Speed(50); Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_TENTACRUEL) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sEmbraceObserved[1] = sEmbraceObserved[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveEmbraceCandidate;
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sEmbraceObserved[1]);
        EXPECT(sEmbraceObserved[2]);
        EXPECT_EQ(sEmbraceTargets[1], B_BATTLER_1 + 1);
        EXPECT_EQ(sEmbraceTargets[2], 0);
        EXPECT_EQ(sEmbraceDamage[1], magicGuard ? 0 : 62);
        EXPECT_EQ(sEmbraceDamage[2], 0);
        EXPECT(sEmbraceTrapped[1]);
        EXPECT(!sEmbraceTrapped[2]);
    }
}

AI_DOUBLE_BATTLE_TEST("Abyssal Embrace AI forecasts only the opposite foe in doubles")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_QUICK_ATTACK); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_PONYTA) { Level(1); HP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TENTACRUEL) { Speed(50); Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_TENTACRUEL) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sEmbraceObserved[2] = sEmbraceObserved[3] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveEmbraceCandidate;
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sEmbraceObserved[2]);
        EXPECT(sEmbraceObserved[3]);
        EXPECT_EQ(sEmbraceTargets[2], B_BATTLER_1 + 1);
        EXPECT_EQ(sEmbraceOtherTargets[2], 0);
        EXPECT_EQ(sEmbraceTargets[3], 0);
        EXPECT_EQ(sEmbraceOtherTargets[3], 0);
        EXPECT_EQ(sEmbraceDamage[2], 62);
        EXPECT_EQ(sEmbraceDamage[3], 0);
    }
}

AI_SINGLE_BATTLE_TEST("Abyssal Embrace AI cannot hard switch to avoid otherwise lethal residual damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TENTACRUEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(17); MaxHP(256); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_RAICHU) { HP(1000); MaxHP(1000); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_SONIC_BOOM); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->hp, 980);
        EXPECT_EQ(opponent->species, SPECIES_RAICHU);
        EXPECT_EQ((u32)opponent->volatiles.abyssalEmbrace, 0);
        EXPECT_EQ(opponent->hp, 1000);
    }
}
