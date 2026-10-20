#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_METEORIC_WRATH) == EFFECT_METEORIC_WRATH);
    ASSUME(GetMoveType(MOVE_METEORIC_WRATH) == TYPE_PSYCHIC);
    ASSUME(GetMovePP(MOVE_METEORIC_WRATH) == 1);
    ASSUME(GetMoveCategory(MOVE_METEORIC_WRATH) == DAMAGE_CATEGORY_STATUS);
    ASSUME(GetMoveEffect(MOVE_INVERSION) == EFFECT_INVERSION);
    ASSUME(GetMoveType(MOVE_INVERSION) == TYPE_NORMAL);
    ASSUME(GetMovePP(MOVE_INVERSION) == 1);
    ASSUME(GetMoveCategory(MOVE_INVERSION) == DAMAGE_CATEGORY_STATUS);
    ASSUME(GetMoveEffect(MOVE_CLOTHESLINE) == EFFECT_CLOTHESLINE);
    ASSUME(GetMoveType(MOVE_CLOTHESLINE) == TYPE_FIGHTING);
    ASSUME(GetMovePower(MOVE_CLOTHESLINE) == 40);
    ASSUME(GetMoveAccuracy(MOVE_CLOTHESLINE) == 95);
    ASSUME(GetMovePP(MOVE_CLOTHESLINE) == 16);
    ASSUME(MoveMakesContact(MOVE_CLOTHESLINE));
    ASSUME(GetMoveCategory(MOVE_CLOTHESLINE) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveEffect(MOVE_ANGSTY_SCREAM) == EFFECT_ANGSTY_SCREAM);
    ASSUME(GetMoveType(MOVE_ANGSTY_SCREAM) == TYPE_DARK);
    ASSUME(GetMovePower(MOVE_ANGSTY_SCREAM) == 60);
    ASSUME(GetMovePP(MOVE_ANGSTY_SCREAM) == 16);
    ASSUME(IsSoundMove(MOVE_ANGSTY_SCREAM));
    ASSUME(MoveIgnoresSubstitute(MOVE_ANGSTY_SCREAM));
}

SINGLE_BATTLE_TEST("Meteoric Wrath ticks once per turn and strikes two turns after casting")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); }
        for (u32 j = 1; j < turns; j++)
            TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("A devastating meteor is approaching!");
        if (turns < 3) {
            NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, player);
        } else {
            MESSAGE("The meteor crashed into the battlefield!");
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, player);
            HP_BAR(player, hp: 0);
            HP_BAR(opponent, hp: 0);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->meteoricWrathTimer, 3 - turns);
        EXPECT_EQ(player->hp, turns == 3 ? 0 : player->maxHP);
        EXPECT_EQ(opponent->hp, turns == 3 ? 0 : opponent->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Meteoric Wrath KOs all four battlers in one impact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_METEORIC_WRATH); }
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, playerLeft);
        HP_BAR(playerLeft, hp: 0);
        HP_BAR(opponentLeft, hp: 0);
        HP_BAR(playerRight, hp: 0);
        HP_BAR(opponentRight, hp: 0);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(playerRight->hp, 0);
        EXPECT_EQ(opponentLeft->hp, 0);
        EXPECT_EQ(opponentRight->hp, 0);
        EXPECT_EQ(gBattleStruct->meteoricWrathTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Meteoric Wrath fails while a meteor is en route without resetting its timer")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); MOVE(opponent, MOVE_METEORIC_WRATH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METEORIC_WRATH, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_METEORIC_WRATH, opponent);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(gBattleStruct->meteoricWrathTimer, 2);
        EXPECT_EQ(gBattleStruct->meteoricWrathBattler, B_BATTLER_0);
    }
}

SINGLE_BATTLE_TEST("Meteoric Wrath survives switching and does not strike replacements twice")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); }
        TURN { SWITCH(player, 1); }
        TURN { SEND_OUT(player, 0); SEND_OUT(opponent, 1); }
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, player);
        HP_BAR(player, hp: 0);
        HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT_EQ(gBattleStruct->meteoricWrathTimer, 0);
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Meteoric Wrath still lands after its caster faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
        TURN {}
        TURN {}
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
        MESSAGE("The meteor crashed into the battlefield!");
        HP_BAR(player, hp: 0);
        HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(opponent->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Meteoric Wrath ignores protection, immunities, Magic Guard and survival effects")
{
    enum Ability ability;
    enum Item item;
    enum Move protection;
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; item = ITEM_FOCUS_SASH; protection = MOVE_PROTECT; }
    PARAMETRIZE { ability = ABILITY_STURDY; item = ITEM_NONE; protection = MOVE_ENDURE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_UMBREON) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(player, MOVE_METEORIC_WRATH); }
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, protection); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_METEORIC_WRATH_IMPACT, player);
        HP_BAR(player, hp: 0);
        HP_BAR(opponent, hp: 0);
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(opponent->hp, 0);
    }
}

SINGLE_BATTLE_TEST("Inversion toggles inverse type matchups")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VAPOREON);
    } WHEN {
        TURN { MOVE(player, MOVE_INVERSION); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("Type matchups were inverted!");
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    } THEN {
        EXPECT(IsTypeChartInverted());
    }
}

SINGLE_BATTLE_TEST("Inversion restores the normal chart when used again")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_INVERSION); MOVE(opponent, MOVE_INVERSION); }
    } SCENE {
        MESSAGE("Type matchups were inverted!");
        MESSAGE("Type matchups returned to normal!");
    } THEN {
        EXPECT(!IsTypeChartInverted());
    }
}

SINGLE_BATTLE_TEST("Inversion toggles the configured inverse battle flag instead of stacking with it")
{
    GIVEN {
        FLAG_SET(B_FLAG_INVERSE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VAPOREON);
    } WHEN {
        TURN { MOVE(player, MOVE_INVERSION); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("Type matchups returned to normal!");
        HP_BAR(opponent);
        MESSAGE("It's not very effective…");
    } THEN {
        EXPECT(!IsTypeChartInverted());
    }
}

DOUBLE_BATTLE_TEST("Inversion toggles once in doubles and lasts beyond five turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_INVERSION); }
        for (u32 j = 0; j < 6; j++)
            TURN {}
    } SCENE {
        MESSAGE("Type matchups were inverted!");
        NOT MESSAGE("Type matchups returned to normal!");
    } THEN {
        EXPECT(IsTypeChartInverted());
    }
}

SINGLE_BATTLE_TEST("Inversion lets Ground hit Flying but does not negate Levitate")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_TORNADUS; }
    PARAMETRIZE { species = SPECIES_EELEKTROSS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_INVERSION); }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        if (species == SPECIES_TORNADUS) {
            HP_BAR(opponent);
            MESSAGE("It's super effective!");
        } else {
            ABILITY_POPUP(opponent, ABILITY_LEVITATE);
            NOT HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Inversion preserves the inverse Ground weakness of Flying types holding Iron Ball")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TORNADUS) { Item(ITEM_IRON_BALL); }
    } WHEN {
        TURN { MOVE(player, MOVE_INVERSION); }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Clothesline gains 40 power per positive Speed stage", s16 damage)
{
    enum Move setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_AGILITY; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_CLOTHESLINE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(3.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Clothesline adds both battlers' boosts but does not subtract negative stages", s16 damage)
{
    u32 setup;
    PARAMETRIZE { setup = 0; }
    PARAMETRIZE { setup = 1; }
    PARAMETRIZE { setup = 2; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(player, setup == 0 ? MOVE_AGILITY : MOVE_CELEBRATE);
            MOVE(opponent, setup == 0 ? MOVE_AGILITY : setup == 1 ? MOVE_SCARY_FACE : MOVE_CELEBRATE);
        }
        if (setup != 0) {
            TURN { MOVE(opponent, MOVE_AGILITY); }
            TURN { MOVE(opponent, MOVE_AGILITY); }
        }
        TURN { MOVE(player, MOVE_CLOTHESLINE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Clothesline uses positive Speed stages from either battler", s16 damage)
{
    bool32 attackerBoosts;
    PARAMETRIZE { attackerBoosts = TRUE; }
    PARAMETRIZE { attackerBoosts = FALSE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN {
            MOVE(player, attackerBoosts ? MOVE_AGILITY : MOVE_CELEBRATE);
            MOVE(opponent, attackerBoosts ? MOVE_CELEBRATE : MOVE_AGILITY);
        }
        TURN { MOVE(player, MOVE_CLOTHESLINE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Clothesline ignores negative Speed stages and caps its power at 200", s16 damage)
{
    u32 boosts;
    PARAMETRIZE { boosts = 0; }
    PARAMETRIZE { boosts = 1; }
    PARAMETRIZE { boosts = 2; }
    PARAMETRIZE { boosts = 3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(1000); MaxHP(1000); }
    } WHEN {
        for (u32 j = 0; j < 3; j++) {
            TURN { MOVE(player, j < boosts ? MOVE_AGILITY : MOVE_CELEBRATE); MOVE(opponent, MOVE_SCARY_FACE); }
        }
        // Opposing drops cancel the user's boosts; boost the opponent instead.
        for (u32 j = 0; j < boosts; j++)
            TURN { MOVE(opponent, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_CLOTHESLINE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
        EXPECT_LT(results[1].damage, results[2].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Angsty Scream doubles only below half HP or with non-volatile status", s16 damage)
{
    u32 hp, status;
    PARAMETRIZE { hp = 400; status = STATUS1_NONE; }
    PARAMETRIZE { hp = 200; status = STATUS1_NONE; }
    PARAMETRIZE { hp = 199; status = STATUS1_NONE; }
    PARAMETRIZE { hp = 400; status = STATUS1_POISON; }
    PARAMETRIZE { hp = 199; status = STATUS1_POISON; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(400); HP(hp); Status1(status); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_ANGSTY_SCREAM, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_GE(results[2].damage, results[0].damage * 2 - 5);
        EXPECT_LE(results[2].damage, results[0].damage * 2 + 1);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[2].damage, results[4].damage);
    }
}

SINGLE_BATTLE_TEST("Angsty Scream bypasses Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(1000); HP(1000); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_ANGSTY_SCREAM); MOVE(opponent, MOVE_SUBSTITUTE); }
    } THEN {
        EXPECT_LT(opponent->hp, 750);
        EXPECT_GT((u32)opponent->volatiles.substituteHP, 0);
    }
}

SINGLE_BATTLE_TEST("Angsty Scream is blocked by Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_ANGSTY_SCREAM); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SOUNDPROOF);
        NOT HP_BAR(opponent);
    }
}
