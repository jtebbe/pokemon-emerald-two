#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Tunnel Trouble is the Apex ability of regular Diglett and Dugtrio only")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_DIGLETT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DIGLETT_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_TUNNEL_TROUBLE, item == ITEM_APEX_METEORITE && (species == SPECIES_DIGLETT || species == SPECIES_DUGTRIO)); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble switches after physical special fixed and trapping Ground attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; }
    PARAMETRIZE { move = MOVE_FISSURE; }
    PARAMETRIZE { move = MOVE_SAND_TOMB; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, hit: TRUE); SEND_OUT(player, 1); if (move == MOVE_FISSURE) SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE);
        SEND_IN_MESSAGE("Wynaut");
    } THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 1); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble switches after successful Ground status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { move = MOVE_SPIKES; }
    PARAMETRIZE { move = MOVE_SHORE_UP; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); HP(50); MaxHP(100); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(player, move); SEND_OUT(player, 1); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE);
        SEND_IN_MESSAGE("Wynaut");
    } THEN {
        if (move == MOVE_SHORE_UP) EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 100);
        if (move == MOVE_SPIKES) EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Tunnel Trouble skips both turns of Dig even with a Power Herb")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { item = ITEM_POWER_HERB; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_TUNNEL_TROUBLE); Item(item); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DIG); }
        if (item != ITEM_POWER_HERB) TURN { SKIP_TURN(player); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIG, player);
        NOT ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE);
    } THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not switch after other move types")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_ROCK_SLIDE; }
    PARAMETRIZE { move = MOVE_SWORDS_DANCE; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble follows a move's dynamic Ground type")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_EARTH_PLATE; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_TUNNEL_TROUBLE); Item(item); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_JUDGMENT); if (item == ITEM_EARTH_PLATE) SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], item == ITEM_EARTH_PLATE ? 1 : 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not switch after a miss Protect or Ground immunity")
{
    u32 block = 0;
    for (u32 j = 0; j < 6; j++) PARAMETRIZE { block = j; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(block == 2 ? SPECIES_PIDGEY : SPECIES_WOBBUFFET) { Ability(block == 3 ? ABILITY_LEVITATE : block == 4 ? ABILITY_EARTH_EATER : ABILITY_NONE); Item(block == 5 ? ITEM_AIR_BALLOON : ITEM_NONE); HP(1000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, block == 0 ? MOVE_MUD_BOMB : MOVE_EARTH_POWER, hit: block != 0); if (block == 1) MOVE(opponent, MOVE_PROTECT); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not switch when the user cannot act")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_PARALYSIS); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_PARALYSIS, TRUE)); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not switch after a failed Ground status move")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_SHORE_UP); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble switches after damaging a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_EARTH_POWER); SEND_OUT(player, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, player); ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE); SEND_IN_MESSAGE("Wynaut"); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble has no effect without a living replacement")
{
    bool32 faintedBench;
    PARAMETRIZE { faintedBench = FALSE; }
    PARAMETRIZE { faintedBench = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        if (faintedBench) PLAYER(SPECIES_WYNAUT) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_EARTH_POWER); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, player); NOT ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE); }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not switch when the final opposing Pokemon faints")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_EARTH_POWER); } }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble escapes trapping effects like U-turn")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_MEAN_LOOK; }
    PARAMETRIZE { move = MOVE_BIND; }
    PARAMETRIZE { move = MOVE_INGRAIN; }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { if (move == MOVE_INGRAIN) MOVE(player, move); else MOVE(opponent, move); }
        TURN { MOVE(player, MOVE_EARTH_POWER); SEND_OUT(player, 1); }
    } SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, player); SEND_IN_MESSAGE("Wynaut"); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble switches once after all hits of a multihit attack")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_BONEMERANG); SEND_OUT(player, 1); } }
    SCENE { HP_BAR(opponent); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE); SEND_IN_MESSAGE("Wynaut"); }
}

DOUBLE_BATTLE_TEST("Tunnel Trouble switches once after damaging all Earthquake targets")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT) { Speed(3); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(4); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); SEND_OUT(playerLeft, 2); } }
    SCENE { HP_BAR(opponentLeft); HP_BAR(playerRight); HP_BAR(opponentRight); ABILITY_POPUP(playerLeft, ABILITY_TUNNEL_TROUBLE); SEND_IN_MESSAGE("Wynaut"); }
}

DOUBLE_BATTLE_TEST("Tunnel Trouble can pivot when only one spread target is vulnerable")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_PIDGEY);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_PIDGEY);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); SEND_OUT(playerLeft, 2); } }
    SCENE { HP_BAR(opponentRight); ABILITY_POPUP(playerLeft, ABILITY_TUNNEL_TROUBLE); SEND_IN_MESSAGE("Wynaut"); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble's replacement receives attacks that have not happened yet")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_EARTH_POWER); MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, player); SEND_IN_MESSAGE("Wynaut"); ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent); HP_BAR(player); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not pass stat boosts or Substitute to its replacement")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SWORDS_DANCE); } TURN { MOVE(player, MOVE_SUBSTITUTE); } TURN { MOVE(player, MOVE_EARTH_POWER); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); EXPECT(!player->volatiles.substitute); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble follows U-turn's Eject Button timing in each ruleset")
{
    u32 gen;
    PARAMETRIZE { gen = GEN_9; }
    PARAMETRIZE { gen = GEN_CHAMPIONS; }
    GIVEN {
        WITH_CONFIG(B_QUEUED_SWITCH_TIMINGS, gen);
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_EJECT_BUTTON); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_EARTH_POWER); SEND_OUT(opponent, 1); if (gen == GEN_CHAMPIONS) SEND_OUT(player, 1); }
    } THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], gen == GEN_CHAMPIONS ? 1 : 0); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not activate for a replacement forced in by Red Card")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_DIGLETT) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_EARTH_POWER); } }
    SCENE { ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent); MESSAGE("Diglett was dragged out!"); NOT ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE); }
    THEN { EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_0], 1); }
}

SINGLE_BATTLE_TEST("Tunnel Trouble does not activate if contact damage knocks out its user")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DRILL_RUN); SEND_OUT(player, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_DRILL_RUN, player); NOT ABILITY_POPUP(player, ABILITY_TUNNEL_TROUBLE); }
    THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0); }
}
