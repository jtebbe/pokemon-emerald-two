#include "global.h"
#include "test/battle.h"
#include "battle_i_choose_you.h"
#include "battle_util.h"
#include "caps.h"
#include "event_data.h"
#include "global.fieldmap.h"
#include "rtc.h"
#include "constants/map_types.h"

SINGLE_BATTLE_TEST("I Choose You grants the entry boosts and effective Speed of its ball")
{
    static const struct
    {
        enum PokeBall ball;
        s8 stages[NUM_BATTLE_STATS];
        u16 speed;
    } cases[] = {
        {BALL_POKE, { [STAT_ATK] = 2 }, 100},
        {BALL_GREAT, { [STAT_DEF] = 2, [STAT_SPDEF] = 2 }, 100},
        {BALL_ULTRA, { [STAT_SPATK] = 2 }, 100},
        {BALL_MASTER, { [STAT_ATK] = 1, [STAT_DEF] = 1, [STAT_SPEED] = 1, [STAT_SPATK] = 1, [STAT_SPDEF] = 1 }, 150},
        {BALL_SAFARI, { [STAT_EVASION] = 1 }, 100},
        {BALL_HEAVY, { [STAT_ATK] = 3, [STAT_DEF] = 3, [STAT_SPATK] = 3, [STAT_SPDEF] = 3 }, 50},
        {BALL_FAST, { [STAT_ATK] = 2, [STAT_SPATK] = 2 }, 100},
        {BALL_SPORT, { [STAT_ACC] = 2 }, 100},
        {BALL_QUICK, { [STAT_SPEED] = 6 }, 400},
    };
    u32 row = 0;
    for (u32 j = 0; j < ARRAY_COUNT(cases); j++)
        PARAMETRIZE { row = j; }

    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(cases[row].ball); Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_I_CHOOSE_YOU);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + cases[row].stages[stat]);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_0, ABILITY_I_CHOOSE_YOU, HOLD_EFFECT_NONE), cases[row].speed);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Fast Ball requires strictly greater effective Speed")
{
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(BALL_FAST); Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("I Choose You's Friend Ball honors its specific immunities and leaves Adrenaline Orb intact")
{
    enum Ability ability = ABILITY_TELEPATHY;
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { ability = ABILITY_OBLIVIOUS; }
    PARAMETRIZE { ability = ABILITY_SCRAPPY; }
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(BALL_FRIEND); Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ADRENALINE_ORB); Ability(ABILITY_RATTLED); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (ability == ABILITY_TELEPATHY));
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->item, ITEM_ADRENALINE_ORB);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("I Choose You's Net and Dive Balls affect both foes")
{
    enum PokeBall ball = BALL_NET;
    PARAMETRIZE { ball = BALL_NET; }
    PARAMETRIZE { ball = BALL_DIVE; }
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(ball); Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DROWZEE);
        OPPONENT(SPECIES_HAUNTER);
    } WHEN { TURN {} } THEN {
        if (ball == BALL_NET) {
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        } else {
            EXPECT_EQ(opponentLeft->types[0], TYPE_WATER);
            EXPECT_EQ(opponentLeft->types[1], TYPE_WATER);
            EXPECT_EQ(opponentRight->types[0], TYPE_WATER);
            EXPECT_EQ(opponentRight->types[1], TYPE_WATER);
            EXPECT(!IS_BATTLER_OF_TYPE(B_BATTLER_3, TYPE_GHOST));
        }
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->types[0], TYPE_PSYCHIC);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Moon and Premier Balls change the secondary type and only Moon converts Normal attacks")
{
    enum PokeBall ball = BALL_MOON;
    PARAMETRIZE { ball = BALL_MOON; }
    PARAMETRIZE { ball = BALL_PREMIER; }
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(ball); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_HAUNTER) { HP(1000); MaxHP(1000); }
    } WHEN { TURN { MOVE(player, MOVE_TACKLE); } } THEN {
        EXPECT_EQ(player->types[0], TYPE_ELECTRIC);
        EXPECT_EQ(player->types[1], ball == BALL_MOON ? TYPE_FAIRY : TYPE_NORMAL);
        EXPECT_EQ(opponent->hp < opponent->maxHP, ball == BALL_MOON);
    }
}

DOUBLE_BATTLE_TEST("I Choose You's Love Ball infatuates the facing foe even though Voltorb is genderless")
{
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(BALL_LOVE); Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DROWZEE) { Gender(MON_MALE); }
        OPPONENT(SPECIES_DROWZEE) { Gender(MON_FEMALE); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ((u32)opponentLeft->volatiles.infatuation, B_BATTLER_0 + 1);
        EXPECT_EQ((u32)opponentRight->volatiles.infatuation, 0);
    }
}

SINGLE_BATTLE_TEST("Infatuation prevents acting on 30 percent of turns")
{
    PASSES_RANDOMLY(30, 100, RNG_INFATUATION);
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(BALL_LOVE); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_CELEBRATE); } } SCENE {
        MESSAGE("The opposing Wobbuffet is immobilized by love!");
    }
}

DOUBLE_BATTLE_TEST("I Choose You's Luxury Ball raises the ally's core stats once per holder per battle")
{
    GIVEN {
        PLAYER(SPECIES_ELECTRODE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_VOLTORB) { Ball(BALL_LUXURY); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SUBSTITUTE); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++) {
            EXPECT_EQ(playerRight->statStages[stat], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE);
        }
    }
}

DOUBLE_BATTLE_TEST("I Choose You's Heal Ball restores a quarter of the holder's and ally's maximum HP")
{
    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(BALL_HEAL); Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(320); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 180);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Cherish Ball restores HP and status in the party when the holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(BALL_CHERISH); Item(ITEM_APEX_METEORITE); HP(50); MaxHP(200); Status1(STATUS1_BURN); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
    } WHEN { TURN { SWITCH(player, 1); } } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 200);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Timer Ball explodes electrically on the third end of turn")
{
    enum Species foe = SPECIES_WOBBUFFET;
    PARAMETRIZE { foe = SPECIES_WOBBUFFET; }
    PARAMETRIZE { foe = SPECIES_ONIX; }
    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(BALL_TIMER); Item(ITEM_APEX_METEORITE); }
        OPPONENT(foe) { HP(2400); MaxHP(2400); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Electrode used Celebrate!");
        MESSAGE("Electrode used Celebrate!");
        MESSAGE("Electrode used Celebrate!");
        MESSAGE("Electrode used Explosion!");
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(opponent->hp < opponent->maxHP, foe != SPECIES_ONIX);
    }
}

WILD_BATTLE_TEST("I Choose You's Lure Ball quadruples catch odds while its holder is active")
{
    bool32 active = FALSE;
    PARAMETRIZE { active = FALSE; }
    PARAMETRIZE { active = TRUE; }
    u32 odds;
    GIVEN {
        WITH_CONFIG(B_MISSING_BADGE_CATCH_MALUS, GEN_7);
        WITH_CONFIG(B_LOW_LEVEL_CATCH_BONUS, GEN_7);
        if (active)
            PLAYER(SPECIES_VOLTORB) { Ball(BALL_LURE); Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        if (!active)
            PLAYER(SPECIES_VOLTORB) { Ball(BALL_LURE); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CLEFFA);
    } WHEN { TURN { USE_ITEM(player, ITEM_POKE_BALL); } } SCENE {
        CATCHING_CHANCE(&odds);
    } THEN {
        EXPECT_EQ(odds, active ? 200 : 50);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Nest Park and Strange Balls use the specific foe in the damage calculation", s16 damage)
{
    static const struct
    {
        enum PokeBall ball;
        enum Species foe;
        uq4_12_t multiplier;
    } cases[] = {
        {BALL_NEST, SPECIES_MAGIKARP, UQ_4_12(2.0)},
        {BALL_NEST, SPECIES_VOLTORB, UQ_4_12(1.0)},
        {BALL_NEST, SPECIES_WOBBUFFET, UQ_4_12(1.0)},
        {BALL_PARK, SPECIES_NOSEPASS, UQ_4_12(1.0)},
        {BALL_PARK, SPECIES_DWEBBLE, UQ_4_12(1.5)},
        {BALL_PARK, SPECIES_WOBBUFFET, UQ_4_12(2.0)},
        {BALL_STRANGE, SPECIES_PICHU, UQ_4_12(0.5)},
        {BALL_STRANGE, SPECIES_VOLTORB, UQ_4_12(1.0)},
        {BALL_STRANGE, SPECIES_ABRA, UQ_4_12(1.5)},
        {BALL_STRANGE, SPECIES_WOBBUFFET, UQ_4_12(2.0)},
    };
    u32 row = 0;
    bool32 apex = FALSE;
    for (u32 j = 0; j < ARRAY_COUNT(cases); j++)
    {
        PARAMETRIZE { row = j; apex = FALSE; }
        PARAMETRIZE { row = j; apex = TRUE; }
    }
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Ball(cases[row].ball); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(cases[row].foe) { Ability(ABILITY_BATTLE_ARMOR); HP(2400); MaxHP(2400); }
    } WHEN { TURN { MOVE(player, MOVE_TACKLE); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(cases); j++)
            EXPECT_MUL_EQ(results[2 * j].damage, cases[j].multiplier, results[2 * j + 1].damage);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Repeat Ball adds 50 percent per repeat up to triple damage and resets on changing moves")
{
    s16 damage[7];
    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(BALL_REPEAT); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(4000); MaxHP(4000); }
    } WHEN {
        for (u32 j = 0; j < 6; j++)
            TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        for (u32 j = 0; j < ARRAY_COUNT(damage); j++)
            HP_BAR(opponent, captureDamage: &damage[j]);
    } THEN {
        for (u32 j = 1; j < 6; j++)
            EXPECT_MUL_EQ(damage[0], UQ_4_12(1.0) + min(4, j) * UQ_4_12(0.5), damage[j]);
        EXPECT_EQ(damage[6], damage[0]);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Dream Ball behaves as Comatose and doubles Sleep Talk damage", s16 damage)
{
    bool32 apex = FALSE;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        if (apex)
            PLAYER(SPECIES_ELECTRODE) { Ball(BALL_DREAM); Item(ITEM_APEX_METEORITE); Moves(MOVE_SLEEP_TALK, MOVE_TACKLE); }
        else
            PLAYER(SPECIES_ELECTRODE) { Ability(ABILITY_COMATOSE); Moves(MOVE_SLEEP_TALK, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(2400); MaxHP(2400); Moves(MOVE_TOXIC); }
    } WHEN { TURN { MOVE(player, MOVE_SLEEP_TALK); MOVE(opponent, MOVE_TOXIC); } } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT(IsAsleepOrComatose(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0)));
        EXPECT_EQ(player->status1, STATUS1_NONE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("I Choose You's Dusk Ball follows the same cave and time rule as the capture ball")
{
    GIVEN {
        PLAYER(SPECIES_ELECTRODE) { Ball(BALL_DUSK); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_TACKLE };
        ctx.abilities[B_BATTLER_0] = ABILITY_I_CHOOSE_YOU;
        u32 cave = gMapHeader.cave;
        u32 mapType = gMapHeader.mapType;
        gMapHeader.cave = FALSE;
        gMapHeader.mapType = MAP_TYPE_ROUTE;
        enum TimeOfDay time = GetTimeOfDay();
        EXPECT_EQ(GetIChooseYouDamageModifier(&ctx), time == TIME_EVENING || time == TIME_NIGHT ? UQ_4_12(2.0) : UQ_4_12(1.0));
        gMapHeader.cave = TRUE;
        EXPECT_EQ(GetIChooseYouDamageModifier(&ctx), UQ_4_12(2.0));
        gMapHeader.cave = cave;
        gMapHeader.mapType = mapType;
    }
}

TEST("I Choose You's Level Ball raises the individual level cap by five")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    FlagClear(FLAG_BADGE01_GET);
    gSaveData.optionsLevelCaps = TRUE;
    CreateMon(mon, SPECIES_VOLTORB, 15, 0, OTID_STRUCT_PRESET(0));
    u32 ball = BALL_LEVEL;
    SetMonData(mon, MON_DATA_POKEBALL, &ball);
    EXPECT_EQ(GetMonLevelCap(mon), 15);
    u32 item = ITEM_APEX_METEORITE;
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(GetMonLevelCap(mon), 20);
    u32 exp = gExperienceTables[GROWTH_MEDIUM_FAST][21];
    SetMonData(mon, MON_DATA_EXP, &exp);
    for (u32 level = 16; level <= 20; level++)
    {
        EXPECT(TryIncrementMonLevel(mon));
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), level);
    }
    EXPECT(!TryIncrementMonLevel(mon));
    gSaveData.optionsLevelCaps = FALSE;
}

TEST("I Choose You's Beast Ball swaps base stats and recalculates them when its Meteorite is removed")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_VOLTORB, 50, 0, OTID_STRUCT_PRESET(0));
    u32 nature = NATURE_HARDY;
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
    u32 originalStats[NUM_STATS];
    for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
        originalStats[stat] = GetMonData(mon, MON_DATA_MAX_HP + stat);
    u32 ball = BALL_BEAST;
    u32 item = ITEM_APEX_METEORITE;
    SetMonData(mon, MON_DATA_POKEBALL, &ball);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), GetSpeciesBaseSpeed(SPECIES_VOLTORB) + 75);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPEED), GetSpeciesBaseHP(SPECIES_VOLTORB) + 20);
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK), originalStats[STAT_SPATK]);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPATK), originalStats[STAT_ATK]);
    EXPECT_EQ(GetMonData(mon, MON_DATA_DEF), originalStats[STAT_SPDEF]);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPDEF), originalStats[STAT_DEF]);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP));
    item = ITEM_NONE;
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    for (enum Stat stat = STAT_HP; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP + stat), originalStats[stat]);
}
