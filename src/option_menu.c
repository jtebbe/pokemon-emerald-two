#include "global.h"
#include "comfy_anim.h"
#include "option_menu.h"
#include "bg.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "gba/m4a_internal.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define tMenuSelection data[0]
#define tTextSpeed data[1]
#define tBattleSceneOff data[2]
#define tBattleStyle data[3]
#define tSound data[4]
#define tButtonMode data[5]
#define tWindowFrameType data[6]
#define tLevelCaps data[7]
#define tTrainerScaling data[8]
#define tLeftArrowSpriteId data[9]
#define tRightArrowSpriteId data[10]

#define OPTION_ROW_HEIGHT 12
#define OPTION_FONT FONT_SMALL
#define OPTION_CURSOR_Y 49
#define OPTION_WINDOW_X 16
#define OPTION_CHOICES_LEFT 104
#define OPTION_CHOICES_RIGHT 198
#define OPTION_CANCEL_TEXT_X 10
#define OPTION_CURSOR_GAP 2
#define OPTION_CURSOR_HALF_WIDTH 4
#define TAG_OPTION_CURSOR 0x1000
#define OPTION_CURSOR_EASE_FRAMES 6

enum
{
    CURSOR_LEFT_X,
    CURSOR_RIGHT_X,
    CURSOR_Y,
    CURSOR_ANIM_COUNT,
};

enum
{
    MENUITEM_TEXTSPEED,
    MENUITEM_BATTLESCENE,
    MENUITEM_BATTLESTYLE,
    MENUITEM_SOUND,
    MENUITEM_BUTTONMODE,
    MENUITEM_FRAMETYPE,
    MENUITEM_LEVELCAPS,
    MENUITEM_TRAINERSCALING,
    MENUITEM_CANCEL,
    MENUITEM_COUNT,
};

enum
{
    WIN_HEADER,
    WIN_OPTIONS
};

STATIC_ASSERT(MENUITEM_COUNT * OPTION_ROW_HEIGHT <= 14 * 8, OptionMenuRowsFitWindow);

#define YPOS_TEXTSPEED    (MENUITEM_TEXTSPEED * OPTION_ROW_HEIGHT)
#define YPOS_BATTLESCENE  (MENUITEM_BATTLESCENE * OPTION_ROW_HEIGHT)
#define YPOS_BATTLESTYLE  (MENUITEM_BATTLESTYLE * OPTION_ROW_HEIGHT)
#define YPOS_SOUND        (MENUITEM_SOUND * OPTION_ROW_HEIGHT)
#define YPOS_BUTTONMODE   (MENUITEM_BUTTONMODE * OPTION_ROW_HEIGHT)
#define YPOS_FRAMETYPE    (MENUITEM_FRAMETYPE * OPTION_ROW_HEIGHT)
#define YPOS_LEVELCAPS    (MENUITEM_LEVELCAPS * OPTION_ROW_HEIGHT)
#define YPOS_TRAINERSCALING (MENUITEM_TRAINERSCALING * OPTION_ROW_HEIGHT)

static void Task_OptionMenuFadeIn(u8 taskId);
static void Task_OptionMenuProcessInput(u8 taskId);
static void Task_OptionMenuSave(u8 taskId);
static void Task_OptionMenuFadeOut(u8 taskId);
static void CreateOptionMenuCursors(u8 taskId);
static void UpdateOptionMenuCursors(u8 taskId);
static void DestroyOptionMenuCursors(u8 taskId);
static void SpriteCB_OptionMenuCursor(struct Sprite *sprite);
static u8 TextSpeed_ProcessInput(u8 selection);
static void TextSpeed_DrawChoices(u8 selection);
static u8 Toggle_ProcessInput(u8 selection);
static void BattleScene_DrawChoices(u8 selection);
static void Toggle_DrawChoices(u8 selection, u8 y);
static u8 BattleStyle_ProcessInput(u8 selection);
static void BattleStyle_DrawChoices(u8 selection);
static u8 Sound_ProcessInput(u8 selection);
static void Sound_DrawChoices(u8 selection);
static u8 FrameType_ProcessInput(u8 selection);
static void FrameType_DrawChoices(u8 selection);
static u8 ButtonMode_ProcessInput(u8 selection);
static void ButtonMode_DrawChoices(u8 selection);
static void DrawHeaderText(void);
static void DrawOptionMenuTexts(void);
static void DrawBgWindowFrames(void);

EWRAM_DATA static bool8 sArrowPressed = FALSE;
EWRAM_DATA static struct ComfyAnim sCursorAnims[CURSOR_ANIM_COUNT] = {0};

static const u8 gText_Option[]             = _("Option");
static const u8 gText_TextSpeedSlow[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Slow");
static const u8 gText_TextSpeedMid[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Mid");
static const u8 gText_TextSpeedFast[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Fast");
static const u8 gText_TextSpeedInstant[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Inst");
static const u8 gText_BattleSceneOn[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}On");
static const u8 gText_BattleSceneOff[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Off");
static const u8 gText_BattleStyleShift[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Shift");
static const u8 gText_BattleStyleSet[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Set");
static const u8 gText_SoundMono[]          = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Mono");
static const u8 gText_SoundStereo[]        = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Stereo");
static const u8 gText_FrameType[]          = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Type");
static const u8 gText_FrameTypeNumber[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}");
static const u8 gText_ButtonTypeNormal[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}Normal");
static const u8 gText_ButtonTypeLR[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}LR");
static const u8 gText_ButtonTypeLEqualsA[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}L=A");

static const u16 sOptionMenuText_Pal[] = INCGFX_U16("graphics/interface/option_menu_text.pal", ".gbapal");
static const u32 sOptionCursorTiles[] = INCGFX_U32("graphics/misc/arrow.png", ".4bpp");
static const u16 sOptionCursorPalette[] = INCGFX_U16("graphics/misc/arrow.pal", ".gbapal");

static const struct SpriteSheet sOptionCursorSpriteSheet =
{
    .data = sOptionCursorTiles,
    .size = sizeof(sOptionCursorTiles),
    .tag = TAG_OPTION_CURSOR,
};

static const struct SpritePalette sOptionCursorSpritePalette =
{
    .data = sOptionCursorPalette,
    .tag = TAG_OPTION_CURSOR,
};

static const struct OamData sOptionCursorOam =
{
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0,
};

static const union AnimCmd sOptionCursorLeftAnim[] =
{
    ANIMCMD_FRAME(0, 0, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sOptionCursorRightAnim[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sOptionCursorAnims[] =
{
    sOptionCursorLeftAnim,
    sOptionCursorRightAnim,
};

static const struct SpriteTemplate sOptionCursorSpriteTemplate =
{
    .tileTag = TAG_OPTION_CURSOR,
    .paletteTag = TAG_OPTION_CURSOR,
    .oam = &sOptionCursorOam,
    .anims = sOptionCursorAnims,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_OptionMenuCursor,
};
// note: this is only used in the Japanese release
static const u8 sEqualSignGfx[] = INCGFX_U8("graphics/interface/option_menu_equals_sign.png", ".4bpp");

static const u8 *const sOptionMenuItemsNames[MENUITEM_COUNT] =
{
    [MENUITEM_TEXTSPEED]   = COMPOUND_STRING("Text Speed"),
    [MENUITEM_BATTLESCENE] = COMPOUND_STRING("Battle Scene"),
    [MENUITEM_BATTLESTYLE] = COMPOUND_STRING("Battle Style"),
    [MENUITEM_SOUND]       = COMPOUND_STRING("Sound"),
    [MENUITEM_BUTTONMODE]  = COMPOUND_STRING("Button Mode"),
    [MENUITEM_FRAMETYPE]   = COMPOUND_STRING("Frame"),
    [MENUITEM_LEVELCAPS]   = COMPOUND_STRING("Level Caps"),
    [MENUITEM_TRAINERSCALING] = COMPOUND_STRING("Trainer Scaling"),
    [MENUITEM_CANCEL]      = COMPOUND_STRING("Cancel"),
};

static const struct WindowTemplate sOptionMenuWinTemplates[] =
{
    [WIN_HEADER] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 26,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    [WIN_OPTIONS] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 26,
        .height = 14,
        .paletteNum = 1,
        .baseBlock = 0x36
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sOptionMenuBgTemplates[] =
{
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    }
};

static const u16 sOptionMenuBg_Pal[] = {RGB(17, 18, 31)};

static void MainCB2(void)
{
    RunTasks();
    for (u32 i = 0; i < CURSOR_ANIM_COUNT; i++)
    {
        if (sCursorAnims[i].inUse)
            TryAdvanceComfyAnim(&sCursorAnims[i]);
    }
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitOptionMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sOptionMenuBgTemplates, ARRAY_COUNT(sOptionMenuBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ChangeBgX(2, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgX(3, 0, BG_COORD_SET);
        ChangeBgY(3, 0, BG_COORD_SET);
        InitWindows(sOptionMenuWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveData.optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sOptionMenuBg_Pal, BG_PLTT_ID(0), sizeof(sOptionMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveData.optionsWindowFrameType)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sOptionMenuText_Pal, BG_PLTT_ID(1), sizeof(sOptionMenuText_Pal));
        gMain.state++;
        break;
    case 6:
        PutWindowTilemap(WIN_HEADER);
        DrawHeaderText();
        gMain.state++;
        break;
    case 7:
        LoadSpriteSheet(&sOptionCursorSpriteSheet);
        LoadSpritePalette(&sOptionCursorSpritePalette);
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(WIN_OPTIONS);
        DrawOptionMenuTexts();
        gMain.state++;
        [[fallthrough]];
    case 9:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 10:
    {
        u8 taskId = CreateTask(Task_OptionMenuFadeIn, 0);

        gTasks[taskId].tMenuSelection = 0;
        gTasks[taskId].tTextSpeed = gSaveData.optionsTextSpeed;
        if (gTasks[taskId].tTextSpeed > OPTIONS_TEXT_SPEED_INSTANT)
            gTasks[taskId].tTextSpeed = OPTIONS_TEXT_SPEED_FAST;
        gTasks[taskId].tBattleSceneOff = gSaveData.optionsBattleSceneOff;
        gTasks[taskId].tBattleStyle = gSaveData.optionsBattleStyle;
        gTasks[taskId].tSound = gSaveData.optionsSound;
        gTasks[taskId].tButtonMode = gSaveData.optionsButtonMode;
        gTasks[taskId].tWindowFrameType = gSaveData.optionsWindowFrameType;
        gTasks[taskId].tLevelCaps = gSaveData.optionsLevelCaps;
        gTasks[taskId].tTrainerScaling = gSaveData.optionsTrainerScaling;

        TextSpeed_DrawChoices(gTasks[taskId].tTextSpeed);
        BattleScene_DrawChoices(gTasks[taskId].tBattleSceneOff);
        BattleStyle_DrawChoices(gTasks[taskId].tBattleStyle);
        Sound_DrawChoices(gTasks[taskId].tSound);
        ButtonMode_DrawChoices(gTasks[taskId].tButtonMode);
        FrameType_DrawChoices(gTasks[taskId].tWindowFrameType);
        Toggle_DrawChoices(gTasks[taskId].tLevelCaps, YPOS_LEVELCAPS);
        Toggle_DrawChoices(gTasks[taskId].tTrainerScaling, YPOS_TRAINERSCALING);
        CreateOptionMenuCursors(taskId);

        CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

static void Task_OptionMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_OptionMenuProcessInput;
}

static void Task_OptionMenuProcessInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tMenuSelection == MENUITEM_CANCEL)
            gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_OptionMenuSave;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        if (gTasks[taskId].tMenuSelection > 0)
            gTasks[taskId].tMenuSelection--;
        else
            gTasks[taskId].tMenuSelection = MENUITEM_CANCEL;
        UpdateOptionMenuCursors(taskId);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        if (gTasks[taskId].tMenuSelection < MENUITEM_CANCEL)
            gTasks[taskId].tMenuSelection++;
        else
            gTasks[taskId].tMenuSelection = 0;
        UpdateOptionMenuCursors(taskId);
    }
    else
    {
        u8 previousOption;

        switch (gTasks[taskId].tMenuSelection)
        {
        case MENUITEM_TEXTSPEED:
            previousOption = gTasks[taskId].tTextSpeed;
            gTasks[taskId].tTextSpeed = TextSpeed_ProcessInput(gTasks[taskId].tTextSpeed);

            if (previousOption != gTasks[taskId].tTextSpeed)
                TextSpeed_DrawChoices(gTasks[taskId].tTextSpeed);
            break;
        case MENUITEM_BATTLESCENE:
            previousOption = gTasks[taskId].tBattleSceneOff;
            gTasks[taskId].tBattleSceneOff = Toggle_ProcessInput(gTasks[taskId].tBattleSceneOff);

            if (previousOption != gTasks[taskId].tBattleSceneOff)
                BattleScene_DrawChoices(gTasks[taskId].tBattleSceneOff);
            break;
        case MENUITEM_BATTLESTYLE:
            previousOption = gTasks[taskId].tBattleStyle;
            gTasks[taskId].tBattleStyle = BattleStyle_ProcessInput(gTasks[taskId].tBattleStyle);

            if (previousOption != gTasks[taskId].tBattleStyle)
                BattleStyle_DrawChoices(gTasks[taskId].tBattleStyle);
            break;
        case MENUITEM_SOUND:
            previousOption = gTasks[taskId].tSound;
            gTasks[taskId].tSound = Sound_ProcessInput(gTasks[taskId].tSound);

            if (previousOption != gTasks[taskId].tSound)
                Sound_DrawChoices(gTasks[taskId].tSound);
            break;
        case MENUITEM_BUTTONMODE:
            previousOption = gTasks[taskId].tButtonMode;
            gTasks[taskId].tButtonMode = ButtonMode_ProcessInput(gTasks[taskId].tButtonMode);

            if (previousOption != gTasks[taskId].tButtonMode)
                ButtonMode_DrawChoices(gTasks[taskId].tButtonMode);
            break;
        case MENUITEM_FRAMETYPE:
            previousOption = gTasks[taskId].tWindowFrameType;
            gTasks[taskId].tWindowFrameType = FrameType_ProcessInput(gTasks[taskId].tWindowFrameType);

            if (previousOption != gTasks[taskId].tWindowFrameType)
                FrameType_DrawChoices(gTasks[taskId].tWindowFrameType);
            break;
        case MENUITEM_LEVELCAPS:
            previousOption = gTasks[taskId].tLevelCaps;
            gTasks[taskId].tLevelCaps = Toggle_ProcessInput(previousOption);
            if (previousOption != gTasks[taskId].tLevelCaps)
                Toggle_DrawChoices(gTasks[taskId].tLevelCaps, YPOS_LEVELCAPS);
            break;
        case MENUITEM_TRAINERSCALING:
            previousOption = gTasks[taskId].tTrainerScaling;
            gTasks[taskId].tTrainerScaling = Toggle_ProcessInput(previousOption);
            if (previousOption != gTasks[taskId].tTrainerScaling)
                Toggle_DrawChoices(gTasks[taskId].tTrainerScaling, YPOS_TRAINERSCALING);
            break;
        default:
            return;
        }

        if (sArrowPressed)
        {
            PlaySE(SE_SELECT);
            sArrowPressed = FALSE;
            UpdateOptionMenuCursors(taskId);
            CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
        }
    }
}

static void Task_OptionMenuSave(u8 taskId)
{
    gSaveData.optionsTextSpeed = gTasks[taskId].tTextSpeed;
    gSaveData.optionsBattleSceneOff = gTasks[taskId].tBattleSceneOff;
    gSaveData.optionsBattleStyle = gTasks[taskId].tBattleStyle;
    gSaveData.optionsSound = gTasks[taskId].tSound;
    gSaveData.optionsButtonMode = gTasks[taskId].tButtonMode;
    gSaveData.optionsWindowFrameType = gTasks[taskId].tWindowFrameType;
    gSaveData.optionsLevelCaps = gTasks[taskId].tLevelCaps;
    gSaveData.optionsTrainerScaling = gTasks[taskId].tTrainerScaling;

    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_OptionMenuFadeOut;
}

static void Task_OptionMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyOptionMenuCursors(taskId);
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

static void CreateOptionMenuCursors(u8 taskId)
{
    memset(sCursorAnims, 0, sizeof(sCursorAnims));
    u8 left = CreateSprite(&sOptionCursorSpriteTemplate, 0, OPTION_CURSOR_Y, 0);
    u8 right = CreateSprite(&sOptionCursorSpriteTemplate, 0, OPTION_CURSOR_Y, 0);
    gTasks[taskId].tLeftArrowSpriteId = left;
    gTasks[taskId].tRightArrowSpriteId = right;
    if (left < MAX_SPRITES)
    {
        gSprites[left].data[0] = CURSOR_LEFT_X;
        StartSpriteAnim(&gSprites[left], 0);
    }
    if (right < MAX_SPRITES)
    {
        gSprites[right].data[0] = CURSOR_RIGHT_X;
        StartSpriteAnim(&gSprites[right], 1);
    }
    UpdateOptionMenuCursors(taskId);
    if (left < MAX_SPRITES)
        SpriteCB_OptionMenuCursor(&gSprites[left]);
    if (right < MAX_SPRITES)
        SpriteCB_OptionMenuCursor(&gSprites[right]);
}

static void SetOptionCursorTarget(u32 axis, s16 target)
{
    struct ComfyAnim *anim = &sCursorAnims[axis];
    struct ComfyAnimEasingConfig config;
    s32 destination = Q_24_8(target);

    if (anim->inUse && anim->config.data.easing.to == destination)
        return;

    InitComfyAnimConfig_Easing(&config);
    config.from = anim->inUse ? anim->position : destination;
    config.to = destination;
    config.durationFrames = OPTION_CURSOR_EASE_FRAMES;
    config.easingFunc = ComfyAnimEasing_EaseOutCubic;
    InitComfyAnim_Easing(&config, anim);
}

static void SpriteCB_OptionMenuCursor(struct Sprite *sprite)
{
    sprite->x = ReadComfyAnimValueSmooth(&sCursorAnims[sprite->data[0]]);
    sprite->y = ReadComfyAnimValueSmooth(&sCursorAnims[CURSOR_Y]);
}

static void UpdateOptionMenuCursors(u8 taskId)
{
    u8 selection = gTasks[taskId].tMenuSelection;
    s16 y = OPTION_CURSOR_Y + selection * OPTION_ROW_HEIGHT;
    s16 leftEdge = OPTION_WINDOW_X + OPTION_CHOICES_LEFT;
    s16 rightEdge = OPTION_WINDOW_X + OPTION_CHOICES_RIGHT;

    if (selection == MENUITEM_FRAMETYPE)
    {
        u8 number[3];
        ConvertIntToDecimalStringN(number, gTasks[taskId].tWindowFrameType + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        rightEdge = OPTION_WINDOW_X + 128 + GetStringWidth(OPTION_FONT, number, 0);
    }
    else if (selection == MENUITEM_CANCEL)
    {
        // Cancel is an action, not a category with separate choices. Bracket
        // its text, inset enough to keep both arrows inside the white window.
        leftEdge = OPTION_WINDOW_X + OPTION_CANCEL_TEXT_X;
        rightEdge = leftEdge + GetStringWidth(OPTION_FONT, sOptionMenuItemsNames[MENUITEM_CANCEL], 0);
    }

    SetOptionCursorTarget(CURSOR_LEFT_X, leftEdge - OPTION_CURSOR_GAP - OPTION_CURSOR_HALF_WIDTH);
    SetOptionCursorTarget(CURSOR_RIGHT_X, rightEdge + OPTION_CURSOR_GAP + OPTION_CURSOR_HALF_WIDTH);
    SetOptionCursorTarget(CURSOR_Y, y);
}

static void DestroyOptionMenuCursors(u8 taskId)
{
    memset(sCursorAnims, 0, sizeof(sCursorAnims));
    u8 left = gTasks[taskId].tLeftArrowSpriteId;
    u8 right = gTasks[taskId].tRightArrowSpriteId;
    if (left < MAX_SPRITES)
        DestroySprite(&gSprites[left]);
    if (right < MAX_SPRITES)
        DestroySprite(&gSprites[right]);
    FreeSpriteTilesByTag(TAG_OPTION_CURSOR);
    FreeSpritePaletteByTag(TAG_OPTION_CURSOR);
}

static void DrawOptionMenuChoice(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[16];
    u16 i;

    for (i = 0; *text != EOS && i < ARRAY_COUNT(dst) - 1; i++)
        dst[i] = *(text++);

    if (style != 0)
    {
        dst[2] = TEXT_COLOR_RED;
        dst[5] = TEXT_COLOR_LIGHT_RED;
    }

    dst[i] = EOS;
    AddTextPrinterParameterized(WIN_OPTIONS, OPTION_FONT, dst, x, y + 1, TEXT_SKIP_DRAW, NULL);
}

static u8 TextSpeed_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < OPTIONS_TEXT_SPEED_INSTANT)
            selection++;
        else
            selection = 0;

        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = OPTIONS_TEXT_SPEED_INSTANT;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void TextSpeed_DrawChoices(u8 selection)
{
    static const u8 *const choices[] =
    {
        [OPTIONS_TEXT_SPEED_SLOW] = gText_TextSpeedSlow,
        [OPTIONS_TEXT_SPEED_MID] = gText_TextSpeedMid,
        [OPTIONS_TEXT_SPEED_FAST] = gText_TextSpeedFast,
        [OPTIONS_TEXT_SPEED_INSTANT] = gText_TextSpeedInstant,
    };
    s32 widths[ARRAY_COUNT(choices)];
    s32 freeSpace = OPTION_CHOICES_RIGHT - OPTION_CHOICES_LEFT;
    s32 x = OPTION_CHOICES_LEFT;

    for (u32 i = 0; i < ARRAY_COUNT(choices); i++)
    {
        widths[i] = GetStringWidth(OPTION_FONT, choices[i], 0);
        freeSpace -= widths[i];
    }
    for (u32 i = 0; i < ARRAY_COUNT(choices); i++)
    {
        DrawOptionMenuChoice(choices[i], x + freeSpace * i / (ARRAY_COUNT(choices) - 1), YPOS_TEXTSPEED, selection == i);
        x += widths[i];
    }
}

static u8 Toggle_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void BattleScene_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleSceneOn, OPTION_CHOICES_LEFT, YPOS_BATTLESCENE, styles[0]);
    DrawOptionMenuChoice(gText_BattleSceneOff, GetStringRightAlignXOffset(OPTION_FONT, gText_BattleSceneOff, OPTION_CHOICES_RIGHT), YPOS_BATTLESCENE, styles[1]);
}

static void Toggle_DrawChoices(u8 selection, u8 y)
{
    DrawOptionMenuChoice(gText_BattleSceneOff, OPTION_CHOICES_LEFT, y, selection == FALSE);
    DrawOptionMenuChoice(gText_BattleSceneOn, GetStringRightAlignXOffset(OPTION_FONT, gText_BattleSceneOn, OPTION_CHOICES_RIGHT), y, selection == TRUE);
}

static u8 BattleStyle_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        sArrowPressed = TRUE;
    }

    return selection;
}

static void BattleStyle_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_BattleStyleShift, OPTION_CHOICES_LEFT, YPOS_BATTLESTYLE, styles[0]);
    DrawOptionMenuChoice(gText_BattleStyleSet, GetStringRightAlignXOffset(OPTION_FONT, gText_BattleStyleSet, OPTION_CHOICES_RIGHT), YPOS_BATTLESTYLE, styles[1]);
}

static u8 Sound_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT))
    {
        selection ^= 1;
        SetPokemonCryStereo(selection);
        sArrowPressed = TRUE;
    }

    return selection;
}

static void Sound_DrawChoices(u8 selection)
{
    u8 styles[2];

    styles[0] = 0;
    styles[1] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_SoundMono, OPTION_CHOICES_LEFT, YPOS_SOUND, styles[0]);
    DrawOptionMenuChoice(gText_SoundStereo, GetStringRightAlignXOffset(OPTION_FONT, gText_SoundStereo, OPTION_CHOICES_RIGHT), YPOS_SOUND, styles[1]);
}

static u8 FrameType_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection < WINDOW_FRAMES_COUNT - 1)
            selection++;
        else
            selection = 0;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = WINDOW_FRAMES_COUNT - 1;

        LoadBgTiles(1, GetWindowFrameTilesPal(selection)->tiles, 0x120, 0x1A2);
        LoadPalette(GetWindowFrameTilesPal(selection)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        sArrowPressed = TRUE;
    }
    return selection;
}

static void FrameType_DrawChoices(u8 selection)
{
    u8 text[16] = {EOS};
    u8 n = selection + 1;
    u16 i;

    for (i = 0; gText_FrameTypeNumber[i] != EOS && i <= 5; i++)
        text[i] = gText_FrameTypeNumber[i];

    // Convert a number to decimal string
    if (n / 10 != 0)
    {
        text[i] = n / 10 + CHAR_0;
        i++;
        text[i] = n % 10 + CHAR_0;
        i++;
    }
    else
    {
        text[i] = n % 10 + CHAR_0;
        i++;
        text[i] = CHAR_SPACER;
        i++;
    }

    text[i] = EOS;

    DrawOptionMenuChoice(gText_FrameType, OPTION_CHOICES_LEFT, YPOS_FRAMETYPE, 0);
    DrawOptionMenuChoice(text, 128, YPOS_FRAMETYPE, 1);
}

static u8 ButtonMode_ProcessInput(u8 selection)
{
    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selection <= 1)
            selection++;
        else
            selection = 0;

        sArrowPressed = TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        if (selection != 0)
            selection--;
        else
            selection = 2;

        sArrowPressed = TRUE;
    }
    return selection;
}

static void ButtonMode_DrawChoices(u8 selection)
{
    s32 widthNormal, widthLR, widthLA, xLR;
    u8 styles[3];

    styles[0] = 0;
    styles[1] = 0;
    styles[2] = 0;
    styles[selection] = 1;

    DrawOptionMenuChoice(gText_ButtonTypeNormal, OPTION_CHOICES_LEFT, YPOS_BUTTONMODE, styles[0]);

    widthNormal = GetStringWidth(OPTION_FONT, gText_ButtonTypeNormal, 0);
    widthLR = GetStringWidth(OPTION_FONT, gText_ButtonTypeLR, 0);
    widthLA = GetStringWidth(OPTION_FONT, gText_ButtonTypeLEqualsA, 0);

    widthLR -= OPTION_CHOICES_RIGHT - OPTION_CHOICES_LEFT;
    xLR = (widthNormal - widthLR - widthLA) / 2 + OPTION_CHOICES_LEFT;
    DrawOptionMenuChoice(gText_ButtonTypeLR, xLR, YPOS_BUTTONMODE, styles[1]);

    DrawOptionMenuChoice(gText_ButtonTypeLEqualsA, GetStringRightAlignXOffset(OPTION_FONT, gText_ButtonTypeLEqualsA, OPTION_CHOICES_RIGHT), YPOS_BUTTONMODE, styles[2]);
}

static void DrawHeaderText(void)
{
    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_HEADER, FONT_NORMAL, gText_Option, 8, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawOptionMenuTexts(void)
{
    u8 i;

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
    for (i = 0; i < MENUITEM_COUNT; i++)
        AddTextPrinterParameterized(WIN_OPTIONS, OPTION_FONT, sOptionMenuItemsNames[i], i == MENUITEM_CANCEL ? OPTION_CANCEL_TEXT_X : 8, (i * OPTION_ROW_HEIGHT) + 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

#define TILE_TOP_CORNER_L 0x1A2
#define TILE_TOP_EDGE     0x1A3
#define TILE_TOP_CORNER_R 0x1A4
#define TILE_LEFT_EDGE    0x1A5
#define TILE_RIGHT_EDGE   0x1A7
#define TILE_BOT_CORNER_L 0x1A8
#define TILE_BOT_EDGE     0x1A9
#define TILE_BOT_CORNER_R 0x1AA

static void DrawBgWindowFrames(void)
{
    //                     bg, tile,              x, y, width, height, palNum
    // Draw title window frame
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  0, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1,  3,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2,  3, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28,  3,  1,  1,  7);

    // Draw options list window frame
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  4, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  5,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  5,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 19,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 19, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 19,  1,  1,  7);

    CopyBgTilemapBufferToVram(1);
}
