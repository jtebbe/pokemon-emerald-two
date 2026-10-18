#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Serrated Slice scatters Spikes only from Steel targets")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_REGISTEEL; }
    PARAMETRIZE { species = SPECIES_AGGRON; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    GIVEN {
        ASSUME(GetMoveCriticalHitStage(MOVE_SERRATED_SLICE) == 1);
        ASSUME(IsSlicingMove(MOVE_SERRATED_SLICE));
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SERRATED_SLICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SERRATED_SLICE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, species == SPECIES_WOBBUFFET ? 0 : 1);
    }
}

SINGLE_BATTLE_TEST("Serrated Slice caps Spikes at three layers")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_REGISTEEL) { HP(2000); MaxHP(2000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SERRATED_SLICE); }
        TURN { MOVE(player, MOVE_SERRATED_SLICE); }
        TURN { MOVE(player, MOVE_SERRATED_SLICE); }
        TURN { MOVE(player, MOVE_SERRATED_SLICE); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
    }
}

SINGLE_BATTLE_TEST("Serrated Slice does not scatter Spikes through Protect or on a miss")
{
    bool32 protect;
    PARAMETRIZE { protect = FALSE; }
    PARAMETRIZE { protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_REGISTEEL);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, protect ? MOVE_PROTECT : MOVE_CELEBRATE); MOVE(player, MOVE_SERRATED_SLICE, hit: protect); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Serrated Slice scatters Spikes when hitting a Steel substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_REGISTEEL) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SERRATED_SLICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SERRATED_SLICE, player);
        SUB_HIT(opponent);
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Dazzling Dash raises Speed only when slower, and crits only when faster")
{
    u32 speed;
    PARAMETRIZE { speed = 50; }
    PARAMETRIZE { speed = 100; }
    PARAMETRIZE { speed = 150; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DAZZLING_DASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_DASH, player);
        HP_BAR(opponent);
        if (speed > 100)
            MESSAGE("A critical hit!");
        else
            NOT MESSAGE("A critical hit!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (speed < 100));
    }
}

SINGLE_BATTLE_TEST("Dazzling Dash ignores accuracy and evasion changes")
{
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_DAZZLING_DASH) == 0);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SAND_ATTACK); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_DAZZLING_DASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_DASH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Dazzling Dash's guaranteed critical hit respects Battle Armor")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_ANORITH) { Ability(ABILITY_BATTLE_ARMOR); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DAZZLING_DASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_DASH, player);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Dazzling Dash does not raise Speed when protected against")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_DAZZLING_DASH); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Corrode damages Steel, lowers both defenses, and removes only Steel typing")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_REGISTEEL; }
    PARAMETRIZE { species = SPECIES_AGGRON; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { HP(1000); MaxHP(1000); Ability(species == SPECIES_REGISTEEL ? ABILITY_LIGHT_METAL : ABILITY_STURDY); }
    } WHEN {
        TURN { MOVE(player, MOVE_CORRODE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CORRODE, player);
        HP_BAR(opponent);
        if (species == SPECIES_REGISTEEL)
            MESSAGE("It's super effective!");
        else
            NOT MESSAGE("It's super effective!"); // Rock resists Poison: 2x Steel * 0.5x Rock.
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(!IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_STEEL));
        if (species == SPECIES_AGGRON)
            EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_ROCK));
    }
}

SINGLE_BATTLE_TEST("Corrode lowers non-Steel targets' defenses without changing their types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CORRODE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_PSYCHIC));
    }
}

SINGLE_BATTLE_TEST("Corrode's typing removal ends on switching out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_REGISTEEL);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CORRODE); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN {
        EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_STEEL));
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Corrode does not remove typing or lower defenses through a substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_REGISTEEL) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_CORRODE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CORRODE, player);
        SUB_HIT(opponent);
    } THEN {
        EXPECT(IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_STEEL));
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Mind Crush uses the user's Sp. Def, not Sp. Atk", s16 damage)
{
    u32 spAtk, spDef;
    PARAMETRIZE { spAtk = 100; spDef = 100; }
    PARAMETRIZE { spAtk = 200; spDef = 100; }
    PARAMETRIZE { spAtk = 100; spDef = 200; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(spAtk); SpDefense(spDef); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_MIND_CRUSH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIND_CRUSH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_GT(results[2].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Mind Crush uses Sp. Def stages instead of Sp. Atk stages", s16 damage)
{
    enum Move setup;
    PARAMETRIZE { setup = MOVE_CELEBRATE; }
    PARAMETRIZE { setup = MOVE_NASTY_PLOT; }
    PARAMETRIZE { setup = MOVE_AMNESIA; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, setup); }
        TURN { MOVE(player, MOVE_MIND_CRUSH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIND_CRUSH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_GT(results[2].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks hits once plus once per fainted, non-egg teammate")
{
    u32 fainted;
    PARAMETRIZE { fainted = 0; }
    PARAMETRIZE { fainted = 1; }
    PARAMETRIZE { fainted = 5; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        for (u32 j = 0; j < 5; j++)
            PLAYER(SPECIES_WOBBUFFET) { HP(j < fainted ? 0 : 100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        for (u32 j = 0; j <= fainted; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
            HP_BAR(opponent);
        }
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks uses each contributor's Sp. Atk and Ghost STAB", s16 damage[3])
{
    enum Species contributor;
    PARAMETRIZE { contributor = SPECIES_WOBBUFFET; }
    PARAMETRIZE { contributor = SPECIES_GASTLY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); }
        PLAYER(contributor) { HP(0); SpAttack(100); }
        PLAYER(contributor) { HP(0); SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        for (u32 j = 0; j < 3; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
            HP_BAR(opponent, captureDamage: &results[i].damage[j]);
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage[0], results[1].damage[0]);
        EXPECT_EQ(results[0].damage[0], results[0].damage[1]);
        EXPECT_MUL_EQ(results[0].damage[1], Q_4_12(1.5), results[1].damage[1]);
        EXPECT_GT(results[0].damage[2], results[0].damage[1]);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks' hit count is not changed by Skill Link or Loaded Dice")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_SKILL_LINK; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHELL_ARMOR; item = ITEM_LOADED_DICE; }
    GIVEN {
        ASSUME(IsMultiHitMove(MOVE_HAUNTING_SHRIEKS));
        PLAYER(SPECIES_CLOYSTER) { Ability(ability); Item(item); }
        PLAYER(SPECIES_GASTLY) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks bypasses substitutes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_GASTLY) { HP(0); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
            HP_BAR(opponent);
        }
        NOT SUB_HIT(opponent);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks is blocked by Soundproof and Normal immunity")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_EXPLOUD; ability = ABILITY_SOUNDPROOF; }
    PARAMETRIZE { species = SPECIES_RATTATA; ability = ABILITY_RUN_AWAY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GASTLY) { HP(0); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player); HP_BAR(opponent); }
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks stops after knocking out its target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GASTLY) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
    } THEN {
        EXPECT(!gBattleStruct->hauntingShrieksBgActive);
    }
}

SINGLE_BATTLE_TEST("Voltaic Rush doubles power only against paralysis", s16 damage)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_POISON; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); }
    } WHEN {
        TURN { MOVE(player, MOVE_VOLTAIC_RUSH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VOLTAIC_RUSH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Voltaic Rush has a 30 percent paralysis chance")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_VOLTAIC_RUSH); }
    } SCENE {
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Corrode strips Steel even if Clear Body prevents its stat drops")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_REGISTEEL) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_CORRODE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT(!IS_BATTLER_OF_TYPE(B_BATTLER_1, TYPE_STEEL));
    }
}

SINGLE_BATTLE_TEST("Dazzling Dash compares Speed before the target's Weak Armor activates")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_KABUTO) { Speed(90); Ability(ABILITY_WEAK_ARMOR); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DAZZLING_DASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_DASH, player);
        MESSAGE("A critical hit!");
        ABILITY_POPUP(opponent, ABILITY_WEAK_ARMOR);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Haunting Shrieks does not use the user's Sp. Atk stages for fainted teammates", s16 damage[2])
{
    bool32 boost;
    PARAMETRIZE { boost = FALSE; }
    PARAMETRIZE { boost = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); }
        PLAYER(SPECIES_GASTLY) { HP(0); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, boost ? MOVE_NASTY_PLOT : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HAUNTING_SHRIEKS); }
    } SCENE {
        for (u32 j = 0; j < 2; j++) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HAUNTING_SHRIEKS, player);
            HP_BAR(opponent, captureDamage: &results[i].damage[j]);
        }
    } FINALLY {
        EXPECT_GT(results[1].damage[0], results[0].damage[0]);
        EXPECT_EQ(results[1].damage[1], results[0].damage[1]);
    }
}
