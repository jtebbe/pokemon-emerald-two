#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

AI_SINGLE_BATTLE_TEST("Reverberations AI includes the echo when comparing move damage")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_REVERBERATIONS); Moves(MOVE_HYPER_VOICE, MOVE_SURF); }
    } WHEN {
        TURN { SCORE_GT(opponent, MOVE_HYPER_VOICE, MOVE_SURF); EXPECT_MOVE(opponent, MOVE_HYPER_VOICE); }
    }
}

AI_SINGLE_BATTLE_TEST("Reverberations AI damage ranges include both hits and Torch Song's intervening boost")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_HYPER_VOICE; }
    PARAMETRIZE { move = MOVE_SNARL; }
    PARAMETRIZE { move = MOVE_TORCH_SONG; }
    s16 damage[2];
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Moves(move); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage[0]);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        s32 stage = opponent->statStages[STAT_SPATK];
        if (move == MOVE_TORCH_SONG)
            opponent->statStages[STAT_SPATK] -= 2;
        uq4_12_t effectiveness;
        struct SimulatedDamage estimate = AI_CalcDamageSaveBattlers(move, B_BATTLER_1, B_BATTLER_0, &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        opponent->statStages[STAT_SPATK] = stage;
        EXPECT_GE(damage[0] + damage[1], estimate.minimum);
        EXPECT_LE(damage[0] + damage[1], estimate.maximum);
        EXPECT_GT(damage[0], damage[1]);
    }
}

AI_SINGLE_BATTLE_TEST("Reverberations AI recognizes that Focus Sash cannot endure an echo")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Item(ITEM_FOCUS_SASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_REVERBERATIONS); Moves(MOVE_HYPER_VOICE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_HYPER_VOICE); }
    } THEN {
        u32 hp = player->hp;
        player->hp = player->maxHP;
        bool32 enduresEcho = CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_HYPER_VOICE);
        bool32 enduresSingle = CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_SURF);
        player->hp = hp;
        EXPECT(!enduresEcho);
        EXPECT(enduresSingle);
    }
}
