#include "global.h"
#include "item.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "heal_location.h"
#include "load_save.h"
#include "new_game.h"
#include "overworld.h"
#include "script.h"
#include "tilesets.h"
#include "constants/heal_locations.h"
#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/map_scripts.h"
#include "constants/maps.h"
#include "test/test.h"

extern const struct MapHeader *const *const gMapGroups[MAP_GROUPS_COUNT];

TEST("Unused object graphics slots resolve safely after removing decorations")
{
    for (u32 graphicsId = 0; graphicsId < NUM_OBJ_EVENT_GFX; graphicsId++)
        EXPECT(GetObjectEventGraphicsInfo(graphicsId) != NULL);
    EXPECT(GetObjectEventGraphicsInfo(NUM_OBJ_EVENT_GFX) == GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_NINJA_BOY));
}

TEST("Empty world has exactly the four requested map groups")
{
    EXPECT_EQ(MAP_GROUPS_COUNT, 4);
    EXPECT_EQ(MAP_GROUP_OUTDOOR, 0);
    EXPECT_EQ(MAP_GROUP_INDOOR, 1);
    EXPECT_EQ(MAP_GROUP_DUNGEON, 2);
    EXPECT_EQ(MAP_GROUP_SPECIAL, 3);
    EXPECT(gMapGroups[MAP_GROUP_OUTDOOR] != NULL);
    EXPECT(gMapGroups[MAP_GROUP_INDOOR] == NULL);
    EXPECT(gMapGroups[MAP_GROUP_DUNGEON] == NULL);
    EXPECT(gMapGroups[MAP_GROUP_SPECIAL] == NULL);
}

TEST("Empty world Outdoor is a 20 by 20 General and Petalburg map")
{
    const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_OUTDOOR), MAP_NUM(MAP_OUTDOOR));
    EXPECT_EQ(map->mapLayoutId, LAYOUT_OUTDOOR);
    EXPECT_EQ(map->mapLayout->width, 20);
    EXPECT_EQ(map->mapLayout->height, 20);
    EXPECT(map->mapLayout->primaryTileset == &gTileset_General);
    EXPECT(map->mapLayout->secondaryTileset == &gTileset_Petalburg);
    EXPECT(map->allowRunning);
    EXPECT(!map->allowEscaping);
}

TEST("Empty world Outdoor has no scripts connections or events")
{
    const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_OUTDOOR), MAP_NUM(MAP_OUTDOOR));
    EXPECT(map->mapScripts == NULL);
    EXPECT(map->connections == NULL);
    EXPECT_EQ(map->events->objectEventCount, 0);
    EXPECT_EQ(map->events->warpCount, 0);
    EXPECT_EQ(map->events->coordEventCount, 0);
    EXPECT_EQ(map->events->bgEventCount, 0);
    EXPECT(map->events->objectEvents == NULL);
    EXPECT(map->events->warps == NULL);
    EXPECT(map->events->coordEvents == NULL);
    EXPECT(map->events->bgEvents == NULL);

    gMapHeader = *map;
    EXPECT(MapHeaderGetScriptTable(MAP_SCRIPT_ON_LOAD) == NULL);
}

TEST("Empty world Outdoor grass is walkable and its border is blocked")
{
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_OUTDOOR), MAP_NUM(MAP_OUTDOOR));
    InitMap();
    for (u32 y = 0; y < 20; y++)
    {
        for (u32 x = 0; x < 20; x++)
        {
            EXPECT_EQ(MapGridGetCollisionAt(x + MAP_OFFSET, y + MAP_OFFSET), 0);
            EXPECT_EQ(MapGridGetElevationAt(x + MAP_OFFSET, y + MAP_OFFSET), 3);
        }
    }
    EXPECT_EQ(MapGridGetCollisionAt(MAP_OFFSET - 1, MAP_OFFSET), 1);
    EXPECT_EQ(MapGridGetCollisionAt(MAP_OFFSET + 20, MAP_OFFSET), 1);
    EXPECT_EQ(MapGridGetCollisionAt(MAP_OFFSET, MAP_OFFSET - 1), 1);
    EXPECT_EQ(MapGridGetCollisionAt(MAP_OFFSET, MAP_OFFSET + 20), 1);
}

TEST("Empty world new game and whiteout both start at Outdoor 10 10")
{
    struct WarpData respawn;
    SetBagItemsPointers();
    NewGameInitData();
    EXPECT_EQ(gSaveData.location.mapGroup, MAP_GROUP(MAP_OUTDOOR));
    EXPECT_EQ(gSaveData.location.mapNum, MAP_NUM(MAP_OUTDOOR));
    EXPECT_EQ(gSaveData.pos.x, 10);
    EXPECT_EQ(gSaveData.pos.y, 10);
    EXPECT_EQ(gSaveData.mapLayoutId, LAYOUT_OUTDOOR);
    EXPECT_EQ(GetHealLocationIndexByWarpData(&gSaveData.lastHealLocation), HEAL_LOCATION_OUTDOOR);
    EXPECT_EQ(GetHealNpcLocalId(HEAL_LOCATION_OUTDOOR), LOCALID_NONE);
    SetWhiteoutRespawnWarpAndHealerNPC(&respawn);
    EXPECT_EQ(respawn.mapGroup, MAP_GROUP(MAP_OUTDOOR));
    EXPECT_EQ(respawn.mapNum, MAP_NUM(MAP_OUTDOOR));
    EXPECT_EQ(respawn.x, 10);
    EXPECT_EQ(respawn.y, 10);
    EXPECT(FlagGet(FLAG_RECEIVED_RUNNING_SHOES));
}
