#include "global.h"
#include "malloc.h"
#include "battle_main.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "menu.h"
#include "international_string_util.h"
#include "menu.h"
#include "menu_specialized.h"
#include "move.h"
#include "move_relearner.h"
#include "palette.h"
#include "player_pc.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "scanline_effect.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "text_window.h"
#include "trig.h"
#include "window.h"
#include "constants/battle_move_effects.h"
#include "constants/characters.h"
#include "constants/songs.h"
#include "gba/io_reg.h"

EWRAM_DATA static u8 sMailboxWindowIds[MAILBOXWIN_COUNT] = {0};
EWRAM_DATA static struct ListMenuItem *sMailboxList = NULL;

static void MailboxMenu_MoveCursorFunc(s32, bool8, struct ListMenu *);
static void MoveRelearnerCursorCallback(s32, bool8, struct ListMenu *);
static void MoveRelearnerDummy(void);

static const struct WindowTemplate sWindowTemplates_MailboxMenu[MAILBOXWIN_COUNT] =
{
    [MAILBOXWIN_TITLE] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x8
    },
    [MAILBOXWIN_LIST] = {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 1,
        .width = 8,
        .height = 18,
        .paletteNum = 15,
        .baseBlock = 0x18
    },
    [MAILBOXWIN_OPTIONS] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 11,
        .height = 8,
        .paletteNum = 15,
        .baseBlock = 0x18
    }
};

static const u8 sPlayerNameTextColors[] =
{
    TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY
};

static const u8 sEmptyItemName[] = _("");



static const struct WindowTemplate sMoveRelearnerWindowTemplates[] =
{
    [RELEARNERWIN_DESC_BATTLE] = {
        .bg = 1,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 16,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0xA
    },
    [RELEARNERWIN_MOVE_LIST] = {
        .bg = 1,
        .tilemapLeft = 19,
        .tilemapTop = 1,
        .width = 10,
        .height = 12,
        .paletteNum = 15,
        .baseBlock = 0x18A
    },
    [RELEARNERWIN_MSG] = {
        .bg = 1,
        .tilemapLeft = 4,
        .tilemapTop = 15,
        .width = 22,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x202
    },
    // Unused. Identical to sMoveRelearnerYesNoMenuTemplate
    [RELEARNERWIN_YESNO] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 8,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x25A
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sMoveRelearnerYesNoMenuTemplate =
{
    .bg = 0,
    .tilemapLeft = 22,
    .tilemapTop = 8,
    .width = 5,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x25A
};


static const struct ListMenuTemplate sMoveRelearnerMovesListTemplate =
{
    .items = NULL,
    .moveCursorFunc = MoveRelearnerCursorCallback,
    .itemPrintFunc = NULL,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = RELEARNERWIN_MOVE_LIST,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NORMAL,
    .cursorKind = CURSOR_BLACK_ARROW,
    .textNarrowWidth = 68,
};

//--------------
// Mailbox menu
//--------------

bool8 MailboxMenu_Alloc(u8 count)
{
    u8 i;

    // + 1 to count for 'Cancel'
    sMailboxList = Alloc((count + 1) * sizeof(*sMailboxList));
    if (sMailboxList == NULL)
        return FALSE;

    for (i = 0; i < ARRAY_COUNT(sMailboxWindowIds); i++)
        sMailboxWindowIds[i] = WINDOW_NONE;

    return TRUE;
}

u8 MailboxMenu_AddWindow(u8 windowIdx)
{
    if (sMailboxWindowIds[windowIdx] == WINDOW_NONE)
    {
        if (windowIdx == MAILBOXWIN_OPTIONS)
        {
            struct WindowTemplate template = sWindowTemplates_MailboxMenu[windowIdx];
            template.width = GetMaxWidthInMenuTable(&gMailboxMailOptions[0], 4);
            sMailboxWindowIds[windowIdx] = AddWindow(&template);
        }
        else // MAILBOXWIN_TITLE or MAILBOXWIN_LIST
        {
            sMailboxWindowIds[windowIdx] = AddWindow(&sWindowTemplates_MailboxMenu[windowIdx]);
        }
        SetStandardWindowBorderStyle(sMailboxWindowIds[windowIdx], FALSE);
    }
    return sMailboxWindowIds[windowIdx];
}

void MailboxMenu_RemoveWindow(u8 windowIdx)
{
    ClearStdWindowAndFrameToTransparent(sMailboxWindowIds[windowIdx], FALSE);
    ClearWindowTilemap(sMailboxWindowIds[windowIdx]);
    RemoveWindow(sMailboxWindowIds[windowIdx]);
    sMailboxWindowIds[windowIdx] = WINDOW_NONE;
}

static u8 UNUSED MailboxMenu_GetWindowId(u8 windowIdx)
{
    return sMailboxWindowIds[windowIdx];
}

static void MailboxMenu_ItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    u8 buffer[30];
    u16 length;

    if (itemId == LIST_CANCEL)
        return;

    StringCopy(buffer, gSaveData.mail[PARTY_SIZE + itemId].playerName);
    ConvertInternationalPlayerName(buffer);
    length = StringLength(buffer);
    if (length < PLAYER_NAME_LENGTH - 1)
        ConvertInternationalString(buffer, LANGUAGE_JAPANESE);
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, 8, y, 0, 0, sPlayerNameTextColors, TEXT_SKIP_DRAW, buffer);
}

u8 MailboxMenu_CreateList(struct PlayerPCItemPageStruct *page)
{
    u16 i;
    for (i = 0; i < page->count; i++)
    {
        sMailboxList[i].name = sEmptyItemName;
        sMailboxList[i].id = i;
    }

    sMailboxList[i].name = gText_Cancel2;
    sMailboxList[i].id = LIST_CANCEL;

    gMultiuseListMenuTemplate.items = sMailboxList;
    gMultiuseListMenuTemplate.totalItems = page->count + 1;
    gMultiuseListMenuTemplate.windowId = sMailboxWindowIds[MAILBOXWIN_LIST];
    gMultiuseListMenuTemplate.header_X = 0;
    gMultiuseListMenuTemplate.item_X = 8;
    gMultiuseListMenuTemplate.cursor_X = 0;
    gMultiuseListMenuTemplate.maxShowed = 8;
    gMultiuseListMenuTemplate.upText_Y = 9;
    gMultiuseListMenuTemplate.cursorPal = 2;
    gMultiuseListMenuTemplate.fillValue = 1;
    gMultiuseListMenuTemplate.cursorShadowPal = 3;
    gMultiuseListMenuTemplate.moveCursorFunc = MailboxMenu_MoveCursorFunc;
    gMultiuseListMenuTemplate.itemPrintFunc = MailboxMenu_ItemPrintFunc;
    gMultiuseListMenuTemplate.fontId = FONT_NORMAL;
    gMultiuseListMenuTemplate.cursorKind = CURSOR_BLACK_ARROW;
    gMultiuseListMenuTemplate.lettersSpacing = 0;
    gMultiuseListMenuTemplate.itemVerticalPadding = 0;
    gMultiuseListMenuTemplate.scrollMultiple = LIST_NO_MULTIPLE_SCROLL;
    return ListMenuInit(&gMultiuseListMenuTemplate, page->itemsAbove, page->cursorPos);
}

static void MailboxMenu_MoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
}

void MailboxMenu_AddScrollArrows(struct PlayerPCItemPageStruct *page)
{
    page->scrollIndicatorTaskId = AddScrollIndicatorArrowPairParameterized(2, 0xC8, 12, 0x94, page->count - page->pageItems + 1, 0x6E, 0x6E, &page->itemsAbove);
}

void MailboxMenu_Free(void)
{
    Free(sMailboxList);
}

//----------------
// Move relearner
//----------------

void InitMoveRelearnerWindows(void)
{
    u8 i;

    InitWindows(sMoveRelearnerWindowTemplates);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(0, 1, BG_PLTT_ID(14));
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);

    for (i = 0; i < ARRAY_COUNT(sMoveRelearnerWindowTemplates) - 1; i++)
        FillWindowPixelBuffer(i, PIXEL_FILL(1));

    DrawStdFrameWithCustomTileAndPalette(RELEARNERWIN_DESC_BATTLE, FALSE, 0x1, 0xE);

    PutWindowTilemap(RELEARNERWIN_MOVE_LIST);
    PutWindowTilemap(RELEARNERWIN_MSG);
    DrawStdFrameWithCustomTileAndPalette(RELEARNERWIN_MOVE_LIST, FALSE, 1, 0xE);
    DrawStdFrameWithCustomTileAndPalette(RELEARNERWIN_MSG, FALSE, 1, 0xE);
    MoveRelearnerDummy();
    ScheduleBgCopyTilemapToVram(1);
}

static void MoveRelearnerDummy(void)
{

}

u8 LoadMoveRelearnerMovesList(const struct ListMenuItem *items, u16 numChoices)
{
    gMultiuseListMenuTemplate = sMoveRelearnerMovesListTemplate;
    gMultiuseListMenuTemplate.totalItems = numChoices;
    gMultiuseListMenuTemplate.items = items;

    if (numChoices < 6)
        gMultiuseListMenuTemplate.maxShowed = numChoices;
    else
        gMultiuseListMenuTemplate.maxShowed = 6;

    return gMultiuseListMenuTemplate.maxShowed;
}

static void MoveRelearnerLoadBattleMoveDescription(u32 chosenMove)
{
    s32 x;
    u8 buffer[32];
    const u8 *str;

    if (B_SHOW_CATEGORY_ICON == TRUE)
        MoveRelearnerShowHideCategoryIcon(chosenMove);

    FillWindowPixelBuffer(RELEARNERWIN_DESC_BATTLE, PIXEL_FILL(1));
    str = gText_MoveRelearnerBattleMoves;
    x = GetStringCenterAlignXOffset(FONT_NORMAL, str, 128);
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, x, 1, TEXT_SKIP_DRAW, NULL);

    str = gText_MoveRelearnerPP;
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, 4, 41, TEXT_SKIP_DRAW, NULL);

    str = gText_MoveRelearnerPower;
    x = GetStringRightAlignXOffset(FONT_NORMAL, str, 106);
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, x, 25, TEXT_SKIP_DRAW, NULL);

    str = gText_MoveRelearnerAccuracy;
    x = GetStringRightAlignXOffset(FONT_NORMAL, str, 106);
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, x, 41, TEXT_SKIP_DRAW, NULL);
    if (chosenMove == LIST_CANCEL)
    {
        // On "Cancel", skip printing move data
        CopyWindowToVram(RELEARNERWIN_DESC_BATTLE, COPYWIN_GFX);
        return;
    }
    str = gTypesInfo[GetMoveType(chosenMove)].name;
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, 4, 25, TEXT_SKIP_DRAW, NULL);

    x = 4 + GetStringWidth(FONT_NORMAL, gText_MoveRelearnerPP, 0);
    ConvertIntToDecimalStringN(buffer, GetMovePP(chosenMove), STR_CONV_MODE_LEFT_ALIGN, 2);
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, buffer, x, 41, TEXT_SKIP_DRAW, NULL);

    if (GetMovePower(chosenMove) < 2)
    {
        str = gText_ThreeDashes;
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, GetMovePower(chosenMove), STR_CONV_MODE_LEFT_ALIGN, 3);
        str = buffer;
    }
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, 106, 25, TEXT_SKIP_DRAW, NULL);

    if (GetMoveAccuracy(chosenMove) == 0)
    {
        str = gText_ThreeDashes;
    }
    else
    {
        ConvertIntToDecimalStringN(buffer, GetMoveAccuracy(chosenMove), STR_CONV_MODE_LEFT_ALIGN, 3);
        str = buffer;
    }
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, FONT_NORMAL, str, 106, 41, TEXT_SKIP_DRAW, NULL);

    const u8 *moveDescription = GetMoveDescription(chosenMove);
    u32 fontId = GetFontIdToFit(moveDescription, FONT_NARROW, 0, WindowWidthPx(RELEARNERWIN_DESC_BATTLE));
    AddTextPrinterParameterized(RELEARNERWIN_DESC_BATTLE, fontId, moveDescription, 0, 65, 0, NULL);
}



static void MoveRelearnerCursorCallback(s32 itemIndex, bool8 onInit, struct ListMenu *list)
{
    if (onInit != TRUE)
        PlaySE(SE_SELECT);
    MoveRelearnerLoadBattleMoveDescription(itemIndex);
}

void MoveRelearnerPrintMessage(u8 *str)
{
    u8 speed;

    FillWindowPixelBuffer(RELEARNERWIN_MSG, PIXEL_FILL(1));
    gTextFlags.canABSpeedUpPrint = TRUE;
    speed = GetPlayerTextSpeedDelay();
    AddTextPrinterParameterized2(RELEARNERWIN_MSG, FONT_NORMAL, str, speed, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, 3);
}

void MoveRelearnerCreateYesNoMenu(void)
{
    CreateYesNoMenu(&sMoveRelearnerYesNoMenuTemplate, 1, 0xE, 0);
}

static const u8 *const sLvlUpStatStrings[NUM_STATS] =
{
    gText_MaxHP,
    gText_Attack,
    gText_Defense,
    gText_SpAtk,
    gText_SpDef,
    gText_Speed
};

void DrawLevelUpWindowPg1(u16 windowId, u16 *statsBefore, u16 *statsAfter, u8 bgClr, u8 fgClr, u8 shadowClr)
{
    u16 i, x;
    s16 statsDiff[NUM_STATS];
    u8 text[12];
    u8 color[3];

    FillWindowPixelBuffer(windowId, PIXEL_FILL(bgClr));

    statsDiff[0] = statsAfter[STAT_HP]    - statsBefore[STAT_HP];
    statsDiff[1] = statsAfter[STAT_ATK]   - statsBefore[STAT_ATK];
    statsDiff[2] = statsAfter[STAT_DEF]   - statsBefore[STAT_DEF];
    statsDiff[3] = statsAfter[STAT_SPATK] - statsBefore[STAT_SPATK];
    statsDiff[4] = statsAfter[STAT_SPDEF] - statsBefore[STAT_SPDEF];
    statsDiff[5] = statsAfter[STAT_SPEED] - statsBefore[STAT_SPEED];

    color[0] = bgClr;
    color[1] = fgClr;
    color[2] = shadowClr;

    for (i = 0; i < NUM_STATS; i++)
    {

        AddTextPrinterParameterized3(windowId,
                                     FONT_NORMAL,
                                     0,
                                     15 * i,
                                     color,
                                     TEXT_SKIP_DRAW,
                                     sLvlUpStatStrings[i]);

        StringCopy(text, (statsDiff[i] >= 0) ? gText_Plus : gText_Dash);
        AddTextPrinterParameterized3(windowId,
                                     FONT_NORMAL,
                                     56,
                                     15 * i,
                                     color,
                                     TEXT_SKIP_DRAW,
                                     text);
        if (abs(statsDiff[i]) <= 9)
            x = 18;
        else
            x = 12;

        ConvertIntToDecimalStringN(text, abs(statsDiff[i]), STR_CONV_MODE_LEFT_ALIGN, 2);
        AddTextPrinterParameterized3(windowId,
                                     FONT_NORMAL,
                                     56 + x,
                                     15 * i,
                                     color,
                                     TEXT_SKIP_DRAW,
                                     text);
    }
}

void DrawLevelUpWindowPg2(u16 windowId, u16 *currStats, u8 bgClr, u8 fgClr, u8 shadowClr)
{
    u16 i, numDigits, x;
    s16 stats[NUM_STATS];
    u8 text[12];
    u8 color[3];

    FillWindowPixelBuffer(windowId, PIXEL_FILL(bgClr));

    stats[0] = currStats[STAT_HP];
    stats[1] = currStats[STAT_ATK];
    stats[2] = currStats[STAT_DEF];
    stats[3] = currStats[STAT_SPATK];
    stats[4] = currStats[STAT_SPDEF];
    stats[5] = currStats[STAT_SPEED];

    color[0] = bgClr;
    color[1] = fgClr;
    color[2] = shadowClr;

    for (i = 0; i < NUM_STATS; i++)
    {
        if (stats[i] > 99)
            numDigits = 3;
        else if (stats[i] > 9)
            numDigits = 2;
        else
            numDigits = 1;

        ConvertIntToDecimalStringN(text, stats[i], STR_CONV_MODE_LEFT_ALIGN, numDigits);
        x = 6 * (4 - numDigits);

        AddTextPrinterParameterized3(windowId,
                                     FONT_NORMAL,
                                     0,
                                     15 * i,
                                     color,
                                     TEXT_SKIP_DRAW,
                                     sLvlUpStatStrings[i]);

        AddTextPrinterParameterized3(windowId,
                                     FONT_NORMAL,
                                     56 + x,
                                     15 * i,
                                     color,
                                     TEXT_SKIP_DRAW,
                                     text);
    }
}

void GetMonLevelUpWindowStats(struct Pokemon *mon, u16 *currStats)
{
    currStats[STAT_HP]    = GetMonData(mon, MON_DATA_MAX_HP);
    currStats[STAT_ATK]   = GetMonData(mon, MON_DATA_ATK);
    currStats[STAT_DEF]   = GetMonData(mon, MON_DATA_DEF);
    currStats[STAT_SPEED] = GetMonData(mon, MON_DATA_SPEED);
    currStats[STAT_SPATK] = GetMonData(mon, MON_DATA_SPATK);
    currStats[STAT_SPDEF] = GetMonData(mon, MON_DATA_SPDEF);
}
