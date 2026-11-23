#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

SINGLE_BATTLE_TEST("The Johto starter families receive their configured Apex abilities")
{
    enum Species species;
    enum Ability ability;
    PARAMETRIZE { species = SPECIES_CHIKORITA; ability = ABILITY_FLORAL_HEALING; }
    PARAMETRIZE { species = SPECIES_BAYLEEF; ability = ABILITY_FLORAL_HEALING; }
    PARAMETRIZE { species = SPECIES_MEGANIUM; ability = ABILITY_FLORAL_HEALING; }
    PARAMETRIZE { species = SPECIES_CYNDAQUIL; ability = ABILITY_VOLCANISM; }
    PARAMETRIZE { species = SPECIES_QUILAVA; ability = ABILITY_VOLCANISM; }
    PARAMETRIZE { species = SPECIES_TYPHLOSION; ability = ABILITY_VOLCANISM; }
    PARAMETRIZE { species = SPECIES_TYPHLOSION_HISUI; ability = ABILITY_SOULSCORCHER; }
    PARAMETRIZE { species = SPECIES_TOTODILE; ability = ABILITY_SCALY_SCRAPPER; }
    PARAMETRIZE { species = SPECIES_CROCONAW; ability = ABILITY_SCALY_SCRAPPER; }
    PARAMETRIZE { species = SPECIES_FERALIGATR; ability = ABILITY_SCALY_SCRAPPER; }
    GIVEN {
        PLAYER(species) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ability);
    }
}

SINGLE_BATTLE_TEST("Volcanism boosts Fire damage by 30 percent", s16 damage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; }
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Volcanism makes Fire super effective against Rock")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_REGIROCK) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("It's super effective!");
    }
}

DOUBLE_BATTLE_TEST("Volcanism consumes only opposing burns and deals one third max HP once")
{
    GIVEN {
        PLAYER(SPECIES_QUILAVA) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); HP(300); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); HP(300); MaxHP(300); }
        OPPONENT(SPECIES_WYNAUT) { Status1(STATUS1_BURN); HP(150); MaxHP(150); }
    } WHEN {
        TURN {}
        TURN {}
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 200);
        EXPECT_EQ(opponentRight->hp, 100);
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        EXPECT(playerRight->status1 & STATUS1_BURN);
        EXPECT_EQ(playerRight->hp, 264);
    }
}

SINGLE_BATTLE_TEST("Volcanism clears burns but respects Magic Guard")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_CLEFAIRY) { Ability(ABILITY_MAGIC_GUARD); Status1(STATUS1_BURN); HP(300); MaxHP(300); }
    } WHEN {
        TURN {}
    } THEN {
        EXPECT_EQ(opponent->hp, 300);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Volcanism stops consuming burns after the holder switches out")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); HP(300); MaxHP(300); }
    } WHEN {
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->hp, 282);
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Soulscorcher halves damage to healthy targets and doubles damage to statused targets", s16 damage)
{
    enum Item item;
    u32 status;
    PARAMETRIZE { item = ITEM_NONE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_POISON; }
    PARAMETRIZE { item = ITEM_APEX_METEORITE; status = STATUS1_BURN; }
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUI) { Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Status1(status); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        for (u32 j = 2; j < 5; j++)
            EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[j].damage);
    }
}

AI_SINGLE_BATTLE_TEST("AI forecasts Volcanism damage and burn eruption")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_REGIROCK) { HP(1200); MaxHP(1200); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_TYPHLOSION) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage damage = AI_CalcDamageSaveBattlers(MOVE_EMBER, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_GT(damage.median, 0);
        EXPECT_EQ(effectiveness, UQ_4_12(2.0));
        player->status1 = STATUS1_BURN;
        EXPECT_EQ(GetBattlerSecondaryDamage(B_BATTLER_0), 400);
    }
}

AI_SINGLE_BATTLE_TEST("AI forecasts Soulscorcher status damage and Ghost immunity bypass")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_PORYGON) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_TYPHLOSION_HISUI) { Item(ITEM_APEX_METEORITE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {}
    } THEN {
        uq4_12_t effectiveness;
        struct SimulatedDamage healthy = AI_CalcDamageSaveBattlers(MOVE_SHADOW_BALL, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_EQ(effectiveness, UQ_4_12(1.0));
        player->status1 = STATUS1_PARALYSIS;
        struct SimulatedDamage statused = AI_CalcDamageSaveBattlers(MOVE_SHADOW_BALL, B_BATTLER_1, B_BATTLER_0,
            &effectiveness, GIMMICK_NONE, GIMMICK_NONE);
        EXPECT_MUL_EQ(healthy.median, Q_4_12(4.0), statused.median);
    }
}

AI_SINGLE_BATTLE_TEST("AI prefers Scaly Scrapper bites for their trapping effect")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_FERALIGATR) { Item(ITEM_APEX_METEORITE); Moves(MOVE_BITE, MOVE_ASSURANCE); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponent, MOVE_BITE);
            SCORE_GT(opponent, MOVE_BITE, MOVE_ASSURANCE);
        }
    }
}

SINGLE_BATTLE_TEST("Soulscorcher allows Ghost moves to hit Normal types")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION_HISUI) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_PORYGON) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing doubles recovery and shares half the actual restored HP")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(playerLeft, damage: -160);
        ABILITY_POPUP(playerLeft, ABILITY_FLORAL_HEALING);
        HP_BAR(playerRight, damage: -80);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 180);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing doubles potion healing and shares it")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA) { Item(ITEM_APEX_METEORITE); HP(20); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { USE_ITEM(playerLeft, ITEM_POTION, partyIndex: 0); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 60);
        EXPECT_EQ(playerRight->hp, 120);
    }
}

DOUBLE_BATTLE_TEST("Two Floral Healing holders double received healing without a recursive chain")
{
    GIVEN {
        PLAYER(SPECIES_BAYLEEF) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        PLAYER(SPECIES_CHIKORITA) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 400);
        EXPECT_EQ(playerRight->hp, 400);
        EXPECT(!gBattleStruct->floralHealing.active);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing respects an ally's Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); }
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_HEAL_BLOCK, target: playerRight); }
        TURN { SWITCH(playerLeft, 2); }
        TURN { MOVE(playerLeft, MOVE_RECOVER); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing protects allied Grass types from status and stat drops")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); }
        PLAYER(SPECIES_BULBASAUR);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerRight); MOVE(opponentRight, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Floral Healing provides Leaf Guard after the holder loses its Grass typing")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SOAK); }
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { MOVE(opponent, MOVE_THUNDER_WAVE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->types[0], TYPE_WATER);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing doubles passive Wish healing and shares it")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_WISH); }
        TURN {}
    } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 180);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing doubles draining recovery and shares the HP actually recovered")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); HP(40); MaxHP(600); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(600); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ABSORB, target: opponentLeft); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &damage);
        HP_BAR(playerLeft);
        HP_BAR(playerRight);
    } THEN {
        u32 healing = 2 * ((damage + 1) / 2);
        EXPECT_EQ(playerLeft->hp, 40 + healing);
        EXPECT_EQ(playerRight->hp, 100 + healing / 2);
    }
}

DOUBLE_BATTLE_TEST("Floral Healing preserves a pending HP update on its ally")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Item(ITEM_APEX_METEORITE); HP(20); MaxHP(200); }
        PLAYER(SPECIES_WOBBUFFET) { HP(200); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PAIN_SPLIT, target: playerRight); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
        EXPECT_EQ(playerRight->hp, 200);
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper has Rough Skin")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE) { Item(ITEM_APEX_METEORITE); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(240); MaxHP(240); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_SCALY_SCRAPPER);
        HP_BAR(opponent, damage: 30);
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper bites trap and deal one quarter max HP each turn")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_CROCONAW) { Item(ITEM_APEX_METEORITE); Level(5); Attack(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
        TURN {}
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_SCALY_SCRAPPER);
        HP_BAR(opponent, damage: 100);
        HP_BAR(opponent, damage: 100);
    } THEN {
        EXPECT_EQ(opponent->hp, 200 - damage);
        EXPECT_EQ((u32)opponent->volatiles.scalyScrapper, B_BATTLER_0 + 1);
        EXPECT(!CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper heals its holder when trapped prey faints from residual damage")
{
    GIVEN {
        PLAYER(SPECIES_FERALIGATR) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); Level(5); Attack(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); Defense(200); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); SEND_OUT(opponent, 1); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(opponent, hp: 0);
        ABILITY_POPUP(player, ABILITY_SCALY_SCRAPPER);
        HP_BAR(player, damage: -50);
    } THEN {
        EXPECT_EQ(player->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper heals after a biting attack immediately KOs its target")
{
    GIVEN {
        PLAYER(SPECIES_FERALIGATR) { Item(ITEM_APEX_METEORITE); HP(100); MaxHP(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper ends when its holder switches out")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_TOTODILE) { Item(ITEM_APEX_METEORITE); Level(5); Attack(10); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(opponent, damage: 100);
    } THEN {
        EXPECT_EQ(opponent->hp, 300 - damage);
        EXPECT(!opponent->volatiles.scalyScrapper);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper releases its prey when the holder faints")
{
    s16 damage;
    GIVEN {
        PLAYER(SPECIES_TOTODILE) { Item(ITEM_APEX_METEORITE); Level(5); Attack(10); HP(1); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(200); Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(opponent, damage: 100);
    } THEN {
        EXPECT_EQ(opponent->hp, 300 - damage);
        EXPECT(!opponent->volatiles.scalyScrapper);
        EXPECT(CanBattlerEscape(B_BATTLER_1));
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper needs a bite that damages the target itself")
{
    enum Move guard, attack;
    PARAMETRIZE { guard = MOVE_PROTECT; attack = MOVE_BITE; }
    PARAMETRIZE { guard = MOVE_SUBSTITUTE; attack = MOVE_BITE; }
    PARAMETRIZE { guard = MOVE_CELEBRATE; attack = MOVE_TACKLE; }
    GIVEN {
        PLAYER(SPECIES_TOTODILE) { Item(ITEM_APEX_METEORITE); Level(5); Attack(10); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(200); Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, guard); MOVE(player, attack, secondaryEffect: FALSE); }
    } THEN {
        EXPECT(!opponent->volatiles.scalyScrapper);
    }
}

SINGLE_BATTLE_TEST("Scaly Scrapper keeps Ghost escape and Magic Guard's damage protection")
{
    bool32 ghost;
    s16 damage;
    PARAMETRIZE { ghost = FALSE; }
    PARAMETRIZE { ghost = TRUE; }
    GIVEN {
        WITH_CONFIG(B_GHOSTS_ESCAPE, GEN_6);
        PLAYER(SPECIES_TOTODILE) { Item(ITEM_APEX_METEORITE); Level(5); Attack(10); }
        OPPONENT(ghost ? SPECIES_GASTLY : SPECIES_WOBBUFFET) {
            Ability(ghost ? ABILITY_LEVITATE : ABILITY_MAGIC_GUARD); HP(400); MaxHP(400); Defense(200);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, secondaryEffect: FALSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(opponent->hp, 400 - damage - (ghost ? 100 : 0));
        EXPECT_EQ(CanBattlerEscape(B_BATTLER_1), ghost);
    }
}
