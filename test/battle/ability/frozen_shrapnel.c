#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Frozen Shrapnel is exclusive to the Alolan Sandshrew line with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_SANDSHREW_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSLASH_ALOLA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSHREW; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSLASH; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_SANDSHREW_ALOLA; item = ITEM_NONE; }
    GIVEN {
        PLAYER(species) { Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player);
        if ((species == SPECIES_SANDSHREW_ALOLA || species == SPECIES_SANDSLASH_ALOLA) && item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
            MESSAGE("Frigid spikes were scattered around the opposing team!");
        } else {
            NOT ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_FROZEN_SHRAPNEL,
                  (species == SPECIES_SANDSHREW_ALOLA || species == SPECIES_SANDSLASH_ALOLA) && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes deal one sixteenth max HP per layer and inflict frostbite")
{
    u32 layers;
    s16 damage, residual;
    PARAMETRIZE { layers = 1; }
    PARAMETRIZE { layers = 2; }
    PARAMETRIZE { layers = 3; }
    GIVEN {
        WITH_CONFIG(B_BURN_DAMAGE, GEN_9);
        WITH_CONFIG(B_USE_FROSTBITE, FALSE);
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(800); MaxHP(800); }
    } WHEN {
        for (u32 i = 0; i < layers; i++) TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        for (u32 i = 0; i < layers; i++) ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        HP_BAR(opponent, captureDamage: &damage);
        MESSAGE("The opposing Wynaut was hurt by the frigid spikes!");
        MESSAGE("The opposing Wynaut got frostbite!");
        STATUS_ICON(opponent, frostbite: TRUE);
        HP_BAR(opponent, captureDamage: &residual);
    } THEN {
        EXPECT_EQ(damage, 50 * layers);
        EXPECT_EQ(residual, 50);
        EXPECT_EQ(opponent->status1, STATUS1_FROSTBITE);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, layers);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel triggers per contact strike and caps Frigid Spikes at three layers")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ARM_THRUST, WITH_RNG(RNG_HITS, 5)); }
    } SCENE {
        for (u32 i = 0; i < 3; i++) {
            HP_BAR(player);
            ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        }
        for (u32 i = 0; i < 2; i++) {
            HP_BAR(player);
            NOT ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, 3);
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 1);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel requires damaging contact and respects Protective Pads and Long Reach")
{
    enum Move move;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_LONG_REACH; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; item = ITEM_PROTECTIVE_PADS; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_MOLD_BREAKER; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount,
                  move == MOVE_SCRATCH && ability != ABILITY_LONG_REACH && item != ITEM_PROTECTIVE_PADS);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel does not activate through Substitute or Protect")
{
    bool32 substitute;
    PARAMETRIZE { substitute = FALSE; }
    PARAMETRIZE { substitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Attack(1); }
    } WHEN {
        TURN { MOVE(player, substitute ? MOVE_SUBSTITUTE : MOVE_PROTECT); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, 0);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel activates on the contact hit that knocks the holder out")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_FROZEN_SHRAPNEL);
        MESSAGE("Sandshrew fainted!");
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes respect Boots, Flying, Levitate and Air Balloon")
{
    enum Species species;
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_NONE; item = ITEM_HEAVY_DUTY_BOOTS; }
    PARAMETRIZE { species = SPECIES_TAILLOW; ability = ABILITY_NONE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_LEVITATE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_NONE; item = ITEM_AIR_BALLOON; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); Item(item); HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NONE_OF { HP_BAR(opponent); STATUS_ICON(opponent, frostbite: TRUE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 800);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes still inflict frostbite through Magic Guard without dealing hazard damage")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_MAGIC_GUARD); HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT HP_BAR(opponent);
        MESSAGE("The opposing Wynaut got frostbite!");
    } THEN {
        EXPECT_EQ(opponent->hp, 800);
        EXPECT_EQ(opponent->status1, STATUS1_FROSTBITE);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes deal untyped damage even to Ice and Steel types while respecting status immunities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_SNORUNT; ability = ABILITY_NONE; }
    PARAMETRIZE { species = SPECIES_MAGNEMITE; ability = ABILITY_MAGMA_ARMOR; }
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { species = SPECIES_WYNAUT; ability = ABILITY_COMATOSE; }
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        NOT STATUS_ICON(opponent, frostbite: TRUE);
    } THEN {
        EXPECT_EQ(damage, 50);
        EXPECT_EQ(opponent->hp, 750);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes do not replace an existing status")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(800); MaxHP(800); Status1(STATUS1_PARALYSIS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponent->hp, 750);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes do not frostbite a target that faints to their damage")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(1); MaxHP(800); }
        OPPONENT(SPECIES_SNORUNT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); SEND_OUT(opponent, 2); }
    } SCENE {
        MESSAGE("The opposing Wynaut was hurt by the frigid spikes!");
        NOT MESSAGE("The opposing Wynaut got frostbite!");
        MESSAGE("The opposing Wynaut fainted!");
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel damage reduction is twenty five percent for physical and special attacks from frostbitten foes", s16 damage)
{
    enum Move move;
    enum Item item;
    u32 status;
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_NONE; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_NONE; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { move = MOVE_SWIFT; item = ITEM_APEX_METEORITE; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; item = ITEM_APEX_METEORITE; status = STATUS1_NONE; }
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Ability(ABILITY_SNOW_CLOAK); Item(item); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOLD_BREAKER); Attack(300); SpAttack(300); Status1(status); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.75), results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel does not reduce fixed damage from frostbitten foes")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_SANDSLASH_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_FROSTBITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(damage, 40);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes can coexist with and be removed alongside ordinary hazards")
{
    enum Move removal;
    PARAMETRIZE { removal = MOVE_RAPID_SPIN; }
    PARAMETRIZE { removal = MOVE_DEFOG; }
    PARAMETRIZE { removal = MOVE_MORTAL_SPIN; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(player, 1); MOVE(opponent, removal); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, 0);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 0);
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_OPPONENT));
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes are swapped with their layer counts by Court Change")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_HIT); }
        TURN { MOVE(opponent, MOVE_COURT_CHANGE); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].frigidSpikesAmount, 2);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].frigidSpikesAmount, 0);
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_FRIGID_SPIKES));
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_FRIGID_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel hazard status respects Safeguard and Misty Terrain")
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(opponent, terrain ? MOVE_MISTY_TERRAIN : MOVE_SAFEGUARD); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 750);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel grounds Flying targets under Gravity")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TAILLOW) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_GRAVITY); }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_FROSTBITE);
        EXPECT_EQ(opponent->hp, 700);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel and Toxic Spikes apply their statuses in hazard placement order")
{
    bool32 frigidFirst;
    PARAMETRIZE { frigidFirst = FALSE; }
    PARAMETRIZE { frigidFirst = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { HP(800); MaxHP(800); }
    } WHEN {
        if (frigidFirst) {
            TURN { MOVE(opponent, MOVE_SCRATCH); }
            TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        } else {
            TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
            TURN { MOVE(opponent, MOVE_SCRATCH); }
        }
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(opponent->status1, frigidFirst ? STATUS1_FROSTBITE : STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel fits in a field with every existing hazard")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_STICKY_WEB); }
    } THEN {
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 5);
        for (enum Hazards hazard = HAZARDS_SPIKES; hazard < HAZARDS_MAX_COUNT; hazard++)
            EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, hazard));
    }
}

SINGLE_BATTLE_TEST("Frozen Shrapnel Frigid Spikes deal at least one HP of damage")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_SANDSHREW_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORUNT) { HP(15); MaxHP(15); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(damage, 1);
        EXPECT_EQ(opponent->hp, 14);
    }
}
