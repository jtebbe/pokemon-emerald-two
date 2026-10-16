#include "global.h"
#include "debug_pc_fill.h"
#include "event_data.h"
#include "load_save.h"
#include "pc_storage.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "save.h"

static enum Species NextSpecies(enum Species species)
{
    // Bound the search even in configurations with no enabled species.
    for (u32 i = 1; i < NUM_SPECIES; i++)
    {
        species = species < NUM_SPECIES - 1 ? species + 1 : 1;
        if (IsSpeciesEnabled(species)) return species;
    }
    return SPECIES_NONE;
}

enum DebugPCFillResult DebugPCFillStep(struct DebugPCFill *fill)
{
    if (fill->failed) return DEBUG_PC_FILL_ERROR;
    if (fill->finished) return DEBUG_PC_FILL_DONE;
    if (PCStorageFaulted() || !SaveFlashAvailable() || LinkBattleStateActive()
        || TradeJournalBlocksSaving() || PCStorageCarrying())
        goto failed;

    if (!fill->started)
    {
        fill->species = NextSpecies(SPECIES_NONE);
        if (fill->species == SPECIES_NONE || !PCStorageOpen()) goto failed;
        fill->started = TRUE;
        return DEBUG_PC_FILL_WORKING;
    }

    // Center the window on the batch, not its first box. All three buffers can
    // then be filled without implicit eviction saves midway through the batch.
    u32 end = min(fill->nextBox + PC_CACHE_BOXES, TOTAL_BOXES_COUNT);
    if (!PCStorageWindow((fill->nextBox + 1) % TOTAL_BOXES_COUNT)) goto failed;
    for (u32 box = fill->nextBox; box < end; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            if (!GetBoxMonDataAt(box, slot, MON_DATA_SPECIES))
            {
                struct BoxPokemon mon;
                // Initialize each species independently: growth rates, abilities,
                // types and experience must not leak from the first species.
                CreateBoxMon(&mon, fill->species, 100, Random32(), OTID_STRUCT_PLAYER_ID);
                GiveBoxMonInitialMoveset(&mon);
                SetBoxMonAt(box, slot, &mon);
                fill->species = NextSpecies(fill->species);
            }

    FlagSet(FLAG_SYS_POKEMON_GET);
    if (!PCStorageAutosave()) goto failed;
    fill->nextBox = end;
    if (end == TOTAL_BOXES_COUNT)
    {
        if (!PCStorageWindow(StorageGetCurrentBox())) goto failed;
        fill->finished = TRUE;
        return DEBUG_PC_FILL_DONE;
    }
    return DEBUG_PC_FILL_WORKING;
failed:
    fill->failed = TRUE;
    return DEBUG_PC_FILL_ERROR;
}
