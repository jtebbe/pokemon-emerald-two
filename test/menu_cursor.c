#include "global.h"
#include "bg.h"
#include "main.h"
#include "menu.h"
#include "start_menu.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "test/test.h"

static const struct BgTemplate sCursorTestBg =
{
    .bg = 0,
    .mapBaseIndex = 31,
};

static const struct WindowTemplate sCursorTestWindows[] =
{
    {
        .bg = 0,
        .width = 7,
        .height = 10,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE,
};

static void InitCursorTestMenu(void)
{
    ResetTasks();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, &sCursorTestBg, 1);
    EXPECT(InitWindows(sCursorTestWindows));
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    // Stand in for the menu labels: the arrow must never erase these pixels.
    FillWindowPixelRect(0, PIXEL_FILL(5), 8, 0, 48, 80);
    InitMenuNormal(0, FONT_NORMAL, 0, 9, 16, 4, 0);
    Menu_SetCursorEasing(TRUE);
}

static u32 CursorPixel(u32 x, u32 y)
{
    u32 offset = ((y / 8) * gWindows[0].window.width + x / 8) * 32
               + (y % 8) * 4 + (x % 8) / 2;
    return (gWindows[0].tileData[offset] >> ((x % 2) * 4)) & 0xF;
}

static u32 CursorTop(void)
{
    for (u32 y = 0; y < 80; y++)
    {
        for (u32 x = 0; x < 8; x++)
        {
            if (CursorPixel(x, y) != 1)
                return y;
        }
    }
    EXPECT(FALSE);
    return 80;
}

static u32 CursorInkCount(void)
{
    u32 count = 0;
    for (u32 y = 0; y < 80; y++)
    {
        for (u32 x = 0; x < 8; x++)
            count += CursorPixel(x, y) != 1;
        EXPECT_EQ(CursorPixel(8, y), 5);
        EXPECT_EQ(CursorPixel(55, y), 5);
    }
    return count;
}

static void EndCursorTestMenu(void)
{
    Menu_SetCursorEasing(FALSE);
    FreeAllWindowBuffers();
    ResetTasks();
    ResetBgsAndClearDma3BusyFlags(0);
}

TEST("Start menu task eases its arrow while selection and confirmation remain immediate")
{
    u16 keys = gMain.newKeys;
    bool8 (*callback)(void) = gMenuCallback;
    InitCursorTestMenu();
    u32 top = CursorTop(), ink = CursorInkCount();
    EXPECT_GT(ink, 0);
    u8 taskId = CreateTask(Task_ShowStartMenu, 0);
    gMain.newKeys = 0;
    gTasks[taskId].func(taskId);
    gMain.newKeys = DPAD_DOWN;
    gMain.vblankCounter1++;
    gTasks[taskId].func(taskId);
    EXPECT_EQ(Menu_GetCursorPos(), 1);
    EXPECT_GT(CursorTop(), top);
    EXPECT_LT(CursorTop(), top + 16);
    EXPECT_EQ(CursorInkCount(), ink);

    // Another input is accepted before the first animation finishes.
    u32 mid = CursorTop();
    gMain.vblankCounter1++;
    gTasks[taskId].func(taskId);
    EXPECT_EQ(Menu_GetCursorPos(), 2);
    EXPECT_GT(CursorTop(), mid);
    EXPECT_LT(CursorTop(), top + 32);
    gMain.newKeys = A_BUTTON;
    EXPECT_EQ(Menu_ProcessInput(), 2);
    gMain.newKeys = 0;
    for (u32 frame = 0; frame < 6; frame++)
    {
        gMain.vblankCounter1++;
        gTasks[taskId].func(taskId);
    }
    EXPECT_EQ(CursorTop(), top + 32);
    EXPECT_EQ(CursorInkCount(), ink);
    EndCursorTestMenu();
    gMenuCallback = callback;
    gMain.newKeys = keys;
}

TEST("Start menu arrow wraps and reverses smoothly without leaving old arrow pixels")
{
    InitCursorTestMenu();
    u32 top = CursorTop(), ink = CursorInkCount();
    EXPECT_EQ(Menu_MoveCursor(-1), 3);
    EXPECT_EQ(CursorTop(), top);
    Menu_UpdateCursorEasing();
    EXPECT_GT(CursorTop(), top);
    EXPECT_LT(CursorTop(), top + 48);
    u32 mid = CursorTop();
    EXPECT_EQ(Menu_MoveCursor(1), 0);
    EXPECT_EQ(CursorTop(), mid);
    Menu_UpdateCursorEasing();
    EXPECT_GT(CursorTop(), top);
    EXPECT_LT(CursorTop(), mid);
    for (u32 frame = 0; frame < 6; frame++)
        Menu_UpdateCursorEasing();
    EXPECT_EQ(CursorTop(), top);
    EXPECT_EQ(CursorInkCount(), ink);
    EndCursorTestMenu();
}

TEST("Opening another normal menu disables start menu cursor easing")
{
    InitCursorTestMenu();
    u32 top = CursorTop();
    Menu_MoveCursor(1);
    Menu_UpdateCursorEasing();
    FillWindowPixelRect(0, PIXEL_FILL(1), 0, 0, 8, 80);
    InitMenuNormal(0, FONT_NORMAL, 0, 9, 16, 4, 1);
    EXPECT_EQ(CursorTop(), top + 16);
    EXPECT_EQ(Menu_MoveCursor(1), 2);
    EXPECT_EQ(CursorTop(), top + 32);
    for (u32 frame = 0; frame < 6; frame++)
        Menu_UpdateCursorEasing();
    EXPECT_EQ(CursorTop(), top + 32);
    EndCursorTestMenu();
}

TEST("Closing windows during cursor easing safely stops further updates")
{
    InitCursorTestMenu();
    Menu_MoveCursor(1);
    Menu_UpdateCursorEasing();
    FreeAllWindowBuffers();
    for (u32 frame = 0; frame < 8; frame++)
        Menu_UpdateCursorEasing();
    EXPECT(gWindows[0].tileData == NULL);
    EndCursorTestMenu();
}
