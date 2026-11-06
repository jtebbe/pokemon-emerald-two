#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Unbroken Stone belongs only to the non-Alolan Geodude line")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GEODUDE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRAVELER; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GEODUDE_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GRAVELER_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GOLEM; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_UNBROKEN_STONE,
            item == ITEM_APEX_METEORITE && (species == SPECIES_GEODUDE || species == SPECIES_GRAVELER || species == SPECIES_GOLEM));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone caps both double and quadruple weaknesses at neutral damage", s16 damage)
{
    bool32 apex;
    enum Move move;
    PARAMETRIZE { move = MOVE_WATER_GUN; apex = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; apex = FALSE; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; apex = TRUE; }
    PARAMETRIZE { move = MOVE_METAL_CLAW; apex = FALSE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        if (apex) NOT MESSAGE("It's super effective!");
        else if (move == MOVE_WATER_GUN) MESSAGE("It's extremely effective!");
        else MESSAGE("It's super effective!");
    } THEN {
        EXPECT_EQ(CalcPartyMonTypeEffectivenessMultiplier(move, SPECIES_GOLEM, ABILITY_UNBROKEN_STONE), UQ_4_12(1.0));
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone preserves resistances and immunities")
{
    enum Move move;
    uq4_12_t effectiveness;
    PARAMETRIZE { move = MOVE_TACKLE; effectiveness = UQ_4_12(0.5); }
    PARAMETRIZE { move = MOVE_POISON_STING; effectiveness = UQ_4_12(0.25); }
    PARAMETRIZE { move = MOVE_THUNDER_SHOCK; effectiveness = UQ_4_12(0.0); }
    GIVEN {
        PLAYER(SPECIES_GEODUDE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        if (effectiveness == UQ_4_12(0.0)) {
            MESSAGE("It doesn't affect Geodude…");
            NOT HP_BAR(player);
        } else {
            HP_BAR(player);
            if (effectiveness == UQ_4_12(0.25)) MESSAGE("It's mostly ineffective…");
            else MESSAGE("It's not very effective…");
        }
    } THEN {
        EXPECT_EQ(CalcPartyMonTypeEffectivenessMultiplier(move, SPECIES_GEODUDE, ABILITY_UNBROKEN_STONE), effectiveness);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone still caps weaknesses against Mold Breaker", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        NOT MESSAGE("It's super effective!");
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone prevents weakness-triggered boosts and leaves fixed damage unchanged")
{
    enum Item item;
    enum Move move;
    PARAMETRIZE { item = ITEM_WEAKNESS_POLICY; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_ABSORB_BULB; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_SEISMIC_TOSS; }
    GIVEN {
        // Force the Apex ability so held-item reactions can be checked independently.
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_UNBROKEN_STONE); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        if (item == ITEM_WEAKNESS_POLICY) {
            EXPECT_EQ(player->item, ITEM_WEAKNESS_POLICY);
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        }
        // Absorb Bulb triggers on Water damage regardless of effectiveness.
        if (item == ITEM_ABSORB_BULB)
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        if (move == MOVE_SEISMIC_TOSS)
            EXPECT_EQ(player->hp, 2950);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone sets Stealth Rock on the contact attacker's field")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_UNBROKEN_STONE);
        MESSAGE("Pointed stones float in the air on the opposing side!");
        HP_BAR(opponent, damage: 100);
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone places only one Stealth Rock hazard across multiple contact strikes")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ARM_THRUST, WITH_RNG(RNG_HITS, 5)); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_UNBROKEN_STONE);
        for (u32 hit = 0; hit < 4; hit++) {
            HP_BAR(player);
            NOT ABILITY_POPUP(player, ABILITY_UNBROKEN_STONE);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 1);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone requires damaging contact and respects Long Reach and Protective Pads")
{
    enum Move move;
    enum Ability ability;
    enum Item item;
    bool32 setsRocks;
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; item = ITEM_NONE; setsRocks = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_NONE; item = ITEM_NONE; setsRocks = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_LONG_REACH; item = ITEM_NONE; setsRocks = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; item = ITEM_PROTECTIVE_PADS; setsRocks = FALSE; }
    PARAMETRIZE { move = MOVE_THUNDER_PUNCH; ability = ABILITY_NONE; item = ITEM_NONE; setsRocks = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_MOLD_BREAKER; item = ITEM_NONE; setsRocks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK), setsRocks);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone does not react to contact blocked by Protect or a Substitute")
{
    bool32 substitute;
    PARAMETRIZE { substitute = FALSE; }
    PARAMETRIZE { substitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (substitute) TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, substitute ? MOVE_CELEBRATE : MOVE_PROTECT); MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone can scatter rocks even when contact knocks out its holder")
{
    GIVEN {
        PLAYER(SPECIES_GEODUDE) { Item(ITEM_APEX_METEORITE); Level(1); HP(1); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    }
}

DOUBLE_BATTLE_TEST("Unbroken Stone places rocks on a friendly contact attacker's side")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SCRATCH, target: playerLeft); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone consumes its own side's rocks before attacking and doubles only that move", s16 boosted, s16 normal)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, opponent);
        ABILITY_POPUP(player, ABILITY_UNBROKEN_STONE);
        MESSAGE("Golem consumed the pointed stones!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        HP_BAR(opponent, captureDamage: &results[i].boosted);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        HP_BAR(opponent, captureDamage: &results[i].normal);
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_PLAYER], 0);
        EXPECT_MUL_EQ(results[i].normal, Q_4_12(2.0), results[i].boosted);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone does not consume rocks on the opponent's side")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_UNBROKEN_STONE);
        MESSAGE("Pointed stones float in the air on the opposing side!");
        NOT MESSAGE("Golem consumed the pointed stones!");
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone leaves other hazards and their queue intact when consuming rocks")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_TOXIC_SPIKES));
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STICKY_WEB));
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_PLAYER], 2);
        EXPECT_EQ(gBattleStruct->hazardsQueue[B_SIDE_PLAYER][0], HAZARDS_TOXIC_SPIKES);
        EXPECT_EQ(gBattleStruct->hazardsQueue[B_SIDE_PLAYER][1], HAZARDS_STICKY_WEB);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone consumes rocks even if the attack is protected against")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        MESSAGE("Golem consumed the pointed stones!");
        MESSAGE("The opposing Wobbuffet protected itself!");
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone does not consume rocks for status or non-Rock moves")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_ROCK_POLISH; }
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    PARAMETRIZE { move = MOVE_TACKLE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone does not consume rocks when sleep stops the attack")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); Status1(3); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_ROCK_THROW); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone boosts every hit of a multi-hit Rock attack", s16 first, s16 second)
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, rocks ? MOVE_STEALTH_ROCK : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROCK_BLAST, WITH_RNG(RNG_HITS, 2)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].first);
        HP_BAR(opponent, captureDamage: &results[i].second);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].first, Q_4_12(2.0), results[1].first);
        EXPECT_MUL_EQ(results[0].second, Q_4_12(2.0), results[1].second);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone waits until Meteor Beam's attacking turn to consume rocks")
{
    bool32 finish;
    PARAMETRIZE { finish = FALSE; }
    PARAMETRIZE { finish = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_METEOR_BEAM); }
        if (finish) TURN { SKIP_TURN(player); }
    } THEN {
        EXPECT_EQ(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK), !finish);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone recognizes the effective Rock type of Revelation Dance", s16 damage)
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, rocks ? MOVE_STEALTH_ROCK : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_REVELATION_DANCE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Unbroken Stone consumes shared rocks for only the first allied Rock attack", s16 first, s16 second)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); Attack(100); Speed(100); }
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); Attack(100); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(3000); MaxHP(3000); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_STEALTH_ROCK); }
        TURN {
            MOVE(playerLeft, MOVE_ROCK_THROW, target: opponentLeft);
            MOVE(playerRight, MOVE_ROCK_THROW, target: opponentRight);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_UNBROKEN_STONE);
        HP_BAR(opponentLeft, captureDamage: &results[i].first);
        NOT ABILITY_POPUP(playerRight, ABILITY_UNBROKEN_STONE);
        HP_BAR(opponentRight, captureDamage: &results[i].second);
    } THEN {
        EXPECT_MUL_EQ(results[i].second, Q_4_12(2.0), results[i].first);
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone spends consumed rocks even when its attack misses")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_ROCK_THROW, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        MESSAGE("Golem consumed the pointed stones!");
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}

DOUBLE_BATTLE_TEST("Unbroken Stone doubles damage to both targets of a spread Rock attack", s16 left, s16 right)
{
    bool32 rocks;
    PARAMETRIZE { rocks = FALSE; }
    PARAMETRIZE { rocks = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponentLeft, rocks ? MOVE_STEALTH_ROCK : MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_ROCK_SLIDE, WITH_RNG(RNG_ACCURACY, TRUE)); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].left);
        HP_BAR(opponentRight, captureDamage: &results[i].right);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].left, Q_4_12(2.0), results[1].left);
        EXPECT_MUL_EQ(results[0].right, Q_4_12(2.0), results[1].right);
    }
}

SINGLE_BATTLE_TEST("Unbroken Stone takes entry hazard damage before it can consume rocks")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GOLEM) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        HP_BAR(player, damage: 50);
    } THEN {
        EXPECT_EQ(player->hp, 750);
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
    }
}
