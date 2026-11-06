#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

static s32 TotalVoodooBoosts(const struct BattlePokemon *mon)
{
    s32 total = 0;
    for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        total += mon->statStages[stat] - DEFAULT_STAT_STAGE;
    return total;
}

SINGLE_BATTLE_TEST("Voodoo Strength boosts once on entry and once at turn end for Galarian Slowking")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_PSYCH_UP, target: player); } } THEN {
        EXPECT_EQ(GetBattlerAbility(0), ABILITY_VOODOO_STRENGTH);
        EXPECT_EQ(TotalVoodooBoosts(player), 4);
        EXPECT_EQ(TotalVoodooBoosts(opponent), 2);
        EXPECT_EQ(player->statStages[STAT_HP], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Voodoo Strength boosts its ally independently including Accuracy and Evasion")
{
    PASSES_RANDOMLY(1, 7, RNG_VOODOO_STRENGTH_ALLY);
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 4);
        EXPECT_EQ(playerLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 4);
        EXPECT_EQ(TotalVoodooBoosts(playerLeft), 4);
        EXPECT_EQ(TotalVoodooBoosts(playerRight), 4);
    }
}

SINGLE_BATTLE_TEST("Voodoo Strength chooses uniformly among all seven non-HP battle stats")
{
    PASSES_RANDOMLY(1, 7, RNG_VOODOO_STRENGTH_SELF);
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 4);
    }
}

SINGLE_BATTLE_TEST("Voodoo Strength skips capped stats and stops safely when all seven are maxed")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            player->statStages[stat] = MAX_STAT_STAGE;
        enum Stat stats[NUM_BATTLE_STATS];
        player->statStages[STAT_ACC] = MAX_STAT_STAGE - 1;
        EXPECT_EQ(GetVoodooStrengthStats(0, ABILITY_VOODOO_STRENGTH, stats), 1);
        EXPECT_EQ(stats[0], STAT_ACC);
        player->statStages[STAT_ACC] = MAX_STAT_STAGE;
        EXPECT_EQ(GetVoodooStrengthStats(0, ABILITY_VOODOO_STRENGTH, stats), 0);
        EXPECT(!TryVoodooStrength(0));
    }
}

DOUBLE_BATTLE_TEST("Voodoo Strength respects Simple and Contrary on its ally")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(TotalVoodooBoosts(playerLeft), 4);
        // Simple caps the second +4 at +6 in the same chosen stat.
        EXPECT_EQ(TotalVoodooBoosts(playerRight), ability == ABILITY_SIMPLE ? 6 : -4);
    }
}

DOUBLE_BATTLE_TEST("Voodoo Strength boosts an ally behind Substitute")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_SUBSTITUTE); } } THEN {
        EXPECT(playerRight->volatiles.substitute);
        EXPECT_EQ(TotalVoodooBoosts(playerRight), 4);
    }
}

DOUBLE_BATTLE_TEST("Voodoo Strength does not boost an ally after it faints")
{
    GIVEN {
        PLAYER(SPECIES_SLOWKING_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: playerRight); } } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
        HP_BAR(playerRight);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->hp, 0);
        EXPECT_EQ(TotalVoodooBoosts(playerLeft), 4);
    }
}
