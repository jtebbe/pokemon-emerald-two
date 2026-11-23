#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_main.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Umbral Witch raises Special Attack twice and pays once per damaging move")
{
    GIVEN {
        PLAYER(SPECIES_MISDREAVUS) { Item(ITEM_APEX_METEORITE); HP(300); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT); }
        TURN { MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_UMBRAL_WITCH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_UMBRAL_WITCH);
        HP_BAR(player, damage: 30);
        NOT HP_BAR(player);
    } THEN { EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); }
}

SINGLE_BATTLE_TEST("Umbral Witch halves incoming Dark damage", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_MISDREAVUS) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_DARK_PULSE); } }
    SCENE { HP_BAR(player, captureDamage: &results[apex].damage); }
    FINALLY { EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage); }
}

SINGLE_BATTLE_TEST("Umbral Witch pays on Protect and misses but not when unable to act")
{
    GIVEN {
        PLAYER(SPECIES_MISMAGIUS) { Item(ITEM_APEX_METEORITE); HP(300); MaxHP(300); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(500); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SHADOW_BALL); }
        TURN { MOVE(player, MOVE_ZAP_CANNON, hit: FALSE); }
        TURN { MOVE(opponent, MOVE_SPORE); MOVE(player, MOVE_SHADOW_BALL); }
    } THEN { EXPECT_EQ(player->hp, 240); }
}

SINGLE_BATTLE_TEST("Legacy Lexicon gives every Unown letter the all-100 battle form and Levitate")
{
    GIVEN {
        PLAYER(SPECIES_UNOWN_QUESTION) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_EARTHQUAKE); } }
    SCENE { ABILITY_POPUP(player, ABILITY_LEGACY_LEXICON); NOT HP_BAR(player); }
    THEN {
        EXPECT_EQ(player->species, SPECIES_UNOWN_LEXICON);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_LEGACY_LEXICON);
        EXPECT_EQ(gSpeciesInfo[player->species].baseHP, 100);
        EXPECT_EQ(gSpeciesInfo[player->species].baseAttack, 100);
        EXPECT_EQ(gSpeciesInfo[player->species].baseDefense, 100);
        EXPECT_EQ(gSpeciesInfo[player->species].baseSpeed, 100);
        EXPECT_EQ(gSpeciesInfo[player->species].baseSpAttack, 100);
        EXPECT_EQ(gSpeciesInfo[player->species].baseSpDefense, 100);
    }
}

SINGLE_BATTLE_TEST("Legacy Lexicon reverts to its original letter and keeps its HP fraction on switching")
{
    GIVEN {
        PLAYER(SPECIES_UNOWN_Z) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { SWITCH(player, 1); } } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_UNOWN_Z);
        u32 hp = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP);
        u32 maxHP = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP);
        EXPECT_LE(maxHP / 2 - hp, 1);
    }
}

SINGLE_BATTLE_TEST("Legacy Lexicon uses 120-power adaptive Hidden Power and changes its own type")
{
    GIVEN {
        PLAYER(SPECIES_UNOWN) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GYARADOS) { Ability(ABILITY_INTIMIDATE); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_HIDDEN_POWER); } } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's extremely effective!");
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_ELECTRIC);
        EXPECT_EQ(player->types[1], TYPE_ELECTRIC);
    }
}

SINGLE_BATTLE_TEST("Psysync strikes twice at full power for one PP", s16 damage)
{
    bool32 apex;
    PARAMETRIZE { apex = FALSE; }
    PARAMETRIZE { apex = TRUE; }
    GIVEN {
        PLAYER(SPECIES_GIRAFARIG) { Item(apex ? ITEM_APEX_METEORITE : ITEM_NONE); MovesWithPP({MOVE_PSYSHOCK, 5}); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(3000); MaxHP(3000); }
    } WHEN { TURN { MOVE(player, MOVE_PSYSHOCK); } } SCENE {
        s16 hit;
        for (u32 i = 0; i < (apex ? 2 : 1); i++)
        {
            HP_BAR(opponent, captureDamage: &hit);
            results[apex].damage += hit;
        }
        NOT HP_BAR(opponent);
    } THEN { EXPECT_EQ(player->pp[0], 4); }
    FINALLY { EXPECT_EQ(results[1].damage, results[0].damage * 2); }
}

SINGLE_BATTLE_TEST("Psysync preserves native Twin Beam hits and does not repeat status moves")
{
    GIVEN {
        PLAYER(SPECIES_FARIGIRAF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); }
        TURN { MOVE(player, MOVE_TWIN_BEAM); }
    } SCENE { HP_BAR(opponent); HP_BAR(opponent); NOT HP_BAR(opponent); }
    THEN { EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1); }
}

SINGLE_BATTLE_TEST("Psysync's delayed Future Sight still hits twice after switching out")
{
    GIVEN {
        PLAYER(SPECIES_GIRAFARIG) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); }
        TURN { SWITCH(player, 1); }
        TURN {}
    } SCENE { HP_BAR(opponent); HP_BAR(opponent); NOT HP_BAR(opponent); }
}

DOUBLE_BATTLE_TEST("Very Punchable redirects attacks even from Grass Pokemon unless Oblivious or Aroma Veil")
{
    enum Ability ability;
    bool32 redirects;
    PARAMETRIZE { ability = ABILITY_NONE; redirects = TRUE; }
    PARAMETRIZE { ability = ABILITY_OBLIVIOUS; redirects = FALSE; }
    PARAMETRIZE { ability = ABILITY_AROMA_VEIL; redirects = FALSE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponentLeft, MOVE_TACKLE, target: playerRight); } }
    SCENE { if (redirects) HP_BAR(playerLeft); else HP_BAR(playerRight); }
}

SINGLE_BATTLE_TEST("Very Punchable lowers all move priorities by two")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_APEX_METEORITE); Speed(500); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN { TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); } }
    SCENE { HP_BAR(player); HP_BAR(opponent); }
    THEN { EXPECT_EQ(GetBattleMovePriorityBeforeFieldAbilities(B_BATTLER_0, ABILITY_VERY_PUNCHABLE, MOVE_QUICK_ATTACK, B_BATTLER_1), -1); }
}

SINGLE_BATTLE_TEST("Da Don's Tax copies an opponent's boost first without stealing their move or its own turn")
{
    GIVEN {
        PLAYER(SPECIES_MURKROW) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN { TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_GROWL); } }
    SCENE {
        ABILITY_POPUP(player, ABILITY_DA_DONS_TAX);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Da Don's Tax copies healing before the opponent and does not charge the copy PP")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); MovesWithPP({MOVE_RECOVER, 5}); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); Speed(50); }
    } WHEN { TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_RECOVER); } }
    SCENE { HP_BAR(player); ABILITY_POPUP(player, ABILITY_DA_DONS_TAX); ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player); }
    THEN { EXPECT_EQ(gBattleStruct->donTaxCopies, 1); EXPECT_EQ(player->pp[0], 4); EXPECT_EQ(player->hp, 400); EXPECT_EQ(opponent->hp, 300); }
}

DOUBLE_BATTLE_TEST("Da Don's Tax ignores its ally's snatchable move")
{
    GIVEN {
        PLAYER(SPECIES_MURKROW) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(playerRight, MOVE_SWORDS_DANCE); } }
    THEN { EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE); }
}

DOUBLE_BATTLE_TEST("Highly Flammable cancels Fire damage and explodes for three quarters of every other target's HP")
{
    GIVEN {
        PLAYER(SPECIES_PINECO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); MaxHP(200); }
    } WHEN { TURN { MOVE(opponentLeft, MOVE_EMBER, target: playerLeft); SEND_OUT(playerLeft, 2); } }
    SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_HIGHLY_FLAMMABLE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXPLOSION, playerLeft);
        HP_BAR(opponentLeft, damage: 600);
        HP_BAR(playerRight, damage: 300);
        HP_BAR(opponentRight, damage: 150);
        HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 100);
        EXPECT_EQ(opponentRight->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Highly Flammable is not triggered by a Fire move blocked by Protect")
{
    GIVEN {
        PLAYER(SPECIES_PINECO) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_EMBER); } }
    SCENE { NONE_OF { ABILITY_POPUP(player, ABILITY_HIGHLY_FLAMMABLE); HP_BAR(player); } }
}

SINGLE_BATTLE_TEST("Highly Flammable also intercepts a targeted Fire status move")
{
    GIVEN {
        PLAYER(SPECIES_PINECO) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN { TURN { MOVE(opponent, MOVE_WILL_O_WISP); SEND_OUT(player, 1); } }
    SCENE {
        ABILITY_POPUP(player, ABILITY_HIGHLY_FLAMMABLE);
        HP_BAR(opponent, damage: 300);
    } THEN { EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0); }
}

SINGLE_BATTLE_TEST("Return Fire fills all four hazard types before stacking and stops at full layers")
{
    GIVEN {
        PLAYER(SPECIES_FORRETRESS) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (u32 i = 0; i < 8; i++)
            TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_SPIKES));
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_TOXIC_SPIKES));
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
        EXPECT(IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STICKY_WEB));
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].spikesAmount, 3);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount, 2);
        EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 4);
    }
}

SINGLE_BATTLE_TEST("Return Fire ignores damage absorbed by Substitute")
{
    GIVEN {
        PLAYER(SPECIES_FORRETRESS) { Item(ITEM_APEX_METEORITE); Speed(200); HP(400); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Attack(10); }
    } WHEN { TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_TACKLE); } }
    THEN { EXPECT_EQ(gBattleStruct->numHazards[B_SIDE_OPPONENT], 0); }
}

AI_SINGLE_BATTLE_TEST("Psysync AI counts both full-power Psychic strikes without changing real HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GIRAFARIG) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Moves(MOVE_CELEBRATE); }
    } WHEN { TURN {} } THEN {
        struct AiCalcValues calc = { .move = MOVE_PSYSHOCK };
        u32 hp = opponent->hp;
        struct SimulatedDamage doubleHit = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        enum Ability saved = gAiLogicData->abilities[B_BATTLER_0];
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_INNER_FOCUS;
        struct SimulatedDamage singleHit = AI_CalcDamage(&calc, B_BATTLER_0, B_BATTLER_1);
        gAiLogicData->abilities[B_BATTLER_0] = saved;
        EXPECT_EQ(doubleHit.median, singleHit.median * 2);
        EXPECT_EQ(opponent->hp, hp);
    }
}

AI_SINGLE_BATTLE_TEST("Da Don's Tax AI recognizes the cost of giving an opponent a free boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_HONCHKROW) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE, MOVE_SCRATCH); }
    } WHEN { TURN { SCORE_GT(opponent, MOVE_SCRATCH, MOVE_SWORDS_DANCE); } }
}

AI_DOUBLE_BATTLE_TEST("Very Punchable AI follows passive redirection rather than targeting its partner")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WYNAUT) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN { TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft); } }
}

AI_SINGLE_BATTLE_TEST("Highly Flammable AI knows a Fire attack KOs Pineco through cancellation")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_PINECO) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EMBER, MOVE_TACKLE); HP(1000); MaxHP(1000); }
    } WHEN { TURN { EXPECT_MOVE(opponent, MOVE_EMBER); } }
}

DOUBLE_BATTLE_TEST("Da Don's Tax gives each opposing holder a copy and never recursively taxes a copy")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(500); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_MURKROW) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_HONCHKROW) { Item(ITEM_APEX_METEORITE); Speed(50); }
    } WHEN { TURN { MOVE(playerLeft, MOVE_SWORDS_DANCE); } } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Legacy Lexicon restores its original Unown letter when it faints")
{
    GIVEN {
        PLAYER(SPECIES_UNOWN_B) { Item(ITEM_APEX_METEORITE); Level(1); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN { TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); } }
    THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_UNOWN_B);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
    }
}

SINGLE_BATTLE_TEST("Legacy Lexicon alone doubles Hidden Power's base power in AI damage calculations")
{
    GIVEN {
        PLAYER(SPECIES_UNOWN) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_GYARADOS) { HP(3000); MaxHP(3000); }
    } WHEN { TURN {} } THEN {
        struct DamageContext ctx = { .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1,
            .move = MOVE_HIDDEN_POWER, .chosenMove = MOVE_HIDDEN_POWER, .aiCalc = TRUE };
        ctx.abilities[B_BATTLER_0] = ABILITY_LEGACY_LEXICON;
        ctx.abilities[B_BATTLER_1] = ABILITY_INTIMIDATE;
        ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
        s32 empowered = CalculateMoveDamage(&ctx);
        ctx.abilities[B_BATTLER_0] = ABILITY_LEVITATE;
        s32 ordinary = CalculateMoveDamage(&ctx);
        EXPECT_MUL_EQ(ordinary, Q_4_12(2.0), empowered);
    }
}
