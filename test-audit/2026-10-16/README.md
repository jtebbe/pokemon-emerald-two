# Full test-suite crash investigation — 2026-10-16

## Result

The requested `make check -j4` run did not finish. Three emulator workers entered repeated crash/output loops after earlier timeouts, while the fourth completed its assigned tests. Each stuck worker exceeded the harness's 1 MiB per-test output limit. The run was stopped after confirming the workers were not progressing; its log is preserved as [full-suite.log](full-suite.log).

To finish the audit without inherited emulator state, every C source file under `test/` was filtered and run against the same unmodified headless test ELF in a fresh four-worker session (`MAKEFLAGS=-j4`). There were 972 files: 942 produced test results and 30 correctly reported “No tests found.” A 45-second host watchdog bounded each file. The computationally heavy `all_anims.c` suite exceeded that limit but then completed successfully with a 180-second allowance: all 11 animation tests passed.

| Final isolated-audit result | Count |
| --- | ---: |
| Passed | 5,362 |
| Failed | 86 |
| TODO placeholders | 354 |
| Failed assumptions / skipped | 7 |
| Known failing | 12 |
| Expected failing self-tests | 9 |
| Total | 5,830 |

The 86 failing test cases are in 53 files: **17 TIMEOUT, 64 FAIL, and 5 INVALID**. Two green CRASH results are intentional test-runner self-tests and are included in its passed count. There were **no unexpected CRASH results in the fresh per-file audit**. This does not invalidate the real crashes in the combined run: isolation removes the shared state/order that triggered them.

Counts refer to the harness's test cases, not every parameter/trial. A failing parameterized case may stop at its first failing parameter.

No game source, test source, or shipping ROM was changed during this investigation. Temporary diagnostic ELF copies were kept separate from the ordinary build. The partner-lookup probe described below is a causal experiment, not a production fix.

## Confirmed findings and recommended repair order

### 1. Single-battle intro partner-slot collision — first runtime fix

[GetPartnerBattler](C:/Users/jateb/Documents/pokeemerald-master/src/battle_util.c:105) calls [GetBattlerAtPosition](C:/Users/jateb/Documents/pokeemerald-master/src/battle_util.c:84), whose search stops at `gBattlersCount`. In a single battle there is no active partner; both sides therefore receive the same fallback battler index, 2.

The intro callbacks still read and clear that nonexistent partner's shiny-animation flags. See [test player intro](C:/Users/jateb/Documents/pokeemerald-master/test/battle_controller_player.c:123), [test opponent intro](C:/Users/jateb/Documents/pokeemerald-master/test/battle_controller_opponent.c:137), and [live player intro](C:/Users/jateb/Documents/pokeemerald-master/src/battle_controller_player.c:1170). Depending on animation timing, one side clears the shared fallback slot before the other finishes waiting. The Aerilate failures stop at turn zero in `Intro_WaitForShinyAnimAndHealthbox`.

Causal check: a temporary copied test ELF used separate physical partner slots for this isolated singles experiment. All eight Aerilate intro timeouts disappeared: the same file went from 2 passed / 9 failed / 1 assumption failure to 10 passed / 1 failed / 1 assumption failure. The remaining failure is an unrelated Electrify animation expectation.

Production repair should avoid waiting on or clearing an absent partner, including in one-versus-two and partner battles, and cover both live and test controllers. Do not blindly ship the probe's XOR mapping; generic battler routing must still respect positions.

### 2. Timeout/crash recovery allows cascading failures — first harness fix

The combined run logged three unexpected CRASH results, then stuck in repeated output loops. Aftermath's opponent-view case crashed there, but all three Aftermath cases pass in a fresh emulator. Analytic also passes in isolation despite being one of the later flooded/stuck tests in the combined run.

[JumpToAgbMainLoop](C:/Users/jateb/Documents/pokeemerald-master/test/test_runner.c:710) explicitly warns that timeout recovery leaves other global state uncleared. [ResetGlobalVariables](C:/Users/jateb/Documents/pokeemerald-master/test/test_runner.c:568) only restores callbacks and clears battle flags. [TearDownBattle](C:/Users/jateb/Documents/pokeemerald-master/test/test_runner_battle.c:2065) frees resources but is not a full reset of every sprite, task, UI, interrupt, and pointer state left by an interrupted battle.

This confirms order-dependent/cascading failures; the exact first invalid pointer in each flooded crash still needs instrumentation. Prefer clean emulator restart/recovery after fatal timeouts/crashes, or a verified complete teardown before resuming. Increasing the timeout alone will not fix this.

### 3. Capture tests stop at a real input screen — test-driver automation missing

Nine of the 17 isolated timeouts are capture-related: capture itself, Comatose/Dream Ball, Power Construct, capture experience, item theft, and wild held-item transfer.

The timeout dumps show `Task_HandleCaughtMonPageInput` still active. [That task](C:/Users/jateb/Documents/pokeemerald-master/src/pokedex.c:4081) explicitly waits for A/B, while [Cmd_displaydexinfo](C:/Users/jateb/Documents/pokeemerald-master/src/battle_script_commands.c:6153) waits for the task to finish. The headless fixture/controller path does not supply the required input.

Add deterministic test-only handling for caught-dex/nickname/other capture UI, then recheck the actual party/PC transfer and autosave path. The current stop occurs before those later paths can be validated; it is not evidence that flash saving is the cause of these particular timeouts.

### 4. Explicit test stats are overwritten by recalculation

All three failures in `test_runner_features.c` reproduce independently: level changes, non-HP form changes, and HP form changes overwrite stats that the test explicitly set.

[Level_](C:/Users/jateb/Documents/pokeemerald-master/test/test_runner_battle.c:2549) calls stat calculation and still has a TODO to preserve explicit stats. [CalculateMonStatsCont](C:/Users/jateb/Documents/pokeemerald-master/src/pokemon.c:845) calculates the new direct-layout/max-31 stats but no longer preserves these synthetic fixture overrides; its `updateSpeedStat` parameter is also unused.

Restore the test-only override contract so scenario setup and speed ordering remain controlled. Keep real game stats at the requested fixed maximum contribution; do not reintroduce stored IVs.

### 5. Stale stat and PP expectations after the intentional mechanics changes

Several failures demonstrably expect old values. For example, [Battle Bond](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/battle_bond.c:52) expects attack 295 while the fixed-31 result is 326, and Regenerator has another exact 31-point discrepancy. Other tests rely on specific damage, KO, berry, or half-HP thresholds that changed with the default stats. Not every mismatch has been individually proven to be a stale expectation, so update fixtures and recompute independent expected results rather than merely accepting observed output.

There are also clear leftover base-PP assertions despite assumptions already being updated to maximum PP. [Copycat](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/copycat.c:25) assumes Pound has 56 PP but expects 34 after one use; the actual result is 55. Me First, Sleep Talk, and Lunar Dance have equivalent obsolete expectations. Zacian/Zamazenta end-of-battle tests expect Behemoth's 8 PP after reverting to Iron Head, whose maximum is now 24.

### 6. Scripted trainer-message fixtures no longer match the naming path

Five battle-message cases fail with strings such as `You are challenged by ???TRAINER?2!?` instead of the synthetic trainer name `2`. The fixture now uses `TRAINER_LINK_OPPONENT` without the old recorded-battle path. [Trainer-class expansion](C:/Users/jateb/Documents/pokeemerald-master/src/battle_message.c:2626) supplies the placeholder class rather than the empty synthetic class that those tests assumed.

Repair the scripted-test trainer/class handling without changing actual trainer or multiplayer names. Recheck both one- and two-opponent messages.

### 7. Cross-test party state leaks into the save test

“Saved party rebuilds cached stats while preserving HP, status, moves and mail” failed only in the combined run: slot 1 contained species 1084 instead of NONE. It passes in isolation, as do all nine save tests.

[The test](C:/Users/jateb/Documents/pokeemerald-master/test/save.c:105) creates slot 0 and sets the party count to one, but does not clear the other party slots. [SavePlayerParty](C:/Users/jateb/Documents/pokeemerald-master/src/load_save.c:121) serializes all six slots, so data left by preceding function tests can be preserved. Establish a clean party fixture/reset contract; do not misclassify this observation as PC journal corruption.

## Scope and remaining uncertainty

All PC storage, save, link-safety, daycare, fusion-storage, Pokémon, and script suites passed independently. This is emulator/unit coverage, not hardware or two-device cable/RFU verification.

There are still individual AI, move-order, rounding, item, and effect failures that need case-by-case diagnosis after the shared defects and fixture contracts above are repaired. The complete list below is intentionally retained; it is not safe to declare all 64 assertion/scene failures stale.

The seven assumption failures and 354 TODOs are distinct from the 86 failures. At least some Hidden Power assumptions still expect the former typing/behavior.

## Failing files

| File | Failing cases |
| --- | ---: |
| [test/battle/ability/aerilate.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/aerilate.c) | 9 |
| [test/battle/ability/battle_bond.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/battle_bond.c) | 1 |
| [test/battle/ability/berserk.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/berserk.c) | 1 |
| [test/battle/ability/comatose.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/comatose.c) | 1 |
| [test/battle/ability/contrary.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/contrary.c) | 1 |
| [test/battle/ability/forecast.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/forecast.c) | 1 |
| [test/battle/ability/gulp_missile.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/gulp_missile.c) | 1 |
| [test/battle/ability/liquid_ooze.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/liquid_ooze.c) | 1 |
| [test/battle/ability/mega_sol.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/mega_sol.c) | 1 |
| [test/battle/ability/pickpocket.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/pickpocket.c) | 1 |
| [test/battle/ability/pickup.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/pickup.c) | 2 |
| [test/battle/ability/power_construct.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/power_construct.c) | 1 |
| [test/battle/ability/protosynthesis.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/protosynthesis.c) | 1 |
| [test/battle/ability/regenerator.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/regenerator.c) | 1 |
| [test/battle/ability/schooling.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/schooling.c) | 1 |
| [test/battle/ability/truant.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ability/truant.c) | 1 |
| [test/battle/ai/ai_combo_attack.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/ai/ai_combo_attack.c) | 1 |
| [test/battle/battle_message.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/battle_message.c) | 5 |
| [test/battle/capture.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/capture.c) | 3 |
| [test/battle/damage_formula.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/damage_formula.c) | 7 |
| [test/battle/exp.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/exp.c) | 1 |
| [test/battle/form_change/end_battle.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/form_change/end_battle.c) | 2 |
| [test/battle/hold_effect/adamant_orb.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/hold_effect/adamant_orb.c) | 1 |
| [test/battle/hold_effect/lustrous_orb.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/hold_effect/lustrous_orb.c) | 1 |
| [test/battle/hold_effect/red_card.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/hold_effect/red_card.c) | 1 |
| [test/battle/hold_effect/restore_hp.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/hold_effect/restore_hp.c) | 1 |
| [test/battle/move_effect/after_you.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/after_you.c) | 1 |
| [test/battle/move_effect/assurance.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/assurance.c) | 3 |
| [test/battle/move_effect/belly_drum.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/belly_drum.c) | 2 |
| [test/battle/move_effect/brine.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/brine.c) | 1 |
| [test/battle/move_effect/copycat.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/copycat.c) | 1 |
| [test/battle/move_effect/curse.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/curse.c) | 1 |
| [test/battle/move_effect/echoed_voice.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/echoed_voice.c) | 1 |
| [test/battle/move_effect/fillet_away.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/fillet_away.c) | 1 |
| [test/battle/move_effect/jungle_healing.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/jungle_healing.c) | 1 |
| [test/battle/move_effect/last_respects.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/last_respects.c) | 2 |
| [test/battle/move_effect/life_dew.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/life_dew.c) | 1 |
| [test/battle/move_effect/low_kick.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/low_kick.c) | 1 |
| [test/battle/move_effect/lunar_dance.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/lunar_dance.c) | 3 |
| [test/battle/move_effect/me_first.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/me_first.c) | 1 |
| [test/battle/move_effect/mud_sport.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/mud_sport.c) | 1 |
| [test/battle/move_effect/multi_hit.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/multi_hit.c) | 1 |
| [test/battle/move_effect/pursuit.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/pursuit.c) | 1 |
| [test/battle/move_effect/quash.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/quash.c) | 1 |
| [test/battle/move_effect/sleep_talk.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/sleep_talk.c) | 1 |
| [test/battle/move_effect/steal_item.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/steal_item.c) | 2 |
| [test/battle/move_effect/strength_sap.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effect/strength_sap.c) | 1 |
| [test/battle/move_effects_combined/axe_kick.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/move_effects_combined/axe_kick.c) | 3 |
| [test/battle/sleep_clause.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/sleep_clause.c) | 1 |
| [test/battle/status1/frostbite.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/status1/frostbite.c) | 1 |
| [test/battle/test_runner_features.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/test_runner_features.c) | 3 |
| [test/battle/weather/strong_winds.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/weather/strong_winds.c) | 1 |
| [test/battle/wild_battle.c](C:/Users/jateb/Documents/pokeemerald-master/test/battle/wild_battle.c) | 1 |

## Every independently failing case

| Result | File | Test |
| --- | --- | --- |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate doesn't change Tera Blast's type when Terastallized |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate changes Tera Blast's type when not Terastallized |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate doesn't affect Terrain Pulse's type |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate doesn't affect Natural Gift's type 1/2 |
| FAIL | `test/battle/ability/aerilate.c` | Aerilate doesn't override Electrify |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate boosts power of affected moves by 20% (Gen7+) or 30% (Gen1-6) 1/4 |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate doesn't affect Judgment / Techno Blast / Multi-Attack's type 1/3 |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate can not turn certain moves into Flying type moves 1/6 |
| TIMEOUT | `test/battle/ability/aerilate.c` | Aerilate turns a Normal-type move into Flying-type move |
| FAIL | `test/battle/ability/battle_bond.c` | Battle Bond transforms player's Greninja - Singles 4/4 |
| FAIL | `test/battle/ability/berserk.c` | Berserk will not activate if the last multi hit move activates a restore berry |
| TIMEOUT | `test/battle/ability/comatose.c` | Comatose boosts Dream Ball's multiplier 1/2 |
| FAIL | `test/battle/ability/contrary.c` | Contrary raises a stat after using a move which would normally lower it: Growl 2/2 |
| FAIL | `test/battle/ability/forecast.c` | Forecast: All Forecast mons revert on the field if Primal Weather is removed due to fainting |
| INVALID | `test/battle/ability/gulp_missile.c` | Gulp Missile: triggers even if the user is fainted by opposing mon |
| INVALID | `test/battle/ability/liquid_ooze.c` | Liquid Ooze causes Strength Sap users to lose HP instead of heal 2/2 |
| FAIL | `test/battle/ability/mega_sol.c` | Synthesis, Morning Sun and Moonlight recover 2/3 of the user's max HP if user has Mega Sol 1/3 |
| FAIL | `test/battle/ability/pickpocket.c` | Pickpocket activates when user has Protective Pads, but not with Punching Glove or Long Reach 1/3 |
| FAIL | `test/battle/ability/pickup.c` | Pickup doesn't grant an used item if its user already restored it |
| FAIL | `test/battle/ability/pickup.c` | Pickup grants an item used by itself in wild battles (Gen9+) |
| TIMEOUT | `test/battle/ability/power_construct.c` | Power Construct Zygarde reverts to its original form upon catching 1/2 |
| FAIL | `test/battle/ability/protosynthesis.c` | Protosynthesis damage calculation is correct 1/16 |
| FAIL | `test/battle/ability/regenerator.c` | Regenerator heals the user and not a fainted party member when switching out with a hit escape move |
| FAIL | `test/battle/ability/schooling.c` | Schooling switches Level 20+ Wishiwashi's form when HP is 25-percent or less at the end of the turn 2/2 |
| FAIL | `test/battle/ability/truant.c` | Gaining Truant after Mega Evolution causes an immediate loafing turn |
| FAIL | `test/battle/ai/ai_combo_attack.c` | Combo Attack: Fusion moves are only incentivised when partners are adjacent in turn order 2/2 |
| FAIL | `test/battle/battle_message.c` | Battle Message: STRINGID_INTROMSG, STRINGID_INTROSENDOUT, STRINGID_RETURNMON, and STRINGID_SWITCHINMON (1v2) |
| FAIL | `test/battle/battle_message.c` | Battle Message: STRINGID_INTROMSG, STRINGID_INTROSENDOUT, STRINGID_RETURNMON, and STRINGID_SWITCHINMON (2v1) |
| FAIL | `test/battle/battle_message.c` | Battle Message: STRINGID_INTROMSG, STRINGID_INTROSENDOUT, STRINGID_RETURNMON, and STRINGID_SWITCHINMON (Doubles) |
| FAIL | `test/battle/battle_message.c` | Battle Message: STRINGID_INTROMSG, STRINGID_INTROSENDOUT, STRINGID_RETURNMON, and STRINGID_SWITCHINMON (Multi) |
| FAIL | `test/battle/battle_message.c` | Battle Message: STRINGID_INTROMSG, STRINGID_INTROSENDOUT, STRINGID_RETURNMON, and STRINGID_SWITCHINMON (Singles) |
| TIMEOUT | `test/battle/capture.c` | Capture: ball data is properly set in captured pokemon 1/28 |
| TIMEOUT | `test/battle/capture.c` | Capture: when CRITICAL_CAPTURE_IF_OWNED is enabled, capture of owned pokemon always appear critical 1/6 |
| TIMEOUT | `test/battle/capture.c` | Capture: when CRITICAL_CAPTURE_IF_OWNED is enabled, failed capture of owned pokemon does not appear critical 1/2 |
| FAIL | `test/battle/damage_formula.c` | Gem boosted Damage calculation 1/16 |
| FAIL | `test/battle/damage_formula.c` | Punching Glove vs Muscle Band Damage calculation 1/16 |
| FAIL | `test/battle/damage_formula.c` | Transistor Damage calculation 1/32 |
| FAIL | `test/battle/damage_formula.c` | A spread move will do correct damage to the second mon if the first target faints from first hit of the spread move (2v1) |
| FAIL | `test/battle/damage_formula.c` | A spread move will do correct damage to the second mon if the first target faints from first hit of the spread move (multibattle) |
| FAIL | `test/battle/damage_formula.c` | A spread move will do correct damage to the second mon if the first target faints from first hit of the spread move (1v2) |
| FAIL | `test/battle/damage_formula.c` | A spread move will do correct damage to the second mon if the first target faints from first hit of the spread move (double battle) |
| TIMEOUT | `test/battle/exp.c` | Pokemon gain experience after catching a Pokemon (Gen6+) 1/3 |
| FAIL | `test/battle/form_change/end_battle.c` | Zamazenta returns its Hero Form upon battle end |
| FAIL | `test/battle/form_change/end_battle.c` | Zacian returns its Hero Form upon battle end |
| FAIL | `test/battle/hold_effect/adamant_orb.c` | Adamant Orb boosts Dialga's Dragon- and Steel-type move damage by 20% 4/4 |
| FAIL | `test/battle/hold_effect/lustrous_orb.c` | Lustrous Orb boosts Palkia's Dragon- and Water-type move damage by 20% 4/4 |
| INVALID | `test/battle/hold_effect/red_card.c` | Red Card is consumed after dragged out replacement has its Speed lowered by Sticky Web |
| FAIL | `test/battle/hold_effect/restore_hp.c` | Sitrus Berry restores HP before Shields Down form change |
| FAIL | `test/battle/move_effect/after_you.c` | After You calculates correct turn order if only one Pokémon is left on the opposing side |
| FAIL | `test/battle/move_effect/assurance.c` | Assurance doubles in power if the target has been damaged by Liquid Ooze |
| FAIL | `test/battle/move_effect/assurance.c` | Assurance doubles in power if Ice Face has been broken by attacker |
| FAIL | `test/battle/move_effect/assurance.c` | Assurance doubles in power if the target has been damaged by Rough Skin |
| FAIL | `test/battle/move_effect/belly_drum.c` | Belly Drum deducts HP if the user has Contrary and is at -6 |
| FAIL | `test/battle/move_effect/belly_drum.c` | Belly Drum cuts the user's HP in half |
| FAIL | `test/battle/move_effect/brine.c` | Brine's power doubles if the target is at 50% or below max HP 2/2 |
| FAIL | `test/battle/move_effect/copycat.c` | Copycat deducts power points from itself, not the copied move |
| FAIL | `test/battle/move_effect/curse.c` | Curse cuts the user's HP in half when used by Ghost-types |
| FAIL | `test/battle/move_effect/echoed_voice.c` | Echoed Voice's power is multiplied for every consecutive turn used, capped at 5 |
| FAIL | `test/battle/move_effect/fillet_away.c` | Fillet Away cuts the user's HP in half |
| FAIL | `test/battle/move_effect/jungle_healing.c` | Jungle Healing recovers 25% of hp and heals their non volatile for both user and partner |
| FAIL | `test/battle/move_effect/last_respects.c` | Last Respects power is multiplied by the amount of fainted mon in the user's side - Player 2/3 |
| FAIL | `test/battle/move_effect/last_respects.c` | Last Respects power is multiplied by the amount of fainted mon in the user's side - Opponent 2/3 |
| FAIL | `test/battle/move_effect/life_dew.c` | Life Dew recovers 25% of hp for both user and partner |
| FAIL | `test/battle/move_effect/low_kick.c` | Low Kick's damage varies based on the target's weight 4/6 |
| FAIL | `test/battle/move_effect/lunar_dance.c` | Lunar Dance causes the user to faint and heals the replacement's HP, PP and status (singles) |
| FAIL | `test/battle/move_effect/lunar_dance.c` | Lunar Dance causes the user to faint and heals the replacement's HP, PP and status (doubles) |
| FAIL | `test/battle/move_effect/lunar_dance.c` | Lunar Dance effect activates only if the switched Pokémon can be healed (Gen8+) 3/3 |
| FAIL | `test/battle/move_effect/me_first.c` | Me First deducts power points from itself, not the copied move |
| FAIL | `test/battle/move_effect/mud_sport.c` | Mud Sport reduces the damage of Electric Type moves by 50% (Gen3-4) or 67% (Gen5+) 2/2 |
| FAIL | `test/battle/move_effect/multi_hit.c` | Scale Shot decreases defense and increases speed after killing opposing with less then 4 hits 1/2 |
| FAIL | `test/battle/move_effect/pursuit.c` | Pursuit doubles in power if attacking while target switches out 2/2 |
| FAIL | `test/battle/move_effect/quash.c` | Quash calculates correct turn order if only one Pokémon is left on the opposing side |
| FAIL | `test/battle/move_effect/sleep_talk.c` | Sleep Talk deducts power points from itself, not the called move |
| TIMEOUT | `test/battle/move_effect/steal_item.c` | Thief and Covet steal target's held item but it's restored if the mon is caught (Gen 9+) 1/2 |
| TIMEOUT | `test/battle/move_effect/steal_item.c` | Thief and Covet steal target's held item but it's restored if the mon is caught (Gen 9+) 1/2 |
| INVALID | `test/battle/move_effect/strength_sap.c` | Strength Sap will drain users HP if target has Liquid Ooze 2/2 |
| FAIL | `test/battle/move_effects_combined/axe_kick.c` | Axe Kick deals damage half the hp to user if def battler protected |
| FAIL | `test/battle/move_effects_combined/axe_kick.c` | Axe Kick deals damage half the hp to user if it fails |
| FAIL | `test/battle/move_effects_combined/axe_kick.c` | Axe Kick still deals crash damage when boosted by Sheer Force 1/2 |
| INVALID | `test/battle/sleep_clause.c` | Sleep Clause: Sleep clause is deactivated when a sleeping mon is woken up forcefully by Uproar |
| FAIL | `test/battle/status1/frostbite.c` | Frostbite reduces the special attack by 50 percent |
| FAIL | `test/battle/test_runner_features.c` | Changing forms doesn't overwrite set stats (HP) |
| FAIL | `test/battle/test_runner_features.c` | Setting level doesn't overwrite set stats 1/4 |
| FAIL | `test/battle/test_runner_features.c` | Changing forms doesn't overwrite set stats (not HP) |
| FAIL | `test/battle/weather/strong_winds.c` | Strong winds don't reduce Synthesis, Morning Sun or Moonlight recovery 1/3 |
| TIMEOUT | `test/battle/wild_battle.c` | An item given by the player to the wild mon is not duplicated if the mon is caught |

## Evidence

- [Combined-run log](full-suite.log): failed/hung `make check -j4` run.
- [Machine-readable isolated audit](isolated-summary.json): exact totals, files, and failures.
- [Raw logs archive](logs.tar.gz): all 972 per-file logs, extended animation success, clean Aftermath reproduction, and the separate partner-slot probe. The first all-animation log ends at the short host watchdog; use `animations-extended.log` for its final passing result.

