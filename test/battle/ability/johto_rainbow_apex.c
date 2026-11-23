#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("The Togepi through Sudowoodo families receive their Apex abilities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_TOGEPI; ability = ABILITY_LUCKY_LIL_EGG; }
    PARAMETRIZE { species = SPECIES_TOGETIC; ability = ABILITY_SERENE_SKIES; }
    PARAMETRIZE { species = SPECIES_TOGEKISS; ability = ABILITY_SERENE_SKIES; }
    PARAMETRIZE { species = SPECIES_NATU; ability = ABILITY_GAZE_BEYOND; }
    PARAMETRIZE { species = SPECIES_XATU; ability = ABILITY_GAZE_BEYOND; }
    PARAMETRIZE { species = SPECIES_MAREEP; ability = ABILITY_FURCHARGED; }
    PARAMETRIZE { species = SPECIES_FLAAFFY; ability = ABILITY_FURCHARGED; }
    PARAMETRIZE { species = SPECIES_AMPHAROS; ability = ABILITY_FURCHARGED; }
    PARAMETRIZE { species = SPECIES_AZURILL; ability = ABILITY_HYPERBUOYANT; }
    PARAMETRIZE { species = SPECIES_MARILL; ability = ABILITY_HYPERBUOYANT; }
    PARAMETRIZE { species = SPECIES_AZUMARILL; ability = ABILITY_HYPERBUOYANT; }
    PARAMETRIZE { species = SPECIES_BONSLY; ability = ABILITY_ACTUALLY_A_TREE; }
    PARAMETRIZE { species = SPECIES_SUDOWOODO; ability = ABILITY_ACTUALLY_A_TREE; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ability);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg guarantees a move's secondary status effect")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    PASSES_RANDOMLY(item == ITEM_NONE ? 30 : 100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(item); Ability(ABILITY_HUSTLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB); }
    } SCENE {
        STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg guarantees Ancient Power's self omniboost")
{
    PASSES_RANDOMLY(100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ANCIENT_POWER); }
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg guarantees flinching when the target can flinch")
{
    PASSES_RANDOMLY(100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AIR_SLASH, player);
        MESSAGE("The opposing Wobbuffet flinched and couldn't move!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg respects Shield Dust and Covert Cloak")
{
    bool32 cloak;
    PARAMETRIZE { cloak = FALSE; }
    PARAMETRIZE { cloak = TRUE; }
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) {
            HP(1000); MaxHP(1000);
            Ability(cloak ? ABILITY_SHADOW_TAG : ABILITY_SHIELD_DUST);
            Item(cloak ? ITEM_COVERT_CLOAK : ITEM_NONE);
        }
    } WHEN { TURN { MOVE(player, MOVE_SLUDGE_BOMB); } } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg preserves primary status effects")
{
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN { TURN { MOVE(player, MOVE_THUNDER_WAVE); } } THEN {
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Lucky Lil' Egg guarantees Secret Power's terrain-dependent effect")
{
    PASSES_RANDOMLY(100, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_MISTY_TERRAIN); }
        TURN { MOVE(player, MOVE_SECRET_POWER); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Serene Skies creates a four-turn rainbow without changing weather or terrain")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    GIVEN {
        PLAYER(SPECIES_TOGEKISS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, MOVE_ELECTRIC_TERRAIN); }
        for (u32 turn = 1; turn < turns; turn++) TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SERENE_SKIES);
        MESSAGE("A rainbow appeared in the sky on your side!");
    } THEN {
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW), turns < 4);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].rainbowTimer, 4 - turns);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_SERENE_SKIES, MOVE_GUST, B_BATTLER_1), turns < 4 ? 1 : 0);
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
    }
}

SINGLE_BATTLE_TEST("Serene Skies grants Flying moves priority while its rainbow is active")
{
    GIVEN {
        PLAYER(SPECIES_TOGETIC) { Item(ITEM_APEX_METEORITE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUST); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_SERENE_SKIES, MOVE_GUST, B_BATTLER_1), 1);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, ABILITY_SERENE_SKIES, MOVE_TACKLE, B_BATTLER_1), 0);
    }
}

DOUBLE_BATTLE_TEST("Serene Skies boosts its ally's Flying priority once even with two holders")
{
    bool32 secondHolder;
    PARAMETRIZE { secondHolder = FALSE; }
    PARAMETRIZE { secondHolder = TRUE; }
    GIVEN {
        PLAYER(SPECIES_TOGETIC) { Item(ITEM_APEX_METEORITE); Speed(5); }
        PLAYER(secondHolder ? SPECIES_TOGEKISS : SPECIES_PIDGEY) { Item(secondHolder ? ITEM_APEX_METEORITE : ITEM_NONE); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_GUST, target: opponentLeft); MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_2, GetBattlerAbility(B_BATTLER_2), MOVE_GUST, B_BATTLER_1), 1);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), MOVE_GUST, B_BATTLER_2), 0);
    }
}

DOUBLE_BATTLE_TEST("Serene Skies priority ends when its holder leaves but its rainbow persists")
{
    GIVEN {
        PLAYER(SPECIES_TOGETIC) { Item(ITEM_APEX_METEORITE); Speed(5); }
        PLAYER(SPECIES_PIDGEY) { Speed(10); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN {
        TURN { SWITCH(playerLeft, 2); MOVE(playerRight, MOVE_GUST, target: opponentLeft); MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, playerRight);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW);
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_2, GetBattlerAbility(B_BATTLER_2), MOVE_GUST, B_BATTLER_1), 0);
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond rejects imperfect base accuracy even with No Guard")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FIRE_BLAST; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    GIVEN {
        PLAYER(SPECIES_NATU) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MACHOP) { Ability(ABILITY_NO_GUARD); }
    } WHEN { TURN { MOVE(opponent, move); } } SCENE {
        ABILITY_POPUP(player, ABILITY_GAZE_BEYOND);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 1000);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond permits accurate, perfect accuracy, and self-targeting moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; }
    GIVEN {
        PLAYER(SPECIES_XATU) { Item(ITEM_APEX_METEORITE); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, move); } } SCENE {
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond's Future Sight keeps neutral critical damage after switching or fainting", s16 damage)
{
    u32 route;
    PARAMETRIZE { route = 0; }
    PARAMETRIZE { route = 1; }
    PARAMETRIZE { route = 2; }
    GIVEN {
        PLAYER(SPECIES_NATU) { Item(ITEM_APEX_METEORITE); SpAttack(100); HP(route == 2 ? 1 : 1000); MaxHP(1000); Speed(10); }
        PLAYER(SPECIES_WYNAUT) { SpAttack(10); Speed(10); }
        OPPONENT(SPECIES_UMBREON) { HP(3000); MaxHP(3000); SpDefense(100); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        if (route == 1) TURN { SWITCH(player, 1); }
        else if (route == 2) TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
        else TURN {}
        TURN {}
        TURN {}
    } SCENE {
        MESSAGE("The opposing Umbreon took the Future Sight attack!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
        MESSAGE("A critical hit!");
    } FINALLY {
        EXPECT_GT(results[0].damage, 0);
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond's Future Sight respects critical-hit immunity")
{
    GIVEN {
        PLAYER(SPECIES_XATU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_UMBREON) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        HP_BAR(opponent);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond's delayed attack uses the original party slot")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_NATU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_UMBREON) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(player, 0); }
        TURN {}
        TURN {}
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Gaze Beyond's other Psychic moves retain Dark immunity")
{
    GIVEN {
        PLAYER(SPECIES_NATU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_UMBREON);
    } WHEN { TURN { MOVE(player, MOVE_PSYCHIC); } } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Furcharged raises Sp. Def once per entry")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FURCHARGED);
        MESSAGE("Ampharos's Sp. Def rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Furcharged doubles physical and special Electric power on every use", s16 first, s16 second)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_THUNDER_SHOCK; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_THUNDER_SHOCK; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SPARK; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SPARK; }
    GIVEN {
        PLAYER(SPECIES_MAREEP) { Item(item); Attack(item == ITEM_NONE ? 200 : 100); SpAttack(item == ITEM_NONE ? 200 : 100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        HP_BAR(opponent, captureDamage: &results[i].second);
    } FINALLY {
        EXPECT_EQ(results[0].first, results[1].first);
        EXPECT_EQ(results[2].first, results[3].first);
        for (u32 j = 0; j < 4; j++) EXPECT_EQ(results[j].first, results[j].second);
    }
}

SINGLE_BATTLE_TEST("Furcharged provides Static's contact paralysis")
{
    PASSES_RANDOMLY(3, 10, RNG_STATIC);
    GIVEN {
        WITH_CONFIG(B_ABILITY_TRIGGER_CHANCE, GEN_9);
        PLAYER(SPECIES_FLAAFFY) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_SCRATCH); } } SCENE {
        ABILITY_POPUP(player, ABILITY_FURCHARGED);
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Furcharged treats Charge as redundant once Sp. Def is maximized")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_CHARGE); }
    } SCENE {
        MESSAGE("Ampharos used Charge!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], MAX_STAT_STAGE);
        EXPECT_EQ((u32)player->volatiles.chargeTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Hyperbuoyant absorbs damaging and status Water moves for core boosts instead of HP")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_SOAK; }
    GIVEN {
        PLAYER(SPECIES_AZURILL) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Hyperbuoyant absorbs Water moves behind Substitute")
{
    GIVEN {
        PLAYER(SPECIES_AZUMARILL) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT(player->volatiles.substitute);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Actually a Tree converts Normal moves to Grass with STAB but no additional conversion boost")
{
    GIVEN {
        PLAYER(SPECIES_BONSLY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_VAPOREON) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SWIFT); } } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    } THEN {
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_SWIFT));
        EXPECT_EQ(player->types[0], TYPE_ROCK);
    }
}

SINGLE_BATTLE_TEST("Actually a Tree grants STAB to native Grass moves", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_VINE_WHIP); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Actually a Tree absorbs Water for one quarter max HP and respects Heal Block")
{
    bool32 blocked;
    PARAMETRIZE { blocked = FALSE; }
    PARAMETRIZE { blocked = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (blocked) TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(player->hp, blocked ? 100 : 200);
    }
}

AI_SINGLE_BATTLE_TEST("Lucky Lil' Egg AI identifies guaranteed flinches")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(10); }
        OPPONENT(SPECIES_TOGEPI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_AIR_SLASH); Speed(100); }
    } WHEN { TURN {} } THEN {
        EXPECT(IsFlinchGuaranteed(B_BATTLER_1, B_BATTLER_0, MOVE_AIR_SLASH));
    }
}

AI_DOUBLE_BATTLE_TEST("Serene Skies AI gives its ally Flying priority")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_TOGETIC) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); Speed(5); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GUST); Speed(10); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCRATCH, target: opponentRight); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
    } THEN {
        EXPECT_EQ(AI_GetMovePriorityWithAbilities(B_BATTLER_3, gAiLogicData->abilities[B_BATTLER_3], MOVE_GUST,
            B_BATTLER_0, gAiLogicData->abilities), 1);
    }
}

AI_SINGLE_BATTLE_TEST("Gaze Beyond AI avoids inaccurate moves into the holder")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NATU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BLIZZARD, MOVE_ICE_BEAM); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_BLIZZARD, MOVE_ICE_BEAM); EXPECT_MOVE(opponent, MOVE_ICE_BEAM); }
    }
}

AI_SINGLE_BATTLE_TEST("Gaze Beyond AI forecasts Future Sight hitting Dark types")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_UMBREON; }
    PARAMETRIZE { species = SPECIES_REGISTEEL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_XATU) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage damage = AI_CalcDamageSaveBattlers(MOVE_FUTURE_SIGHT, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_EQ(effectiveness, UQ_4_12(1.0));
        EXPECT_GT(damage.median, 0);
    }
}

AI_SINGLE_BATTLE_TEST("Furcharged AI forecasts permanent Charge without stacking the move")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_MAREEP) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage first = AI_CalcDamageSaveBattlers(MOVE_THUNDER_SHOCK, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        opponent->volatiles.chargeTimer = 2;
        struct SimulatedDamage charged = AI_CalcDamageSaveBattlers(MOVE_THUNDER_SHOCK, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_EQ(first.median, charged.median);
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids Water attacks into Hyperbuoyant and Actually a Tree")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_MARILL; }
    PARAMETRIZE { species = SPECIES_SUDOWOODO; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_SCRATCH); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_WATER_GUN, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}

AI_DOUBLE_BATTLE_TEST("Hyperbuoyant AI can target its ally with Water for an omniboost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_MARILL) { Item(ITEM_APEX_METEORITE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentRight, MOVE_WATER_GUN, target: opponentLeft); }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
