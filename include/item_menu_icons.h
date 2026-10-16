#ifndef GUARD_ITEM_MENU_ICONS_H
#define GUARD_ITEM_MENU_ICONS_H

extern const struct CompressedSpriteSheet gBagMaleSpriteSheet;
extern const struct CompressedSpriteSheet gBagFemaleSpriteSheet;
extern const struct SpritePalette gBagPaletteTable;
extern const struct CompressedSpriteSheet gBerryCheckCircleSpriteSheet;
extern const struct SpritePalette gBerryCheckCirclePaletteTable;

void RemoveBagSprite(u8 id);
void AddBagVisualSprite(u8 bagPocketId);
void SetBagVisualPocketId(u8 bagPocketId, bool8 isSwitchingPockets);
void ShakeBagSprite(void);
void AddSwitchPocketRotatingBallSprite(s16 rotationDirection);
void AddBagItemIconSprite(enum Item itemId, u8 id);
void RemoveBagItemIconSprite(u8 id);
void CreateItemMenuSwapLine(void);
void SetItemMenuSwapLineInvisibility(bool8 invisible);
void UpdateItemMenuSwapLinePos(u8 y);

// Note: Because of removing gDecompressionBuffer and allowing to create more than one berry sprite at a time, the memory for gfx is allocated dynamically.
// CreateBerryTagSprite stores the allocated pointer in data[6] and data[7]; keep these fields reserved.
#define BERRY_ICON_GFX_PTR_DATA_ID 6
u32 CreateBerryTagSprite(u32 id, s32 x, s32 y);
// Needs to always call either of these after creating a Berry Icon sprite, because it frees allocated memory!
void DestroyBerryIconSprite(u32 spriteId, u32 berryId, bool32 freePal);
void DestroyBerryIconSpritePtr(struct Sprite *sprite, u32 berryId, bool32 freePal);

void FreeBerryIconSpritePalette(u32 berryId); // Unused atm, because it's also handled by DestroyBerryIconSprite. Leaving it as it is, because it may still be useful in some custom cases.

u8 CreateBerryFlavorCircleSprite(s16 x);

#endif // GUARD_ITEM_MENU_ICONS_H
