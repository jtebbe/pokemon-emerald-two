# Test-suite repairs — 2026-10-16

This follows the pre-fix investigation in [README.md](README.md). The original audit and its logs are retained unchanged.

## Repairs

- **Battle intro hangs:** intro bookkeeping now resolves a missing partner to the battler itself instead of letting both sides share an absent battler slot. Actual partner routing still uses battler positions. Regression tests cover singles and reordered doubles.
- **Cascading crashes after a timeout:** fatal test failures restart through the cartridge reset vector, clearing runtime state while retaining the test cursor, expected result, and previous failure status. Recovery reconstructs worker assignment using the active test filter. BIOS SoftReset is deliberately not used because it clears the persistent runner state. An intentional timeout self-test verifies that execution resumes.
- **Capture input hangs:** the test driver skips interactive Pokédex and nickname screens, without skipping the actual capture or Pokémon transfer. Capture parameters start with clean Pokédex ownership data.
- **Test isolation:** each test begins with empty runtime parties and party counts, not only reset save data. This prevents preceding tests from supplying stale party slots to save tests.
- **Stat fixtures:** explicit test-only stat overrides survive level recalculation and form changes. Implicit action-order speeds survive ordinary recalculation but allow new form stats to take effect. None of these overrides are stored in Pokémon or save data; production stat generation remains fixed at the requested contribution of 31.
- **Battle messages:** synthetic opponents no longer acquire a placeholder trainer class. Placeholder scratch space resets for each expansion; empty classes do not add a leading space. Separate opponents and partners resolve their own names, NPC partners use their trainer data, and both Pokémon in an ordinary double battle retain their shared trainer name.
- **Outdated expectations:** PP assertions use maximum PP; Hidden Power tests use its fixed Psychic typing. Damage-formula benchmarks pin their input stats instead of depending on species defaults. Knockout and berry tests set the thresholds they actually need. Half-HP and quarter-HP expectations account for integer rounding.
- **AI combo regression:** the Fusion test now checks the exact adjacency bonus. It does not incorrectly require Flamethrower to beat a stronger unboosted Fusion Flare. The focused four-case combo suite passes.

The Hidden Power Anticipation case is now implemented and passing rather than marked known-failing, and its obsolete dynamic-type TODO was removed. No failing tests were newly marked known-failing, skipped, or converted to TODOs to obtain a passing run.

## Verification

The complete `make check -j4` run exits successfully. See [verified-suite.log](verified-suite.log).

| Result | Count |
| --- | ---: |
| Passed | 5,458 |
| Expected-failure self-tests | 10 |
| Pre-existing known-failing cases | 11 |
| TODO placeholders | 353 |
| Unexpected failures, crashes, or timeouts | 0 |
| Failed assumptions | 0 |
| Total | 5,832 |

The intentional recovery tests include two CRASH results and one TIMEOUT result; these are expected, green results, not remaining crashes. The previously unexpected failures from the original audit are resolved. The total differs from the pre-fix audit because two intro regression tests and a timeout self-test were added, and one obsolete Hidden Power TODO was removed.

The normal `make -j4` ROM build succeeds; see [verified-rom-build.log](verified-rom-build.log). The built ROM is `pokeemerald.gba`. Save layout, PC box count, PC recovery journals, daycare, and fusion mechanics were not changed by these repairs.

The suite still contains pre-existing TODO placeholders and explicitly known-failing cases; these are distinct from unexpected test failures. Green CRASH/TIMEOUT results in the runner's own recovery tests are intentional. Emulator tests do not substitute for hardware or two-device multiplayer testing.

## Follow-up: provenance of remaining TODOs and known failures

Compared the remaining markers against original repository HEAD `ece650b5c6` (`14/10/26 master to upcoming merge (#10917)`). All eleven active known-failing test bodies are unchanged from that baseline, including their existing `KNOWN_FAILING` declarations. They cover older-generation behavior, Champions Sheer Force timing, sleep-clause/Transform interaction, Neutralizing Gas timing, Mirror Armor/Court Change, and NPC item-use test-driver limitations. No remaining known-failure declaration was introduced by this refactor.

Two inherited TODOs were nevertheless made obsolete by the requested changes and are now cleaned up:

- Deleted `test/battle/hold_effect/friendship_up.c`, which contained only a Soothe Bell TODO for the removed friendship system. The original file remains recoverable from Git.
- Replaced Counter's old dynamic-Hidden-Power TODO with an executable, two-parameter regression: fixed 60-power special Psychic Hidden Power is rejected by Counter and reflected for twice its damage by Mirror Coat.

All **351 remaining named TODO placeholders** already exist in the original HEAD. These are pre-existing coverage gaps, not tests newly deferred by our work. This is a source/history comparison, not a separate build of unmodified HEAD.

Focused verification: `make check -j4 TESTS=test/battle/move_effect/reflect_damage.c` succeeds with **28 passed / 5 existing TODOs / 0 unexpected failures**. See [todo-reflect-verification.log](todo-reflect-verification.log). Only tests changed in this follow-up; the complete-suite totals above describe the preceding full verification, not a new full run after this follow-up.
