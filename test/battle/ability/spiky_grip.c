#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Spiky Grip traps on contact and deals one sixth max HP per turn without stacking")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Level(40); }
        OPPONENT(SPECIES_MAKUHITA) { HP(240); MaxHP(240); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
        TURN {}
    } SCENE {
        HP_BAR(opponent, damage: 40);
        ABILITY_POPUP(player, ABILITY_SPIKY_GRIP);
        MESSAGE("The opposing Makuhita was caught in Spiky Grip!");
        MESSAGE("The opposing Makuhita is hurt by Spiky Grip!");
        HP_BAR(opponent, damage: 40);
        NOT ABILITY_POPUP(player, ABILITY_SPIKY_GRIP);
    } THEN {
        EXPECT_EQ(opponent->hp, 40);
        EXPECT_EQ((u32)opponent->volatiles.spikyGrip, B_BATTLER_0 + 1);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Spiky Grip ends when Pinsir switches out or faints")
{
    bool32 faint;
    PARAMETRIZE { faint = FALSE; }
    PARAMETRIZE { faint = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Level(40); HP(1); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_MAKUHITA) { HP(240); MaxHP(240); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); }
        if (faint) {
            TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
        } else {
            TURN { SWITCH(player, 1); }
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 160);
        EXPECT(!opponent->volatiles.spikyGrip);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
        EXPECT(!IsSpikyGripPreventingMove(B_BATTLER_1, MOVE_TACKLE, FALSE));
    }
}

SINGLE_BATTLE_TEST("Spiky Grip needs a damaging contact hit on the target itself")
{
    enum Move guard, move;
    u32 expectedHp;
    PARAMETRIZE { guard = MOVE_SUBSTITUTE; move = MOVE_SEISMIC_TOSS; expectedHp = 180; }
    PARAMETRIZE { guard = MOVE_PROTECT; move = MOVE_SEISMIC_TOSS; expectedHp = 240; }
    PARAMETRIZE { guard = MOVE_CELEBRATE; move = MOVE_DRAGON_RAGE; expectedHp = 200; }
    PARAMETRIZE { guard = MOVE_CELEBRATE; move = MOVE_GROWL; expectedHp = 240; }
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Level(40); Speed(50); }
        OPPONENT(SPECIES_MAKUHITA) { HP(240); MaxHP(240); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, guard); MOVE(player, move); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SPIKY_GRIP);
    } THEN {
        EXPECT(!opponent->volatiles.spikyGrip);
        EXPECT_EQ(opponent->hp, expectedHp);
    }
}

SINGLE_BATTLE_TEST("Spiky Grip blocks physical moves already chosen and leaves special and status moves available")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Level(40); Speed(100); }
        OPPONENT(SPECIES_MAKUHITA) { HP(400); MaxHP(400); Speed(50); Moves(MOVE_TACKLE, MOVE_WATER_GUN, MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("Spiky Grip prevents physical moves!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
    } THEN {
        EXPECT_EQ(CheckMoveLimitations(B_BATTLER_1, 0, MOVE_LIMITATION_SPIKY_GRIP), 1u << 0);
    }
}

SINGLE_BATTLE_TEST("Spiky Grip respects Ghost escape and Magic Guard's indirect damage immunity")
{
    bool32 ghost;
    s16 damage;
    PARAMETRIZE { ghost = FALSE; }
    PARAMETRIZE { ghost = TRUE; }
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Level(40); Attack(60); }
        OPPONENT(ghost ? SPECIES_GASTLY : SPECIES_MAKUHITA) { Ability(ghost ? ABILITY_LEVITATE : ABILITY_MAGIC_GUARD); HP(120); MaxHP(120); Defense(80); }
    } WHEN {
        TURN { MOVE(player, ghost ? MOVE_BITE : MOVE_SEISMIC_TOSS, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(opponent->hp, 120 - damage - (ghost ? 20 : 0));
        EXPECT_EQ(CanBattlerEscape(B_BATTLER_1), ghost);
    }
}

SINGLE_BATTLE_TEST("Spiky Grip blocks Photon Geyser only when it resolves as physical")
{
    bool32 physical;
    PARAMETRIZE { physical = TRUE; }
    PARAMETRIZE { physical = FALSE; }
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); Speed(100); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_NECROZMA) { Speed(50); Attack(physical ? 100 : 50); SpAttack(physical ? 50 : 100); }
    } WHEN {
        TURN { MOVE(player, MOVE_SEISMIC_TOSS); MOVE(opponent, MOVE_PHOTON_GEYSER); }
    } SCENE {
        if (physical) {
            MESSAGE("Spiky Grip prevents physical moves!");
            NOT HP_BAR(player);
        } else {
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(IsSpikyGripPreventingMove(B_BATTLER_1, MOVE_PHOTON_GEYSER, FALSE), physical);
    }
}

DOUBLE_BATTLE_TEST("Spiky Grip traps each enemy hit by a spread contact move and leaves allies free")
{
    GIVEN {
        PLAYER(SPECIES_PINSIR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_BRUTAL_SWING); }
    } THEN {
        EXPECT(!playerRight->volatiles.spikyGrip);
        EXPECT_EQ((u32)opponentLeft->volatiles.spikyGrip, B_BATTLER_0 + 1);
        EXPECT_EQ((u32)opponentRight->volatiles.spikyGrip, B_BATTLER_0 + 1);
    }
}
