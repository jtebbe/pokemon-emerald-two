#include "global.h"
#include "battle_main.h"
#include "decompress.h"
#include "graphics.h"
#include "item.h"
#include "item_icon.h"
#include "malloc.h"
#include "move.h"
#include "sprite.h"
#include "constants/items.h"
#include "pokevial.h" // Pokevial Branch

// EWRAM vars
EWRAM_DATA u8 *gItemIconDecompressionBuffer = NULL;
EWRAM_DATA u8 *gItemIcon4x4Buffer = NULL;

#if TESTING
EWRAM_DATA u16 gTestLastItemIconSpriteItemId = ITEM_NONE;
EWRAM_DATA u16 gTestLastItemIconSpriteTilesTag = 0;
EWRAM_DATA u16 gTestLastItemIconSpritePaletteTag = 0;
EWRAM_DATA const void *gTestLastItemIconSpritePic = NULL;
EWRAM_DATA const u16 *gTestLastItemIconSpritePalette = NULL;
EWRAM_DATA u8 gTestItemIconSpriteCount = 0;
EWRAM_DATA u16 gTestItemIconSpriteItemIds[TEST_ITEM_ICON_RECORD_COUNT] = {0};
EWRAM_DATA u16 gTestItemIconSpriteTilesTags[TEST_ITEM_ICON_RECORD_COUNT] = {0};
EWRAM_DATA u16 gTestItemIconSpritePaletteTags[TEST_ITEM_ICON_RECORD_COUNT] = {0};
EWRAM_DATA const void *gTestItemIconSpritePics[TEST_ITEM_ICON_RECORD_COUNT] = {0};
EWRAM_DATA const u16 *gTestItemIconSpritePalettes[TEST_ITEM_ICON_RECORD_COUNT] = {0};
#endif

// const rom data
#include "data/item_icon_table.h"

static const struct OamData sOamData_ItemIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 2,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_ItemIcon[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_ItemIcon[] =
{
    sSpriteAnim_ItemIcon
};

const struct SpriteTemplate gItemIconSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_ItemIcon,
    .anims = sSpriteAnimTable_ItemIcon,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// code
bool8 AllocItemIconTemporaryBuffers(void)
{
    gItemIconDecompressionBuffer = Alloc(0x120);
    if (gItemIconDecompressionBuffer == NULL)
        return FALSE;

    gItemIcon4x4Buffer = AllocZeroed(0x200);
    if (gItemIcon4x4Buffer == NULL)
    {
        Free(gItemIconDecompressionBuffer);
        return FALSE;
    }

    return TRUE;
}

void FreeItemIconTemporaryBuffers(void)
{
    Free(gItemIconDecompressionBuffer);
    Free(gItemIcon4x4Buffer);
}

void CopyItemIconPicTo4x4Buffer(const void *src, void *dest)
{
    u8 i;

    for (i = 0; i < 3; i++)
        CpuCopy16(src + i * 96, dest + i * 128, 0x60);
}

#if TESTING
static void RecordItemIconSpriteForTest(u16 tilesTag, u16 paletteTag, u16 itemId)
{
    u8 index = gTestItemIconSpriteCount;

    gTestLastItemIconSpriteItemId = itemId;
    gTestLastItemIconSpriteTilesTag = tilesTag;
    gTestLastItemIconSpritePaletteTag = paletteTag;
    gTestLastItemIconSpritePic = GetItemIconPic(itemId);
    gTestLastItemIconSpritePalette = GetItemIconPalette(itemId);

    if (index < TEST_ITEM_ICON_RECORD_COUNT)
    {
        gTestItemIconSpriteItemIds[index] = itemId;
        gTestItemIconSpriteTilesTags[index] = tilesTag;
        gTestItemIconSpritePaletteTags[index] = paletteTag;
        gTestItemIconSpritePics[index] = gTestLastItemIconSpritePic;
        gTestItemIconSpritePalettes[index] = gTestLastItemIconSpritePalette;
    }
    if (gTestItemIconSpriteCount < 0xFF)
        gTestItemIconSpriteCount++;
}
#endif

u8 AddItemIconSprite(u16 tilesTag, u16 paletteTag, u16 itemId)
{
    if (!AllocItemIconTemporaryBuffers())
    {
        return MAX_SPRITES;
    }
    else
    {
        u8 spriteId;
        struct SpriteSheet spriteSheet;
        struct SpritePalette spritePalette;
        struct SpriteTemplate *spriteTemplate;

#if TESTING
        RecordItemIconSpriteForTest(tilesTag, paletteTag, itemId);
#endif
        DecompressDataWithHeaderWram(GetItemIconPic(itemId), gItemIconDecompressionBuffer);
        CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
        spriteSheet.data = gItemIcon4x4Buffer;
        spriteSheet.size = 0x200;
        spriteSheet.tag = tilesTag;
        LoadSpriteSheet(&spriteSheet);

        spritePalette.data = GetItemIconPalette(itemId);
        spritePalette.tag = paletteTag;
        LoadSpritePalette(&spritePalette);

        spriteTemplate = Alloc(sizeof(*spriteTemplate));
        CpuCopy16(&gItemIconSpriteTemplate, spriteTemplate, sizeof(*spriteTemplate));
        spriteTemplate->tileTag = tilesTag;
        spriteTemplate->paletteTag = paletteTag;
        spriteId = CreateSprite(spriteTemplate, 0, 0, 0);

        FreeItemIconTemporaryBuffers();
        Free(spriteTemplate);

        return spriteId;
    }
}

u8 AddCustomItemIconSprite(const struct SpriteTemplate *customSpriteTemplate, u16 tilesTag, u16 paletteTag, u16 itemId)
{
    if (!AllocItemIconTemporaryBuffers())
    {
        return MAX_SPRITES;
    }
    else
    {
        u8 spriteId;
        struct SpriteSheet spriteSheet;
        struct SpritePalette spritePalette;
        struct SpriteTemplate *spriteTemplate;

#if TESTING
        RecordItemIconSpriteForTest(tilesTag, paletteTag, itemId);
#endif
        DecompressDataWithHeaderWram(GetItemIconPic(itemId), gItemIconDecompressionBuffer);
        CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
        spriteSheet.data = gItemIcon4x4Buffer;
        spriteSheet.size = 0x200;
        spriteSheet.tag = tilesTag;
        LoadSpriteSheet(&spriteSheet);

        spritePalette.data = GetItemIconPalette(itemId);
        spritePalette.tag = paletteTag;
        LoadSpritePalette(&spritePalette);

        spriteTemplate = Alloc(sizeof(*spriteTemplate));
        CpuCopy16(customSpriteTemplate, spriteTemplate, sizeof(*spriteTemplate));
        spriteTemplate->tileTag = tilesTag;
        spriteTemplate->paletteTag = paletteTag;
        spriteId = CreateSprite(spriteTemplate, 0, 0, 0);

        FreeItemIconTemporaryBuffers();
        Free(spriteTemplate);

        return spriteId;
    }
}

const void *GetItemIconPic(u16 itemId)
{
    if (itemId == ITEM_LIST_END)
        return gItemIcon_ReturnToFieldArrow; // Use last icon, the "return to field" arrow
    if (itemId >= ITEMS_COUNT)
        return gItemsInfo[0].iconPic;
    if (gItemsInfo[itemId].pocket == POCKET_TM_HM)
    {
        if (GetItemTMHMIndex(itemId) > NUM_TECHNICAL_MACHINES)
            return gItemIcon_HM;
        return gItemIcon_TM;
    }

    // Start Pokevial Branch
    if (itemId == ITEM_POKEVIAL)
        return PokevialGetDoseIcon();
    // End Pokevial Branch

    return gItemsInfo[itemId].iconPic;
}

const u16 *GetItemIconPalette(u16 itemId)
{
    if (itemId == ITEM_LIST_END)
        return gItemIconPalette_ReturnToFieldArrow;
    if (itemId >= ITEMS_COUNT)
        return gItemsInfo[0].iconPalette;
    if (gItemsInfo[itemId].pocket == POCKET_TM_HM)
        return gTypesInfo[GetMoveType(GetItemTMHMMoveId(itemId))].paletteTMHM;

    return gItemsInfo[itemId].iconPalette;
}
