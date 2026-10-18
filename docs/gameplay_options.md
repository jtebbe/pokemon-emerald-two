# Gameplay options

The Options menu includes two independent OFF/ON switches. Both default to OFF,
and their values are stored with the other options without growing the save layout.

Text Speed offers Slow, Mid, Fast, and Inst (instant text). This is a saved menu
setting, not a compile-time override. Scripts that force Mid speed still do so.

## Level Caps

OFF disables the level-cap system. ON enables the existing badge/milestone cap
progression: 15, 19, 24, 29, 31, 33, 42, 46, then 58 before becoming Champion.
After that, the cap is 100. The thresholds are defined in `src/caps.c`.

The current configuration uses hard caps: battle experience, daycare level gains,
Rare Candies and EXP Candies cannot raise a Pokemon beyond the current cap.
Enabling the option does not lower Pokemon that already exceed it. Cap behavior
and progression source remain configurable in `include/config/caps.h`.

## Trainer Scaling

ON sets every generated NPC trainer Pokemon, including NPC partners, to the
highest level among the player's non-egg party members. Fainted members count.
The level is captured when the NPC party is generated. With no non-egg party
members, the configured trainer levels are retained.

OFF uses the levels from the trainer party data. Wild encounters, static and gift
Pokemon, link opponents, and helpers that create the player's own party are not
scaled. Scaling does not change species, explicit moves, items, abilities or EVs;
stats and automatic level-up movesets are generated at the scaled level. Level
Caps and Trainer Scaling are independent: scaling does not clamp trainers to the cap.
