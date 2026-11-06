#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"

extern AiScoreFunc sDynamicAiFunc;
static struct SimulatedDamage sDamage[MAX_MON_MOVES];
static s32 sScore[MAX_MON_MOVES];
static bool32 sObserveKO;
static u32 sHitsToKO;
static bool32 sObserved[MAX_MON_MOVES];
static s32 ObserveGloriousMane(u32 attacker, u32 defender, u32 move, s32 score)
{
    if (attacker == B_BATTLER_1 && defender == B_BATTLER_0 && gBattleResults.battleTurnCounter == 0 && !gAiLogicData->aiPredictionInProgress)
    {
        u32 slot = gAiThinkingStruct->movesetIndex;
        sDamage[slot] = gAiLogicData->simulatedDmg[attacker][defender][slot];
        sScore[slot] = score;
        sObserved[slot] = TRUE;
        if (sObserveKO)
            sHitsToKO = GetNoOfHitsToKOBattler(attacker, defender, slot, AI_ATTACKING, CONSIDER_ENDURE);
    }
    return score;
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI predicts physical special and fixed damage at full and reduced HP")
{
    enum Move move = MOVE_SCRATCH;
    bool32 full = FALSE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_DRAGON_RAGE, MOVE_SEISMIC_TOSS};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; full = FALSE; }
        PARAMETRIZE { move = moves[j]; full = TRUE; }
    }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN { sObserved[0] = FALSE; sDynamicAiFunc = ObserveGloriousMane; TURN { EXPECT_MOVE(opponent, move); } }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved[0]);
        u32 actual = (full ? 3000 : 2999) - player->hp;
        EXPECT_GE(actual, sDamage[0].minimum);
        EXPECT_LE(actual, sDamage[0].maximum);
        if (full) EXPECT_EQ(sDamage[0].median, 1); else EXPECT_GT(sDamage[0].median, 1);
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_GLORIOUS_MANE);
    }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI predicts unprotected later hits of regular multihit attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_DOUBLE_KICK; }
    PARAMETRIZE { move = MOVE_BONEMERANG; }
    PARAMETRIZE { move = MOVE_FURY_ATTACK; }
    PARAMETRIZE { move = MOVE_TRIPLE_AXEL; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN { sObserved[0] = FALSE; sDynamicAiFunc = ObserveGloriousMane; TURN { EXPECT_MOVE(opponent, move); } }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved[0]);
        EXPECT_GT(sDamage[0].median, 2);
        EXPECT_GE(3000 - player->hp, sDamage[0].minimum);
        EXPECT_LE(3000 - player->hp, sDamage[0].maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI predicts Parental Bond and the other Apex extra-hit damage")
{
    enum Species species;
    enum Ability ability;
    enum Move move;
    enum Item item;
    PARAMETRIZE { species = SPECIES_KANGASKHAN; ability = ABILITY_PARENTAL_BOND; move = MOVE_SCRATCH; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_RATICATE; ability = ABILITY_STRONG_JAW; move = MOVE_BITE; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_REVERBERATIONS; move = MOVE_HYPER_VOICE; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_CROBAT; ability = ABILITY_INNER_FOCUS; move = MOVE_DOUBLE_KICK; item = ITEM_APEX_METEORITE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(species) { Ability(ability); Item(item); Moves(move); }
    } WHEN { sObserved[0] = FALSE; sDynamicAiFunc = ObserveGloriousMane; TURN { EXPECT_MOVE(opponent, move); } }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved[0]);
        EXPECT_GT(sDamage[0].median, 1);
        EXPECT_GE(3000 - player->hp, sDamage[0].minimum);
        EXPECT_LE(3000 - player->hp, sDamage[0].maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI predicts Beat Up and Haunting Shrieks contributions")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_BEAT_UP; }
    PARAMETRIZE { move = MOVE_HAUNTING_SHRIEKS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(move == MOVE_HAUNTING_SHRIEKS ? 0 : 100); }
    } WHEN { sObserved[0] = FALSE; sDynamicAiFunc = ObserveGloriousMane; TURN { EXPECT_MOVE(opponent, move); } }
    THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved[0]);
        EXPECT_GT(sDamage[0].median, 1);
        EXPECT_GE(3000 - player->hp, sDamage[0].minimum);
        EXPECT_LE(3000 - player->hp, sDamage[0].maximum);
    }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI understands the HP condition on Dazzling and Clear Body")
{
    bool32 full;
    PARAMETRIZE { full = FALSE; }
    PARAMETRIZE { full = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN {
        EXPECT_EQ(Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_0, MOVE_QUICK_ATTACK, gAiLogicData), full);
        EXPECT_EQ(CanLowerStat(B_BATTLER_1, B_BATTLER_0, gAiLogicData, STAT_ATK), !full);
        EXPECT_EQ(CanLowerStat(B_BATTLER_1, B_BATTLER_0, gAiLogicData, STAT_DEF), !full);
    }
}

AI_DOUBLE_BATTLE_TEST("Glorious Mane AI recognizes conditional partner priority protection even against Mold Breaker")
{
    bool32 full;
    enum Ability ability;
    PARAMETRIZE { full = FALSE; ability = ABILITY_NONE; }
    PARAMETRIZE { full = TRUE; ability = ABILITY_NONE; }
    PARAMETRIZE { full = FALSE; ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { full = TRUE; ability = ABILITY_MOLD_BREAKER; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(full ? 3000 : 2999); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} }
    THEN { EXPECT_EQ(Ai_IsPriorityBlocked(B_BATTLER_1, B_BATTLER_2, MOVE_QUICK_ATTACK, gAiLogicData), full); }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI avoids priority attacks blocked at full HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_QUICK_ATTACK, MOVE_WATER_GUN); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_WATER_GUN, MOVE_QUICK_ATTACK); EXPECT_MOVE(opponent, MOVE_WATER_GUN); } }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI prefers multihit damage over a capped single attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(20); MaxHP(20); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Attack(300); Moves(MOVE_DOUBLE_KICK, MOVE_FLAMETHROWER); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_DOUBLE_KICK, MOVE_FLAMETHROWER); EXPECT_MOVE(opponent, MOVE_DOUBLE_KICK); SEND_OUT(player, 1); } }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI accounts for Tangling Hair and contact immunity", s32 score)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LONG_REACH; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN { sObserved[0] = FALSE; sDynamicAiFunc = ObserveGloriousMane; TURN { EXPECT_MOVE(opponent, MOVE_SCRATCH); } }
    THEN { sDynamicAiFunc = NULL; EXPECT(sObserved[0]); results[i].score = sScore[0]; }
    FINALLY { EXPECT_EQ(results[0].score + DECENT_EFFECT, results[1].score); }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI heals small HP loss to restore its defenses before a lethal attack")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(4000); HP(3000); MaxHP(3000); Speed(1); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(999); MaxHP(1000); Speed(100); Moves(MOVE_RECOVER, MOVE_EARTH_POWER); }
    } WHEN { TURN { MOVE(player, MOVE_FLAMETHROWER, secondaryEffect: FALSE); SCORE_GT(opponent, MOVE_RECOVER, MOVE_EARTH_POWER); EXPECT_MOVE(opponent, MOVE_RECOVER); } }
    THEN { EXPECT_EQ(opponent->hp, 999); }
}

AI_SINGLE_BATTLE_TEST("Glorious Mane AI knows that subsequent turns no longer have the damage cap")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(4000); Moves(MOVE_BRICK_BREAK); }
    } WHEN {
        sObserved[0] = FALSE;
        sObserveKO = TRUE;
        sDynamicAiFunc = ObserveGloriousMane;
        TURN { EXPECT_MOVE(opponent, MOVE_BRICK_BREAK); }
    } THEN {
        sDynamicAiFunc = NULL;
        sObserveKO = FALSE;
        EXPECT(sObserved[0]);
        EXPECT_EQ(sHitsToKO, 2);
        EXPECT_EQ(player->hp, 999);
    }
}

AI_DOUBLE_BATTLE_TEST("Glorious Mane AI does not add Parental Bond hits to spread attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_DUGTRIO_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_KANGASKHAN) { Ability(ABILITY_PARENTAL_BOND); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        sObserved[0] = FALSE;
        sDynamicAiFunc = ObserveGloriousMane;
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    } THEN {
        sDynamicAiFunc = NULL;
        EXPECT(sObserved[0]);
        EXPECT_EQ(sDamage[0].median, 1);
        EXPECT_EQ(playerLeft->hp, 2999);
    }
}
