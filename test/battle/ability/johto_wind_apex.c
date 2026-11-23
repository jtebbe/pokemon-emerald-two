#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("The Hoppip through Wooper families receive their Apex abilities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_HOPPIP; ability = ABILITY_WINDBORNE; }
    PARAMETRIZE { species = SPECIES_SKIPLOOM; ability = ABILITY_WINDBORNE; }
    PARAMETRIZE { species = SPECIES_JUMPLUFF; ability = ABILITY_WINDBORNE; }
    PARAMETRIZE { species = SPECIES_AIPOM; ability = ABILITY_MONKEY_BUSINESS; }
    PARAMETRIZE { species = SPECIES_AMBIPOM; ability = ABILITY_MONKEY_BUSINESS; }
    PARAMETRIZE { species = SPECIES_SUNKERN; ability = ABILITY_SOL_STRENGTH; }
    PARAMETRIZE { species = SPECIES_SUNFLORA; ability = ABILITY_SOL_STRENGTH; }
    PARAMETRIZE { species = SPECIES_YANMA; ability = ABILITY_OMNIMOVEMENT; }
    PARAMETRIZE { species = SPECIES_YANMEGA; ability = ABILITY_OMNIMOVEMENT; }
    PARAMETRIZE { species = SPECIES_WOOPER; ability = ABILITY_NO_THOUGHTS; }
    PARAMETRIZE { species = SPECIES_QUAGSIRE; ability = ABILITY_NO_THOUGHTS; }
    PARAMETRIZE { species = SPECIES_WOOPER_PALDEA; ability = ABILITY_MUD_WALLOWING; }
    PARAMETRIZE { species = SPECIES_CLODSIRE; ability = ABILITY_MUD_WALLOWING; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ability); }
}

SINGLE_BATTLE_TEST("Windborne raises evasion after its successful Tailwind and not a failed repeat")
{
    GIVEN {
        PLAYER(SPECIES_JUMPLUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
        TURN { MOVE(player, MOVE_TAILWIND); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WINDBORNE);
        MESSAGE("Jumpluff's evasiveness rose!");
        MESSAGE("But it failed!");
    } THEN { EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Windborne gains evasion on entry under its own side's Tailwind")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_HOPPIP) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
        TURN { SWITCH(player, 1); }
    } SCENE { ABILITY_POPUP(player, ABILITY_WINDBORNE); } THEN {
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Windborne's evasion trigger belongs to its own Tailwind rather than an ally's")
{
    GIVEN {
        PLAYER(SPECIES_JUMPLUFF) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_TAILWIND); } } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Windborne doubles physical and special attacking stats only in Tailwind", s16 damage)
{
    enum Move move;
    bool32 wind;
    PARAMETRIZE { move = MOVE_SCRATCH; wind = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; wind = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; wind = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; wind = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SKIPLOOM) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { if (wind) MOVE(player, MOVE_TAILWIND); }
        TURN { MOVE(player, move, criticalHit: FALSE); }
    } SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Windborne loses its offense multiplier when Tailwind expires", s16 first; s16 last)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_JUMPLUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
        TURN {}
        TURN {}
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        HP_BAR(opponent, captureDamage: &results[i].last);
    } FINALLY { EXPECT_MUL_EQ(results[0].last, Q_4_12(2.0), results[0].first); }
}

DOUBLE_BATTLE_TEST("Monkey Business removes both foes' items without consuming berries or harming its ally")
{
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_DEDENNE) { Ability(ABILITY_CHEEK_POUCH); Item(ITEM_SITRUS_BERRY); HP(200); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CHOICE_SCARF); }
    } WHEN { TURN {} } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MONKEY_BUSINESS);
        NOT HP_BAR(opponentLeft);
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_APEX_METEORITE);
        EXPECT_EQ(playerRight->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        EXPECT_EQ(opponentLeft->hp, 200);
        EXPECT_EQ(gBattleStruct->partyState[B_TRAINER_OPPONENT_A][0].usedHeldItem, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Monkey Business respects Sticky Hold and permanently protected held items")
{
    enum Species species;
    enum Item item;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_LEFTOVERS; ability = ABILITY_STICKY_HOLD; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { species = SPECIES_VENUSAUR; item = ITEM_VENUSAURITE; ability = ABILITY_OVERGROW; }
    GIVEN {
        PLAYER(SPECIES_AMBIPOM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Item(item); Ability(ability); }
    } WHEN { TURN {} } THEN { EXPECT_EQ(opponent->item, item); }
}

SINGLE_BATTLE_TEST("Monkey Business activates Unburden without permanently destroying the held item")
{
    GIVEN {
        PLAYER(SPECIES_AMBIPOM) { Item(ITEM_APEX_METEORITE); Speed(50); }
        OPPONENT(SPECIES_DRIFBLIM) { Ability(ABILITY_UNBURDEN); Item(ITEM_AIR_BALLOON); Speed(100); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT(opponent->volatiles.unburdenActive);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_1, ABILITY_UNBURDEN, HOLD_EFFECT_NONE), 200);
        // THEN runs after the normal end-of-battle held-item restoration.
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_AIR_BALLOON);
    }
}

SINGLE_BATTLE_TEST("Monkey Business removal persists through the target switching out and back")
{
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DRIFBLIM) { Item(ITEM_AIR_BALLOON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN { EXPECT_EQ(opponent->item, ITEM_NONE); }
}

SINGLE_BATTLE_TEST("Monkey Business includes Defiant without triggering on its own stat drops")
{
    bool32 self;
    PARAMETRIZE { self = FALSE; }
    PARAMETRIZE { self = TRUE; }
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { if (self) MOVE(player, MOVE_SUPERPOWER); else MOVE(opponent, MOVE_GROWL); } } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (self ? -1 : 1));
    }
}

SINGLE_BATTLE_TEST("Monkey Business includes Technician for moves at or below 60 power", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SLASH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SLASH; }
    GIVEN {
        PLAYER(SPECIES_AIPOM) { Ability(ABILITY_PICKUP); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Sol Strength doubles both offenses in ordinary and extreme sun", s16 damage)
{
    enum Move move;
    enum Ability weather;
    PARAMETRIZE { move = MOVE_SCRATCH; weather = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; weather = ABILITY_DROUGHT; }
    PARAMETRIZE { move = MOVE_SCRATCH; weather = ABILITY_DESOLATE_LAND; }
    PARAMETRIZE { move = MOVE_SWIFT; weather = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SWIFT; weather = ABILITY_DROUGHT; }
    PARAMETRIZE { move = MOVE_SWIFT; weather = ABILITY_DESOLATE_LAND; }
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(weather); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(2.0), results[4].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Sol Strength doubles both defenses and Speed in sun", s16 damage)
{
    enum Move move;
    bool32 sun;
    PARAMETRIZE { move = MOVE_SCRATCH; sun = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; sun = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; sun = FALSE; }
    PARAMETRIZE { move = MOVE_SWIFT; sun = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(sun ? ABILITY_DROUGHT : ABILITY_SHADOW_TAG); Speed(100); }
        OPPONENT(SPECIES_SUNFLORA) { Item(ITEM_APEX_METEORITE); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_1, ABILITY_SOL_STRENGTH, HOLD_EFFECT_APEX_METEORITE), sun ? 200 : 100);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Sol Strength heals one eighth each sun turn and respects Heal Block")
{
    bool32 blocked;
    PARAMETRIZE { blocked = FALSE; }
    PARAMETRIZE { blocked = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SUNKERN) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); }
    } WHEN { TURN { if (blocked) MOVE(opponent, MOVE_HEAL_BLOCK); } } THEN {
        EXPECT_EQ(player->hp, blocked ? 100 : 150);
    }
}

DOUBLE_BATTLE_TEST("Sol Strength stops immediately under Cloud Nine or overwritten sun")
{
    bool32 cloud;
    PARAMETRIZE { cloud = FALSE; }
    PARAMETRIZE { cloud = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SUNFLORA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(cloud ? ABILITY_CLOUD_NINE : ABILITY_SHADOW_TAG); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DROUGHT); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { if (!cloud) MOVE(opponentLeft, MOVE_RAIN_DANCE); } } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, ABILITY_SOL_STRENGTH, HOLD_EFFECT_APEX_METEORITE), 100);
    }
}

SINGLE_BATTLE_TEST("Omnimovement acts first within its bracket despite Speed and Trick Room")
{
    bool32 room;
    PARAMETRIZE { room = FALSE; }
    PARAMETRIZE { room = TRUE; }
    GIVEN {
        PLAYER(SPECIES_YANMEGA) { Item(ITEM_APEX_METEORITE); Speed(room ? 100 : 1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(room ? 1 : 100); HP(3000); MaxHP(3000); }
    } WHEN {
        if (room) TURN { MOVE(opponent, MOVE_TRICK_ROOM); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
    }
}

SINGLE_BATTLE_TEST("Omnimovement cannot outrun a higher priority bracket")
{
    GIVEN {
        PLAYER(SPECIES_YANMA) { Item(ITEM_APEX_METEORITE); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_QUICK_ATTACK); } } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
    }
}

SINGLE_BATTLE_TEST("Omnimovement reduces only noncontact accuracy by twenty percent")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PASSES_RANDOMLY(move == MOVE_WATER_GUN ? 80 : 100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_YANMEGA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } } SCENE { HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Omnimovement leaves perfect accuracy moves accurate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_YANMEGA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SWIFT); } } SCENE { HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("No Thoughts blocks Psychic attacks and targeted Psychic status through Mold Breaker")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_PSYCHIC; }
    PARAMETRIZE { move = MOVE_HEAL_BLOCK; }
    GIVEN {
        PLAYER(SPECIES_QUAGSIRE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN { TURN { MOVE(opponent, move); } } SCENE {
        ABILITY_POPUP(player, ABILITY_NO_THOUGHTS);
        NOT HP_BAR(player);
    } THEN { EXPECT_EQ((u32)player->volatiles.healBlockTimer, 0); }
}

SINGLE_BATTLE_TEST("No Thoughts includes Oblivious against infatuation Taunt and Intimidate")
{
    GIVEN {
        PLAYER(SPECIES_WOOPER) { Item(ITEM_APEX_METEORITE); Gender(MON_MALE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INTIMIDATE); Gender(MON_FEMALE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ATTRACT); }
        TURN { MOVE(opponent, MOVE_TAUNT); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ((u32)player->volatiles.infatuation, 0);
        EXPECT_EQ((u32)player->volatiles.tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("No Thoughts ignores opposing offensive and defensive stages", s16 damage)
{
    bool32 boosted, holderAttacks;
    PARAMETRIZE { boosted = FALSE; holderAttacks = FALSE; }
    PARAMETRIZE { boosted = TRUE; holderAttacks = FALSE; }
    PARAMETRIZE { boosted = FALSE; holderAttacks = TRUE; }
    PARAMETRIZE { boosted = TRUE; holderAttacks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_QUAGSIRE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { if (boosted) MOVE(opponent, MOVE_BULK_UP); }
        TURN { if (holderAttacks) MOVE(player, MOVE_SCRATCH, criticalHit: FALSE); else MOVE(opponent, MOVE_SCRATCH, criticalHit: FALSE); }
    } SCENE {
        if (holderAttacks) HP_BAR(opponent, captureDamage: &results[i].damage);
        else HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Mud Wallowing absorbs Ground and Water for a quarter HP even through Mold Breaker")
{
    enum Move move;
    bool32 blocked;
    PARAMETRIZE { move = MOVE_WATER_GUN; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; blocked = TRUE; }
    GIVEN {
        PLAYER(SPECIES_CLODSIRE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        if (blocked) TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(opponent, move); }
    } THEN { EXPECT_EQ(player->hp, blocked ? 100 : 200); }
}

AI_SINGLE_BATTLE_TEST("AI avoids No Thoughts Psychic immunity and both Mud Wallowing immunities")
{
    enum Species species;
    enum Move move;
    PARAMETRIZE { species = SPECIES_QUAGSIRE; move = MOVE_PSYCHIC; }
    PARAMETRIZE { species = SPECIES_CLODSIRE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { species = SPECIES_CLODSIRE; move = MOVE_EARTH_POWER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_SCRATCH); }
    } WHEN { TURN { SCORE_LT(opponent, move, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
}

AI_SINGLE_BATTLE_TEST("Omnimovement AI forecasts ordering and its noncontact accuracy penalty")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_YANMEGA) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_SCRATCH); Speed(100); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], 80);
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][1], 100);
        EXPECT(AI_IsSlower(B_BATTLER_1, B_BATTLER_0, MOVE_SCRATCH, MOVE_SCRATCH, CONSIDER_PRIORITY));
    }
}

AI_SINGLE_BATTLE_TEST("Windborne AI values Tailwind's offensive boost even when already faster")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_JUMPLUFF) { Item(ITEM_APEX_METEORITE); Moves(MOVE_TAILWIND, MOVE_CELEBRATE, MOVE_GUST); Speed(100); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_TAILWIND, MOVE_CELEBRATE); } }
}

AI_SINGLE_BATTLE_TEST("Monkey Business switch forecasts preserve actual items and the next candidate's cache")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CHOICE_BAND); Moves(MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_AMBIPOM) { Item(ITEM_APEX_METEORITE); Moves(MOVE_TACKLE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); Speed(100); }
    } WHEN { TURN {} } THEN {
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_FORCED);
        EXPECT_EQ(player->item, ITEM_CHOICE_BAND);
        EXPECT_EQ(gAiLogicData->items[B_BATTLER_0], ITEM_CHOICE_BAND);
        EXPECT_EQ(gAiLogicData->holdEffects[B_BATTLER_0], HOLD_EFFECT_CHOICE_BAND);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_CHOICE_BAND);
    }
}

AI_SINGLE_BATTLE_TEST("Sol Strength forecasts hypothetical sunlight without changing the real weather")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SUNFLORA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        u32 realWeather = gBattleWeather;
        struct AiCalcValues calc = { .move = MOVE_SCRATCH, .weather = B_WEATHER_NONE, .terrain = B_TERRAIN_NONE };
        struct SimulatedDamage normalOut = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        struct SimulatedDamage normalIn = AI_CalcDamage(&calc, B_BATTLER_1, B_BATTLER_0);
        calc.weather = B_WEATHER_SUN_NORMAL;
        struct SimulatedDamage sunOut = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        struct SimulatedDamage sunIn = AI_CalcDamage(&calc, B_BATTLER_1, B_BATTLER_0);
        EXPECT_GT(sunOut.median, normalOut.median);
        EXPECT_LT(sunIn.median, normalIn.median);
        EXPECT_EQ(gBattleWeather, realWeather);
    }
}
