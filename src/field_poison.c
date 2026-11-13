#include "global.h"
#include "main.h"
#include "overworld.h"
#include "battle.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_message_box.h"
#include "field_poison.h"
#include "fldeff_misc.h"
#include "party_menu.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/field_poison.h"
#include "constants/form_change_types.h"
#include "constants/party_menu.h"

static bool32 AllMonsFainted(void)
{
    for (enum PartySlot slot = 0; slot < PARTY_SIZE; slot++)
    {
        struct Pokemon *pokemon = &gParties[B_TRAINER_PLAYER][slot];

        if (IsMonValidAndAliveForBattle(pokemon))
            return FALSE;
    }
    return TRUE;
}

static void FaintFromFieldPoison(u8 partyIdx)
{
    struct Pokemon *pokemon = &gParties[B_TRAINER_PLAYER][partyIdx];
    u32 status = STATUS1_NONE;

    if (OW_POISON_DAMAGE < GEN_4)

    SetMonData(pokemon, MON_DATA_STATUS, &status);
    GetMonData(pokemon, MON_DATA_NICKNAME, gStringVar1);
    StringGet_Nickname(gStringVar1);
}

static bool32 MonFaintedFromPoison(enum PartySlot slot)
{
    u32 remainingHealth = (OW_POISON_DAMAGE < GEN_4) ? 0 : 1;
    struct Pokemon *pokemon = &gParties[B_TRAINER_PLAYER][slot];

    if (IsMonValidForBattle(pokemon) && GetMonData(pokemon, MON_DATA_HP) == remainingHealth && GetAilmentFromStatus(GetMonData(pokemon, MON_DATA_STATUS)) == AILMENT_PSN)
        return TRUE;

    return FALSE;
}

#define tState    data[0]
#define tPartyIdx data[1]

static void Task_TryFieldPoisonWhiteOut(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    switch (tState)
    {
    case 0:
        for (; tPartyIdx < PARTY_SIZE; tPartyIdx++)
        {
            if (MonFaintedFromPoison(tPartyIdx))
            {
                FaintFromFieldPoison(tPartyIdx);
                ShowFieldMessage(gText_PkmnFainted_FldPsn);
                tState++;
                return;
            }
        }
        tState = 2; // Finished checking party
        break;
    case 1:
        // Wait for "{mon} fainted" message, then return to party loop
        if (IsFieldMessageBoxHidden())
            tState--;
        break;
    case 2:
        if (AllMonsFainted())
        {
            SetMainCallback2(CB2_WhiteOut);
            UpdateFollowingPokemon();
        }
        ScriptContext_Enable();
        DestroyTask(taskId);
        break;
    }
}

#undef tState
#undef tPartyIdx

void TryFieldPoisonWhiteOut(void)
{
    CreateTask(Task_TryFieldPoisonWhiteOut, 80);
    ScriptContext_Stop();
}

s32 DoPoisonFieldEffect(void)
{
    int i;
    u32 hp;
    struct Pokemon *pokemon = gParties[B_TRAINER_PLAYER];
    u32 numPoisoned = 0;
    u32 numFainted = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(pokemon, MON_DATA_SPECIES) && GetAilmentFromStatus(GetMonData(pokemon, MON_DATA_STATUS)) == AILMENT_PSN)
        {
            // Apply poison damage
            hp = GetMonData(pokemon, MON_DATA_HP);
            if (OW_POISON_DAMAGE < GEN_4 && (hp == 0 || --hp == 0))
            {
                TryFormChange(&gParties[B_TRAINER_PLAYER][i], FORM_CHANGE_FAINT, B_TRAINER_PLAYER);
                numFainted++;
            }
            else if (OW_POISON_DAMAGE >= GEN_4 && (hp == 1 || --hp == 1))
                numFainted++;

            SetMonData(pokemon, MON_DATA_HP, &hp);
            numPoisoned++;
        }
        pokemon++;
    }

    // Do screen flash effect
    if (numFainted != 0 || numPoisoned != 0)
        FldEffPoison_Start();

    if (numFainted != 0)
        return FLDPSN_FNT;

    if (numPoisoned != 0)
        return FLDPSN_PSN;

    return FLDPSN_NONE;
}
