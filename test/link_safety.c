#include "global.h"
#include "link.h"
#include "link_protocol.h"
#include "load_save.h"
#include "malloc.h"
#include "save.h"
#include "union_room.h"
#include "constants/union_room.h"
#include "test/test.h"

TEST("Wireless discovery rejects retired facility activities and empty group slots")
{
    const u32 retired[] = {ACTIVITY_UNUSED_14, ACTIVITY_UNUSED_15, ACTIVITY_UNUSED_28};
    for (u32 i = 0; i < ARRAY_COUNT(retired); i++)
    {
        EXPECT(!IsUnionRoomActivityAcceptable(retired[i], 0xFF));
        EXPECT(!IsUnionRoomActivityAcceptable(retired[i] | IN_UNION_ROOM, 0xFF));
        EXPECT(!IsUnionRoomActivityAcceptable(retired[i], LINK_GROUP_SINGLE_BATTLE));
    }
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_BATTLE_SINGLE, LINK_GROUP_UNUSED_20));
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_TRADE, LINK_GROUP_UNUSED_12));
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_BATTLE_SINGLE, LINK_GROUP_UNUSED_21));
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_TRADE, LINK_GROUP_UNK_14));
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_TRADE, NUM_LINK_GROUP_TYPES));
    EXPECT(!IsUnionRoomActivityAcceptable(ACTIVITY_TRADE, NUM_LINK_GROUP_TYPES + 1));
    EXPECT(IsUnionRoomActivityAcceptable(ACTIVITY_BATTLE_SINGLE, LINK_GROUP_SINGLE_BATTLE));
    EXPECT(IsUnionRoomActivityAcceptable(ACTIVITY_BATTLE_DOUBLE, LINK_GROUP_DOUBLE_BATTLE));
    EXPECT(IsUnionRoomActivityAcceptable(ACTIVITY_BATTLE_MULTI, LINK_GROUP_MULTI_BATTLE));
    EXPECT(IsUnionRoomActivityAcceptable(ACTIVITY_TRADE, LINK_GROUP_TRADE));
    EXPECT(IsUnionRoomActivityAcceptable(ACTIVITY_CHAT | IN_UNION_ROOM, 0xFF));
}

TEST("Link and PC journal CRC optimization preserves the original bitwise format")
{
    EXPECT_EQ(LinkDataHash("123456789", 9), 0xCBF43926);
    EXPECT_EQ(LinkDataHash("", 0), 0);
    u8 bytes[257];
    for (u32 i = 0; i < sizeof(bytes); i++) bytes[i] = i * 71 + 11;
    for (u32 offset = 0; offset < 4; offset++)
        for (u32 size = 0; size <= sizeof(bytes) - offset; size++)
        {
            u32 crc = 0xFFFFFFFF;
            for (u32 i = 0; i < size; i++)
            {
                crc ^= bytes[offset + i];
                for (u32 bit = 0; bit < 8; bit++)
                    crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1));
            }
            EXPECT_EQ(LinkDataHash(bytes + offset, size), ~crc);
        }
}

static void StoreHome(u8 *storage)
{
    struct SaveSector *home = (void *)storage;
    memset(home, 0, sizeof(*home));
    memcpy(home->data, &gSaveData, SECTOR_DATA_SIZE);
    home->id = 0;
    home->counter = gSaveCounter;
    home->signature = SECTOR_SIGNATURE;
    u32 sum = 0;
    for (u32 i = 0; i < SECTOR_DATA_SIZE / 4; i++)
        sum += ((u32 *)home->data)[i];
    home->checksum = (sum >> 16) + sum;
}

static u8 *SetUpTrade(void)
{
    u8 *storage = Alloc(SECTOR_SIZE * 3);
    memset(storage, 0xFF, SECTOR_SIZE * 3);
    TradeJournalTestStorage(storage, -1);
    ResetSaveData();
    ZeroPlayerPartyMons();
    gSaveCounter = 17;
    gSaveData.playerTrainerId[0] = 1;
    memcpy(gSaveData.playerName, COMPOUND_STRING("ALICE"), 6);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 30, 123, OTID_STRUCT_PRESET(1));
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
    gParties[B_TRAINER_PLAYER][0].hp = gParties[B_TRAINER_PLAYER][0].maxHP - 7;
    gParties[B_TRAINER_PLAYER][0].status = STATUS1_POISON;
    gParties[B_TRAINER_PLAYER][0].mail = 2;
    gSaveData.mail[2].itemId = ITEM_ORANGE_MAIL;
    gSaveData.mail[2].words[0] = 123;
    SavePlayerParty();
    StoreHome(storage);
    return storage;
}

static bool32 PrepareTrade(void)
{
    struct TradeJournalOffer offers[2];
    TradeJournalGetOffer(&offers[0], 0);
    offers[1] = offers[0];
    offers[1].trainerId = 2;
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_ABRA, 30, 5678, OTID_STRUCT_PRESET(2));
    CalculateMonStats(&gParties[B_TRAINER_OPPONENT_A][0]);
    struct BoxPokemon mon;
    SaveMonToBox(&mon, &gParties[B_TRAINER_OPPONENT_A][0]);
    offers[1].monHash = LinkDataHash(&mon, sizeof(mon));
    memset(offers[1].name, 0, sizeof(offers[1].name));
    memcpy(offers[1].name, COMPOUND_STRING("BOB"), 4);
    return TradeJournalPrepare(offers, 0);
}

static struct TradeJournalOffer PeerOffer(u8 phase)
{
    struct TradeJournalMetadata meta;
    TradeJournalRead(&meta);
    struct TradeJournalOffer peer = {0};
    peer.buildId = LinkProtocolBuildId();
    peer.trainerId = meta.peerTrainerId;
    memcpy(peer.name, meta.peerName, sizeof(peer.name));
    peer.sequence = meta.peerSequence;
    memcpy(peer.transaction, meta.transaction, sizeof(peer.transaction));
    peer.monHash = meta.offeredHash;
    peer.phase = phase;
    return peer;
}

static void TearDownTrade(u8 *storage)
{
    TradeJournalTestStorage(NULL, -1);
    Free(storage);
    ResetSaveData();
}

TEST("Link protocol fingerprints reject vanilla, mismatched and unterminated headers")
{
    struct LinkPlayerBlock block = {0};
    EXPECT(!LinkProtocolCheck(&block));
    LinkProtocolStamp(&block);
    EXPECT(LinkProtocolCheck(&block));
    block.magic1[0] ^= 1;
    EXPECT(!LinkProtocolCheck(&block));
    LinkProtocolStamp(&block);
    block.magic2[15] = 'x';
    EXPECT(!LinkProtocolCheck(&block));
    LinkProtocolStamp(&block);
    block.linkPlayer.lp_field_2 ^= 1;
    EXPECT(!LinkProtocolCheck(&block));
}

TEST("Link protocol uses the standard CRC32 check vector")
{
    EXPECT_EQ(LinkDataHash("123456789", 9), 0xCBF43926u);
}

TEST("Link battles restore damage, status, party order, held items, bag and mail without saving")
{
    u8 *storage = SetUpTrade();
    gSaveData.bag.items[0].itemId = ITEM_POTION;
    gSaveData.bag.items[0].quantity = 8;
    u32 hp = gParties[B_TRAINER_PLAYER][0].hp;
    SnapshotLinkBattleState();
    EXPECT(LinkBattleStateActive());
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_ERROR);
    ZeroPlayerPartyMons();
    gSaveData.bag.items[0].quantity = 1;
    gSaveData.mail[2].words[0] = 0;
    SnapshotLinkBattleState(); // Nested entry must not replace the original snapshot.
    RestoreLinkBattleState();
    EXPECT(!LinkBattleStateActive());
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].hp, hp);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].status, STATUS1_POISON);
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].mail, 2);
    EXPECT_EQ(gSaveData.bag.items[0].quantity, 8);
    EXPECT_EQ(gSaveData.mail[2].words[0], 123);
    gParties[B_TRAINER_PLAYER][0].hp--;
    RestoreLinkBattleState(); // An unrelated later error cannot restore a stale snapshot.
    EXPECT_EQ(gParties[B_TRAINER_PLAYER][0].hp, hp - 1);
    TearDownTrade(storage);
}

TEST("Trade journal keeps uncertain trades locked across RAM resets")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    EXPECT(TradeJournalBlocksSaving());
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_ERROR);
    ResetSaveData();
    EXPECT(TradeJournalNeedsPeer());
    EXPECT(TradeJournalRestoreHome());
    struct SaveData *home = (void *)storage;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_PIKACHU);
    TearDownTrade(storage);
}

TEST("Trade journal torn READY writes keep the last complete PREPARED record")
{
    u32 cut;
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = 64; }
    PARAMETRIZE { cut = SECTOR_SIGNATURE_OFFSET; }
    PARAMETRIZE { cut = SECTOR_SIZE - 1; }
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    TradeJournalTestStorage(storage, cut);
    EXPECT(!TradeJournalReady());
    TradeJournalTestStorage(storage, -1);
    struct TradeJournalMetadata meta;
    EXPECT(TradeJournalRead(&meta));
    EXPECT_EQ(meta.phase, TRADE_JOURNAL_PREPARED);
    EXPECT(TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

TEST("Trade journal CRC rejects a corrupt newer record without losing the prior phase")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade()); // A
    EXPECT(TradeJournalReady()); // B
    storage[SECTOR_SIZE * 2 + 100] ^= 1;
    struct TradeJournalMetadata meta;
    EXPECT(TradeJournalRead(&meta));
    EXPECT_EQ(meta.phase, TRADE_JOURNAL_PREPARED);
    TearDownTrade(storage);
}

TEST("Trade journal PREPARED never commits merely because its peer is absent")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    EXPECT(!TradeJournalCommit());
    EXPECT(!TradeJournalFinish());
    EXPECT(!TradeJournalLoadCandidate());
    EXPECT(TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

TEST("Trade journal READY and COMMITTED retain exact evolved Pokemon, mail and dex after power loss")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    gParties[B_TRAINER_PLAYER][0].mail = 1;
    gSaveData.mail[1].words[0] = 456;
    gSaveData.dexCaught[3] = 0x88;
    EXPECT(TradeJournalReady());
    ResetSaveData();
    EXPECT(TradeJournalLoadCandidate());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ALAKAZAM);
    EXPECT_EQ(gSaveData.mail[1].words[0], 456);
    EXPECT_EQ(gSaveData.dexCaught[3], 0x88);
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalBlocksSaving());
    memset(storage, 0xFF, SECTOR_SIZE); // Power lost while replacing the home sector.
    EXPECT(TradeJournalRestoreHome());
    struct SaveData *home = (void *)storage;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_ALAKAZAM);
    EXPECT_EQ(home->playerPartyMail[0], 1);
    EXPECT_EQ(home->mail[1].words[0], 456);
    EXPECT_EQ(home->dexCaught[3], 0x88);
    EXPECT(TradeJournalFinish());
    EXPECT(!TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

TEST("Trade journal refuses the wrong partner, transaction, sequence or offered Pokemon")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    struct TradeJournalOffer peer = PeerOffer(TRADE_JOURNAL_READY);
    EXPECT(TradeJournalSameTransaction(&peer));
    peer.trainerId++;
    EXPECT(!TradeJournalReconcile(&peer));
    peer = PeerOffer(TRADE_JOURNAL_READY);
    peer.transaction[0]++;
    EXPECT(!TradeJournalReconcile(&peer));
    peer = PeerOffer(TRADE_JOURNAL_READY);
    peer.sequence++;
    EXPECT(!TradeJournalReconcile(&peer));
    peer = PeerOffer(TRADE_JOURNAL_READY);
    peer.monHash++;
    EXPECT(!TradeJournalReconcile(&peer));
    EXPECT(TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

TEST("Trade journal cancels PREPARED with peer READY but refuses impossible peer COMMITTED")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    struct TradeJournalOffer peer = PeerOffer(TRADE_JOURNAL_COMMITTED);
    EXPECT(!TradeJournalReconcile(&peer));
    peer.phase = TRADE_JOURNAL_READY;
    EXPECT(TradeJournalReconcile(&peer));
    struct TradeJournalMetadata meta;
    EXPECT(TradeJournalRead(&meta));
    EXPECT_EQ(meta.phase, TRADE_JOURNAL_ABORTED);
    EXPECT(!TradeJournalNeedsPeer());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(gSaveData.mail[2].words[0], 123);
    TearDownTrade(storage);
}

TEST("Trade journal reconciles both READY and one-sided COMMITTED without rolling back")
{
    u8 peerPhase;
    PARAMETRIZE { peerPhase = TRADE_JOURNAL_READY; }
    PARAMETRIZE { peerPhase = TRADE_JOURNAL_COMMITTED; }
    PARAMETRIZE { peerPhase = TRADE_JOURNAL_DONE; }
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    struct TradeJournalOffer peer = PeerOffer(peerPhase);
    EXPECT(TradeJournalReconcile(&peer));
    EXPECT(TradeJournalNeedsPeer());
    peer.phase = TRADE_JOURNAL_COMMITTED;
    EXPECT(TradeJournalReconcile(&peer));
    EXPECT(!TradeJournalNeedsPeer());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ALAKAZAM);
    struct SaveData *home = (void *)storage;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_ALAKAZAM);
    TearDownTrade(storage);
}

TEST("Trade journal completed receipts retain the original offered hash for a recovering peer")
{
    u8 *storage = SetUpTrade();
    struct TradeJournalOffer original, receipt;
    EXPECT(TradeJournalGetOffer(&original, 0));
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalFinish());
    EXPECT(TradeJournalGetOffer(&receipt, PARTY_SIZE));
    EXPECT_EQ(receipt.monHash, original.monHash);
    EXPECT_EQ(receipt.phase, TRADE_JOURNAL_DONE);
    TearDownTrade(storage);
}

TEST("Trade journal completed transactions never overwrite a newer ordinary save")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalFinish());
    gSaveData.playerParty[0].data.species = SPECIES_MEW;
    gSaveCounter++;
    StoreHome(storage);
    EXPECT(TradeJournalRestoreHome());
    struct SaveData *home = (void *)storage;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_MEW);
    TearDownTrade(storage);
}

TEST("Trade journal only uses reserved sectors outside the save image and Hall of Fame")
{
    EXPECT_LT(SECTOR_ID_HOF_2, SECTOR_ID_TRADE_A);
    EXPECT_LT(NUM_SECTORS_PER_SLOT, SECTOR_ID_TRADE_A);
    EXPECT_EQ(SECTOR_ID_TRADE_B + 1, SECTORS_COUNT);
    EXPECT_LE(offsetof(struct SaveData, linkTrade) + sizeof(struct TradeJournalMetadata), SECTOR_DATA_SIZE);
    EXPECT_LE(sizeof(struct TradeJournalMetadata), 128);
}

TEST("Trade journal validates the actual swapped Pokemon against both durable offers")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    EXPECT(TradeJournalValidateSwap(0, 0));
    EXPECT(!TradeJournalValidateSwap(1, 0));
    EXPECT(!TradeJournalValidateSwap(0, PARTY_SIZE));
    gParties[B_TRAINER_OPPONENT_A][0].box.personality++;
    EXPECT(!TradeJournalValidateSwap(0, 0));
    EXPECT(TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

TEST("Trade journal direct wireless trades validate a selected peer Pokemon buffered at slot zero")
{
    u8 *storage = SetUpTrade();
    struct TradeJournalOffer offers[2];
    EXPECT(TradeJournalGetOffer(&offers[0], 0));
    offers[1] = offers[0];
    offers[1].trainerId = 2;
    offers[1].partySlot = 4; // The partner's fifth party slot is sent alone.
    offers[1].monHash = offers[0].monHash;
    gParties[B_TRAINER_OPPONENT_A][0] = gParties[B_TRAINER_PLAYER][0];
    ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][4]);
    EXPECT(TradeJournalPrepare(offers, 0));
    EXPECT(TradeJournalValidateSwap(0, 0));
    EXPECT(!TradeJournalValidateSwap(0, 4));
    TearDownTrade(storage);
}

TEST("Trade journal recovery can cancel a peer whose first prepare write failed")
{
    u32 peerSequence;
    PARAMETRIZE { peerSequence = 0; }
    // The clean machine may have saved/traded again after its failed prepare.
    // Our PREPARED record proves this particular trade never reached commit.
    PARAMETRIZE { peerSequence = 3; }
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    struct TradeJournalOffer peer = PeerOffer(TRADE_JOURNAL_EMPTY);
    peer.sequence = peerSequence;
    memset(peer.transaction, 0, sizeof(peer.transaction));
    EXPECT(TradeJournalReconcile(&peer));
    EXPECT(!TradeJournalNeedsPeer());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    TearDownTrade(storage);
}

TEST("Trade journal torn home-sector writes recover from the intact committed image")
{
    u32 cut;
    PARAMETRIZE { cut = 0; }
    PARAMETRIZE { cut = 2000; }
    PARAMETRIZE { cut = SECTOR_SIZE - 1; }
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    TradeJournalTestStorage(storage, cut);
    EXPECT(!TradeJournalRestoreHome());
    TradeJournalTestStorage(storage, -1);
    EXPECT(TradeJournalRestoreHome());
    struct SaveData *home = (void *)storage;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_ALAKAZAM);
    EXPECT(TradeJournalNeedsPeer());
    TearDownTrade(storage);
}

// Two independent machines, each with its own flash and volatile party. Keeping
// only the relevant RAM chunk makes this fit the normal GBA test heap.
struct JournalPeer
{
    u8 flash[SECTOR_SIZE * 3];
    u8 ram[SECTOR_DATA_SIZE];
    struct Pokemon party[PARTY_SIZE];
};

static void SwitchPeer(struct JournalPeer *peer)
{
    TradeJournalTestStorage(peer->flash, -1);
    memcpy(&gSaveData, peer->ram, sizeof(peer->ram));
    memcpy(gParties[B_TRAINER_PLAYER], peer->party, sizeof(peer->party));
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    gSaveCounter = 17;
}

static void RememberPeer(struct JournalPeer *peer)
{
    memcpy(peer->ram, &gSaveData, sizeof(peer->ram));
    memcpy(peer->party, gParties[B_TRAINER_PLAYER], sizeof(peer->party));
}

TEST("Trade journal two-machine restart recovery agrees after asymmetric interruptions")
{
    u8 phaseA, phaseB;
    bool32 exchanged;
    PARAMETRIZE { phaseA = TRADE_JOURNAL_PREPARED; phaseB = TRADE_JOURNAL_PREPARED; exchanged = FALSE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_PREPARED; phaseB = TRADE_JOURNAL_READY; exchanged = FALSE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_READY; phaseB = TRADE_JOURNAL_PREPARED; exchanged = FALSE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_READY; phaseB = TRADE_JOURNAL_READY; exchanged = TRUE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_READY; phaseB = TRADE_JOURNAL_COMMITTED; exchanged = TRUE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_COMMITTED; phaseB = TRADE_JOURNAL_READY; exchanged = TRUE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_COMMITTED; phaseB = TRADE_JOURNAL_COMMITTED; exchanged = TRUE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_DONE; phaseB = TRADE_JOURNAL_COMMITTED; exchanged = TRUE; }
    PARAMETRIZE { phaseA = TRADE_JOURNAL_COMMITTED; phaseB = TRADE_JOURNAL_DONE; exchanged = TRUE; }
    struct JournalPeer *peers = AllocZeroed(sizeof(*peers) * 2);
    struct TradeJournalOffer offers[2];
    u8 phases[2] = {phaseA, phaseB};
    for (u32 i = 0; i < 2; i++)
    {
        ResetSaveData();
        ZeroPlayerPartyMons();
        gSaveCounter = 17;
        memset(peers[i].flash, 0xFF, sizeof(peers[i].flash));
        TradeJournalTestStorage(peers[i].flash, -1);
        gSaveData.playerTrainerId[0] = i + 1;
        memcpy(gSaveData.playerName, i == 0 ? COMPOUND_STRING("ALICE") : COMPOUND_STRING("BOB"), i == 0 ? 6 : 4);
        CreateMon(&gParties[B_TRAINER_PLAYER][0], i == 0 ? SPECIES_PIKACHU : SPECIES_ABRA, 30, i + 1, OTID_STRUCT_PRESET(i + 1));
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
        gPartiesCount[B_TRAINER_PLAYER] = 1;
        SavePlayerParty();
        StoreHome(peers[i].flash);
        EXPECT(TradeJournalGetOffer(&offers[i], 0));
        RememberPeer(&peers[i]);
    }
    for (u32 i = 0; i < 2; i++)
    {
        SwitchPeer(&peers[i]);
        EXPECT(TradeJournalPrepare(offers, i));
        if (phases[i] >= TRADE_JOURNAL_READY)
        {
            CreateMon(&gParties[B_TRAINER_PLAYER][0], i == 0 ? SPECIES_ALAKAZAM : SPECIES_PIKACHU, 30, (i ^ 1) + 1, OTID_STRUCT_PRESET((i ^ 1) + 1));
            CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
            EXPECT(TradeJournalReady());
        }
        if (phases[i] >= TRADE_JOURNAL_COMMITTED)
            EXPECT(TradeJournalCommit());
        if (phases[i] == TRADE_JOURNAL_DONE)
            EXPECT(TradeJournalFinish());
        // Reboot: only the durable home sector and journal survive.
        EXPECT(TradeJournalRestoreHome());
        memcpy(&gSaveData, peers[i].flash, SECTOR_DATA_SIZE);
        LoadPlayerParty();
        RememberPeer(&peers[i]);
    }
    for (u32 round = 0; round < 3; round++)
    {
        for (u32 i = 0; i < 2; i++)
        {
            SwitchPeer(&peers[i]);
            EXPECT(TradeJournalGetOffer(&offers[i], PARTY_SIZE));
        }
        for (u32 i = 0; i < 2; i++)
        {
            SwitchPeer(&peers[i]);
            EXPECT(TradeJournalReconcile(&offers[i ^ 1]));
            RememberPeer(&peers[i]);
        }
    }
    for (u32 i = 0; i < 2; i++)
    {
        SwitchPeer(&peers[i]);
        EXPECT(!TradeJournalNeedsPeer());
        enum Species expected = exchanged ? (i == 0 ? SPECIES_ALAKAZAM : SPECIES_PIKACHU)
                                         : (i == 0 ? SPECIES_PIKACHU : SPECIES_ABRA);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), expected);
        struct SaveData *home = (void *)peers[i].flash;
        EXPECT_EQ(home->playerParty[0].data.species, expected);
    }
    TradeJournalTestStorage(NULL, -1);
    Free(peers);
    ResetSaveData();
}

TEST("Trade journal boot repair verifies every fixed checkpoint chunk before replacing home")
{
    u8 corruptField;
    PARAMETRIZE { corruptField = 0; }
    PARAMETRIZE { corruptField = 1; }
    PARAMETRIZE { corruptField = 2; }
    PARAMETRIZE { corruptField = 3; }
    u8 *small = SetUpTrade();
    Free(small);
    struct SaveSector *flash = AllocZeroed((NUM_SECTORS_PER_SLOT + 2) * SECTOR_SIZE);
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        u32 id = i;
        u32 offset = id * SECTOR_DATA_SIZE;
        u32 size = offset < sizeof(gSaveData) ? min(sizeof(gSaveData) - offset, SECTOR_DATA_SIZE) : 0;
        memcpy(flash[i].data, (u8 *)&gSaveData + min(offset, sizeof(gSaveData)), size);
        flash[i].id = id;
        flash[i].counter = gSaveCounter;
        flash[i].signature = SECTOR_SIGNATURE;
        u32 sum = 0;
        for (u32 j = 0; j < size / 4; j++)
            sum += ((u32 *)flash[i].data)[j];
        flash[i].checksum = (sum >> 16) + sum;
    }
    memset(&flash[NUM_SECTORS_PER_SLOT], 0xFF, SECTOR_SIZE * 2);
    TradeJournalTestFullStorage((u8 *)flash, -1);
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    // Sector 1 is the other core chunk; sector zero must remain untouched.
    u32 counter = flash[1].counter, signature = flash[1].signature;
    u16 id = flash[1].id, checksum = flash[1].checksum;
    if (corruptField == 0) flash[1].counter++;
    if (corruptField == 1) flash[1].signature++;
    if (corruptField == 2) flash[1].id++;
    if (corruptField == 3) flash[1].checksum++;
    EXPECT(!TradeJournalRestoreHome());
    struct SaveData *home = (void *)flash[SECTOR_ID_HOME].data;
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_PIKACHU);
    flash[1].counter = counter;
    flash[1].signature = signature;
    flash[1].id = id;
    flash[1].checksum = checksum;
    EXPECT(TradeJournalRestoreHome());
    EXPECT_EQ(home->playerParty[0].data.species, SPECIES_ALAKAZAM);
    TradeJournalTestStorage(NULL, -1);
    Free(flash);
    ResetSaveData();
}

TEST("Trade journal newer partner sequences resolve a lost final acknowledgement without replay")
{
    u8 peerPhase;
    PARAMETRIZE { peerPhase = TRADE_JOURNAL_DONE; }
    PARAMETRIZE { peerPhase = TRADE_JOURNAL_PREPARED; }
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ALAKAZAM, 45, 5678, OTID_STRUCT_PRESET(2));
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    struct TradeJournalOffer peer = PeerOffer(peerPhase);
    peer.sequence += 3;
    peer.transaction[0]++;
    peer.peerTrainerId = 99; // Its newer trade may be with another person.
    EXPECT(TradeJournalReconcile(&peer));
    EXPECT(!TradeJournalNeedsPeer());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ALAKAZAM);
    TearDownTrade(storage);
}

TEST("Trade journal serving an older committed peer does not unlock a newer uncertain trade")
{
    u8 *storage = SetUpTrade();
    EXPECT(PrepareTrade());
    EXPECT(TradeJournalReady());
    EXPECT(TradeJournalCommit());
    EXPECT(TradeJournalFinish());
    EXPECT(PrepareTrade());
    struct TradeJournalOffer older = PeerOffer(TRADE_JOURNAL_COMMITTED);
    older.peerTrainerId = READ_OTID_FROM_SAVE;
    memcpy(older.peerName, gSaveData.playerName, sizeof(older.peerName));
    older.peerSequence = 1;
    older.sequence = 1;
    older.transaction[0]++;
    EXPECT(TradeJournalReconcile(&older));
    EXPECT(TradeJournalNeedsPeer());
    struct TradeJournalMetadata meta;
    EXPECT(TradeJournalRead(&meta));
    EXPECT_EQ(meta.sequence, 2);
    EXPECT_EQ(meta.phase, TRADE_JOURNAL_PREPARED);
    TearDownTrade(storage);
}
