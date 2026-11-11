#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Yaoi on Ice starts snow on entry and copies another battler's dance")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_MR_MIME_GALAR; }
    PARAMETRIZE { species = SPECIES_MR_RIME; }
    GIVEN {
        WITH_CONFIG(B_SNOW_WARNING, GEN_8);
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUIVER_DANCE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_YAOI_ON_ICE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_YAOI_ON_ICE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, player);
    } THEN {
        EXPECT_EQ(GetWeather(), B_WEATHER_SNOW);
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
