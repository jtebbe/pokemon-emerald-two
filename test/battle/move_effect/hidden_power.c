#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Hidden Power remains 60 BP and special, choosing Psychic against Levitate Koffing")
{
    GIVEN {
        ASSUME(GetMovePower(MOVE_HIDDEN_POWER) == 60);
        ASSUME(GetMoveType(MOVE_HIDDEN_POWER) == TYPE_PSYCHIC);
        ASSUME(GetMoveCategory(MOVE_HIDDEN_POWER) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING) { Item(ITEM_PAYAPA_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_HIDDEN_POWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HIDDEN_POWER, player);
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Hidden Power picks the strongest type matchup and avoids absorbers")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }
    PARAMETRIZE { ability = ABILITY_VOLT_ABSORB; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GYARADOS) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_HIDDEN_POWER); } } SCENE {
        HP_BAR(opponent);
        if (ability == ABILITY_INTIMIDATE) MESSAGE("It's extremely effective!");
        else MESSAGE("It's super effective!");
    } THEN {
        enum Type type = gLastUsedMoveType[B_BATTLER_0];
        if (ability == ABILITY_INTIMIDATE)
            EXPECT_EQ(type, TYPE_ELECTRIC);
        else
            EXPECT_EQ(type, TYPE_ROCK);
    }
}

SINGLE_BATTLE_TEST("Hidden Power randomly chooses among tied best matchups")
{
    u32 index;
    enum Type type;
    PARAMETRIZE { index = 0; type = TYPE_FIGHTING; }
    PARAMETRIZE { index = 1; type = TYPE_ROCK; }
    PARAMETRIZE { index = 2; type = TYPE_STEEL; }
    PARAMETRIZE { index = 3; type = TYPE_FIRE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLACEON) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_HIDDEN_POWER, WITH_RNG(RNG_HIDDEN_POWER_TYPE, index)); } } THEN {
        EXPECT_EQ(gLastUsedMoveType[B_BATTLER_0], type);
    }
}

SINGLE_BATTLE_TEST("Hidden Power respects Normalize and lets Electrify override Normalize")
{
    bool32 electrify;
    PARAMETRIZE { electrify = FALSE; }
    PARAMETRIZE { electrify = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NORMALIZE); Speed(1); }
        OPPONENT(SPECIES_GASTLY) { Speed(100); }
    } WHEN { TURN { if (electrify) MOVE(opponent, MOVE_ELECTRIFY); MOVE(player, MOVE_HIDDEN_POWER); } }
    SCENE {
        if (electrify) HP_BAR(opponent);
        else { NOT HP_BAR(opponent); MESSAGE("It doesn't affect the opposing Gastly…"); }
    } THEN {
        EXPECT_EQ(gLastUsedMoveType[B_BATTLER_0], electrify ? TYPE_ELECTRIC : TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Hidden Power AI forecasts the real target and leaves battle state intact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GYARADOS) { Ability(ABILITY_VOLT_ABSORB); HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        struct DamageContext ctx = { .move = MOVE_HIDDEN_POWER, .chosenMove = MOVE_HIDDEN_POWER,
            .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .aiCalc = TRUE };
        ctx.abilities[B_BATTLER_0] = ABILITY_SHADOW_TAG;
        ctx.abilities[B_BATTLER_1] = ABILITY_VOLT_ABSORB;
        u32 hp = opponent->hp;
        s16 passive = gBattleStruct->passiveHpUpdate[B_BATTLER_1];
        EXPECT_EQ(GetHiddenPowerMoveType(&ctx, FALSE), TYPE_ROCK);
        EXPECT_EQ(opponent->hp, hp);
        EXPECT_EQ(gBattleStruct->passiveHpUpdate[B_BATTLER_1], passive);
    }
}
