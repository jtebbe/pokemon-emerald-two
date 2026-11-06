#include "global.h"
#include "test/battle.h"
#include "battle_main.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Succulent Trap AI uses known abilities for priority and distinguishes raw Speed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_SUCCULENT_TRAP);
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0, gAiLogicData->abilities), -2);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, MOVE_CELEBRATE, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, MOVE_CELEBRATE, DONT_CONSIDER_PRIORITY));
        enum Ability saved = gAiLogicData->abilities[B_BATTLER_0];
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_NONE;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0, gAiLogicData->abilities), 0);
        gAiLogicData->abilities[B_BATTLER_0] = saved;
    }
}

AI_DOUBLE_BATTLE_TEST("Succulent Trap AI applies priority only when an attack targets the holder")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_EXTREME_SPEED, B_BATTLER_0, gAiLogicData->abilities), 0);
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_EXTREME_SPEED, B_BATTLER_2, gAiLogicData->abilities), 2);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_EXTREME_SPEED, MOVE_QUICK_ATTACK, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_2, MOVE_EXTREME_SPEED, MOVE_QUICK_ATTACK, CONSIDER_PRIORITY));
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_GROWL, B_BATTLER_0, gAiLogicData->abilities), 0);
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_EARTHQUAKE, B_BATTLER_2, gAiLogicData->abilities), -2);
    }
}

static bool32 sTrapObserved[PARTY_SIZE];
static u32 sTrapEvasion[PARTY_SIZE], sTrapOwnEvasion[PARTY_SIZE], sTrapStatus[PARTY_SIZE], sTrapSpeed[PARTY_SIZE];
static u32 sTrapAttack[PARTY_SIZE], sTrapSpAttack[PARTY_SIZE];
static s32 sTrapPriority[PARTY_SIZE];
static struct SimulatedDamage sTrapDamage[PARTY_SIZE];
static void ObserveTrapCandidate(struct SwitchAiContext *context)
{
    if (context->partyMonIndex == 0 || context->partyMonIndex > 2 || gBattlerPartyIndexes[context->battler] != 0)
        return;
    u32 slot = context->partyMonIndex;
    enum BattlerId foe = context->opposingBattler;
    sTrapObserved[slot] = TRUE;
    sTrapEvasion[slot] = gBattleMons[foe].statStages[STAT_EVASION];
    sTrapOwnEvasion[slot] = gBattleMons[context->battler].statStages[STAT_EVASION];
    sTrapStatus[slot] = gBattleMons[foe].status1;
    sTrapSpeed[slot] = gAiLogicData->speedStats[foe];
    sTrapAttack[slot] = gBattleMons[foe].statStages[STAT_ATK];
    sTrapSpAttack[slot] = gBattleMons[foe].statStages[STAT_SPATK];
    sTrapPriority[slot] = AI_GetMovePriorityWithAbilities(foe, gAiLogicData->abilities[foe], MOVE_QUICK_ATTACK, context->battler, gAiLogicData->abilities);
    sTrapDamage[slot] = gAiLogicData->simulatedDmg[foe][context->battler][MOVESLOT_0];
}

AI_SINGLE_BATTLE_TEST("Succulent Trap AI forecasts entry evasion drops stat protection and stat reactions")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_COMPETITIVE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_CLEAR_AMULET; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_WHITE_HERB; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(1000); MaxHP(1000); Attack(100); Speed(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(2); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sTrapObserved[1] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveTrapCandidate;
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTrapObserved[1]);
        EXPECT_EQ(sTrapEvasion[1], ability == ABILITY_CONTRARY ? DEFAULT_STAT_STAGE + 1
                  : ability == ABILITY_SIMPLE ? DEFAULT_STAT_STAGE - 2
                  : ability == ABILITY_MIRROR_ARMOR || ability == ABILITY_CLEAR_BODY || item == ITEM_CLEAR_AMULET || item == ITEM_WHITE_HERB ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(sTrapOwnEvasion[1], ability == ABILITY_MIRROR_ARMOR ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(sTrapStatus[1], STATUS1_NONE);
        EXPECT_EQ(sTrapAttack[1], ability == ABILITY_DEFIANT ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(sTrapSpAttack[1], ability == ABILITY_COMPETITIVE ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("Succulent Trap AI isolates entry poison evasion Speed and Guts between switch candidates")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_QUICK_FEET; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MACHAMP; ability = ABILITY_GUTS; item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); Item(item); HP(1000); MaxHP(1000); Attack(100); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(200); Moves(MOVE_DEFOG); }
        OPPONENT(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Defense(100); Speed(1); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_VICTREEBEL) { Ability(ABILITY_CHLOROPHYLL); HP(1000); MaxHP(1000); Defense(100); Speed(2); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sTrapObserved[1] = sTrapObserved[2] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveTrapCandidate;
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_DEFOG); }
        TURN { MOVE(player, MOVE_QUICK_ATTACK); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTrapObserved[1]);
        EXPECT(sTrapObserved[2]);
        EXPECT_EQ(sTrapEvasion[1], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(sTrapEvasion[2], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(sTrapStatus[1], STATUS1_POISON);
        EXPECT_EQ(sTrapStatus[2], STATUS1_NONE);
        EXPECT_EQ(sTrapSpeed[1], species == SPECIES_WOBBUFFET ? 150 : 100);
        EXPECT_EQ(sTrapSpeed[2], 100);
        EXPECT_EQ(sTrapPriority[1], -1);
        EXPECT_EQ(sTrapPriority[2], 1);
        if (species == SPECIES_MACHAMP) EXPECT_GT(sTrapDamage[1].median, sTrapDamage[2].median);
    }
}

AI_SINGLE_BATTLE_TEST("Succulent Trap AI does not predict the poison priority bonus through immunity or curing berries")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_IMMUNITY; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MAGNEMITE; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_PECHA_BERRY; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_LUM_BERRY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); Item(item); HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PONYTA) { HP(1); Level(1); Speed(200); Moves(MOVE_DEFOG); }
        OPPONENT(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_SONIC_BOOM); }
    } WHEN {
        sTrapObserved[1] = FALSE;
        gDynamicAiMonChoiceFunc = ObserveTrapCandidate;
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_DEFOG); }
        TURN { MOVE(player, MOVE_QUICK_ATTACK); EXPECT_SEND_OUT(opponent, 1); }
    } THEN {
        gDynamicAiMonChoiceFunc = NULL;
        EXPECT(sTrapObserved[1]);
        EXPECT_EQ(sTrapEvasion[1], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(sTrapStatus[1], STATUS1_NONE);
        EXPECT_EQ(sTrapPriority[1], 0);
    }
}

AI_DOUBLE_BATTLE_TEST("Succulent Trap AI values poison setup only when it improves its own or its partner's trap")
{
    u32 holder;
    bool32 equal;
    PARAMETRIZE { holder = 0; equal = FALSE; }
    PARAMETRIZE { holder = 1; equal = FALSE; }
    PARAMETRIZE { holder = 2; equal = FALSE; }
    PARAMETRIZE { holder = 1; equal = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_VICTREEBEL) { Item(holder == 1 ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE, MOVE_TOXIC); }
        OPPONENT(SPECIES_VICTREEBEL) { Item(holder == 2 ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        s32 score = 0;
        playerLeft->status1 = STATUS1_NONE;
        if (equal) playerLeft->statStages[STAT_EVASION] = DEFAULT_STAT_STAGE;
        IncreasePoisonScore(B_BATTLER_1, B_BATTLER_0, MOVE_TOXIC, &score);
        EXPECT_EQ(score, holder && !equal ? DECENT_EFFECT : WEAK_EFFECT);
    }
}

AI_DOUBLE_BATTLE_TEST("Succulent Trap AI sees priority attacks bypass Dazzling when the holder reduces them to zero priority")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DAZZLING); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft);
    } THEN {
        EXPECT(!Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_QUICK_ATTACK, gAiLogicData));
        EXPECT(Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_2, MOVE_QUICK_ATTACK, gAiLogicData));
    }
}

AI_DOUBLE_BATTLE_TEST("Succulent Trap AI uses known redirection abilities and their exceptions for priority")
{
    enum Ability redirectAbility;
    enum Move move;
    PARAMETRIZE { redirectAbility = ABILITY_MUSHROOM_MADNESS; move = MOVE_TACKLE; }
    PARAMETRIZE { redirectAbility = ABILITY_STORM_DRAIN; move = MOVE_WATER_GUN; }
    PARAMETRIZE { redirectAbility = ABILITY_LIGHTNING_ROD; move = MOVE_THUNDER_SHOCK; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(redirectAbility); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, move, B_BATTLER_0, gAiLogicData->abilities), 0);
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_STALWART;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_STALWART, move, B_BATTLER_0, gAiLogicData->abilities), -1);
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_NONE;
        gAiLogicData->abilities[B_BATTLER_2] = ABILITY_NONE;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, move, B_BATTLER_0, gAiLogicData->abilities), -1);
    }
}

AI_DOUBLE_BATTLE_TEST("Succulent Trap AI follows active Follow Me and respects powder immunity")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VICTREEBEL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        gSideTimers[B_SIDE_PLAYER].followmeTimer = 1;
        gSideTimers[B_SIDE_PLAYER].followmeTarget = B_BATTLER_0;
        gSideTimers[B_SIDE_PLAYER].followmePowder = FALSE;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_2, gAiLogicData->abilities), -1);
        gSideTimers[B_SIDE_PLAYER].followmeTarget = B_BATTLER_2;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_NONE, MOVE_TACKLE, B_BATTLER_0, gAiLogicData->abilities), 0);
        gSideTimers[B_SIDE_PLAYER].followmeTarget = B_BATTLER_0;
        gSideTimers[B_SIDE_PLAYER].followmePowder = TRUE;
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_OVERCOAT;
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, ABILITY_OVERCOAT, MOVE_TACKLE, B_BATTLER_2, gAiLogicData->abilities), 0);
    }
}
