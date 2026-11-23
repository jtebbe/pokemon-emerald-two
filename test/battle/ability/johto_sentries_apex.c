#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

static u32 SentryAccuracy(enum BattlerId attacker, enum BattlerId target, enum Move move)
{
    struct BattleCalcValues cv = { .battlerAtk = attacker, .battlerDef = target, .move = move };
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.abilities[battler] = GetBattlerAbility(battler);
        cv.holdEffects[battler] = GetBattlerHoldEffect(battler);
    }
    return GetTotalAccuracy(&cv, GetWeather());
}

SINGLE_BATTLE_TEST("The early Johto families receive their Apex abilities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_SENTRET; ability = ABILITY_SPEEDY_SENTRY; }
    PARAMETRIZE { species = SPECIES_FURRET; ability = ABILITY_SPEEDY_SENTRY; }
    PARAMETRIZE { species = SPECIES_HOOTHOOT; ability = ABILITY_DARKENED_SKIES; }
    PARAMETRIZE { species = SPECIES_NOCTOWL; ability = ABILITY_DARKENED_SKIES; }
    PARAMETRIZE { species = SPECIES_LEDYBA; ability = ABILITY_PEOPLES_CHAMP; }
    PARAMETRIZE { species = SPECIES_LEDIAN; ability = ABILITY_PEOPLES_CHAMP; }
    PARAMETRIZE { species = SPECIES_SPINARAK; ability = ABILITY_MALIGNED_THREADS; }
    PARAMETRIZE { species = SPECIES_ARIADOS; ability = ABILITY_MALIGNED_THREADS; }
    PARAMETRIZE { species = SPECIES_CHINCHOU; ability = ABILITY_ABYSSAL_SHOCK; }
    PARAMETRIZE { species = SPECIES_LANTURN; ability = ABILITY_ABYSSAL_SHOCK; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ability);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry cancels a super effective attack and lets its owner choose the replacement")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(300); MaxHP(300); Speed(10); }
        PLAYER(SPECIES_WYNAUT) { Speed(10); }
        PLAYER(SPECIES_PIKACHU) { Speed(10); }
        OPPONENT(SPECIES_MACHOP) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); SEND_OUT(player, 2); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPEEDY_SENTRY);
        NOT HP_BAR(player);
        MESSAGE("The opposing Machop's Attack fell!");
        MESSAGE("The opposing Machop's Sp. Atk fell!");
        SEND_IN_MESSAGE("Pikachu");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 300);
        EXPECT_EQ(player->species, SPECIES_PIKACHU);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry works once even without a replacement")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MACHOP);
    } WHEN {
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); }
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPEEDY_SENTRY);
        NOT HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KARATE_CHOP, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->speedySentryUsed);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry does not reset after switching")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_MACHOP);
    } WHEN {
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); SEND_OUT(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_KARATE_CHOP); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPEEDY_SENTRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KARATE_CHOP, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->speedySentryUsed);
        EXPECT_LT(player->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry is not spent by misses, Protect, or neutral hits")
{
    enum Move attack;
    bool32 protect;
    PARAMETRIZE { attack = MOVE_FOCUS_BLAST; protect = FALSE; }
    PARAMETRIZE { attack = MOVE_KARATE_CHOP; protect = TRUE; }
    PARAMETRIZE { attack = MOVE_TACKLE; protect = FALSE; }
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MACHOP);
    } WHEN {
        TURN {
            if (protect) MOVE(player, MOVE_PROTECT);
            if (attack == MOVE_FOCUS_BLAST) MOVE(opponent, attack, hit: FALSE);
            else MOVE(opponent, attack);
        }
    } THEN {
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->speedySentryUsed);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry includes Inner Focus and blocks Intimidate")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(10); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry cancels all hits and requires the Apex Meteorite")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Item(item); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_HITMONLEE);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_KICK); }
    } THEN {
        if (item == ITEM_APEX_METEORITE) EXPECT_EQ(player->hp, 1000);
        else EXPECT_LT(player->hp, 1000);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry cancels the attack even when its stat drops are blocked")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); }
    } THEN {
        EXPECT_EQ(player->hp, 1000);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->speedySentryUsed);
    }
}

SINGLE_BATTLE_TEST("Speedy Sentry is not spent on a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); }
        OPPONENT(SPECIES_MACHOP) { Speed(10); Attack(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_KARATE_CHOP); }
    } THEN {
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->speedySentryUsed);
    }
}

DOUBLE_BATTLE_TEST("Speedy Sentry cancels the entire spread attack")
{
    GIVEN {
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_GYARADOS) { Ability(ABILITY_MINDLESS_RAMPAGE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_MACH_PUNCH, target: playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 1000);
        EXPECT_EQ(playerRight->hp, 1000);
    }
}

DOUBLE_BATTLE_TEST("Darkened Skies weakens all other unprotected types without stacking")
{
    enum Species species;
    u32 expected;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; expected = 75; }
    PARAMETRIZE { species = SPECIES_GASTLY; expected = 100; }
    PARAMETRIZE { species = SPECIES_UMBREON; expected = 100; }
    PARAMETRIZE { species = SPECIES_PIDGEY; expected = 100; }
    GIVEN {
        PLAYER(SPECIES_HOOTHOOT) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_NOCTOWL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(SentryAccuracy(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE), expected);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_3, B_BATTLER_2, MOVE_TACKLE), 75);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_0, B_BATTLER_1, MOVE_TACKLE), 100);
    }
}

DOUBLE_BATTLE_TEST("Darkened Skies affects allies and ends when the holder leaves")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN {}
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT_EQ(SentryAccuracy(B_BATTLER_2, B_BATTLER_1, MOVE_TACKLE), 100);
    }
}

SINGLE_BATTLE_TEST("Darkened Skies includes Mind's Eye and prevents sleep and accuracy drops")
{
    enum Move attack;
    PARAMETRIZE { attack = MOVE_TACKLE; }
    PARAMETRIZE { attack = MOVE_KARATE_CHOP; }
    GIVEN {
        PLAYER(SPECIES_NOCTOWL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GASTLY) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); }
        TURN { MOVE(player, attack); MOVE(opponent, MOVE_SAND_ATTACK); }
        TURN { MOVE(opponent, MOVE_SPORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, attack, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_0, B_BATTLER_1, MOVE_TACKLE), 100);
    }
}

SINGLE_BATTLE_TEST("Darkened Skies prevents Rest")
{
    GIVEN {
        PLAYER(SPECIES_HOOTHOOT) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_REST); } } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Darkened Skies cures entry sleep and prevents delayed Yawn")
{
    GIVEN {
        PLAYER(SPECIES_HOOTHOOT) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_SLEEP_TURN(3)); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_YAWN); }
        TURN {}
        TURN {}
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("People's Champ gains a core omniboost on every entry and KO")
{
    bool32 reenter;
    PARAMETRIZE { reenter = FALSE; }
    PARAMETRIZE { reenter = TRUE; }
    GIVEN {
        PLAYER(SPECIES_LEDIAN) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
        if (reenter)
        {
            TURN { SWITCH(player, 1); }
            TURN { SWITCH(player, 0); }
        }
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + (reenter ? 1 : 2));
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads' web attacks lower both defenses")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_STRING_SHOT; }
    PARAMETRIZE { move = MOVE_ELECTROWEB; }
    PARAMETRIZE { move = MOVE_SPIDER_WEB; }
    PARAMETRIZE { move = MOVE_TOXIC_THREAD; }
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN { TURN { MOVE(player, move); } } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Maligned Threads applies defense drops to both targets of Electroweb")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_ELECTROWEB); } } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads does not debuff a missed or protected target")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            if (protect) MOVE(opponent, MOVE_PROTECT);
            MOVE(player, MOVE_ELECTROWEB, hit: protect);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads adds defense drops to Silk Trap")
{
    GIVEN {
        PLAYER(SPECIES_SPINARAK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SILK_TRAP); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads webs persist after their setter leaves")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_MALIGNED_WEB));
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STICKY_WEB));
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads webs respect Boots, grounding, and Clear Body")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    s32 expected;
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_SHADOW_TAG; item = ITEM_HEAVY_DUTY_BOOTS; expected = 0; }
    PARAMETRIZE { species = SPECIES_PIDGEY; ability = ABILITY_KEEN_EYE; item = ITEM_NONE; expected = 0; }
    PARAMETRIZE { species = SPECIES_BELDUM; ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; expected = 0; }
    PARAMETRIZE { species = SPECIES_SHUCKLE; ability = ABILITY_CONTRARY; item = ITEM_NONE; expected = 1; }
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + expected);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + expected);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + expected);
    }
}

SINGLE_BATTLE_TEST("Maligned Threads webs can be removed or Court Changed")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_RAPID_SPIN; }
    PARAMETRIZE { move = MOVE_DEFOG; }
    PARAMETRIZE { move = MOVE_COURT_CHANGE; }
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { MOVE(opponent, move); }
        if (move == MOVE_COURT_CHANGE) TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_MALIGNED_WEB));
        if (move == MOVE_COURT_CHANGE)
        {
            EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_MALIGNED_WEB));
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        }
    }
}

DOUBLE_BATTLE_TEST("Abyssal Shock reduces opponents' evasion for the holder and ally")
{
    GIVEN {
        PLAYER(SPECIES_LANTURN) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(SentryAccuracy(B_BATTLER_0, B_BATTLER_1, MOVE_TACKLE), 150);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_2, B_BATTLER_3, MOVE_TACKLE), 150);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_1, B_BATTLER_0, MOVE_TACKLE), 100);
        EXPECT_EQ(SentryAccuracy(B_BATTLER_2, B_BATTLER_0, MOVE_TACKLE), 100);
    }
}

DOUBLE_BATTLE_TEST("Abyssal Shock cannot miss paralyzed opponents and shields both allies", s16 damage)
{
    bool32 paralysis;
    PARAMETRIZE { paralysis = FALSE; }
    PARAMETRIZE { paralysis = TRUE; }
    GIVEN {
        PLAYER(SPECIES_LANTURN) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(paralysis ? STATUS1_PARALYSIS : STATUS1_NONE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SWIFT, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        HP_BAR(playerLeft);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } THEN {
        if (paralysis)
        {
            struct BattleCalcValues cv = { .battlerAtk = B_BATTLER_2, .battlerDef = B_BATTLER_1, .move = MOVE_ZAP_CANNON };
            cv.abilities[B_BATTLER_0] = ABILITY_ABYSSAL_SHOCK;
            EXPECT(CanMoveSkipAccuracyCalc(&cv, GetWeather(), CHECK_TRIGGER));
        }
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0 / 3), results[1].damage);
    }
}

AI_SINGLE_BATTLE_TEST("Speedy Sentry AI switches into a predicted super effective attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_MACHOP) { Moves(MOVE_KARATE_CHOP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_KARATE_CHOP); EXPECT_SWITCH(opponent, 1); EXPECT_SEND_OUT(opponent, 0); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(gBattleStruct->partyState[B_TRAINER_OPPONENT_A][1].speedySentryUsed);
    }
}

AI_SINGLE_BATTLE_TEST("Speedy Sentry AI avoids wasting a super effective hit on an unused trigger")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_FURRET) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MACHOP) { Moves(MOVE_KARATE_CHOP, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_KARATE_CHOP, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_SINGLE_BATTLE_TEST("Darkened Skies AI knows accuracy drops cannot affect it")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NOCTOWL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SAND_ATTACK, MOVE_TACKLE); }
    } WHEN {
        TURN { SCORE_LT(opponent, MOVE_SAND_ATTACK, MOVE_TACKLE); EXPECT_MOVE(opponent, MOVE_TACKLE); }
    }
}

AI_DOUBLE_BATTLE_TEST("Darkened Skies AI forecasts the field accuracy penalty")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_NOCTOWL) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][0], 75);
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_2][0], 75);
    }
}

AI_DOUBLE_BATTLE_TEST("Abyssal Shock AI forecasts its ally's accuracy against healthy and paralyzed foes")
{
    bool32 paralysis;
    PARAMETRIZE { paralysis = FALSE; }
    PARAMETRIZE { paralysis = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Status1(paralysis ? STATUS1_PARALYSIS : STATUS1_NONE); }
        PLAYER(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_LANTURN) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DYNAMIC_PUNCH); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_3][B_BATTLER_0][0], paralysis ? 101 : 75);
    }
}
