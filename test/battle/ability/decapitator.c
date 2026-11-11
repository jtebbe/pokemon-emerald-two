#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Decapitator boosts cuts and KOs surviving targets strictly below ten percent HP")
{
    u32 hp;
    PARAMETRIZE { hp = 44; }
    PARAMETRIZE { hp = 45; }
    PARAMETRIZE { hp = 46; }
    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Item(ITEM_APEX_METEORITE); Level(50); Attack(100); }
        OPPONENT(SPECIES_MAKUHITA) { Defense(100); HP(hp); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_CUT, WITH_RNG(RNG_DAMAGE_MODIFIER, 0), criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, damage: 35);
        if (hp == 44) {
            ABILITY_POPUP(player, ABILITY_DECAPITATOR);
            HP_BAR(opponent, hp: 0);
        } else {
            NOT ABILITY_POPUP(player, ABILITY_DECAPITATOR);
        }
    } THEN {
        EXPECT_EQ(gBattleMovePower, 75);
        EXPECT_EQ(opponent->hp, hp == 44 ? 0 : hp - 35);
    }
}

SINGLE_BATTLE_TEST("Decapitator executes a target after Focus Sash leaves it below ten percent")
{
    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Item(ITEM_APEX_METEORITE); Level(50); Attack(500); }
        OPPONENT(SPECIES_MAKUHITA) { Item(ITEM_FOCUS_SASH); Defense(100); HP(100); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_CUT); }
    } SCENE {
        HP_BAR(opponent, hp: 1);
        ITEM_POPUP(opponent, ITEM_FOCUS_SASH);
        ABILITY_POPUP(player, ABILITY_DECAPITATOR);
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Decapitator only executes when a slicing move damages the actual target")
{
    bool32 substitute;
    PARAMETRIZE { substitute = TRUE; }
    PARAMETRIZE { substitute = FALSE; }
    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Item(ITEM_APEX_METEORITE); Level(50); Attack(100); }
        OPPONENT(SPECIES_MAKUHITA) { Defense(100); HP(substitute ? 65 : 25); MaxHP(200); }
    } WHEN {
        if (substitute)
            TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, substitute ? MOVE_CUT : MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0), criticalHit: FALSE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_DECAPITATOR);
    } THEN {
        EXPECT_EQ(opponent->hp, substitute ? 15 : 6);
    }
}

SINGLE_BATTLE_TEST("Decapitator inherits Inner Focus's flinch and Intimidate protection")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_8);
        PLAYER(SPECIES_SCIZOR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BITE, secondaryEffect: TRUE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
