#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Sickening Greed is the Apex ability of regular Meowth and Persian only")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_MEOWTH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERSIAN; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MEOWTH_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERSIAN_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MEOWTH_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERRSERKER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERSIAN; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SICKENING_GREED, item == ITEM_APEX_METEORITE && (species == SPECIES_MEOWTH || species == SPECIES_PERSIAN)); }
}

SINGLE_BATTLE_TEST("Sickening Greed raises all five core stats after Pay Day and still scatters coins")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_PAY_DAY); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player); HP_BAR(opponent); MESSAGE("Coins were scattered everywhere!"); ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed's Pay Day bonus also applies after misses Protect and immunity")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(block == 2 ? SPECIES_GASTLY : SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        if (block == 0) TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); }
        TURN { MOVE(player, MOVE_PAY_DAY, hit: block != 0); if (block == 1) MOVE(opponent, MOVE_PROTECT); }
    } SCENE { ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { EXPECT_EQ(opponent->hp, opponent->maxHP); for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sickening Greed gives no bonus when sleep paralysis or flinching prevents Pay Day")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(1); Status1(block == 0 ? STATUS1_SLEEP : block == 1 ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_PAY_DAY, WITH_RNG(RNG_PARALYSIS, TRUE)); if (block == 2) MOVE(opponent, MOVE_FAKE_OUT); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed does not boost after ordinary non-KO attacks")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_SCRATCH); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed raises all core stats after physical special and fixed damage knockouts")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_DRAGON_RAGE; }
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, move); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sickening Greed combines Pay Day and its knockout for two boosts")
{
    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_PAY_DAY); SEND_OUT(opponent, 1); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 2); }
}

DOUBLE_BATTLE_TEST("Sickening Greed rewards each direct knockout including an ally")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(3); }
        OPPONENT(SPECIES_WYNAUT) { Speed(4); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_EARTHQUAKE); SEND_OUT(opponentLeft, 2); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + 3); }
}

SINGLE_BATTLE_TEST("Sickening Greed stacks across repeated Pay Day uses and knockouts")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_PAY_DAY); SEND_OUT(opponent, 1); } TURN { MOVE(player, MOVE_PAY_DAY); SEND_OUT(opponent, 2); } TURN { MOVE(player, MOVE_PAY_DAY); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 5); }
}

SINGLE_BATTLE_TEST("Sickening Greed caps individual stats without preventing its other boosts")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_BELLY_DRUM); } TURN { MOVE(player, MOVE_PAY_DAY); } }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE); for (enum Stat stat = STAT_DEF; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sickening Greed stops boosting once all five core stats are capped")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); Defense(4000); } }
    WHEN { for (u32 turn = 0; turn < 7; turn++) TURN { MOVE(player, MOVE_PAY_DAY); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], MAX_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE); EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed does not increase the damage of the Pay Day that grants the boost", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN { PLAYER(SPECIES_PERSIAN) { Ability(ABILITY_LIMBER); Item(item); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_PAY_DAY, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { EXPECT_EQ(results[0].damage, results[1].damage); }
}

SINGLE_BATTLE_TEST("Sickening Greed boosts Pay Day damage starting with the next use", s16 first, s16 second)
{
    PARAMETRIZE {}
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Attack(300); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); } }
    WHEN { TURN { MOVE(player, MOVE_PAY_DAY, criticalHit: FALSE); } TURN { MOVE(player, MOVE_PAY_DAY, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].first); HP_BAR(opponent, captureDamage: &results[i].second); }
    THEN { EXPECT_GT(results[i].second, results[i].first); }
}

SINGLE_BATTLE_TEST("Sickening Greed grants one knockout boost after an entire multihit attack")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Level(50); Attack(300); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(100); Defense(100); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_HIT, criticalHit: FALSE); SEND_OUT(opponent, 1); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

DOUBLE_BATTLE_TEST("Sickening Greed does not boost when an ally causes the knockout")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_WYNAUT) { Speed(4); }
    } WHEN { TURN { MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft); SEND_OUT(opponentLeft, 2); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed does not trigger on a residual poison knockout")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SEND_OUT(opponent, 1); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Sickening Greed rewards Pay Day called by Metronome")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_METRONOME, WITH_RNG(RNG_METRONOME, MOVE_PAY_DAY)); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

DOUBLE_BATTLE_TEST("Sickening Greed rewards each Pay Day use called by Instruct")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_ORANGURU) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_PAY_DAY, target: opponentLeft); MOVE(playerRight, MOVE_INSTRUCT, target: playerLeft); } }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("Sickening Greed does not boost a user knocked out by contact damage")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); HP(1); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Item(ITEM_ROCKY_HELMET); Speed(2); }
        OPPONENT(SPECIES_WYNAUT) { Speed(3); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); SEND_OUT(opponent, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player); NOT ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
}

SINGLE_BATTLE_TEST("Sickening Greed skips boosts after the final opposing Pokemon is knocked out")
{
    GIVEN { PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(100); } OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); } }
    WHEN { TURN { MOVE(player, MOVE_PAY_DAY); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
}

SINGLE_BATTLE_TEST("Sickening Greed rewards Pay Day against a Substitute without counting it as a knockout")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_PAY_DAY); } }
    SCENE { SUB_HIT(opponent); ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Sickening Greed does not give a forced-in replacement the previous user's Pay Day boost")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_MEOWTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_PAY_DAY); } }
    SCENE { MESSAGE("Meowth was dragged out!"); NOT ABILITY_POPUP(player, ABILITY_SICKENING_GREED); }
    THEN { EXPECT_EQ(player->species, SPECIES_MEOWTH); for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE); }
}
