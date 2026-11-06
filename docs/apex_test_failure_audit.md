# Apex test failure audit — November 6, 2026

## Finding: our expanded switch-in AI overflows the battle-test stack

The full `make check -j4 TESTS=` run on `shadows-master` reported 112 unsuccessful tests: 107 failures and 5 invalid tests. Every one of those 112 tests passed when rerun independently, using the same source code. This is a regression in shared-run execution, rather than evidence of 112 independent battle-mechanics bugs.

The confirmed cause is a stack overflow triggered by our expanded AI forecasting and switching code. Battle test functions, including their `THEN` blocks, execute on the **1,024-byte** stack in `BattleTestData.stack`. Two existing tests call `GetMostSuitableMonToSwitchInto` directly from `THEN`:

- `Ilex Whimsy switch-in prediction leaves the real side’s timers and flags unchanged` — `test/battle/ai/apex_abilities.c`.
- `AI switch-in simulation does not leave a benched Distortion ability active` — `test/battle/ai/distortion.c`.

Both tests still report PASS, but a temporary stack canary detects an overwrite below the stack. Immediately after each call, ambient overworld weather changes from **0 (WEATHER_NONE)** to **6 (WEATHER_FOG_HORIZONTAL)**. The overwritten weather state survives into later tests on that emulator worker. The existing overworld fog rule then creates permanent **Misty Terrain** in their battles.

This explains the common symptoms: blocked poison/burn/paralysis/sleep/confusion, curing berries never activating, missing status RNG calls, unexpected terrain, and reduced Dragon damage. For example, Scale Shot needs extra hits to KO, Fickle Beam fails its expected damage ratio, and Twister’s grounded-versus-airborne comparison changes because Misty Terrain halves Dragon damage only against grounded targets.

All 112 unsuccessful outcomes in the original run came from workers 0 and 1; workers 2 and 3 reported none. The two overflowing switch-in tests were assigned to those affected workers.

## Connection to our changes

The uncommitted Apex AI changes increased stack use along the existing switch-in call chain. A controlled compiler comparison of HEAD source versus current source, using the current headers and the same ARM compiler/optimization settings, reports these local-frame sizes:

| Function | HEAD source | Current source | Increase |
| --- | ---: | ---: | ---: |
| `GetMostSuitableMonToSwitchInto` | 296 bytes | 328 bytes | 32 bytes |
| `InitializeSwitchinCandidate` | 104 bytes | 168 bytes | 64 bytes |
| `AI_CalcDamage` | 236 bytes | 332 bytes | 96 bytes |
| **Combined additional local-frame storage on this path** | | | **192 bytes** |

These are compiler-reported local frames, excluding saved registers, outgoing arguments, and other callees. They are a controlled source comparison, not measurements from a retained pre-change ROM. The old temporary snapshots are no longer available in this environment. The HEAD AI source needed only an API compatibility adjustment for the changed `IsDazzlingAbility` signature to compile against current headers.

The growth comes from the added entry-effect snapshots, forecasts, and damage-simulation bookkeeping in our AI work. The problem is not specific to a Pokémon using Leek Master or Noblesse Oblige: these common routines run for other switch candidates too.

## Verification

- Re-ran all 112 reported failing/invalid test definitions in isolated emulator runs: **112/112 passed**. Parameterized definitions ran their complete parameter sets.
- Added temporary stack guards and weather tracing: both switch-in tests overwrote the 1 KB stack and changed weather from 0 to 6.
- Temporarily gave test functions a separate **4 KB diagnostic stack**, without changing battle or AI logic. Both affected files passed all **13 tests**, with **zero stack-overflow events and zero weather-contamination events**.
- Removed all temporary instrumentation and the diagnostic stack, restoring the original test-runner source byte for byte, including preserving the pre-existing difficulty-reset change.
- The two intentional CRASH tests and the intentional TIMEOUT test are test-runner self-tests, not additional unexpected crashes. The 11 KNOWN_FAILING, 351 TODO, and 10 EXPECTED_FAILING entries were excluded from this audit’s 112 cases.

## Permanent test-stack correction

The battle-test runner now uses a separate, 8-byte-aligned **4,096-byte EWRAM stack**, with **64-byte guard regions on both sides**. The old inline 1 KB stack was removed from `BattleTestData`; `sBackupMapData` itself remains 20,480 bytes. Existing compile-time and runtime checks still enforce fixture and parameterized-result capacity.

The stack is repainted only at the same estimate/setup/fixture boundaries where the old inline stack was cleared. Its contents remain live between calls, preserving pointers captured by `HP_BAR` and other scene events. Guards are checked before and after calls and on error/teardown paths. Corruption becomes a test-runner ERROR and cannot be accepted as an expected assertion failure or a failed statistical trial. The runner prints an observed stack high-water mark for each worker. This watermark measures written stack bytes; the guard checks and remaining headroom are also important.

Four self-tests cover both guard regions, watermark calculation, and a 2 KB local frame with a captured damage value. The two switch-in regressions also explicitly verify that weather remains unchanged after forecasting. No weather reset was added to hide corruption, and no production battle/AI code was changed by this correction.

### Memory and platform limits

The linked test ROM was checked against the pre-fix memory map:

| Resource | Before | After | Constraint/result |
| --- | ---: | ---: | --- |
| Static EWRAM allocation, including the fixed heap | 230,968 bytes | 235,208 bytes | 26,936 bytes remain in the 256 KiB region |
| Static IWRAM allocation | 29,564 bytes | 29,564 bytes | Unchanged; system-stack clearance remains 2,756 bytes |
| Heap capacity | 115,968 bytes | 115,968 bytes | Unchanged |
| `sBackupMapData` capacity | 20,480 bytes | 20,480 bytes | Unchanged; fixture/result checks retained |
| Dedicated stack, guards, watermark, and alignment | — | 4,240 additional bytes | All additional storage is in EWRAM |
| ROM address range | Fits | Fits | Linker-enforced 32 MiB limit retained |

The stack storage is compiled only into test ROMs. Production IWRAM and heap limits were not increased or reduced.

### Final validation of the permanent correction

`make check -j4 TESTS=` completed successfully on `shadows-master` in **29 minutes 3 seconds**:

- **7,045 passed; zero unexpected failures or errors.**
- All **112 previous failures/invalid tests are resolved**, with four new stack self-tests added.
- Existing classifications remain unchanged: **11 KNOWN_FAILING, 351 TODO, 10 EXPECTED_FAILING**.
- No stack-guard corruption was reported.
- Worker high-water marks were **2,412 / 2,320 / 2,136 / 2,136 bytes**. The largest observed use leaves **1,684 bytes** of the 4 KB stack untouched.
- An earlier focused run passed **108 checks**, including all six unchanged AI timing ceilings and both corruption-producing switch-in regressions.
- Rechecking the linked image confirmed the memory figures above, unchanged IWRAM allocation and heap capacity, retained fixture capacity checks, 8-byte stack alignment, and compliance with the ROM address limit.

The fix changes test infrastructure and regression assertions only; production battle mechanics and AI code are unchanged. Guard checks run when test functions return and on error paths; the GBA does not provide hardware memory protection, so the measured headroom and future high-water reports remain part of the validation.

Current-session validation artifacts:

- `/tmp/guarded-stack-fix/full-suite/test.log` and `result.json`.
- `/tmp/guarded-stack-fix/memory-report.json`.
- `/tmp/guarded-stack-fix/regressions/results.json`.


## Follow-up maintenance

1. Keep the dedicated stack in EWRAM and retain the guard/capacity checks. Use its high-water reports and compiler stack-usage comparisons when extending AI forecasting.
2. Treat guard failures as corruption. Resetting weather alone would hide the problem and is not a substitute for stack bounds checks.
3. Retain full-suite runs with `-j4` and the AI timing checks; future AI changes can affect both stack requirements and normal battle performance.

## Evidence files

Diagnostic logs and the complete machine-readable inventory are under `/tmp/failure-audit-nov06/` in the current execution environment:

- `failures.json`: all original failure details and worker assignments.
- `all-isolated-results.json`: isolated rerun result and log path for every case.
- `confirmed-overflow-0.log`, `confirmed-overflow-1.log`: stack canaries and weather changes.
- `expanded-stack-0.log`, `expanded-stack-1.log`: 4 KB diagnostic-stack control runs.
- `head-battle_ai_switch.s`, `current-battle_ai_switch.s`, `head-battle_ai_util.s`, `current-battle_ai_util.s`: compiler frame comparisons.

## Complete isolated-rerun inventory

All rows below passed in isolation. “Full-run result” records their original outcome before isolation.

| Source | Test | Full-run result | Isolated result |
| --- | --- | --- | --- |
| `test/battle/ai/serpents_retort.c:128` | Serpent's Retort AI distinguishes harmful poison from immunity and beneficial poison | FAIL | PASS |
| `test/battle/ai/slowpoke_apex.c:156` | Spiced Dreams switch AI predicts entry sleep and its power boost without leaking status to other candidates | FAIL | PASS |
| `test/battle/ai/succulent_trap.c:189` | Succulent Trap AI values poison setup only when it improves its own or its partner's trap | FAIL | PASS |
| `test/battle/ai/values_moves_over_splash.c:594` | AI values moves above Splash, 701-800 | FAIL | PASS |
| `test/battle/ai/values_moves_over_splash.c:236` | AI values moves above Splash, 201-300 | FAIL | PASS |
| `test/battle/ai/vicious_stings.c:83` | Vicious Stings AI rewards poisoning to enable Merciless | FAIL | PASS |
| `test/battle/hold_effect/berserk_gene.c:266` | Berserk Gene confusion can be healed with bag items | FAIL | PASS |
| `test/battle/hold_effect/cure_status.c:393` | Lum Berry properly cures a battler affected by a non-volatiles status and confusion | FAIL | PASS |
| `test/battle/hold_effect/cure_status.c:173` | Cheri and Lum Berries cure paralysis | FAIL | PASS |
| `test/battle/hold_effect/cure_status.c:130` | Chesto Berry cures sleep when Yawn takes effect | FAIL | PASS |
| `test/battle/hold_effect/cure_status.c:111` | Chesto and Lum Berries cure sleep | FAIL | PASS |
| `test/battle/hold_effect/cure_status.c:67` | Rawst and Lum Berries cure burn | FAIL | PASS |
| `test/battle/ai/auroral_herald.c:117` | Auroral Herald AI values frostbite only against susceptible targets | FAIL | PASS |
| `test/battle/ai/blazing_rush.c:79` | Blazing Rush AI respects burn immunity curing berries Guts and Quick Feet in entry forecasts | FAIL | PASS |
| `test/battle/ai/check_bad_move.c:398` | AI avoids hypnosis when it can not put target to sleep | FAIL | PASS |
| `test/battle/ai/check_bad_move.c:330` | AI penalizes Yawn when target can self-status with Flame/Toxic Orb | FAIL | PASS |
| `test/battle/ai/hypnotic_pattern.c:89` | Hypnotic Pattern AI favors useful entry sleep and avoids immune or instantly waking foes | FAIL | PASS |
| `test/battle/ai/lulling_melodies.c:146` | Lulling Melodies AI rewards sleep setup to enable its damage bonus | FAIL | PASS |
| `test/battle/ai/moon_attuned.c:136` | Moon-Attuned switch AI predicts the entry status cure | FAIL | PASS |
| `test/battle/ai/mushroom_madness.c:130` | Mushroom Madness AI redirects its status-move targeting as well | FAIL | PASS |
| `test/battle/ai/values_moves_over_splash.c:69` | AI values moves above Splash, 1-100 | FAIL | PASS |
| `test/battle/hold_effect/ability_shield.c:120` | Ability Shield protects against Mycelium Might (no message) | FAIL | PASS |
| `test/battle/hold_effect/berserk_gene.c:287` | Berserk Gene confusion can be healed with used held items | FAIL | PASS |
| `test/battle/move_animations/all_anims.c:1984` | Move Animations work Gen2 | FAIL | PASS |
| `test/battle/hold_effect/life_orb.c:103` | Life Orb does not activate if on a confusion hit | FAIL | PASS |
| `test/battle/hold_effect/terrain_extender.c:131` | Terrain Extender makes terrain created by moves last 8 turns | FAIL | PASS |
| `test/battle/hold_effect/zoom_lens.c:24` | Zoom Lens does not boost accuracy against a target that switched in this turn | FAIL | PASS |
| `test/battle/item_effect/cure_status.c:752` | Full Heal, Heal Powder and Local Specialties heal a battler from being confused | FAIL | PASS |
| `test/battle/move_effect/beak_blast.c:289` | Beak Blast's charging message is shown regardless if it would've missed | FAIL | PASS |
| `test/battle/move_effect/beak_blast.c:96` | Beak Blast burns only when contact moves are used | FAIL | PASS |
| `test/battle/move_effect/camouflage.c:106` | Camouflage fails if the user already has the environment's type | FAIL | PASS |
| `test/battle/move_animations/all_anims.c:2269` | Move Animations work Gen6 | FAIL | PASS |
| `test/battle/move_effect/confuse.c:50` | Teeter Dance can confuse foes and allies | FAIL | PASS |
| `test/battle/move_effect/confuse.c:17` | Teeter Dance confuses target | FAIL | PASS |
| `test/battle/move_animations/all_anims.c:2127` | Move Animations work Gen4 | FAIL | PASS |
| `test/battle/move_effect/court_change.c:86` | Court Change swaps entry hazards used by the player | FAIL | PASS |
| `test/battle/move_effect/court_change.c:57` | Court Change swaps entry hazards used by the opponent | FAIL | PASS |
| `test/battle/move_effect/dark_void.c:80` | Dark Void can be used by Pokémon other than Darkrai in Gen 4-6 | FAIL | PASS |
| `test/battle/move_effect/defog.c:475` | Defog removes terrain (Gen 8+) | FAIL | PASS |
| `test/battle/move_effect/disable.c:398` | Disable's timer only counts down when trying to use a move (Gen 1-2) | FAIL | PASS |
| `test/battle/move_effect/beak_blast.c:255` | Beak Blast burns even if the target has Unseen Fist | FAIL | PASS |
| `test/battle/move_effect/claws_cuts_priority.c:5` | Frigid Claws inflicts frostbite, not freeze, with a 20 percent chance | FAIL | PASS |
| `test/battle/move_effect/encore.c:151` | Encore forces the last move used while asleep | FAIL | PASS |
| `test/battle/move_effect/defog.c:525` | Defog removes Toxic Spikes from target's side | FAIL | PASS |
| `test/battle/move_effect/fickle_beam.c:9` | Fickle Beam deals double damage 30% of the time | FAIL | PASS |
| `test/battle/move_effect/defog.c:236` | Defog doesn't remove Mist or Safeguard from the user's side | FAIL | PASS |
| `test/battle/move_effect/disable.c:508` | Disable's timer counts down regardless of the action (Gen 3+) | FAIL | PASS |
| `test/battle/move_effect/flatter.c:127` | Flatter confuses the target even when at -6 Sp. Atk and has Contrary | FAIL | PASS |
| `test/battle/move_effect/flatter.c:51` | Flatter raises the target's Sp. Atk even if they're already confused | FAIL | PASS |
| `test/battle/move_effect/disable.c:142` | Disabled moves can still be used via Sleep Talk | FAIL | PASS |
| `test/battle/move_effect/focus_punch.c:432` | AI will Incapacitate -> Substitute -> Focus Punch if able | INVALID | PASS |
| `test/battle/move_effect/final_gambit.c:74` | Final Gambit does not faint user if attacker fails to attack | FAIL | PASS |
| `test/battle/move_effect/future_sight.c:471` | Toxic Chain can inflict bad poison from Future Sight if the user is still on the field | FAIL | PASS |
| `test/battle/move_effect/grudge.c:95` | Grudge's effect disappears if the user takes a new turn - Sleep | FAIL | PASS |
| `test/battle/move_effect/hit_switch_target.c:138` | Dragon Tail switches target out and incoming mon has Immunity negated by Mold Breaker | FAIL | PASS |
| `test/battle/move_effect/ice_spinner.c:41` | Ice Spinner and Steel Roller remove a terrain from field | FAIL | PASS |
| `test/battle/move_effect/instruct.c:155` | Instruct doesn't bypass sleep | FAIL | PASS |
| `test/battle/move_effect/magic_coat.c:25` | Magic Coat prints the correct message when bouncing back a move | FAIL | PASS |
| `test/battle/move_effect/misty_terrain.c:86` | Misty Terrain uses the defender's ability when checking if it is grounded | FAIL | PASS |
| `test/battle/move_effect/pledge.c:37` | Rainbow doubles the chance of secondary move effects | FAIL | PASS |
| `test/battle/move_effect/misty_terrain.c:13` | Misty Terrain protects grounded battlers from non-volatile status conditions | FAIL | PASS |
| `test/battle/move_effect/multi_hit.c:255` | Scale Shot decreases defense and increases speed after killing opposing with less then 4 hits | FAIL | PASS |
| `test/battle/move_effect/nature_power.c:41` | Nature Power can target ally (Gen6+) | FAIL | PASS |
| `test/battle/move_effect/reflect_damage.c:81` | Reflect Damage: Counter is affected by Protect effects if it was damaged by that battler | FAIL | PASS |
| `test/battle/move_effect/sleep.c:42` | Hypnosis inflicts 1-2 turns of sleep (Champions) | INVALID | PASS |
| `test/battle/move_effect/spite.c:15` | Spite reduces the PP the last move used even while asleep | FAIL | PASS |
| `test/battle/move_effect/steel_roller.c:69` | Steel Roller wont be chosen by AI if there is no terrain on the field | FAIL | PASS |
| `test/battle/move_effect/stomping_tantrum.c:53` | Stomping Tantrum will deal double damage if user failed to attack due to paralysis | FAIL | PASS |
| `test/battle/move_effect/storm_sun_fists.c:146` | Storm Sacrifice cannot replace primal sun and does not charge HP on failure | FAIL | PASS |
| `test/battle/move_effect/take_heart.c:89` | Take Heart cures sleep when used by Sleep Talk | FAIL | PASS |
| `test/battle/move_effect/terrain_boost.c:107` | Terrain Boost: Misty Explosion increases in power by 50% when affected by Misty Terrain | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:266` | Toxic Spikes print bad poison for 2 layers | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:225` | Toxic Spikes inflicts poison on switch in after Primal Reversed mon fainted | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:88` | Toxic Spikes inflicts poison on subsequent switch ins | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:70` | Toxic Spikes fails after 2 layers | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:45` | Toxic Spikes inflicts bad poison on switch in | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:23` | Toxic Spikes inflicts poison on switch in | FAIL | PASS |
| `test/battle/move_effect_secondary/freeze.c:23` | Powder Snow inflicts freeze | FAIL | PASS |
| `test/battle/move_effect_secondary/thrash.c:39` | Thrash confuses the user after it finishes | FAIL | PASS |
| `test/battle/move_effects_combined/axe_kick.c:19` | Axe Kick confuses the target | FAIL | PASS |
| `test/battle/move_effects_combined/flinch_status.c:14` | Thunder, Ice and Fire Fang inflict status 10% of the time | FAIL | PASS |
| `test/battle/move_effects_combined/toxic_thread.c:125` | Toxic Thread fails if speed can't be lowered and status can't be inflicted | FAIL | PASS |
| `test/battle/move_effects_combined/toxic_thread.c:61` | Toxic Thread still inflicts Poison if speed can't be lowered | FAIL | PASS |
| `test/battle/move_flags/damages_airborne_double_damage.c:21` | Being airborne causes the target to take double damage from certain moves | FAIL | PASS |
| `test/battle/move_flags/recoil.c:79` | Recoil: Flare Blitz deals 33% of recoil damage to the user and can burn target | FAIL | PASS |
| `test/battle/starting_status/hazards.c:172` | Starting Toxic Spikes badly poison the player's switch-in | FAIL | PASS |
| `test/battle/status1/burn.c:55` | Will-O-Wisp burns target | FAIL | PASS |
| `test/battle/status1/paralysis.c:133` | Thunder Wave prints failure when the target already has a different non-volatile status | FAIL | PASS |
| `test/battle/status1/paralysis.c:90` | Thunder Wave doesn't affect Electric types (Gen6+) | FAIL | PASS |
| `test/battle/status1/sleep.c:35` | Sleep: Spore affects grass types (Gen1-5) | FAIL | PASS |
| `test/battle/volatiles/confusion.c:33` | Confusion self hit does not consume Gems | INVALID | PASS |
| `test/battle/move_effect/retaliate.c:89` | Retaliate works with passive damage | INVALID | PASS |
| `test/battle/move_effect/sky_drop.c:247` | Sky Drop: If target was locked into a move that would confuse, the target will be freed and confusion occurs immediately (attacker faints due to status) | FAIL | PASS |
| `test/battle/move_effect/sleep.c:10` | Hypnosis inflicts 1-3 turns of sleep (Gen9-) | INVALID | PASS |
| `test/battle/move_effect/steel_roller.c:52` | Steel Roller will fail if there is no Terrain | FAIL | PASS |
| `test/battle/move_effect/storm_sun_fists.c:337` | Infernal Fists has a 30 percent burn chance on its first punch | FAIL | PASS |
| `test/battle/move_effect/swagger.c:21` | Swagger increases the target's Attack by 2 stages and confuses them | FAIL | PASS |
| `test/battle/move_effect/toxic.c:19` | Toxic inflicts bad poison | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:250` | Toxic Spikes print normal poison for 1 layer | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:203` | Toxic Spikes are not removed by Poison-type Pokémon affected by Magnet Rise on switch in | FAIL | PASS |
| `test/battle/move_effect/toxic_spikes.c:134` | Toxic Spikes do not poison airborne Pokemon | FAIL | PASS |
| `test/battle/move_effect_secondary/burn.c:123` | Scald should burn a Water-type Pokémon | FAIL | PASS |
| `test/battle/move_effect_secondary/burn.c:98` | Matcha Gatcha can burn both targets | FAIL | PASS |
| `test/battle/move_effect_secondary/burn.c:67` | Matcha Gotcha inflicts burn 20% of the time | FAIL | PASS |
| `test/battle/move_effect_secondary/dire_claw.c:9` | Dire Claw can inflict poison, paralysis or sleep | FAIL | PASS |
| `test/battle/move_effect_secondary/paralysis.c:83` | Body Slam should paralyze Normal-types | FAIL | PASS |
| `test/battle/move_effect_secondary/paralysis.c:59` | Thunder Shock cannot paralyze an Electric-type (Gen6+) | FAIL | PASS |
| `test/battle/move_effect_secondary/thrash.c:135` | Thrash confuses the user after it finishes even if move failed | FAIL | PASS |
| `test/battle/move_effect_secondary/thrash.c:85` | Thrash confuses the user if it is canceled on turn 3 of 3, Protect | FAIL | PASS |
| `test/battle/move_effect_secondary/tri_attack.c:12` | Tri Attack can inflict paralysis, burn or freeze | FAIL | PASS |
| `test/battle/move_effects_combined/relic_song.c:9` | Relic Song has a 10% chance to put the target to sleep | FAIL | PASS |
| `test/battle/starting_status/hazards.c:63` | Starting Toxic Spikes badly poison the opposing switch-in | FAIL | PASS |
