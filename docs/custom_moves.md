# Custom moves

These moves use the usual battle rules for Protect, immunities, abilities,
items and stat stages unless described below. PP values are the base values;
this project always grants maximum PP (8 for a 5-PP move, 16 for a 10-PP move,
32 for a 20-PP move).

| Move | Type/category | Power / accuracy / base PP | Effect |
| --- | --- | --- | --- |
| Serrated Slice | Steel / physical | 80 / 100 / 10 | Slicing, +1 critical-hit stage. Hitting a Steel target adds one Spikes layer to its side, up to three. |
| Dazzling Dash | Fairy / physical | 60 / always hits / 20 | Contact. A faster user gets a guaranteed critical hit; a slower user gains one Speed stage after hitting. A tie gives neither bonus. |
| Corrode | Poison / special | 40 / 100 / 20 | Lowers Defense and Sp. Def by one each. Poison's matchup against Steel is 2x instead of immune; the other defensive type still applies normally. Removes the target's Steel typing until switching out. |
| Mind Crush | Psychic / special | 80 / 100 / 10 | Uses the user's Sp. Def and its stages instead of Sp. Atk, while still attacking the target's Sp. Def. |
| Haunting Shrieks | Ghost / special | 30 per hit / 100 / 10 | Sound, bypasses Substitute. One hit from the user, then one per fainted non-egg teammate in party order. |
| Voltaic Rush | Electric / physical | 70 / 100 / 10 | 30% paralysis chance. Base power doubles against an already-paralyzed target. |
| Storm Sacrifice | Water / status | — / self / 5 | Pays 1/4 max HP for permanent rain and Electric Terrain. Can faint the user. |
| Sunbloom | Grass / special | 60 / 100 / 5 | Sets sun for 5 turns (8 with Heat Rock), or gains 1.5x base power if sun is already active without refreshing it. |
| Infernal Fists | Fire / physical | 20 per hit / 100 / 5 | Three contact punches, each with 30% burn chance. Guaranteed critical hits against already-burned targets. |

Storm Sacrifice rounds its HP payment down, to a minimum of 1 HP; Magic Guard
does not prevent this payment. Existing rain and Electric Terrain become
permanent too. Each persists independently through switching and fainting until
replaced or removed. Normal primal-weather, locked overworld-weather and Sky
Battle restrictions still apply; a blocked use does not charge HP.

Sunbloom sets sun after a successful hit, including hits on substitutes and
knockouts, but not misses, protection or immunity. Suppressed sun does not boost
its power. Infernal Fists rechecks burns before each hit, so a burn from one
punch guarantees crits on later punches. Critical-hit immunity still applies.

Dazzling Dash compares actual modified Speed, not turn order; Trick Room and
priority do not reverse its condition. Its Speed bonus uses the comparison
before damage, so effects such as Weak Armor do not change that decision.
Battle Armor, Shell Armor and Lucky Chant still prevent its critical hits.

Haunting Shrieks stays Ghost-type throughout. Later hits use that teammate's
stored Sp. Atk and get ordinary Ghost STAB only if that teammate has a Ghost
type. They do not borrow the active user's Sp. Atk stages or STAB/tera bonus.
The user remains the attacker for level, items and other battle modifiers.
Living teammates, eggs and empty party slots do not contribute. The volley
stops if the user or target faints, and does not gain Parental Bond hits.

Haunting Shrieks holds the dark background across its Hyper Voice animations
and plays each contributor's cry. The background is restored after the volley,
including an early knockout. The other custom animations reuse Metal Claw and
Slash, Trailblaze with sparkles, Meditate and Psychic, and Spark with Quick
Attack movement respectively.

Storm Sacrifice combines Hyper Beam's opening shake with Rain Dance under the
thunder background, then restores it. Sunbloom uses a brief Solar Beam volley
with shortened audio followed by Sunny Day. Infernal Fists uses Fire Punch for
each hit.

Corrode replaces Acid in slot 51, retaining that move's animation and replacing
it in all generation learnsets, egg moves, trainer inputs and the learnability
catalogue. Historical learnset import data is normalized by the import helper.
The other custom moves have new IDs; their species learnset distribution is not
assigned yet. They are available to the debug move editor, battle tests and
modern Metronome rolls (except Dark Imitation, which is excluded to prevent
nested move-calling).

Regression tests are in `test/battle/move_effect/custom_moves.c`,
`test/battle/move_effect/storm_sun_fists.c`,
`test/battle/ai/custom_moves.c` and `test/battle/move_animations/custom_moves.c`.
The animation tests execute the scripts from both sides and check resource
cleanup, as well as complete and interrupted Haunting Shrieks volleys.

AI-specific regression coverage in `test/battle/ai/previous_custom_moves.c`
checks all six original custom moves' damage estimates and doubles targeting.
Serrated Slice gets hazard credit only against Steel with room for another
Spikes layer; Shield Dust does not stop this primary effect. Dazzling Dash
accounts for modified Speed, critical-hit immunity, Contrary and Trick Room.
Corrode values useful Steel removal independently of its blockable stat drops,
considers the partner's attacks and restores all speculative typing changes.
Mind Crush values Sp. Def setup rather than Sp. Atk setup. Haunting Shrieks
uses its actual contributors and recognizes sound/type immunity and multi-hit
knockouts through Focus Sash. Voltaic Rush recognizes existing paralysis,
Electric immunity and the value of paralysis setup for its partner.

## Claws, Cuts and conditional priority

- Frigid Claws: Ice, physical, 85 power, 100 accuracy, 10 base PP; contact and
  slicing. Its 20% secondary effect always inflicts frostbite, regardless of the
  configurable freeze/frostbite setting. Uses Slash followed by the ice effect.
- Creeping Cuts: Bug, physical, 40 power per hit, 100 accuracy, 10 base PP;
  contact and slicing. Always hits twice. The Fairy matchup is 2x instead of
  0.5x; other types still apply normally. Uses Fury Cutter.
- Aura Pulse: Fighting, special, 50 power, 20 base PP; pulse move. Bypasses
  accuracy and evasion, but not Protect or immunities. Uses a smaller Aura
  Sphere projectile with shorter travel, windup and sound.
- Fierce Rushdown: Dragon, physical, 90 power, 100 accuracy, 10 base PP;
  contact. Gains +1 priority against targets at or below half their maximum
  HP (without rounding odd maximum HP upward). Uses Dragon Rush.
- Dark Imitation: Dark, status, 100 accuracy, 5 base PP. Executes the target's
  selected move for this turn once, even if the target has already acted.
  Its own accuracy check must succeed before the copied move is executed.
  Can copy status moves, including self-targeting moves; no permanent moveset
  change, copied-slot PP deduction or Me First damage bonus. Gains +1 priority
  against targets below full HP. Fails against switching targets and unsafe or
  recursive move callers. Uses Mimic under a temporary dark background.
- Simplify: Normal, status, 100 accuracy, 5 base PP. Changes all target type
  slots to pure Normal, including an added third type. Fails on Terastallized
  or already pure Normal targets. Unlike Soak, it can affect Multitype and RKS
  System. Gains +1 priority against two or more distinct current types. Uses
  Simple Beam and can be reflected by Magic Coat/Magic Bounce.

As with all moves in this project, actual maximum PP is always PP-Maxed:
10 base PP becomes 16, 20 becomes 32, and 5 becomes 8.

Target-dependent priority is shared by turn ordering, priority blockers and AI
speed, damage, switching and terrain calculations. Prankster stacks with the
conditional bonus; Quash overrides it. The AI uses the predicted/last known
move for Dark Imitation rather than reading the player's actual selection.
Simplify scores changes to type effectiveness and STAB instead of inheriting
Soak's Electric/Grass preferences. Regression coverage is in
`test/battle/move_effect/claws_cuts_priority.c` and
`test/battle/ai/claws_cuts_priority.c`, plus the shared animation tests.

## Mud Bath, Gemshot and Harsh Zephyr

- Mud Bath: Ground, status, 5 base PP. Replaces the user's secondary type with
  Ground while retaining its primary and any added third type. Restores one
  third of maximum HP (rounded down, at least 1 HP), capped at maximum HP, and
  cures non-volatile status. It succeeds at full HP for the typing/cure, but
  fails without any of those effects if the user is Terastallized or already
  Ground-typed. Normal action restrictions still apply; Sleep Talk can call it
  to cure sleep. Counts as a healing move for Heal Block, Triage and Snatch.
  Uses Mud Sport's animation. The type change ends on switching out.
- Gemshot: Rock, special, 45 power, 100 accuracy, 10 base PP. Targets both foes.
  Hits twice if Stealth Rock is on their side when the move executes; otherwise
  hits once. Hazards on the user's side do not grant the extra hit. Neither
  Skill Link nor Loaded Dice changes this condition. Uses Power Gem with an
  impact on each affected opponent. Each volley is animated separately.
- Harsh Zephyr: Flying, special, 85 power, 100 accuracy, 10 base PP. Targets one
  foe normally and both foes while the user's side has Tailwind. Rechecks at
  execution, including Tailwind set earlier in the same turn, and supports
  permanent starting Tailwind. Each hit lowers the target's Speed one stage
  as a guaranteed secondary effect, respecting Substitute, Contrary, Clear
  Body, Covert Cloak, Shield Dust and Sheer Force. It is a wind move. Uses one
  Air Slash animation per affected opponent under one sky background.

The AI shares the live hit count/targeting rules, including multi-hit Focus
Sash handling, spread damage reduction and Tera's weak-move power floor.
Mud Bath scoring includes healing, harmful-status cures, Ground STAB and the
defensive typing change; it is not considered usable recovery once the user
has become Ground-typed or Terastallized. Regression tests are in
`test/battle/move_effect/mud_gems_zephyr.c`,
`test/battle/ai/mud_gems_zephyr.c` and the shared custom animation tests.
