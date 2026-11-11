#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

DOUBLE_BATTLE_TEST("Pure Bliss halves damage to both sides and multiple holders do not stack", s16 allyDamage, s16 leftFoeDamage, s16 rightFoeDamage)
{
    u32 holders;
    PARAMETRIZE { holders = 0; }
    PARAMETRIZE { holders = 1; }
    PARAMETRIZE { holders = 2; }
    GIVEN {
        PLAYER(SPECIES_HAPPINY) { Ability(ABILITY_NATURAL_CURE); Item(holders >= 1 ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1000); MaxHP(1000); }
        PLAYER(SPECIES_MACHOP);
        OPPONENT(SPECIES_BLISSEY) { Ability(ABILITY_NATURAL_CURE); Item(holders == 2 ? ITEM_APEX_METEORITE : ITEM_NONE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].allyDamage);
        HP_BAR(opponentLeft, captureDamage: &results[i].leftFoeDamage);
        HP_BAR(opponentRight, captureDamage: &results[i].rightFoeDamage);
    } FINALLY {
        for (u32 j = 1; j <= 2; j++)
        {
            EXPECT_EQ(results[j].allyDamage, max(1, results[0].allyDamage / 2));
            EXPECT_EQ(results[j].leftFoeDamage, max(1, results[0].leftFoeDamage / 2));
            EXPECT_EQ(results[j].rightFoeDamage, max(1, results[0].rightFoeDamage / 2));
        }
    }
}

SINGLE_BATTLE_TEST("Pure Bliss halves fixed damage and stops after the holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_CHANSEY) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_MACHOP) { HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MAKUHITA) { Level(51); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, damage: 25);
        HP_BAR(player, damage: 51);
    }
}

SINGLE_BATTLE_TEST("Pure Bliss reduces a one-hit KO move to half the target's current HP")
{
    GIVEN {
        PLAYER(SPECIES_BLISSEY) { Item(ITEM_APEX_METEORITE); Level(50); HP(101); MaxHP(200); }
        OPPONENT(SPECIES_MACHOP) { Level(50); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FISSURE); }
    } THEN {
        EXPECT_EQ(player->hp, 51);
    }
}

SINGLE_BATTLE_TEST("Pure Bliss reduces a delayed Future Sight when the holder replaces its caster", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_RALTS);
        PLAYER(SPECIES_CHANSEY) { Item(item); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, max(1, results[0].damage / 2));
    }
}

SINGLE_BATTLE_TEST("Pure Bliss applies to the whole Meteoric Wrath impact even if it KOs the holder")
{
    GIVEN {
        PLAYER(SPECIES_CHANSEY) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_MACHOP);
        OPPONENT(SPECIES_MAKUHITA) { HP(101); MaxHP(101); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_METEORIC_WRATH); }
        TURN {}
        TURN { SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player, hp: 0);
        HP_BAR(opponent, hp: 51);
    }
}

AI_DOUBLE_BATTLE_TEST("AI predicts half damage between other battlers while Pure Bliss is active")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_HAPPINY) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_MACHOP);
        OPPONENT(SPECIES_MAKUHITA) { Level(51); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAKUHITA) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage normal = AI_CalcDamageSaveBattlers(MOVE_STRENGTH, B_BATTLER_1, B_BATTLER_2, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        struct SimulatedDamage fixed = AI_CalcDamageSaveBattlers(MOVE_SEISMIC_TOSS, B_BATTLER_1, B_BATTLER_2, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        enum Move *moves = GetMovesArray(B_BATTLER_1);
        enum Move savedMove = moves[0];
        moves[0] = MOVE_STRENGTH;
        CalcBattlerAiMovesData(gAiLogicData, B_BATTLER_1, B_BATTLER_2, B_WEATHER_NONE, B_TERRAIN_NONE);
        s32 batchedDamage = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][0].median;

        u16 holderHP = playerLeft->hp;
        playerLeft->hp = 0;
        struct SimulatedDamage withoutAura = AI_CalcDamageSaveBattlers(MOVE_STRENGTH, B_BATTLER_1, B_BATTLER_2, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        CalcBattlerAiMovesData(gAiLogicData, B_BATTLER_1, B_BATTLER_2, B_WEATHER_NONE, B_TERRAIN_NONE);
        s32 batchedWithoutAura = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][0].median;
        playerLeft->hp = holderHP;
        moves[0] = savedMove;

        EXPECT_EQ(normal.median, max(1, withoutAura.median / 2));
        EXPECT_EQ(batchedDamage, normal.median);
        EXPECT_EQ(batchedWithoutAura, withoutAura.median);
        EXPECT_EQ(fixed.median, 25);
    }
}
