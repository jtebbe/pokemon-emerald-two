#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_main.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Mephitic Dust AI predicts Poison move priority without changing speed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_ACID_SPRAY); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_ACID_SPRAY); } }
    THEN {
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_ACID_SPRAY, MOVE_SCRATCH, CONSIDER_PRIORITY));
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_ACID_SPRAY, MOVE_SCRATCH, DONT_CONSIDER_PRIORITY));
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SILVER_WIND, MOVE_SCRATCH, CONSIDER_PRIORITY));
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_MEPHITIC_DUST);
    }
}

AI_SINGLE_BATTLE_TEST("Mephitic Dust AI predicts both kinds of poison priority penalty")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Status1(status); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY), status != STATUS1_NONE);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_QUICK_ATTACK, CONSIDER_PRIORITY));
    }
}

AI_SINGLE_BATTLE_TEST("Mephitic Dust AI chooses Poison priority to KO before a faster opponent")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(100); Attack(4000); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Moves(MOVE_SLUDGE_BOMB, MOVE_SHADOW_BALL); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_SLUDGE_BOMB, MOVE_SHADOW_BALL); EXPECT_MOVE(opponent, MOVE_SLUDGE_BOMB); SEND_OUT(player, 1); }
    } THEN { EXPECT_EQ(opponent->hp, 1); }
}

AI_SINGLE_BATTLE_TEST("Mephitic Dust opposing aura makes AI use priority to KO before an otherwise slower opponent")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); HP(1); Speed(1); Attack(4000); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); Status1(STATUS1_POISON); Speed(100); Moves(MOVE_QUICK_ATTACK, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SCORE_GT(opponent, MOVE_QUICK_ATTACK, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
    } THEN { EXPECT_GT(opponent->hp, 0); }
}

AI_SINGLE_BATTLE_TEST("Mephitic Dust AI avoids Poison moves stopped by priority blockers")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PSYCHIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_DAZZLING; }
    PARAMETRIZE { ability = ABILITY_QUEENLY_MAJESTY; }
    PARAMETRIZE { ability = ABILITY_ARMOR_TAIL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Moves(MOVE_SLUDGE_BOMB, MOVE_SILVER_WIND); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_SILVER_WIND, MOVE_SLUDGE_BOMB); EXPECT_MOVE(opponent, MOVE_SILVER_WIND); } }
}

AI_DOUBLE_BATTLE_TEST("Mephitic Dust AI recognizes its partner's aura against either opposing slot")
{
    u32 holder;
    PARAMETRIZE { holder = 0; }
    PARAMETRIZE { holder = 1; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); HP(3000); MaxHP(3000); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_TOXIC_POISON); HP(3000); MaxHP(3000); Speed(99); }
        OPPONENT(SPECIES_VENOMOTH) { Item(holder == 0 ? ITEM_APEX_METEORITE : ITEM_NONE); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VENOMOTH) { Item(holder == 1 ? ITEM_APEX_METEORITE : ITEM_NONE); Speed(2); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_3, B_BATTLER_2, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY));
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_1, gAiLogicData->abilities[B_BATTLER_1], MOVE_POISON_JAB, B_BATTLER_0, gAiLogicData->abilities), holder == 0 ? 1 : 0);
    }
}

AI_SINGLE_BATTLE_TEST("Mephitic Dust AI's priority forecast uses known abilities instead of revealing hidden field abilities")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        enum Ability savedAbility = gAiLogicData->abilities[B_BATTLER_0];
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY));
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_NONE;
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY));
        gAiLogicData->abilities[B_BATTLER_0] = savedAbility;
    }
}

AI_DOUBLE_BATTLE_TEST("Mephitic Dust AI values poison setup from its own or its partner's aura")
{
    u32 holder;
    PARAMETRIZE { holder = 0; }
    PARAMETRIZE { holder = 1; }
    PARAMETRIZE { holder = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_VENOMOTH) { Item(holder == 1 ? ITEM_APEX_METEORITE : ITEM_NONE); Moves(MOVE_CELEBRATE, MOVE_TOXIC); }
        OPPONENT(SPECIES_VENOMOTH) { Item(holder == 2 ? ITEM_APEX_METEORITE : ITEM_NONE); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        s32 score = 0;
        playerLeft->status1 = STATUS1_NONE;
        IncreasePoisonScore(B_BATTLER_1, B_BATTLER_0, MOVE_TOXIC, &score);
        EXPECT_EQ(score, holder ? DECENT_EFFECT : WEAK_EFFECT);
    }
}
