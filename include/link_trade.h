#ifndef GUARD_LINK_TRADE_H
#define GUARD_LINK_TRADE_H

enum LinkTradeMode { LINK_TRADE_PREFLIGHT, LINK_TRADE_PREPARE, LINK_TRADE_FINALIZE };
enum LinkTradeResult { LINK_TRADE_WAIT, LINK_TRADE_OK, LINK_TRADE_RECOVERED, LINK_TRADE_ERROR };
void LinkTradeStart(enum LinkTradeMode mode, u32 partySlot);
enum LinkTradeResult LinkTradePoll(void);

#endif
