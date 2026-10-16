# origin/upcoming merge — 2026-10-16

Merged upstream tip `8b47321cc92b6d5c1cf5b86103637c01d2fc6544` into `shadows-master`, retaining the refactor commit `efe54397e4` as the first-parent history. The common ancestor was `ece650b5c6`.

The three incoming commits add Battle Info and the debug battle win/lose options. The merge used our side for overlapping edits; no unresolved textual conflicts occurred.

## Compatibility adjustments

- Battle Info and its input hints use the unified `gSaveData` instead of the removed save-block pointers.
- NPC partner names use ordinary trainer data rather than the removed Frontier name helper.
- Removed the new screen's G-Max effects, Steelsurge hazard entry, Frontier include, and retired Pokérus checks. Those systems remain deleted.
- Retained upstream's Battle Info UI, duration tracking, charging-state changes, and debug battle options.
- Save data layout, flat Pokémon structures, the three-box cache, and the 40-box limit are unchanged.

## Verification

- `make -j4`: successful normal ROM build.
- `make check -j4`: successful full-suite run; **5,459 passed, 10 expected-failure self-tests, 11 pre-existing known-failing cases, and 351 TODOs**, for 5,831 total cases.
- No unexpected failures, crashes, timeouts, or failed assumptions.
- No unresolved conflicts or whitespace errors.

See [upcoming-merge-tests.log](upcoming-merge-tests.log) and [upcoming-merge-build.log](upcoming-merge-build.log). The new UI has compile/regression coverage, not a separate manual visual or two-device multiplayer verification. Nothing was pushed.
