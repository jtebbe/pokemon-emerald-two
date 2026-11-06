#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Glorious Mane is the Apex ability of Alolan Diglett and Dugtrio only")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_DIGLETT_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DIGLETT; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_DUGTRIO_ALOLA; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_GLORIOUS_MANE, item == ITEM_APEX_METEORITE && (species == SPECIES_DIGLETT_ALOLA || species == SPECIES_DUGTRIO_ALOLA)); }
}

SINGLE_BATTLE_TEST("Glorious Mane caps physical special fixed and OHKO attacks only at full HP", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    bool32 full = FALSE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_DRAGON_RAGE, MOVE_SEISMIC_TOSS, MOVE_FISSURE};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; full = FALSE; }
        PARAMETRIZE { move = moves[j]; full = TRUE; }
    }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); Speed(1); }
    } WHEN { TURN { MOVE(player, move, hit: TRUE, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { for (u32 j = 0; j < ARRAY_COUNT(moves); j++) { EXPECT_GT(results[j * 2].damage, 1); EXPECT_EQ(results[j * 2 + 1].damage, 1); } }
}

SINGLE_BATTLE_TEST("Glorious Mane does not cap later attacks after HP loss", s16 first, s16 second)
{
    PARAMETRIZE {}
    GIVEN { PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_SCRATCH); } TURN { MOVE(player, MOVE_SCRATCH); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].first); HP_BAR(opponent, captureDamage: &results[i].second); }
    THEN { EXPECT_EQ(results[i].first, 1); EXPECT_GT(results[i].second, 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane caps only the first body hit of a multihit attack", s16 first, s16 second)
{
    PARAMETRIZE {}
    GIVEN { PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_DOUBLE_KICK); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].first); HP_BAR(opponent, captureDamage: &results[i].second); }
    THEN { EXPECT_EQ(results[i].first, 1); EXPECT_GT(results[i].second, 1); EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane regains its damage cap after healing to full HP", s16 first, s16 second)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(opponent, MOVE_SCRATCH); } TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_SCRATCH); } }
    SCENE {
        HP_BAR(player, captureDamage: &results[i].first);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
        HP_BAR(player, captureDamage: &results[i].second);
    } THEN { EXPECT_EQ(results[i].first, 1); EXPECT_EQ(results[i].second, 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane does not reduce Stealth Rock or Spikes damage", s16 rock, s16 spikes)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1600); MaxHP(1600); }
    } WHEN { TURN { MOVE(player, MOVE_STEALTH_ROCK); } TURN { MOVE(player, MOVE_SPIKES); } TURN { SWITCH(opponent, 1); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].rock); HP_BAR(opponent, captureDamage: &results[i].spikes); }
    THEN { EXPECT_EQ(results[i].rock, 50); EXPECT_EQ(results[i].spikes, 200); EXPECT_EQ(opponent->hp, 1350); }
}

SINGLE_BATTLE_TEST("Glorious Mane does not reduce status damage and loses Dazzling afterward", s16 statusDamage, s16 attackDamage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(512); MaxHP(512); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} TURN { MOVE(opponent, MOVE_QUICK_ATTACK); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].statusDamage); ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent); HP_BAR(player, captureDamage: &results[i].attackDamage); }
    THEN { EXPECT_EQ(results[i].statusDamage, 32); EXPECT_GT(results[i].attackDamage, 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane does not reduce Leech Seed damage", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(512); MaxHP(512); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_LEECH_SEED); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN { EXPECT_EQ(results[i].damage, 64); }
}

SINGLE_BATTLE_TEST("Glorious Mane does not reduce recoil damage", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_EDGE); } }
    SCENE { HP_BAR(player, captureDamage: &results[i].damage); }
    THEN { EXPECT_GT(results[i].damage, 1); }
}

DOUBLE_BATTLE_TEST("Glorious Mane does not cap allied attacks but retains native Tangling Hair for that hit", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_SCRATCH, target: playerLeft); } }
    SCENE { HP_BAR(playerLeft, captureDamage: &results[0].damage); ABILITY_POPUP(playerLeft, ABILITY_GLORIOUS_MANE); }
    THEN { EXPECT_GT(results[0].damage, 1); EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane does not cap damage dealt to Substitute", u16 subDamage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(500); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_SUBSTITUTE); } TURN { MOVE(player, MOVE_RECOVER); } TURN { MOVE(opponent, MOVE_SCRATCH); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player); HP_BAR(player); SUB_HIT(player, captureDamage: &results[i].subDamage); }
    THEN { EXPECT_GT(results[i].subDamage, 1); EXPECT_EQ(player->hp, 800); EXPECT_LT((u32)player->volatiles.substituteHP, 199); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Glorious Mane blocks external drops to all seven battle stats only at full HP")
{
    enum Move move = MOVE_GROWL;
    enum Stat stat = STAT_ATK;
    u32 stages = 1;
    bool32 full = FALSE;
    const enum Move moves[] = {MOVE_GROWL, MOVE_TAIL_WHIP, MOVE_CONFIDE, MOVE_METAL_SOUND, MOVE_SCARY_FACE, MOVE_SMOKESCREEN, MOVE_SWEET_SCENT};
    const enum Stat stats[] = {STAT_ATK, STAT_DEF, STAT_SPATK, STAT_SPDEF, STAT_SPEED, STAT_ACC, STAT_EVASION};
    const u32 drops[] = {1, 1, 1, 2, 2, 1, 2};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; stat = stats[j]; stages = drops[j]; full = FALSE; }
        PARAMETRIZE { move = moves[j]; stat = stats[j]; stages = drops[j]; full = TRUE; }
    }
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, move, hit: TRUE); } }
    THEN { EXPECT_EQ(player->statStages[stat], full ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - stages); }
}

SINGLE_BATTLE_TEST("Glorious Mane permits self-inflicted stat drops at full HP")
{
    GIVEN { PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_SUPERPOWER); } }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane protects the first hit's secondary stat drop and permits a later hit's drop")
{
    GIVEN { PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(opponent, MOVE_TROP_KICK); } TURN { MOVE(opponent, MOVE_TROP_KICK); } }
    THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane's Tangling Hair responds only to hits received at full HP")
{
    bool32 full;
    PARAMETRIZE { full = FALSE; }
    PARAMETRIZE { full = TRUE; }
    GIVEN { PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_SCRATCH); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], full ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Glorious Mane's Tangling Hair respects contact protection")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_LONG_REACH; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_PROTECTIVE_PADS; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_PUNCHING_GLOVE; }
    GIVEN { PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); } OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, MOVE_FIRE_PUNCH, secondaryEffect: FALSE); } }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], ability == ABILITY_NONE && item == ITEM_NONE ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE); EXPECT_EQ(opponent->hp, 2999); }
}

SINGLE_BATTLE_TEST("Glorious Mane blocks damaging and Prankster status priority only at full HP")
{
    bool32 full;
    enum Move move;
    PARAMETRIZE { full = FALSE; move = MOVE_QUICK_ATTACK; }
    PARAMETRIZE { full = TRUE; move = MOVE_QUICK_ATTACK; }
    PARAMETRIZE { full = FALSE; move = MOVE_GROWL; }
    PARAMETRIZE { full = TRUE; move = MOVE_GROWL; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); }
        OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { if (full) { ABILITY_POPUP(opponent, ABILITY_GLORIOUS_MANE); NOT ANIMATION(ANIM_TYPE_MOVE, move, player); } else { ANIMATION(ANIM_TYPE_MOVE, move, player); } }
    THEN { if (full) EXPECT_EQ(opponent->hp, 3000); else if (move == MOVE_GROWL) EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1); else EXPECT_LT(opponent->hp, 2999); }
}

DOUBLE_BATTLE_TEST("Glorious Mane's Dazzling protects its partner from either allied slot only at full HP")
{
    bool32 full, right;
    PARAMETRIZE { full = FALSE; right = FALSE; }
    PARAMETRIZE { full = TRUE; right = FALSE; }
    PARAMETRIZE { full = FALSE; right = TRUE; }
    PARAMETRIZE { full = TRUE; right = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(right ? SPECIES_WOBBUFFET : SPECIES_DUGTRIO_ALOLA) { Item(right ? ITEM_NONE : ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
        OPPONENT(right ? SPECIES_DUGTRIO_ALOLA : SPECIES_WOBBUFFET) { Item(right ? ITEM_APEX_METEORITE : ITEM_NONE); HP(full ? 3000 : 2999); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: right ? opponentLeft : opponentRight); } }
    THEN { if (right) EXPECT_EQ(opponentLeft->hp == (full ? 3000 : 2999), full); else EXPECT_EQ(opponentRight->hp == (full ? 3000 : 2999), full); }
}

DOUBLE_BATTLE_TEST("Glorious Mane does not block allied priority attacks")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_QUICK_ATTACK, target: playerLeft); } }
    THEN { EXPECT_LT(playerLeft->hp, 2999); }
}

SINGLE_BATTLE_TEST("Glorious Mane restores Clear Body and Dazzling immediately after healing to full")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(2999); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_GROWL); } TURN { MOVE(opponent, MOVE_QUICK_ATTACK); } }
    THEN { EXPECT_EQ(player->hp, 3000); EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Glorious Mane's Apex protections survive Mold Breaker and its variants")
{
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_TERAVOLT; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_TURBOBLAZE; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; move = MOVE_QUICK_ATTACK; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; move = MOVE_GROWL; }
    GIVEN { PLAYER(SPECIES_WOBBUFFET) { Ability(ability); } OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, move); } }
    THEN { EXPECT_EQ(opponent->hp, move == MOVE_SCRATCH ? 2999 : 3000); EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); if (move == MOVE_SCRATCH) EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Glorious Mane preserves type immunities before applying the damage cap")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THUNDERBOLT; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; }
    GIVEN { PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); } }
    WHEN { TURN { MOVE(player, move); } }
    THEN { EXPECT_EQ(opponent->hp, 3000); }
}

SINGLE_BATTLE_TEST("Glorious Mane's capped OHKO damage still interrupts Focus Punch")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(player, MOVE_FOCUS_PUNCH); MOVE(opponent, MOVE_FISSURE, hit: TRUE); } }
    SCENE { HP_BAR(player, damage: 1); NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_PUNCH, player); }
    THEN { EXPECT_EQ(player->hp, 2999); EXPECT_EQ(opponent->hp, 3000); }
}
