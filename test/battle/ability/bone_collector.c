#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Bone Collector adds 20 percent base power for each fainted ally or foe with the Meteorite")
{
    enum Item item;
    bool32 allyFainted, foeFainted;
    PARAMETRIZE { item = ITEM_APEX_METEORITE; allyFainted = FALSE; foeFainted = FALSE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; allyFainted = TRUE;  foeFainted = FALSE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; allyFainted = FALSE; foeFainted = TRUE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; allyFainted = TRUE;  foeFainted = TRUE; }
    PARAMETRIZE { item = ITEM_NONE;           allyFainted = TRUE;  foeFainted = TRUE; }
    GIVEN {
        PLAYER(SPECIES_CUBONE) { Item(item); }
        PLAYER(SPECIES_WOBBUFFET) { HP(allyFainted ? 0 : 1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(foeFainted ? 0 : 1); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        u32 bonus = item == ITEM_APEX_METEORITE ? 20 * (allyFainted + foeFainted) : 0;
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_TACKLE) * (100 + bonus) / 100);
    }
}

DOUBLE_BATTLE_TEST("Bone Collector counts shared parties once and immediately loses the bonus for a revived Pokemon")
{
    bool32 revive;
    PARAMETRIZE { revive = FALSE; }
    PARAMETRIZE { revive = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(0); }
    } WHEN {
        TURN {
            if (revive)
                USE_ITEM(playerRight, ITEM_REVIVE, 2);
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
        }
    } THEN {
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_TACKLE) * (revive ? 120 : 140) / 100);
    }
}

MULTI_BATTLE_TEST("Bone Collector counts all four trainers and caps at triple base power")
{
    u32 faintedInLastParty;
    PARAMETRIZE { faintedInLastParty = 0; }
    PARAMETRIZE { faintedInLastParty = 1; }
    PARAMETRIZE { faintedInLastParty = 3; }
    GIVEN {
        PLAYER(SPECIES_MAROWAK) { Item(ITEM_APEX_METEORITE); }
        PARTNER(SPECIES_WOBBUFFET);
        OPPONENT_A(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
        OPPONENT_B(SPECIES_WOBBUFFET);
        for (u32 i = 0; i < 3; i++)
        {
            PLAYER(SPECIES_WOBBUFFET) { HP(0); }
            PARTNER(SPECIES_WOBBUFFET) { HP(0); }
            OPPONENT_A(SPECIES_WOBBUFFET) { HP(0); }
            OPPONENT_B(SPECIES_WOBBUFFET) { HP(i < faintedInLastParty ? 0 : 1); }
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); }
    } THEN {
        EXPECT_EQ(gBattleMovePower, GetMovePower(MOVE_TACKLE) * (faintedInLastParty == 0 ? 280 : 300) / 100);
    }
}
