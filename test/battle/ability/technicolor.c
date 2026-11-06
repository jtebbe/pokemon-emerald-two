#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Technicolor gives Kecleon distinct new types on entry and at each turn end")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT_TYPE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT_TYPE, opponent);
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        NOT ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(SPECIES_KECLEON), ABILITY_TECHNICOLOR);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_TECHNICOLOR);
        EXPECT(IsTechnicolorType(player->types[0])); EXPECT(IsTechnicolorType(player->types[1]));
        EXPECT_NE(player->types[0], player->types[1]);
        for (u32 first = 0; first < 2; first++)
            for (u32 second = 0; second < 2; second++)
                EXPECT_NE(player->types[first], opponent->types[second]);
        EXPECT_EQ(GetTechnicolorTypeHistory(B_BATTLER_0)[0], player->types[0]);
        EXPECT_EQ(GetTechnicolorTypeHistory(B_BATTLER_0)[1], player->types[1]);
    }
}

SINGLE_BATTLE_TEST("Technicolor excludes Stellar None and Mystery from both type slots across repeated rolls")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        for (u32 roll = 0; roll < 100; roll++) {
            enum Type old1 = player->types[0], old2 = player->types[1];
            EXPECT(TryTechnicolorTypeChange(B_BATTLER_0));
            EXPECT(IsTechnicolorType(player->types[0])); EXPECT(IsTechnicolorType(player->types[1]));
            EXPECT_NE(player->types[0], player->types[1]);
            EXPECT_NE(player->types[0], old1); EXPECT_NE(player->types[0], old2);
            EXPECT_NE(player->types[1], old1); EXPECT_NE(player->types[1], old2);
        }
        EXPECT(!IsTechnicolorType(TYPE_NONE));
        EXPECT(!IsTechnicolorType(TYPE_MYSTERY));
        EXPECT(!IsTechnicolorType(TYPE_STELLAR));
    }
}

SINGLE_BATTLE_TEST("Technicolor remembers its last pair when switching resets its visible types")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        enum Type *history = GetTechnicolorTypeHistory(B_BATTLER_0);
        history[0] = TYPE_FIRE;
        history[1] = TYPE_WATER;
        SET_BATTLER_TYPE(B_BATTLER_0, TYPE_NORMAL);
        enum Type pool[NUMBER_OF_MON_TYPES];
        EXPECT_EQ(GetTechnicolorTypePool(B_BATTLER_0, pool), 16);
        EXPECT(TryTechnicolorTypeChange(B_BATTLER_0));
        EXPECT_NE(player->types[0], TYPE_FIRE); EXPECT_NE(player->types[0], TYPE_WATER);
        EXPECT_NE(player->types[1], TYPE_FIRE); EXPECT_NE(player->types[1], TYPE_WATER);
    }
}

SINGLE_BATTLE_TEST("Technicolor activates again on entry and never rolls while benched")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN {}
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        NOT ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
    }
}

SINGLE_BATTLE_TEST("Technicolor requires an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_COLOR_CHANGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_COLOR_CHANGE);
        EXPECT_EQ(player->types[0], TYPE_NORMAL);
        EXPECT_EQ(player->types[1], TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Technicolor's Normal attacks select the uniquely most effective type")
{
    enum Species species;
    enum Type type;
    PARAMETRIZE { species = SPECIES_CHARIZARD; type = TYPE_ROCK; }
    PARAMETRIZE { species = SPECIES_SWAMPERT; type = TYPE_GRASS; }
    PARAMETRIZE { species = SPECIES_SCIZOR; type = TYPE_FIRE; }
    PARAMETRIZE { species = SPECIES_TYRANITAR; type = TYPE_FIGHTING; }
    PARAMETRIZE { species = SPECIES_GARCHOMP; type = TYPE_ICE; }
    PARAMETRIZE { species = SPECIES_GYARADOS; type = TYPE_ELECTRIC; }
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(species) { Ability(ABILITY_NONE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], type);
    }
}

SINGLE_BATTLE_TEST("Technicolor converts special Normal attacks as well as physical ones")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_GRASS);
    }
}

DOUBLE_BATTLE_TEST("Technicolor chooses independently for each target of a spread move")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARIZARD) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_ROCK);
        EXPECT_EQ(gLastHitByType[B_BATTLER_3], TYPE_GRASS);
    }
}

DOUBLE_BATTLE_TEST("Technicolor chooses its attack type against the actual redirected target")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARIZARD) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_FOLLOW_ME); MOVE(playerLeft, MOVE_STRENGTH, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentLeft);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_3], TYPE_GRASS);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Technicolor's per-target types also drive Color Change after a spread move")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARIZARD) { Ability(ABILITY_COLOR_CHANGE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SWAMPERT) { Ability(ABILITY_COLOR_CHANGE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } THEN {
        EXPECT_EQ(opponentLeft->types[0], TYPE_ROCK);
        EXPECT_EQ(opponentRight->types[0], TYPE_GRASS);
    }
}

DOUBLE_BATTLE_TEST("Technicolor resolves each spread target's category under Distortion", s16 rock; s16 grass)
{
    u32 specialAttack;
    PARAMETRIZE { specialAttack = 100; }
    PARAMETRIZE { specialAttack = 200; }
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(specialAttack); }
        PLAYER(SPECIES_GIRATINA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].rock);
        HP_BAR(opponentRight, captureDamage: &results[i].grass);
    } FINALLY {
        EXPECT_EQ(results[0].rock, results[1].rock);
        EXPECT_GT(results[1].grass, results[0].grass * 3 / 2);
    }
}

SINGLE_BATTLE_TEST("Technicolor avoids types blocked by Levitate Air Balloon and absorption abilities")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_LEVITATE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_AIR_BALLOON; }
    PARAMETRIZE { ability = ABILITY_EARTH_EATER; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_AGGRON) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_FIGHTING);
    }
}

SINGLE_BATTLE_TEST("Technicolor can choose a weakness that bypasses Wonder Guard")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SPIRITOMB) { Ability(ABILITY_WONDER_GUARD); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_FAIRY);
    }
}

SINGLE_BATTLE_TEST("Technicolor type ties prefer STAB and selection does not prepare a rejected absorption")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_TORKOAL) { Ability(ABILITY_WATER_ABSORB); HP(500); MaxHP(1000); }
    } WHEN {
        TURN {}
    } THEN {
        player->types[0] = TYPE_ROCK; player->types[1] = TYPE_GRASS;
        struct DamageContext ctx = {0};
        ctx.battlerAtk = B_BATTLER_0; ctx.battlerDef = B_BATTLER_1;
        ctx.move = ctx.chosenMove = MOVE_TACKLE; ctx.moveType = TYPE_NORMAL;
        ctx.abilities[0] = ABILITY_TECHNICOLOR; ctx.abilities[1] = ABILITY_WATER_ABSORB;
        gBattleStruct->passiveHpUpdate[1] = 37;
        gBattleCommunication[MULTISTRING_CHOOSER] = 7;
        EXPECT_EQ(GetTechnicolorMoveType(&ctx), TYPE_ROCK);
        EXPECT_EQ(gBattleStruct->passiveHpUpdate[1], 37);
        EXPECT_EQ(gBattleCommunication[MULTISTRING_CHOOSER], 7);
        EXPECT_EQ(opponent->hp, 500);
        EXPECT(IsTechnicolorType(GetTechnicolorMoveType(&ctx)));
    }
}

SINGLE_BATTLE_TEST("Technicolor leaves non-Normal attacks and Normal status moves unchanged")
{
    enum Move move;
    enum Type type;
    PARAMETRIZE { move = MOVE_WATER_GUN; type = TYPE_WATER; }
    PARAMETRIZE { move = MOVE_GROWL; type = TYPE_NORMAL; }
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SWAMPERT) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(gLastUsedMoveType[B_BATTLER_0], type);
    }
}

SINGLE_BATTLE_TEST("Technicolor type queries restore their target and cannot leak conversion into another battler's move")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CHARIZARD);
    } WHEN {
        TURN {}
    } THEN {
        enum BattlerId originalTarget = gBattlerTarget;
        SetTypeBeforeUsingMoveForTarget(MOVE_STRENGTH, B_BATTLER_0, B_BATTLER_1, ABILITY_TECHNICOLOR, HOLD_EFFECT_APEX_METEORITE);
        EXPECT_EQ(GetBattleMoveType(MOVE_STRENGTH), TYPE_ROCK);
        EXPECT_EQ(gBattlerTarget, originalTarget);
        SetTypeBeforeUsingMove(MOVE_TACKLE, B_BATTLER_1, ABILITY_BLAZE, HOLD_EFFECT_NONE);
        EXPECT_EQ(GetBattleMoveType(MOVE_TACKLE), TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Technicolor only converts Weather Ball while its resolved type is Normal")
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_SWAMPERT) { Ability(rain ? ABILITY_DRIZZLE : ABILITY_TORRENT); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], rain ? TYPE_WATER : TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Technicolor respects inverse type effectiveness")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_SCIZOR) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_INVERSION); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Technicolor cannot be suppressed by Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CHARIZARD) { Ability(ABILITY_NEUTRALIZING_GAS); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TECHNICOLOR);
    } THEN {
        EXPECT_EQ(gLastHitByType[B_BATTLER_1], TYPE_ROCK);
    }
}

SINGLE_BATTLE_TEST("Technicolor does not bypass sound-based immunity by converting Hyper Voice")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CHARIZARD) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
