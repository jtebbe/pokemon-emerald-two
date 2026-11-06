#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Serpent's Retort is Arbok's Apex ability and excludes Ekans")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_ARBOK; item = ITEM_APEX_METEORITE; }
    PARAMETRIZE { species = SPECIES_ARBOK; item = ITEM_NONE; }
    PARAMETRIZE { species = SPECIES_EKANS; item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(species) { Ability(ABILITY_SHED_SKIN); Item(item); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        if (species == SPECIES_ARBOK && item == ITEM_APEX_METEORITE) {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player);
            MESSAGE("Wobbuffet was badly poisoned!");
        } else {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
                HP_BAR(player);
            }
        }
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1) == ABILITY_SERPENTS_RETORT, species == SPECIES_ARBOK && item == ITEM_APEX_METEORITE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort responds to contact and noncontact attacks with one eighth max HP and bad poison")
{
    enum Move move;
    s16 retort, poison;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &retort);
        MESSAGE("Wobbuffet was badly poisoned!");
        STATUS_ICON(player, badPoison: TRUE);
        HP_BAR(player, captureDamage: &poison);
    } THEN {
        EXPECT_EQ(retort, 100);
        EXPECT_EQ(poison, 50);
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

static const struct { enum Species species; u32 damage; } sRetortTypes[] = {
    { SPECIES_WOBBUFFET, 100 }, { SPECIES_GOGOAT, 200 }, { SPECIES_CLEFAIRY, 200 },
    { SPECIES_WHIMSICOTT, 400 }, { SPECIES_EKANS, 50 }, { SPECIES_SANDSHREW, 50 },
    { SPECIES_ROGGENROLA, 50 }, { SPECIES_DUSKULL, 50 }, { SPECIES_GASTLY, 25 },
    { SPECIES_MAGNEMITE, 0 }, { SPECIES_DIGLETT_ALOLA, 0 }, { SPECIES_BIBAREL, 100 },
};

SINGLE_BATTLE_TEST("Serpent's Retort scales only with the Poison type chart including dual types and Steel immunity")
{
    u32 variant = 0;
    s16 damage;
    for (u32 j = 0; j < ARRAY_COUNT(sRetortTypes); j++)
        PARAMETRIZE { variant = j; }
    GIVEN {
        PLAYER(sRetortTypes[variant].species) { Ability(ABILITY_IMMUNITY); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        if (sRetortTypes[variant].damage) {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player, captureDamage: &damage);
        } else {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
                HP_BAR(player);
            }
        }
    } THEN {
        if (sRetortTypes[variant].damage)
            EXPECT_EQ(damage, sRetortTypes[variant].damage);
        EXPECT_EQ(player->hp, 800 - sRetortTypes[variant].damage);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort is not reduced or blocked by defensive abilities or items")
{
    enum Ability ability;
    enum Item item;
    s16 damage;
    PARAMETRIZE { ability = ABILITY_MAGIC_GUARD; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_WONDER_GUARD; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_FILTER; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SOLID_ROCK; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHIELD_DUST; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_STICKY_SPRAY; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
        MESSAGE("Wobbuffet was badly poisoned!");
    } THEN {
        EXPECT_EQ(damage, 100);
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort has minimum one damage for a nonimmune attacker")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_IMMUNITY); HP(7); MaxHP(7); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(damage, 1);
        EXPECT_EQ(player->hp, 6);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort respects poison immunities without suppressing its damage")
{
    enum Species species;
    enum Ability ability;
    s16 damage;
    PARAMETRIZE { species = SPECIES_SNORLAX; ability = ABILITY_IMMUNITY; }
    PARAMETRIZE { species = SPECIES_KOMALA; ability = ABILITY_COMATOSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PURIFYING_SALT; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; ability = ABILITY_PASTEL_VEIL; }
    PARAMETRIZE { species = SPECIES_EKANS; ability = ABILITY_SHED_SKIN; }
    GIVEN {
        PLAYER(species) { Ability(ability); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
        NOT STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(damage, species == SPECIES_EKANS ? 50 : 100);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort does not replace an existing status or upgrade regular poison")
{
    u32 status;
    s16 damage;
    PARAMETRIZE { status = STATUS1_BURN; }
    PARAMETRIZE { status = STATUS1_POISON; }
    PARAMETRIZE { status = STATUS1_PARALYSIS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(status); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
        NOT STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(damage, 100);
        EXPECT_EQ(player->status1, status);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort poison respects Safeguard and Misty Terrain")
{
    enum Move protection;
    PARAMETRIZE { protection = MOVE_SAFEGUARD; }
    PARAMETRIZE { protection = MOVE_MISTY_TERRAIN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, protection); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player);
        NOT STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(player->hp, 700);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort triggers independently of contact protection and ability suppression")
{
    enum Ability ability;
    enum Item item;
    s16 damage;
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NEUTRALIZING_GAS; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_LONG_REACH; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_SHEER_FORCE; item = ITEM_NONE; }
    PARAMETRIZE { ability = ABILITY_NONE; item = ITEM_PROTECTIVE_PADS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_CRUNCH, secondaryEffect: FALSE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
        STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(damage, 100);
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort does not trigger from status moves missed attacks or Protect")
{
    enum Move move;
    bool32 protect = FALSE;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_MEGA_PUNCH; }
    PARAMETRIZE { move = MOVE_SCRATCH; protect = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); }
    } WHEN {
        TURN { MOVE(player, move, hit: move != MOVE_MEGA_PUNCH); if (protect) MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort does not trigger when Arbok's Substitute absorbs the attack even if it breaks")
{
    u32 attack;
    PARAMETRIZE { attack = 1; }
    PARAMETRIZE { attack = 1000; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(attack); HP(800); MaxHP(800); Speed(100); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(800); MaxHP(800); Defense(100); Speed(200); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 600);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort bypasses the attacker's Substitute for damage and poison")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
        STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(damage, 100);
        EXPECT_EQ((u32)player->volatiles.substituteHP, 200);
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort does not trigger if Arbok is KOed by the attack")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(opponent, 1); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort triggers when Endure leaves Arbok alive at one HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(2); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ENDURE); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player);
        STATUS_ICON(player, badPoison: TRUE);
    } THEN {
        EXPECT_EQ(opponent->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort can KO the attacker without poisoning a fainted Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(800); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player);
        NONE_OF {
            MESSAGE("Wobbuffet was badly poisoned!");
            STATUS_ICON(player, badPoison: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort responds to every survived strike of a multi hit attack")
{
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_KICK); }
    } SCENE {
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage[0]);
        MESSAGE("Wobbuffet was badly poisoned!");
        HP_BAR(opponent);
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage[1]);
    } THEN {
        EXPECT_EQ(damage[0], 100);
        EXPECT_EQ(damage[1], 100);
        EXPECT_EQ(player->hp, 550);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort stops a multi hit attack when its attacker faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_IMMUNITY); HP(150); MaxHP(800); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ARM_THRUST, WITH_RNG(RNG_HITS, 5)); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
        HP_BAR(opponent);
        HP_BAR(player);
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort uses the attacker's current Tera type")
{
    enum Type type;
    s16 damage;
    PARAMETRIZE { type = TYPE_STEEL; }
    PARAMETRIZE { type = TYPE_GHOST; }
    PARAMETRIZE { type = TYPE_POISON; }
    GIVEN {
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_IMMUNITY); TeraType(type); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, gimmick: GIMMICK_TERA); }
    } SCENE {
        if (type == TYPE_STEEL) {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
                HP_BAR(player);
            }
        } else {
            ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
            HP_BAR(player, captureDamage: &damage);
        }
    } THEN {
        if (type != TYPE_STEEL)
            EXPECT_EQ(damage, 50);
        EXPECT_EQ(player->hp, type == TYPE_STEEL ? 800 : 750);
    }
}

DOUBLE_BATTLE_TEST("Serpent's Retort responds separately for both surviving targets of a spread attack")
{
    s16 damage[2];
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_IMMUNITY); HP(800); MaxHP(800); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE); }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_SERPENTS_RETORT);
        HP_BAR(playerLeft, captureDamage: &damage[0]);
        ABILITY_POPUP(opponentRight, ABILITY_SERPENTS_RETORT);
        HP_BAR(playerLeft, captureDamage: &damage[1]);
    } THEN {
        EXPECT_EQ(damage[0], 100);
        EXPECT_EQ(damage[1], 100);
        EXPECT_EQ(playerLeft->hp, 600);
    }
}

SINGLE_BATTLE_TEST("Serpent's Retort includes extra types added by Forest's Curse and Trick-or-Treat")
{
    enum Move move;
    u32 expected;
    s16 damage;
    PARAMETRIZE { move = MOVE_FORESTS_CURSE; expected = 200; }
    PARAMETRIZE { move = MOVE_TRICK_OR_TREAT; expected = 50; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_IMMUNITY); HP(800); MaxHP(800); }
        OPPONENT(SPECIES_ARBOK) { Item(ITEM_APEX_METEORITE); HP(3000); MaxHP(3000); }
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ABILITY_POPUP(opponent, ABILITY_SERPENTS_RETORT);
        HP_BAR(player, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(damage, expected);
    }
}
