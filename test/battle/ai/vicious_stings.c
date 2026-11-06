#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

AI_SINGLE_BATTLE_TEST("Vicious Stings AI predicts Dark STAB and Merciless damage while respecting crit protection")
{
    enum Species species;
    enum Ability ability;
    u32 status;
    s16 damage;
    PARAMETRIZE { species = SPECIES_SNORLAX; ability = ABILITY_THICK_FAT; status = 0; }
    PARAMETRIZE { species = SPECIES_SNORLAX; ability = ABILITY_THICK_FAT; status = STATUS1_POISON; }
    PARAMETRIZE { species = SPECIES_KINGLER; ability = ABILITY_SHELL_ARMOR; status = STATUS1_POISON; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE, MOVE_STRENGTH); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_BITE, MOVE_STRENGTH); EXPECT_MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        if (status && ability != ABILITY_SHELL_ARMOR)
            MESSAGE("A critical hit!");
        else
            NOT MESSAGE("A critical hit!");
    } THEN {
        struct SimulatedDamage predicted = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][MOVESLOT_0];
        EXPECT_GE(damage, predicted.minimum);
        EXPECT_LE(damage, predicted.maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Vicious Stings AI knows Dark ordering without treating it as an increased priority level")
{
    enum Move enemyMove;
    PARAMETRIZE { enemyMove = MOVE_TACKLE; }
    PARAMETRIZE { enemyMove = MOVE_QUICK_ATTACK; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SNORLAX) { Speed(100); HP(3000); MaxHP(3000); Moves(enemyMove); }
        OPPONENT(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, enemyMove); EXPECT_MOVE(opponent, MOVE_BITE); }
    } THEN {
        EXPECT_EQ(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_BITE, enemyMove, CONSIDER_PRIORITY), enemyMove == MOVE_TACKLE);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_BITE, enemyMove, DONT_CONSIDER_PRIORITY));
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_1), ABILITY_VICIOUS_STINGS);
    }
}

AI_SINGLE_BATTLE_TEST("Vicious Stings AI recognizes an opposing Apex holder's Dark ordering without omniscience")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_BEEDRILL) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_SNORLAX) { Speed(100); HP(3000); MaxHP(3000); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_VICIOUS_STINGS);
        EXPECT(!AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE, MOVE_BITE, CONSIDER_PRIORITY));
        EXPECT(AI_IsFaster(B_BATTLER_1, B_BATTLER_0, MOVE_QUICK_ATTACK, MOVE_BITE, CONSIDER_PRIORITY));
    }
}

AI_SINGLE_BATTLE_TEST("Vicious Stings AI rewards poisoning to enable Merciless")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY);
        PLAYER(SPECIES_SNORLAX) { Ability(ABILITY_THICK_FAT); HP(3000); MaxHP(3000); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BEEDRILL) { Ability(ABILITY_SWARM); Item(item); Moves(MOVE_TOXIC, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        // Evaluate against the same healthy target, independently of the selected move.
        s32 score = 0;
        player->status1 = 0;
        IncreasePoisonScore(B_BATTLER_1, B_BATTLER_0, MOVE_TOXIC, &score);
        EXPECT_EQ(score, item == ITEM_APEX_METEORITE ? DECENT_EFFECT : WEAK_EFFECT);
    }
}
