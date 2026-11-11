#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Training Pupil raises Attack and Defense on entry and boosts only punches and kicks")
{
    enum Move move;
    enum Item item;
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_ICE_PUNCH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_AQUA_STEP; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;           move = MOVE_ICE_PUNCH; }
    GIVEN {
        PLAYER(SPECIES_TYROGUE) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        bool32 apex = item == ITEM_APEX_METEORITE;
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + apex);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + apex);
        EXPECT_EQ(gBattleMovePower, GetMovePower(move) * (apex && move != MOVE_TACKLE ? 120 : 100) / 100);
    }
}

SINGLE_BATTLE_TEST("The punching and kicking masters add a 30 percent second strike for one PP")
{
    enum Species species;
    enum Move move;
    s16 first, second;
    PARAMETRIZE { species = SPECIES_HITMONCHAN; move = MOVE_ICE_PUNCH; }
    PARAMETRIZE { species = SPECIES_HITMONLEE;  move = MOVE_AQUA_STEP; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); MovesWithPP({move, 5}); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Defense(100); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0), secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &first);
        HP_BAR(opponent, captureDamage: &second);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(second, uq4_12_multiply_by_int_half_down(UQ_4_12(0.3), first));
        EXPECT_EQ(gBattleMovePower, GetMovePower(move) * 120 / 100);
        EXPECT_EQ(player->pp[0], 4);
    }
}

SINGLE_BATTLE_TEST("The martial masters move first only within the priority bracket and with their own move class")
{
    enum Species species;
    enum Move move, foeMove;
    bool32 first;
    PARAMETRIZE { species = SPECIES_HITMONCHAN; move = MOVE_ICE_PUNCH; foeMove = MOVE_SCRATCH;      first = TRUE; }
    PARAMETRIZE { species = SPECIES_HITMONLEE;  move = MOVE_AQUA_STEP; foeMove = MOVE_SCRATCH;      first = TRUE; }
    PARAMETRIZE { species = SPECIES_HITMONLEE;  move = MOVE_AQUA_STEP; foeMove = MOVE_QUICK_ATTACK; first = FALSE; }
    PARAMETRIZE { species = SPECIES_HITMONLEE;  move = MOVE_ICE_PUNCH; foeMove = MOVE_SCRATCH;      first = FALSE; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); Speed(10); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(300); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, foeMove); }
    } SCENE {
        if (first)
        {
            ANIMATION(ANIM_TYPE_MOVE, move, player);
            ANIMATION(ANIM_TYPE_MOVE, foeMove, opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, foeMove, opponent);
            ANIMATION(ANIM_TYPE_MOVE, move, player);
        }
    }
}

SINGLE_BATTLE_TEST("Kicking Master preserves Double Kick, Triple Kick, and Triple Axel's native hits")
{
    enum Move move;
    u32 hits;
    PARAMETRIZE { move = MOVE_DOUBLE_KICK; hits = 2; }
    PARAMETRIZE { move = MOVE_TRIPLE_KICK; hits = 3; }
    PARAMETRIZE { move = MOVE_TRIPLE_AXEL; hits = 3; }
    GIVEN {
        PLAYER(SPECIES_HITMONLEE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        for (u32 i = 0; i < hits; i++)
            HP_BAR(opponent);
        NOT HP_BAR(opponent);
    }
}
