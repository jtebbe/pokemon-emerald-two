#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Fused Necrozma can Mega Evolve holding Necrozmite")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; }
    GIVEN {
        PLAYER(species) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("Necrozma's Necrozmite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
        MESSAGE("Necrozma has Mega Evolved into Mega Necrozma!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_NECROZMA_ULTRA);
    }
}

DOUBLE_BATTLE_TEST("Mega Evolution's order is determined by Speed - opponent faster")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_NECROZMA_DAWN_WINGS) { Item(ITEM_NECROZMITE); Speed(3); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(4); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(playerLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("The opposing Necrozma's Necrozmite is reacting to 2's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponentLeft);
        MESSAGE("The opposing Necrozma has Mega Evolved into Mega Necrozma!");
        MESSAGE("Necrozma's Necrozmite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, playerLeft);
        MESSAGE("Necrozma has Mega Evolved into Mega Necrozma!");
    }
}

DOUBLE_BATTLE_TEST("Mega Evolution's order is determined by Speed - player faster")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); Speed(5); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_NECROZMA_DAWN_WINGS) { Item(ITEM_NECROZMITE); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(4); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(playerLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("Necrozma's Necrozmite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, playerLeft);
        MESSAGE("Necrozma has Mega Evolved into Mega Necrozma!");
        MESSAGE("The opposing Necrozma's Necrozmite is reacting to 2's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponentLeft);
        MESSAGE("The opposing Necrozma has Mega Evolved into Mega Necrozma!");
    }
}

SINGLE_BATTLE_TEST("Mega Evolution affects turn order")
{
    GIVEN {
        WITH_CONFIG(B_MEGA_EVO_TURN_ORDER, GEN_7);
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("Necrozma used Celebrate!");
        MESSAGE("The opposing Wobbuffet used Celebrate!");
    } THEN {
        EXPECT_EQ(player->speed, 294); // 2 * 129 base Speed + 31 + 5, at level 100.
    }
}

DOUBLE_BATTLE_TEST("Mega Evolution happens after switching, but before Focus Punch-like Moves")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FOCUS_PUNCH) == EFFECT_FOCUS_PUNCH);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(opponentRight, 2); MOVE(playerRight, MOVE_FOCUS_PUNCH, gimmick: GIMMICK_MEGA, target: opponentLeft); MOVE(playerLeft, MOVE_FOCUS_PUNCH, target: opponentLeft); }
        TURN {}
    } SCENE {
        MESSAGE("2 withdrew Wobbuffet!");
        MESSAGE("2 sent out Wobbuffet!");

        MESSAGE("Necrozma's Necrozmite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, playerRight);
        MESSAGE("Necrozma has Mega Evolved into Mega Necrozma!");

        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FOCUS_PUNCH_SETUP, playerRight);
        MESSAGE("Necrozma is tightening its focus!");

        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FOCUS_PUNCH_SETUP, playerLeft);
        MESSAGE("Wobbuffet is tightening its focus!");
    }
}

SINGLE_BATTLE_TEST("Necrozma and an opponent can both Mega Evolve on the same turn")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); Speed(3); }
        OPPONENT(SPECIES_GARDEVOIR) { Item(ITEM_GARDEVOIRITE); Speed(2); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("Necrozma's Necrozmite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
        MESSAGE("Necrozma has Mega Evolved into Mega Necrozma!");

        MESSAGE("The opposing Gardevoir's Gardevoirite is reacting to 2's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The opposing Gardevoir has Mega Evolved into Mega Gardevoir!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_NECROZMA_ULTRA);
        EXPECT_EQ(opponent->species, SPECIES_GARDEVOIR_MEGA);
    }
}

SINGLE_BATTLE_TEST("Necrozmite only enables Mega Evolution for fused Necrozma")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_NECROZMA; item = ITEM_NECROZMITE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; item = ITEM_AMPHAROSITE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; item = ITEM_AMPHAROSITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->species, species);
        EXPECT_EQ(CanMegaEvolve(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), FALSE);
    }
}

SINGLE_BATTLE_TEST("Necrozma uses the trainer's single Mega Evolution")
{
    GIVEN {
        PLAYER(SPECIES_NECROZMA_DUSK_MANE) { Item(ITEM_NECROZMITE); }
        PLAYER(SPECIES_NECROZMA_DAWN_WINGS) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_NECROZMA_DAWN_WINGS);
        EXPECT_EQ(CanMegaEvolve(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), FALSE);
    }
}

SINGLE_BATTLE_TEST("Necrozma returns its proper Form upon battle end after Mega Evolving")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; }
    GIVEN {
        PLAYER(species) { Item(ITEM_NECROZMITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_NECROZMA_ULTRA);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), species);
    }
}

SINGLE_BATTLE_TEST("Necrozma follows Mega Evolution fainting rules")
{
    enum Species species;
    u32 generation;
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; generation = GEN_9; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; generation = GEN_9; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DUSK_MANE; generation = GEN_CHAMPIONS; }
    PARAMETRIZE { species = SPECIES_NECROZMA_DAWN_WINGS; generation = GEN_CHAMPIONS; }
    GIVEN {
        WITH_CONFIG(B_FAINTING_KEEPS_FORM, generation);
        PLAYER(species) { HP(1); Item(ITEM_NECROZMITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA);
            MOVE(opponent, MOVE_SCRATCH);
            SEND_OUT(player, 1);
        }
        TURN { USE_ITEM(player, ITEM_REVIVE, 0); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ(player->species, generation >= GEN_CHAMPIONS ? SPECIES_NECROZMA_ULTRA : species);
    }
}

