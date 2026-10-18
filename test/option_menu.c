#include "global.h"
#include "gpu_regs.h"
#include "main.h"
#include "option_menu.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "constants/rgb.h"
#include "test/test.h"

static void OptionMenuFrame(u16 keys)
{
    gMain.newKeys = keys;
    gMain.callback2();
}

static void SettleOptionCursors(void)
{
    for (u32 i = 0; i < 6; i++)
        OptionMenuFrame(0);
}

static u8 FindOptionCursor(s16 x)
{
    for (u32 i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].x == x)
            return i;
    }
    EXPECT(FALSE);
    return SPRITE_NONE;
}

TEST("Options menu saves gameplay switches and all four text speeds on exit")
{
    bool32 initiallyEnabled, useCancel;
    PARAMETRIZE { initiallyEnabled = FALSE; useCancel = FALSE; }
    PARAMETRIZE { initiallyEnabled = TRUE; useCancel = FALSE; }
    PARAMETRIZE { initiallyEnabled = FALSE; useCancel = TRUE; }
    PARAMETRIZE { initiallyEnabled = TRUE; useCancel = TRUE; }
    MainCallback callback = gMain.callback2, savedCallback = gMain.savedCallback;
    IntrCallback vblank = gMain.vblankCallback, hblank = gMain.hblankCallback;
    u8 state = gMain.state;
    u16 keys = gMain.newKeys;
    struct PaletteFadeControl fade = gPaletteFade;
    u32 initialSpeed = initiallyEnabled ? OPTIONS_TEXT_SPEED_INSTANT : OPTIONS_TEXT_SPEED_SLOW;
    u32 expectedSpeed = (initialSpeed + (useCancel ? 3 : 1)) % 4;

    gSaveData.optionsTextSpeed = initialSpeed;
    gSaveData.optionsLevelCaps = initiallyEnabled;
    gSaveData.optionsTrainerScaling = initiallyEnabled;
    gSaveData.optionsWindowFrameType = 8; // Type 9, to test the change to two digits.
    gMain.savedCallback = callback;
    SetMainCallback2(CB2_InitOptionMenu);
    for (u32 i = 0; i < 16 && gMain.callback2 == CB2_InitOptionMenu; i++)
        OptionMenuFrame(0);
    EXPECT(gMain.callback2 != CB2_InitOptionMenu);
    gPaletteFade.active = FALSE;
    OptionMenuFrame(0);
    EXPECT_EQ(GetGpuReg(REG_OFFSET_DISPCNT) & DISPCNT_WIN0_ON, 0);
    EXPECT_EQ(GetGpuReg(REG_OFFSET_BLDCNT), 0);
    EXPECT_EQ(GetGpuReg(REG_OFFSET_BLDY), 0);
    EXPECT_EQ(gPlttBufferUnfaded[BG_PLTT_ID(1) + 1], RGB_WHITE);
    u8 left = FindOptionCursor(114), right = FindOptionCursor(220);
    EXPECT_EQ(gSprites[left].y, 49);
    EXPECT_EQ(gSprites[right].y, 49);
    EXPECT_EQ(120 - (gSprites[left].x + 4), 2);
    EXPECT_EQ((gSprites[right].x - 4) - 214, 2);
    EXPECT_EQ(gSprites[left].oam.matrixNum & ST_OAM_HFLIP, ST_OAM_HFLIP);
    EXPECT_EQ(gSprites[right].oam.matrixNum & ST_OAM_HFLIP, 0);
    EXPECT_EQ((u32)gSprites[left].oam.tileNum, (u32)gSprites[right].oam.tileNum);
    u16 tileTag = GetSpriteTileTagByTileStart(gSprites[left].oam.tileNum);
    u16 paletteTag = GetSpritePaletteTagByPaletteNum(gSprites[left].oam.paletteNum);
    EXPECT_NE(tileTag, TAG_NONE);
    EXPECT_NE(paletteTag, TAG_NONE);

    OptionMenuFrame(DPAD_UP);
    EXPECT_GT(gSprites[left].y, 49);
    EXPECT_LT(gSprites[left].y, 145);
    EXPECT_EQ(gSprites[left].y, gSprites[right].y);
    EXPECT_GT(gSprites[left].x, 20);
    EXPECT_LT(gSprites[left].x, 114);
    SettleOptionCursors();
    EXPECT_EQ(gSprites[left].y, 145);
    EXPECT_EQ(gSprites[right].y, 145);
    EXPECT_EQ(26 - (gSprites[left].x + 4), 2);
    EXPECT_EQ((gSprites[right].x - 4) - (26 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Cancel"), 0)), 2);
    OptionMenuFrame(DPAD_DOWN);
    EXPECT_GT(gSprites[left].y, 49);
    EXPECT_LT(gSprites[left].y, 145);
    // New inputs retarget the easing at its current position, without a snap.
    s16 returningY = gSprites[left].y;
    OptionMenuFrame(DPAD_UP);
    EXPECT_GT(gSprites[left].y, returningY);
    EXPECT_LT(gSprites[left].y, 145);
    returningY = gSprites[left].y;
    OptionMenuFrame(DPAD_DOWN);
    EXPECT_LT(gSprites[left].y, returningY);
    EXPECT_GT(gSprites[left].y, 49);
    SettleOptionCursors();
    EXPECT_EQ(gSprites[left].y, 49);
    EXPECT_EQ(gSprites[right].y, 49);

    // Both directions wrap between Slow and Inst; edits only apply on exit.
    OptionMenuFrame(useCancel ? DPAD_LEFT : DPAD_RIGHT);
    EXPECT_EQ((u32)gSaveData.optionsTextSpeed, initialSpeed);

    // Frame's right edge follows the number, including one/two-digit changes.
    for (u32 i = 0; i < 5; i++)
        OptionMenuFrame(DPAD_DOWN);
    SettleOptionCursors();
    EXPECT_EQ(gSprites[left].x, 114);
    EXPECT_EQ((gSprites[right].x - 4) - (144 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("9"), 0)), 2);
    OptionMenuFrame(DPAD_RIGHT);
    EXPECT_GT((gSprites[right].x - 4) - (144 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("9"), 0)), 2);
    EXPECT_LT((gSprites[right].x - 4) - (144 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("10"), 0)), 2);
    SettleOptionCursors();
    EXPECT_EQ((gSprites[right].x - 4) - (144 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("10"), 0)), 2);
    OptionMenuFrame(DPAD_LEFT);
    SettleOptionCursors();
    EXPECT_EQ((gSprites[right].x - 4) - (144 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("9"), 0)), 2);
    OptionMenuFrame(DPAD_DOWN);
    SettleOptionCursors();
    EXPECT_EQ(gSprites[left].x, 114);
    EXPECT_EQ(gSprites[right].x, 220);
    EXPECT_EQ(gSprites[left].y, 121);
    EXPECT_EQ(gSprites[right].y, 121);
    OptionMenuFrame(DPAD_RIGHT);
    OptionMenuFrame(DPAD_DOWN);
    OptionMenuFrame(DPAD_LEFT);
    EXPECT_EQ((u32)gSaveData.optionsLevelCaps, initiallyEnabled);
    EXPECT_EQ((u32)gSaveData.optionsTrainerScaling, initiallyEnabled);
    if (useCancel)
        OptionMenuFrame(B_BUTTON);
    else
    {
        OptionMenuFrame(DPAD_DOWN);
        OptionMenuFrame(A_BUTTON);
    }
    OptionMenuFrame(0);
    EXPECT_EQ((u32)gSaveData.optionsLevelCaps, !initiallyEnabled);
    EXPECT_EQ((u32)gSaveData.optionsTrainerScaling, !initiallyEnabled);
    EXPECT_EQ((u32)gSaveData.optionsTextSpeed, expectedSpeed);

    gPaletteFade.active = FALSE;
    OptionMenuFrame(0);
    EXPECT_EQ(gMain.callback2, callback);
    EXPECT(!gSprites[left].inUse);
    EXPECT(!gSprites[right].inUse);
    EXPECT_EQ(GetSpriteTileStartByTag(tileTag), TAG_NONE);
    EXPECT_EQ(IndexOfSpritePaletteTag(paletteTag), 0xFF);
    ResetTasks();
    SetMainCallback2(callback);
    gMain.savedCallback = savedCallback;
    gMain.state = state;
    gMain.newKeys = keys;
    SetVBlankCallback(vblank);
    SetHBlankCallback(hblank);
    gPaletteFade = fade;
}

TEST("Options menu gameplay labels fit before the choices")
{
    EXPECT_LE(8 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Level Caps"), 0), 104);
    EXPECT_LE(8 + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Trainer Scaling"), 0), 104);
    EXPECT_LE(9 * 12, 14 * 8);
}

TEST("All four text speed choices fit between the option cursors")
{
    s32 width = GetStringWidth(FONT_SMALL, COMPOUND_STRING("Slow"), 0)
              + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Mid"), 0)
              + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Fast"), 0)
              + GetStringWidth(FONT_SMALL, COMPOUND_STRING("Inst"), 0);
    EXPECT_LE(width + 3 * 2, 198 - 104);
}

TEST("Text speed follows the saved menu selection, including instant")
{
    u32 speed;
    PARAMETRIZE { speed = OPTIONS_TEXT_SPEED_SLOW; }
    PARAMETRIZE { speed = OPTIONS_TEXT_SPEED_MID; }
    PARAMETRIZE { speed = OPTIONS_TEXT_SPEED_FAST; }
    PARAMETRIZE { speed = OPTIONS_TEXT_SPEED_INSTANT; }
    u32 savedSpeed = gSaveData.optionsTextSpeed;
    bool32 forceMid = gTextFlags.forceMidTextSpeed;
    gTextFlags.forceMidTextSpeed = FALSE;
    gSaveData.optionsTextSpeed = speed;
    EXPECT_EQ(GetPlayerTextSpeed(), speed);
    EXPECT_EQ(IsPlayerTextSpeedInstant(), speed == OPTIONS_TEXT_SPEED_INSTANT);

    gTextFlags.forceMidTextSpeed = TRUE;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_MID);
    EXPECT(!IsPlayerTextSpeedInstant());
    EXPECT_EQ((u32)gSaveData.optionsTextSpeed, speed);
    gSaveData.optionsTextSpeed = savedSpeed;
    gTextFlags.forceMidTextSpeed = forceMid;
}
