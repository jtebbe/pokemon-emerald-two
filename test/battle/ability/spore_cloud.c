#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Spore Cloud is the Apex ability of Oddish, Gloom, and Vileplume only")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_ODDISH; }
    PARAMETRIZE { species = SPECIES_GLOOM; }
    PARAMETRIZE { species = SPECIES_VILEPLUME; }
    PARAMETRIZE { species = SPECIES_BELLOSSOM; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SPORE_CLOUD, species != SPECIES_BELLOSSOM);
        EXPECT_EQ(!!opponent->status1, species != SPECIES_BELLOSSOM);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud chooses each of three viable statuses with equal probability")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_SLEEP; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PASSES_RANDOMLY(1, 3, RNG_SPORE_CLOUD);
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        if (status == STATUS1_SLEEP) STATUS_ICON(opponent, sleep: TRUE);
        else if (status == STATUS1_TOXIC_POISON) STATUS_ICON(opponent, badPoison: TRUE);
        else STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud splits its roll equally between the two remaining viable statuses")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PASSES_RANDOMLY(1, 2, RNG_SPORE_CLOUD);
    GIVEN {
        PLAYER(SPECIES_GLOOM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INSOMNIA); }
    } WHEN {
        TURN {}
    } SCENE {
        if (status == STATUS1_SLEEP) STATUS_ICON(opponent, sleep: TRUE);
        else if (status == STATUS1_TOXIC_POISON) STATUS_ICON(opponent, badPoison: TRUE);
        else STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud always applies the sole viable status and uses the proper message")
{
    enum Species species;
    enum Ability ability;
    u32 status;
    PARAMETRIZE { species = SPECIES_MAGNEMITE; ability = ABILITY_STURDY; status = STATUS1_SLEEP; }
    PARAMETRIZE { species = SPECIES_PIKACHU; ability = ABILITY_INSOMNIA; status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { species = SPECIES_EKANS; ability = ABILITY_INSOMNIA; status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_VILEPLUME) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        if (status == STATUS1_SLEEP) {
            MESSAGE("The opposing Magnemite fell asleep!");
            STATUS_ICON(opponent, sleep: TRUE);
        } else if (status == STATUS1_TOXIC_POISON) {
            MESSAGE("The opposing Pikachu was badly poisoned!");
            STATUS_ICON(opponent, badPoison: TRUE);
        } else {
            MESSAGE("The opposing Ekans is paralyzed, so it may be unable to move!");
            STATUS_ICON(opponent, paralysis: TRUE);
        }
    } THEN {
        EXPECT(opponent->status1 & status);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud selects viable statuses separately for both foes and never affects its ally")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGNEMITE);
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INSOMNIA); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponentLeft, sleep: TRUE);
        STATUS_ICON(opponentRight, paralysis: TRUE);
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT(opponentRight->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud still affects a viable foe when the other foe is fully protected")
{
    GIVEN {
        PLAYER(SPECIES_GLOOM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PURIFYING_SALT); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INSOMNIA); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT(opponentRight->status1 & STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud does nothing if no status is viable")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { ability = ABILITY_COMATOSE; }
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud never replaces an existing status")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_TOXIC_POISON; }
    PARAMETRIZE { status = STATUS1_SLEEP; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_VILEPLUME) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT(opponent->status1 & status);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud respects entry terrain even when the terrain setter is slower")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MISTY_SURGE; }
    PARAMETRIZE { ability = ABILITY_ELECTRIC_SURGE; }
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_MAGNEMITE) { Ability(ability); Speed(1); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud can affect an airborne foe in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MISTY_SURGE); Item(ITEM_AIR_BALLOON); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud respects ally Sweet Veil and Pastel Veil")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SWEET_VEIL; }
    PARAMETRIZE { ability = ABILITY_PASTEL_VEIL; }
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_INSOMNIA); }
        OPPONENT(SPECIES_MAGNEMITE) { Ability(ability); }
    } WHEN {
        TURN {}
    } THEN {
        if (ability == ABILITY_PASTEL_VEIL) EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        else EXPECT(opponentLeft->status1 & STATUS1_TOXIC_POISON);
        if (ability == ABILITY_SWEET_VEIL) EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        else EXPECT(opponentRight->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud respects Safeguard")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SAFEGUARD); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAFEGUARD, opponent);
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud ignores Substitute and attack secondary-effect protection")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponent, paralysis: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
        EXPECT(opponent->volatiles.substitute);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud is blocked by Grass typing, Overcoat, and Safety Goggles")
{
    enum Ability ability;
    enum Item item;
    enum Species species;
    PARAMETRIZE { species = SPECIES_BULBASAUR; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_OVERCOAT; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_SAFETY_GOGGLES; }
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); }
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud can trigger again on re-entry but never triggers every turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_REFRESH, WITH_RNG(RNG_PARALYSIS, FALSE)); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponent, paralysis: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFRESH, opponent);
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponent, paralysis: TRUE);
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud cannot trigger if entry hazards knock out its holder")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); SEND_OUT(player, 2); }
    } SCENE {
        HP_BAR(player, hp: 0);
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud sleep activates Sleep Clause before selecting a status for the second opponent")
{
    GIVEN {
        FLAG_SET(B_FLAG_SLEEP_CLAUSE);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGNEMITE);
        OPPONENT(SPECIES_MAGNEMITE);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        EXPECT_EQ(GetSporeCloudEffects(B_BATTLER_0, B_BATTLER_3, ABILITY_STURDY), 0);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud status activates Synchronize and a curing berry normally")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CHERI_BERRY; }
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SYNCHRONIZE); Item(item); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponent, paralysis: TRUE);
        ABILITY_POPUP(opponent, ABILITY_SYNCHRONIZE);
        STATUS_ICON(player, paralysis: TRUE);
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ(!!(opponent->status1 & STATUS1_PARALYSIS), item == ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Spore Cloud does not reactivate merely because an opponent switches in")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
        STATUS_ICON(opponent, paralysis: TRUE);
        MESSAGE("2 sent out Wynaut!");
        NOT ABILITY_POPUP(player, ABILITY_SPORE_CLOUD);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud rechecks Sleep Clause after a curing berry frees the first opponent")
{
    GIVEN {
        FLAG_SET(B_FLAG_SLEEP_CLAUSE);
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGNEMITE) { Item(ITEM_CHESTO_BERRY); }
        OPPONENT(SPECIES_MAGNEMITE);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT(opponentRight->status1 & STATUS1_SLEEP);
    }
}

DOUBLE_BATTLE_TEST("Spore Cloud continues to the correct second opponent after Synchronize reflects the first status")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SYNCHRONIZE); }
        OPPONENT(SPECIES_MAGNEMITE);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(playerLeft->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT(opponentLeft->status1 & STATUS1_PARALYSIS);
        EXPECT(opponentRight->status1 & STATUS1_SLEEP);
    }
}
