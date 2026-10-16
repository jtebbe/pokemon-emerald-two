#include "global.h"
#include "party_menu.h"
#include "coins.h"
#include "constants/coins.h"
#include "battle.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "item.h"
#include "item_use.h"
#include "fieldmap.h"
#include "new_game.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokemon_storage_system.h"
#include "string_util.h"

static EWRAM_DATA u8 sPCBoxToSendMon = 0;

u8 GetPlayerTrainerIdOnesDigit(void)
{
    return (u16)((gSaveData.playerTrainerId[1] << 8) | gSaveData.playerTrainerId[0]) % 10;
}

u8 GetBattleOutcome(void)
{
    return gBattleOutcome;
}

void IsGrassTypeInParty(void)
{
    u8 i;
    enum Species species;
    struct Pokemon *pokemon;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        pokemon = &gParties[B_TRAINER_PLAYER][i];
        if (GetMonData(pokemon, MON_DATA_SPECIES) && !GetMonData(pokemon, MON_DATA_IS_EGG))
        {
            species = GetMonData(pokemon, MON_DATA_SPECIES);
            if (GetSpeciesType(species, 0) == TYPE_GRASS || GetSpeciesType(species, 1) == TYPE_GRASS)
            {
                gSpecialVar_Result = TRUE;
                return;
            }
        }
    }
    gSpecialVar_Result = FALSE;
}

bool8 Special_AreLeadMonEVsMaxedOut(void)
{
    if (GetMonEVCount(&gParties[B_TRAINER_PLAYER][GetLeadMonIndex()]) >= MAX_TOTAL_EVS)
        return TRUE;

    return FALSE;
}

bool8 ScriptCheckFreePokemonStorageSpace(void)
{
    return CheckFreePokemonStorageSpace();
}

u8 GetLeadMonIndex(void)
{
    u8 i;
    u8 partyCount = CalculatePlayerPartyCount();
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_EGG
         && GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) != SPECIES_NONE)
            return i;
    }
    return 0;
}

enum Species ScriptGetPartyMonSpecies(void)
{
    return GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004], MON_DATA_SPECIES_OR_EGG, NULL);
}

bool8 BufferTMHMMoveName(void)
{
    if (gItemsInfo[gSpecialVar_0x8004].pocket == POCKET_TM_HM)
    {
        StringCopy(gStringVar2, GetMoveName(ItemIdToBattleMoveId(gSpecialVar_0x8004)));
        return TRUE;
    }

    return FALSE;
}

void SetPCBoxToSendMon(u8 boxId)
{
    sPCBoxToSendMon = boxId;
}

u16 GetPCBoxToSendMon(void)
{
    return sPCBoxToSendMon;
}

bool8 ShouldShowBoxWasFullMessage(void)
{
    if (!FlagGet(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE))
    {
        if (StorageGetCurrentBox() != VarGet(VAR_PC_BOX_TO_SEND_MON))
        {
            FlagSet(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
            return TRUE;
        }
    }
    return FALSE;
}

void SetHiddenItemFlag(void)
{
    FlagSet(gSpecialVar_0x8004);
}

bool8 CheckAddCoins(void)
{
    return GetCoins() + gSpecialVar_0x8009 <= MAX_COINS;
}

bool8 IsDestinationBoxFull(void)
{
    int box;
    int i;
    SetPCBoxToSendMon(VarGet(VAR_PC_BOX_TO_SEND_MON));
    box = StorageGetCurrentBox();
    do
    {
        for (i = 0; i < IN_BOX_COUNT; i++)
        {
            if (GetBoxMonData(GetBoxedMonPtr(box, i), MON_DATA_SPECIES, 0) == SPECIES_NONE)
            {
                if (GetPCBoxToSendMon() != box)
                    FlagClear(FLAG_SHOWN_BOX_WAS_FULL_MESSAGE);
                VarSet(VAR_PC_BOX_TO_SEND_MON, box);
                return ShouldShowBoxWasFullMessage();
            }
        }

        if (++box == TOTAL_BOXES_COUNT)
            box = 0;
    } while (box != StorageGetCurrentBox());
    return FALSE;
}

void GetObjectPosition(u16* xPointer, u16* yPointer, u32 localId, u32 useTemplate)
{
    u32 objectId;
    struct ObjectEvent* objEvent;

    if (useTemplate)
    {
        const struct ObjectEventTemplate *objTemplate = FindObjectEventTemplateByLocalId(localId, gSaveData.objectEventTemplates, gMapHeader.events->objectEventCount);
        *xPointer = objTemplate->x;
        *yPointer = objTemplate->y;
        return;
    }

    objectId = GetObjectEventIdByLocalId(localId);
    objEvent = &gObjectEvents[objectId];
    *xPointer = objEvent->currentCoords.x - MAP_OFFSET;
    *yPointer = objEvent->currentCoords.y - MAP_OFFSET;
}

bool32 CheckObjectAtXY(u32 x, u32 y)
{
    u32 i;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (!gObjectEvents[i].active)
            continue;

        if (gObjectEvents[i].currentCoords.x != x)
            continue;

        if (gObjectEvents[i].currentCoords.y != y)
            continue;
        return TRUE;
    }
    return FALSE;
}

bool32 CheckPartyHasSpecies(enum Species givenSpecies)
{
    u32 partyIndex;

    for (partyIndex = 0; partyIndex < CalculatePlayerPartyCount(); partyIndex++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_SPECIES) == givenSpecies)
            return TRUE;

    return FALSE;
}

void SetSeenMon(void)
{
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(gSpecialVar_0x8004), 2);
}

bool8 DoesPlayerPartyContainSpecies(void)
{
    u8 partyCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG, NULL) == gSpecialVar_0x8004)
            return TRUE;
    }
    return FALSE;
}

bool8 PlayerPartyContainsSpeciesWithPlayerID(void)
{
    // 8004 = species
    u8 playerCount = CalculatePlayerPartyCount();
    u8 i;
    for (i = 0; i < playerCount; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG, NULL) == gSpecialVar_0x8004
            && GetTrainerId(gSaveData.playerTrainerId) == GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_OT_ID, NULL))
            return TRUE;
    }
    return FALSE;
}
