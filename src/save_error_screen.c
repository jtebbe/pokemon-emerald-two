#include "global.h"
#include "bg.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "save.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "constants/rgb.h"

// This screen never touches flash. Restart performs normal journal recovery.
static void DrawSaveErrorMessage(const u8 *message)
{
    static const struct BgTemplate bg =
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
    };
    static const struct WindowTemplate windows[] =
    {
        {
            .bg = 0,
            .tilemapLeft = 3,
            .tilemapTop = 2,
            .width = 24,
            .height = 16,
            .paletteNum = 15,
            .baseBlock = 1,
        },
        DUMMY_WIN_TEMPLATE
    };
    const u8 colors[] = {TEXT_COLOR_TRANSPARENT, TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_LIGHT_GRAY};

    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, &bg, 1);
    LoadBgTiles(0, gTextWindowFrame1_Gfx, 0x120, 0x214);
    DeactivateAllTextPrinters();
    ResetTasks();
    ResetPaletteFade();
    LoadPalette(gTextWindowFrame1_Pal, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    InitWindows(windows);
    DrawStdFrameWithCustomTileAndPalette(0, TRUE, 0x214, 0xE);
    AddTextPrinterParameterized4(0, FONT_NORMAL, 8, 1, 0, 0, colors, 0, message);
    CopyWindowToVram(0, COPYWIN_FULL);
    TransferPlttBuffer();
    *(u16 *)PLTT = RGB(17, 18, 31);
    ShowBg(0);
    gMain.state = 1;
}

void CB2_SaveErrorScreen(void)
{
    if (gMain.state == 0)
    {
        DrawSaveErrorMessage(COMPOUND_STRING(
            "{COLOR RED}Saving failed.{COLOR DARK_GRAY}\n"
            "\n"
            "Restart to recover the last\n"
            "completed operation.\n"
            "\n"
            "Press A to restart."));
    }
    else if (JOY_NEW(A_BUTTON))
    {
        DoSoftReset();
    }
}

void CB2_FlashNotDetectedScreen(void)
{
    if (gMain.state == 0)
        DrawSaveErrorMessage(COMPOUND_STRING(
            "{COLOR RED}ERROR! {COLOR DARK_GRAY}Flash memory not detected!\n"
            "\n"
            "If playing on an emulator, set your\n"
            "save type setting to\n"
            "Flash 1Mb/128K and reload the ROM.\n"
            "\n"
            "If playing on hardware, your cart\n"
            "does not have a working flash chip."));
}
