#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Hypnotic Pattern belongs to Poliwag, Poliwhirl, and Poliwrath, but not Politoed")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_POLIWAG; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_POLIWHIRL; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_POLIWRATH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_POLITOED; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_POLIWAG; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_HYPNOTIC_PATTERN,
                  species != SPECIES_POLITOED && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern entry sleep prevents exactly one action, then the foe wakes")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_3; }
    PARAMETRIZE { generation = GEN_5; }
    PARAMETRIZE { generation = GEN_9; }
    PARAMETRIZE { generation = GEN_CHAMPIONS; }
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, generation);
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
        MESSAGE("The opposing Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        MESSAGE("The opposing Wobbuffet is fast asleep.");
        MESSAGE("The opposing Wobbuffet woke up!");
        STATUS_ICON(opponent, sleep: FALSE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Hypnotic Pattern sleeps both eligible opponents without affecting its partner")
{
    GIVEN {
        PLAYER(SPECIES_POLIWHIRL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_HYPNOTIC_PATTERN);
        STATUS_ICON(opponentLeft, sleep: TRUE);
        STATUS_ICON(opponentRight, sleep: TRUE);
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_SLEEP_TURN(1));
        EXPECT_EQ(opponentRight->status1, STATUS1_SLEEP_TURN(1));
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern respects sleep and status immunity abilities")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_INSOMNIA; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; }
    PARAMETRIZE { ability = ABILITY_COMATOSE; }
    PARAMETRIZE { ability = ABILITY_PURIFYING_SALT; }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Hypnotic Pattern respects allied Sweet Veil")
{
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SLURPUFF) { Ability(ABILITY_SWEET_VEIL); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(playerLeft, ABILITY_HYPNOTIC_PATTERN);
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Hypnotic Pattern still sleeps the other foe when one foe is immune")
{
    GIVEN {
        PLAYER(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INSOMNIA); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_SLEEP_TURN(1));
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern respects entry Electric and Misty Terrain even from slower setters")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_ELECTRIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_MISTY_SURGE; }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(1); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern can sleep airborne foes over sleep-blocking terrain")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_ELECTRIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_MISTY_SURGE; }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(ITEM_AIR_BALLOON); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_SLEEP_TURN(1));
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern respects Safeguard but bypasses Substitute")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_SUBSTITUTE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, protection); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, protection == MOVE_SUBSTITUTE ? STATUS1_SLEEP_TURN(1) : STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern neither replaces status nor refreshes existing sleep")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(4); }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
    } THEN {
        EXPECT_EQ(opponent->status1, status & STATUS1_SLEEP ? STATUS1_SLEEP_TURN(3) : status);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern is not powder and bypasses Grass Overcoat Safety Goggles and secondary protection")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { species = SPECIES_BULBASAUR; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_SLEEP_TURN(1));
    }
}

DOUBLE_BATTLE_TEST("Hypnotic Pattern respects Sleep Clause separately for each opponent")
{
    bool32 clause;
    PARAMETRIZE { clause = FALSE; }
    PARAMETRIZE { clause = TRUE; }
    GIVEN {
        if (clause) FLAG_SET(B_FLAG_SLEEP_CLAUSE);
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_SLEEP_TURN(1));
        EXPECT_EQ(opponentRight->status1, clause ? STATUS1_NONE : STATUS1_SLEEP_TURN(1));
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern sleep can be cured immediately by Chesto or Lum Berry")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_CHESTO_BERRY; }
    PARAMETRIZE { item = ITEM_LUM_BERRY; }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, sleep: TRUE);
        STATUS_ICON(opponent, sleep: FALSE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern shortest sleep respects Early Bird")
{
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_EARLY_BIRD); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, sleep: TRUE);
        MESSAGE("The opposing Wobbuffet woke up!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern late pivot entry preserves sleep for the foe's next action")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
        STATUS_ICON(opponent, sleep: TRUE);
        MESSAGE("The opposing Wobbuffet is fast asleep.");
        MESSAGE("The opposing Wobbuffet woke up!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern can activate again after switching out and returning")
{
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
        STATUS_ICON(opponent, sleep: TRUE);
        STATUS_ICON(opponent, sleep: FALSE);
        ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_SLEEP_TURN(1));
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern doubles physical and special damage against sleep and Comatose", s16 damage)
{
    enum Move move;
    u32 status;
    enum Ability ability;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; status = STATUS1_BURN; ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_SLEEP_TURN(2); ability = ABILITY_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_NONE; ability = ABILITY_COMATOSE; }
    }
    GIVEN {
        PLAYER(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
            EXPECT_MUL_EQ(results[j * 3].damage, UQ_4_12(2.0), results[j * 3 + 1].damage);
            EXPECT_EQ(results[j * 3 + 1].damage, results[j * 3 + 2].damage);
        }
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern boosts the entry hit before sleep ends but not hits after waking", s16 asleep, s16 awake)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
        TURN {}
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].asleep);
        MESSAGE("The opposing Wobbuffet woke up!");
        HP_BAR(opponent, captureDamage: &results[i].awake);
    } THEN {
        EXPECT_MUL_EQ(results[i].awake, UQ_4_12(2.0), results[i].asleep);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern leaves fixed damage unchanged")
{
    GIVEN {
        PLAYER(SPECIES_POLIWRATH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern does not prevent sleeping foes using Snore")
{
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SNORE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNORE, opponent);
        MESSAGE("The opposing Wobbuffet woke up!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern cannot put foes to sleep during Uproar")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_UPROAR); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Hypnotic Pattern does not shorten sleep inflicted by the user's ordinary moves")
{
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Speed(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFRESH); }
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_SLEEP_TURNS, 4)); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HYPNOTIC_PATTERN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFRESH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_SLEEP_TURN(3));
    }
}

DOUBLE_BATTLE_TEST("Hypnotic Pattern does not double damage dealt to a sleeping ally", s16 damage)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_SLEEP_TURN(2); }
    GIVEN {
        PLAYER(SPECIES_POLIWAG) { Item(ITEM_APEX_METEORITE); Speed(200); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INSOMNIA); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INSOMNIA); Speed(99); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WATER_GUN, target: playerRight, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
