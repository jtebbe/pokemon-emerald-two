# Apex abilities

Every species and form has its own `SpeciesInfo.apexAbility`. Omitted initializers
default to `ABILITY_NONE`; forms do not inherit this field from their base species.
Assign an Apex ability alongside the normal slots:

```c
.abilities = { ABILITY_PRESSURE, ABILITY_NONE, ABILITY_TELEPATHY },
.apexAbility = ABILITY_TIME_DOMINUS,
```

Holding `ITEM_APEX_METEORITE` replaces all normal ability slots with this field,
including `ABILITY_NONE` for species whose Apex ability has not been assigned.
Taking the item off outside battle restores the normal ability slot. This does
not add any persistent data to individual Pokémon or change their save format.

The Meteorite uses the original Meteorite graphic and palette. It can be given,
taken, and reassigned, but cannot be sold, tossed, consumed, stolen, swapped,
destroyed, or removed during battle. Remove it before releasing or trading its
holder. It remains active through Klutz, Embargo, and Magic Room.

An active Apex ability cannot be copied, swapped, overwritten, suppressed, or
ignored by Mold Breaker-style effects. Transform still copies the target's
appearance and battle data, but does not copy an Apex ability or replace the
transforming Pokémon's own Apex ability.

On entry, a holder displays the held-item popup and announces that it is
unleashing its potential before entry abilities activate. This happens once per
Pokémon's party slot per battle, including unassigned Apex abilities. The reveal
records both the item and actual Apex ability for the AI without requiring
omniscience. Switching out and returning does not repeat the announcement.

## Blazing Glory

Charmander, Charmeleon, Charizard, and both Mega Charizard forms have
`ABILITY_BLAZING_GLORY`. At the late end-of-turn ability step, each living holder
loses one sixth of its maximum HP (rounded down, minimum one). Surviving holders
gain one Attack, Sp. Atk, and Speed stage, capped normally at +6. The HP cost still
applies when all three stats are capped and cannot be blocked by Protect or a
Substitute. It also activates on a switch-in turn, but never on the bench.

The ability popup is followed by the HP loss and "[Pokémon] is burning up!" If
the cost instead causes a faint, it announces "[Pokémon] burned too bright!"
before the usual faint message and does not grant boosts. Other residual damage
that faints the holder earlier does not activate Blazing Glory.

AI residual damage and switch-in forecasts include the cost. Protect scoring
values a surviving holder's useful boosts, without encouraging a lethal stall.
Tests are in `test/battle/ability/blazing_glory.c` and
`test/battle/ai/blazing_glory.c`.

## Rootbound

Bulbasaur, Ivysaur, Venusaur, and Mega Venusaur have `ABILITY_ROOTBOUND`. Each entry
raises Defense and Sp. Def by one stage and applies ordinary Ingrain, including
its healing, grounding, switching restrictions, and phazing immunity. The entry
uses the ability popup, stat-boost animation, and Ingrain animation/message.
Existing roots do not prevent the defensive boosts on a later entry.

Growth raises Attack and Sp. Atk by two stages normally, or four in effective
sunlight, still capped at +6. Other stat changes and other Pokémon's Growth are
unaffected. Weather suppression follows Growth's normal rules. AI setup scoring
and switch-in damage prediction include these effects; defensive entry boosts
are applied once even in doubles. Regression coverage is in
`test/battle/ability/rootbound.c` and `test/battle/ai/rootbound.c`.

## Ilex Whimsy

Celebi's Apex ability is `ABILITY_ILEX_WHIMSY`. On entry it sets its side's Mist,
Safeguard, and Lucky Chant for five turns, with the Lucky Chant animation and
"[Pokémon] is shielding its side!" message. Existing active protections retain
their timers; only missing protections are added. If all three are active,
nothing happens. It can activate again on a later entry after protections end
or are removed. The usual removal, expiry, and Infiltrator rules still apply.

AI move scoring recognizes these protections, avoids redundant setup and blocked
status/stat-drop moves, and includes Lucky Chant in critical-hit damage prediction.
Switch-in prediction simulates Ilex Whimsy after entry hazards and restores the
real field afterward. The ability does not retroactively block Toxic Spikes.

## Distortion

Both Giratina forms have `ABILITY_DISTORTION`. While at least one living, on-field
holder is active, damaging moves use type-based categories instead of the modern
per-move physical/special split. This is a reversion, not an inversion:

- Normal, Fighting, Flying, Poison, Ground, Rock, Bug, Ghost, and Steel are physical.
- Fire, Water, Grass, Electric, Ice, Psychic, Dragon, Dark, Fairy, and Stellar are special.
- Status moves remain status moves. Types, contact flags, and additional effects
  are not changed.

The move's resolved battle type determines its category, including type-changing
moves such as Weather Ball. Type-based categories also take priority over
stat-adaptive category selection. AI damage, setup and screen evaluation, battle
category displays, damage tracking, and category-sensitive effects share these
rules. A resolved hit retains its category for damage tracking and reactions if
it KOs the last holder; subsequent hits/actions use the current field state.
AI damage calculations cache aura presence only within one simulation and
restore the previous cache afterward, so switch candidates and later field
changes are always evaluated afresh.

It applies to both sides, does not stack, and ends when the last holder faints or
leaves the field. Bench Pokémon and Pokémon outside battle do not activate it.
Tests are in `test/battle/ability/distortion.c` and `test/battle/ai/distortion.c`.

## Time Dominus

Both Dialga forms have `ABILITY_TIME_DOMINUS` as their Apex ability.

- Its holder's move-created weather, terrain, rooms, screens, Mist, Safeguard,
  Tailwind, Lucky Chant, Gravity, Water/Mud Sport, Fairy Lock, and combined Pledge
  side effects start with 255 turns. The usual end-of-turn countdown applies.
- Existing endless effects stay endless, including Storm Sacrifice. Rooms can
  still be toggled off and effects can still be cleared or replaced normally.
- Opponents' timers, sleep, confusion, trapping, Taunt, Perish Song, delayed
  attacks, and one-turn action effects are not extended.
- Charge moves execute in one turn without consuming a Power Herb. Charge boosts
  such as Meteor Beam's and Geomancy's still apply. Sky Drop deals its damage
  immediately rather than carrying its target for a turn. Skipping a charge turn
  announces "[Pokémon] compressed time!"; skipping recharge does not.
- Recharge moves do not create a recharge turn.

AI ability resolution and charge/recharge evaluation recognize the Meteorite
and Time Dominus. Beneficial long-duration weather, terrain, screens, Tailwind,
Trick Room, Safeguard, and Lucky Chant receive additional setup value, without
encouraging already-active or unfavorable effects. Regression tests are in
`test/apex_abilities.c`, `test/battle/ability/apex_abilities.c`, and
`test/battle/ability/ilex_whimsy.c`, with AI cases in
`test/battle/ai/apex_abilities.c`.

## Space Dominus

Both Palkia forms have `ABILITY_SPACE_DOMINUS`. Its damaging attacks skip accuracy
checks, including accuracy/evasion stages, but do not gain No Guard's ability to
hit semi-invulnerable targets. Status moves retain normal accuracy. OHKO moves
still obey level restrictions and Sturdy.

Moves with an intrinsic increased critical-hit stage bypass personal and side
protection fully, without shield contact penalties. Focus Energy, items, and
whether a particular hit actually crits do not affect this check. The ability
does not clear the target's protection against other attackers.

## Omnipotence

Every Arceus form has `ABILITY_OMNIPOTENCE`. Its damaging moves' combined type
effectiveness is floored at 1×, retaining super-effective bonuses. This bypasses
type-chart immunities/resistances, not Levitate, Wonder Guard, absorption
abilities, Air Balloon, Magnet Rise, or move-specific failure requirements.
Status-move immunities and normal accuracy remain unchanged.
Defensive ability resistance, such as Tera Shell, also remains effective.

Moves with effective priority zero or lower bypass personal and side protection
fully, without shield contact penalties. Positive-priority attacks remain
blocked, including conditional priority such as Grassy Glide in Grassy Terrain.

Both abilities use shared battle/AI calculations for accuracy, damage and Protect
evaluation. Regression cases are in `test/battle/ability/space_omnipotence.c` and
`test/battle/ai/space_omnipotence.c`.
