#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Piercing Horn doubles damage for horn moves only", s16 damage)
{
    enum Move move;
    bool32 apex;
    PARAMETRIZE { move = MOVE_SEA_HORN; apex = FALSE; }
    PARAMETRIZE { move = MOVE_SEA_HORN; apex = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; apex = FALSE; }
    PARAMETRIZE { move = MOVE_TACKLE; apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SEAKING) { Ability(ABILITY_WATER_VEIL); Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetBattleMovePriorityForTarget(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), move, B_BATTLER_1),
            apex && move == MOVE_SEA_HORN ? -4 : 0);
    } FINALLY {
        EXPECT_EQ(results[1].damage, 2 * results[0].damage);
        EXPECT_EQ(results[3].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Horn moves after priority-zero moves and bypasses accuracy")
{
    GIVEN {
        PLAYER(SPECIES_GOLDEEN) { Item(ITEM_APEX_METEORITE); Speed(150); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_MEGAHORN, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGAHORN, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Piercing Horn skips Horn Drill's accuracy roll and respects its other restrictions")
{
    enum Ability ability;
    u32 level;
    PARAMETRIZE { ability = ABILITY_ROCK_HEAD; level = 50; }
    PARAMETRIZE { ability = ABILITY_ROCK_HEAD; level = 51; }
    PARAMETRIZE { ability = ABILITY_STURDY; level = 50; }
    GIVEN {
        PLAYER(SPECIES_GOLDEEN) { Item(ITEM_APEX_METEORITE); Level(50); }
        OPPONENT(SPECIES_GOLEM) { Ability(ability); Level(level); HP(200); MaxHP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_HORN_DRILL, hit: FALSE); }
    } THEN {
        EXPECT_EQ(opponent->hp, level == 50 && ability != ABILITY_STURDY ? 0 : 200);
    }
}

SINGLE_BATTLE_TEST("Sea Horn is a contact move with increased critical-hit chance and can flinch")
{
    GIVEN {
        ASSUME(GetMoveType(MOVE_SEA_HORN) == TYPE_WATER);
        ASSUME(GetMovePower(MOVE_SEA_HORN) == 75);
        ASSUME(GetMoveAccuracy(MOVE_SEA_HORN) == 100);
        ASSUME(gMovesInfo[MOVE_SEA_HORN].pp == 20);
        ASSUME(GetMovePP(MOVE_SEA_HORN) == 32);
        ASSUME(GetMoveCategory(MOVE_SEA_HORN) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveTarget(MOVE_SEA_HORN) == TARGET_SELECTED);
        ASSUME(GetMoveCriticalHitStage(MOVE_SEA_HORN) == 1);
        ASSUME(MoveMakesContact(MOVE_SEA_HORN));
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_SEA_HORN, MOVE_EFFECT_FLINCH, 30));
        PLAYER(SPECIES_GOLDEEN);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEA_HORN, secondaryEffect: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEA_HORN, player);
        MESSAGE("The opposing Wobbuffet flinched and couldn't move!");
    }
}
