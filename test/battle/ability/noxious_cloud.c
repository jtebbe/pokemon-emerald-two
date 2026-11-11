#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Noxious Cloud badly poisons contact attackers and upgrades regular poison")
{
    enum Item item;
    enum Move move;
    u32 startStatus;
    bool32 badlyPoisoned;
    PARAMETRIZE { item = ITEM_NONE;            move = MOVE_SCRATCH; startStatus = 0;             badlyPoisoned = TRUE; }
    PARAMETRIZE { item = ITEM_NONE;            move = MOVE_SCRATCH; startStatus = STATUS1_POISON; badlyPoisoned = TRUE; }
    PARAMETRIZE { item = ITEM_PROTECTIVE_PADS; move = MOVE_SCRATCH; startStatus = 0;             badlyPoisoned = FALSE; }
    PARAMETRIZE { item = ITEM_NONE;            move = MOVE_SWIFT;   startStatus = 0;             badlyPoisoned = FALSE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Status1(startStatus); }
        OPPONENT(SPECIES_WEEZING) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(player->status1 & STATUS1_ANY, badlyPoisoned ? STATUS1_TOXIC_POISON : STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Noxious Cloud immediately advances bad poison and leaves the next tick increased")
{
    s16 first, contact, endTurn;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_TOXIC_POISON); HP(160); MaxHP(160); }
        OPPONENT(SPECIES_KOFFING) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {}
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &first);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_NOXIOUS_CLOUD);
        HP_BAR(player, captureDamage: &contact);
        HP_BAR(player, captureDamage: &endTurn);
    } THEN {
        EXPECT_EQ(first, 10);
        EXPECT_EQ(contact, 20);
        EXPECT_EQ(endTurn, 30);
        EXPECT_EQ(player->status1 & STATUS1_TOXIC_COUNTER, STATUS1_TOXIC_TURN(3));
    }
}

SINGLE_BATTLE_TEST("Noxious Cloud grants Levitate and stops opposing Berries")
{
    GIVEN {
        PLAYER(SPECIES_KOFFING) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SITRUS_BERRY); HP(40); MaxHP(160); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(opponent->hp, 40);
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
    }
}
