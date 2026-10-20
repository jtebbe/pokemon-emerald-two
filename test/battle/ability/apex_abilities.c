#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Time Dominus activates only with an Apex Meteorite on either Dialga form")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_DIALGA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_DIALGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DIALGA_ORIGIN; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), item == ITEM_APEX_METEORITE ? ABILITY_TIME_DOMINUS : player->ability);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, item == ITEM_APEX_METEORITE ? 254 : 3);
    }
}

SINGLE_BATTLE_TEST("An unassigned Apex ability is NONE rather than a normal ability fallback")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_APEX_METEORITE); Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_SCRATCH, 10}); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_NONE);
        EXPECT_EQ(opponent->pp[0], 9);
        EXPECT(!CanBattlerGetOrLoseItem(B_BATTLER_1, B_BATTLER_0, ITEM_APEX_METEORITE));
    }
}

SINGLE_BATTLE_TEST("Time Dominus extends move-created weather terrain rooms and side effects to 255 turns")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    PARAMETRIZE { move = MOVE_SUNNY_DAY; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; }
    PARAMETRIZE { move = MOVE_ELECTRIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_GRASSY_TERRAIN; }
    PARAMETRIZE { move = MOVE_MISTY_TERRAIN; }
    PARAMETRIZE { move = MOVE_PSYCHIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_TRICK_ROOM; }
    PARAMETRIZE { move = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { move = MOVE_WONDER_ROOM; }
    PARAMETRIZE { move = MOVE_GRAVITY; }
    PARAMETRIZE { move = MOVE_REFLECT; }
    PARAMETRIZE { move = MOVE_LIGHT_SCREEN; }
    PARAMETRIZE { move = MOVE_MIST; }
    PARAMETRIZE { move = MOVE_SAFEGUARD; }
    PARAMETRIZE { move = MOVE_TAILWIND; }
    PARAMETRIZE { move = MOVE_LUCKY_CHANT; }
    PARAMETRIZE { move = MOVE_WATER_SPORT; }
    PARAMETRIZE { move = MOVE_MUD_SPORT; }
    PARAMETRIZE { move = MOVE_FAIRY_LOCK; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        switch (move)
        {
        case MOVE_RAIN_DANCE: case MOVE_SUNNY_DAY: case MOVE_SANDSTORM: case MOVE_SNOWSCAPE:
            EXPECT_EQ(gBattleStruct->weatherDuration, 254);
            EXPECT_EQ(gBattleStruct->weatherDurationTotal, 255);
            break;
        case MOVE_ELECTRIC_TERRAIN: case MOVE_GRASSY_TERRAIN: case MOVE_MISTY_TERRAIN: case MOVE_PSYCHIC_TERRAIN:
            EXPECT_EQ(gFieldTimers.terrainTimer, 254);
            EXPECT_EQ(gFieldTimers.terrainTimerTotal, 255);
            break;
        case MOVE_TRICK_ROOM: EXPECT_EQ(gFieldTimers.trickRoomTimer, 254); break;
        case MOVE_MAGIC_ROOM: EXPECT_EQ(gFieldTimers.magicRoomTimer, 254); break;
        case MOVE_WONDER_ROOM: EXPECT_EQ(gFieldTimers.wonderRoomTimer, 254); break;
        case MOVE_GRAVITY: EXPECT_EQ(gFieldTimers.gravityTimer, 254); break;
        case MOVE_REFLECT:
            EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimer, 254);
            EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimerTotal, 255);
            break;
        case MOVE_LIGHT_SCREEN:
            EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].lightscreenTimer, 254);
            EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].lightscreenTimerTotal, 255);
            break;
        case MOVE_MIST: EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].mistTimer, 254); break;
        case MOVE_SAFEGUARD: EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].safeguardTimer, 254); break;
        case MOVE_TAILWIND: EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 254); break;
        case MOVE_LUCKY_CHANT: EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].luckyChantTimer, 254); break;
        case MOVE_WATER_SPORT: EXPECT_EQ(gFieldTimers.waterSportTimer, 254); break;
        case MOVE_MUD_SPORT: EXPECT_EQ(gFieldTimers.mudSportTimer, 254); break;
        case MOVE_FAIRY_LOCK: EXPECT_EQ(gFieldTimers.fairyLockTimer, 254); break;
        default: EXPECT(FALSE); break;
        }
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
    }
}

SINGLE_BATTLE_TEST("Time Dominus does not extend an opponent's move-created timers")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); }
    } THEN {
        EXPECT_EQ(gBattleStruct->weatherDuration, 4);
        EXPECT_EQ(gBattleStruct->weatherDurationTotal, 5);
    }
}

SINGLE_BATTLE_TEST("Time Dominus rooms can still be toggled off")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TRICK_ROOM; }
    PARAMETRIZE { move = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { move = MOVE_WONDER_ROOM; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 0);
        EXPECT_EQ(gFieldTimers.magicRoomTimer, 0);
        EXPECT_EQ(gFieldTimers.wonderRoomTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Time Dominus leaves Storm Sacrifice's endless weather and terrain endless")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_SACRIFICE); }
    } THEN {
        EXPECT_EQ(gBattleStruct->weatherDuration, 0);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Time Dominus skips damaging charge turns including semi-invulnerable moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { move = MOVE_METEOR_BEAM; }
    PARAMETRIZE { move = MOVE_ELECTRO_SHOT; }
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_DIG; }
    PARAMETRIZE { move = MOVE_DIVE; }
    PARAMETRIZE { move = MOVE_SKY_DROP; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Dialga compressed time!");
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT(!player->volatiles.chargeTurn);
        EXPECT_EQ((u32)player->volatiles.semiInvulnerable, STATE_NONE);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
        if (move == MOVE_METEOR_BEAM || move == MOVE_ELECTRO_SHOT)
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Time Dominus skips Geomancy's charge without losing its boosts")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GEOMANCY); }
    } SCENE {
        MESSAGE("Dialga compressed time!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT(!player->volatiles.chargeTurn);
    }
}

SINGLE_BATTLE_TEST("Time Dominus skips recharge turns entirely")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_HYPER_BEAM; }
    PARAMETRIZE { move = MOVE_ROAR_OF_TIME; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, move); }
    } SCENE {
        NOT MESSAGE("Dialga compressed time!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ((u32)player->volatiles.rechargeTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Apex abilities cannot be suppressed replaced or swapped")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GASTRO_ACID; }
    PARAMETRIZE { move = MOVE_CORE_ENFORCER; }
    PARAMETRIZE { move = MOVE_WORRY_SEED; }
    PARAMETRIZE { move = MOVE_SIMPLE_BEAM; }
    PARAMETRIZE { move = MOVE_ENTRAINMENT; }
    PARAMETRIZE { move = MOVE_SKILL_SWAP; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
        TURN { MOVE(player, MOVE_TAILWIND); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_TELEPATHY);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 254);
        EXPECT(!player->volatiles.gastroAcid);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas and Magic Room do not disable an Apex ability")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGIC_ROOM); }
        TURN { MOVE(player, MOVE_TAILWIND); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
        EXPECT_EQ(GetBattlerHoldEffect(B_BATTLER_0), HOLD_EFFECT_APEX_METEORITE);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 254);
    }
}

SINGLE_BATTLE_TEST("Trace and Role Play cannot copy an Apex ability")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_RALTS) { Ability(ABILITY_TRACE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROLE_PLAY); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_TRACE);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
    }
}

SINGLE_BATTLE_TEST("Transform copies shape but neither borrows nor replaces an Apex ability")
{
    bool32 apexTransforms;
    PARAMETRIZE { apexTransforms = FALSE; }
    PARAMETRIZE { apexTransforms = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_DITTO) { Ability(ABILITY_LIMBER); }
    } WHEN {
        if (apexTransforms)
            TURN { MOVE(player, MOVE_TRANSFORM); }
        else
            TURN { MOVE(opponent, MOVE_TRANSFORM); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_LIMBER);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Apex Meteorite cannot be removed stolen switched or corroded in battle")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_KNOCK_OFF; }
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }
    PARAMETRIZE { move = MOVE_CORROSIVE_GAS; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
        EXPECT_EQ(opponent->item, ITEM_ORAN_BERRY);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
    }
}

SINGLE_BATTLE_TEST("Apex Meteorite cannot be consumed by Fling")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Time Dominus's extended effects persist after switching out beyond their normal duration")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); MOVE(opponent, MOVE_TAILWIND); }
        TURN { SWITCH(player, 1); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 249);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].tailwindTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Contact abilities cannot replace or swap Time Dominus")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_COFAGRIGUS; ability = ABILITY_MUMMY; }
    PARAMETRIZE { species = SPECIES_RUNERIGUS; ability = ABILITY_WANDERING_SPIRIT; }
    PARAMETRIZE { species = SPECIES_OINKOLOGNE; ability = ABILITY_LINGERING_AROMA; }
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TIME_DOMINUS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ability);
    }
}
