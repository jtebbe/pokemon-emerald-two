#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "malloc.h"
#include "pokemon.h"
#include "party_menu.h"
#include "event_data.h"
#include "constants/abilities.h"
#include "random.h"
#include "battle_scripts.h"
#include "window.h"
#include "constants/battle_string_ids.h"

void AllocateBattleGfxResources(void)
{
    gBattleAnimBgTileBuffer = AllocZeroed(0x2000);
    gBattleAnimBgTilemapBuffer = AllocZeroed(0x1000);
}

static void FreeBattleGfxResources(void)
{
    FREE_AND_SET_NULL(gBattleAnimBgTileBuffer);
    FREE_AND_SET_NULL(gBattleAnimBgTilemapBuffer);
}

void CloseMainBattleScreen(void)
{
    FreeBattleGfxResources();
    FreeAllWindowBuffers();
    HideWeatherIndicator(TRUE);
}

void AllocateBattleResources(void)
{
    {
        ;
    }

    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gAiBattleData = AllocZeroed(sizeof(*gAiBattleData));
    gAiThinkingStruct = AllocZeroed(sizeof(*gAiThinkingStruct));
    gAiLogicData = AllocZeroed(sizeof(*gAiLogicData));
    gAiPartyData = AllocZeroed(sizeof(*gAiPartyData));
    gBattleHistory = AllocZeroed(sizeof(*gBattleHistory));

#if B_FLAG_SKY_BATTLE
    gBattleStruct->isSkyBattle = FlagGet(B_FLAG_SKY_BATTLE);
#endif

    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattleResources->battleScriptsStack = AllocZeroed(sizeof(*gBattleResources->battleScriptsStack));
    gBattleResources->battleCallbackStack = AllocZeroed(sizeof(*gBattleResources->battleCallbackStack));
    gBattleResources->beforeLvlUp = AllocZeroed(sizeof(*gBattleResources->beforeLvlUp));

    gLinkBattleSendBuffer = AllocZeroed(BATTLE_BUFFER_LINK_SIZE);
    gLinkBattleRecvBuffer = AllocZeroed(BATTLE_BUFFER_LINK_SIZE);

    AllocateBattleGfxResources();

    ;
}

void FreeBattleResources(void)
{
    {
        ;
    }

    memset(&gFieldTimers, 0, sizeof(struct FieldTimer));
    memset(&gSideTimers, 0, sizeof(struct SideTimer));

    if (gBattleResources != NULL)
    {
        FREE_AND_SET_NULL(gBattleStruct);
        FREE_AND_SET_NULL(gAiBattleData);
        FREE_AND_SET_NULL(gAiThinkingStruct);
        FREE_AND_SET_NULL(gAiLogicData);
        FREE_AND_SET_NULL(gAiPartyData);
        FREE_AND_SET_NULL(gBattleHistory);

        FREE_AND_SET_NULL(gBattleResources->battleScriptsStack);
        FREE_AND_SET_NULL(gBattleResources->battleCallbackStack);
        FREE_AND_SET_NULL(gBattleResources->beforeLvlUp);
        FREE_AND_SET_NULL(gBattleResources);

        FREE_AND_SET_NULL(gLinkBattleSendBuffer);
        FREE_AND_SET_NULL(gLinkBattleRecvBuffer);

        FreeBattleGfxResources();
    }
}



void SwitchPartyOrderInGameMulti(enum BattlerId battler, enum PartyMon partyId)
{
    if (IsOnPlayerSide(battler))
    {
        s32 i;
        enum PartyMon battlerPartyId = gBattlerPartyIndexes[battler];
        enum PartyMon switchInPartyId = partyId;
        enum BattleTrainer trainer = GetBattlerTrainer(battler);

        // In 6v6 multis, the partner party is stored in gParties[B_TRAINER_PARTNER]
        // and uses indexes 0-2, but we still use the combined party order.
        if (IsMultiBattle() == TRUE && !AreMultiPartiesFullTeams() && trainer == B_TRAINER_PARTNER)
        {
            battlerPartyId = (enum PartyMon)(battlerPartyId + MULTI_PARTY_SIZE);
            switchInPartyId = (enum PartyMon)(switchInPartyId + MULTI_PARTY_SIZE);
        }

        for (enum BattlerId battlerId = 0; battlerId < gBattlersCount; battlerId++)
        {
            if (!IsOnPlayerSide(battlerId))
                continue;

            // Don't update battler's orders for party menu if the switching battler and updating battler
            // don't share a party, unless it's a 6v6 multi where player and partner party are temporarily
            // merged for party menu and summary screen viewing
            if (!(IsMultiBattle() == TRUE && !AreMultiPartiesFullTeams()) && !BattlersShareParty(battler, battlerId))
                continue;

            for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
                gBattlePartyCurrentOrder[i] = gBattleStruct->battlerPartyOrders[battlerId][i];

            SwitchPartyMonSlots(GetBattleSlotFromBattlePartyId(battlerPartyId), GetBattleSlotFromBattlePartyId(switchInPartyId));

            for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
                gBattleStruct->battlerPartyOrders[battlerId][i] = gBattlePartyCurrentOrder[i];
        }
    }
}



struct Pokemon *GetBattlerParty(enum BattlerId battler)
{
    return gParties[GetBattlerTrainer(battler)];
}

struct Pokemon *GetTrainerParty(enum BattleTrainer trainer)
{
    return gParties[trainer];
}

struct Pokemon* GetBattlerMon(enum BattlerId battler)
{
    enum PartyMon index = gBattlerPartyIndexes[battler];

    return &GetBattlerParty(battler)[index];
}
