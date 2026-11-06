#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Mental Master is the Abra line's Apex ability")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_ABRA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_KADABRA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ABRA; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_MENTAL_MASTER, item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Mental Master reveals one to four known moves and the foe's item and ability")
{
    u32 count;
    PARAMETRIZE { count = 1; }
    PARAMETRIZE { count = 2; }
    PARAMETRIZE { count = 3; }
    PARAMETRIZE { count = 4; }
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Item(ITEM_LEFTOVERS); Ability(ABILITY_SHADOW_TAG);
            if (count == 1) Moves(MOVE_CELEBRATE);
            else if (count == 2) Moves(MOVE_CELEBRATE, MOVE_COUNTER);
            else if (count == 3) Moves(MOVE_CELEBRATE, MOVE_COUNTER, MOVE_MIRROR_COAT);
            else Moves(MOVE_CELEBRATE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_DESTINY_BOND);
        }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MENTAL_MASTER);
        if (count == 1)
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds a Leftovers, and wields Shadow Tag.");
        else if (count == 2)
            MESSAGE("The opposing Wobbuffet knows Celebrate, Counter, holds a Leftovers, and wields Shadow Tag.");
        else if (count == 3)
            MESSAGE("The opposing Wobbuffet knows Celebrate, Counter, Mirror Coat, holds a Leftovers, and wields Shadow Tag.");
        else
            MESSAGE("The opposing Wobbuffet knows Celebrate, Counter, Mirror Coat, Destiny Bond, holds a Leftovers, and wields Shadow Tag.");
    } THEN {
        EXPECT_EQ(gBattleHistory->heldItems[B_BATTLER_1], ITEM_LEFTOVERS);
        EXPECT_EQ(gBattleHistory->abilities[B_BATTLER_1], ABILITY_SHADOW_TAG);
        EXPECT_EQ(gBattleHistory->itemEffects[B_BATTLER_1], HOLD_EFFECT_LEFTOVERS);
        for (u32 slot = 0; slot < count; slot++) {
            EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_1][slot], opponent->moves[slot]);
            EXPECT_EQ(gAiPartyData->mons[B_SIDE_OPPONENT][0].moves[slot], opponent->moves[slot]);
        }
    }
}

SINGLE_BATTLE_TEST("Mental Master uses the correct item article and handles no held item")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_ORAN_BERRY; }
    PARAMETRIZE { item = ITEM_UTILITY_UMBRELLA; }
    PARAMETRIZE { item = ITEM_X_ATTACK; }
    PARAMETRIZE { item = ITEM_HP_UP; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_KADABRA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } SCENE {
        if (item == ITEM_NONE)
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds no item, and wields Shadow Tag.");
        else if (item == ITEM_UTILITY_UMBRELLA)
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds a Utility Umbrella, and wields Shadow Tag.");
        else if (item == ITEM_X_ATTACK)
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds an X Attack, and wields Shadow Tag.");
        else if (item == ITEM_HP_UP)
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds an HP Up, and wields Shadow Tag.");
        else
            MESSAGE("The opposing Wobbuffet knows Celebrate, holds an Oran Berry, and wields Shadow Tag.");
    }
}

DOUBLE_BATTLE_TEST("Mental Master reveals both active foes but not its ally or opposing reserves")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_LUM_BERRY); Moves(MOVE_COUNTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Item(ITEM_NONE); Moves(MOVE_CELEBRATE, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_LEFTOVERS); Moves(MOVE_COUNTER); }
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MENTAL_MASTER);
        MESSAGE("The opposing Wobbuffet knows Celebrate, holds an Oran Berry, and wields Shadow Tag.");
        MESSAGE("The opposing Pikachu knows Celebrate, Thunderbolt, holds no item, and wields Static.");
    } THEN {
        EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_2][0], MOVE_NONE);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_OPPONENT][2].moves[0], MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Mental Master reveals updated moves items and abilities when it re-enters")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE, MOVE_TRICK); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TRICK); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet knows Celebrate, Trick, holds an Oran Berry, and wields Telepathy.");
        MESSAGE("The opposing Wobbuffet knows Celebrate, Trick, holds no item, and wields Telepathy.");
    } THEN {
        EXPECT_EQ(gBattleHistory->heldItems[B_BATTLER_1], ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Mental Master scrolls long reveals without truncating moves items or abilities")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(ABILITY_NEUTRALIZING_GAS); Item(ITEM_ABILITY_SHIELD);
            Moves(MOVE_CELEBRATE, MOVE_PRECIPICE_BLADES, MOVE_THOUSAND_ARROWS, MOVE_HYPERSPACE_FURY);
        }
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("The opposing Wobbuffet knows Celebrate, Precipice Blades, Thousand Arrows, Hyperspace Fury, holds an Ability Shield, and wields Neutralizing Gas.");
    }
}

SINGLE_BATTLE_TEST("Mental Master never misses physical special or status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DYNAMIC_PUNCH; }
    PARAMETRIZE { move = MOVE_ZAP_CANNON; }
    PARAMETRIZE { move = MOVE_HYPNOSIS; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, hit: FALSE, criticalHit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (move == MOVE_HYPNOSIS) STATUS_ICON(opponent, sleep: TRUE);
        else HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mental Master ignores accuracy and evasion stages but cannot hit absent targets")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        struct BattleCalcValues cv = {
            .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_HYPNOSIS,
            .abilities[B_BATTLER_0] = ABILITY_MENTAL_MASTER,
        };
        player->statStages[STAT_ACC] = MIN_STAT_STAGE;
        opponent->statStages[STAT_EVASION] = MAX_STAT_STAGE;
        EXPECT(CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
        opponent->hp = 0;
        EXPECT(!CanMoveSkipAccuracyCalc(&cv, 0, AI_CHECK));
    }
}

SINGLE_BATTLE_TEST("Mental Master does not make incoming attacks accurate")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ZAP_CANNON, hit: FALSE); }
    } SCENE {
        NOT HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Mental Master respects semi-invulnerability")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_DIG; }
    PARAMETRIZE { move = MOVE_DIVE; }
    PARAMETRIZE { move = MOVE_PHANTOM_FORCE; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mental Master preserves Protect and type or ability immunities")
{
    enum Move move;
    enum Ability ability;
    enum Species species;
    PARAMETRIZE { move = MOVE_PROTECT; species = SPECIES_WOBBUFFET; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_CELEBRATE; species = SPECIES_GASTLY; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_CELEBRATE; species = SPECIES_WOBBUFFET; ability = ABILITY_WONDER_GUARD; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mental Master blocks opposing damaging and Prankster status priority moves")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_EXTREME_SPEED; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_FAKE_OUT; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; ability = ABILITY_PRANKSTER; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MENTAL_MASTER);
        NOT ANIMATION(ANIM_TYPE_MOVE, move, opponent);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Mental Master does not grant its partner priority immunity")
{
    bool32 targetMaster;
    PARAMETRIZE { targetMaster = FALSE; }
    PARAMETRIZE { targetMaster = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: targetMaster ? playerLeft : playerRight); }
    } SCENE {
        if (targetMaster) NOT HP_BAR(playerLeft);
        else HP_BAR(playerRight);
    }
}

DOUBLE_BATTLE_TEST("Mental Master allows allied priority moves")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_QUICK_ATTACK, target: playerLeft); }
    } SCENE {
        HP_BAR(playerLeft);
    }
}

SINGLE_BATTLE_TEST("Mental Master priority immunity survives Mold Breaker and ability-bypassing attacks")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_QUICK_ATTACK; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { move = MOVE_GRASSY_GLIDE; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        if (move == MOVE_GRASSY_GLIDE) TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(opponent, move); }
    } SCENE {
        NOT HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Mental Master does not block opposing priority moves targeting the field")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PRANKSTER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
    }
}

SINGLE_BATTLE_TEST("Mental Master guarantees OHKO accuracy while preserving Sturdy and level restrictions")
{
    enum Ability ability;
    u32 level;
    PARAMETRIZE { ability = ABILITY_NONE; level = 50; }
    PARAMETRIZE { ability = ABILITY_NONE; level = 51; }
    PARAMETRIZE { ability = ABILITY_STURDY; level = 50; }
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); Level(50); }
        OPPONENT(SPECIES_REGISTEEL) { Ability(ability); Level(level); }
    } WHEN {
        TURN { MOVE(player, MOVE_FISSURE, hit: FALSE); }
    } SCENE {
        if (level == 50 && ability != ABILITY_STURDY) HP_BAR(opponent);
        else NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Mental Master reveals the foe's active Apex ability rather than its regular ability")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_POLITOED) { Item(ITEM_APEX_METEORITE); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("The opposing Politoed knows Celebrate, holds an Apex Meteorite, and wields Drops of God.");
    } THEN {
        EXPECT_EQ(gBattleHistory->abilities[B_BATTLER_1], ABILITY_DROPS_OF_GOD);
    }
}

DOUBLE_BATTLE_TEST("Mental Master reveals abilities even while Neutralizing Gas suppresses them")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("The opposing Wobbuffet knows Celebrate, holds no item, and wields Static.");
        MESSAGE("The opposing Koffing knows Celebrate, holds no item, and wields Neutralizing Gas.");
    } THEN {
        EXPECT_EQ(gBattleHistory->abilities[B_BATTLER_1], ABILITY_STATIC);
    }
}

DOUBLE_BATTLE_TEST("Mental Master blocks only its own hit of an opposing priority spread attack")
{
    GIVEN {
        PLAYER(SPECIES_ALAKAZAM) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_VENOMOTH) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SLUDGE_WAVE, secondaryEffect: FALSE); }
    } SCENE {
        NOT HP_BAR(playerLeft);
        HP_BAR(playerRight);
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_LT(playerRight->hp, playerRight->maxHP);
    }
}

SINGLE_BATTLE_TEST("Mental Master refreshes previously revealed move slots after Mimic changes them")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_CELEBRATE, MOVE_MIMIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_MIMIC); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet knows Celebrate, Mimic, holds no item, and wields Telepathy.");
        MESSAGE("The opposing Wobbuffet knows Celebrate, Tackle, holds no item, and wields Telepathy.");
    } THEN {
        EXPECT_EQ(gBattleHistory->usedMoves[B_BATTLER_1][1], MOVE_TACKLE);
        EXPECT_EQ(gAiPartyData->mons[B_SIDE_OPPONENT][0].moves[1], MOVE_TACKLE);
    }
}
