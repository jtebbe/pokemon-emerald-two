#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "battle_main.h"

SINGLE_BATTLE_TEST("Lulling Melodies is the Apex ability of the Igglybuff line")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_IGGLYBUFF; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_JIGGLYPUFF; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WIGGLYTUFF; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_WIGGLYTUFF; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(species) { Ability(ABILITY_CUTE_CHARM); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0) == ABILITY_LULLING_MELODIES, item == ITEM_APEX_METEORITE && species != SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies gives damaging sound moves one priority stage only when no active foe is asleep")
{
    u32 status;
    enum Ability ability;
    PARAMETRIZE { status = STATUS1_NONE; ability = ABILITY_NONE; }
    PARAMETRIZE { status = STATUS1_SLEEP; ability = ABILITY_NONE; }
    PARAMETRIZE { status = STATUS1_NONE; ability = ABILITY_COMATOSE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Status1(status); Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, FALSE));
            MOVE(opponent, status || ability == ABILITY_COMATOSE ? MOVE_SNORE : MOVE_CELEBRATE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (status || ability == ABILITY_COMATOSE) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SNORE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_LULLING_MELODIES, MOVE_HYPER_VOICE), status || ability == ABILITY_COMATOSE ? 0 : 1);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies does not increase priority of status sound moves or nonsound attacks")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_SING; }
    PARAMETRIZE { move = MOVE_PERISH_SONG; }
    PARAMETRIZE { move = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, move, player);
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_LULLING_MELODIES, move), 0);
        if (move != MOVE_SING) EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Lulling Melodies loses priority when either opposing slot is asleep but not when an ally is asleep")
{
    u32 sleepingSlot;
    PARAMETRIZE { sleepingSlot = B_BATTLER_1; }
    PARAMETRIZE { sleepingSlot = B_BATTLER_3; }
    PARAMETRIZE { sleepingSlot = B_BATTLER_2; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(sleepingSlot == B_BATTLER_2 ? STATUS1_SLEEP : STATUS1_NONE); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(sleepingSlot == B_BATTLER_1 ? STATUS1_SLEEP : STATUS1_NONE); Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(sleepingSlot == B_BATTLER_3 ? STATUS1_SLEEP : STATUS1_NONE); Speed(99); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, FALSE)); }
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_LULLING_MELODIES, MOVE_HYPER_VOICE), sleepingSlot == B_BATTLER_2 ? 1 : 0);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies ignores sleeping opponents on the bench")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Status1(STATUS1_SLEEP); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies gives damaging sound moves a thirty percent chance to inflict sleep")
{
    PASSES_RANDOMLY(3, 10, RNG_LULLING_MELODIES);
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        ABILITY_POPUP(player, ABILITY_LULLING_MELODIES);
        MESSAGE("The opposing Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies doubles physical special and sound attack damage against sleeping targets", s16 damage)
{
    enum Move move = MOVE_SCRATCH;
    u32 status = STATUS1_NONE;
    const enum Move moves[] = {MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_HYPER_VOICE};
    for (u32 j = 0; j < ARRAY_COUNT(moves); j++) {
        PARAMETRIZE { move = moves[j]; status = STATUS1_NONE; }
        PARAMETRIZE { move = moves[j]; status = STATUS1_SLEEP; }
    }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Attack(300); SpAttack(300); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); Defense(100); SpDefense(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, move, criticalHit: FALSE, WITH_RNG(RNG_LULLING_MELODIES, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        for (u32 j = 0; j < ARRAY_COUNT(moves); j++) EXPECT_MUL_EQ(results[j * 2].damage, UQ_4_12(2.0), results[j * 2 + 1].damage);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies treats Comatose as asleep for double damage", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_COMATOSE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies does not double fixed damage")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); HP(3000); MaxHP(3000); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
    }
}

DOUBLE_BATTLE_TEST("Lulling Melodies also applies priority and sleep to physically resolved sound moves")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); Attack(300); }
        PLAYER(SPECIES_GIRATINA_ORIGIN) { Item(ITEM_APEX_METEORITE); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(99); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, playerLeft);
        STATUS_ICON(opponentLeft, sleep: TRUE);
        STATUS_ICON(opponentRight, sleep: TRUE);
    } THEN {
        EXPECT_EQ(GetBattleMoveCategory(MOVE_HYPER_VOICE), DAMAGE_CATEGORY_PHYSICAL);
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT(opponentRight->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies respects Insomnia Vital Spirit Comatose Purifying Salt Shield Dust and Covert Cloak")
{
    enum Ability ability = ABILITY_INSOMNIA;
    enum Item item = ITEM_NONE;
    PARAMETRIZE { ability = ABILITY_INSOMNIA; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_VITAL_SPIRIT; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_COMATOSE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_PURIFYING_SALT; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_SOUNDPROOF; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LULLING_MELODIES);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Lulling Melodies respects an opposing partner's Sweet Veil")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_SLURPUFF) { Ability(ABILITY_SWEET_VEIL); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ALLURING_VOICE, target: opponentLeft, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies respects Safeguard and sleep-blocking terrains")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_ELECTRIC_TERRAIN; }
    PARAMETRIZE { protection = MOVE_MISTY_TERRAIN; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, protection); }
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies does not replace existing major status conditions")
{
    u32 status;
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_POISON; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(status); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, status);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies cannot inflict sleep when an attack misses is protected or is immune")
{
    enum Move defense;
    PARAMETRIZE { defense = MOVE_PROTECT; }
    PARAMETRIZE { defense = MOVE_TRICK_OR_TREAT; }
    PARAMETRIZE { defense = MOVE_CELEBRATE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        if (defense == MOVE_TRICK_OR_TREAT) TURN { MOVE(player, MOVE_TRICK_OR_TREAT); }
        TURN {
            if (defense == MOVE_PROTECT) MOVE(opponent, MOVE_PROTECT);
            MOVE(player, defense == MOVE_CELEBRATE ? MOVE_SNARL : MOVE_HYPER_VOICE,
                 hit: defense != MOVE_CELEBRATE, WITH_RNG(RNG_LULLING_MELODIES, TRUE));
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies preserves sound moves bypassing Substitute and can inflict sleep through it")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } SCENE {
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
        EXPECT(opponent->volatiles.substitute);
    }
}

DOUBLE_BATTLE_TEST("Lulling Melodies can sleep awake targets when another opponent is already asleep and respects Sleep Clause")
{
    bool32 clause;
    PARAMETRIZE { clause = FALSE; }
    PARAMETRIZE { clause = TRUE; }
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        if (clause) FLAG_SET(B_FLAG_SLEEP_CLAUSE);
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT_EQ(!!(opponentRight->status1 & STATUS1_SLEEP), !clause);
    }
}

DOUBLE_BATTLE_TEST("Lulling Melodies restores priority after a sleeping opposing slot faints")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Status1(STATUS1_SLEEP); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); Speed(100); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_QUICK_ATTACK, target: opponentLeft); }
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE, WITH_RNG(RNG_LULLING_MELODIES, FALSE)); }
    } THEN {
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_0, ABILITY_LULLING_MELODIES, MOVE_HYPER_VOICE), 1);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies priority is blocked by Psychic Terrain and priority-blocking abilities")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_PSYCHIC_SURGE; }
    PARAMETRIZE { ability = ABILITY_DAZZLING; }
    PARAMETRIZE { ability = ABILITY_QUEENLY_MAJESTY; }
    PARAMETRIZE { ability = ABILITY_ARMOR_TAIL; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } SCENE {
        NONE_OF {
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_LULLING_MELODIES);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 3000);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies priority can be blocked by Quick Guard")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_QUICK_GUARD); MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->hp, 3000);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies does not put a target to sleep after knocking it out")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_ALLURING_VOICE, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LULLING_MELODIES);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies cannot inflict sleep during Uproar")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_UPROAR, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies applies new sleep after dealing damage without doubling that first hit", s16 damage)
{
    bool32 sleep;
    PARAMETRIZE { sleep = FALSE; }
    PARAMETRIZE { sleep = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ALLURING_VOICE, criticalHit: FALSE, WITH_RNG(RNG_LULLING_MELODIES, sleep)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (sleep) STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_EQ(!!(opponent->status1 & STATUS1_SLEEP), sleep);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies stacks with Wake-Up Slap before the target wakes", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Ability(ABILITY_CUTE_CHARM); Item(item); Speed(100); Attack(300); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP); HP(3000); MaxHP(3000); Defense(100); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_WAKE_UP_SLAP, criticalHit: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Lulling Melodies cannot trigger from sound moves prevented by Throat Chop")
{
    GIVEN {
        PLAYER(SPECIES_WIGGLYTUFF) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); Moves(MOVE_ALLURING_VOICE, MOVE_SCRATCH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_THROAT_CHOP); }
        TURN { MOVE(player, MOVE_ALLURING_VOICE, allowed: FALSE); MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_LULLING_MELODIES, TRUE)); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
