#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Amped Bloom chains the matching type once in sun or rain without spending PP")
{
    enum Species species;
    enum Ability weather;
    enum Move primary, followUp;
    u32 followUpPP;
    PARAMETRIZE {
        species = SPECIES_VOLTORB_HISUI; weather = ABILITY_DROUGHT;
        primary = MOVE_ABSORB; followUp = MOVE_SHOCK_WAVE; followUpPP = 5;
    }
    PARAMETRIZE {
        species = SPECIES_ELECTRODE_HISUI; weather = ABILITY_DRIZZLE;
        primary = MOVE_SHOCK_WAVE; followUp = MOVE_ABSORB; followUpPP = 0;
    }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); MovesWithPP({primary, 5}, {followUp, followUpPP}); Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(weather); HP(500); MaxHP(500); Speed(50); }
    } WHEN {
        TURN { MOVE(player, primary); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, primary, player);
        ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, followUp, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
    } THEN {
        EXPECT_EQ(player->pp[0], 4);
        EXPECT_EQ(player->pp[1], followUpPP);
    }
}

SINGLE_BATTLE_TEST("Amped Bloom needs the Meteorite, active matching weather, and a move of the other type")
{
    enum Item item;
    enum Ability weather;
    enum Move followUp;
    PARAMETRIZE { item = ITEM_NONE;          weather = ABILITY_DROUGHT; followUp = MOVE_SHOCK_WAVE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; weather = ABILITY_NONE;    followUp = MOVE_SHOCK_WAVE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; weather = ABILITY_DRIZZLE; followUp = MOVE_SHOCK_WAVE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; weather = ABILITY_DROUGHT; followUp = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_VOLTORB_HISUI) { Item(item); Moves(MOVE_ABSORB, followUp); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(weather); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        NOT ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
    }
}

SINGLE_BATTLE_TEST("Amped Bloom activates only once after all hits of a Grass move")
{
    GIVEN {
        PLAYER(SPECIES_ELECTRODE_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BULLET_SEED, MOVE_CHARGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED, WITH_RNG(RNG_HITS, 2)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        NOT ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
    }
}

DOUBLE_BATTLE_TEST("Amped Bloom picks a known matching move and a random opponent after a status move")
{
    enum Move followUp;
    u32 choice;
    PARAMETRIZE { followUp = MOVE_SHOCK_WAVE; choice = 0; }
    PARAMETRIZE { followUp = MOVE_THUNDERBOLT; choice = 1; }
    PASSES_RANDOMLY(1, 2, RNG_RANDOM_TARGET);
    GIVEN {
        PLAYER(SPECIES_ELECTRODE_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_GRASSY_TERRAIN, MOVE_SHOCK_WAVE, MOVE_SWIFT, MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); HP(500); MaxHP(500); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_GRASSY_TERRAIN, WITH_RNG(RNG_AMPED_BLOOM_MOVE, choice)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_AMPED_BLOOM);
        ANIMATION(ANIM_TYPE_MOVE, followUp, playerLeft, target: opponentRight);
    }
}

SINGLE_BATTLE_TEST("Amped Bloom cannot follow a move stopped by flinching")
{
    GIVEN {
        PLAYER(SPECIES_VOLTORB_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_ABSORB, MOVE_SHOCK_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_ABSORB); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
            ABILITY_POPUP(player, ABILITY_AMPED_BLOOM);
        }
    }
}
