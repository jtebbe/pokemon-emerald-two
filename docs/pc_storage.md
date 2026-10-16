# Streamed PC storage

The PC has **40 boxes / 1,200 slots**. A boxed Pokemon is 72 bytes. Only three
30-Pokemon buffers (6,480 bytes plus small metadata) are allocated in EWRAM.
The window wraps around: box 1 has boxes 40 and 2 as neighbors.

Occupancy comes directly from the species field; egg state comes only from
`PokemonData.isEgg`. The old duplicate header flags and Bad Egg behavior are
removed. Their three header bits are unused padding, retaining the existing
72-byte format and form-change counter position. Old values in those bits are
ignored rather than used to override the Pokemon's species or egg state.
Flash checksums and recovery journals still validate saved pages.

Box names, wallpapers, daycare and fusion partner data remain in the resident
save structure. There is no hidden full-PC RAM image or second complete save.

## Access and saving

`PCStorageWindow` loads the requested box and its two neighbors. The existing
box accessors go through this cache. Arbitrary scans can load other windows;
read-only scans do not save. Before discarding an unsaved dirty window, the game
commits it through the compact log during a PC session, or through a full-save
transaction outside the PC.

Pointers from `GetBoxedMonPtr` or `PCStorageBox` are only valid until another
window is loaded. Copy the Pokemon before looking up a distant destination.
`SetBoxMonAt` explicitly handles a cached source pointer that could be evicted.
Dirty detection compares a RAM hash with the baseline recorded on load/commit,
including raw-pointer edits. It never rereads flash to check whether a cached
box changed. Incoming boxes are copied directly from checksum-validated pages,
with shared pages reused within a window load.

The first PC opening after load performs a full game save, then prepares a
compact edit journal. Later openings resume the pending journal, appending
party/bag/field changes since the previous visit without a full checkpoint.
The durable core baseline, record index, sequence and append position survive
between sessions; they must not be reset to the current unsaved overworld state.
Completed PC changes save automatically, including Pokemon
pickups/placements, item transfers, metadata changes and screen transitions.
Each edit appends only changed core bytes, changed Pokemon slots, and cursor
changes, with long zero runs compressed. Ordinary moves do not erase sectors or
rewrite entire save pages. Actual edits request saves at stable input
boundaries; idle frames do not scan the save image, hash boxes or access flash.
Simply browsing to another box does not write anything. The selected box and
elapsed clock time persist with the next real edit or once on PC exit. Callers
editing raw pointers outside the normal setters should request an autosave when
returning to the PC UI; eviction and explicit saves still detect those edits.
The UI does not save partially completed animations. Input waits until an edit's
final seal has verified; background and sprite animation continue while the
record writer emits at most 128 bytes per tick. Longer operations display a temporary
Saving/Please wait indicator while the PC screen is visible. Box loading
overlays sealed edits from flash, so evicting a durably edited box does not force
a full save. Raw edits outside this normal UI workflow still use the synchronous
full-save fallback before dirty eviction. A direct deposit into a distant box
can seal its intermediate pickup synchronously within that operation, but still
uses a small record rather than a full checkpoint.

When the journal fills (16 KB or 128 records), it checkpoints the previous edits
before appending the next one. Ordinary explicit saves and scripted save/transfer
paths also drain it before reusing the shared recovery area. These rarer
checkpoints still lock input and take longer than a normal pickup/placement;
page programming yields between 512-byte chunks, but a hardware sector erase cannot be
split across frames. Real-device latency and endurance still need measurement.

PC exit only seals the final small record and suspends the session; it does not
checkpoint or erase the pending journal. Box reads outside the PC continue to
overlay those sealed records. Reopening appends more records to the same log.
There is no automatic background folding during overworld play, and unsaved
overworld progress after exit is not included in reset recovery until another
save or PC opening records it. A reset replays the log before loading the game,
so quitting without a manual save still preserves the sealed PC operations.

The final record starts alongside the closing animation. Only the confirmed
exit task and closing effect may advance while the writer is busy; the PC cannot
accept edits or free its buffers until both finish. Exit-only flags and party
counts are captured before the final record, and the screen transition does not
trigger a second save for elapsed clock ticks. If that final record fills the
journal, rollover can still cause a longer exit; the suspension request survives
checkpointing and publication into the fresh journal.

In addition to the three box buffers, compact commits reserve a 6 KB serialized
record buffer, one resident-core baseline and a 1 KB flash-record index. These
are not additional box caches or a full PC/save backup.

## Flash layout

| Sectors | Contents |
| --- | --- |
| 0–1 | Resident save data (compile-time limited to two pages) |
| 2–23 | Packed boxed Pokemon; unused final-page tail stores the PC cursor |
| 24–27, 30–31 | Shared recovery area; layout below depends on the operation |
| 28–29 | Hall of Fame |

Each sector is 4,096 bytes, with 4,084 data bytes and a 12-byte footer. Each
ordinary full-save transaction includes both core pages, all dirty cached box
pages, and the cursor page if needed. Three adjacent boxes, including a wraparound window, fit in the
six-page journal together with the core and cursor changes.

While a PC journal is pending, sectors 24–27 hold the compact append-only edit
log. Sector 30 is a one-page checkpoint shadow; sector 31 is not used by this
log. Each edit record has a sequence, CRC and a seal programmed last. Torn
trailing records are ignored; sealed records are replayed in order. Home pages
remain unchanged until checkpointing, and ordinary loads overlay their sealed
records. Idle frames never fold or rewrite pages in the background.

Checkpointing constructs one page's after-image, writes and verifies the sealed
shadow, then replaces that home page with its signature last. Pages whose final
contents match their home image are skipped, even if intermediate records touched
them. Checksum equality is only a prefilter: skipping requires an exact byte
comparison. Empty cursor records are canonicalized so a completed pickup/drop
does not leave irrelevant origin metadata that forces another page rewrite.

A verified page's shadow remains sealed until the next changed page reuses it;
replaying it after reset is harmless. This removes the previous erase-after-each-
page followed by erase-before-the-next-page duplication. The final shadow is
retired before the log header, and never before its home page verifies. Small
state transitions and final signatures run alongside their associated page work
instead of each consuming another display frame. Startup repairs a sealed shadow first, then
replays the compact log idempotently. The log header is erased only after all
touched home pages are complete. A reset anywhere in checkpointing can therefore
repeat the work without losing an already sealed move. Explicit non-PC saves
drain the log before reusing the recovery area for a full-save transaction.

Full-save journal images have CRC-protected contents and destinations. The
signature is programmed last; the last image seals the transaction. No home page changes
until every image is sealed and verified. Restart validates the whole transaction
before replaying it, and retires the seal only after all home writes verify.
Replay is idempotent. An unsealed transaction leaves the previous save intact.
Only destination pages are erased by the writer; there is no redundant erase
of all six journal pages before each save. Unused journal pages need not be
cleared because the previous commit seal is already retired. Recovery scans
signatures first and reads full journal pages only when a seal exists. CRC-32
uses a small lookup table with exactly the same results as the original bit loop.

Creating or explicitly overwriting a new save initializes backing pages directly;
that initial creation is not an atomic replacement for an older save. The new
layout has a new signature and does not import old-format saves. The compact-log
optimization does not change the existing forty-box home layout or signature.
Keep this ROM until an open compact log has been checkpointed; older builds do
not understand its recovery records. Never rewind emulator save states or edit
the flash image independently while a recovery journal is active.

## Pokemon or items on the cursor

The last PC page records the carried Pokemon (up to a complete box), their
original free slots, and any cursor-held item. During play this uses the PC UI's
existing moving-Pokemon buffers, not a fourth box cache. Swapping Pokemon retains
the original rollback hole for the displaced Pokemon.

After restart, carried Pokemon return to their original box slots, or to a free
party slot for a party-origin pickup. Cursor-held items return to the bag. Recovery
and clearing the cursor record form one atomic transaction, so restarting again
does not duplicate anything. Recovery reuses the three cache buffers before the
normal window loads. Contradictory slots or insufficient bag/party space fail
closed rather than overwriting Pokemon or discarding items.

## Multiplayer and errors

Link battles cannot write the PC or ordinary save. Pending trades reserve sectors
30–31, block PC entry and normal saving, and require the same partner as described
in [multiplayer saves](multiplayer_saves.md). Completed trade receipts persist in
core sector zero when an ordinary save reuses those sectors.
The trade protocol's pre-trade checkpoint must drain the PC log first. The
low-level PREPARED writer refuses a pending/active PC log even if a future caller
skips that checkpoint; it cannot overwrite the shared sectors directly. Pending
PC journals may remain during link battles, but only read-only overlays are
allowed; battles cannot append, reopen, flush or otherwise modify them.

A failed PC write stops further saving and asks for a restart. It does not run
the legacy destructive save-failure retry screen. On restart, the last sealed
transaction is recovered before any PC data is loaded.

## Verification

The debug menu's **Fill PC Boxes Fast** action processes one three-box window
per task step, saving each batch before displaying it as complete. It yields
between transactions so progress can redraw, preserves occupied slots, restores
the selected box's window, and does not report success until box 40 is durable.
Flash errors stop the fill and request recovery instead of continuing through
the remaining slots. Individual flash transactions still pause input briefly.
The cartridge writer programs a sector body in one driver call, with its signature
left erased, then writes the signature last and verifies the completed page.
This avoids repeating bank/polling setup for every byte without publishing a
partially written transaction.

After closing the fill-complete message, reopen the debug menu and choose
**PC/Bag → Open Pokémon Storage** to use the regular PC interface. The shortcut
uses the existing storage-access script, including autosave and pending-trade
checks, and releases field controls normally when the PC closes.

`make check -j4 TESTS=test/pc_storage.c` covers all 1,200 slots, three-buffer
residency, read-only scans, dirty eviction, raw-pointer edits, carried Pokemon and
items, swaps, full-box movement, daycare/NPC transfers, fusion metadata, Hall of
Fame isolation, shared trade journal use, corruption and simulated interrupted
flash writes. It also covers the actual debug-fill batches, preserving existing
Pokemon, final-box durability and failed writes. One full-fill test uses the
shipping cartridge flash driver in an isolated emulator, rather than the
in-memory fault-injection adapter. Physical GBA hardware and real cable/RFU
integration checks remain necessary before release.

The cartridge-driver navigation benchmark checks two clean switches in at most
four GBA frames (about 67 ms), with no writes or erases. A thousand stable idle
checks must perform zero flash reads, writes or erases. These limits cover the
storage backend, not the PC's normal scrolling animation or sprite loading.

Compact-log tests cover full-box moves, distant swaps, item/party recovery,
metadata edits, torn record seals, CRC damage, resets during shadow/home writes,
eviction overlays and repeated journal rollover. The cartridge-driver benchmark
currently measures single pickup/placement records of 140/108 bytes, with no
erases or home-sector rewrites: staging takes at most two GBA frames, followed
by two or three writer ticks, each at most two frames. These are backend bounds
in the emulator, not an end-to-end UI or physical-hardware measurement.

The persistent-journal exit cartridge benchmark includes one render-frame wait
per writer tick, unlike a tight synchronous flush. All four cases (clock-only,
same-page move, distant move and a pickup returned to its original slot) now
measure 5–6 frames, about 0.08–0.10 seconds, for a final 32-byte record, with zero
erases or home-page rewrites. The previous checkpoint-on-exit implementation
took 42–114 frames for these same cases. These include saving five minutes of
elapsed play time, but not UI drawing, field reload or the closing animation.
They assume room remains in the log; actual hardware still needs measurement.
The same cartridge benchmark records an overworld money change while reopening
the retained journal: 6–9 frames and 32 appended bytes, with no erases. This
includes restoring the selected box window when it is no longer cached, but not
the PC interface's normal graphics initialization.

Tests cover reopening without erases, retained three-box overlays, overworld
party/bag/mail snapshots, idle overworld frames, every final-record publication
byte, repeated cross-session rollover, normal-save interruption on both sides
of its commit seal, trade reservation, explicit new-game overwrite and repeated
reset recovery. Checkpoint tests still assert programmed-byte/erase limits,
checksum-collision safety and resets at every shadow/home/retirement stage.
