#include "global.h"
#include "test/battle.h"

AI_SINGLE_BATTLE_TEST("AI avoids a lethal Storm Sacrifice")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(25); MaxHP(100); Moves(MOVE_STORM_SACRIFICE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_WATER_GUN); }
    }
}

AI_SINGLE_BATTLE_TEST("AI accounts for Sunbloom's sunny power boost")
{
    bool32 sunny;
    PARAMETRIZE { sunny = FALSE; }
    PARAMETRIZE { sunny = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNBLOOM, MOVE_GIGA_DRAIN); }
    } WHEN {
        if (sunny)
            TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { EXPECT_MOVE(opponent, sunny ? MOVE_SUNBLOOM : MOVE_GIGA_DRAIN); }
    }
}

AI_SINGLE_BATTLE_TEST("AI accounts for Haunting Shrieks' fainted contributors")
{
    bool32 fainted;
    PARAMETRIZE { fainted = FALSE; }
    PARAMETRIZE { fainted = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_SHADOW_BALL, MOVE_HAUNTING_SHRIEKS); }
        OPPONENT(SPECIES_GASTLY) { HP(fainted ? 0 : 100); MaxHP(100); SpAttack(300); }
        OPPONENT(SPECIES_SHUPPET) { HP(fainted ? 0 : 100); MaxHP(100); SpAttack(300); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, fainted ? MOVE_HAUNTING_SHRIEKS : MOVE_SHADOW_BALL); }
    }
}

AI_SINGLE_BATTLE_TEST("AI uses the correct attacking stat for Mind Crush")
{
    bool32 highSpDef;
    PARAMETRIZE { highSpDef = FALSE; }
    PARAMETRIZE { highSpDef = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); }
        OPPONENT(SPECIES_WOBBUFFET) {
            SpAttack(highSpDef ? 10 : 300);
            SpDefense(highSpDef ? 300 : 10);
            Moves(MOVE_PSYCHIC, MOVE_MIND_CRUSH);
        }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, highSpDef ? MOVE_MIND_CRUSH : MOVE_PSYCHIC); }
    }
}
