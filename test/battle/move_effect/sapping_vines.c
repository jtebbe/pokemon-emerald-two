#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sapping Vines raises and lowers the same randomly chosen core stat")
{
    enum Stat stat = STAT_ATK;
    for (enum Stat core = STAT_ATK; core < NUM_STATS; core++)
        PARAMETRIZE { stat = core; }
    GIVEN {
        PLAYER(SPECIES_EXEGGCUTE);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SAPPING_VINES, WITH_RNG(RNG_SAPPING_VINES, stat)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAPPING_VINES, player);
        HP_BAR(opponent);
    } THEN {
        for (enum Stat core = STAT_ATK; core < NUM_BATTLE_STATS; core++)
        {
            EXPECT_EQ(player->statStages[core], DEFAULT_STAT_STAGE + (core == stat));
            EXPECT_EQ(opponent->statStages[core], DEFAULT_STAT_STAGE - (core == stat));
        }
    }
}

SINGLE_BATTLE_TEST("Great Minds applies Sapping Vines' stat changes on each strike")
{
    GIVEN {
        PLAYER(SPECIES_EXEGGUTOR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SAPPING_VINES, WITH_RNG(RNG_SAPPING_VINES, STAT_SPEED)); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 5);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 5);
    }
}
