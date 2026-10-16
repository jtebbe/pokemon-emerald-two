#include "global.h"
#include "link.h"
#include "link_protocol.h"
#include "link_build_id.h"

static EWRAM_DATA u8 sProtocolError = LINK_PROTOCOL_DEFAULT;

void LinkProtocolSetError(enum LinkProtocolError error)
{
    sProtocolError = error;
}

const u8 *LinkProtocolErrorMessage(void)
{
    if (sProtocolError == LINK_PROTOCOL_PC_ERROR)
        return COMPOUND_STRING("PC save could not finish.\nRestart the game to recover\nthe last completed operation.");
    if (sProtocolError == LINK_PROTOCOL_WRONG_BUILD)
        return COMPOUND_STRING("Different game builds.\nBoth players must use\nthe exact same ROM.");
    if (sProtocolError == LINK_PROTOCOL_RECOVERED)
        return COMPOUND_STRING("Trade recovery completed.\nRestart the game before\nplaying or trading again.");
    if (TradeJournalNeedsPeer())
        return COMPOUND_STRING("Trade not confirmed.\nReconnect to the same\npartner using the same ROM.\nSaving is locked until then.");
    if (sProtocolError == LINK_PROTOCOL_TRADE_ERROR)
        return COMPOUND_STRING("Trade connection ended.\nRestart the game before\nplaying or trading again.");
    return NULL;
}

u32 LinkProtocolBuildId(void)
{
    return LINK_BUILD_ID;
}

// CRC-32 also covers padding bytes: all protocol records are zero-initialized.
u32 LinkDataHash(const void *data, u32 size)
{
    static const u32 table[16] = {
        0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC,
        0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
        0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C,
        0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C,
    };
    const u8 *bytes = data;
    u32 crc = 0xFFFFFFFF;
    for (u32 i = 0; i < size; i++)
    {
        crc ^= bytes[i];
        crc = (crc >> 4) ^ table[crc & 15];
        crc = (crc >> 4) ^ table[crc & 15];
    }
    return ~crc;
}

void LinkProtocolStamp(struct LinkPlayerBlock *block)
{
    LinkProtocolStampMagic(block->magic1, block->magic2);
    block->linkPlayer.lp_field_2 = (u16)LINK_BUILD_ID;
}

bool32 LinkProtocolCheck(const struct LinkPlayerBlock *block)
{
    return LinkProtocolCheckMagic(block->magic1, block->magic2)
        && block->linkPlayer.lp_field_2 == (u16)LINK_BUILD_ID;
}

void LinkProtocolStampMagic(void *first, void *last)
{
    memcpy(first, LINK_BUILD_MAGIC_A, sizeof(LINK_BUILD_MAGIC_A));
    memcpy(last, LINK_BUILD_MAGIC_B, sizeof(LINK_BUILD_MAGIC_B));
}

bool32 LinkProtocolCheckMagic(const void *first, const void *last)
{
    return memcmp(first, LINK_BUILD_MAGIC_A, sizeof(LINK_BUILD_MAGIC_A)) == 0
        && memcmp(last, LINK_BUILD_MAGIC_B, sizeof(LINK_BUILD_MAGIC_B)) == 0;
}

bool32 LinkProtocolPlayersCompatible(void)
{
    u32 count = GetLinkPlayerCount();
    if (count < 2 || count > MAX_LINK_PLAYERS)
        return FALSE;
    for (u32 i = 0; i < count; i++)
        if (gLinkPlayers[i].lp_field_2 != (u16)LINK_BUILD_ID)
            return FALSE;
    return TRUE;
}
