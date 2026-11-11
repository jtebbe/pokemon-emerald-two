#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

TEST("Gravity Falls uses exact height thresholds and doubles only the bonus")
{
    static const struct
    {
        u16 targetHeight;
        uq4_12_t normal;
        uq4_12_t gravity;
    } cases[] = {
        {101, UQ_4_12(1.0),  UQ_4_12(1.0)},
        {100, UQ_4_12(1.0),  UQ_4_12(1.0)},
        { 99, UQ_4_12(1.25), UQ_4_12(1.5)},
        { 76, UQ_4_12(1.25), UQ_4_12(1.5)},
        { 75, UQ_4_12(1.5),  UQ_4_12(2.0)},
        { 51, UQ_4_12(1.5),  UQ_4_12(2.0)},
        { 50, UQ_4_12(1.75), UQ_4_12(2.5)},
        { 26, UQ_4_12(1.75), UQ_4_12(2.5)},
        { 25, UQ_4_12(2.0),  UQ_4_12(3.0)},
        {  0, UQ_4_12(2.0),  UQ_4_12(3.0)},
    };

    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        EXPECT_EQ(GetGravityFallsDamageModifier(100, cases[i].targetHeight, FALSE), cases[i].normal);
        EXPECT_EQ(GetGravityFallsDamageModifier(100, cases[i].targetHeight, TRUE), cases[i].gravity);
    }
}

SINGLE_BATTLE_TEST("Gravity Falls boosts physical and special damage only with the Apex Meteorite", s16 normal; s16 gravity)
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { move = MOVE_TACKLE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_TACKLE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_APEX_METEORITE; }
    GIVEN {
        ASSUME(GetSpeciesHeight(SPECIES_WOBBUFFET) * 4 <= GetSpeciesHeight(SPECIES_EXEGGUTOR_ALOLA));
        PLAYER(SPECIES_EXEGGUTOR_ALOLA) { Ability(ABILITY_HARVEST); Item(item); Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
        TURN { MOVE(opponent, MOVE_GRAVITY); }
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].normal);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
        HP_BAR(opponent, captureDamage: &results[i].gravity);
    } FINALLY {
        for (u32 base = 0; base < 4; base += 2)
        {
            EXPECT_EQ(results[base].normal, results[base].gravity);
            EXPECT_EQ(results[base + 1].normal, 2 * results[base].normal);
            EXPECT_EQ(results[base + 1].gravity, 3 * results[base].normal);
        }
    }
}
