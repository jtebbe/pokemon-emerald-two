#include "global.h"
#include "test/battle.h"
#include "battle_anim.h"
#include "battle_util.h"
#include "sprite.h"

TEST("Reverberations is Loudred and Exploud's hidden ability with Scrappy in slot two")
{
    const enum Species species[] = {SPECIES_LOUDRED, SPECIES_EXPLOUD};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetSpeciesAbility(species[i], 0), ABILITY_SOUNDPROOF);
        EXPECT_EQ(GetSpeciesAbility(species[i], 1), ABILITY_SCRAPPY);
        EXPECT_EQ(GetSpeciesAbility(species[i], 2), ABILITY_REVERBERATIONS);
    }
    EXPECT_EQ(GetSpeciesAbility(SPECIES_WHISMUR, 2), ABILITY_RATTLED);
}

TEST("Reverberations bans unrepeatable sound effects and ignores innate multi-hit moves")
{
    const enum Move moves[] = {
        MOVE_PARTING_SHOT, MOVE_SING, MOVE_GRASS_WHISTLE, MOVE_DRAGON_CHEER,
        MOVE_ROAR, MOVE_SUPERSONIC, MOVE_PERISH_SONG, MOVE_HAUNTING_SHRIEKS,
    };
    for (u32 i = 0; i < ARRAY_COUNT(moves); i++) {
        EXPECT(IsMoveReverberationsBanned(moves[i]));
        EXPECT(!IsMoveReverberationsAffected(B_BATTLER_0, moves[i]));
    }
}

SINGLE_BATTLE_TEST("Reverberations adds a 30 percent second hit and spends one PP")
{
    s16 damage[2];
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent, captureDamage: &damage[0]);
        HP_BAR(opponent, captureDamage: &damage[1]);
        MESSAGE("The Pokémon was hit 2 times!");
        NOT HP_BAR(opponent);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT_MUL_EQ(damage[0], Q_4_12(0.3), damage[1]);
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_HYPER_VOICE) - 1);
        EXPECT_EQ(gLoadFail, FALSE);
        EXPECT_EQ(gSpriteAllocs, 0);
    }
}

SINGLE_BATTLE_TEST("Reverberations does not duplicate non-sound moves")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_PULSE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Reverberations repeats sound stat drops at full strength")
{
    enum Move move;
    enum Stat stat;
    u32 drop;
    PARAMETRIZE { move = MOVE_GROWL; stat = STAT_ATK; drop = 2; }
    PARAMETRIZE { move = MOVE_NOBLE_ROAR; stat = STAT_ATK; drop = 2; }
    PARAMETRIZE { move = MOVE_SCREECH; stat = STAT_DEF; drop = 4; }
    PARAMETRIZE { move = MOVE_METAL_SOUND; stat = STAT_SPDEF; drop = 4; }
    PARAMETRIZE { move = MOVE_CONFIDE; stat = STAT_SPATK; drop = 2; }
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_ACCURACY, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[stat], DEFAULT_STAT_STAGE - drop);
        if (move == MOVE_NOBLE_ROAR)
            EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->pp[0], GetMovePP(move) - 1);
    }
}

DOUBLE_BATTLE_TEST("Reverberations repeats Howl for both allies without boosting opponents")
{
    GIVEN {
        ASSUME(IsSoundMove(MOVE_HOWL));
        ASSUME(GetMoveTarget(MOVE_HOWL) == TARGET_USER_AND_ALLY);
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Moves(MOVE_HOWL); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HOWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HOWL, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HOWL, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->pp[0], GetMovePP(MOVE_HOWL) - 1);
    }
}

SINGLE_BATTLE_TEST("Reverberations repeats Heal Bell even after the first chime cures the party")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Status1(STATUS1_BURN); Moves(MOVE_HEAL_BELL); }
        PLAYER(SPECIES_WHISMUR) { Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HEAL_BELL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_STATUS), STATUS1_NONE);
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_HEAL_BELL) - 1);
    }
}

DOUBLE_BATTLE_TEST("Reverberations does not retry a Soundproof target when echoing Growl")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(opponentRight, ABILITY_SOUNDPROOF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, playerLeft);
        NOT ABILITY_POPUP(opponentRight, ABILITY_SOUNDPROOF);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Reverberations echoes spread sound attacks even if one foe faints on the first hit")
{
    enum Move move = MOVE_HYPER_VOICE;
    bool32 knockout = FALSE;
    for (u32 i = 0; i < 2; i++) {
        PARAMETRIZE { move = MOVE_HYPER_VOICE; knockout = i; }
        PARAMETRIZE { move = MOVE_BOOMBURST; knockout = i; }
    }
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(knockout ? 1 : 3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, move); }
    } SCENE {
        if (move == MOVE_BOOMBURST)
            HP_BAR(playerRight);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        if (move == MOVE_BOOMBURST)
            HP_BAR(playerRight);
        if (!knockout)
            HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Reverberations still echoes when the selected foe has Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentRight);
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentLeft);
    }
}

SINGLE_BATTLE_TEST("Reverberations breaks Focus Sash with its second hit")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        OPPONENT(SPECIES_CATERPIE) { Level(1); Item(ITEM_FOCUS_SASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent, hp: 1);
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Reverberations does not hit a protected or Soundproof target twice")
{
    bool32 soundproof;
    PARAMETRIZE { soundproof = FALSE; }
    PARAMETRIZE { soundproof = TRUE; }
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(soundproof ? ABILITY_SOUNDPROOF : ABILITY_TELEPATHY); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); if (!soundproof) { MOVE(opponent, MOVE_PROTECT); } }
    } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Reverberations does not duplicate a sound move's existing multihit count")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_GASTLY) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        MESSAGE("The Pokémon was hit 2 times!");
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Reverberations triggers Life Orb recoil only once")
{
    s16 recoil;
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); Item(ITEM_LIFE_ORB); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        HP_BAR(player, captureDamage: &recoil);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(recoil, 100);
    }
}

SINGLE_BATTLE_TEST("Reverberations applies Torch Song's Sp Atk boost after each hit")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TORCH_SONG); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Reverberations does not echo Sing or Grass Whistle")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SING; }
    PARAMETRIZE { move = MOVE_GRASS_WHISTLE; }
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_ACCURACY, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        STATUS_ICON(opponent, sleep: TRUE);
        NOT ANIMATION(ANIM_TYPE_MOVE, move, player);
    }
}

SINGLE_BATTLE_TEST("Reverberations lets Parting Shot switch out exactly once")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_WHISMUR);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PARTING_SHOT); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PARTING_SHOT, player);
        SEND_IN_MESSAGE("Whismur");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->species, SPECIES_WHISMUR);
    }
}

DOUBLE_BATTLE_TEST("Neutralizing Gas suppresses Reverberations for sound damage and status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_HYPER_VOICE; }
    PARAMETRIZE { move = MOVE_GROWL; }
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_REVERBERATIONS); }
        PLAYER(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        NOT ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        if (move == MOVE_HYPER_VOICE) {
            HP_BAR(opponentLeft);
            HP_BAR(opponentRight);
            NONE_OF {
                HP_BAR(opponentLeft);
                HP_BAR(opponentRight);
            }
        }
    } THEN {
        if (move == MOVE_GROWL) {
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        }
    }
}
