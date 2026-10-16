#ifndef GUARD_LINK_PROTOCOL_H
#define GUARD_LINK_PROTOCOL_H

struct LinkPlayerBlock;
u32 LinkProtocolBuildId(void);
u32 LinkDataHash(const void *data, u32 size);
void LinkProtocolStamp(struct LinkPlayerBlock *block);
bool32 LinkProtocolCheck(const struct LinkPlayerBlock *block);
bool32 LinkProtocolPlayersCompatible(void);
void LinkProtocolStampMagic(void *first, void *last);
bool32 LinkProtocolCheckMagic(const void *first, const void *last);
enum LinkProtocolError { LINK_PROTOCOL_DEFAULT, LINK_PROTOCOL_WRONG_BUILD, LINK_PROTOCOL_TRADE_ERROR, LINK_PROTOCOL_RECOVERED, LINK_PROTOCOL_PC_ERROR };
void LinkProtocolSetError(enum LinkProtocolError error);
const u8 *LinkProtocolErrorMessage(void);

#endif
