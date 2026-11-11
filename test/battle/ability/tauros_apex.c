#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Furious Charge intimidates on entry, blocks Intimidate, and boosts recoil moves")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_TAUROS; }
    PARAMETRIZE { species = SPECIES_TAUROS_PALDEA_COMBAT; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, GEN_7);
        PLAYER(species) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_TAKE_DOWN) * 120 / 100);
    }
}

SINGLE_BATTLE_TEST("Furious Charge prevents major status and keeps its holder taunted after ordinary Taunt expires")
{
    enum Move statusMove;
    PARAMETRIZE { statusMove = MOVE_WILL_O_WISP; }
    PARAMETRIZE { statusMove = MOVE_TOXIC; }
    PARAMETRIZE { statusMove = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { statusMove = MOVE_SPORE; }
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Item(ITEM_APEX_METEORITE); Moves(MOVE_HOLD_BACK, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HOLD_BACK); MOVE(opponent, statusMove); }
        for (u32 i = 0; i < 5; i++)
            TURN { MOVE(player, MOVE_HOLD_BACK); }
        TURN { MOVE(player, MOVE_GROWL, allowed: FALSE); MOVE(player, MOVE_HOLD_BACK); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(IsBattlerTaunted(B_BATTLER_0, ABILITY_FURIOUS_CHARGE));
        EXPECT_EQ(CheckMoveLimitations(B_BATTLER_0, 0, MOVE_LIMITATION_TAUNT), 1u << 1);
    }
}

SINGLE_BATTLE_TEST("Blaze Heritage raises Attack and Speed on entry and reacts to stat drops with Defiant")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS_PALDEA_BLAZE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLAZE_HERITAGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
        ABILITY_POPUP(player, ABILITY_BLAZE_HERITAGE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Aqua Heritage raises both defenses on entry and blocks opposing stat drops with Clear Body")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS_PALDEA_AQUA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); }
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
