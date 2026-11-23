#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_apex_johto.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Naughty or Nice can choose each damage fraction and never heals itself")
{
    u32 fraction, roll;
    PARAMETRIZE { fraction = 16; roll = 0; }
    PARAMETRIZE { fraction = 8; roll = 1; }
    PARAMETRIZE { fraction = 4; roll = 2; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DELIBIRD) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); }
    } WHEN { TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_NAUGHTY_OR_NICE, roll)); SEND_OUT(player, 1); } }
    THEN { EXPECT_EQ(opponent->hp, 1600 - 1600 / fraction); EXPECT_EQ(player->hp, 100); }
}

DOUBLE_BATTLE_TEST("Naughty or Nice heals allies and damages each opponent using valid independent fractions")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(1600); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1600); MaxHP(1600); }
    } WHEN { TURN {} }
    THEN {
        EXPECT(playerRight->hp == 200 || playerRight->hp == 300 || playerRight->hp == 500);
        EXPECT(opponentLeft->hp == 1500 || opponentLeft->hp == 1400 || opponentLeft->hp == 1200);
        EXPECT(opponentRight->hp == 1500 || opponentRight->hp == 1400 || opponentRight->hp == 1200);
    }
}

DOUBLE_BATTLE_TEST("Naughty or Nice respects indirect-damage immunity and Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); }
        PLAYER(SPECIES_DELIBIRD) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MAGIC_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_FORTIFICATION); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HEAL_BLOCK, target: playerRight); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE { NONE_OF { HP_BAR(playerRight); HP_BAR(opponentLeft); HP_BAR(opponentRight); } }
    THEN { EXPECT_EQ(playerRight->hp, 100); }
}

SINGLE_BATTLE_TEST("Graceful Glide supplies two turns of Tailwind Flying critical hits")
{
    GIVEN {
        PLAYER(SPECIES_MANTINE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUST); }
        TURN { MOVE(player, MOVE_GUST); }
        TURN { MOVE(player, MOVE_GUST); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRACEFUL_GLIDE);
        HP_BAR(opponent); MESSAGE("A critical hit!");
        HP_BAR(opponent); MESSAGE("A critical hit!");
        MESSAGE("Your side's tailwind petered out!");
        HP_BAR(opponent); NOT MESSAGE("A critical hit!");
    } THEN { EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 0); }
}

SINGLE_BATTLE_TEST("Graceful Glide does not shorten or refresh an existing Tailwind")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MANTYKE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); }
        TURN { SWITCH(player, 1); }
    } THEN { EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 2); }
}

SINGLE_BATTLE_TEST("Graceful Glide does not grant critical hits to other types and respects critical immunity")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_WATER_GUN; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_GUST; ability = ABILITY_BATTLE_ARMOR; }
    GIVEN {
        PLAYER(SPECIES_MANTYKE) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { NOT MESSAGE("A critical hit!"); }
}

SINGLE_BATTLE_TEST("Bladed Wings boosts ordinary slicing moves and nonslicing Flying moves", s16 damage)
{
    bool32 apex;
    enum Move move;
    PARAMETRIZE { apex = FALSE; move = MOVE_SLASH; }
    PARAMETRIZE { apex = TRUE; move = MOVE_SLASH; }
    PARAMETRIZE { apex = FALSE; move = MOVE_GUST; }
    PARAMETRIZE { apex = TRUE; move = MOVE_GUST; }
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Bladed Wings critical hits bypass chart resistance but not immunity", s16 damage)
{
    bool32 crit;
    PARAMETRIZE { crit = FALSE; }
    PARAMETRIZE { crit = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PIKACHU) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_GUST, criticalHit: crit); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[crit].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(3), results[1].damage); }
}

SINGLE_BATTLE_TEST("Bladed Wings leaves type-chart immunities and ability immunities intact")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    GIVEN {
        PLAYER(SPECIES_SKARMORY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GASTLY) { Ability(ABILITY_LEVITATE); }
    } WHEN { TURN { MOVE(player, move, criticalHit: TRUE); } }
    SCENE { NOT HP_BAR(opponent); }
}

SINGLE_BATTLE_TEST("Emblazened burns escalate from one eighth even after their source leaves")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN {
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE { HP_BAR(opponent, damage: 100); HP_BAR(opponent, damage: 200); HP_BAR(opponent, damage: 400); }
    THEN { EXPECT(opponent->status1 & STATUS1_EMBLAZENED_BURN); EXPECT(opponent->status1 & STATUS1_BURN); }
}

SINGLE_BATTLE_TEST("Emblazened leaves ordinary burns unchanged")
{
    GIVEN {
        WITH_CONFIG(B_BURN_DAMAGE, GEN_9);
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Status1(STATUS1_BURN); }
    } WHEN { TURN {} TURN {} }
    THEN { EXPECT_EQ(opponent->hp, 700); EXPECT(!(opponent->status1 & STATUS1_EMBLAZENED_BURN)); }
}

SINGLE_BATTLE_TEST("Emblazened's burn counter resets on switching while its status persists")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } THEN { EXPECT_EQ(opponent->hp, 600); EXPECT(opponent->status1 & STATUS1_EMBLAZENED_BURN); }
}

SINGLE_BATTLE_TEST("Emblazened's burn respects Magic Guard and Heatproof")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; }
    PARAMETRIZE { ability = ABILITY_HEATPROOF; }
    GIVEN {
        PLAYER(SPECIES_HOUNDOOM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(player, MOVE_WILL_O_WISP); } TURN {} }
    THEN { EXPECT_EQ(opponent->hp, ability == ABILITY_MAGIC_GUARD ? 800 : 650); }
}

SINGLE_BATTLE_TEST("Emblazened burn cures remove the variant as well")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_RAWST_BERRY); }
    } WHEN { TURN { MOVE(player, MOVE_WILL_O_WISP); } }
    THEN { EXPECT_EQ(opponent->status1, STATUS1_NONE); }
}

SINGLE_BATTLE_TEST("Emblazened has Flash Fire immunity and an immediate Fire damage boost", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_HOUNDOOM) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_UNNERVE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_EMBER); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[apex].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); }
}

SINGLE_BATTLE_TEST("Emblazened absorbs Fire before taking any Fire hit")
{
    GIVEN { PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN { MOVE(opponent, MOVE_EMBER); } }
    SCENE { ABILITY_POPUP(player, ABILITY_EMBLAZENED); NOT HP_BAR(player); }
}

SINGLE_BATTLE_TEST("Unstoppable Roll maximizes Speed and boosts physical and special moves", s16 damage)
{
    bool32 apex;
    enum Move move;
    PARAMETRIZE { apex = FALSE; move = MOVE_TACKLE; }
    PARAMETRIZE { apex = TRUE; move = MOVE_TACKLE; }
    PARAMETRIZE { apex = FALSE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { apex = TRUE; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_DONPHAN) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); Ability(ABILITY_STURDY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, move); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPEED], apex ? MAX_STAT_STAGE : DEFAULT_STAT_STAGE); }
    FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Unstoppable Roll choice locks into status moves too and resets on switching")
{
    GIVEN {
        PLAYER(SPECIES_PHANPY) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE, MOVE_TACKLE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, allowed: FALSE); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN { EXPECT_EQ(gBattleStruct->choicedMove[B_BATTLER_0], MOVE_TACKLE); }
}

SINGLE_BATTLE_TEST("Forest Weirding lasts only the opening turn and provides grassy healing")
{
    GIVEN {
        PLAYER(SPECIES_STANTLER) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(800); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN { TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent); ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player); }
    THEN { EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM)); EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_NONE); EXPECT_EQ(player->hp, 150); }
}

SINGLE_BATTLE_TEST("Copyright Fraud copies an opponent's six raw stats and preserves HP percentage")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); HP(50); MaxHP(200); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(400); Attack(123); Defense(234); SpAttack(345); SpDefense(456); Speed(567); Ability(ABILITY_HUGE_POWER); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(player->maxHP, 400); EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->attack, 123); EXPECT_EQ(player->defense, 234);
        EXPECT_EQ(player->spAttack, 345); EXPECT_EQ(player->spDefense, 456); EXPECT_EQ(player->speed, 567);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_COPYRIGHT_FRAUD);
    }
}

DOUBLE_BATTLE_TEST("Copyright Fraud prefers the ally's raw stats over the opponent or ability-modified stats")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(123); Defense(234); SpAttack(345); SpDefense(456); Speed(567); Ability(ABILITY_HUGE_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(321); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(playerLeft->attack, 123); EXPECT_EQ(playerLeft->speed, 567); EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

DOUBLE_BATTLE_TEST("Copyright Fraud preserves damage percentages and original party stats across switches")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); Attack(20); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Attack(300); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 160); EXPECT_EQ(playerLeft->maxHP, 400);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 80);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP), 200);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK), 20);
    }
}

DOUBLE_BATTLE_TEST("Copyright Fraud retains an exact HP ratio instead of gaining HP through repeated rounding")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); HP(7); MaxHP(13); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(31); MaxHP(31); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(16); MaxHP(400); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_ENDEAVOR, target: playerLeft); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { SWITCH(playerLeft, 0); }
    } THEN { EXPECT_EQ(playerLeft->hp, 16); EXPECT_EQ(playerLeft->maxHP, 31); }
}

SINGLE_BATTLE_TEST("Lon-Lon Bred has two successful self-heals per battle in singles")
{
    GIVEN {
        PLAYER(SPECIES_MILTANK) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SEISMIC_TOSS); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } THEN { EXPECT_EQ(player->hp, 200); EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->lonLonBredUses, 2); }
}

DOUBLE_BATTLE_TEST("Lon-Lon Bred heals its ally by half max HP rather than healing itself")
{
    GIVEN {
        PLAYER(SPECIES_MILTANK) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(playerLeft->hp, 100); EXPECT_EQ(playerRight->hp, 300); EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->lonLonBredUses, 1); }
}

SINGLE_BATTLE_TEST("Lon-Lon Bred saves its charges when entering at full health")
{
    GIVEN {
        PLAYER(SPECIES_MILTANK) { Item(ITEM_APEX_METEORITE); HP(400); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN { EXPECT_EQ(player->hp, 400); EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->lonLonBredUses, 1); }
}

SINGLE_BATTLE_TEST("Smoochum shares Snow Siren with Jynx")
{
    GIVEN { PLAYER(SPECIES_SMOOCHUM) { Item(ITEM_APEX_METEORITE); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_SNOW_SIREN); EXPECT_EQ(GetSpeciesApexAbility(SPECIES_JYNX), ABILITY_SNOW_SIREN); }
}

AI_SINGLE_BATTLE_TEST("Emblazened AI predicts the next escalating burn tick")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { MOVE(player, MOVE_WILL_O_WISP); } }
    THEN { EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_1), 200); }
}

SINGLE_BATTLE_TEST("Emblazened doubles the rounded initial burn damage and continues growing against Heatproof")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(807); MaxHP(807); Ability(ABILITY_HEATPROOF); }
    } WHEN { TURN { MOVE(player, MOVE_WILL_O_WISP); } TURN {} TURN {} TURN {} }
    THEN { EXPECT_EQ(opponent->hp, 57); EXPECT_EQ(GetEmblazenedBurnDamage(B_BATTLER_1), 1600); }
}

SINGLE_BATTLE_TEST("Emblazened also marks burns inflicted by damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN { TURN { MOVE(player, MOVE_EMBER); } }
    SCENE { HP_BAR(opponent); HP_BAR(opponent, damage: 100); }
    THEN { EXPECT(opponent->status1 & STATUS1_EMBLAZENED_BURN); }
}

SINGLE_BATTLE_TEST("Psycho Shift preserves Emblazened's burn variant and starts a fresh escalation")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Status1(STATUS1_BURN | STATUS1_EMBLAZENED_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
    } WHEN { TURN {} TURN { MOVE(player, MOVE_PSYCHO_SHIFT); } }
    THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(opponent->status1 & STATUS1_EMBLAZENED_BURN);
        EXPECT_EQ(opponent->hp, 700);
    }
}

SINGLE_BATTLE_TEST("Copyright Fraud's copied HP faints normally without changing persistent max HP")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(200); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(400); MaxHP(400); Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); SEND_OUT(player, 1); } }
    THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP), 200);
    }
}

DOUBLE_BATTLE_TEST("Forest Weirding affects only the remainder of a mid-turn entry")
{
    GIVEN {
        WITH_CONFIG(B_RECALC_TURN_AFTER_ACTIONS, GEN_8);
        PLAYER(SPECIES_WOBBUFFET) { Speed(70); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        PLAYER(SPECIES_WYRDEER) { Item(ITEM_APEX_METEORITE); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN {}
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_FOREST_WEIRDING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } THEN { EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM)); EXPECT_EQ((u32)gFieldTimers.terrain, B_TERRAIN_NONE); }
}

DOUBLE_BATTLE_TEST("Lon-Lon Bred does not spend a charge on a Heal-Blocked ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); }
        PLAYER(SPECIES_MILTANK) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HEAL_BLOCK, target: playerRight); }
        TURN { SWITCH(playerLeft, 2); }
    } THEN { EXPECT_EQ(playerRight->hp, 100); EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->lonLonBredUses, 0); }
}

AI_DOUBLE_BATTLE_TEST("Johto entry forecasts restore real HP and never consume healing charges or copied stats")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(800); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SMEARGLE) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); Attack(20); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DELIBIRD) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MILTANK) { Item(ITEM_APEX_METEORITE); Speed(100); Moves(MOVE_TACKLE); }
    } WHEN { TURN {} }
    THEN {
        GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_MID_BATTLE_FORCED);
        EXPECT_EQ(playerLeft->hp, 400); EXPECT_EQ(playerRight->hp, 400); EXPECT_EQ(opponentRight->hp, 100);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_ATK), 20);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_MAX_HP), 200);
    }
}

AI_SINGLE_BATTLE_TEST("Bladed Wings AI applies the resistance floor only to critical-hit forecasts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_SKARMORY) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PIKACHU) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1,
            .move = MOVE_GUST, .moveType = TYPE_FLYING, .aiCalc = TRUE };
        ctx.abilities[B_BATTLER_0] = ABILITY_BLADED_WINGS;
        EXPECT_EQ(CalcTypeEffectivenessMultiplier(&ctx), UQ_4_12(0.5));
        ctx.isCrit = TRUE;
        EXPECT_EQ(CalcTypeEffectivenessMultiplier(&ctx), UQ_4_12(1));
    }
}
