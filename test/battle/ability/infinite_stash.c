#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Infinite Stash is the Apex ability of the Alolan Rattata line including Totem Raticate")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_RATTATA_ALOLA; }
    PARAMETRIZE { species = SPECIES_RATICATE_ALOLA; }
    PARAMETRIZE { species = SPECIES_RATICATE_ALOLA_TOTEM; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
    } THEN {
        EXPECT_EQ(GetSpeciesApexAbility(species), ABILITY_INFINITE_STASH);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_INFINITE_STASH);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->ateBerry);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_0)->usedHeldItem, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash requires Apex activation and does not replace standard Rattata's Gnashing Teeth")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_RATTATA_ALOLA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_RATICATE_ALOLA; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_RATTATA; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_RATICATE; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
    } THEN {
        EXPECT_NE(GetBattlerAbility(B_BATTLER_0), ABILITY_INFINITE_STASH);
    }
}

static const enum Item sHealingBerries[] = { ITEM_SITRUS_BERRY, ITEM_FIGY_BERRY, ITEM_WIKI_BERRY, ITEM_MAGO_BERRY, ITEM_AGUAV_BERRY, ITEM_IAPAPA_BERRY };

SINGLE_BATTLE_TEST("Infinite Stash randomly eats healing berries below full HP even above normal berry thresholds")
{
    enum Item berry = ITEM_NONE;
    for (u32 j = 0; j < ARRAY_COUNT(sHealingBerries); j++)
        PARAMETRIZE { berry = sHealingBerries[j]; }
    PASSES_RANDOMLY(1, 6, RNG_INFINITE_STASH);
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); Nature(NATURE_HARDY); HP(1199); MaxHP(1200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ITEM_POPUP(player, berry);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player);
        NONE_OF {
            ITEM_POPUP(player, ITEM_ORAN_BERRY);
            ITEM_POPUP(player, ITEM_APEX_METEORITE);
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash checks every nature and never chooses its disliked flavor or Oran Berry")
{
    u32 nature = NATURE_HARDY;
    for (u32 candidate = 0; candidate < NUM_NATURES; candidate++)
        PARAMETRIZE { nature = candidate; }
    PASSES_RANDOMLY(1, 1, RNG_INFINITE_STASH);
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); Nature(nature); HP(600); MaxHP(1200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        static const enum Item flavorBerries[] = { ITEM_FIGY_BERRY, ITEM_IAPAPA_BERRY, ITEM_MAGO_BERRY, ITEM_WIKI_BERRY, ITEM_AGUAV_BERRY };
        NONE_OF {
            ITEM_POPUP(player, ITEM_ORAN_BERRY);
            ITEM_POPUP(player, ITEM_APEX_METEORITE);
            if (nature / 5 != nature % 5)
                ITEM_POPUP(player, flavorBerries[nature % 5]);
            MESSAGE("Raticate became confused!");
        }
    } THEN {
        EXPECT_GT(player->hp, 600);
        EXPECT_EQ((u32)player->volatiles.confusionTimer, 0);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash randomly eats stat boosting berries at full HP")
{
    enum Item berry;
    PARAMETRIZE { berry = ITEM_LIECHI_BERRY; }
    PARAMETRIZE { berry = ITEM_GANLON_BERRY; }
    PARAMETRIZE { berry = ITEM_SALAC_BERRY; }
    PARAMETRIZE { berry = ITEM_PETAYA_BERRY; }
    PARAMETRIZE { berry = ITEM_APICOT_BERRY; }
    PARAMETRIZE { berry = ITEM_STARF_BERRY; }
    PASSES_RANDOMLY(1, 6, RNG_INFINITE_STASH);
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        ITEM_POPUP(player, berry);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        s32 sum = 0;
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            sum += player->statStages[stat] - DEFAULT_STAT_STAGE;
        EXPECT_EQ(sum, berry == ITEM_STARF_BERRY ? 2 : 1);
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash names the chosen healing berry in the stash and consumption dialogue")
{
    PASSES_RANDOMLY(1, 6, RNG_INFINITE_STASH);
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); Nature(NATURE_HARDY); HP(600); MaxHP(1200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("Raticate pulled out a Sitrus Berry from its stash!");
        MESSAGE("Raticate ate its Sitrus Berry!");
        ITEM_POPUP(player, ITEM_SITRUS_BERRY);
        HP_BAR(player);
        NONE_OF {
            MESSAGE("Raticate ate its Apex Meteorite!");
            ITEM_POPUP(player, ITEM_APEX_METEORITE);
        }
    }
}

SINGLE_BATTLE_TEST("Infinite Stash names the chosen stat berry without consuming the Meteorite")
{
    PASSES_RANDOMLY(1, 6, RNG_INFINITE_STASH);
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } SCENE {
        MESSAGE("Raticate pulled out a Liechi Berry from its stash!");
        MESSAGE("Raticate ate its Liechi Berry!");
        ITEM_POPUP(player, ITEM_LIECHI_BERRY);
        MESSAGE("Raticate's Attack rose!");
        NOT ITEM_POPUP(player, ITEM_APEX_METEORITE);
    } THEN {
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash chooses a fresh berry every turn and continues after switching")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
        TURN {}
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        NOT ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
    } THEN {
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash skips capped stat berries and has nothing helpful once all core stats are capped")
{
    bool32 allCapped;
    PARAMETRIZE { allCapped = FALSE; }
    PARAMETRIZE { allCapped = TRUE; }
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(5000); MaxHP(5000); SpDefense(1000); Speed(1); }
    } WHEN {
        for (u32 turn = 0; turn < 6; turn++)
            TURN { MOVE(player, MOVE_ANCIENT_POWER, secondaryEffect: TRUE); }
        if (!allCapped)
            TURN { MOVE(opponent, MOVE_CONFIDE); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        if (!allCapped) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFIDE, opponent);
            ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
            ONE_OF {
                ITEM_POPUP(player, ITEM_PETAYA_BERRY);
                ITEM_POPUP(player, ITEM_STARF_BERRY);
            }
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NOT ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
    } THEN {
        for (enum Stat stat = STAT_ATK; stat < NUM_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], MAX_STAT_STAGE);
    }
}

static const struct { enum Move move; enum Item berry; } sAlolanWeaknesses[] = {
    { MOVE_ARM_THRUST, ITEM_CHOPLE_BERRY },
    { MOVE_BUG_BITE, ITEM_TANGA_BERRY },
    { MOVE_DISARMING_VOICE, ITEM_ROSELI_BERRY },
};

SINGLE_BATTLE_TEST("Infinite Stash eats the appropriate resistance berry before a super effective attack", s16 damage)
{
    enum Item item = ITEM_NONE;
    u32 attack = 0;
    for (u32 j = 0; j < ARRAY_COUNT(sAlolanWeaknesses); j++) {
        PARAMETRIZE { attack = j; item = ITEM_NONE; }
        PARAMETRIZE { attack = j; item = ITEM_APEX_METEORITE; }
    }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE_ALOLA) { Ability(ABILITY_THICK_FAT); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            if (attack == 0) MOVE(player, sAlolanWeaknesses[attack].move, WITH_RNG(RNG_HITS, 2));
            else MOVE(player, sAlolanWeaknesses[attack].move);
        }
    } SCENE {
        if (item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(opponent, ABILITY_INFINITE_STASH);
            ITEM_POPUP(opponent, sAlolanWeaknesses[attack].berry);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        }
        ANIMATION(ANIM_TYPE_MOVE, sAlolanWeaknesses[attack].move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(sAlolanWeaknesses); j++)
            EXPECT_MUL_EQ(results[j * 2].damage, Q_4_12(0.5), results[j * 2 + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash resistance dialogue names Chople Berry and preserves the Apex Meteorite")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_KARATE_CHOP); }
    } SCENE {
        MESSAGE("The opposing Raticate pulled out a Chople Berry from its stash!");
        MESSAGE("The opposing Raticate ate its Chople Berry!");
        ITEM_POPUP(opponent, ITEM_CHOPLE_BERRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KARATE_CHOP, player);
        HP_BAR(opponent);
        MESSAGE("The opposing Raticate's Chople Berry lessened the damage it took!");
        NONE_OF {
            MESSAGE("The opposing Raticate ate its Apex Meteorite!");
            MESSAGE("The opposing Raticate's Apex Meteorite lessened the damage it took!");
            ITEM_POPUP(opponent, ITEM_APEX_METEORITE);
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_APEX_METEORITE);
        EXPECT(GetBattlerPartyState(B_BATTLER_1)->ateBerry);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_1)->usedHeldItem, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash resistance berries apply to every strike of a multihit attack", s16 damage[3])
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE_ALOLA) { Ability(ABILITY_THICK_FAT); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ARM_THRUST, WITH_RNG(RNG_HITS, 3)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage[0]);
        HP_BAR(opponent, captureDamage: &results[i].damage[1]);
        HP_BAR(opponent, captureDamage: &results[i].damage[2]);
    } FINALLY {
        for (u32 j = 0; j < 3; j++)
            EXPECT_MUL_EQ(results[0].damage[j], Q_4_12(0.5), results[1].damage[j]);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash does not pull resistance berries for neutral resisted immune missed or protected attacks")
{
    enum Move move;
    bool32 protect = FALSE;
    bool32 miss = FALSE;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_CONFUSION; }
    PARAMETRIZE { move = MOVE_KARATE_CHOP; protect = TRUE; }
    PARAMETRIZE { move = MOVE_HIGH_JUMP_KICK; miss = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            MOVE(player, move, hit: !miss, secondaryEffect: FALSE);
            if (protect) MOVE(opponent, MOVE_PROTECT);
            else MOVE(opponent, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ITEM_POPUP(opponent, ITEM_CHOPLE_BERRY);
            ITEM_POPUP(opponent, ITEM_TANGA_BERRY);
            ITEM_POPUP(opponent, ITEM_ROSELI_BERRY);
        }
    }
}

SINGLE_BATTLE_TEST("Infinite Stash berry eating is blocked by Unnerve but not Mold Breaker", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { ability = ABILITY_UNNERVE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_KARATE_CHOP); }
    } SCENE {
        if (ability == ABILITY_UNNERVE) {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_INFINITE_STASH);
                ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
            }
        } else {
            ITEM_POPUP(opponent, ITEM_CHOPLE_BERRY);
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash heals through held item suppression")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { move = MOVE_EMBARGO; }
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(600); MaxHP(1200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player);
    } THEN {
        EXPECT_GT(player->hp, 600);
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash does not eat a useless healing berry under Heal Block")
{
    GIVEN {
        WITH_CONFIG(B_HEAL_BLOCKING, GEN_5);
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(600); MaxHP(1200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
    } THEN {
        EXPECT_EQ(player->hp, 600);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash unlocks Belch after eating a virtual berry")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BELCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BELCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELCH, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash has a matching resistance berry for every ordinary attack type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        for (enum Type type = TYPE_NORMAL; type <= TYPE_FAIRY; type++) {
            if (type == TYPE_MYSTERY)
                continue;
            enum Item berry = GetInfiniteStashResistBerry(type);
            EXPECT_NE(berry, ITEM_NONE);
            EXPECT_EQ(GetItemHoldEffect(berry), HOLD_EFFECT_RESIST_BERRY);
            EXPECT_EQ(GetItemHoldEffectParam(berry), type);
        }
        EXPECT_EQ(GetInfiniteStashResistBerry(TYPE_STELLAR), ITEM_NONE);
        EXPECT_EQ(GetInfiniteStashResistBerry(TYPE_MYSTERY), ITEM_NONE);
    }
}

DOUBLE_BATTLE_TEST("Infinite Stash selects separate resistance berries for both targets of a spread attack")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_RATTATA_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DAZZLING_GLEAM); }
    } SCENE {
        ITEM_POPUP(opponentLeft, ITEM_ROSELI_BERRY);
        ITEM_POPUP(opponentRight, ITEM_ROSELI_BERRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        MESSAGE("The opposing Rattata's Roseli Berry lessened the damage it took!");
        MESSAGE("The opposing Raticate's Roseli Berry lessened the damage it took!");
    } THEN {
        EXPECT_EQ(opponentLeft->item, ITEM_APEX_METEORITE);
        EXPECT_EQ(opponentRight->item, ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Infinite Stash does not eat an end turn berry while fainted")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLA) { Item(ITEM_APEX_METEORITE); HP(1); MaxHP(1000); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INFINITE_STASH);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        }
    }
}
