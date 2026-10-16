#include "global.h"
#include "link.h"
#include "link_trade.h"
#include "link_protocol.h"
#include "save.h"
#include "load_save.h"
#include "fieldmap.h"
#include "pc_storage.h"

// Deliberately small: no second party, save image, or flash-sector RAM buffer.
static EWRAM_DATA struct
{
    struct TradeJournalOffer offers[2];
    u16 timer;
    u8 mode;
    u8 slot;
    u8 state;
    u8 nextState;
    u8 round;
    bool8 recovering;
} sTradeProtocol;

void LinkTradeStart(enum LinkTradeMode mode, u32 partySlot)
{
    memset(&sTradeProtocol, 0, sizeof(sTradeProtocol));
    sTradeProtocol.mode = mode;
    sTradeProtocol.slot = partySlot;
}

static bool32 Pending(u8 phase)
{
    return phase == TRADE_JOURNAL_PREPARED || phase == TRADE_JOURNAL_READY
        || phase == TRADE_JOURNAL_COMMITTED;
}

static void Barrier(u8 nextState)
{
    // A fast peer must not overwrite the block buffer before a slower peer has
    // copied the previous round and durably recorded its decision.
    SetLinkStandbyCallback();
    sTradeProtocol.nextState = nextState;
    sTradeProtocol.state = 11;
    sTradeProtocol.timer = 0;
}

enum LinkTradeResult LinkTradePoll(void)
{
    u32 id = GetMultiplayerId();
    if (PCStorageFaulted() || !gReceivedRemoteLinkPlayers || GetLinkPlayerCount() != 2
        || !LinkProtocolPlayersCompatible() || ++sTradeProtocol.timer > 1800)
        return LINK_TRADE_ERROR;
    switch (sTradeProtocol.state)
    {
    case 0:
        if (!IsLinkTaskFinished())
            break;
        ResetBlockReceivedFlags();
        if (sTradeProtocol.mode == LINK_TRADE_FINALIZE && !TradeJournalReady())
            return LINK_TRADE_ERROR;
        Barrier(1);
        break;
    case 1:
        if (!IsLinkTaskFinished())
            break;
        if (!TradeJournalGetOffer(&sTradeProtocol.offers[id], PARTY_SIZE))
            return LINK_TRADE_ERROR;
        if (SendBlock(0, &sTradeProtocol.offers[id], sizeof(struct TradeJournalOffer)))
            sTradeProtocol.state = 2;
        break;
    case 2:
        if (GetBlockReceivedStatus() != 3 || !IsLinkTaskFinished())
            break;
        for (u32 i = 0; i < 2; i++)
        {
            memcpy(&sTradeProtocol.offers[i], gBlockRecvBuffer[i], sizeof(struct TradeJournalOffer));
            if (sTradeProtocol.offers[i].buildId != LinkProtocolBuildId()
                || sTradeProtocol.offers[i].trainerId != gLinkPlayers[i].trainerId
                || memcmp(sTradeProtocol.offers[i].name, gLinkPlayers[i].name, PLAYER_NAME_LENGTH + 1) != 0)
                return LINK_TRADE_ERROR;
        }
        ResetBlockReceivedFlags();
        if (Pending(sTradeProtocol.offers[0].phase) || Pending(sTradeProtocol.offers[1].phase))
            sTradeProtocol.recovering = TRUE;
        if (sTradeProtocol.round < 2)
        {
            if (!TradeJournalReconcile(&sTradeProtocol.offers[id ^ 1]))
                return LINK_TRADE_ERROR;
            sTradeProtocol.round++;
            Barrier(1);
            break;
        }
        if (Pending(sTradeProtocol.offers[0].phase) || Pending(sTradeProtocol.offers[1].phase))
        {
            if (sTradeProtocol.mode != LINK_TRADE_FINALIZE && sTradeProtocol.recovering
                && !TradeJournalNeedsPeer())
                return LINK_TRADE_RECOVERED;
            return LINK_TRADE_ERROR;
        }
        Barrier(12);
        break;
    case 12:
        if (sTradeProtocol.mode == LINK_TRADE_FINALIZE)
        {
            ClearContinueGameWarpStatus2();
            return LINK_TRADE_OK;
        }
        if (sTradeProtocol.recovering)
            return LINK_TRADE_RECOVERED; // Reload the checkpoint before any new trade.
        if (sTradeProtocol.mode == LINK_TRADE_PREFLIGHT)
            return LINK_TRADE_OK;
        SetLinkStandbyCallback();
        sTradeProtocol.state = 3;
        break;
    case 3:
        if (!IsLinkTaskFinished())
            break;
        SaveMapView();
        SetContinueGameWarpStatusToDynamicWarp();
        if (!SaveFlashAvailable() || LinkBattleStateActive() || TradeJournalBlocksSaving())
            return LINK_TRADE_ERROR;
        sTradeProtocol.state = 4;
        sTradeProtocol.timer = 0;
        break;
    case 4:
        if (sTradeProtocol.timer < 5)
            break;
        sTradeProtocol.timer = 0;
        // The streamed layout commits atomically through its recovery journal.
        // Do not offer a trade if the checkpoint failed.
        if (TrySavingData(SAVE_LINK) != SAVE_STATUS_OK)
            return LINK_TRADE_ERROR;
        Barrier(7);
        break;
    case 7:
        if (!IsLinkTaskFinished())
            break;
        if (!TradeJournalGetOffer(&sTradeProtocol.offers[id], sTradeProtocol.slot))
            return LINK_TRADE_ERROR;
        if (SendBlock(0, &sTradeProtocol.offers[id], sizeof(struct TradeJournalOffer)))
            sTradeProtocol.state = 8;
        break;
    case 8:
        if (GetBlockReceivedStatus() != 3 || !IsLinkTaskFinished())
            break;
        for (u32 i = 0; i < 2; i++)
            memcpy(&sTradeProtocol.offers[i], gBlockRecvBuffer[i], sizeof(struct TradeJournalOffer));
        ResetBlockReceivedFlags();
        if (!TradeJournalPrepare(sTradeProtocol.offers, id))
            return LINK_TRADE_ERROR;
        Barrier(9);
        break;
    case 9:
        if (!IsLinkTaskFinished())
            break;
        if (!TradeJournalGetOffer(&sTradeProtocol.offers[id], PARTY_SIZE))
            return LINK_TRADE_ERROR;
        if (SendBlock(0, &sTradeProtocol.offers[id], sizeof(struct TradeJournalOffer)))
            sTradeProtocol.state = 10;
        break;
    case 10:
        if (GetBlockReceivedStatus() != 3 || !IsLinkTaskFinished())
            break;
        memcpy(&sTradeProtocol.offers[id ^ 1], gBlockRecvBuffer[id ^ 1], sizeof(struct TradeJournalOffer));
        ResetBlockReceivedFlags();
        if (sTradeProtocol.offers[id ^ 1].phase != TRADE_JOURNAL_PREPARED
            || !TradeJournalSameTransaction(&sTradeProtocol.offers[id ^ 1]))
            return LINK_TRADE_ERROR;
        Barrier(13);
        break;
    case 11:
        if (IsLinkTaskFinished())
        {
            sTradeProtocol.state = sTradeProtocol.nextState;
            sTradeProtocol.timer = 0;
        }
        break;
    case 13:
        return LINK_TRADE_OK;
    }
    return LINK_TRADE_WAIT;
}
