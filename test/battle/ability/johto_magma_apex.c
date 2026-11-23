#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Bear Necessities blocks sleep and Rest while letting Normal attacks hit Ghosts")
{
    GIVEN {
        PLAYER(SPECIES_TEDDIURSA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_MISDREAVUS);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPORE); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_REST); }
    } SCENE { HP_BAR(opponent); NOT HP_BAR(player); }
    THEN { EXPECT_EQ(player->status1, STATUS1_NONE); EXPECT_EQ(player->hp, 100); }
}

SINGLE_BATTLE_TEST("Bear Necessities includes Guts and removes burn's Attack penalty", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_URSARING) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_QUICK_FEET); Status1(STATUS1_BURN); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
    } WHEN { TURN { MOVE(player, MOVE_TACKLE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[apex].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(3.0), results[1].damage); }
}

SINGLE_BATTLE_TEST("Bear Necessities retains Scrappy's Intimidate immunity")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN { TURN {} } THEN { EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Lunar Revival restores a quarter HP and cancels only the pending move")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(400); Speed(1); MovesWithPP({MOVE_TACKLE, 5}); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(500); }
    } WHEN { TURN { MOVE(opponent, MOVE_DRAGON_RAGE); MOVE(player, MOVE_TACKLE); } }
    SCENE {
        HP_BAR(player, damage: 40);
        ABILITY_POPUP(player, ABILITY_LUNAR_REVIVAL);
        HP_BAR(player, hp: 100);
        MESSAGE("Ursaluna is catching its breath!");
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->pp[0], 5);
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->lunarRevivalUsed);
        EXPECT_EQ(gBattleResults.playerFaintCounter, 0);
    }
}

SINGLE_BATTLE_TEST("Lunar Revival is once per battle rather than once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SEISMIC_TOSS); SEND_OUT(player, 1); }
    } THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0); }
}

SINGLE_BATTLE_TEST("Lunar Revival after the user's action does not cancel its next turn")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); Speed(500); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_LUNAR_REVIVAL);
        HP_BAR(opponent);
        NOT MESSAGE("Ursaluna is catching its breath!");
    }
}

SINGLE_BATTLE_TEST("Lunar Revival can be overcome by later hits of the same multihit move")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(40); Defense(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Attack(500); }
    } WHEN { TURN { MOVE(opponent, MOVE_DOUBLE_KICK); SEND_OUT(player, 1); } }
    SCENE { ABILITY_POPUP(player, ABILITY_LUNAR_REVIVAL); MESSAGE("Ursaluna fainted!"); }
}

SINGLE_BATTLE_TEST("Lunar Revival does not award Moxie for a prevented faint")
{
    GIVEN {
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOXIE); }
    } WHEN { TURN { MOVE(opponent, MOVE_DRAGON_RAGE); } }
    THEN { EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

SINGLE_BATTLE_TEST("Giga Drill Break critically strikes with contact and makes Normal super effective on Ghost")
{
    GIVEN {
        PLAYER(SPECIES_DUNSPARCE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_MISDREAVUS) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_TACKLE); } }
    SCENE { HP_BAR(opponent); MESSAGE("It's super effective!"); MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Giga Drill Break leaves non-contact crits alone and respects Battle Armor")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_SWIFT; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_BATTLE_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_DUDUNSPARCE_THREE_SEGMENT) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN { TURN { MOVE(player, move); } } SCENE { NOT MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Fangs from Above boosts biting damage like Strong Jaw", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GLIGAR) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_CRUNCH); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[apex].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); }
}

SINGLE_BATTLE_TEST("Fangs from Above sets Tailwind after using a blocked bite without refreshing it")
{
    GIVEN {
        PLAYER(SPECIES_GLISCOR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_BITE); }
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE { ABILITY_POPUP(player, ABILITY_FANGS_FROM_ABOVE); NOT ABILITY_POPUP(player, ABILITY_FANGS_FROM_ABOVE); }
    THEN { EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 2); }
}

SINGLE_BATTLE_TEST("Scaredy-Maw Intimidates on entry and stays when no replacement exists")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Scaredy-Maw switches to a chosen replacement when an Intimidator enters")
{
    GIVEN {
        PLAYER(SPECIES_SNUBBULL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
    } WHEN { TURN { SWITCH(opponent, 1); SEND_OUT(player, 1); } }
    SCENE { ABILITY_POPUP(player, ABILITY_SCAREDY_MAW); SEND_IN_MESSAGE("Wobbuffet"); }
    THEN { EXPECT_EQ(player->species, SPECIES_WOBBUFFET); }
}

SINGLE_BATTLE_TEST("Scaredy-Maw does not switch for ordinary Attack drops")
{
    GIVEN {
        PLAYER(SPECIES_SNUBBULL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_GROWL); } }
    THEN { EXPECT_EQ(player->species, SPECIES_SNUBBULL); }
}

SINGLE_BATTLE_TEST("Prickly Defense sets both Spikes and Toxic Spikes on contact and caps each independently")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { for (u32 i = 0; i < 4; i++) TURN { MOVE(opponent, MOVE_TACKLE); } }
    THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 2);
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 2);
    }
}

SINGLE_BATTLE_TEST("Prickly Defense ignores non-contact hits and Substitute damage")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Item(ITEM_APEX_METEORITE); Speed(200); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Attack(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWIFT); }
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_TACKLE); }
    } THEN { EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 0); }
}

SINGLE_BATTLE_TEST("Barbed Strikes stacks contact barbs to three layers")
{
    GIVEN {
        PLAYER(SPECIES_OVERQWIL) { Item(ITEM_APEX_METEORITE); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); Defense(500); }
    } WHEN { for (u32 i = 0; i < 4; i++) TURN { MOVE(player, MOVE_TACKLE); } }
    SCENE {
        for (u32 i = 1; i <= 4; i++) {
            HP_BAR(opponent);
            MESSAGE("The opposing Wobbuffet was hurt by its barbs!");
            HP_BAR(opponent, damage: min(i, 3) * 100);
        }
    } THEN { EXPECT_EQ((u32)opponent->volatiles.barbStacks, 3); }
}

SINGLE_BATTLE_TEST("Barbed Strikes persists after its source leaves and clears when its target leaves")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH_HISUI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN { EXPECT_EQ((u32)opponent->volatiles.barbStacks, 0); }
}

SINGLE_BATTLE_TEST("Barbed Strikes retains Poison Point")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_POISON_POINT, TRUE)); } }
    THEN { EXPECT(opponent->status1 & STATUS1_POISON); }
}

SINGLE_BATTLE_TEST("Fortification blocks incoming status moves but allows its own setup")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_TOXIC); }
        TURN { MOVE(opponent, MOVE_GROWL); }
        TURN { MOVE(opponent, MOVE_PERISH_SONG); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ((u32)player->volatiles.perishSong, FALSE);
    }
}

SINGLE_BATTLE_TEST("Fortification blocks poison residual damage and move recoil")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_POISON); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_DOUBLE_EDGE); } TURN {} }
    SCENE { NOT HP_BAR(player); } THEN { EXPECT_EQ(player->hp, 400); }
}

SINGLE_BATTLE_TEST("Horn Launch lowers contact priority and forces a random replacement")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Item(ITEM_APEX_METEORITE); Speed(500); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(2); }
    } WHEN { TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_TACKLE); } }
    SCENE { HP_BAR(player); HP_BAR(opponent); MESSAGE("The opposing Wynaut was dragged out!"); }
    THEN { EXPECT_EQ(GetBattleMovePriorityBeforeFieldAbilities(B_BATTLER_0, ABILITY_HORN_LAUNCH, MOVE_QUICK_ATTACK, B_BATTLER_1), -1); }
}

SINGLE_BATTLE_TEST("Horn Launch does not phase on non-contact moves and prevents critical hits")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { MOVE(opponent, MOVE_FROST_BREATH); MOVE(player, MOVE_SWIFT); } }
    SCENE { NOT MESSAGE("A critical hit!"); }
    THEN { EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET); }
}

SINGLE_BATTLE_TEST("Icy Assassin guarantees critical hits and multiplies their damage by 1.3", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SNEASEL) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_FROST_BREATH); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[apex].damage); MESSAGE("A critical hit!"); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage); }
}

SINGLE_BATTLE_TEST("Icy Assassin's ordinary non-contact attacks also always crit")
{
    GIVEN {
        PLAYER(SPECIES_WEAVILE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_SWIFT); } }
    SCENE { MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Highlander adds one crit stage per fainted ally and ignores empty slots")
{
    GIVEN {
        PLAYER(SPECIES_SNEASEL_HISUI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_TACKLE };
        ctx.abilities[B_BATTLER_0] = ABILITY_HIGHLANDER;
        EXPECT_EQ(CountHighlanderFaintedAllies(B_BATTLER_0), 2);
        EXPECT_EQ(CalcCritChanceStage(&ctx), 2);
    }
}

SINGLE_BATTLE_TEST("Highlander deals 1.5x damage only as the last healthy party member", s16 damage)
{
    bool32 last;
    PARAMETRIZE { last = FALSE; }
    PARAMETRIZE { last = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SNEASLER) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(last ? 0 : 100); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_SWIFT); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[last].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); }
}

DOUBLE_BATTLE_TEST("18,000F Magma burns other occupants and later entrants on either side")
{
    GIVEN {
        PLAYER(SPECIES_SLUGMA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHARMANDER);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(playerRight, 2); SWITCH(opponentRight, 2); } }
    THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT(playerRight->status1 & STATUS1_BURN);
        EXPECT(opponentLeft->status1 & STATUS1_BURN);
        EXPECT(opponentRight->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("18,000F Magma absorbs Ground moves and respects ordinary burn immunities")
{
    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_CHARMANDER);
    } WHEN { TURN { MOVE(opponent, MOVE_EARTHQUAKE); } }
    SCENE { ABILITY_POPUP(player, ABILITY_MAGMA_18000F); HP_BAR(player, hp: 200); }
    THEN { EXPECT_EQ(opponent->status1, STATUS1_NONE); EXPECT_EQ(player->hp, 200); }
}

AI_SINGLE_BATTLE_TEST("Lunar Revival AI counts a second life and does not predict a single-hit KO")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_URSALUNA_BLOODMOON) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Level(100); }
    } WHEN { TURN {} } THEN {
        EXPECT_EQ(GetNoOfHitsToKOBattlerDmg(100, B_BATTLER_0), 2);
        EXPECT(CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_SEISMIC_TOSS));
        GetBattlerPartyState(B_BATTLER_0)->lunarRevivalUsed = TRUE;
        EXPECT_EQ(GetNoOfHitsToKOBattlerDmg(100, B_BATTLER_0), 1);
        EXPECT(!CanEndureHit(B_BATTLER_1, B_BATTLER_0, MOVE_SEISMIC_TOSS));
    }
}

AI_SINGLE_BATTLE_TEST("Fortification AI rejects status attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC, MOVE_TACKLE); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_TACKLE, MOVE_TOXIC); } }
}

AI_SINGLE_BATTLE_TEST("Fangs from Above AI values a bite's Tailwind")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_GLISCOR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE, MOVE_TACKLE); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_BITE, MOVE_TACKLE); } }
}

SINGLE_BATTLE_TEST("Fortification also blocks entry hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); }
    } SCENE { NOT HP_BAR(player); }
}

SINGLE_BATTLE_TEST("Barbed Strikes does not apply through Protect or Substitute or with non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_OVERQWIL) { Item(ITEM_APEX_METEORITE); Attack(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_SWIFT); }
    } THEN { EXPECT_EQ((u32)opponent->volatiles.barbStacks, 0); }
}

SINGLE_BATTLE_TEST("Fortification blocks Struggle recoil while preserving attack damage")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_STRUGGLE); } }
    SCENE { HP_BAR(opponent); NOT HP_BAR(player); }
    THEN { EXPECT_EQ(player->hp, 400); }
}

SINGLE_BATTLE_TEST("Fortification blocks confusion and disobedience self-damage")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} } THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_0,
            .move = MOVE_NONE, .chosenMove = MOVE_NONE, .moveType = TYPE_MYSTERY,
            .isSelfInflicted = TRUE, .fixedBasePower = 40 };
        ctx.abilities[B_BATTLER_0] = ABILITY_FORTIFICATION;
        EXPECT_EQ(CalculateMoveDamage(&ctx), 0);
    }
}

SINGLE_BATTLE_TEST("18,000F Magma cannot activate after its holder faints to entry hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MAGCARGO) { Item(ITEM_APEX_METEORITE); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); SEND_OUT(player, 0); }
    } THEN { EXPECT_EQ(opponent->status1, STATUS1_NONE); }
}

AI_SINGLE_BATTLE_TEST("18,000F Magma switch forecasts never burn real Pokemon or persist between candidates")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGCARGO) { Item(ITEM_APEX_METEORITE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN { TURN {} } THEN {
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_FORCED);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), STATUS1_NONE);
    }
}
