#include "global.h"
#include "battle_anim.h"
#include "item_icon.h"
#include "sprite.h"
#include "task.h"
#include "test/battle.h"

static void ResetItemAnimTest(void)
{
    u32 i;

    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    gAnimVisualTaskCount = 0;
    gBattleAnimArgs[0] = 0;
    gBattleAnimArgs[1] = 0;
    gBattleAnimArgs[2] = 0;
    gBattleAnimArgs[3] = 0;
    gBattleAnimArgs[4] = 0;
    gBattleAnimArgs[5] = 0;
    gBattleAnimAttacker = 0;
    gBattleAnimTarget = 1;
    gLastUsedItem = ITEM_NONE;
    gBattleAnimItem1 = ITEM_NONE;
    gBattleAnimItem2 = ITEM_NONE;
    gTestBattleAnimItemFallbackCount = 0;
    gTestLastKnockOffItemAnimUsedFallback = FALSE;
    gTestLastItemIconSpriteItemId = ITEM_NONE;
    gTestLastItemIconSpriteTilesTag = TAG_NONE;
    gTestLastItemIconSpritePaletteTag = TAG_NONE;
    gTestLastItemIconSpritePic = NULL;
    gTestLastItemIconSpritePalette = NULL;
    gTestItemIconSpriteCount = 0;
    for (i = 0; i < TEST_ITEM_ICON_RECORD_COUNT; i++)
    {
        gTestItemIconSpriteItemIds[i] = ITEM_NONE;
        gTestItemIconSpriteTilesTags[i] = TAG_NONE;
        gTestItemIconSpritePaletteTags[i] = TAG_NONE;
        gTestItemIconSpritePics[i] = NULL;
        gTestItemIconSpritePalettes[i] = NULL;
    }
}

static void RunItemAnimTask(TaskFunc taskFunc)
{
    u8 taskId;

    gAnimVisualTaskCount = 1;
    taskId = CreateTask(TaskDummy, 0);
    taskFunc(taskId);
}

static void ExpectLastItemIcon(u16 item, u16 tilesTag, u16 paletteTag)
{
    EXPECT_EQ(gTestLastItemIconSpriteItemId, item);
    EXPECT_EQ(gTestLastItemIconSpriteTilesTag, tilesTag);
    EXPECT_EQ(gTestLastItemIconSpritePaletteTag, paletteTag);
    EXPECT(gTestLastItemIconSpritePic == GetItemIconPic(item));
    EXPECT(gTestLastItemIconSpritePalette == GetItemIconPalette(item));
    EXPECT(GetSpriteTileStartByTag(tilesTag) != TAG_NONE);
    EXPECT(IndexOfSpritePaletteTag(paletteTag) != 0xFF);
}

static void ExpectRecordedItemIcon(u8 index, u16 item, u16 tilesTag, u16 paletteTag)
{
    EXPECT_EQ(gTestItemIconSpriteItemIds[index], item);
    EXPECT_EQ(gTestItemIconSpriteTilesTags[index], tilesTag);
    EXPECT_EQ(gTestItemIconSpritePaletteTags[index], paletteTag);
    EXPECT(gTestItemIconSpritePics[index] == GetItemIconPic(item));
    EXPECT(gTestItemIconSpritePalettes[index] == GetItemIconPalette(item));
}

TEST("Fling item animation uses the flung item's icon and palette")
{
    ResetItemAnimTest();
    gLastUsedItem = ITEM_GRASS_GEM;
    gBattleAnimArgs[0] = 20;
    gBattleAnimArgs[1] = 0;
    gBattleAnimArgs[2] = 0;
    gBattleAnimArgs[3] = 0;
    gBattleAnimArgs[4] = 35;
    gBattleAnimArgs[5] = -25;

    RunItemAnimTask(AnimTask_CreateFlingItem);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    ExpectLastItemIcon(ITEM_GRASS_GEM, ANIM_TAG_ITEM_BAG, ANIM_TAG_ITEM_BAG);
}

TEST("Bestow item animation uses the bestowed item's icon and palette")
{
    ResetItemAnimTest();
    gLastUsedItem = ITEM_CHOICE_SPECS;
    gBattleAnimArgs[0] = 20;
    gBattleAnimArgs[1] = 0;
    gBattleAnimArgs[2] = 0;
    gBattleAnimArgs[3] = 0;
    gBattleAnimArgs[4] = 40;
    gBattleAnimArgs[5] = -25;

    RunItemAnimTask(AnimTask_CreateBestowItem);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    ExpectLastItemIcon(ITEM_CHOICE_SPECS, ANIM_TAG_ITEM_BAG, ANIM_TAG_ITEM_BAG);
}

TEST("Thief and Covet item steal animation uses the stolen item's icon and palette")
{
    ResetItemAnimTest();
    gLastUsedItem = ITEM_THESAURUS;

    RunItemAnimTask(AnimTask_CreateStealItem);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    ExpectLastItemIcon(ITEM_THESAURUS, ANIM_TAG_ITEM_BAG, ANIM_TAG_ITEM_BAG);
}

TEST("Trick and Switcheroo item animations use both swapped item icons and palettes")
{
    ResetItemAnimTest();
    gBattleAnimItem1 = ITEM_GRASS_GEM;
    gBattleAnimItem2 = ITEM_CHOICE_SPECS;
    gBattleAnimArgs[0] = -40;

    RunItemAnimTask(AnimTask_CreateTrickItems);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    EXPECT_EQ(gTestItemIconSpriteCount, 2);
    ExpectRecordedItemIcon(0, ITEM_GRASS_GEM, 0xD930, 0xD930);
    ExpectRecordedItemIcon(1, ITEM_CHOICE_SPECS, 0xD931, 0xD931);
}

TEST("Trick and Switcheroo item animations show nothing when the user has no item")
{
    ResetItemAnimTest();
    gBattleAnimItem1 = ITEM_NONE;
    gBattleAnimItem2 = ITEM_CHOICE_SPECS;
    gBattleAnimArgs[0] = -40;

    RunItemAnimTask(AnimTask_CreateTrickItems);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    EXPECT_EQ(gTestItemIconSpriteCount, 1);
    ExpectRecordedItemIcon(0, ITEM_CHOICE_SPECS, 0xD931, 0xD931);
    EXPECT(GetSpriteTileStartByTag(0xD930) == TAG_NONE);
    EXPECT(IndexOfSpritePaletteTag(0xD930) == 0xFF);
}

TEST("Trick and Switcheroo item animations show nothing when the target has no item")
{
    ResetItemAnimTest();
    gBattleAnimItem1 = ITEM_GRASS_GEM;
    gBattleAnimItem2 = ITEM_NONE;
    gBattleAnimArgs[0] = -40;

    RunItemAnimTask(AnimTask_CreateTrickItems);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 0);
    EXPECT_EQ(gTestItemIconSpriteCount, 1);
    ExpectRecordedItemIcon(0, ITEM_GRASS_GEM, 0xD930, 0xD930);
    EXPECT(GetSpriteTileStartByTag(0xD931) == TAG_NONE);
    EXPECT(IndexOfSpritePaletteTag(0xD931) == 0xFF);
}

TEST("Fling item animation falls back to the stock item bag for ITEM_NONE")
{
    ResetItemAnimTest();
    gLastUsedItem = ITEM_NONE;
    gBattleAnimArgs[4] = 35;
    gBattleAnimArgs[5] = -25;

    RunItemAnimTask(AnimTask_CreateFlingItem);

    EXPECT_EQ(gTestBattleAnimItemFallbackCount, 1);
    EXPECT_EQ(gTestItemIconSpriteCount, 0);
    EXPECT(GetSpriteTileStartByTag(ANIM_TAG_ITEM_BAG) != TAG_NONE);
    EXPECT(IndexOfSpritePaletteTag(ANIM_TAG_ITEM_BAG) != 0xFF);
}
