#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Honed Hunter uses Hone Claws on entry, doubles Speed in rain and boosts slicing moves")
{
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_WEATHER_RAIN_TEMPORARY);
    GIVEN {
        PLAYER(SPECIES_KABUTOPS) { Item(ITEM_APEX_METEORITE); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(75); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HONED_HUNTER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HONE_CLAWS, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLASH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_SLASH) * 3 / 2);
    }
}

SINGLE_BATTLE_TEST("Aerial Terror applies Intimidate and reduces damage only from grounded opponents")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_AIR_BALLOON; }
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Item(ITEM_APEX_METEORITE); HP(300); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Level(90); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_NIGHT_SHADE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AERIAL_TERROR);
        ABILITY_POPUP(player, ABILITY_AERIAL_TERROR);
        HP_BAR(player, damage: item == ITEM_NONE ? 60 : 90);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Aerial Terror prevents opponents from eating Berries")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SITRUS_BERRY); HP(49); MaxHP(100); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
        EXPECT_EQ(opponent->hp, 49);
    }
}

SINGLE_BATTLE_TEST("Hanger Attack heals one sixteenth and grants priority and critical hits strictly below half HP")
{
    u32 hp;
    PARAMETRIZE { hp = 80; }
    PARAMETRIZE { hp = 79; }
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(160); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (hp == 80)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        if (hp < 80)
        {
            MESSAGE("A critical hit!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
        else
        {
            NOT MESSAGE("A critical hit!");
        }
        HP_BAR(player, damage: -10);
    } THEN {
        EXPECT_EQ(player->hp, hp + 10);
    }
}

SINGLE_BATTLE_TEST("Iceborn, Shockborn and Flameborn set their fields and add the matching status to attacks")
{
    enum Species species;
    enum Move move;
    u32 status;
    PARAMETRIZE { species = SPECIES_ARTICUNO; move = MOVE_ICE_SHARD; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { species = SPECIES_ZAPDOS; move = MOVE_SHOCK_WAVE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { species = SPECIES_MOLTRES; move = MOVE_INCINERATE; status = STATUS1_BURN; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT(opponent->status1 & status);
        if (species == SPECIES_ARTICUNO)
        {
            EXPECT(gBattleWeather & B_WEATHER_SNOW);
        }
        else if (species == SPECIES_MOLTRES)
        {
            EXPECT(gBattleWeather & B_WEATHER_SUN);
        }
        else
        {
            EXPECT_EQ((enum BattleTerrain)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
            EXPECT(!IsBattlerGrounded(B_BATTLER_0, ABILITY_SHOCKBORN, HOLD_EFFECT_APEX_METEORITE));
            EXPECT_EQ(gBattleMovePower, GetMovePower(move) * (B_TERRAIN_TYPE_BOOST >= GEN_8 ? 130 : 150) / 100);
        }
    }
}

DOUBLE_BATTLE_TEST("Frigid Stare frostbites both opponents on entry and guarantees Psychic critical hits")
{
    bool32 targetAlly;
    PARAMETRIZE { targetAlly = FALSE; }
    PARAMETRIZE { targetAlly = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARTICUNO_GALAR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); Status1(targetAlly ? STATUS1_FROSTBITE : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CONFUSION, target: targetAlly ? playerRight : opponentLeft, criticalHit: FALSE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_FRIGID_STARE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, playerLeft);
        if (targetAlly)
            NOT MESSAGE("A critical hit!");
        else
            MESSAGE("A critical hit!");
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_FROSTBITE);
        EXPECT(opponentRight->status1 & STATUS1_FROSTBITE);
        EXPECT_EQ(playerRight->status1, targetAlly ? STATUS1_FROSTBITE : STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Rapid Runner ignores hazards and boosts Attack and Speed once per kicking move")
{
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_STEALTH_ROCK_PLAYER);
    SetStartingStatus(STARTING_STATUS_SPIKES_PLAYER_L3);
    SetStartingStatus(STARTING_STATUS_TOXIC_SPIKES_PLAYER_L2);
    GIVEN {
        PLAYER(SPECIES_ZAPDOS_GALAR) { Item(ITEM_APEX_METEORITE); HP(200); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Blackened Wings burns targets of damaging and targeted status Dark moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DARK_PULSE; }
    PARAMETRIZE { move = MOVE_TAUNT; }
    GIVEN {
        PLAYER(SPECIES_MOLTRES_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Blackened Wings boosts Flying damage once and then clears both weather and terrain", s16 damage)
{
    bool32 fieldActive;
    PARAMETRIZE { fieldActive = FALSE; }
    PARAMETRIZE { fieldActive = TRUE; }
    ResetStartingStatuses();
    if (fieldActive)
    {
        SetStartingStatus(STARTING_STATUS_WEATHER_RAIN_TEMPORARY);
        SetStartingStatus(STARTING_STATUS_PSYCHIC_TERRAIN_TEMPORARY);
    }
    GIVEN {
        PLAYER(SPECIES_MOLTRES_GALAR) { Item(ITEM_APEX_METEORITE); Level(100); SpAttack(350); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH, hit: TRUE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
        EXPECT_EQ((enum BattleTerrain)gFieldTimers.terrain, B_TERRAIN_NONE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}
