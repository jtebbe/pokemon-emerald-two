#include "global.h"
#include "save.h"
#include "load_save.h"
#include "link_protocol.h"
#include "pc_move_journal.h"
#include "gba/flash_internal.h"

// Two copy-on-write images of logical sector zero, NOT two complete saves.
// READY contains the exact post-evolution party/mail/dex image. It is never
// applied or discarded without exchanging durable state with the same peer.
#define JOURNAL_SIGNATURE (SECTOR_SIGNATURE ^ 0x544A4E31u)
#define META_OFFSET offsetof(struct SaveData, linkTrade)
STATIC_ASSERT(META_OFFSET + sizeof(struct TradeJournalMetadata) <= SECTOR_DATA_SIZE, TradeFitsHomeSector);

#if TESTING
static u8 *sTestStorage;
static s32 sFailAfterBytes;
static bool8 sTestFullStorage;
void TradeJournalTestStorage(u8 *storage, s32 failAfterBytes)
{
    sTestStorage = storage;
    sFailAfterBytes = failAfterBytes;
    sTestFullStorage = FALSE;
}

void TradeJournalTestFullStorage(u8 *storage, s32 failAfterBytes)
{
    TradeJournalTestStorage(storage, failAfterBytes);
    sTestFullStorage = TRUE;
}

static u32 TestSectorIndex(u32 sector)
{
    if (sTestFullStorage)
        return sector == SECTOR_ID_TRADE_A ? NUM_SECTORS_PER_SLOT
             : sector == SECTOR_ID_TRADE_B ? NUM_SECTORS_PER_SLOT + 1 : sector;
    return sector == SECTOR_ID_TRADE_A ? 1 : sector == SECTOR_ID_TRADE_B ? 2 : 0;
}
#endif

static void ReadSector(u32 sector, u32 offset, void *dest, u32 size)
{
#if TESTING
    if (sTestStorage != NULL)
    {
        u32 index = TestSectorIndex(sector);
        memcpy(dest, sTestStorage + index * SECTOR_SIZE + offset, size);
        return;
    }
#endif
    SaveFlashRead(sector, offset, dest, size);
}

static bool32 WriteSector(u32 sector)
{
#if TESTING
    if (sTestStorage == NULL)
#endif
        return SaveFlashWriteBuffer(sector);
#if TESTING
    u8 verify[64];
    const u8 *src = (const u8 *)&gSaveDataBuffer;
#if TESTING
    u8 *testDest = NULL;
    if (sTestStorage != NULL)
    {
        u32 index = TestSectorIndex(sector);
        testDest = sTestStorage + index * SECTOR_SIZE;
        if (sFailAfterBytes == 0)
            return FALSE;
        memset(testDest, 0xFF, SECTOR_SIZE);
    }
    else
#endif
    if (EraseFlashSector(sector))
        return FALSE;

    // The four signature bytes are the last bytes programmed. A torn image is
    // never a valid replacement for the previous durable journal image.
    for (u32 n = 0; n < SECTOR_SIZE; n++)
    {
        u32 offset = n < SECTOR_SIGNATURE_OFFSET ? n
                   : n < SECTOR_SIZE - 4 ? n + 4
                   : SECTOR_SIGNATURE_OFFSET + n - (SECTOR_SIZE - 4);
#if TESTING
        if (testDest != NULL)
        {
            if (sFailAfterBytes == 0)
                return FALSE;
            if (sFailAfterBytes > 0)
                sFailAfterBytes--;
            testDest[offset] &= src[offset];
        }
        else
#endif
        if (ProgramFlashByte(sector, offset, src[offset]))
            return FALSE;
    }
    for (u32 offset = 0; offset < SECTOR_SIZE; offset += sizeof(verify))
    {
        ReadSector(sector, offset, verify, sizeof(verify));
        if (memcmp(verify, src + offset, sizeof(verify)) != 0)
            return FALSE;
    }
    return TRUE;
#endif
}

static struct TradeJournalMetadata *BufferMetadata(void)
{
    return (void *)(gSaveDataBuffer.data + META_OFFSET);
}

static bool32 ValidJournal(void)
{
    const struct TradeJournalMetadata *meta = BufferMetadata();
    u32 crc = LinkDataHash(gSaveDataBuffer.data, SECTOR_DATA_SIZE)
            ^ LinkDataHash(&gSaveDataBuffer.counter, sizeof(gSaveDataBuffer.counter));
    return gSaveDataBuffer.signature == JOURNAL_SIGNATURE
        && gSaveDataBuffer.id == (u16)crc && gSaveDataBuffer.checksum == (crc >> 16)
        && meta->reserved == 0
        && meta->phase >= TRADE_JOURNAL_PREPARED && meta->phase <= TRADE_JOURNAL_ABORTED;
}

// Leaves the newest valid image in the shared save scratch buffer.
static s32 ReadNewest(void)
{
#if TESTING
    if (!SaveFlashAvailable() && sTestStorage == NULL)
#else
    if (!SaveFlashAvailable())
#endif
        return -1;
    u32 generation = 0;
    s32 newest = -1;
    for (u32 sector = SECTOR_ID_TRADE_A; sector <= SECTOR_ID_TRADE_B; sector++)
    {
        ReadSector(sector, 0, &gSaveDataBuffer, SECTOR_SIZE);
        if (ValidJournal() && (newest < 0 || (s32)(gSaveDataBuffer.counter - generation) > 0))
        {
            generation = gSaveDataBuffer.counter;
            newest = sector;
        }
    }
    if (newest >= 0)
        ReadSector(newest, 0, &gSaveDataBuffer, SECTOR_SIZE);
    return newest;
}

static bool32 ReceiptSupersedes(const struct TradeJournalMetadata *receipt, const struct TradeJournalMetadata *journal)
{
    if (receipt->phase != TRADE_JOURNAL_DONE && receipt->phase != TRADE_JOURNAL_ABORTED)
        return FALSE;
    s32 sequenceDifference = receipt->sequence - journal->sequence;
    return sequenceDifference > 0
        || (sequenceDifference == 0 && receipt->buildId == journal->buildId
            && receipt->checkpointCounter == journal->checkpointCounter
            && receipt->peerTrainerId == journal->peerTrainerId
            && memcmp(receipt->transaction, journal->transaction, sizeof(receipt->transaction)) == 0);
}

bool32 TradeJournalRead(struct TradeJournalMetadata *metadata)
{
    s32 newest = ReadNewest();
    struct TradeJournalMetadata journal;
    if (newest >= 0)
        journal = *BufferMetadata();
#if TESTING
    if (!SaveFlashAvailable() && sTestStorage == NULL)
#else
    if (!SaveFlashAvailable())
#endif
        return FALSE;
    // The durable receipt wins even if power fails between erasing 30 and 31,
    // leaving an older READY/COMMITTED image in the other shared journal page.
    ReadSector(0, 0, &gSaveDataBuffer, SECTOR_SIZE);
    if (gSaveDataBuffer.signature == SECTOR_SIGNATURE && gSaveDataBuffer.id == 0
        && gSaveDataBuffer.checksum == SaveSectorChecksum(gSaveDataBuffer.data, SECTOR_DATA_SIZE)
        && BufferMetadata()->phase >= TRADE_JOURNAL_PREPARED
        && BufferMetadata()->phase <= TRADE_JOURNAL_ABORTED
        && (newest < 0 || ReceiptSupersedes(BufferMetadata(), &journal)))
    {
        *metadata = *BufferMetadata();
        return TRUE;
    }
    if (newest >= 0)
        *metadata = journal;
    return newest >= 0;
}

bool32 TradeJournalNeedsPeer(void)
{
    struct TradeJournalMetadata meta;
    return TradeJournalRead(&meta)
        && (meta.phase == TRADE_JOURNAL_PREPARED || meta.phase == TRADE_JOURNAL_READY
            || meta.phase == TRADE_JOURNAL_COMMITTED);
}

bool32 TradeJournalBlocksSaving(void)
{
    return TradeJournalNeedsPeer();
}

static bool32 Persist(u8 phase, bool32 capture)
{
#if TESTING
    if (!SaveFlashAvailable() && sTestStorage == NULL)
#else
    if (!SaveFlashAvailable())
#endif
        return FALSE;
    s32 newest = ReadNewest();
    u32 generation = newest < 0 ? 1 : gSaveDataBuffer.counter + 1;
    if (capture)
        memcpy(gSaveDataBuffer.data, &gSaveData, SECTOR_DATA_SIZE);
    BufferMetadata()->phase = phase;
    gSaveDataBuffer.counter = generation;
    gSaveDataBuffer.signature = JOURNAL_SIGNATURE;
    u32 crc = LinkDataHash(gSaveDataBuffer.data, SECTOR_DATA_SIZE)
            ^ LinkDataHash(&gSaveDataBuffer.counter, sizeof(gSaveDataBuffer.counter));
    gSaveDataBuffer.id = crc;
    gSaveDataBuffer.checksum = crc >> 16;
    if (!WriteSector(newest == SECTOR_ID_TRADE_A ? SECTOR_ID_TRADE_B : SECTOR_ID_TRADE_A))
        return FALSE;
    gSaveData.linkTrade = *BufferMetadata();
    return TRUE;
}

bool32 TradeJournalGetOffer(struct TradeJournalOffer *offer, u32 partySlot)
{
    struct TradeJournalMetadata meta;
    memset(offer, 0, sizeof(*offer));
    offer->buildId = LinkProtocolBuildId();
    offer->trainerId = READ_OTID_FROM_SAVE;
    memcpy(offer->name, gSaveData.playerName, sizeof(offer->name));
    if (TradeJournalRead(&meta))
    {
        offer->sequence = meta.sequence;
        offer->phase = meta.phase;
        memcpy(offer->transaction, meta.transaction, sizeof(offer->transaction));
        offer->peerTrainerId = meta.peerTrainerId;
        offer->peerSequence = meta.peerSequence;
        offer->offeredHash = meta.offeredHash;
        memcpy(offer->peerName, meta.peerName, sizeof(offer->peerName));
        offer->peerSlot = meta.peerSlot;
        if (TradeJournalNeedsPeer() || partySlot == PARTY_SIZE)
        {
            if (TradeJournalNeedsPeer() && meta.buildId != offer->buildId)
                return FALSE;
            offer->partySlot = meta.partySlot;
            offer->monHash = LinkDataHash(&meta.outgoing, sizeof(meta.outgoing));
            return TRUE;
        }
    }
    offer->partySlot = partySlot;
    if (partySlot < PARTY_SIZE)
    {
        struct BoxPokemon saved;
        SaveMonToBox(&saved, &gParties[B_TRAINER_PLAYER][partySlot]);
        offer->monHash = LinkDataHash(&saved, sizeof(saved));
    }
    return TRUE;
}

bool32 TradeJournalPrepare(const struct TradeJournalOffer offers[2], u32 localId)
{
    // The protocol checkpoints before PREPARED. A direct or future caller must
    // not claim shared recovery sectors while a durable PC log still owns them.
    if (PCMoveJournalPending() || PCMoveJournalSession() || PCMoveJournalBusy()) return FALSE;
    if (localId > 1 || TradeJournalNeedsPeer())
        return FALSE;
    const struct TradeJournalOffer *local = &offers[localId], *peer = &offers[localId ^ 1];
    if (local->partySlot >= gPartiesCount[B_TRAINER_PLAYER]
        || peer->partySlot >= PARTY_SIZE || local->buildId != LinkProtocolBuildId()
        || peer->buildId != local->buildId || local->trainerId != READ_OTID_FROM_SAVE)
        return FALSE;
    struct BoxPokemon outgoing;
    SaveMonToBox(&outgoing, &gParties[B_TRAINER_PLAYER][local->partySlot]);
    if (LinkDataHash(&outgoing, sizeof(outgoing)) != local->monHash)
        return FALSE;
    struct TradeJournalMetadata *meta = &gSaveData.linkTrade;
    memset(meta, 0, sizeof(*meta));
    meta->sequence = local->sequence + 1;
    meta->peerSequence = peer->sequence + 1;
    // Offers are ordered by link ID, so both sides calculate the same token.
    meta->transaction[0] = LinkDataHash(offers, sizeof(*offers) * 2);
    meta->transaction[1] = LinkDataHash(&offers[1], sizeof(*offers)) ^ local->buildId;
    meta->peerTrainerId = peer->trainerId;
    memcpy(meta->peerName, peer->name, sizeof(meta->peerName));
    meta->buildId = local->buildId;
    meta->partySlot = local->partySlot;
    meta->peerSlot = peer->partySlot;
    meta->outgoing = outgoing;
    meta->offeredHash = peer->monHash;
    meta->checkpointCounter = gSaveCounter;
    SavePlayerParty();
    return Persist(TRADE_JOURNAL_PREPARED, TRUE);
}

static bool32 Transition(u8 from, u8 to, bool32 capture)
{
    struct TradeJournalMetadata meta;
    if (!TradeJournalRead(&meta) || meta.phase != from || meta.buildId != LinkProtocolBuildId())
        return FALSE;
    gSaveData.linkTrade = meta;
    return Persist(to, capture);
}

bool32 TradeJournalReady(void)
{
    SavePlayerParty();
    return Transition(TRADE_JOURNAL_PREPARED, TRADE_JOURNAL_READY, TRUE);
}

bool32 TradeJournalCommit(void)
{
    return Transition(TRADE_JOURNAL_READY, TRADE_JOURNAL_COMMITTED, FALSE);
}

bool32 TradeJournalLoadCandidate(void)
{
    if (ReadNewest() < 0 || BufferMetadata()->buildId != LinkProtocolBuildId()
        || (BufferMetadata()->phase != TRADE_JOURNAL_READY
            && BufferMetadata()->phase != TRADE_JOURNAL_COMMITTED
            && BufferMetadata()->phase != TRADE_JOURNAL_DONE))
        return FALSE;
    memcpy(&gSaveData, gSaveDataBuffer.data, SECTOR_DATA_SIZE);
    LoadPlayerParty();
    return TRUE;
}

static u16 SectorChecksum(u32 size)
{
    u32 sum = 0;
    for (u32 i = 0; i < size / 4; i++)
        sum += ((u32 *)gSaveDataBuffer.data)[i];
    return (sum >> 16) + sum;
}

static u16 HomeChecksum(void)
{
    return SectorChecksum(SECTOR_DATA_SIZE);
}

bool32 TradeJournalRestoreHome(void)
{
    s32 newest = ReadNewest();
    if (newest < 0)
        return TRUE;
    struct TradeJournalMetadata meta = *BufferMetadata();
    if (meta.phase != TRADE_JOURNAL_COMMITTED && meta.phase != TRADE_JOURNAL_DONE
        && meta.phase != TRADE_JOURNAL_ABORTED)
        return TRUE;
    ReadSector(SECTOR_ID_HOME, 0, &gSaveDataBuffer, SECTOR_SIZE);
    if (gSaveDataBuffer.signature == SECTOR_SIGNATURE && gSaveDataBuffer.id == 0
        && gSaveDataBuffer.checksum == HomeChecksum())
    {
        if (ReceiptSupersedes(BufferMetadata(), &meta))
            return TRUE;
        // A later ordinary save supersedes a completed journal. Never replay it.
        if ((s32)(gSaveDataBuffer.counter - meta.checkpointCounter) > 0)
            return TRUE;
        if (gSaveDataBuffer.counter == meta.checkpointCounter
            && memcmp(BufferMetadata(), &meta, sizeof(meta)) == 0)
            return TRUE;
    }
    // Do not combine a journal with an unrelated or partly overwritten save.
    if (meta.buildId != LinkProtocolBuildId())
        return FALSE;
    // The other core page must match the checkpoint generation. PC pages are
    // independently versioned; validate their identities and contents instead.
    for (u32 sector = 0; sector < NUM_SECTORS_PER_SLOT; sector++)
    {
        if (sector == SECTOR_ID_HOME)
            continue;
        ReadSector(sector, 0, &gSaveDataBuffer, SECTOR_SIZE);
#if TESTING
        if (sTestStorage != NULL && !sTestFullStorage)
            break; // The unit-test flash contains only one home sector.
#endif
        if (gSaveDataBuffer.signature != SECTOR_SIGNATURE
            || (gSaveDataBuffer.id < NUM_CORE_SAVE_SECTORS && gSaveDataBuffer.counter != meta.checkpointCounter)
            || gSaveDataBuffer.id != sector)
            return FALSE;
        u32 size = SaveSectorDataSize(gSaveDataBuffer.id);
        if (gSaveDataBuffer.checksum != SectorChecksum(size))
            return FALSE;
    }
    ReadSector(newest, 0, &gSaveDataBuffer, SECTOR_SIZE);
    gSaveDataBuffer.id = 0;
    gSaveDataBuffer.checksum = HomeChecksum();
    gSaveDataBuffer.signature = SECTOR_SIGNATURE;
    gSaveDataBuffer.counter = meta.checkpointCounter;
    return WriteSector(SECTOR_ID_HOME);
}

bool32 TradeJournalFinish(void)
{
    return Transition(TRADE_JOURNAL_COMMITTED, TRADE_JOURNAL_DONE, FALSE)
        && TradeJournalRestoreHome();
}

bool32 TradeJournalAbort(void)
{
    struct TradeJournalMetadata meta;
    if (!TradeJournalRead(&meta) || meta.buildId != LinkProtocolBuildId()
        || (meta.phase != TRADE_JOURNAL_PREPARED && meta.phase != TRADE_JOURNAL_READY))
        return FALSE;
    ReadSector(SECTOR_ID_HOME, 0, &gSaveDataBuffer, SECTOR_SIZE);
    if (gSaveDataBuffer.signature != SECTOR_SIGNATURE || gSaveDataBuffer.id != 0
        || gSaveDataBuffer.checksum != HomeChecksum() || gSaveDataBuffer.counter != meta.checkpointCounter)
        return FALSE;
    memcpy(&gSaveData, gSaveDataBuffer.data, SECTOR_DATA_SIZE);
    gSaveData.linkTrade = meta;
    LoadPlayerParty();
    return Persist(TRADE_JOURNAL_ABORTED, TRUE) && TradeJournalRestoreHome();
}

bool32 TradeJournalSameTransaction(const struct TradeJournalOffer *peer)
{
    struct TradeJournalMetadata meta;
    return TradeJournalRead(&meta) && peer->buildId == meta.buildId
        && peer->trainerId == meta.peerTrainerId
        && memcmp(peer->name, meta.peerName, sizeof(peer->name)) == 0
        && peer->sequence == meta.peerSequence
        && memcmp(peer->transaction, meta.transaction, sizeof(meta.transaction)) == 0
        && peer->monHash == meta.offeredHash;
}

bool32 TradeJournalValidateSwap(u32 partySlot, u32 peerSlot)
{
    struct TradeJournalMetadata meta;
    struct BoxPokemon mon;
    if (!TradeJournalRead(&meta) || meta.phase != TRADE_JOURNAL_PREPARED
        || meta.buildId != LinkProtocolBuildId() || partySlot != meta.partySlot
        || partySlot >= PARTY_SIZE || peerSlot >= PARTY_SIZE)
        return FALSE;
    SaveMonToBox(&mon, &gParties[B_TRAINER_PLAYER][partySlot]);
    if (memcmp(&mon, &meta.outgoing, sizeof(mon)) != 0)
        return FALSE;
    SaveMonToBox(&mon, &gParties[B_TRAINER_OPPONENT_A][peerSlot]);
    // Direct Union Room trades buffer the selected peer mon at index zero,
    // regardless of its original party slot. Bind its content, not that index.
    return LinkDataHash(&mon, sizeof(mon)) == meta.offeredHash;
}

void TradeJournalClear(void)
{
#if TESTING
    if (sTestStorage != NULL)
    {
        memset(sTestStorage + TestSectorIndex(SECTOR_ID_TRADE_A) * SECTOR_SIZE, 0xFF, SECTOR_SIZE * 2);
        return;
    }
#endif
    if (!SaveFlashAvailable())
        return;
    SaveFlashErase(SECTOR_ID_TRADE_A);
    SaveFlashErase(SECTOR_ID_TRADE_B);
}

bool32 TradeJournalReconcile(const struct TradeJournalOffer *peer)
{
    struct TradeJournalMetadata meta;
    bool32 ownPending = TradeJournalNeedsPeer();
    bool32 peerPending = peer->phase == TRADE_JOURNAL_PREPARED
        || peer->phase == TRADE_JOURNAL_READY || peer->phase == TRADE_JOURNAL_COMMITTED;
    if (peer->buildId != LinkProtocolBuildId() || peer->phase > TRADE_JOURNAL_ABORTED)
        return FALSE;
    bool32 haveRecord = TradeJournalRead(&meta);
    bool32 same = haveRecord && TradeJournalSameTransaction(peer);
    if (!ownPending && !peerPending)
        return TRUE;
    if (!same)
    {
        // A DONE machine can lose the final network acknowledgement, reboot,
        // and subsequently start another transaction. Its strictly newer local
        // sequence proves it finished the old transaction before reusing the
        // journal. A COMMITTED peer may finish from that proof, not roll back.
        if (haveRecord && meta.phase == TRADE_JOURNAL_COMMITTED
            && peer->phase != TRADE_JOURNAL_EMPTY
            && peer->trainerId == meta.peerTrainerId
            && memcmp(peer->name, meta.peerName, sizeof(peer->name)) == 0
            && (s32)(peer->sequence - meta.peerSequence) > 0)
            return TradeJournalLoadCandidate() && TradeJournalFinish();
        // Serve that older peer without touching our newer transaction, which
        // may still require recovery with a different partner.
        if (haveRecord && peer->phase == TRADE_JOURNAL_COMMITTED
            && peer->peerTrainerId == READ_OTID_FROM_SAVE
            && memcmp(peer->peerName, gSaveData.playerName, sizeof(peer->peerName)) == 0
            && (s32)(meta.sequence - peer->peerSequence) > 0)
            return TRUE;
        // A prepare may fail on one machine before its first durable record.
        // Only a PREPARED peer (never READY/COMMITTED) can safely be cancelled.
        if (ownPending)
        {
            if (meta.phase != TRADE_JOURNAL_PREPARED || peerPending
                || peer->trainerId != meta.peerTrainerId
                || memcmp(peer->name, meta.peerName, sizeof(peer->name)) != 0
                || (s32)(peer->sequence + 1 - meta.peerSequence) < 0)
                return FALSE;
            return TradeJournalAbort();
        }
        return peer->phase == TRADE_JOURNAL_PREPARED
            && peer->peerTrainerId == READ_OTID_FROM_SAVE
            && memcmp(peer->peerName, gSaveData.playerName, sizeof(peer->peerName)) == 0
            && (s32)((haveRecord ? meta.sequence : 0) + 1 - peer->peerSequence) >= 0;
    }
    if (meta.phase == TRADE_JOURNAL_PREPARED || peer->phase == TRADE_JOURNAL_PREPARED
        || meta.phase == TRADE_JOURNAL_ABORTED || peer->phase == TRADE_JOURNAL_ABORTED)
    {
        if (meta.phase == TRADE_JOURNAL_COMMITTED || meta.phase == TRADE_JOURNAL_DONE
            || peer->phase == TRADE_JOURNAL_COMMITTED || peer->phase == TRADE_JOURNAL_DONE)
            return FALSE;
        return meta.phase == TRADE_JOURNAL_ABORTED || TradeJournalAbort();
    }
    if (meta.phase == TRADE_JOURNAL_READY)
        return TradeJournalLoadCandidate() && TradeJournalCommit();
    if (meta.phase == TRADE_JOURNAL_COMMITTED && peer->phase != TRADE_JOURNAL_READY)
        return TradeJournalLoadCandidate() && TradeJournalFinish();
    return meta.phase == TRADE_JOURNAL_COMMITTED || meta.phase == TRADE_JOURNAL_DONE;
}
