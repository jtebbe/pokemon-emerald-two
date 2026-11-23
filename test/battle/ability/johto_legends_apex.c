#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_apex_johto.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Thunder Runner gains Speed and Evasion once when entering Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_ELECTRIC_SURGE); Speed(100); }
    } WHEN { TURN {} TURN {} }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Thunder Runner boosts on terrain activation without refreshing on an unchanged terrain")
{
    GIVEN {
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGNET_RISE); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); }
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
    } THEN { EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("Thunder Runner paralyzes foes on offensive and defensive contact")
{
    bool32 attack;
    PARAMETRIZE { attack = FALSE; }
    PARAMETRIZE { attack = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { if (attack) MOVE(player, MOVE_TACKLE); else MOVE(opponent, MOVE_TACKLE); } }
    THEN { EXPECT(opponent->status1 & STATUS1_PARALYSIS); EXPECT_EQ(player->status1, STATUS1_NONE); }
}

SINGLE_BATTLE_TEST("Thunder Runner respects paralysis immunity and does not paralyze through noncontact")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_LIMBER; }
    GIVEN {
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Ability(ability); }
    } WHEN { TURN { MOVE(player, move); } }
    THEN { EXPECT_EQ(opponent->status1, STATUS1_NONE); }
}

DOUBLE_BATTLE_TEST("Thunder Runner never paralyzes allies on contact in either direction")
{
    GIVEN {
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerLeft, MOVE_TACKLE, target: playerRight); MOVE(playerRight, MOVE_TACKLE, target: playerLeft); } }
    THEN { EXPECT_EQ(playerLeft->status1, STATUS1_NONE); EXPECT_EQ(playerRight->status1, STATUS1_NONE); }
}

SINGLE_BATTLE_TEST("Eruption Herald calls Eruption after a status sound move without needing a move slot or extra PP")
{
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); MovesWithPP({MOVE_GROWL, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_GROWL); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player); ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); ANIMATION(ANIM_TYPE_MOVE, MOVE_ERUPTION, player); HP_BAR(opponent); NOT HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->pp[0], 9); EXPECT_EQ(player->moves[0], MOVE_GROWL); EXPECT_EQ(player->moves[1], MOVE_NONE); }
}

SINGLE_BATTLE_TEST("Eruption Herald follows damaging sound moves once without recursive Eruptions")
{
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_HYPER_VOICE); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); HP_BAR(opponent); NOT HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Eruption Herald uses Eruption's normal current-HP damage formula")
{
    u32 hp;
    s16 calledDamage, directDamage;
    PARAMETRIZE { hp = 100; }
    PARAMETRIZE { hp = 400; }
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); HP(hp); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_GROWL); } TURN { MOVE(player, MOVE_ERUPTION); } }
    SCENE { HP_BAR(opponent, captureDamage: &calledDamage); HP_BAR(opponent, captureDamage: &directDamage); }
    THEN { EXPECT_EQ(calledDamage, directDamage); }
}

DOUBLE_BATTLE_TEST("Eruption Herald's extra Eruption hits both foes and never its ally")
{
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_GROWL); } }
    SCENE { ABILITY_POPUP(playerLeft, ABILITY_ERUPTION_HERALD); HP_BAR(opponentLeft); HP_BAR(opponentRight); NOT HP_BAR(playerRight); }
}

SINGLE_BATTLE_TEST("Eruption Herald does not activate when flinching or Throat Chop prevents a sound move")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FAKE_OUT; }
    PARAMETRIZE { move = MOVE_THROAT_CHOP; }
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); MovesWithPP({MOVE_GROWL, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(500); }
    } WHEN { TURN { MOVE(opponent, move); MOVE(player, MOVE_GROWL); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); }
    THEN { EXPECT_EQ(player->pp[0], 10); }
}

SINGLE_BATTLE_TEST("Eruption Herald still follows sound moves blocked by Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Ability(ABILITY_SOUNDPROOF); }
    } WHEN { TURN { MOVE(player, MOVE_GROWL); } }
    SCENE { ABILITY_POPUP(opponent, ABILITY_SOUNDPROOF); ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Eruption Herald still follows a missed sound move")
{
    GIVEN { PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_SING, hit: FALSE); } }
    SCENE { ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); MESSAGE("Entei used Eruption!"); }
}

SINGLE_BATTLE_TEST("Eruption Herald does not fire from a replacement after Parting Shot")
{
    GIVEN {
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_PARTING_SHOT); SEND_OUT(player, 1); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_ERUPTION_HERALD); }
}

SINGLE_BATTLE_TEST("Purifying Nomad cleanses its entire party on entry including bench statuses")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_FROSTBITE); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN | STATUS1_EMBLAZENED_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_PARALYSIS); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(player->hp, 100); EXPECT_EQ(player->status1, STATUS1_NONE);
        for (u32 slot = 0; slot < 4; slot++) EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_STATUS), STATUS1_NONE);
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
    }
}

DOUBLE_BATTLE_TEST("Purifying Nomad cures its active ally through Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SOUNDPROOF); Status1(STATUS1_BURN); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(playerRight->status1, STATUS1_NONE); EXPECT_EQ(playerRight->hp, 400); }
}

SINGLE_BATTLE_TEST("Purifying Nomad sets two turns of Tailwind only when switching out")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { SWITCH(player, 1); } }
    SCENE { ABILITY_POPUP(player, ABILITY_PURIFYING_NOMAD); }
    THEN { EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 1); EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND); }
}

SINGLE_BATTLE_TEST("Purifying Nomad does not set Tailwind when it faints")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); } }
    THEN { EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND)); }
}

SINGLE_BATTLE_TEST("Purifying Nomad does not shorten an existing Tailwind on exit")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_TAILWIND); } TURN { SWITCH(player, 1); } }
    THEN { EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 2); }
}

DOUBLE_BATTLE_TEST("Purifying Nomad preserves the correct switch-out battler when its Tailwind activates Wind Rider")
{
    GIVEN {
        PLAYER(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_BRAMBLEGHAST) { Ability(ABILITY_WIND_RIDER); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); } }
    THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(playerRight->species, SPECIES_BRAMBLEGHAST);
        EXPECT(!gBattleStruct->battlerState[B_BATTLER_2].notOnField);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 1);
    }
}

DOUBLE_BATTLE_TEST("Eruption Herald follows a Dancer-copied sound move during end-of-turn ability actions")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        PLAYER(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_BELLOSSOM) { Ability(ABILITY_ENDLESS_LUAU); HP(3000); MaxHP(3000); Speed(100); Moves(MOVE_CELEBRATE, MOVE_CLANGOROUS_SOUL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(opponentLeft, MOVE_CELEBRATE); } }
    SCENE { ABILITY_POPUP(playerRight, ABILITY_ERUPTION_HERALD); ANIMATION(ANIM_TYPE_MOVE, MOVE_ERUPTION, playerRight); }
    THEN {
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->hp, 2000);
        EXPECT_LT(opponentRight->hp, 3000);
        EXPECT(!gBattleStruct->endTurnAbilityMoveActive);
    }
}

SINGLE_BATTLE_TEST("Mountain Titan grants Fighting immunity to the whole Tyranitar line")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_LARVITAR; }
    PARAMETRIZE { species = SPECIES_PUPITAR; }
    PARAMETRIZE { species = SPECIES_TYRANITAR; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN { TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); } }
    SCENE { ABILITY_POPUP(player, ABILITY_MOUNTAIN_TITAN); NOT HP_BAR(player); }
    THEN { EXPECT_EQ(gBattleWeather, B_WEATHER_NONE); }
}

SINGLE_BATTLE_TEST("Mountain Titan retains Sand Spit after other damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_TYRANITAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_WATER_GUN); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_MOUNTAIN_TITAN); }
    THEN { EXPECT(gBattleWeather & B_WEATHER_SANDSTORM); }
}

SINGLE_BATTLE_TEST("Mountain Titan permits its own Fighting status moves")
{
    GIVEN { PLAYER(SPECIES_TYRANITAR) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(player, MOVE_BULK_UP); } }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("40-Day Storm doubles Flying damage in rain", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_LUGIA) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_PRESSURE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_GUST); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[apex].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(2), results[1].damage); }
}

SINGLE_BATTLE_TEST("40-Day Storm summons endless rain before bypassing Protect with its Flying move")
{
    GIVEN {
        PLAYER(SPECIES_LUGIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GUST); } TURN {} TURN {} TURN {} TURN {} TURN {} }
    SCENE { HP_BAR(opponent); }
    THEN { EXPECT(gBattleWeather & B_WEATHER_RAIN); EXPECT_EQ(gBattleStruct->weatherDuration, 0); }
}

SINGLE_BATTLE_TEST("40-Day Storm upgrades temporary rain and can be overwritten afterward")
{
    GIVEN {
        PLAYER(SPECIES_LUGIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_GUST); } TURN { MOVE(opponent, MOVE_SUNNY_DAY); } }
    THEN { EXPECT(gBattleWeather & B_WEATHER_SUN); EXPECT_GT(gBattleStruct->weatherDuration, 0); }
}

SINGLE_BATTLE_TEST("40-Day Storm respects Cloud Nine and primal weather when checking protection")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CLOUD_NINE; }
    PARAMETRIZE { ability = ABILITY_DESOLATE_LAND; }
    GIVEN {
        PLAYER(SPECIES_LUGIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GUST); } }
    SCENE { NOT HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Golden Rainbow revives an ally to full HP on entry")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    SCENE { ABILITY_POPUP(player, ABILITY_GOLDEN_RAINBOW); }
    THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 400); EXPECT(GetBattlerPartyState(B_BATTLER_0)->goldenRainbowUsed); }
}

SINGLE_BATTLE_TEST("Golden Rainbow stays available when there is no fainted ally")
{
    GIVEN { PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); } PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_GOLDEN_RAINBOW); }
    THEN { EXPECT(!GetBattlerPartyState(B_BATTLER_0)->goldenRainbowUsed); }
}

SINGLE_BATTLE_TEST("Golden Rainbow is once per holder per battle across switches")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); MaxHP(40); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE { ABILITY_POPUP(player, ABILITY_GOLDEN_RAINBOW); NOT ABILITY_POPUP(player, ABILITY_GOLDEN_RAINBOW); }
    THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HP), 0); }
}

SINGLE_BATTLE_TEST("Golden Rainbow can trigger on a later entry when an ally has since fainted")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(40); MaxHP(40); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 2); }
        TURN { SWITCH(player, 0); }
    } THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 40); EXPECT(GetBattlerPartyState(B_BATTLER_0)->goldenRainbowUsed); }
}

SINGLE_BATTLE_TEST("Golden Rainbow can choose either fainted ally while leaving the other fainted")
{
    u32 pick;
    PARAMETRIZE { pick = 0; }
    PARAMETRIZE { pick = 1; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(0); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); MaxHP(400); }
        PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_GOLDEN_RAINBOW, pick)); SEND_OUT(player, 3); } }
    THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][pick + 1], MON_DATA_HP), 400);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2 - pick], MON_DATA_HP), 0);
    }
}

DOUBLE_BATTLE_TEST("Golden Rainbow can restore a fainted ally into an empty doubles position")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(500); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(opponentLeft, MOVE_SURF, WITH_RNG(RNG_GOLDEN_RAINBOW, 1)); SEND_OUT(playerLeft, 2); } }
    THEN { EXPECT_EQ(playerLeft->species, SPECIES_HO_OH); EXPECT_EQ(playerRight->hp, 400); EXPECT(!(gAbsentBattlerFlags & (1u << B_BATTLER_2))); }
}

AI_SINGLE_BATTLE_TEST("Thunder Runner AI accounts for its terrain Speed and Evasion stages")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RAIKOU) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_ELECTRIC_SURGE); Speed(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(gAiLogicData->speedStats[B_BATTLER_0], 150); EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], 75); }
}

AI_SINGLE_BATTLE_TEST("Mountain Titan AI rejects Fighting attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_TYRANITAR) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CLOSE_COMBAT); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_CLOSE_COMBAT); } }
}

AI_SINGLE_BATTLE_TEST("Eruption Herald AI values a sound move's Eruption follow-up")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ENTEI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_GROWL, MOVE_CELEBRATE); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_GROWL, MOVE_CELEBRATE); } }
}

AI_SINGLE_BATTLE_TEST("40-Day Storm AI forecasts its own rain and Protect bypass without changing real weather")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_LUGIA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        u32 weather = gBattleWeather;
        struct AiCalcValues calc = { .move = MOVE_GUST, .weather = B_WEATHER_NONE };
        struct SimulatedDamage empowered = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        EXPECT(AI_CanBypassProtect(B_BATTLER_0, B_BATTLER_1, MOVE_GUST));
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_NONE;
        struct SimulatedDamage normal = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_40_DAY_STORM;
        EXPECT_MUL_EQ(normal.median, Q_4_12(2), empowered.median);
        EXPECT_EQ(gBattleWeather, weather);
    }
}

AI_DOUBLE_BATTLE_TEST("Purifying Nomad and Golden Rainbow forecasts do not cure or revive real party members")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(800); Status1(STATUS1_PARALYSIS); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SUICUNE) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_BURN); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_HO_OH) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(0); Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_FORCED);
        EXPECT_EQ(opponentRight->hp, 400); EXPECT(opponentRight->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_STATUS), STATUS1_BURN);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][4], MON_DATA_HP), 0);
        EXPECT(!gBattleStruct->partyState[B_TRAINER_OPPONENT_A][3].goldenRainbowUsed);
    }
}
