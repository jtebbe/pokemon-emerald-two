#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Scrap Brawler is exclusive to Galarian Meowth and Perrserker with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_MEOWTH_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERRSERKER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_PERRSERKER; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_MEOWTH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_MEOWTH_ALOLA; item = ITEM_APEX_METEORITE; }
    GIVEN { PLAYER(species) { Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_SCRAP_BRAWLER, item == ITEM_APEX_METEORITE && (species == SPECIES_MEOWTH_GALAR || species == SPECIES_PERRSERKER)); }
}

SINGLE_BATTLE_TEST("Scrap Brawler includes Tough Claws only for contact moves", s16 damage)
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SWIFT; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_SWIFT; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Ability(ABILITY_BATTLE_ARMOR); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_HEAVY_DUTY_BOOTS); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage); EXPECT_EQ(results[2].damage, results[3].damage); }
}

SINGLE_BATTLE_TEST("Scrap Brawler randomly sets Spikes or Stealth Rock with equal odds")
{
    PASSES_RANDOMLY(1, 2, RNG_SCRAP_BRAWLER);
    GIVEN {
        PLAYER(SPECIES_MEOWTH_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SWIFT); } }
    SCENE { ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); MESSAGE("Spikes were scattered on the ground all around the opposing side!"); }
}

SINGLE_BATTLE_TEST("Scrap Brawler activates its newly set hazard immediately after contact damage", s16 hazardDamage)
{
    u32 roll;
    PARAMETRIZE { roll = 0; }
    PARAMETRIZE { roll = 1; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, roll)); } }
    SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        HP_BAR(opponent, captureDamage: &results[i].hazardDamage);
        if (roll == 0) MESSAGE("The opposing Wobbuffet was hurt by the spikes!");
        else MESSAGE("Pointed stones dug into the opposing Wobbuffet!");
    } THEN {
        EXPECT_EQ(results[i].hazardDamage, 100);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, roll == 0);
        EXPECT_EQ(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK), roll == 1);
    }
}

SINGLE_BATTLE_TEST("Scrap Brawler non-contact attacks set hazards without immediately activating them", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_MEOWTH_GALAR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(player, MOVE_SWIFT, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); NOT HP_BAR(opponent); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1); EXPECT_EQ(opponent->hp, 800 - results[i].damage); }
}

SINGLE_BATTLE_TEST("Scrap Brawler respects Heavy-Duty Boots Magic Guard and suppressed items", s16 hazardDamage)
{
    enum Item item;
    enum Ability ability;
    bool32 magicRoom;
    PARAMETRIZE { item = ITEM_HEAVY_DUTY_BOOTS; ability = ABILITY_NONE; magicRoom = FALSE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_MAGIC_GUARD; magicRoom = FALSE; }
    PARAMETRIZE { item = ITEM_HEAVY_DUTY_BOOTS; ability = ABILITY_KLUTZ; magicRoom = FALSE; }
    PARAMETRIZE { item = ITEM_HEAVY_DUTY_BOOTS; ability = ABILITY_NONE; magicRoom = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Ability(ability); HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { if (magicRoom) MOVE(opponent, MOVE_MAGIC_ROOM); MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        if (magicRoom || ability == ABILITY_KLUTZ) HP_BAR(opponent, captureDamage: &results[i].hazardDamage);
        else NOT HP_BAR(opponent);
    } THEN { EXPECT_EQ(results[i].hazardDamage, magicRoom || ability == ABILITY_KLUTZ ? 375 : 0); }
}

SINGLE_BATTLE_TEST("Scrap Brawler respects grounding for Spikes but Flying targets still take Stealth Rock", s16 hazardDamage)
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    u32 roll;
    PARAMETRIZE { species = SPECIES_TAILLOW; ability = ABILITY_NONE; item = ITEM_NONE; roll = 0; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_LEVITATE; item = ITEM_NONE; roll = 0; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; item = ITEM_AIR_BALLOON; roll = 0; }
    PARAMETRIZE { species = SPECIES_TAILLOW; ability = ABILITY_NONE; item = ITEM_NONE; roll = 1; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); Item(item); HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, roll)); } }
    SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        // Air Balloon is popped by the attack, after this immediate activation.
        if (roll == 1) HP_BAR(opponent, captureDamage: &results[i].hazardDamage);
        else NOT MESSAGE("The opposing Wobbuffet was hurt by the spikes!");
    } THEN { EXPECT_EQ(results[i].hazardDamage, roll == 1 ? 200 : 0); }
}

SINGLE_BATTLE_TEST("Scrap Brawler does not set hazards after status moves misses Protect or type immunity")
{
    u32 block;
    PARAMETRIZE { block = 0; }
    PARAMETRIZE { block = 1; }
    PARAMETRIZE { block = 2; }
    PARAMETRIZE { block = 3; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(block == 3 ? SPECIES_GASTLY : SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(player, block == 0 ? MOVE_GROWL : block == 1 ? MOVE_METAL_CLAW : MOVE_SCRATCH, hit: block != 1); if (block == 2) MOVE(opponent, MOVE_PROTECT); } }
    SCENE { NOT ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); }
    THEN { EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 0); }
}

SINGLE_BATTLE_TEST("Scrap Brawler stacks Spikes to three and still activates capped hazards")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); }
    } WHEN { for (u32 i = 0; i < 4; i++) TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE {
        for (u32 i = 0; i < 3; i++) { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent); }
        HP_BAR(opponent); NOT ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent);
    } THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3); EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 1); }
}

SINGLE_BATTLE_TEST("Scrap Brawler triggers all existing damaging hazards as well as the new layer", s16 spikes, s16 rocks)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        HP_BAR(opponent, captureDamage: &results[i].rocks);
        MESSAGE("Pointed stones dug into the opposing Wobbuffet!");
        HP_BAR(opponent, captureDamage: &results[i].spikes);
        MESSAGE("The opposing Wobbuffet was hurt by the spikes!");
    } THEN { EXPECT_EQ(results[i].rocks, 375); EXPECT_EQ(results[i].spikes, 375); }
}

SINGLE_BATTLE_TEST("Scrap Brawler activates Sticky Web and Toxic Spikes on a contact hit")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        MESSAGE("The opposing Wobbuffet was badly poisoned!");
        STATUS_ICON(opponent, badPoison: TRUE);
        MESSAGE("The opposing Wobbuffet was caught in a sticky web!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        HP_BAR(opponent);
    } THEN { EXPECT_EQ(opponent->status1 & STATUS1_TOXIC_POISON, STATUS1_TOXIC_POISON); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1); }
}

SINGLE_BATTLE_TEST("Scrap Brawler allows Poison targets to absorb Toxic Spikes without skipping the next hazard")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GRIMER) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); }
    } SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); MESSAGE("The toxic spikes disappeared from the ground around the opposing side!"); HP_BAR(opponent); MESSAGE("The opposing Grimer was hurt by the spikes!"); }
    THEN { EXPECT_EQ(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_TOXIC_SPIKES), FALSE); EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1); }
}

SINGLE_BATTLE_TEST("Scrap Brawler activates after every contact strike")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent); HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 2); }
}

SINGLE_BATTLE_TEST("Scrap Brawler hazard damage can knock out the target and stop subsequent strikes")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(1); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(40); MaxHP(800); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent); MESSAGE("The opposing Wobbuffet fainted!"); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1); }
}

SINGLE_BATTLE_TEST("Scrap Brawler sets hazards on a direct knockout without damaging the fainted target again")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_HEAVY_DUTY_BOOTS); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); MESSAGE("The opposing Wobbuffet fainted!"); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1); }
}

SINGLE_BATTLE_TEST("Scrap Brawler works through Substitute and does not require a secondary effect", s16 hazardDamage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Speed(1); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Item(ITEM_COVERT_CLOAK); HP(800); MaxHP(800); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE { HP_BAR(opponent); SUB_HIT(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent, captureDamage: &results[i].hazardDamage); }
    THEN { EXPECT_EQ(results[i].hazardDamage, 100); EXPECT(opponent->volatiles.substitute); }
}

DOUBLE_BATTLE_TEST("Scrap Brawler sets hazards independently for spread targets without immediately activating non-contact hits")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_SWIFT, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); } }
    SCENE { HP_BAR(opponentLeft); HP_BAR(opponentRight); ABILITY_POPUP(playerLeft, ABILITY_SCRAP_BRAWLER); ABILITY_POPUP(playerLeft, ABILITY_SCRAP_BRAWLER); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 2); EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].spikesAmount, 0); }
}

SINGLE_BATTLE_TEST("Scrap Brawler activates Frigid Spikes and inflicts frostbite", s16 frigidDamage)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER);
        HP_BAR(opponent, captureDamage: &results[i].frigidDamage);
        MESSAGE("The opposing Wobbuffet was hurt by the frigid spikes!");
        MESSAGE("The opposing Wobbuffet got frostbite!");
        STATUS_ICON(opponent, frostbite: TRUE);
        HP_BAR(opponent);
    } THEN { EXPECT_EQ(results[i].frigidDamage, 187); EXPECT_EQ(opponent->status1, STATUS1_FROSTBITE); }
}

SINGLE_BATTLE_TEST("Scrap Brawler does not activate status hazards against Boots or Magic Guard")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_HEAVY_DUTY_BOOTS; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); }
    } SCENE { NOT MESSAGE("The opposing Wobbuffet was caught in a sticky web!"); }
    THEN { EXPECT_EQ(opponent->status1, STATUS1_NONE); EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE); }
}

DOUBLE_BATTLE_TEST("Scrap Brawler does not force hazard effects on its ally")
{
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SPIKES); }
        TURN { MOVE(playerLeft, MOVE_SCRATCH, target: playerRight); }
    } SCENE { NOT ABILITY_POPUP(playerLeft, ABILITY_SCRAP_BRAWLER); }
    THEN { EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].spikesAmount, 1); EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0); }
}

SINGLE_BATTLE_TEST("Scrap Brawler contact hazards finish off targets saved by Focus Sash or Sturdy")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_STURDY; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_FOCUS_SASH; }
    GIVEN {
        PLAYER(SPECIES_PERRSERKER) { Item(ITEM_APEX_METEORITE); Attack(4000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(100); MaxHP(100); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_HEAVY_DUTY_BOOTS); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_SCRAP_BRAWLER, 0)); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(player, ABILITY_SCRAP_BRAWLER); HP_BAR(opponent); MESSAGE("The opposing Wobbuffet fainted!"); }
}
