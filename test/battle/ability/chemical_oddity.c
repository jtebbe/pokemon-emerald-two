#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

static s32 TotalChemicalBoosts(const struct BattlePokemon *mon)
{
    s32 total = 0;
    for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
        total += mon->statStages[stat] - DEFAULT_STAT_STAGE;
    return total;
}

SINGLE_BATTLE_TEST("Chemical Oddity is the Apex ability of Alolan Grimer and Muk only")
{
    enum Species species = SPECIES_GRIMER_ALOLA;
    enum Item item = ITEM_APEX_METEORITE;
    PARAMETRIZE { species = SPECIES_GRIMER_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MUK_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRIMER_ALOLA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MUK_ALOLA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_GRIMER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MUK; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        bool32 active = item == ITEM_APEX_METEORITE && (species == SPECIES_GRIMER_ALOLA || species == SPECIES_MUK_ALOLA);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_CHEMICAL_ODDITY, active);
        EXPECT_EQ(TotalChemicalBoosts(player), active ? 6 : 0);
        EXPECT_EQ(player->statStages[STAT_HP], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Chemical Oddity can put all six stages into any one of the five core stats")
{
    enum Stat stat = STAT_ATK;
    for (enum Stat i = STAT_ATK; i < NUM_STATS; i++)
        PARAMETRIZE { stat = i; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        // The switch-in rolls use the RNG override for the pivot action.
        TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_CHEMICAL_ODDITY, stat - STAT_ATK)); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHEMICAL_ODDITY);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[stat], MAX_STAT_STAGE);
        EXPECT_EQ(TotalChemicalBoosts(player), 6);
    }
}

DOUBLE_BATTLE_TEST("Chemical Oddity boosts each holder once without affecting foes or allies")
{
    bool32 secondHolder = FALSE;
    PARAMETRIZE { secondHolder = FALSE; }
    PARAMETRIZE { secondHolder = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_GRIMER_ALOLA) { Item(secondHolder ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(TotalChemicalBoosts(playerLeft), 6);
        EXPECT_EQ(TotalChemicalBoosts(playerRight), secondHolder ? 6 : 0);
        EXPECT_EQ(TotalChemicalBoosts(opponentLeft), 0);
        EXPECT_EQ(TotalChemicalBoosts(opponentRight), 0);
    }
}

SINGLE_BATTLE_TEST("Chemical Oddity rolls on every reentry and does not trigger again at turn end")
{
    GIVEN {
        PLAYER(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHEMICAL_ODDITY);
        ABILITY_POPUP(player, ABILITY_CHEMICAL_ODDITY);
        NOT ABILITY_POPUP(player, ABILITY_CHEMICAL_ODDITY);
    } THEN {
        EXPECT_EQ(TotalChemicalBoosts(player), 6);
    }
}

SINGLE_BATTLE_TEST("Chemical Oddity rerolls among available stats and stops safely when all core stats are capped")
{
    GIVEN {
        PLAYER(SPECIES_MUK_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        u8 boosts[NUM_STATS];
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            player->statStages[stat] = MAX_STAT_STAGE;
        EXPECT_EQ(RollChemicalOddityBoosts(B_BATTLER_0, boosts), 0);
        for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
            EXPECT_EQ(boosts[stat], 0);

        player->statStages[STAT_DEF] -= 2;
        EXPECT_EQ(RollChemicalOddityBoosts(B_BATTLER_0, boosts), 2);
        EXPECT_EQ(boosts[STAT_DEF], 2);
        EXPECT_EQ(boosts[STAT_SPDEF], 0);
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE - 2);

        // A new roll uses the current stages, with no cached distribution.
        player->statStages[STAT_DEF] = MAX_STAT_STAGE;
        player->statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE;
        EXPECT_EQ(RollChemicalOddityBoosts(B_BATTLER_0, boosts), 6);
        EXPECT_EQ(boosts[STAT_DEF], 0);
        EXPECT_EQ(boosts[STAT_SPDEF], 6);
    }
}

DOUBLE_BATTLE_TEST("Chemical Oddity remains active under Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_GRIMER_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_CHEMICAL_ODDITY);
        EXPECT_EQ(TotalChemicalBoosts(playerLeft), 6);
    }
}
