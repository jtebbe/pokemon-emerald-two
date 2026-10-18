### Note: This guide was written for version 1.14.0, it doesn't apply to earlier versions of expansions. This guide also assumes you are using the default P_LEARNSET_HELPER_TEACHABLE config (TRUE)

# Generating and editing teachable learnsets

## Basics:
When you `make`, a file will be generated in `src/data/pokemon/teachable_learnsets.h` containing the learnsets of your Pokemon. The generated learnset of a pokemon is based on 3 things, its potential moveset defined in `src/data/pokemon/custom_learnables.json`, the teaching type of the species defined in the species data files, and the TMs and tutors available in your game. We will explain each of these one by one.

This project uses `src/data/pokemon/level_up_learnsets/custom.h` for level-up moves. It was initially copied from the active Generation 9 learnsets. Edit that header directly; the `gen_*.h` headers are retained only as reference data.

For egg moves, edit `src/data/pokemon/custom_egg_moves.h`. Every base-stage species has an explicit list, including empty lists for species without egg moves. The original `egg_moves.h` is reference data only. These lists preserve existing inheritance; breeding eligibility still comes from species egg groups.

### Table of Contents:

1. [Potential Teachables]
2. [Teaching Types]
    - [Default Learnset]
    - [TM Illiterate]
    - [All Teachables]
3. [Available Moves]
    - [TMs/Hms]
    - [Tutors from scripts]
    - [Extra Tutors]

## Potential Teachables

The potential moveset of each pokemon is defined in `src/data/pokemon/custom_learnables.json`. Edit each species' list of quoted `MOVE_*` constants to change its TM/HM and tutor compatibility. This file was initially copied from the existing `all_learnables.json`, preserving the current learnsets and Corrode replacement. The old JSON and the import files in `tools/learnset_helpers/porymoves_files` are reference data, not active build inputs.

The custom JSON is tracked source data: neither `make` nor `make clean-teachables` deletes or regenerates it. Changes to it automatically rebuild `teachable_learnsets.h` on the next build. `make clean-teachables` clears only generated headers and helper intermediates, so the next build regenerates those from the custom JSON. Do not edit the generated header directly.

## Teaching Types

In addition to their teachable learnset described in `src/data/pokemon/custom_learnables.json`, Pokemon will be able to learn additional moves based on their teaching types defined in their species data (found in `src/data/pokemon/species_info/gen_X_families.h` for vanilla Pokemon and `src/data/pokemon/species_info.h` for fakemons). There are currently three teaching types in Expansion: Default Learning, TM Illiterate and All Teachables.

### Default Learning

This is the default one, so if a species don't have a teaching type defined in its species data, it will will default to this. If you want to explicitly make a species this way, then add `.teachingType = DEFAULT_LEARNING` to the species data.
Default Learning mean in addtion to the moves described in its additional moveset, the Pokemon will be able to learn the universal moves (like Tera Blast or Hidden Power). Universal moves are listed in `src/data/pokemon/special_movesets.json` (there are multiple lists in the file, make sure to edit the one named "universalMoves")

### TM Illiterate

`.teachingType = TM_ILLITERATE`
TM Illiterate mean a pokemon will only learn move from its potential moveset and not any universal moves. If the config `P_TM_LITERACY` is set to GEN_5 or lower. Universal moves will be removed from the pokemon potential learnset.

### All Teachables

`.teachingType = ALL_TEACHABLES`
All Teachables mean a pokemon will be able to learn every teachable move that are present in your game whether tutor or TM similarly to Mew in official games. If you want a TM or a Tutor move to not be accesible to All Teachables pokemon, you can add it to the signature teachables list in `src/data/pokemon/special_movesets.json` (there are multiple lists in the file, make sure to edit the one named "signatureTeachables")

So, for example, if you a remove a TM from your game, the move associated with that TM will be removed from the teachable elarnset of your pokemon.

## Available Moves

The teachable learnset is based on the potential moveset and the universal moves as described in the sections above but it will only be conposed of moves that are available in your game. Let's see how the code detects the moves that are available.

### TMs/Hms

The script detects the TMs and HMs available in your game based on the list in `include/constants/tms_hms.h`. If you add a new TM, the pokemon with that move in their potential moveset will receive that move in their teachable learnset. And if you remove a TM, the move will be deleted from your pokemon's teachable learnsets.

### Tutors from scripts

The script look for tutors moves in your script files, wether map scripts `/data/maps/*/scripts.inc` or other function scripts `/data/scripts/*.inc`. It looks for the pattern used for move tutors in vanilla emerald `setvar VAR_0x8005, MOVE_*`. So try to add move tutors this way if you want them to be properl detected.

### Extra Tutors

If you somehow add a move that can be learned in new custom ways that don't fit with the previous patterns, you can add the move to the extra tutors list in `src/data/pokemon/special_movesets.json` (there are multiple lists in the file, make sure to edit the one named "extraTutors"). This shouldn't be necessary for most users and if you use this list, remember you will need to add/remove moves from this list and the code that teaches moves.

