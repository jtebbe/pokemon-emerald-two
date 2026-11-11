#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Double Teaming boosts base power independently of Adaptability STAB")
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_STRENGTH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_STRENGTH; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Ability(ABILITY_EARLY_BIRD); Item(item); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        bool32 apex = item == ITEM_APEX_METEORITE;
        struct DamageContext ctx = {
            .battlerAtk = B_BATTLER_0,
            .move = move,
            .moveType = GetMoveType(move),
            .abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0),
        };
        EXPECT_EQ(gBattleMovePower, GetMovePower(move) * (apex ? 130 : 100) / 100);
        EXPECT_EQ(GetSameTypeAttackBonusModifier(&ctx), move == MOVE_WATER_GUN ? UQ_4_12(1.0) : apex ? UQ_4_12(2.0) : UQ_4_12(1.5));
    }
}

SINGLE_BATTLE_TEST("Double Teaming keeps Adaptability STAB after Terastallizing")
{
    enum Type teraType;
    enum Move move;
    PARAMETRIZE { teraType = TYPE_NORMAL; move = MOVE_STRENGTH; }
    PARAMETRIZE { teraType = TYPE_WATER; move = MOVE_WATER_GUN; }
    GIVEN {
        // Force the ability to test the STAB calculation. The Meteorite occupies the gimmick slot.
        PLAYER(SPECIES_KANGASKHAN) { Ability(ABILITY_DOUBLE_TEAMING); TeraType(teraType); }
        OPPONENT(SPECIES_MAKUHITA) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, gimmick: GIMMICK_TERA); }
    } THEN {
        struct DamageContext ctx = {
            .battlerAtk = B_BATTLER_0,
            .move = move,
            .moveType = teraType,
            .abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0),
        };
        EXPECT_EQ(GetTeraMultiplier(&ctx), teraType == TYPE_NORMAL ? UQ_4_12(2.25) : UQ_4_12(2.0));
    }
}

SINGLE_BATTLE_TEST("Double Teaming gives Normal and Fighting moves Scrappy's Ghost immunity bypass")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_KARATE_CHOP; }
    GIVEN {
        PLAYER(SPECIES_KANGASKHAN) { Ability(ABILITY_EARLY_BIRD); Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_DUSKULL) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Double Teaming inherits Scrappy's generation-specific Intimidate immunity")
{
    u32 gen;
    PARAMETRIZE { gen = GEN_7; }
    PARAMETRIZE { gen = GEN_8; }
    GIVEN {
        WITH_CONFIG(B_UPDATED_INTIMIDATE, gen);
        PLAYER(SPECIES_KANGASKHAN) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (gen < GEN_8));
    }
}
