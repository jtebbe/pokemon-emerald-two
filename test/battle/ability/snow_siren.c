#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Snow Siren boosts sound damage only in snow", s16 damage)
{
    enum Move move;
    enum Item item;
    bool32 snow;
    PARAMETRIZE { move = MOVE_SNARL; item = ITEM_NONE; snow = TRUE; }
    PARAMETRIZE { move = MOVE_SNARL; item = ITEM_APEX_METEORITE; snow = TRUE; }
    PARAMETRIZE { move = MOVE_SNARL; item = ITEM_APEX_METEORITE; snow = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_NONE; snow = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_APEX_METEORITE; snow = TRUE; }
    ResetStartingStatuses();
    if (snow)
        SetStartingStatus(STARTING_STATUS_WEATHER_SNOW);
    GIVEN {
        PLAYER(SPECIES_JYNX) { Item(item); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0), criticalHit: FALSE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage * 3 / 2);
        EXPECT_EQ(results[2].damage, results[0].damage);
        EXPECT_EQ(results[4].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Snow Siren sound moves skip accuracy in snow but still respect Soundproof")
{
    bool32 snow, soundproof;
    PARAMETRIZE { snow = TRUE; soundproof = FALSE; }
    PARAMETRIZE { snow = FALSE; soundproof = FALSE; }
    PARAMETRIZE { snow = TRUE; soundproof = TRUE; }
    ResetStartingStatuses();
    SetStartingStatus(snow ? STARTING_STATUS_WEATHER_SNOW : STARTING_STATUS_WEATHER_HAIL);
    GIVEN {
        PLAYER(SPECIES_JYNX) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA) { Ability(soundproof ? ABILITY_SOUNDPROOF : ABILITY_GUTS); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL, hit: FALSE); }
    } SCENE {
        if (snow && !soundproof) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
            HP_BAR(opponent);
        } else {
            NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        }
    }
}

SINGLE_BATTLE_TEST("Snow Siren gives Snow Cloak's accuracy penalty in hail and snow")
{
    bool32 snow;
    PARAMETRIZE { snow = TRUE; }
    PARAMETRIZE { snow = FALSE; }
    ResetStartingStatuses();
    SetStartingStatus(snow ? STARTING_STATUS_WEATHER_SNOW : STARTING_STATUS_WEATHER_HAIL);
    GIVEN {
        PLAYER(SPECIES_JYNX) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MAKUHITA);
    } WHEN {
        TURN {}
    } THEN {
        struct BattleCalcValues cv = {
            .battlerAtk = B_BATTLER_1,
            .battlerDef = B_BATTLER_0,
            .move = MOVE_TACKLE,
            .abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0),
        };
        EXPECT_EQ(GetTotalAccuracy(&cv, GetWeather()), 80);
    }
}
