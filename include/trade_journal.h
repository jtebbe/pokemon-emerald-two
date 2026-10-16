#ifndef GUARD_TRADE_JOURNAL_H
#define GUARD_TRADE_JOURNAL_H

// PREPARED and READY are deliberately not automatically rolled back on reboot.
enum TradeJournalPhase
{
    TRADE_JOURNAL_EMPTY,
    TRADE_JOURNAL_PREPARED,
    TRADE_JOURNAL_READY,
    TRADE_JOURNAL_COMMITTED,
    TRADE_JOURNAL_DONE,
    TRADE_JOURNAL_ABORTED,
};

struct TradeJournalMetadata
{
    u32 sequence;
    u32 transaction[2];
    u32 peerTrainerId;
    u32 peerSequence;
    u32 checkpointCounter;
    u32 buildId;
    u8 peerName[PLAYER_NAME_LENGTH + 1];
    u8 phase;
    u8 partySlot;
    u8 peerSlot;
    u8 reserved; // Keep the on-flash metadata layout stable; must be zero.
    struct BoxPokemon outgoing;
    u32 offeredHash;
};

struct TradeJournalOffer
{
    u32 buildId;
    u32 trainerId;
    u32 sequence;
    u32 monHash;
    u32 transaction[2];
    u32 peerTrainerId;
    u32 peerSequence;
    u32 offeredHash;
    u8 name[PLAYER_NAME_LENGTH + 1];
    u8 peerName[PLAYER_NAME_LENGTH + 1];
    u8 phase;
    u8 partySlot;
    u8 peerSlot;
    u8 reserved;
};

bool32 TradeJournalRead(struct TradeJournalMetadata *metadata);
bool32 TradeJournalNeedsPeer(void);
bool32 TradeJournalBlocksSaving(void);
bool32 TradeJournalPrepare(const struct TradeJournalOffer offers[2], u32 localId);
bool32 TradeJournalReady(void);
bool32 TradeJournalCommit(void);
bool32 TradeJournalFinish(void);
bool32 TradeJournalAbort(void);
bool32 TradeJournalRestoreHome(void);
bool32 TradeJournalGetOffer(struct TradeJournalOffer *offer, u32 partySlot);
bool32 TradeJournalSameTransaction(const struct TradeJournalOffer *peer);
bool32 TradeJournalLoadCandidate(void);
bool32 TradeJournalValidateSwap(u32 partySlot, u32 peerSlot);
// Advance only on proof from the same partner; never infer a commit from silence.
bool32 TradeJournalReconcile(const struct TradeJournalOffer *peer);
void TradeJournalClear(void);

#if TESTING
// Test storage is [home sector, journal A, journal B], allocated from the test heap.
void TradeJournalTestStorage(u8 *storage, s32 failAfterBytes);
// Normal chunks followed by journal A/B, for validating the real boot scan.
void TradeJournalTestFullStorage(u8 *storage, s32 failAfterBytes);
#endif

#endif
