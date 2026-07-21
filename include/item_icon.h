#ifndef GUARD_ITEM_ICON_H
#define GUARD_ITEM_ICON_H

extern u8 *gItemIconDecompressionBuffer;
extern u8 *gItemIcon4x4Buffer;

extern const struct SpriteTemplate gItemIconSpriteTemplate;

bool8 AllocItemIconTemporaryBuffers(void);
void FreeItemIconTemporaryBuffers(void);
void CopyItemIconPicTo4x4Buffer(const void *src, void *dest);
u8 AddItemIconSprite(u16 tilesTag, u16 paletteTag, u16 itemId);
u8 AddCustomItemIconSprite(const struct SpriteTemplate *customSpriteTemplate, u16 tilesTag, u16 paletteTag, u16 itemId);
const void *GetItemIconPic(u16 itemId);
const u16 *GetItemIconPalette(u16 itemId);

#if TESTING
#define TEST_ITEM_ICON_RECORD_COUNT 8

extern u16 gTestLastItemIconSpriteItemId;
extern u16 gTestLastItemIconSpriteTilesTag;
extern u16 gTestLastItemIconSpritePaletteTag;
extern const void *gTestLastItemIconSpritePic;
extern const u16 *gTestLastItemIconSpritePalette;
extern u8 gTestItemIconSpriteCount;
extern u16 gTestItemIconSpriteItemIds[TEST_ITEM_ICON_RECORD_COUNT];
extern u16 gTestItemIconSpriteTilesTags[TEST_ITEM_ICON_RECORD_COUNT];
extern u16 gTestItemIconSpritePaletteTags[TEST_ITEM_ICON_RECORD_COUNT];
extern const void *gTestItemIconSpritePics[TEST_ITEM_ICON_RECORD_COUNT];
extern const u16 *gTestItemIconSpritePalettes[TEST_ITEM_ICON_RECORD_COUNT];
#endif

#endif //GUARD_ITEM_ICON_H
