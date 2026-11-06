#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "pokemon.h"

SINGLE_BATTLE_TEST("Relentless Chase belongs to Hisuian Growlithe and Arcanine with an Apex Meteorite")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_GROWLITHE_HISUI; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE_HISUI; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_GROWLITHE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARCANINE_HISUI; item = ITEM_NONE; }
    GIVEN { PLAYER(species) { Ability(ABILITY_FLASH_FIRE); Item(item); } OPPONENT(SPECIES_WOBBUFFET); }
    WHEN { TURN {} }
    THEN { EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_RELENTLESS_CHASE, item == ITEM_APEX_METEORITE && (species == SPECIES_GROWLITHE_HISUI || species == SPECIES_ARCANINE_HISUI)); }
}

SINGLE_BATTLE_TEST("Relentless Chase boosts physical and special hits against the outgoing Pokemon by fifty percent", s16 damage)
{
    enum Move move;
    bool32 switching;
    PARAMETRIZE { move = MOVE_SCRATCH; switching = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; switching = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; switching = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; switching = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Attack(100); SpAttack(100); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { if (switching) SWITCH(opponent, 1); MOVE(player, move, criticalHit: FALSE); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, move, player); HP_BAR(opponent, captureDamage: &results[i].damage); if (switching) MESSAGE("2 sent out Wynaut!"); }
    THEN { if (switching) { EXPECT_EQ(opponent->hp, 3000); EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_HP), 3000 - results[i].damage); } }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage); EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage); }
}

SINGLE_BATTLE_TEST("Relentless Chase intercepts fast U-turn Volt Switch and Parting Shot pivots")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN { TURN { MOVE(opponent, move); MOVE(player, MOVE_SCRATCH); SEND_OUT(opponent, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, move, opponent); ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player); HP_BAR(opponent); MESSAGE("2 sent out Wynaut!"); }
    THEN { EXPECT_LT(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_HP), 3000); EXPECT_EQ(opponent->hp, 3000); }
}

SINGLE_BATTLE_TEST("Relentless Chase cannot act again after moving before a pivot")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_U_TURN); SEND_OUT(opponent, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player); HP_BAR(opponent); ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponent); NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player); }
}

SINGLE_BATTLE_TEST("Relentless Chase prevents the move's own recoil only on intercepted hits", s16 recoil)
{
    enum Move move;
    bool32 switching;
    PARAMETRIZE { move = MOVE_DOUBLE_EDGE; switching = FALSE; }
    PARAMETRIZE { move = MOVE_DOUBLE_EDGE; switching = TRUE; }
    PARAMETRIZE { move = MOVE_FLARE_BLITZ; switching = FALSE; }
    PARAMETRIZE { move = MOVE_FLARE_BLITZ; switching = TRUE; }
    PARAMETRIZE { move = MOVE_HEAD_SMASH; switching = FALSE; }
    PARAMETRIZE { move = MOVE_HEAD_SMASH; switching = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); }
    } WHEN { TURN { if (switching) SWITCH(opponent, 1); MOVE(player, move, hit: TRUE); } }
    SCENE { HP_BAR(opponent); if (!switching) HP_BAR(player, captureDamage: &results[i].recoil); else NOT HP_BAR(player); }
    THEN { if (switching) EXPECT_EQ(player->hp, 800); else EXPECT_GT(results[i].recoil, 0); }
}

SINGLE_BATTLE_TEST("Relentless Chase suppresses fixed HP move recoil after an intercepted hit")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_STEEL_BEAM; }
    PARAMETRIZE { move = MOVE_CHLOROBLAST; }
    PARAMETRIZE { move = MOVE_STRUGGLE; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, move); } }
    SCENE { HP_BAR(opponent); NOT HP_BAR(player); }
    THEN { EXPECT_EQ(player->hp, 800); }
}

SINGLE_BATTLE_TEST("Relentless Chase still takes Rough Skin Iron Barbs and Rocky Helmet retaliation")
{
    enum Ability ability;
    enum Item item;
    PARAMETRIZE { ability = ABILITY_ROUGH_SKIN; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_IRON_BARBS; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_ROCKY_HELMET; }
    PARAMETRIZE { ability = ABILITY_ROUGH_SKIN; item = ITEM_ROCKY_HELMET; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_FLARE_BLITZ); } }
    THEN { EXPECT_EQ(player->hp, 800 - (ability == ABILITY_NONE ? 0 : 100) - (item == ITEM_NONE ? 0 : 133)); }
}

SINGLE_BATTLE_TEST("Relentless Chase still takes Aftermath damage on an intercepted knockout")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_AFTERMATH); HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_FLARE_BLITZ); SEND_OUT(opponent, 1); } }
    THEN { EXPECT_EQ(player->hp, 600); EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_HP), 0); }
}

SINGLE_BATTLE_TEST("Relentless Chase does not prevent crash damage when the intercepted move cannot hit")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_GASTLY);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_HIGH_JUMP_KICK); } }
    SCENE { NOT HP_BAR(opponent); HP_BAR(player); }
    THEN { EXPECT_EQ(player->hp, 400); }
}

SINGLE_BATTLE_TEST("Relentless Chase intercepts opponent-targeted status moves")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_WILL_O_WISP); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player); STATUS_ICON(opponent, burn: TRUE); MESSAGE("2 sent out Wynaut!"); }
    THEN { EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_STATUS), STATUS1_BURN); EXPECT_EQ(opponent->status1, STATUS1_NONE); }
}

SINGLE_BATTLE_TEST("Relentless Chase leaves self-targeted moves after the switch")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_SWORDS_DANCE); } }
    SCENE { MESSAGE("2 sent out Wynaut!"); ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player); }
}

SINGLE_BATTLE_TEST("Relentless Chase preserves Pursuit's own doubled power alongside its fifty percent bonus", s16 damage)
{
    bool32 switching;
    PARAMETRIZE { switching = FALSE; }
    PARAMETRIZE { switching = TRUE; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Defense(100); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { if (switching) SWITCH(opponent, 1); MOVE(player, MOVE_PURSUIT, criticalHit: FALSE); } }
    SCENE { HP_BAR(opponent, captureDamage: &results[i].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(3.0), results[1].damage); }
}

SINGLE_BATTLE_TEST("Relentless Chase applies its bonus to every strike against a switching target")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(5000); MaxHP(5000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_ARM_THRUST, WITH_RNG(RNG_HITS, 5)); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_ARM_THRUST, player); for (u32 hit = 0; hit < 5; hit++) HP_BAR(opponent); MESSAGE("2 sent out Wynaut!"); }
}

DOUBLE_BATTLE_TEST("Relentless Chase boosts only the switching target of a spread move", s16 leaving, s16 staying)
{
    PARAMETRIZE {}
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); SpDefense(100); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponentLeft, 2); MOVE(playerLeft, MOVE_SURF); } }
    SCENE { HP_BAR(opponentLeft, captureDamage: &results[i].leaving); HP_BAR(playerRight); HP_BAR(opponentRight, captureDamage: &results[i].staying); }
    THEN { EXPECT_MUL_EQ(results[i].staying, Q_4_12(1.5), results[i].leaving); }
}

SINGLE_BATTLE_TEST("Relentless Chase retains ordinary contact status effects during interceptions")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_STATIC); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_STATIC, TRUE)); } }
    SCENE { HP_BAR(opponent); ABILITY_POPUP(opponent, ABILITY_STATIC); STATUS_ICON(player, paralysis: TRUE); MESSAGE("2 sent out Wynaut!"); }
    THEN { EXPECT_EQ(player->status1, STATUS1_PARALYSIS); }
}

SINGLE_BATTLE_TEST("Relentless Chase cannot intercept if sleep prevents its action")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); Status1(STATUS1_SLEEP); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_SCRATCH); } }
    SCENE { NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player); MESSAGE("2 sent out Wynaut!"); }
}

SINGLE_BATTLE_TEST("Relentless Chase preserves U-turn's own pivot after intercepting a switch")
{
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN { TURN { SWITCH(opponent, 1); MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player); HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->species, SPECIES_WOBBUFFET); EXPECT_EQ(opponent->species, SPECIES_WYNAUT); }
}

DOUBLE_BATTLE_TEST("Relentless Chase bypasses Follow Me and Rage Powder while intercepting a pivot")
{
    enum Move attention;
    PARAMETRIZE { attention = MOVE_FOLLOW_ME; }
    PARAMETRIZE { attention = MOVE_RAGE_POWDER; }
    GIVEN {
        PLAYER(SPECIES_ARCANINE_HISUI) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { HP(3000); MaxHP(3000); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(opponentRight, attention); MOVE(opponentLeft, MOVE_U_TURN, target: playerLeft); MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft); SEND_OUT(opponentLeft, 2); } }
    SCENE { ANIMATION(ANIM_TYPE_MOVE, attention, opponentRight); ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponentLeft); ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft); HP_BAR(opponentLeft); }
    THEN { EXPECT_EQ(opponentRight->hp, 3000); }
}
