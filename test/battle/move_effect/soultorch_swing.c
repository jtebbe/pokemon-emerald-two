#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Soultorch Swing combines Fire and Ghost type effectiveness", s16 damage)
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_MANAPHY; } // Fire resistance
    PARAMETRIZE { species = SPECIES_WOBBUFFET; } // Ghost weakness
    PARAMETRIZE { species = SPECIES_FROSLASS; } // Weak to both
    PARAMETRIZE { species = SPECIES_EEVEE; } // Ghost immunity
    GIVEN {
        ASSUME(GetMoveType(MOVE_SOULTORCH_SWING) == TYPE_FIRE);
        ASSUME(GetMoveArgType(MOVE_SOULTORCH_SWING) == TYPE_GHOST);
        ASSUME(GetMoveCategory(MOVE_SOULTORCH_SWING) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMovePower(MOVE_SOULTORCH_SWING) == 90);
        ASSUME(GetMoveAccuracy(MOVE_SOULTORCH_SWING) == 100);
        ASSUME(GetMovePP(MOVE_SOULTORCH_SWING) == 16); // 10 base PP, always PP Max'd.
        ASSUME(GetMoveTarget(MOVE_SOULTORCH_SWING) == TARGET_SELECTED);
        ASSUME(IsDanceMove(MOVE_SOULTORCH_SWING));
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); }
        OPPONENT(species) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOULTORCH_SWING, secondaryEffect: FALSE); }
    } SCENE {
        if (species == SPECIES_EEVEE)
        {
            MESSAGE("It doesn't affect the opposing Eevee…");
            NOT HP_BAR(opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SOULTORCH_SWING, player);
            HP_BAR(opponent, captureDamage: &results[i].damage);
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage / 4);
        EXPECT_EQ(results[2].damage, 2 * results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Soultorch Swing has a 30 percent chance to burn")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOULTORCH_SWING); }
    } SCENE {
        HP_BAR(opponent);
        STATUS_ICON(opponent, burn: TRUE);
    }
}
