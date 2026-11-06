#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Sticky Spray sets opposing Sticky Web on entry for each eligible species on either side")
{
    enum Species species = SPECIES_CATERPIE;
    bool32 opposing = FALSE;
    for (u32 i = SPECIES_CATERPIE; i <= SPECIES_KAKUNA; i++)
    {
        if (i == SPECIES_BUTTERFREE)
            continue;
        PARAMETRIZE { species = i; opposing = FALSE; }
        PARAMETRIZE { species = i; opposing = TRUE; }
    }
    GIVEN {
        PLAYER(opposing ? SPECIES_WOBBUFFET : species) { Speed(opposing ? 100 : 10); Item(opposing ? ITEM_NONE : ITEM_APEX_METEORITE); }
        OPPONENT(opposing ? species : SPECIES_WOBBUFFET) { Speed(opposing ? 10 : 100); Item(opposing ? ITEM_APEX_METEORITE : ITEM_NONE); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(opposing ? opponent : player, ABILITY_STICKY_SPRAY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, opposing ? opponent : player);
        if (opposing)
            MESSAGE("A sticky web has been laid out on the ground on your side!");
        else
            MESSAGE("A sticky web has been laid out on the ground on the opposing side!");
    } THEN {
        enum BattleSide targetSide = opposing ? B_SIDE_PLAYER : B_SIDE_OPPONENT;
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_STICKY_SPRAY);
        EXPECT(IsHazardOnSide(targetSide, HAZARDS_STICKY_WEB));
        EXPECT(!IsHazardOnSide(targetSide ^ BIT_SIDE, HAZARDS_STICKY_WEB));
        EXPECT_EQ(gBattleStruct->numHazards[targetSide], 1);
        EXPECT_EQ(gSideTimers[targetSide].stickyWebBattlerId, opposing ? B_BATTLER_1 : B_BATTLER_0);
        // The opposing entrant has already completed its switch-in events.
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sticky Spray requires an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Ability(ABILITY_SHIELD_DUST); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BUG_BITE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        HP_BAR(player);
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STICKY_WEB));
    }
}

SINGLE_BATTLE_TEST("Sticky Spray blocks Poison and Bug attacks and targeted status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_POISON_JAB; }
    PARAMETRIZE { move = MOVE_SLUDGE_BOMB; }
    PARAMETRIZE { move = MOVE_BUG_BITE; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; }
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_POISON_GAS; }
    PARAMETRIZE { move = MOVE_STRING_SHOT; }
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        // Entry activation followed by the move immunity.
        ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        MESSAGE("It doesn't affect Caterpie…");
        NONE_OF {
            HP_BAR(player);
            STATUS_ICON(player, poison: TRUE);
            STATUS_ICON(player, badPoison: TRUE);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->status1, 0);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Sticky Spray cannot be bypassed by Mold Breaker, Omnipotence or Neutralizing Gas")
{
    enum Species attacker;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { attacker = SPECIES_HAXORUS; ability = ABILITY_MOLD_BREAKER; item = ITEM_NONE; }
    PARAMETRIZE { attacker = SPECIES_WEEZING; ability = ABILITY_NEUTRALIZING_GAS; item = ITEM_NONE; }
    PARAMETRIZE { attacker = SPECIES_ARCEUS; ability = ABILITY_MULTITYPE; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(attacker) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SLUDGE_BOMB); }
    } SCENE {
        MESSAGE("It doesn't affect Caterpie…");
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_STICKY_SPRAY);
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STICKY_WEB));
    }
}

SINGLE_BATTLE_TEST("Sticky Spray stays grounded and can be poisoned by Toxic Spikes")
{
    ResetStartingStatuses();
    SetStartingStatus(STARTING_STATUS_TOXIC_SPIKES_PLAYER_L1);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        STATUS_ICON(player, poison: TRUE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT(player->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Sticky Spray does not block opposing field hazards")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_STICKY_WEB; }
    PARAMETRIZE { move = MOVE_TOXIC_SPIKES; }
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, move == MOVE_STICKY_WEB ? HAZARDS_STICKY_WEB : HAZARDS_TOXIC_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Sticky Spray webs affect grounded replacements and respect Boots and Flying")
{
    enum Species replacement;
    enum Item item;
    PARAMETRIZE { replacement = SPECIES_WYNAUT; item = ITEM_NONE; }
    PARAMETRIZE { replacement = SPECIES_WYNAUT; item = ITEM_HEAVY_DUTY_BOOTS; }
    PARAMETRIZE { replacement = SPECIES_PIDGEY; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_METAPOD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(replacement) { Item(item); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - (replacement == SPECIES_WYNAUT && item == ITEM_NONE));
    }
}

SINGLE_BATTLE_TEST("Sticky Spray does not stack webs and restores removed webs only on reentry")
{
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_METAPOD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_RAPID_SPIN); }
        TURN {}
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(player, 0); }
        TURN { SWITCH(opponent, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        MESSAGE("A sticky web has been laid out on the ground on the opposing side!");
        SEND_IN_MESSAGE("Metapod");
        NOT ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        MESSAGE("The opposing Wobbuffet used Rapid Spin!");
        NOT ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        MESSAGE("2 sent out Wynaut!");
        SEND_IN_MESSAGE("Caterpie");
        ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        MESSAGE("A sticky web has been laid out on the ground on the opposing side!");
        MESSAGE("2 sent out Wobbuffet!");
        MESSAGE("The opposing Wobbuffet's Speed fell!");
    } THEN {
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Sticky Spray sets one web per side in doubles and cannot be bounced")
{
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_METAPOD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_HATTERENE) { Ability(ABILITY_MAGIC_BOUNCE); }
        OPPONENT(SPECIES_KAKUNA) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STICKY_WEB));
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STICKY_WEB));
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_PLAYER], 1);
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 1);
    }
}

SINGLE_BATTLE_TEST("Sticky Spray webs retain their source for Mirror Armor")
{
    GIVEN {
        WITH_CONFIG(B_MIRROR_ARMOR_STICKY_WEB, GEN_8);
        PLAYER(SPECIES_WEEDLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CORVIKNIGHT) { Ability(ABILITY_MIRROR_ARMOR); Item(ITEM_IRON_BALL); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MIRROR_ARMOR);
        MESSAGE("Weedle's Speed fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sticky Spray checks the actual move type after Normalize")
{
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_DELCATTY) { Ability(ABILITY_NORMALIZE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BUG_BUZZ); }
    } SCENE {
        MESSAGE("The opposing Delcatty used Bug Buzz!");
        NOT ABILITY_POPUP(player, ABILITY_STICKY_SPRAY);
        HP_BAR(player);
    }
}

DOUBLE_BATTLE_TEST("Sticky Spray blocks an ally's Poison spread attack without protecting the other targets")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_CATERPIE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SLUDGE_WAVE); }
    } SCENE {
        MESSAGE("Wobbuffet used Sludge Wave!");
        ABILITY_POPUP(playerRight, ABILITY_STICKY_SPRAY);
        MESSAGE("It doesn't affect Caterpie…");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_WAVE, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        NOT HP_BAR(playerRight);
    }
}
