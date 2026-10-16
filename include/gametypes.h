#ifndef GUARD_GAMETYPES_H
#define GUARD_GAMETYPES_H

#include "gba/types.h"

//
// This header includes typedefs for fields that commonly appear throughout
// the codebase, and which ROM hacks might benefit from being able to widen.
//
// These typedefs include the underlying type in their name for two reasons:
//
//  - Game Freak wasn't fully consistent about field widths throughout
//    their codebase. For example, when Region Map Sections are persistently
//    stored in savedata, they're stored as 8-bit values; but much of the
//    codebase handles them as 16-bit values.
//
//  - The game stores one contiguous save image. Explicit-width fields make
//    the image size and compiler-inserted alignment predictable.
//
// Accordingly, this file generally doesn't contain just single types, but
// rather families of types. For example, Region Map Sections are saved as
// u8s within the player's save file, but are sometimes handled as u16s or
// even s16s and ints; and so there are multiple typedefs for Map Sections
// corresponding to each of these underlying types, and each typedef has a
// name which indicates the underlying type.
//
// For a given family of typedefs, the smallest one should be considered
// the "real" or "canonical" type. Continuing with Map Sections as our
// example, the smallest type is an 8-bit integer, and so any values that
// can't fit in an 8-bit integer will be truncated and lost at some point
// within the codebase. Therefore mapsec_u8_t is the "canonical" type for
// Map Sections, and the larger typedefs just exist to describe situations
// where the game handles Map Sections inconsistently with that "canon."
//

// Map Sections are named areas that can appear in the region map. Each
// individual map can be assigned to a Map Section as appropriate. The
// possible values are in constants/region_map_sections.h.
//
// If you choose to widen Map Sections, be aware that Met Locations (below)
// are based on Map Sections and will also be widened.
typedef u8  mapsec_u8_t;
typedef u16 mapsec_u16_t;
typedef s16 mapsec_s16_t;
typedef s32 mapsec_s32_t;

// Met Locations for caught Pokemon use the same values as Map Sections,
// except that 0xFD and 0xFE have special meanings.
//
// Because this value appears inside every Pokemon's data, widening it will
// consume more flash space. Account for alignment in PokemonData and
// BoxPokemon when changing it; PC pages and trade journals store the direct
// BoxPokemon layout, so a size change also requires a save-format update.
typedef mapsec_u8_t metloc_u8_t;

#endif //GUARD_GAMETYPES_H
