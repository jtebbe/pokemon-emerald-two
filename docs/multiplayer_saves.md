# Multiplayer without a backup save

The normal save has one flash image: resident data in sectors 0–1 and forty
streamed PC boxes in sectors 2–23. Hall of Fame occupies sectors 28–29. Ordinary
saves use sectors 24–27 and 30–31 as a six-page atomic recovery journal.
PC edit journals use 24–27 for compact records and 30 for a checkpoint shadow.
Sealed records persist across PC exits/reopenings; ordinary saves, pre-trade
checkpoints, journal rollover and boot recovery drain them before shared-sector
reuse. This does not reserve additional flash or reduce the forty-box capacity.
During a pending trade, sectors 30–31 instead hold alternating, CRC-protected
trade journal images of **logical sector zero only**. They are not a second
complete save. Ordinary saves and PC access are blocked until that trade resolves.

The rearranged save layout uses a new save signature; older saves are incompatible.

## Battles

Wired, wireless, and multi battles snapshot the original party before healing or
reducing/reordering it, and snapshot bag contents and mail in RAM. Normal return
and link-error handling restore that snapshot. Battles do not automatically save
afterward, and ordinary save entry points refuse to write during a battle session.
Resetting loses unsaved progress, as it does outside multiplayer.

## Build compatibility

The existing link-player handshake includes a generated fingerprint of game code,
configuration, and species/move data, without changing the LinkPlayer block size. Different
fingerprints are rejected before exchanging raw parties. Vanilla games and other
hacks are unsupported. The fingerprint is generated independently of Git state
and normal/test build directories; generated source outputs are excluded in
favor of their source inputs. Graphics-only differences are not fingerprinted.
Wireless player-list relays also carry the complete fingerprint; new participants
are checked before the host relays their data to the other players.

Retired facility and record-mixing activities are neither advertised nor accepted. Their numeric
activity/group slots remain unused, and wireless discovery rejects empty or
out-of-range group entries without dereferencing a missing activity list.

## Trade protocol

A new trade first checks both players' previous journal records. Pending trades
are reconciled **before** party exchange in the trade menu, and checked again at
the animation entry point used by direct wireless trades.

1. Checkpoint the ordinary save before preparing either outgoing Pokémon.
   This drains any pending PC edit log; PREPARED refuses to reserve its shared
   sectors while a PC log/session still owns them.
2. Persist PREPARED records, including partner identity, both monotonic sequence
   numbers, transaction token, and original Pokémon hashes. Exchange proofs.
3. Swap Pokémon and mail in RAM, then run trade evolution and move learning.
4. Persist READY with the exact resulting party, mail, and Pokédex data.
5. Exchange READY proofs before persisting COMMITTED; exchange COMMITTED proofs
   before marking DONE and replacing the home save sector.

Every packet round has a link standby barrier: a fast console cannot send the
next packet while the other console is still reading or writing the previous
round. Journal writes erase the alternate sector, program the signature last,
and verify the result; they reuse the existing save scratch buffer.

On restart, a committed journal can repair an interrupted home-sector write.
It must match the remaining checkpoint's sector IDs, signatures and checksums.
The second core sector must match the checkpoint counter; PC sectors retain
independent generations because unmodified boxes are not rewritten.
A later ordinary save supersedes a completed journal and must never be replayed.
Party, attached/PC mail, Pokédex flags, and transaction metadata are constrained
by a compile-time assertion to fit the home sector. PC-stored fusion partners
and other save sectors are not changed by party-to-party trades.

## Interrupted trades

PREPARED, READY, and COMMITTED require reconnection to the same partner. The game
warns that ordinary saving is locked and any new progress is temporary. Another
partner cannot resolve the transaction or start a new trade. After reconciliation,
the connection ends and the game reloads the durable checkpoint.

- If either side stopped at PREPARED, neither side could have committed: both
  retain their original Pokémon. This also covers a failed first prepare write.
- READY/READY or READY/COMMITTED can finish using the exact stored after-images.
- COMMITTED/DONE can finish without re-running evolution or exchanging Pokémon
  again. Completed receipts retain the original outgoing hash.
- If a DONE partner lost the last network acknowledgement and has since reused
  its journal, its strictly newer monotonic sequence proves completion of the
  older COMMITTED trade. Helping the older peer does not unlock a newer pending
  transaction with somebody else.
- Missing, contradictory, wrong-partner, or incompatible-build evidence fails
  closed; there is no timeout-based rollback.

Keep the same ROM until an interrupted trade has been recovered. Do not replace,
edit, or rewind either player's save independently during recovery. A deliberately
confirmed new-game overwrite clears the journal along with the old save.

Ordinary saves and the pre-trade checkpoint now use the sealed PC recovery
journal: unsealed writes leave home pages untouched, while sealed writes replay
the entire transaction after restart. The trade state machine checks the checkpoint
result before exchanging offers and synchronizes both peers after the commit;
there are no legacy sector-rotation or signature-replacement stages. Home is
always physical sector zero. Creating or explicitly overwriting a new
save still initializes its backing pages directly and must not be interrupted.
Emulator save-state rewinds/cloned saves and flash corruption destroying recovery
evidence cannot be made safe by this protocol.

## Capacity and verification

Hall of Fame saves check the main-save commit and both history-page writes in
both Emerald and FRLG. Any failure opens a read-only restart screen, including
errors with no damaged-sector bit set. It never retries, wipes sectors, or
resumes gameplay after an uncertain save. Pressing A restarts through normal
journal recovery. A failed history write can leave the main save safely committed
while the Hall of Fame history is incomplete; the history is not a backup save.
The startup warning for missing flash remains available independently.

Forty boxes (1,200 slots) are enabled. Only three boxes are resident in RAM; see
[PC storage](pc_storage.md) for cache, autosave and recovery behavior. Completed
trade receipts are copied into core sector zero before the shared journal is
reused. The next trade reads that durable receipt if sectors 30–31 no longer
contain trade images. A durable DONE/ABORTED receipt also takes precedence if
interrupted journal erasure leaves an older pending phase behind. Pending
transactions cannot be displaced by PC autosaves.

`make check -j4 TESTS=test/link_safety.c` exercises build checks, RAM restoration,
torn journal/home writes, corrupted records, wrong peers and transaction proofs,
and nine two-machine restart/reconciliation scenarios. It simulates independent
flash images; it is **not** a real cable/RFU integration test. Before release,
exercise complete wired, wireless, multi battles and repeated trades on two
instances/devices, including disconnects/resets at every durable phase, mail,
trade evolution/move learning, wrong builds, and returning from recovery.
