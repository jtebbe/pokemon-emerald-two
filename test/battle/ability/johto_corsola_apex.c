#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Ancient Wool combines Fur Coat and Thick Fat", s16 damage)
{
    bool32 apex;
    enum Move move;
    PARAMETRIZE { apex = FALSE; move = MOVE_TACKLE; }
    PARAMETRIZE { apex = TRUE; move = MOVE_TACKLE; }
    PARAMETRIZE { apex = FALSE; move = MOVE_FLAME_WHEEL; }
    PARAMETRIZE { apex = TRUE; move = MOVE_FLAME_WHEEL; }
    PARAMETRIZE { apex = FALSE; move = MOVE_ICE_BEAM; }
    PARAMETRIZE { apex = TRUE; move = MOVE_ICE_BEAM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAMOSWINE) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_OBLIVIOUS); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, Q_4_12(4), results[2].damage);
        EXPECT_MUL_EQ(results[5].damage, Q_4_12(2), results[4].damage);
    }
}

SINGLE_BATTLE_TEST("Ancient Wool keeps its armor against guaranteed critical hits and Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_PILOSWINE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); }
    } WHEN { TURN { MOVE(opponent, MOVE_WICKED_BLOW); } }
    SCENE { HP_BAR(player); NOT MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Algae Addiction absorbs Grass and adds Grass without removing either native type")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_VINE_WHIP); } }
    SCENE { ABILITY_POPUP(player, ABILITY_ALGAE_ADDICTION); HP_BAR(player, hp: 200); }
    THEN {
        EXPECT_EQ(player->types[0], TYPE_WATER);
        EXPECT_EQ(player->types[1], TYPE_ROCK);
        EXPECT_EQ(player->types[2], TYPE_GRASS);
        EXPECT(player->volatiles.algaeActive);
        EXPECT_EQ(CheckDynamicMoveType(GetBattlerMon(B_BATTLER_0), MOVE_TACKLE, B_BATTLER_0, MON_IN_BATTLE), TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Algae Addiction gains its coat at full HP or under Heal Block")
{
    bool32 full;
    PARAMETRIZE { full = FALSE; }
    PARAMETRIZE { full = TRUE; }
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 400 : 100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(opponent, MOVE_ABSORB); }
    } THEN {
        EXPECT_EQ(player->hp, full ? 400 : 100);
        EXPECT_EQ(player->types[2], TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Algae Addiction resets its coat and Normal conversion on switching")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ABSORB); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT(!player->volatiles.algaeActive);
        EXPECT_EQ(CheckDynamicMoveType(GetBattlerMon(B_BATTLER_0), MOVE_TACKLE, B_BATTLER_0, MON_IN_BATTLE), TYPE_NORMAL);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Ecocollapse repeats the only available type and damages its own holder")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MISDREAVUS) { HP(400); MaxHP(400); }
    } WHEN { TURN {} TURN {} }
    THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(opponent->hp, 200);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->ecoLastType, TYPE_GHOST);
    }
}

SINGLE_BATTLE_TEST("Ecocollapse cannot select the same type on successive turns when another exists")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA_GALAR) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MEW) { HP(400); MaxHP(400); }
    } WHEN { TURN {} TURN {} }
    THEN { EXPECT_EQ(player->hp, 300); EXPECT_EQ(opponent->hp, 300); }
}

DOUBLE_BATTLE_TEST("Ecocollapse hits matching allies and respects Magic Guard and Fortification")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_MISDREAVUS) { HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MISDREAVUS) { Ability(ABILITY_MAGIC_GUARD); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MISDREAVUS) { Ability(ABILITY_FORTIFICATION); HP(400); MaxHP(400); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(playerLeft->hp, 300); EXPECT_EQ(playerRight->hp, 300);
        EXPECT_EQ(opponentLeft->hp, 400); EXPECT_EQ(opponentRight->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Ecocollapse resolves all lethal damage before either side's faint")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MISDREAVUS) { HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { SEND_OUT(player, 1); SEND_OUT(opponent, 1); } }
    SCENE { HP_BAR(player, hp: 0); HP_BAR(opponent, hp: 0); MESSAGE("Cursola fainted!"); }
}

DOUBLE_BATTLE_TEST("Cleaning Station heals and cures its ally on entry without losing positive stages")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(800); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_REMORAID) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SWORDS_DANCE); MOVE(opponentLeft, MOVE_GROWL); MOVE(opponentRight, MOVE_LEER); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->hp, 400);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Cleaning Station removes Heal Block while retaining Substitute and Aqua Ring")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        PLAYER(SPECIES_REMORAID) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_AQUA_RING); MOVE(opponentLeft, MOVE_HEAL_BLOCK, target: playerRight); }
        TURN { MOVE(playerRight, MOVE_SUBSTITUTE); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN {
        EXPECT(playerRight->volatiles.substitute);
        EXPECT(playerRight->volatiles.aquaRing);
        EXPECT_EQ((u32)playerRight->volatiles.healBlockTimer, 0);
        EXPECT_EQ(playerRight->hp, 750);
    }
}

SINGLE_BATTLE_TEST("Octocannon adds a stronger shot before or after its normal move without extra PP", s16 normal, s16 counter)
{
    bool32 first;
    PARAMETRIZE { first = FALSE; }
    PARAMETRIZE { first = TRUE; }
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(first ? 500 : 1); MovesWithPP({MOVE_DRAGON_PULSE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
    } WHEN { TURN { MOVE(player, MOVE_DRAGON_PULSE); MOVE(opponent, MOVE_TACKLE); } }
    SCENE {
        if (first) { HP_BAR(opponent, captureDamage: &results[i].normal); }
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_OCTOCANNON);
        HP_BAR(opponent, captureDamage: &results[i].counter);
        if (!first) { HP_BAR(opponent, captureDamage: &results[i].normal); }
    } THEN { EXPECT_EQ(player->pp[0], 9); }
    FINALLY {
        EXPECT_MUL_EQ(results[0].normal, Q_4_12(1.5), results[0].counter);
        EXPECT_MUL_EQ(results[1].normal, Q_4_12(1.5), results[1].counter);
    }
}

SINGLE_BATTLE_TEST("Octocannon retaliates only once against an incoming multihit move")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(500); }
    } WHEN { TURN { MOVE(opponent, MOVE_DOUBLE_KICK); MOVE(player, MOVE_DRAGON_PULSE); } }
    SCENE {
        HP_BAR(player); HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent);
        HP_BAR(opponent); NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Octocannon does not react to Substitute damage, Protect, a miss, or a noneligible queued move")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(500); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); Attack(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_TACKLE, hit: FALSE); }
    } SCENE { NOT ABILITY_POPUP(player, ABILITY_OCTOCANNON); }
}

SINGLE_BATTLE_TEST("Octocannon counters never recursively trigger another Octocannon")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(500); }
        OPPONENT(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_DRAGON_PULSE); MOVE(opponent, MOVE_DRAGON_PULSE); } }
    SCENE {
        HP_BAR(opponent); ABILITY_POPUP(opponent, ABILITY_OCTOCANNON); HP_BAR(player);
        HP_BAR(player); ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent);
        NONE_OF { HP_BAR(player); HP_BAR(opponent); }
    }
}

SINGLE_BATTLE_TEST("Octocannon fires charge moves immediately but preserves the scheduled charge turn")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(500); }
    } WHEN { TURN { MOVE(player, MOVE_SOLAR_BEAM); MOVE(opponent, MOVE_TACKLE); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent); NOT HP_BAR(opponent); }
    THEN { EXPECT(player->volatiles.chargeTurn); EXPECT(player->volatiles.multipleTurns); }
}

SINGLE_BATTLE_TEST("Octocannon extra Hyper Beam does not impose recharge before the scheduled move")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(500); }
    } WHEN { TURN { MOVE(player, MOVE_HYPER_BEAM); MOVE(opponent, MOVE_TACKLE); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent); HP_BAR(opponent); }
    THEN { EXPECT(player->volatiles.rechargeTimer); }
}

SINGLE_BATTLE_TEST("Mega Launcher boosts Hydro Pump, beams and cannons", s16 damage)
{
    enum Move move;
    bool32 boosted;
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; boosted = FALSE; }
    PARAMETRIZE { move = MOVE_HYDRO_PUMP; boosted = TRUE; }
    PARAMETRIZE { move = MOVE_ICE_BEAM; boosted = FALSE; }
    PARAMETRIZE { move = MOVE_ICE_BEAM; boosted = TRUE; }
    PARAMETRIZE { move = MOVE_FLASH_CANNON; boosted = FALSE; }
    PARAMETRIZE { move = MOVE_FLASH_CANNON; boosted = TRUE; }
    PARAMETRIZE { move = MOVE_DYNAMAX_CANNON; boosted = FALSE; }
    PARAMETRIZE { move = MOVE_DYNAMAX_CANNON; boosted = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(boosted ? ABILITY_MEGA_LAUNCHER : ABILITY_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { for (u32 n = 0; n < 8; n += 2) EXPECT_MUL_EQ(results[n].damage, Q_4_12(1.5), results[n + 1].damage); }
}

SINGLE_BATTLE_TEST("Dynamax Cannon is an ordinary attack and Mega Launcher retains distinct pulse flags")
{
    GIVEN { PLAYER(SPECIES_WOBBUFFET); OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN {
        EXPECT_EQ(GetMovePower(MOVE_DYNAMAX_CANNON), 100);
        EXPECT_EQ(GetMoveAccuracy(MOVE_DYNAMAX_CANNON), 100);
        EXPECT_EQ(gMovesInfo[MOVE_DYNAMAX_CANNON].pp, 5);
        EXPECT_EQ(GetMovePP(MOVE_DYNAMAX_CANNON), 8);
        EXPECT_EQ(GetMoveEffect(MOVE_DYNAMAX_CANNON), EFFECT_HIT);
        EXPECT_EQ(GetMoveAdditionalEffectCount(MOVE_DYNAMAX_CANNON), 0);
        EXPECT(!IsPulseMove(MOVE_ICE_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_SIMPLE_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_HYDRO_CANNON));
        EXPECT(IsMegaLauncherMove(MOVE_HYPER_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_AURORA_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_CHARGE_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_MOONGEIST_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_PSYBEAM));
        EXPECT(IsMegaLauncherMove(MOVE_SIGNAL_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_SOLAR_BEAM));
        EXPECT(IsMegaLauncherMove(MOVE_SPIKE_CANNON));
        EXPECT(IsMegaLauncherMove(MOVE_ZAP_CANNON));
        EXPECT(IsMegaLauncherMove(MOVE_FLEUR_CANNON));
    }
}

AI_SINGLE_BATTLE_TEST("Ecocollapse AI forecasts the expected matching-type damage without rolling RNG")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MEW) { Moves(MOVE_CELEBRATE); HP(400); MaxHP(400); }
    } WHEN { TURN {} }
    THEN {
        GetBattlerPartyState(B_BATTLER_0)->ecoTypeChosen = FALSE;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 50);
        GetBattlerPartyState(B_BATTLER_0)->ecoTypeChosen = TRUE;
        GetBattlerPartyState(B_BATTLER_0)->ecoLastType = TYPE_GHOST;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 100);
    }
}

DOUBLE_BATTLE_TEST("Octocannon reacts separately to its ally and both opponents without extra PP")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(500); MovesWithPP({MOVE_DRAGON_PULSE, 1}); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
    } WHEN { TURN {
        MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentLeft);
        MOVE(playerRight, MOVE_TACKLE, target: playerLeft);
        MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
        MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
    } } SCENE {
        HP_BAR(opponentLeft); HP_BAR(playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); HP_BAR(opponentLeft);
        HP_BAR(playerLeft); ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); HP_BAR(opponentLeft);
        HP_BAR(playerLeft); ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); HP_BAR(opponentLeft);
        NONE_OF { HP_BAR(opponentLeft); HP_BAR(opponentRight); }
    } THEN { EXPECT_EQ(playerLeft->pp[0], 0); }
}

SINGLE_BATTLE_TEST("Octocannon cannot fire after being knocked out")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(400); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(500); }
    } WHEN { TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); } }
    SCENE { NONE_OF { ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent); } }
}

DOUBLE_BATTLE_TEST("Octocannon always fires at its queued target before or after its own turn")
{
    bool32 first, ally;
    PARAMETRIZE { first = FALSE; ally = FALSE; }
    PARAMETRIZE { first = TRUE; ally = FALSE; }
    PARAMETRIZE { first = FALSE; ally = TRUE; }
    PARAMETRIZE { first = TRUE; ally = TRUE; }
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(first ? 500 : 1); MovesWithPP({MOVE_DRAGON_PULSE, 10}); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
    } WHEN { TURN {
        MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentRight);
        if (ally) MOVE(playerRight, MOVE_DOUBLE_KICK, target: playerLeft);
        else MOVE(opponentLeft, MOVE_DOUBLE_KICK, target: playerLeft);
    } } SCENE {
        if (first) HP_BAR(opponentRight);
        HP_BAR(playerLeft); HP_BAR(playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); HP_BAR(opponentRight);
        if (!first) HP_BAR(opponentRight);
        NONE_OF { HP_BAR(opponentLeft); HP_BAR(playerRight); HP_BAR(opponentRight); }
    } THEN { EXPECT_EQ(playerLeft->pp[0], 9); }
}

DOUBLE_BATTLE_TEST("Octocannon's allied trigger still requires real damage and a surviving holder")
{
    bool32 lethal;
    PARAMETRIZE { lethal = FALSE; }
    PARAMETRIZE { lethal = TRUE; }
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(lethal ? 40 : 3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(500); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
    } WHEN {
        if (lethal) TURN {
            MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentRight);
            MOVE(playerRight, MOVE_DRAGON_RAGE, target: playerLeft);
            SEND_OUT(playerLeft, 2);
        }
        else {
            TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_TACKLE, target: playerLeft); }
            TURN { MOVE(playerLeft, MOVE_SUBSTITUTE); }
            TURN { MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentRight); MOVE(playerRight, MOVE_TACKLE, target: playerLeft); }
            TURN { MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentRight); MOVE(playerRight, MOVE_TACKLE, target: playerLeft, hit: FALSE); }
        }
    } SCENE { NOT ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); }
}

SINGLE_BATTLE_TEST("Octocannon keeps charge-turn boosts on immediate Meteor Beam shots")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(500); }
    } WHEN { TURN { MOVE(player, MOVE_METEOR_BEAM); MOVE(opponent, MOVE_TACKLE); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent); }
    THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT(player->volatiles.chargeTurn);
    }
}

SINGLE_BATTLE_TEST("Octocannon can counter after Future Sight resolves at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FUTURE_SIGHT); }
        TURN {}
        TURN { MOVE(player, MOVE_WATER_PULSE); }
    } SCENE {
        HP_BAR(opponent); HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent);
    }
}

DOUBLE_BATTLE_TEST("Cleaning Station clears harmful volatiles without stripping beneficial ones")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    THEN {
        playerRight->volatiles.tarShot = TRUE;
        playerRight->volatiles.tormentTimer = 3;
        playerRight->volatiles.telekinesisTimer = 3;
        playerRight->volatiles.syrupBombTimer = 3;
        playerRight->volatiles.confusionTimer = 3;
        playerRight->volatiles.infatuation = 1;
        playerRight->volatiles.wrapped = TRUE;
        playerRight->volatiles.leechSeed = TRUE;
        playerRight->volatiles.tauntTimer = 3;
        playerRight->volatiles.disableTimer = 3;
        playerRight->volatiles.disabledMove = MOVE_TACKLE;
        playerRight->volatiles.perishSong = TRUE;
        playerRight->volatiles.barbStacks = 3;
        playerRight->volatiles.substitute = TRUE;
        playerRight->volatiles.aquaRing = TRUE;
        playerRight->volatiles.root = TRUE;
        playerRight->statStages[STAT_ATK] = 8;
        playerRight->statStages[STAT_SPDEF] = 2;
        playerRight->status1 = STATUS1_PARALYSIS;
        ApplyCleaningStation(B_BATTLER_2);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT(!playerRight->volatiles.tarShot);
        EXPECT_EQ((u32)playerRight->volatiles.tormentTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.telekinesisTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.syrupBombTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.confusionTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.infatuation, 0);
        EXPECT(!playerRight->volatiles.wrapped);
        EXPECT(!playerRight->volatiles.leechSeed);
        EXPECT_EQ((u32)playerRight->volatiles.tauntTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.disableTimer, 0);
        EXPECT_EQ((u32)playerRight->volatiles.disabledMove, MOVE_NONE);
        EXPECT(!playerRight->volatiles.perishSong);
        EXPECT_EQ((u32)playerRight->volatiles.barbStacks, 0);
        EXPECT(playerRight->volatiles.substitute);
        EXPECT(playerRight->volatiles.aquaRing);
        EXPECT(playerRight->volatiles.root);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], 8);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("Cleaning Station switch forecasts restore the real ally and subsequent candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); HP(400); MaxHP(800); }
        OPPONENT(SPECIES_REMORAID) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN { TURN {} }
    THEN {
        opponentRight->status1 = STATUS1_PARALYSIS;
        opponentRight->statStages[STAT_ATK] = 2;
        opponentRight->volatiles.confusionTimer = 3;
        opponentRight->volatiles.healBlockTimer = 3;
        opponentRight->volatiles.aquaRing = TRUE;
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_FORCED);
        EXPECT_EQ(opponentRight->hp, 400);
        EXPECT_EQ(opponentRight->status1, STATUS1_PARALYSIS);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], 2);
        EXPECT_EQ((u32)opponentRight->volatiles.confusionTimer, 3);
        EXPECT_EQ((u32)opponentRight->volatiles.healBlockTimer, 3);
        EXPECT(opponentRight->volatiles.aquaRing);
    }
}

AI_SINGLE_BATTLE_TEST("Algae Addiction's absorption forecasts cannot activate its coat")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_CORSOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_VINE_WHIP, MOVE_TACKLE); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_VINE_WHIP); } }
    THEN {
        EXPECT(!player->volatiles.algaeActive);
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

AI_SINGLE_BATTLE_TEST("Octocannon AI includes Mega Launcher in its eligible move forecasts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        struct AiCalcValues calc = { .move = MOVE_DYNAMAX_CANNON };
        struct SimulatedDamage empowered = AI_CalcDamage(&calc, B_BATTLER_1, B_BATTLER_0);
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_NONE;
        struct SimulatedDamage ordinary = AI_CalcDamage(&calc, B_BATTLER_1, B_BATTLER_0);
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_OCTOCANNON;
        EXPECT_MUL_EQ(ordinary.median, Q_4_12(1.5), empowered.median);
    }
}

SINGLE_BATTLE_TEST("Algae Addiction's converted Normal moves gain Grass STAB without an ate power bonus")
{
    s16 converted, grass;
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ABSORB); }
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_BRANCH_POKE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &converted);
        HP_BAR(opponent, captureDamage: &grass);
    } THEN { EXPECT_EQ(converted, grass); }
}

SINGLE_BATTLE_TEST("Ecocollapse's type history belongs to its party member across switches")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MISDREAVUS) { HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MEW) { HP(400); MaxHP(400); }
    } WHEN {
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->ecoLastType, TYPE_PSYCHIC);
        EXPECT_EQ(opponent->hp, 300);
    }
}

SINGLE_BATTLE_TEST("Ecocollapse uses actual defensive Tera typing rather than original types")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_MEW) { TeraType(TYPE_GHOST); HP(400); MaxHP(400); }
    } WHEN { TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_TERA); } }
    THEN { EXPECT_EQ(player->hp, 300); EXPECT_EQ(opponent->hp, 300); }
}

SINGLE_BATTLE_TEST("Octocannon can use an eligible status move without spending additional PP")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); MovesWithPP({MOVE_HEAL_PULSE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(1000); Speed(500); }
    } WHEN { TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_HEAL_PULSE); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_OCTOCANNON); HP_BAR(opponent, hp: 850); HP_BAR(opponent, hp: 1000); }
    THEN { EXPECT_EQ(player->pp[0], 9); }
}

DOUBLE_BATTLE_TEST("Octocannon can counter a survived allied or opposing Meteoric Wrath impact")
{
    bool32 allyCast;
    PARAMETRIZE { allyCast = FALSE; }
    PARAMETRIZE { allyCast = TRUE; }
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(500); }
        PLAYER(SPECIES_CHANSEY) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(200); }
    } WHEN {
        TURN {
            if (allyCast) MOVE(playerRight, MOVE_METEORIC_WRATH);
            else MOVE(opponentLeft, MOVE_METEORIC_WRATH);
        }
        TURN {}
        TURN { MOVE(playerLeft, MOVE_DRAGON_PULSE, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight);
        HP_BAR(playerLeft); HP_BAR(opponentLeft); HP_BAR(playerRight); HP_BAR(opponentRight);
        ABILITY_POPUP(playerLeft, ABILITY_OCTOCANNON); HP_BAR(opponentRight);
    }
}
