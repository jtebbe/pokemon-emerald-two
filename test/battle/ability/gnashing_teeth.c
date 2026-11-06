#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Gnashing Teeth gives standard Rattata and Raticate two bites and a Defense drop per hit")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_RATTATA; }
    PARAMETRIZE { species = SPECIES_RATICATE; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet's Defense fell!");
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet's Defense fell!");
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_GNASHING_TEETH);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_GNASHING_TEETH);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->pp[0], GetMovePP(MOVE_BITE) - 1);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth is unavailable to Alolan Rattata Raticate and Totem Raticate")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_RATTATA_ALOLA; }
    PARAMETRIZE { species = SPECIES_RATICATE_ALOLA; }
    PARAMETRIZE { species = SPECIES_RATICATE_ALOLA_TOTEM; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_NE(GetSpeciesApexAbility(species), ABILITY_GNASHING_TEETH);
        EXPECT_NE(GetBattlerAbility(B_BATTLER_0), ABILITY_GNASHING_TEETH);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth requires the Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth includes Strong Jaw's 1.5x boost and a 0.3x second hit", s16 first, s16 second)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Ability(ABILITY_RUN_AWAY); Item(item); }
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        if (item == ITEM_APEX_METEORITE)
            HP_BAR(opponent, captureDamage: &results[i].second);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].first, Q_4_12(1.5), results[1].first);
        EXPECT_MUL_EQ(results[1].first, Q_4_12(0.3), results[1].second);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth's first Defense drop strengthens its second bite", s16 first, s16 second)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        HP_BAR(opponent, captureDamage: &results[i].second);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (ability == ABILITY_TELEPATHY ? 2 : 0));
    } FINALLY {
        EXPECT_EQ(results[0].first, results[1].first);
        EXPECT_GT(results[1].second, results[0].second);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth applies to every biting move")
{
    enum Move move = MOVE_BITE;
    const enum Move bites[] = {MOVE_BITE, MOVE_HYPER_FANG, MOVE_CRUNCH, MOVE_POISON_FANG, MOVE_THUNDER_FANG,
        MOVE_ICE_FANG, MOVE_FIRE_FANG, MOVE_PSYCHIC_FANGS, MOVE_JAW_LOCK, MOVE_FISHIOUS_REND};
    for (u32 i = 0; i < ARRAY_COUNT(bites); i++)
        PARAMETRIZE { move = bites[i]; }
    GIVEN {
        ASSUME(IsBitingMove(move));
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth does not modify non-biting attacks or status moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_SCARY_FACE; }
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (move == MOVE_TACKLE) {
            HP_BAR(opponent);
            NOT HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth adds its guaranteed drops to Crunch's original secondary effect")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CRUNCH, secondaryEffect: TRUE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 4);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth's Defense drops respect stat and secondary-effect protections")
{
    enum Ability ability;
    enum Item item;
    bool32 mist;
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; item = ITEM_NONE; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_FULL_METAL_BODY; item = ITEM_NONE; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_BIG_PECKS; item = ITEM_NONE; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; item = ITEM_CLEAR_AMULET; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; item = ITEM_COVERT_CLOAK; mist = FALSE; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; item = ITEM_NONE; mist = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        if (mist)
            TURN { MOVE(opponent, MOVE_MIST); }
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth's Defense drops interact with Contrary Simple Defiant and Mirror Armor")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_CONTRARY; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    PARAMETRIZE { ability = ABILITY_DEFIANT; }
    PARAMETRIZE { ability = ABILITY_MIRROR_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (ability == ABILITY_CONTRARY ? 2 : ability == ABILITY_SIMPLE ? -4 : ability == ABILITY_MIRROR_ARMOR ? 0 : -2));
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - (ability == ABILITY_MIRROR_ARMOR ? 2 : 0));
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_DEFIANT ? 4 : 0));
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth does not drop Defense through a substitute but its echo can hit after breaking it")
{
    bool32 breaks;
    PARAMETRIZE { breaks = FALSE; }
    PARAMETRIZE { breaks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Attack(breaks ? 300 : 50); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(breaks ? 200 : 800); MaxHP(breaks ? 200 : 800); Defense(100); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        SUB_HIT(opponent);
        if (breaks) {
            MESSAGE("The opposing Wobbuffet's substitute faded!");
            HP_BAR(opponent);
        } else {
            SUB_HIT(opponent);
            NOT HP_BAR(opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - breaks);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth does not repeat missed protected or immune attacks")
{
    u32 failure;
    PARAMETRIZE { failure = 0; }
    PARAMETRIZE { failure = 1; }
    PARAMETRIZE { failure = 2; }
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(failure == 2 ? SPECIES_GENGAR : SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            if (failure == 1) { MOVE(opponent, MOVE_PROTECT); }
            MOVE(player, failure != 1 ? MOVE_HYPER_FANG : MOVE_BITE, hit: failure != 0);
        }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth uses one accuracy check for both bites")
{
    PASSES_RANDOMLY(9, 10, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_HYPER_FANG) == 90);
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_FANG); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth can break Focus Sash and Sturdy with its second hit")
{
    bool32 sturdy;
    PARAMETRIZE { sturdy = FALSE; }
    PARAMETRIZE { sturdy = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Attack(300); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(sturdy ? ABILITY_STURDY : ABILITY_TELEPATHY); Item(sturdy ? ITEM_NONE : ITEM_FOCUS_SASH); HP(100); MaxHP(100); Defense(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, hp: 1);
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth stops after its first hit knocks out the target")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); Attack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Gnashing Teeth stops its second hit if contact damage knocks out the attacker")
{
    GIVEN {
        PLAYER(SPECIES_RATTATA) { Item(ITEM_APEX_METEORITE); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player, hp: 0);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Neutralizing Gas cannot suppress Gnashing Teeth")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_GNASHING_TEETH);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}
