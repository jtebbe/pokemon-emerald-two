#include "global.h"
#include "battle_util.h"
#include "constants/flags.h"
#include "event_data.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Multivolve requires an Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_EEVEE) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_DUSKULL);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_EEVEE);
        EXPECT_NE(GetBattlerAbility(B_BATTLER_0), ABILITY_MULTIVOLVE);
    }
}

SINGLE_BATTLE_TEST("Multivolve uses Jolteon's badge-scaled moves and preset stats")
{
    bool32 allBadges;
    PARAMETRIZE { allBadges = FALSE; }
    PARAMETRIZE { allBadges = TRUE; }
    GIVEN {
        for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        {
            if (allBadges)
                FlagSet(flag);
            else
                FlagClear(flag);
        }
        PLAYER(SPECIES_DUSKULL) { Moves(MOVE_BATON_PASS); }
        PLAYER(SPECIES_EEVEE) { Item(ITEM_APEX_METEORITE); Level(50); Nature(NATURE_ADAMANT); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_DUSKULL) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_MULTIVOLVE, 1)); SEND_OUT(player, 1); }
    } THEN {
        for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
            FlagClear(flag);
        EXPECT_EQ(player->species, SPECIES_JOLTEON);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_HIGH_VOLTAGE);
        EXPECT_EQ(player->moves[0], allBadges ? MOVE_ALLURING_VOICE : MOVE_THUNDER_SHOCK);
        EXPECT_EQ(player->moves[1], allBadges ? MOVE_THUNDERBOLT : MOVE_GROWL);
        EXPECT_EQ(player->moves[2], allBadges ? MOVE_VOLT_SWITCH : MOVE_TAIL_WHIP);
        EXPECT_EQ(player->moves[3], allBadges ? MOVE_SHADOW_BALL : MOVE_SWIFT);
        EXPECT_EQ(player->speed, CalculateNonHpStat(GetSpeciesBaseStat(SPECIES_JOLTEON, STAT_SPEED), 50, 252, NATURE_JOLLY, STAT_SPEED));
        EXPECT_EQ(player->spAttack, CalculateNonHpStat(GetSpeciesBaseStat(SPECIES_JOLTEON, STAT_SPATK), 50, 252, NATURE_JOLLY, STAT_SPATK));
        EXPECT_EQ(player->spDefense, CalculateNonHpStat(GetSpeciesBaseStat(SPECIES_JOLTEON, STAT_SPDEF), 50, 4, NATURE_JOLLY, STAT_SPDEF));
    }
}

SINGLE_BATTLE_TEST("Multivolve can trigger the incoming form's switch-in Apex ability")
{
    PASSES_RANDOMLY(1, 8, RNG_MULTIVOLVE);
    GIVEN {
        PLAYER(SPECIES_EEVEE) { Item(ITEM_APEX_METEORITE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_DUSKULL);
    } WHEN {
        TURN { MOVE(player, moveSlot: 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MULTIVOLVE);
        ABILITY_POPUP(player, ABILITY_RAINBOW_SKIES);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_SYLVEON);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_RAINBOW_SKIES);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_RAINBOW);
    }
}

SINGLE_BATTLE_TEST("Multivolve restores Eevee's party data and keeps its HP ratio on reentry")
{
    s16 damage;
    bool32 reenter;
    PARAMETRIZE { reenter = FALSE; }
    PARAMETRIZE { reenter = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DUSKULL) { Moves(MOVE_BATON_PASS); }
        PLAYER(SPECIES_EEVEE) {
            Item(ITEM_APEX_METEORITE); Level(50); HP(100); MaxHP(200); Nature(NATURE_ADAMANT);
            Moves(MOVE_TACKLE, MOVE_GROWL);
        }
        OPPONENT(SPECIES_DUSKULL) { Level(30); }
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_MULTIVOLVE, 1)); SEND_OUT(player, 1); }
        TURN { MOVE(player, moveSlot: 1); MOVE(opponent, MOVE_NIGHT_SHADE); }
        TURN { SWITCH(player, 0); }
        if (reenter)
            TURN { MOVE(player, MOVE_BATON_PASS, WITH_RNG(RNG_MULTIVOLVE, 0)); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("Eevee became Jolteon!");
        HP_BAR(player, captureDamage: &damage);
        if (reenter)
            MESSAGE("Eevee became Vaporeon!");
    } THEN {
        struct PartyState *state = &gBattleStruct->partyState[B_TRAINER_PLAYER][1];
        struct Pokemon *eevee = &gParties[B_TRAINER_PLAYER][1];
        u32 lastMax = state->multivolveMaxHp;
        u32 lastHp = (lastMax + 1) / 2 - damage;

        EXPECT_EQ(damage, 30);
        EXPECT_EQ(lastMax, GetSpeciesBaseStat(SPECIES_JOLTEON, STAT_HP) + 75);
        EXPECT_EQ(state->multivolveHp, lastHp);
        EXPECT_EQ(GetMonData(eevee, MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(eevee, MON_DATA_MAX_HP), 200);
        EXPECT_EQ(GetMonData(eevee, MON_DATA_HP), (lastHp * 200 + lastMax / 2) / lastMax);
        EXPECT_EQ(GetNature(eevee), NATURE_ADAMANT);
        EXPECT_EQ(GetMonData(eevee, MON_DATA_MOVE1), MOVE_TACKLE);
        EXPECT_EQ(GetMonData(eevee, MON_DATA_MOVE2), MOVE_GROWL);

        if (reenter)
        {
            u32 formMax = GetSpeciesBaseStat(SPECIES_VAPOREON, STAT_HP) + 75;
            EXPECT_EQ(player->species, SPECIES_VAPOREON);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_AQUATIC_FORM);
            EXPECT_EQ(player->maxHP, formMax);
            EXPECT_EQ(player->hp, (lastHp * formMax + lastMax / 2) / lastMax);
        }
    }
}

SINGLE_BATTLE_TEST("Multivolve applies HP items on the active form's HP scale")
{
    GIVEN {
        PLAYER(SPECIES_EEVEE) { Item(ITEM_APEX_METEORITE); Level(50); HP(100); MaxHP(200); }
        OPPONENT(SPECIES_DUSKULL);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POTION, partyIndex: 0); }
    } SCENE {
        HP_BAR(player, damage: -20);
    } THEN {
        u32 formMax = GetSpeciesBaseStat(player->species, STAT_HP) + 75;
        u32 formHp = (formMax + 1) / 2 + 20;
        EXPECT_NE(player->species, SPECIES_EEVEE);
        EXPECT_EQ(player->maxHP, formMax);
        EXPECT_EQ(player->hp, formHp);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), (formHp * 200 + formMax / 2) / formMax);
    }
}
