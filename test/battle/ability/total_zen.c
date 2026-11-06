#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Total Zen belongs to regular Slowpoke Slowbro and Mega Slowbro only")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_SLOWPOKE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWBRO; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWBRO_MEGA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWPOKE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SLOWBRO; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_SLOWKING; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWPOKE_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWBRO_GALAR; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SLOWKING_GALAR; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        bool32 eligible = item == ITEM_APEX_METEORITE
            && (species == SPECIES_SLOWPOKE || species == SPECIES_SLOWBRO || species == SPECIES_SLOWBRO_MEGA);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_TOTAL_ZEN, eligible);
    }
}

SINGLE_BATTLE_TEST("Total Zen absorbs physical special and targeted status Psychic moves and raises both special stats")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_PSYCHIC; }
    PARAMETRIZE { move = MOVE_PSYSHOCK; }
    PARAMETRIZE { move = MOVE_ZEN_HEADBUTT; }
    PARAMETRIZE { move = MOVE_HYPNOSIS; }
    PARAMETRIZE { move = MOVE_HEAL_PULSE; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); HP(500); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        MESSAGE("Slowbro's Sp. Atk rose!");
        MESSAGE("Slowbro's Sp. Def rose!");
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 500);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Total Zen keeps absorbing when one or both special stats are capped")
{
    bool32 capAttack, capDefense;
    PARAMETRIZE { capAttack = TRUE; capDefense = FALSE; }
    PARAMETRIZE { capAttack = FALSE; capDefense = TRUE; }
    PARAMETRIZE { capAttack = TRUE; capDefense = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 j = 0; j < 3; j++) {
            if (capAttack) TURN { MOVE(player, MOVE_NASTY_PLOT); }
            if (capDefense) TURN { MOVE(player, MOVE_AMNESIA); }
        }
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], capAttack ? MAX_STAT_STAGE : DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], capDefense ? MAX_STAT_STAGE : DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Total Zen does not absorb its own Psychic buffs or field moves")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_PSYCHIC_TERRAIN); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_PSYCHIC);
    }
}

SINGLE_BATTLE_TEST("Total Zen absorbs a multihit Psychic move once before any hits")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TWIN_BEAM); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Total Zen does not activate when Protect blocks the Psychic move")
{
    GIVEN {
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_TOTAL_ZEN); HP_BAR(player); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Total Zen absorbs Psychic moves before they can damage a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        NOT MESSAGE("The substitute took damage for Slowbro!");
    } THEN {
        EXPECT(player->volatiles.substitute);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Total Zen absorbs allied Psychic moves without redirecting moves aimed elsewhere")
{
    GIVEN {
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_PSYCHIC, target: playerLeft); }
        TURN { MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_TOTAL_ZEN);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Total Zen's absorption raises stats despite Covert Cloak")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TOTAL_ZEN); Item(ITEM_COVERT_CLOAK); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Total Zen ignores opposing Attack and Sp Atk boosts and drops", s16 before; s16 after)
{
    bool32 physical, drop;
    PARAMETRIZE { physical = TRUE; drop = FALSE; }
    PARAMETRIZE { physical = TRUE; drop = TRUE; }
    PARAMETRIZE { physical = FALSE; drop = FALSE; }
    PARAMETRIZE { physical = FALSE; drop = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpAttack(100); }
    } WHEN {
        TURN { MOVE(opponent, physical ? MOVE_SCRATCH : MOVE_SWIFT); }
        if (drop) TURN { MOVE(player, physical ? MOVE_CHARM : MOVE_EERIE_IMPULSE); }
        else TURN { MOVE(opponent, physical ? MOVE_SWORDS_DANCE : MOVE_NASTY_PLOT); }
        TURN { MOVE(opponent, physical ? MOVE_SCRATCH : MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].before);
        HP_BAR(player, captureDamage: &results[i].after);
    } THEN {
        EXPECT_EQ(results[i].before, results[i].after);
    }
}

SINGLE_BATTLE_TEST("Total Zen ignores opposing Defense and Sp Def boosts and drops", s16 before; s16 after)
{
    bool32 physical, drop;
    PARAMETRIZE { physical = TRUE; drop = FALSE; }
    PARAMETRIZE { physical = TRUE; drop = TRUE; }
    PARAMETRIZE { physical = FALSE; drop = FALSE; }
    PARAMETRIZE { physical = FALSE; drop = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, physical ? MOVE_SCRATCH : MOVE_SURF); }
        if (drop) TURN { MOVE(player, physical ? MOVE_SCREECH : MOVE_METAL_SOUND); }
        else TURN { MOVE(opponent, physical ? MOVE_IRON_DEFENSE : MOVE_AMNESIA); }
        TURN { MOVE(player, physical ? MOVE_SCRATCH : MOVE_SURF); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].before);
        HP_BAR(opponent, captureDamage: &results[i].after);
    } THEN {
        EXPECT_EQ(results[i].before, results[i].after);
    }
}

SINGLE_BATTLE_TEST("Total Zen retains its own absorbed special attack boost", s16 before; s16 after)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SURF); }
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
        TURN { MOVE(player, MOVE_SURF); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].before);
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        HP_BAR(opponent, captureDamage: &results[i].after);
    } THEN {
        EXPECT_MUL_EQ(results[i].before, UQ_4_12(1.5), results[i].after);
    }
}

SINGLE_BATTLE_TEST("Total Zen ignores opposing accuracy and evasion stages while preserving its own")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        struct BattleCalcValues cv = {0};
        cv.battlerAtk = B_BATTLER_0; cv.battlerDef = B_BATTLER_1; cv.move = MOVE_TACKLE;
        cv.abilities[B_BATTLER_0] = ABILITY_TOTAL_ZEN;
        u32 baseline = GetTotalAccuracy(&cv, B_WEATHER_NONE);
        opponent->statStages[STAT_EVASION] = MAX_STAT_STAGE;
        EXPECT_EQ(GetTotalAccuracy(&cv, B_WEATHER_NONE), baseline);
        player->statStages[STAT_ACC] = MIN_STAT_STAGE;
        EXPECT_LT(GetTotalAccuracy(&cv, B_WEATHER_NONE), baseline);
        player->statStages[STAT_ACC] = DEFAULT_STAT_STAGE;
        opponent->statStages[STAT_EVASION] = DEFAULT_STAT_STAGE;
        cv.battlerAtk = B_BATTLER_1; cv.battlerDef = B_BATTLER_0;
        baseline = GetTotalAccuracy(&cv, B_WEATHER_NONE);
        opponent->statStages[STAT_ACC] = MIN_STAT_STAGE;
        EXPECT_EQ(GetTotalAccuracy(&cv, B_WEATHER_NONE), baseline);
        player->statStages[STAT_EVASION] = MAX_STAT_STAGE;
        EXPECT_LT(GetTotalAccuracy(&cv, B_WEATHER_NONE), baseline);
    }
}

SINGLE_BATTLE_TEST("Total Zen blocks Attract Captivate and Taunt using its own ability popup")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_ATTRACT; }
    PARAMETRIZE { move = MOVE_CAPTIVATE; }
    PARAMETRIZE { move = MOVE_TAUNT; }
    GIVEN {
        WITH_CONFIG(B_OBLIVIOUS_TAUNT, GEN_9);
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); Gender(MON_MALE); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_FEMALE); }
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        MESSAGE("It doesn't affect Slowpoke…");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ((u32)player->volatiles.infatuation, 0);
        EXPECT_EQ((u32)player->volatiles.tauntTimer, 0);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Total Zen prevents Cute Charm infatuation")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); Gender(MON_MALE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CUTE_CHARM); Gender(MON_FEMALE); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_CUTE_CHARM, TRUE)); }
    } THEN {
        EXPECT_EQ((u32)player->volatiles.infatuation, 0);
    }
}

SINGLE_BATTLE_TEST("Total Zen follows Oblivious generation rules for Taunt and Intimidate")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_5; }
    PARAMETRIZE { generation = GEN_7; }
    PARAMETRIZE { generation = GEN_9; }
    GIVEN {
        WITH_CONFIG(B_OBLIVIOUS_TAUNT, generation);
        WITH_CONFIG(B_UPDATED_INTIMIDATE, generation);
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAUNT); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], generation >= GEN_8 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->volatiles.tauntTimer != 0, generation < GEN_6);
    }
}

SINGLE_BATTLE_TEST("Total Zen also blocks both Guardian's Growl drops")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_9);
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_ARCANINE) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Total Zen cannot be bypassed by Mold Breaker or suppressed by Neutralizing Gas")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { ability = ABILITY_NEUTRALIZING_GAS; }
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
        TURN { MOVE(opponent, MOVE_TAUNT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ((u32)player->volatiles.tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Total Zen does not absorb non-Psychic attacks")
{
    GIVEN {
        PLAYER(SPECIES_SLOWPOKE) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Total Zen retains its own absorbed special defense boost", s16 before; s16 after)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_SLOWBRO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWIFT); }
        TURN { MOVE(opponent, MOVE_PSYCHIC); }
        TURN { MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].before);
        ABILITY_POPUP(player, ABILITY_TOTAL_ZEN);
        HP_BAR(player, captureDamage: &results[i].after);
    } THEN {
        EXPECT_MUL_EQ(results[i].after, UQ_4_12(1.5), results[i].before);
    }
}
