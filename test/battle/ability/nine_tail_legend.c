#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_controllers.h"

SINGLE_BATTLE_TEST("Nine-Tail Legend belongs only to non-Alolan Vulpix and Ninetales")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_VULPIX; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_VULPIX_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_NINETALES; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_NINE_TAIL_LEGEND,
                  item == ITEM_APEX_METEORITE && (species == SPECIES_VULPIX || species == SPECIES_NINETALES));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend permanently seals only the first damaging move of an opponent")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_POUND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_POUND); }
        for (u32 turn = 0; turn < 9; turn++) TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_POUND); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        MESSAGE("The opposing Wobbuffet's Scratch was sealed for the rest of the battle!");
        NOT ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_POUND));
        EXPECT_EQ((enum Move)opponent->volatiles.disabledMove, MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend keeps the seal and first-hit history when either Pokemon switches")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_POUND, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_POUND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_POUND); }
        TURN { MOVE(opponent, MOVE_POUND, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 0); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_POUND); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        MESSAGE("The opposing Wobbuffet's Scratch was sealed for the rest of the battle!");
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        MESSAGE("The opposing Wynaut's Pound was sealed for the rest of the battle!");
        NOT ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_POUND));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend gives each holder an independent seal against the same attacker")
{
    GIVEN {
        PLAYER(SPECIES_VULPIX) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_POUND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_POUND); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_POUND, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_POUND));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend does not spend its trigger on a miss or Protect")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER, MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        if (protect) TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_THUNDER); }
        else TURN { MOVE(opponent, MOVE_THUNDER, hit: FALSE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_WATER_GUN));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_THUNDER));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend does not seal attacks absorbed by the holder's Substitute")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
    } THEN {
        EXPECT(!IsNineTailLegendTriggered(B_BATTLER_1, B_BATTLER_0));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend does not seal an immune attack")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TRICK_OR_TREAT); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_WATER_GUN));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend seals noncontact attacks despite Mental Herb Aroma Veil and attacker Substitute")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_AROMA_VEIL; item = ITEM_MENTAL_HERB; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; item = ITEM_ABILITY_SHIELD; }
    PARAMETRIZE { ability = ABILITY_SHEER_FORCE; item = ITEM_PROTECTIVE_PADS; }
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); Moves(MOVE_SUBSTITUTE, MOVE_WATER_PULSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_WATER_PULSE); }
        TURN { MOVE(opponent, MOVE_WATER_PULSE, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_WATER_PULSE));
        EXPECT_EQ(opponent->item, item);
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend does not interrupt the remaining hits of a multistrike attack")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SKILL_LINK); Moves(MOVE_FURY_ATTACK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FURY_ATTACK); }
        TURN { MOVE(opponent, MOVE_FURY_ATTACK, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        HP_BAR(player);
        HP_BAR(player);
        HP_BAR(player);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend seals the move that calls a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); Moves(MOVE_SLEEP_TALK, MOVE_WATER_GUN, MOVE_FLY, MOVE_DIG); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SLEEP_TALK); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SLEEP_TALK));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_WATER_GUN));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend still seals an attack that knocks the holder out")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend leaves Struggle usable when the only move is sealed")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { FORCED_MOVE(opponent); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
    } THEN {
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_STRUGGLE));
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend raises the holder's critical-hit ratio by two stages across generations")
{
    u32 gen = GEN_9, passes = 1, trials = 2;
    PARAMETRIZE { gen = GEN_1; passes = 25; trials = 32; }
    PARAMETRIZE { gen = GEN_2; passes = 1; trials = 4; }
    PARAMETRIZE { gen = GEN_5; passes = 1; trials = 4; }
    PARAMETRIZE { gen = GEN_6; passes = 1; trials = 2; }
    PARAMETRIZE { gen = GEN_9; passes = 1; trials = 2; }
    PASSES_RANDOMLY(passes, trials, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, gen);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

DOUBLE_BATTLE_TEST("Nine-Tail Legend raises an ally's critical ratio and does not boost opponents")
{
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SLASH, target: opponentLeft); }
    } SCENE {
        MESSAGE("A critical hit!");
    } THEN {
        enum Ability abilities[MAX_BATTLERS_COUNT];
        for (u32 j = 0; j < gBattlersCount; j++) abilities[j] = GetBattlerAbility(j);
        EXPECT_EQ(GetNineTailLegendCritBoost(B_BATTLER_0, abilities), 2);
        EXPECT_EQ(GetNineTailLegendCritBoost(B_BATTLER_2, abilities), 2);
        EXPECT_EQ(GetNineTailLegendCritBoost(B_BATTLER_1, abilities), 0);
        EXPECT_EQ(GetNineTailLegendCritBoost(B_BATTLER_3, abilities), 0);
    }
}

DOUBLE_BATTLE_TEST("Nine-Tail Legend does not stack the aura of two holders")
{
    PASSES_RANDOMLY(1, 2, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_VULPIX) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend respects critical-hit protection")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_BATTLE_ARMOR; }
    PARAMETRIZE { ability = ABILITY_SHELL_ARMOR; }
    PARAMETRIZE { ability = ABILITY_BATTLE_KING; }
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend records a delayed attack against its original party member")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FUTURE_SIGHT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_FUTURE_SIGHT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(opponent, 1); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 0); }
        TURN { MOVE(opponent, MOVE_FUTURE_SIGHT, allowed: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NINE_TAIL_LEGEND);
        MESSAGE("The opposing Wobbuffet's Future Sight was sealed for the rest of the battle!");
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_FUTURE_SIGHT));
    }
}

DOUBLE_BATTLE_TEST("Nine-Tail Legend never seals a friendly attack")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: playerLeft); }
    } SCENE {
        NOT ABILITY_POPUP(playerLeft, ABILITY_NINE_TAIL_LEGEND);
    } THEN {
        EXPECT(!IsNineTailLegendTriggered(B_BATTLER_2, B_BATTLER_0));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_2, MOVE_SCRATCH));
    }
}

DOUBLE_BATTLE_TEST("Nine-Tail Legend's allied critical bonus ends when the holder leaves the field")
{
    bool32 faint;
    PARAMETRIZE { faint = FALSE; }
    PARAMETRIZE { faint = TRUE; }
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(1); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        if (faint) TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); SEND_OUT(playerLeft, 2); }
        else TURN { SWITCH(playerLeft, 2); }
    } THEN {
        enum Ability abilities[MAX_BATTLERS_COUNT];
        for (u32 j = 0; j < gBattlersCount; j++) abilities[j] = GetBattlerAbility(j);
        EXPECT_EQ(GetNineTailLegendCritBoost(B_BATTLER_2, abilities), 0);
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend's seal coexists with temporary Disable and survives its expiration")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_POUND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, MOVE_POUND); }
        TURN { MOVE(player, MOVE_DISABLE); MOVE(opponent, MOVE_CELEBRATE); }
        for (u32 turn = 0; turn < 9; turn++) TURN { MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SCRATCH, allowed: FALSE); MOVE(opponent, MOVE_POUND); }
    } THEN {
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_POUND));
        EXPECT_EQ((enum Move)opponent->volatiles.disabledMove, MOVE_NONE);
    }
}

DOUBLE_BATTLE_TEST("Nine-Tail Legend tracks both holders hit by a spread attack without duplicating the seal")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_VULPIX) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SURF, MOVE_SCRATCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SURF); }
        TURN { MOVE(opponentLeft, MOVE_SCRATCH, target: playerRight); }
    } THEN {
        EXPECT(IsNineTailLegendTriggered(B_BATTLER_1, B_BATTLER_0));
        EXPECT(IsNineTailLegendTriggered(B_BATTLER_1, B_BATTLER_2));
        EXPECT(IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SURF));
        EXPECT(!IsMoveNineTailLegendDisabled(B_BATTLER_1, MOVE_SCRATCH));
        EXPECT_EQ(gBattleStruct->nineTailLegend[GetBattlerTrainer(B_BATTLER_1)][gBattlerPartyIndexes[B_BATTLER_1]].disabledCount, 1);
    }
}

SINGLE_BATTLE_TEST("Nine-Tail Legend's critical bonus respects Lucky Chant")
{
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_NINETALES) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_LUCKY_CHANT); }
        TURN { MOVE(player, MOVE_SLASH); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    }
}
