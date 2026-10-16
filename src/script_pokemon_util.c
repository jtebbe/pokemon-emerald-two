#include "global.h"
#include "battle.h"
#include "battle_gfx_sfx_util.h"
#include "berry.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "event_data.h"
#include "international_string_util.h"
#include "item.h"
#include "link.h"
#include "link_rfu.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "ow_abilities.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "pc_storage.h"
#include "load_save.h"
#include "random.h"
#include "random_mon_generation.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "wild_encounter.h"
#include "constants/abilities.h"
#include "constants/items.h"

static void CB2_ReturnFromChooseHalfParty(void);
static void HealPlayerBoxes(void);

void HealPlayerParty(void)
{
    u32 i;
    for (i = 0; i < gPartiesCount[B_TRAINER_PLAYER]; i++)
        HealPokemon(&gParties[B_TRAINER_PLAYER][i]);
    if (OW_PC_HEAL >= GEN_8 && !LinkBattleStateActive() && !TradeJournalNeedsPeer())
        HealPlayerBoxes();

    // Recharge Tera Orb, if possible.
    if (!IsTeraOrbCharged() && CheckBagHasItem(ITEM_TERA_ORB, 1))
        FlagSet(B_FLAG_TERA_ORB_CHARGED);
}

static void HealPlayerBoxes(void)
{
    int boxId, boxPosition;
    struct BoxPokemon *boxMon;

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            boxMon = GetBoxedMonPtr(boxId, boxPosition);
            if (GetBoxMonData(boxMon, MON_DATA_SPECIES))
                HealBoxPokemon(boxMon);
        }
    }
}

u8 ScriptGiveEgg(enum Species species)
{
    struct Pokemon mon;
    u8 isEgg;

    CreateEgg(&mon, species, TRUE);
    isEgg = TRUE;
    SetMonData(&mon, MON_DATA_IS_EGG, &isEgg);

    return GiveCapturedMonToPlayer(&mon);
}

// TODO verify that this is really always the same output as the script special variant
u8 HasEnoughMonsForDoubleBattle2(void)
{
    return GetMonsStateToDoubles() == PLAYER_HAS_TWO_USABLE_MONS; 
}

void HasEnoughMonsForDoubleBattle(void)
{
    switch (GetMonsStateToDoubles())
    {
    case PLAYER_HAS_TWO_USABLE_MONS:
        gSpecialVar_Result = PLAYER_HAS_TWO_USABLE_MONS;
        break;
    case PLAYER_HAS_ONE_MON:
        gSpecialVar_Result = PLAYER_HAS_ONE_MON;
        break;
    case PLAYER_HAS_ONE_USABLE_MON:
        gSpecialVar_Result = PLAYER_HAS_ONE_USABLE_MON;
        break;
    }
}





void CreateScriptedWildMon(enum Species species, u8 level, enum Item item)
{
    u8 heldItem[2];

    ZeroEnemyPartyMons();
    u32 personality = GetMonPersonality(species,
        GetSynchronizedGender(STATIC_WILDMON_ORIGIN, species),
        GetSynchronizedNature(STATIC_WILDMON_ORIGIN, species),
        RANDOM_UNOWN_LETTER);
    CreateMonWithStats(&gParties[B_TRAINER_OPPONENT_A][0], species, level, personality, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);
    if (item)
    {
        heldItem[0] = item;
        heldItem[1] = item >> 8;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, heldItem);
    }
}
void CreateScriptedDoubleWildMon(enum Species species1, u8 level1, enum Item item1, enum Species species2, u8 level2, enum Item item2)
{
    u8 heldItem1[2];
    u8 heldItem2[2];

    ZeroEnemyPartyMons();
    u32 personality = GetMonPersonality(species1,
        GetSynchronizedGender(STATIC_WILDMON_ORIGIN, species1),
        GetSynchronizedNature(STATIC_WILDMON_ORIGIN, species1),
        RANDOM_UNOWN_LETTER);
    CreateMonWithStats(&gParties[B_TRAINER_OPPONENT_A][0], species1, level1, personality, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);
    if (item1)
    {
        heldItem1[0] = item1;
        heldItem1[1] = item1 >> 8;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, heldItem1);
    }

    personality = GetMonPersonality(species2,
        GetSynchronizedGender(STATIC_WILDMON_ORIGIN, species2),
        GetSynchronizedNature(STATIC_WILDMON_ORIGIN, species2),
        RANDOM_UNOWN_LETTER);
    CreateMonWithStats(&gParties[B_TRAINER_OPPONENT_A][1], species2, level2, personality, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][1]);
    if (item2)
    {
        heldItem2[0] = item2;
        heldItem2[1] = item2 >> 8;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HELD_ITEM, heldItem2);
    }
}

void ScriptSetMonMoveSlot(u8 monIndex, enum Move move, enum MoveSlot slot)
{
// Allows monIndex to go out of bounds of gParties[B_TRAINER_PLAYER]. Doesn't occur in vanilla
#ifdef BUGFIX
    if (monIndex >= PARTY_SIZE)
#else
    if (monIndex > PARTY_SIZE)
#endif
        monIndex = gPartiesCount[B_TRAINER_PLAYER] - 1;

    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][monIndex], move, slot);
}

// Note: When control returns to the event script, gSpecialVar_Result will be
// TRUE if the party selection was successful.
void ChooseHalfPartyForBattle(void)
{
    gMain.savedCallback = CB2_ReturnFromChooseHalfParty;
    gSpecialVar_0x8005 = MULTI_PARTY_SIZE;
    InitChooseHalfPartyForBattle(0);
}

static void CB2_ReturnFromChooseHalfParty(void)
{
    switch (gSelectedOrderFromParty[0])
    {
    case 0:
        gSpecialVar_Result = FALSE;
        break;
    default:
        gSpecialVar_Result = TRUE;
        break;
    }

    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}





void ReducePlayerPartyToSelectedMons(void)
{
    struct Pokemon party[PARTY_SIZE];
    int i;

    CpuFill32(0, party, sizeof party);

    // copy the selected Pokémon according to the order.
    for (i = 0; i < PARTY_SIZE; i++)
        if (gSelectedOrderFromParty[i]) // as long as the order keeps going (did the player select 1 mon? 2? 3?), do not stop
            party[i] = gParties[B_TRAINER_PLAYER][gSelectedOrderFromParty[i] - 1]; // index is 0 based, not literal

    CpuFill32(0, gParties[B_TRAINER_PLAYER], sizeof gParties[B_TRAINER_PLAYER]);

    // overwrite the first 4 with the order copied to.
    for (i = 0; i < PARTY_SIZE; i++)
        gParties[B_TRAINER_PLAYER][i] = party[i];

    CalculatePlayerPartyCount();
}

void CheckTeraType(struct ScriptContext *ctx)
{
    u32 partyIndex = VarGet(ScriptReadHalfword(ctx));

    Script_RequestEffects(SCREFF_V1);

    gSpecialVar_Result = TYPE_NONE;

    if (partyIndex < PARTY_SIZE)
        gSpecialVar_Result = GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_TERA_TYPE);
}

void SetTeraType(struct ScriptContext *ctx)
{
    enum Type type = ScriptReadByte(ctx);
    u32 partyIndex = VarGet(ScriptReadHalfword(ctx));

    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);

    if (type < NUMBER_OF_MON_TYPES && partyIndex < PARTY_SIZE)
        SetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_TERA_TYPE, &type);
}

/* Creates a Pokemon via script
 * if side/slot are assigned, it will create the mon at the assigned party location
 * if slot == PARTY_SIZE, it will give the mon to first available party or storage slot
 */
u32 ScriptGiveMonParameterized(u8 side, u8 slot, struct PokemonTemplate *monTemplate)
{
    struct Pokemon mon;

    CreateMonFromTemplate(&mon, monTemplate);

    if (side == B_SIDE_PLAYER)
        return GiveScriptedMonToPlayer(&mon, slot);

    assertf(slot < PARTY_SIZE, "invalid slot: %d", slot)
    {
        return MON_CANT_GIVE;
    }
    CopyMon(&gParties[B_TRAINER_OPPONENT_A][slot], &mon, sizeof(struct Pokemon));
    return MON_GIVEN_TO_PARTY;
}

u32 ScriptGiveMon(enum Species species, u8 level, enum Item item)
{
    struct Pokemon mon;
    u8 heldItem[2];

    CreateRandomMon(&mon, species, level);
    if (item)
    {
        heldItem[0] = item;
        heldItem[1] = item >> 8;
        SetMonData(&mon, MON_DATA_HELD_ITEM, heldItem);
    }

    return GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
}

#define PARSE_FLAG(n, default_) (flags & (1 << (n))) ? VarGet(ScriptReadHalfword(ctx)) : (default_)

/* Give or create a mon to either player or opponent
 */

void ScrCmd_createmon(struct ScriptContext *ctx)
{
    u32 i;
    u8 side                   = ScriptReadByte(ctx);
    u8 slot                   = ScriptReadByte(ctx);

    struct PokemonTemplate monTemplate = {0};
    monTemplate.species      = VarGet(ScriptReadHalfword(ctx));
    monTemplate.level        = VarGet(ScriptReadHalfword(ctx));

    u32 flags                 = ScriptReadWord(ctx);
    monTemplate.heldItem     = PARSE_FLAG(0, ITEM_NONE);
    if (flags & (1 << 1))
    {
        monTemplate.ball = VarGet(ScriptReadHalfword(ctx));
        monTemplate.doNotUseDefaultBall = TRUE;
    }
    monTemplate.nature       = PARSE_FLAG(2, NATURE_RANDOM);
    if (flags & (1 << 3))
    {
        monTemplate.abilityNum = VarGet(ScriptReadHalfword(ctx));
        monTemplate.doNotUseDefaultAbility = TRUE;
    }
    monTemplate.gender       = PARSE_FLAG(4, MON_GENDER_RANDOM);

    for (i = 0; i < NUM_STATS; i++)
        monTemplate.evs[i]   = PARSE_FLAG(5 + i, 0);

    for (i = 0; i < MAX_MON_MOVES; i++)
        monTemplate.moves[i] = PARSE_FLAG(11 + i, MOVE_DEFAULT);

    if (flags & (1 << 15))
    {
        monTemplate.isShiny = VarGet(ScriptReadHalfword(ctx));
        monTemplate.doNotUseDefaultShinyness = TRUE;
    }

    if (flags & (1 << 16))
    {
        monTemplate.teraType = VarGet(ScriptReadHalfword(ctx));
        monTemplate.doNotUseDefaultTeraType = TRUE;
    }
    monTemplate.isEgg        = PARSE_FLAG(17, FALSE);
    if (side == B_SIDE_PLAYER)
    {
        Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
        monTemplate.origin = GIFTMON_ORIGIN;
    }
    else
    {
        Script_RequestEffects(SCREFF_V1);
        monTemplate.origin = STATIC_WILDMON_ORIGIN;
    }

    monTemplate.ignoreTotalEvCheck = flags >> 18;

    gSpecialVar_Result = ScriptGiveMonParameterized(side, slot, &monTemplate);
}

#undef PARSE_FLAG

void Script_GetChosenMonOffensiveEVs(void)
{
    ConvertIntToDecimalStringN(gStringVar1, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_ATK_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_SPATK_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_SPEED_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
}

void Script_GetChosenMonDefensiveEVs(void)
{
    ConvertIntToDecimalStringN(gStringVar1, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_HP_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_DEF_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_SPDEF_EV), STR_CONV_MODE_LEFT_ALIGN, 3);
}

void Script_SetStatus1(struct ScriptContext *ctx)
{
    u32 status1 = VarGet(ScriptReadHalfword(ctx));
    u32 slot = VarGet(ScriptReadHalfword(ctx));

    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);

    if (slot >= PARTY_SIZE)
    {
        enum Species species;

        for (slot = 0; slot < PARTY_SIZE; slot++)
        {
            species = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES);
            if (species != SPECIES_NONE
             && species != SPECIES_EGG
             && GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP) != 0)
                SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_STATUS, &status1);
        }
    }
    else
    {
        SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_STATUS, &status1);
    }
}

void Script_SetKO(struct ScriptContext *ctx)
{
    u32 slot = VarGet(ScriptReadHalfword(ctx));

    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);

    if (slot < PARTY_SIZE)
    {
        u32 hp = 0;
        SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP, &hp);
    }
}

void Script_GiveRandomBerry(struct ScriptContext *ctx)
{
    enum BerryId loBerry = ScriptReadByte(ctx);
    enum BerryId hiBerry = ScriptReadByte(ctx);

    gSpecialVar_Result = BerryTypeToItemId(RandomUniform(RNG_RANDOM_BERRY, loBerry, hiBerry));
}
