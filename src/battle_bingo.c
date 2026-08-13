#include "global.h"
#include "battle.h"
#include "battle_bingo.h"
#include "battle_factory_screen.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "bg.h"
#include "bingo_mons.h"
#include "data.h"
#include "gpu_regs.h"
#include "item.h"
#include "item_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "random.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trainer_util.h"
#include "window.h"
#include "international_string_util.h"
#include "constants/bingo_mons.h"
#include "constants/characters.h"
#include "constants/battle_ai.h"
#include "constants/party_menu.h"
#include "constants/pokemon.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define TAG_BINGO_SQUARE_GFX_START  0x3000
#define TAG_BINGO_SQUARE_PAL_1      0x3040
#define TAG_BINGO_SQUARE_PAL_2      0x3041
#define TAG_BINGO_HP_BAR_PAL_GREEN  0x3017
#define TAG_BINGO_HP_BAR_PAL_YELLOW 0x3018
#define TAG_BINGO_HP_BAR_PAL_RED    0x3019
#define TAG_BINGO_HP_BAR_GFX        0x301A
#define TAG_BINGO_TEXT_PAL          0x301B
#define TAG_BINGO_TEXT_GFX_START    0x301C
#define TAG_BINGO_CURSOR_GFX        0x3021
#define TAG_BINGO_CURSOR_PAL        0x3022
#define TAG_BINGO_LINE_GFX_START    0x3023
#define TAG_BINGO_LINE_PAL          0x3027
#define BINGO_SQUARE_SPRITE_SIZE    (16 * 16 / 2)
#define BINGO_LINE_SPRITE_SIZE      (16 * 16 / 2)
#define BINGO_MAX_LINE_SPRITES      96
#define BINGO_HP_BAR_FRAME_COUNT     8
#define BINGO_HP_BAR_SEGMENTS        4
#define BINGO_HP_BAR_SEGMENT_WIDTH   8
#define BINGO_HP_BAR_SEGMENT_SIZE    (8 * 8 / 2)
#define BINGO_HP_BAR_SPRITE_SIZE     (BINGO_HP_BAR_FRAME_COUNT * BINGO_HP_BAR_SEGMENT_SIZE)
#define BINGO_HP_BAR_WIDTH           (BINGO_HP_BAR_SEGMENTS * BINGO_HP_BAR_SEGMENT_WIDTH)
#define BINGO_HP_BAR_PALETTE_SIZE    16
#define BINGO_CURSOR_SPRITE_SIZE     0x800
#define BINGO_TEXT_SPRITE_WIDTH      32
#define BINGO_TEXT_SPRITE_HEIGHT     16
#define BINGO_TEXT_SPRITE_SIZE       (BINGO_TEXT_SPRITE_WIDTH * BINGO_TEXT_SPRITE_HEIGHT / 2)
#define BINGO_TITLE_TEXT_SPRITE_WIDTH  64
#define BINGO_TITLE_TEXT_SPRITE_HEIGHT 32
#define BINGO_TITLE_TEXT_SPRITE_SIZE   (BINGO_TITLE_TEXT_SPRITE_WIDTH * BINGO_TITLE_TEXT_SPRITE_HEIGHT / 2)
#define BINGO_TEXT_Y_OFFSET          0
#define BINGO_TITLE_TEXT_Y_OFFSET    0
#define BINGO_BOARD_SIZE            5

#define tCursorArea data[0]
#define tCursorX data[1]
#define tCursorY data[2]
#define tCursorSpriteId data[3]
#define tSelectedPartyIndex data[4]
#define tSelectedSquareRow data[5]
#define tSelectedSquareCol data[6]
#define tExitResult data[7]

enum
{
    BINGO_TEXT_BAG,
    BINGO_TEXT_QUIT,
    BINGO_TEXT_POINTS,
    BINGO_TEXT_POINT_VALUE,
    BINGO_TEXT_CARD_TITLE,
    BINGO_TEXT_COUNT
};

enum
{
    BINGO_WIN_MESSAGE,
};

enum
{
    BINGO_CURSOR_AREA_BOARD,
    BINGO_CURSOR_AREA_PARTY,
    BINGO_CURSOR_AREA_BAG,
    BINGO_CURSOR_AREA_QUIT,
};

enum
{
    BINGO_SQUARE_BOSS,
    BINGO_SQUARE_BUG,
    BINGO_SQUARE_COMPLETE,
    BINGO_SQUARE_DARK,
    BINGO_SQUARE_DRAGON,
    BINGO_SQUARE_ELECTRIC,
    BINGO_SQUARE_FAIRY,
    BINGO_SQUARE_FIGHTING,
    BINGO_SQUARE_FIRE,
    BINGO_SQUARE_FLYING,
    BINGO_SQUARE_GHOST,
    BINGO_SQUARE_GRASS,
    BINGO_SQUARE_GROUND,
    BINGO_SQUARE_ICE,
    BINGO_SQUARE_ITEM,
    BINGO_SQUARE_MYSTERY,
    BINGO_SQUARE_NORMAL,
    BINGO_SQUARE_POISON,
    BINGO_SQUARE_PSYCHIC,
    BINGO_SQUARE_ROCK,
    BINGO_SQUARE_STEEL,
    BINGO_SQUARE_WATER,
    BINGO_SQUARE_COUNT
};

enum
{
    BINGO_RUNTIME_WILD,
    BINGO_RUNTIME_ITEM,
    BINGO_RUNTIME_BOSS,
};

enum
{
    BINGO_LINE_HORIZONTAL,
    BINGO_LINE_VERTICAL,
    BINGO_LINE_DIAGONAL_DOWN_RIGHT,
    BINGO_LINE_DIAGONAL_DOWN_LEFT,
    BINGO_LINE_GFX_COUNT,
};

struct BattleBingoSquareState
{
    u8 visibleSquare;
    u8 kind;
    bool8 isMystery;
    bool8 cleared;
    u8 spriteId;
    u16 bingoMonId;
    u16 item;
    u16 trainer;
};

struct BattleBingoRuntime
{
    bool8 initialized;
    bool8 playBoardMusic;
    bool8 pickingPartner;
    bool8 partnerPickLocked;
    bool8 partyBackedUp;
    bool8 boardCompletePending;
    bool8 bagBackedUp;
    u8 boardId;
    u8 selectedRow;
    u8 selectedCol;
    u8 selectedPartyIndex;
    u8 cursorArea;
    u8 cursorX;
    u8 cursorY;
    u8 originalPartyCount;
    u8 bingoCount;
    u8 lineSpriteCount;
    u16 completedLineFlags;
    struct BattleBingoSquareState squares[BINGO_BOARD_SIZE][BINGO_BOARD_SIZE];
    struct Pokemon partyBackup[PARTY_SIZE];
    struct Bag bagBackup;
    struct Pokemon wildBattleMon;
    bool8 wildBattleMonValid;
    u8 textSpriteIds[BINGO_TEXT_COUNT];
    u8 lineSpriteIds[BINGO_MAX_LINE_SPRITES];
    u8 message[64];
};

static void CB2_InitBattleBingoBoard(void);
static void CB2_WaitForBattleBingoBoardFadeOut(void);
static void CB2_RunBattleBingoBoard(void);
static void CB2_ReturnToBattleBingoBoard(void);
static void CB2_ReturnToBattleBingoBoardAfterBattle(void);
static void VBlankCB_BattleBingoBoard(void);
static void Task_BattleBingoBoard(u8 taskId);
static void Task_BattleBingoBoardFadeOut(u8 taskId);
static void Task_BattleBingoBoardFadeToBag(u8 taskId);
static void Task_BattleBingoBoardFadeToSummary(u8 taskId);
static void Task_BattleBingoBoardWaitForBattleTransition(u8 taskId);
static void Task_BattleBingoBoardWaitItemFanfare(u8 taskId);
static void Task_BattleBingoBoardMessage(u8 taskId);
static void Task_BattleBingoBoardCompleteMessage(u8 taskId);
static void Task_BattleBingoBoardCompleteFadeOut(u8 taskId);
static void ExitBattleBingoBoard(u8 taskId);
static void StartBattleBingoRunExit(u8 taskId, enum BattleBingoResult result);
static void FinishBattleBingoRun(enum BattleBingoResult result);
static enum BattleBingoResult GetBattleBingoExitResultByScore(void);
static void BackupBattleBingoBag(void);
static void GiveBattleBingoStartingItems(const struct BattleBingoBoardRules *rules);
static void RestoreBattleBingoBag(void);
static void WipeBattleBingoParty(void);
static void CloseBattleBingoBoardForSubmenu(u8 taskId);
static void LoadBattleBingoSquareSprites(void);
static void LoadBattleBingoHpBarSprites(void);
static void LoadBattleBingoTextSprites(void);
static void LoadBattleBingoCursorSprite(void);
static void LoadBattleBingoLineSprites(void);
static void InitBattleBingoHpBarPalette(u16 *dest, u8 colorBase);
static void CreateBattleBingoBoardSprites(void);
static u8 CreateBattleBingoSquareSprite(u8 row, u8 col, u8 squareId);
static void CreateBattleBingoCompletedLineSprites(void);
static void CreateBattleBingoLineSprites(u8 line);
static void CreateBattleBingoLineChunk(s16 x, s16 y, u8 lineGfx);
static void CreateBattleBingoPartyIcons(void);
static void CreateBattleBingoPartyHpBars(void);
static void CreateBattleBingoPartyStatusIcons(void);
static void CreateBattleBingoHpBar(u8 x, u8 y, u8 hpPercent);
static u8 CreateBattleBingoCursorSprite(void);
static void RememberBattleBingoCursor(u8 area, u8 x, u8 y);
static void RestoreBattleBingoCursor(u8 taskId);
static void MoveBattleBingoCursor(u8 taskId, s8 deltaX, s8 deltaY);
static void UpdateBattleBingoCursorSpritePosition(u8 taskId);
static void HandleBattleBingoCursorSelect(u8 taskId);
static void HandleBattleBingoPartnerPickInput(u8 taskId);
static void InitBattleBingoRuntime(enum BattleBingoBoardId boardId);
static u16 FindBattleBingoMonByTags(u64 requiredTags, u64 forbiddenTags, u16 skip);
static u16 GetBattleBingoItemByIndex(const struct BattleBingoBoardRules *rules, u8 itemIndex);
static u16 GetBattleBingoBoardMusic(const struct BattleBingoBoardRules *rules);
static u8 CountBattleBingoItemRules(const struct BattleBingoBoardRules *rules);
static u8 GetBattleBingoTypeSquare(u8 type);
static void UpdateBattleBingoLines(bool8 drawNewLines);
static bool8 IsBattleBingoLineCleared(u8 line);
static bool8 IsBattleBingoBoardComplete(void);
static u32 GetBattleBingoClearedSquareMask(void);
static void UpdateBattleBingoPointText(void);
static void StartBattleBingoPartnerPick(u8 taskId, bool8 locked);
static void CancelBattleBingoPartnerPick(u8 taskId);
static void ClearBattleBingoSelectedSquare(bool8 drawNewLines);
static void TryStartBattleBingoBoardComplete(u8 taskId);
static void ShowBattleBingoPrompt(const u8 *text);
static void HideBattleBingoPrompt(void);
static void GiveBattleBingoSelectedItem(u8 taskId);
static void StartBattleBingoWildBattle(u8 taskId);
static void StartBattleBingoBossBattle(u8 taskId);
static void PrepareBattleBingoWildParty(u16 bingoMonId);
static void BackupBattleBingoPartnerParty(u8 partyIndex);
static bool8 TryGetBattleBingoCaughtMon(struct Pokemon *capturedMon);
static void RestoreBattleBingoPartnerParty(bool8 captured);
static bool8 BattleBingoPlayerHasRevives(void);
static bool8 BattleBingoPlayerHasUsableMons(void);
static void CreateBattleBingoTextLabels(void);
static u8 CreateBattleBingoTextLabel(u8 labelId, s16 x, s16 y, const u8 *text);
static u8 CreateBattleBingoTitleTextLabel(u8 labelId, s16 x, s16 y, const u8 *text);

static const u32 sBattleBingoBoard_Gfx[] = INCGFX_U32("graphics/bingo/board/board_tiles.png", ".4bpp.smol");
static const u16 sBattleBingoBoard_Pal[] = INCGFX_U16("graphics/bingo/board/board_tiles.png", ".gbapal");
static const u32 sBattleBingoBoard_Tilemap[] = INCGFX_U32("graphics/bingo/board/board_tiles.bin", ".smolTM");
static const u32 sBattleBingoHpBar_Gfx[] = INCGFX_U32("graphics/bingo/board/hp_bar.png", ".4bpp");
static const u16 sBattleBingoHpBar_Pal[] = INCGFX_U16("graphics/bingo/board/hp_bar.pal", ".gbapal");
static const u8 sBattleBingoCursor_Gfx[] = INCGFX_U8("graphics/pokemon_storage/hand_cursor.png", ".4bpp");
static const u16 sBattleBingoCursor_Pal[] = INCGFX_U16("graphics/pokemon_storage/hand_cursor.png", ".gbapal");
static u16 sBattleBingoHpBarYellowPal[BINGO_HP_BAR_PALETTE_SIZE];
static u16 sBattleBingoHpBarRedPal[BINGO_HP_BAR_PALETTE_SIZE];
static const u32 sBattleBingoText_Gfx[BINGO_TEXT_SPRITE_SIZE / sizeof(u32)] = {0};
static const u32 sBattleBingoTitleText_Gfx[BINGO_TITLE_TEXT_SPRITE_SIZE / sizeof(u32)] = {0};
static const u16 sBattleBingoText_Pal[] = {RGB_BLACK, RGB_WHITE, RGB(5, 5, 5), RGB_GRAY};
static const u16 sBattleBingoLine_Pal[] = INCGFX_U16("graphics/bingo/board/bingo_bar.pal", ".gbapal");
static const u8 sBattleBingoText_Bag[] = _("Bag");
static const u8 sBattleBingoText_Quit[] = _("Quit");
static const u8 sBattleBingoText_Points[] = _("Points");
static const u8 sBattleBingoText_PickPartner[] = _("Pick a partner!");
static const u8 sBattleBingoText_GotItem[] = _("You got a {STR_VAR_1}!");
static const u8 sBattleBingoText_BoardComplete[] = _("Board Complete!");
static const u8 sBattleBingoText_BossIntro[] = _("The Bingo Boss blocks your path!");
static const u8 sBattleBingoText_BossDefeat[] = _("The Bingo Boss was defeated!");
static EWRAM_DATA struct BattleBingoRuntime sBattleBingo = {0};
static EWRAM_DATA u16 sBattleBingoLastResult = BATTLE_BINGO_RESULT_LOSS;
static EWRAM_DATA u16 sBattleBingoLastPrizeMoney = 0;

static const u32 sBattleBingoSquareBoss_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_boss.png", ".4bpp");
static const u32 sBattleBingoSquareBug_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_bug.png", ".4bpp");
static const u32 sBattleBingoSquareComplete_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_complete.png", ".4bpp");
static const u32 sBattleBingoSquareDark_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_dark.png", ".4bpp");
static const u32 sBattleBingoSquareDragon_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_dragon.png", ".4bpp");
static const u32 sBattleBingoSquareElectric_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_electric.png", ".4bpp");
static const u32 sBattleBingoSquareFairy_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_fairy.png", ".4bpp");
static const u32 sBattleBingoSquareFighting_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_fighting.png", ".4bpp");
static const u32 sBattleBingoSquareFire_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_fire.png", ".4bpp");
static const u32 sBattleBingoSquareFlying_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_flying.png", ".4bpp");
static const u32 sBattleBingoSquareGhost_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_ghost.png", ".4bpp");
static const u32 sBattleBingoSquareGrass_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_grass.png", ".4bpp");
static const u32 sBattleBingoSquareGround_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_ground.png", ".4bpp");
static const u32 sBattleBingoSquareIce_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_ice.png", ".4bpp");
static const u32 sBattleBingoSquareItem_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_item.png", ".4bpp");
static const u32 sBattleBingoSquareMystery_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_mystery.png", ".4bpp");
static const u32 sBattleBingoSquareNormal_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_normal.png", ".4bpp");
static const u32 sBattleBingoSquarePoison_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_poison.png", ".4bpp");
static const u32 sBattleBingoSquarePsychic_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_psychic.png", ".4bpp");
static const u32 sBattleBingoSquareRock_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_rock.png", ".4bpp");
static const u32 sBattleBingoSquareSteel_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_steel.png", ".4bpp");
static const u32 sBattleBingoSquareWater_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_water.png", ".4bpp");
static const u32 sBattleBingoLineHorizontal_Gfx[] = INCGFX_U32("graphics/bingo/board/horizontal_bingo.png", ".4bpp");
static const u32 sBattleBingoLineVertical_Gfx[] = INCGFX_U32("graphics/bingo/board/vertical_bingo.png", ".4bpp");
static const u32 sBattleBingoLineDiagonalDownRight_Gfx[] = INCGFX_U32("graphics/bingo/board/diagonal_down_right_bingo.png", ".4bpp");
static const u32 sBattleBingoLineDiagonalDownLeft_Gfx[] = INCGFX_U32("graphics/bingo/board/diagonal_down_left_bingo.png", ".4bpp");

static const u16 sBattleBingoSquarePal1[] = INCGFX_U16("graphics/bingo/squares/bingo_square_palette_1.pal", ".gbapal");
static const u16 sBattleBingoSquarePal2[] = INCGFX_U16("graphics/bingo/squares/bingo_square_palette_2.pal", ".gbapal");

static const struct BgTemplate sBattleBingoBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 2,
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .priority = 0,
    },
};

static const struct WindowTemplate sBattleBingoWindowTemplates[] =
{
    [BINGO_WIN_MESSAGE] =
    {
        .bg = 1,
        .tilemapLeft = 14,
        .tilemapTop = 1,
        .width = 15,
        .height = 4,
        .paletteNum = DLG_WINDOW_PALETTE_NUM,
        .baseBlock = 0x194,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct OamData sBattleBingoSquareOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const struct OamData sBattleBingoHpBarOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,
};

static const struct OamData sBattleBingoTextOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
    .priority = 1,
};

static const struct OamData sBattleBingoLineOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const struct OamData sBattleBingoCursorOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
};

static const struct OamData sBattleBingoTitleTextOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
    .priority = 1,
};

static const union AnimCmd sBattleBingoSquareAnim[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBattleBingoSquareAnimTable[] =
{
    sBattleBingoSquareAnim,
};

static const union AnimCmd sBattleBingoHpBarAnim0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim1[] =
{
    ANIMCMD_FRAME(1, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim2[] =
{
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim3[] =
{
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim4[] =
{
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim5[] =
{
    ANIMCMD_FRAME(5, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim6[] =
{
    ANIMCMD_FRAME(6, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoHpBarAnim7[] =
{
    ANIMCMD_FRAME(7, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBattleBingoHpBarAnimTable[] =
{
    sBattleBingoHpBarAnim0,
    sBattleBingoHpBarAnim1,
    sBattleBingoHpBarAnim2,
    sBattleBingoHpBarAnim3,
    sBattleBingoHpBarAnim4,
    sBattleBingoHpBarAnim5,
    sBattleBingoHpBarAnim6,
    sBattleBingoHpBarAnim7,
};

static const union AnimCmd sBattleBingoTextAnim[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd sBattleBingoCursorAnim[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sBattleBingoCursorAnimTable[] =
{
    sBattleBingoCursorAnim,
};

static const union AnimCmd *const sBattleBingoTextAnimTable[] =
{
    sBattleBingoTextAnim,
};

static const u32 *const sBattleBingoSquareGfx[BINGO_SQUARE_COUNT] =
{
    [BINGO_SQUARE_BOSS] = sBattleBingoSquareBoss_Gfx,
    [BINGO_SQUARE_BUG] = sBattleBingoSquareBug_Gfx,
    [BINGO_SQUARE_COMPLETE] = sBattleBingoSquareComplete_Gfx,
    [BINGO_SQUARE_DARK] = sBattleBingoSquareDark_Gfx,
    [BINGO_SQUARE_DRAGON] = sBattleBingoSquareDragon_Gfx,
    [BINGO_SQUARE_ELECTRIC] = sBattleBingoSquareElectric_Gfx,
    [BINGO_SQUARE_FAIRY] = sBattleBingoSquareFairy_Gfx,
    [BINGO_SQUARE_FIGHTING] = sBattleBingoSquareFighting_Gfx,
    [BINGO_SQUARE_FIRE] = sBattleBingoSquareFire_Gfx,
    [BINGO_SQUARE_FLYING] = sBattleBingoSquareFlying_Gfx,
    [BINGO_SQUARE_GHOST] = sBattleBingoSquareGhost_Gfx,
    [BINGO_SQUARE_GRASS] = sBattleBingoSquareGrass_Gfx,
    [BINGO_SQUARE_GROUND] = sBattleBingoSquareGround_Gfx,
    [BINGO_SQUARE_ICE] = sBattleBingoSquareIce_Gfx,
    [BINGO_SQUARE_ITEM] = sBattleBingoSquareItem_Gfx,
    [BINGO_SQUARE_MYSTERY] = sBattleBingoSquareMystery_Gfx,
    [BINGO_SQUARE_NORMAL] = sBattleBingoSquareNormal_Gfx,
    [BINGO_SQUARE_POISON] = sBattleBingoSquarePoison_Gfx,
    [BINGO_SQUARE_PSYCHIC] = sBattleBingoSquarePsychic_Gfx,
    [BINGO_SQUARE_ROCK] = sBattleBingoSquareRock_Gfx,
    [BINGO_SQUARE_STEEL] = sBattleBingoSquareSteel_Gfx,
    [BINGO_SQUARE_WATER] = sBattleBingoSquareWater_Gfx,
};

static const u32 *const sBattleBingoLineGfx[BINGO_LINE_GFX_COUNT] =
{
    [BINGO_LINE_HORIZONTAL] = sBattleBingoLineHorizontal_Gfx,
    [BINGO_LINE_VERTICAL] = sBattleBingoLineVertical_Gfx,
    [BINGO_LINE_DIAGONAL_DOWN_RIGHT] = sBattleBingoLineDiagonalDownRight_Gfx,
    [BINGO_LINE_DIAGONAL_DOWN_LEFT] = sBattleBingoLineDiagonalDownLeft_Gfx,
};

static const u16 sBattleBingoSquarePaletteTags[BINGO_SQUARE_COUNT] =
{
    [BINGO_SQUARE_BOSS] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_BUG] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_COMPLETE] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_DARK] = TAG_BINGO_SQUARE_PAL_2,
    [BINGO_SQUARE_DRAGON] = TAG_BINGO_SQUARE_PAL_2,
    [BINGO_SQUARE_ELECTRIC] = TAG_BINGO_SQUARE_PAL_2,
    [BINGO_SQUARE_FAIRY] = TAG_BINGO_SQUARE_PAL_2,
    [BINGO_SQUARE_FIGHTING] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_FIRE] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_FLYING] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_GHOST] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_GRASS] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_GROUND] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_ICE] = TAG_BINGO_SQUARE_PAL_2,
    [BINGO_SQUARE_ITEM] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_MYSTERY] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_NORMAL] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_POISON] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_PSYCHIC] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_ROCK] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_STEEL] = TAG_BINGO_SQUARE_PAL_1,
    [BINGO_SQUARE_WATER] = TAG_BINGO_SQUARE_PAL_1,
};

static const u8 sBattleBingoSquareSpriteX[BINGO_BOARD_SIZE] = {96, 128, 160, 192, 224};
static const u8 sBattleBingoSquareSpriteY[BINGO_BOARD_SIZE] = {16, 48, 80, 112, 144};
static const u8 sBattleBingoPartyIconX[BATTLE_BINGO_MAX_PARTY_SIZE] = {17, 17, 17, 17};
static const u8 sBattleBingoPartyIconY[BATTLE_BINGO_MAX_PARTY_SIZE] = {41, 65, 89, 113};
static const u8 sBattleBingoPartyHpBarY[BATTLE_BINGO_MAX_PARTY_SIZE] = {48, 72, 96, 120};
static const u8 sBattleBingoPartyStatusY[BATTLE_BINGO_MAX_PARTY_SIZE] = {45, 69, 93, 117};
static const u8 sBattleBingoPartyCursorY[BATTLE_BINGO_MAX_PARTY_SIZE] = {42, 66, 90, 114};
static const u8 sBattleBingoActionCursorX[] = {35, 75};
static const u8 sBattleBingoBossEdgePositions[16] =
{
    0, 1, 2, 3, 4,
    5, 9,
    10, 14,
    15, 19,
    20, 21, 22, 23, 24,
};

void ShowBattleBingoBoard(void)
{
    if (!sBattleBingo.initialized)
        InitBattleBingoRuntime(BATTLE_BINGO_BOARD_FWG);

    sBattleBingo.playBoardMusic = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    SetMainCallback2(CB2_WaitForBattleBingoBoardFadeOut);
}

static void CB2_WaitForBattleBingoBoardFadeOut(void)
{
    UpdatePaletteFade();

    if (!gPaletteFade.active)
        SetMainCallback2(CB2_InitBattleBingoBoard);
}

static void CB2_InitBattleBingoBoard(void)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);
    u32 i;

    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetVBlankCallback(NULL);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBattleBingoBgTemplates, ARRAY_COUNT(sBattleBingoBgTemplates));
    SetBgTilemapBuffer(0, Alloc(BG_SCREEN_SIZE));
    SetBgTilemapBuffer(1, Alloc(BG_SCREEN_SIZE));
    InitWindows(sBattleBingoWindowTemplates);

    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sBattleBingo.pickingPartner = FALSE;
    sBattleBingo.partnerPickLocked = FALSE;
    sBattleBingo.lineSpriteCount = 0;
    for (i = 0; i < BINGO_TEXT_COUNT; i++)
        sBattleBingo.textSpriteIds[i] = MAX_SPRITES;
    for (i = 0; i < BINGO_MAX_LINE_SPRITES; i++)
        sBattleBingo.lineSpriteIds[i] = MAX_SPRITES;
    DeactivateAllTextPrinters();
    SetDefaultFontsPointer();
    ScanlineEffect_Stop();

    DecompressAndLoadBgGfxUsingHeap(0, sBattleBingoBoard_Gfx, 0, 0, 0);
    CopyToBgTilemapBuffer(0, sBattleBingoBoard_Tilemap, 0, 0);
    LoadPalette(sBattleBingoBoard_Pal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
    if (sBattleBingo.playBoardMusic)
    {
        PlayNewMapMusic(GetBattleBingoBoardMusic(rules));
        sBattleBingo.playBoardMusic = FALSE;
    }

    ChangeBgX(0, 0, BG_COORD_SET);
    ChangeBgY(0, 0, BG_COORD_SET);
    ChangeBgX(1, 0, BG_COORD_SET);
    ChangeBgY(1, 0, BG_COORD_SET);
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(0);
    CopyBgTilemapBufferToVram(1);
    ShowBg(0);
    ShowBg(1);
    LoadMessageBoxGfx(BINGO_WIN_MESSAGE, DLG_WINDOW_BASE_TILE_NUM, BG_PLTT_ID(DLG_WINDOW_PALETTE_NUM));
    LoadBattleBingoSquareSprites();
    LoadBattleBingoHpBarSprites();
    LoadBattleBingoTextSprites();
    LoadBattleBingoCursorSprite();
    LoadBattleBingoLineSprites();
    LoadMonIconPalettes();
    LoadPartyMenuAilmentGfx();
    CreateBattleBingoBoardSprites();
    CreateBattleBingoCompletedLineSprites();
    CreateBattleBingoPartyIcons();
    CreateBattleBingoPartyHpBars();
    CreateBattleBingoPartyStatusIcons();
    CreateBattleBingoTextLabels();
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    SetVBlankCallback(VBlankCB_BattleBingoBoard);
    {
        u8 taskId = CreateTask(Task_BattleBingoBoard, 0);
        gTasks[taskId].tCursorSpriteId = CreateBattleBingoCursorSprite();
        RestoreBattleBingoCursor(taskId);
        if (sBattleBingo.boardCompletePending)
            TryStartBattleBingoBoardComplete(taskId);
    }
    SetMainCallback2(CB2_RunBattleBingoBoard);
}

static void CB2_RunBattleBingoBoard(void)
{
    RunTextPrinters();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_ReturnToBattleBingoBoard(void)
{
    SetMainCallback2(CB2_InitBattleBingoBoard);
}

static void CB2_ReturnToBattleBingoBoardAfterBattle(void)
{
    bool8 cleared = (gBattleOutcome == B_OUTCOME_WON || gBattleOutcome == B_OUTCOME_CAUGHT);
    bool8 captured = (gBattleOutcome == B_OUTCOME_CAUGHT);
    bool8 wasWildBattle = sBattleBingo.partyBackedUp;

    if (sBattleBingo.partyBackedUp)
        RestoreBattleBingoPartnerParty(captured);

    if (cleared)
        ClearBattleBingoSelectedSquare(FALSE);

    if (!cleared && !BattleBingoPlayerHasRevives() && (!wasWildBattle || !BattleBingoPlayerHasUsableMons()))
    {
        FinishBattleBingoRun(GetBattleBingoExitResultByScore());
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
    }
    else
    {
        sBattleBingo.playBoardMusic = !sBattleBingo.boardCompletePending;
        SetMainCallback2(CB2_InitBattleBingoBoard);
    }
}

static void VBlankCB_BattleBingoBoard(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_BattleBingoBoard(u8 taskId)
{
    if (gPaletteFade.active)
        return;

    if (!BattleBingoPlayerHasUsableMons() && !BattleBingoPlayerHasRevives())
    {
        StartBattleBingoRunExit(taskId, GetBattleBingoExitResultByScore());
        return;
    }

    if (sBattleBingo.pickingPartner)
    {
        HandleBattleBingoPartnerPickInput(taskId);
        return;
    }

    if (JOY_REPEAT(DPAD_LEFT))
        MoveBattleBingoCursor(taskId, -1, 0);
    else if (JOY_REPEAT(DPAD_RIGHT))
        MoveBattleBingoCursor(taskId, 1, 0);
    else if (JOY_REPEAT(DPAD_UP))
        MoveBattleBingoCursor(taskId, 0, -1);
    else if (JOY_REPEAT(DPAD_DOWN))
        MoveBattleBingoCursor(taskId, 0, 1);
    else if (JOY_NEW(A_BUTTON))
        HandleBattleBingoCursorSelect(taskId);
}

static void Task_BattleBingoBoardFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FinishBattleBingoRun(gTasks[taskId].tExitResult);
        ExitBattleBingoBoard(taskId);
    }
}

static void Task_BattleBingoBoardFadeToBag(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CloseBattleBingoBoardForSubmenu(taskId);
        GoToBagMenu(ITEMMENULOCATION_FIELD, POCKETS_COUNT, CB2_ReturnToBattleBingoBoard);
    }
}

static void Task_BattleBingoBoardFadeToSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 partyIndex = gTasks[taskId].tSelectedPartyIndex;
        u8 partyCount = CalculatePlayerPartyCount();

        CloseBattleBingoBoardForSubmenu(taskId);
        ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, gParties[B_TRAINER_PLAYER], partyIndex, partyCount - 1, CB2_ReturnToBattleBingoBoard);
    }
}

static void Task_BattleBingoBoardWaitForBattleTransition(u8 taskId)
{
    if (IsBattleTransitionDone() == TRUE)
    {
        CloseBattleBingoBoardForSubmenu(taskId);
        SetMainCallback2(CB2_InitBattle);
    }
}

static void Task_BattleBingoBoardWaitItemFanfare(u8 taskId)
{
    if (IsFanfareTaskInactive())
        gTasks[taskId].func = Task_BattleBingoBoardMessage;
}

static void Task_BattleBingoBoardMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        HideBattleBingoPrompt();
        PlaySE(SE_SELECT);
        TryStartBattleBingoBoardComplete(taskId);
        if (sBattleBingo.boardCompletePending)
            return;
        gTasks[taskId].func = Task_BattleBingoBoard;
    }
}

static void Task_BattleBingoBoardCompleteMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        HideBattleBingoPrompt();
        PlaySE(SE_SELECT);
        gTasks[taskId].tExitResult = BATTLE_BINGO_RESULT_COMPLETE;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BattleBingoBoardCompleteFadeOut;
    }
}

static void Task_BattleBingoBoardCompleteFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FinishBattleBingoRun(gTasks[taskId].tExitResult);
        ExitBattleBingoBoard(taskId);
    }
}

static void ExitBattleBingoBoard(u8 taskId)
{
    SetVBlankCallback(NULL);
    FreeAllWindowBuffers();
    Free(GetBgTilemapBuffer(0));
    Free(GetBgTilemapBuffer(1));
    FreeMonIconPalettes();
    DestroyTask(taskId);
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

static void StartBattleBingoRunExit(u8 taskId, enum BattleBingoResult result)
{
    gTasks[taskId].tExitResult = result;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_BattleBingoBoardFadeOut;
}

static void FinishBattleBingoRun(enum BattleBingoResult result)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);

    sBattleBingoLastResult = result;
    if ((result == BATTLE_BINGO_RESULT_PRIZE || result == BATTLE_BINGO_RESULT_COMPLETE) && rules != NULL)
        sBattleBingoLastPrizeMoney = BattleBingoGetPrizeMoney(sBattleBingo.bingoCount, rules->prizeMultiplierQ8_8);
    else
        sBattleBingoLastPrizeMoney = 0;

    RestoreBattleBingoBag();
    WipeBattleBingoParty();
    if (sBattleBingoLastPrizeMoney != 0)
        AddMoney(&gSaveBlock1Ptr->money, sBattleBingoLastPrizeMoney);

    sBattleBingo.initialized = FALSE;
    sBattleBingo.boardCompletePending = FALSE;
    sBattleBingo.partyBackedUp = FALSE;
}

static enum BattleBingoResult GetBattleBingoExitResultByScore(void)
{
    return sBattleBingo.bingoCount == 0 ? BATTLE_BINGO_RESULT_LOSS : BATTLE_BINGO_RESULT_PRIZE;
}

static void BackupBattleBingoBag(void)
{
    sBattleBingo.bagBackup = gSaveBlock1Ptr->bag;
    sBattleBingo.bagBackedUp = TRUE;
}

static void GiveBattleBingoStartingItems(const struct BattleBingoBoardRules *rules)
{
    if (rules == NULL || rules->startWithoutBall)
        return;

    AddBagItem(ITEM_MASTER_BALL, 1);
}

static void RestoreBattleBingoBag(void)
{
    if (sBattleBingo.bagBackedUp)
        gSaveBlock1Ptr->bag = sBattleBingo.bagBackup;
}

static void WipeBattleBingoParty(void)
{
    u32 i;

    for (i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
    CalculatePlayerPartyCount();
}

static void CloseBattleBingoBoardForSubmenu(u8 taskId)
{
    SetVBlankCallback(NULL);
    FreeAllWindowBuffers();
    Free(GetBgTilemapBuffer(0));
    Free(GetBgTilemapBuffer(1));
    FreeMonIconPalettes();
    DestroyTask(taskId);
}

static void LoadBattleBingoHpBarSprites(void)
{
    const struct SpriteSheet sheet =
    {
        .data = sBattleBingoHpBar_Gfx,
        .size = BINGO_HP_BAR_SPRITE_SIZE,
        .tag = TAG_BINGO_HP_BAR_GFX,
    };
    const struct SpritePalette palettes[] =
    {
        {sBattleBingoHpBar_Pal, TAG_BINGO_HP_BAR_PAL_GREEN},
        {sBattleBingoHpBarYellowPal, TAG_BINGO_HP_BAR_PAL_YELLOW},
        {sBattleBingoHpBarRedPal, TAG_BINGO_HP_BAR_PAL_RED},
        {},
    };

    InitBattleBingoHpBarPalette(sBattleBingoHpBarYellowPal, 3);
    InitBattleBingoHpBarPalette(sBattleBingoHpBarRedPal, 5);
    LoadSpritePalettes(palettes);
    LoadSpriteSheet(&sheet);
}

static void InitBattleBingoHpBarPalette(u16 *dest, u8 colorBase)
{
    u32 i;

    for (i = 0; i < BINGO_HP_BAR_PALETTE_SIZE; i++)
        dest[i] = sBattleBingoHpBar_Pal[i];

    dest[1] = sBattleBingoHpBar_Pal[colorBase];
    dest[2] = sBattleBingoHpBar_Pal[colorBase + 1];
}

static void LoadBattleBingoTextSprites(void)
{
    u32 i;
    const struct SpritePalette palette = {sBattleBingoText_Pal, TAG_BINGO_TEXT_PAL};

    LoadSpritePalette(&palette);

    for (i = 0; i < BINGO_TEXT_COUNT; i++)
    {
        const struct SpriteSheet sheet =
        {
            .data = (i == BINGO_TEXT_CARD_TITLE) ? sBattleBingoTitleText_Gfx : sBattleBingoText_Gfx,
            .size = (i == BINGO_TEXT_CARD_TITLE) ? BINGO_TITLE_TEXT_SPRITE_SIZE : BINGO_TEXT_SPRITE_SIZE,
            .tag = TAG_BINGO_TEXT_GFX_START + i,
        };

        LoadSpriteSheet(&sheet);
    }
}

static void LoadBattleBingoCursorSprite(void)
{
    const struct SpriteSheet sheet =
    {
        .data = sBattleBingoCursor_Gfx,
        .size = BINGO_CURSOR_SPRITE_SIZE,
        .tag = TAG_BINGO_CURSOR_GFX,
    };
    const struct SpritePalette palette =
    {
        .data = sBattleBingoCursor_Pal,
        .tag = TAG_BINGO_CURSOR_PAL,
    };

    LoadSpritePalette(&palette);
    LoadSpriteSheet(&sheet);
}

static void LoadBattleBingoLineSprites(void)
{
    u32 i;
    const struct SpritePalette palette =
    {
        .data = sBattleBingoLine_Pal,
        .tag = TAG_BINGO_LINE_PAL,
    };

    LoadSpritePalette(&palette);
    for (i = 0; i < BINGO_LINE_GFX_COUNT; i++)
    {
        const struct SpriteSheet sheet =
        {
            .data = sBattleBingoLineGfx[i],
            .size = BINGO_LINE_SPRITE_SIZE,
            .tag = TAG_BINGO_LINE_GFX_START + i,
        };

        LoadSpriteSheet(&sheet);
    }
}

static void CreateBattleBingoTextLabels(void)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(BATTLE_BINGO_BOARD_FWG);

    sBattleBingo.textSpriteIds[BINGO_TEXT_CARD_TITLE] = CreateBattleBingoTitleTextLabel(BINGO_TEXT_CARD_TITLE, 40, 16, rules->title);
    sBattleBingo.textSpriteIds[BINGO_TEXT_BAG] = CreateBattleBingoTextLabel(BINGO_TEXT_BAG, 20, 140, sBattleBingoText_Bag);
    sBattleBingo.textSpriteIds[BINGO_TEXT_QUIT] = CreateBattleBingoTextLabel(BINGO_TEXT_QUIT, 56, 140, sBattleBingoText_Quit);
    sBattleBingo.textSpriteIds[BINGO_TEXT_POINTS] = CreateBattleBingoTextLabel(BINGO_TEXT_POINTS, 28, 28, sBattleBingoText_Points);
    UpdateBattleBingoPointText();
}

static u8 CreateBattleBingoTextLabel(u8 labelId, s16 x, s16 y, const u8 *text)
{
    u8 spriteId;
    s32 textX = GetStringCenterAlignXOffset(FONT_SMALL_NARROWER, text, BINGO_TEXT_SPRITE_WIDTH);
    static const union TextColor textColor =
    {
        .background = TEXT_COLOR_TRANSPARENT,
        .foreground = TEXT_COLOR_DARK_GRAY,
        .shadow = TEXT_COLOR_TRANSPARENT,
        .accent = TEXT_COLOR_TRANSPARENT,
    };
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_TEXT_GFX_START + labelId,
        .paletteTag = TAG_BINGO_TEXT_PAL,
        .oam = &sBattleBingoTextOam,
        .anims = sBattleBingoTextAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    spriteId = CreateSprite(&template, x, y, 8);
    if (spriteId != MAX_SPRITES)
    {
        FillSpriteRectColor(spriteId, 0, 0, BINGO_TEXT_SPRITE_WIDTH, BINGO_TEXT_SPRITE_HEIGHT, TEXT_COLOR_TRANSPARENT);
        AddSpriteTextPrinterParameterized6(spriteId, FONT_SMALL_NARROWER, textX, BINGO_TEXT_Y_OFFSET, 0, 0, textColor, 0, text);
    }

    return spriteId;
}

static u8 CreateBattleBingoTitleTextLabel(u8 labelId, s16 x, s16 y, const u8 *text)
{
    u8 spriteId;
    s32 textX = GetStringCenterAlignXOffset(FONT_SMALL_NARROWER, text, BINGO_TITLE_TEXT_SPRITE_WIDTH);
    static const union TextColor textColor =
    {
        .background = TEXT_COLOR_TRANSPARENT,
        .foreground = TEXT_COLOR_DARK_GRAY,
        .shadow = TEXT_COLOR_TRANSPARENT,
        .accent = TEXT_COLOR_TRANSPARENT,
    };
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_TEXT_GFX_START + labelId,
        .paletteTag = TAG_BINGO_TEXT_PAL,
        .oam = &sBattleBingoTitleTextOam,
        .anims = sBattleBingoTextAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    spriteId = CreateSprite(&template, x, y, 8);
    if (spriteId != MAX_SPRITES)
    {
        FillSpriteRectColor(spriteId, 0, 0, BINGO_TITLE_TEXT_SPRITE_WIDTH, BINGO_TITLE_TEXT_SPRITE_HEIGHT, TEXT_COLOR_TRANSPARENT);
        AddSpriteTextPrinterParameterized6(spriteId, FONT_SMALL_NARROWER, textX, BINGO_TITLE_TEXT_Y_OFFSET, 0, 0, textColor, 0, text);
    }

    return spriteId;
}

void ShowBattleBingoStarterSelect(void)
{
    sBattleBingo.initialized = FALSE;
    DoBattleBingoStarterSelectScreen();
}

static void CreateBattleBingoPartyIcons(void)
{
    u32 i;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        u32 personality;
        u8 spriteId;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        spriteId = CreateMonIcon(species, SpriteCB_MonIcon, sBattleBingoPartyIconX[i], sBattleBingoPartyIconY[i], 8, personality);
        if (spriteId != MAX_SPRITES)
            gSprites[spriteId].oam.priority = 1;
    }
}

static void CreateBattleBingoPartyHpBars(void)
{
    u32 i;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        u32 hp;
        u32 maxHp;
        u8 hpPercent;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        hp = GetMonData(mon, MON_DATA_HP);
        maxHp = GetMonData(mon, MON_DATA_MAX_HP);
        if (maxHp == 0 || hp == 0)
            continue;

        hpPercent = (hp * 100 + maxHp - 1) / maxHp;
        CreateBattleBingoHpBar(32, sBattleBingoPartyHpBarY[i], hpPercent);
    }
}

static void CreateBattleBingoPartyStatusIcons(void)
{
    u32 i;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        u8 ailment;
        u8 spriteId;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        ailment = GetMonAilment(mon);
        if (ailment == AILMENT_NONE || ailment == AILMENT_PKRS)
            continue;

        spriteId = CreateSprite(&gSpriteTemplate_StatusIcons, 47, sBattleBingoPartyStatusY[i], 8);
        if (spriteId != MAX_SPRITES)
        {
            StartSpriteAnim(&gSprites[spriteId], ailment - 1);
            gSprites[spriteId].oam.priority = 1;
        }
    }
}

static u8 CreateBattleBingoCursorSprite(void)
{
    u8 spriteId;
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_CURSOR_GFX,
        .paletteTag = TAG_BINGO_CURSOR_PAL,
        .oam = &sBattleBingoCursorOam,
        .anims = sBattleBingoCursorAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    spriteId = CreateSprite(&template, sBattleBingoSquareSpriteX[0] - 8, sBattleBingoSquareSpriteY[0] + 8, 0);
    return spriteId;
}

static void MoveBattleBingoCursor(u8 taskId, s8 deltaX, s8 deltaY)
{
    u8 area = gTasks[taskId].tCursorArea;
    s16 x = gTasks[taskId].tCursorX;
    s16 y = gTasks[taskId].tCursorY;
    u8 oldArea = area;
    s16 oldX = x;
    s16 oldY = y;

    if (area == BINGO_CURSOR_AREA_BOARD)
    {
        if (deltaX < 0 && x == 0)
        {
            area = BINGO_CURSOR_AREA_PARTY;
            x = 0;
            if (y >= BATTLE_BINGO_MAX_PARTY_SIZE)
                y = BATTLE_BINGO_MAX_PARTY_SIZE - 1;
        }
        else
        {
            x += deltaX;
            y += deltaY;

            if (x < 0)
                x = BINGO_BOARD_SIZE - 1;
            else if (x >= BINGO_BOARD_SIZE)
                x = 0;

            if (y < 0)
                y = BINGO_BOARD_SIZE - 1;
            else if (y >= BINGO_BOARD_SIZE)
                y = 0;
        }
    }
    else if (area == BINGO_CURSOR_AREA_PARTY)
    {
        if (deltaX > 0)
        {
            area = BINGO_CURSOR_AREA_BOARD;
            x = 0;
        }
        else if (deltaY > 0)
        {
            if (y < BATTLE_BINGO_MAX_PARTY_SIZE - 1)
                y++;
            else
                area = BINGO_CURSOR_AREA_BAG;
        }
        else if (deltaY < 0)
        {
            if (y > 0)
                y--;
            else
                y = BATTLE_BINGO_MAX_PARTY_SIZE - 1;
        }
    }
    else if (area == BINGO_CURSOR_AREA_BAG)
    {
        if (deltaX > 0)
            area = BINGO_CURSOR_AREA_QUIT;
        else if (deltaY < 0)
        {
            area = BINGO_CURSOR_AREA_PARTY;
            y = BATTLE_BINGO_MAX_PARTY_SIZE - 1;
        }
        else if (deltaY > 0)
        {
            area = BINGO_CURSOR_AREA_BOARD;
            x = 0;
            y = BINGO_BOARD_SIZE - 1;
        }
    }
    else if (area == BINGO_CURSOR_AREA_QUIT)
    {
        if (deltaX < 0)
            area = BINGO_CURSOR_AREA_BAG;
        else if (deltaX > 0 || deltaY > 0)
        {
            area = BINGO_CURSOR_AREA_BOARD;
            x = 0;
            y = BINGO_BOARD_SIZE - 1;
        }
        else if (deltaY < 0)
        {
            area = BINGO_CURSOR_AREA_PARTY;
            y = BATTLE_BINGO_MAX_PARTY_SIZE - 1;
        }
    }

    if (area == oldArea && x == oldX && y == oldY)
        return;

    PlaySE(SE_SELECT);
    gTasks[taskId].tCursorArea = area;
    gTasks[taskId].tCursorX = x;
    gTasks[taskId].tCursorY = y;
    if (!sBattleBingo.pickingPartner)
        RememberBattleBingoCursor(area, x, y);
    UpdateBattleBingoCursorSpritePosition(taskId);
}

static void RememberBattleBingoCursor(u8 area, u8 x, u8 y)
{
    sBattleBingo.cursorArea = area;
    sBattleBingo.cursorX = x;
    sBattleBingo.cursorY = y;
}

static void RestoreBattleBingoCursor(u8 taskId)
{
    u8 area = sBattleBingo.cursorArea;
    u8 x = sBattleBingo.cursorX;
    u8 y = sBattleBingo.cursorY;

    switch (area)
    {
    case BINGO_CURSOR_AREA_PARTY:
        x = 0;
        if (y >= BATTLE_BINGO_MAX_PARTY_SIZE)
            y = BATTLE_BINGO_MAX_PARTY_SIZE - 1;
        break;
    case BINGO_CURSOR_AREA_BAG:
    case BINGO_CURSOR_AREA_QUIT:
        x = 0;
        y = 0;
        break;
    case BINGO_CURSOR_AREA_BOARD:
    default:
        area = BINGO_CURSOR_AREA_BOARD;
        if (x >= BINGO_BOARD_SIZE)
            x = 0;
        if (y >= BINGO_BOARD_SIZE)
            y = 0;
        break;
    }

    gTasks[taskId].tCursorArea = area;
    gTasks[taskId].tCursorX = x;
    gTasks[taskId].tCursorY = y;
    RememberBattleBingoCursor(area, x, y);
    UpdateBattleBingoCursorSpritePosition(taskId);
}

static void UpdateBattleBingoCursorSpritePosition(u8 taskId)
{
    u8 spriteId = gTasks[taskId].tCursorSpriteId;

    if (spriteId != MAX_SPRITES)
    {
        switch (gTasks[taskId].tCursorArea)
        {
        case BINGO_CURSOR_AREA_PARTY:
            gSprites[spriteId].x = 24;
            gSprites[spriteId].y = sBattleBingoPartyCursorY[gTasks[taskId].tCursorY];
            break;
        case BINGO_CURSOR_AREA_BAG:
            gSprites[spriteId].x = sBattleBingoActionCursorX[0];
            gSprites[spriteId].y = 135;
            break;
        case BINGO_CURSOR_AREA_QUIT:
            gSprites[spriteId].x = sBattleBingoActionCursorX[1];
            gSprites[spriteId].y = 135;
            break;
        case BINGO_CURSOR_AREA_BOARD:
        default:
            gSprites[spriteId].x = sBattleBingoSquareSpriteX[gTasks[taskId].tCursorX] + 8;
            gSprites[spriteId].y = sBattleBingoSquareSpriteY[gTasks[taskId].tCursorY] - 8;
            break;
        }
    }
}

static void HandleBattleBingoCursorSelect(u8 taskId)
{
    u8 partyIndex;
    u8 partyCount;
    enum Species species;
    struct BattleBingoSquareState *square;

    switch (gTasks[taskId].tCursorArea)
    {
    case BINGO_CURSOR_AREA_PARTY:
        partyIndex = gTasks[taskId].tCursorY;
        partyCount = CalculatePlayerPartyCount();
        species = GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_SPECIES_OR_EGG);
        if (partyIndex < partyCount && species != SPECIES_NONE && species != SPECIES_EGG)
        {
            PlaySE(SE_SELECT);
            gTasks[taskId].tSelectedPartyIndex = partyIndex;
            RememberBattleBingoCursor(BINGO_CURSOR_AREA_PARTY, 0, partyIndex);
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            gTasks[taskId].func = Task_BattleBingoBoardFadeToSummary;
        }
        break;
    case BINGO_CURSOR_AREA_BAG:
        PlaySE(SE_SELECT);
        RememberBattleBingoCursor(BINGO_CURSOR_AREA_BAG, 0, 0);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BattleBingoBoardFadeToBag;
        break;
    case BINGO_CURSOR_AREA_QUIT:
        PlaySE(SE_SELECT);
        StartBattleBingoRunExit(taskId, GetBattleBingoExitResultByScore());
        break;
    case BINGO_CURSOR_AREA_BOARD:
    default:
        square = &sBattleBingo.squares[gTasks[taskId].tCursorY][gTasks[taskId].tCursorX];
        if (square->cleared)
            break;
        if (!BattleBingoSquareIsReachable(GetBattleBingoClearedSquareMask(), gTasks[taskId].tCursorY, gTasks[taskId].tCursorX))
        {
            PlaySE(SE_FAILURE);
            break;
        }

        PlaySE(SE_SELECT);
        sBattleBingo.selectedRow = gTasks[taskId].tCursorY;
        sBattleBingo.selectedCol = gTasks[taskId].tCursorX;
        gTasks[taskId].tSelectedSquareRow = sBattleBingo.selectedRow;
        gTasks[taskId].tSelectedSquareCol = sBattleBingo.selectedCol;
        RememberBattleBingoCursor(BINGO_CURSOR_AREA_BOARD, sBattleBingo.selectedCol, sBattleBingo.selectedRow);

        switch (square->kind)
        {
        case BINGO_RUNTIME_ITEM:
            GiveBattleBingoSelectedItem(taskId);
            break;
        case BINGO_RUNTIME_BOSS:
            StartBattleBingoBossBattle(taskId);
            break;
        case BINGO_RUNTIME_WILD:
        default:
            StartBattleBingoPartnerPick(taskId, square->isMystery);
            break;
        }
        break;
    }
}

static void HandleBattleBingoPartnerPickInput(u8 taskId)
{
    u8 partyIndex;
    u8 partyCount;
    enum Species species;
    u32 hp;

    if (JOY_REPEAT(DPAD_UP))
        MoveBattleBingoCursor(taskId, 0, -1);
    else if (JOY_REPEAT(DPAD_DOWN))
        MoveBattleBingoCursor(taskId, 0, 1);
    else if (JOY_NEW(B_BUTTON) && !sBattleBingo.partnerPickLocked)
        CancelBattleBingoPartnerPick(taskId);
    else if (JOY_NEW(A_BUTTON))
    {
        partyIndex = gTasks[taskId].tCursorY;
        partyCount = CalculatePlayerPartyCount();
        species = GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_SPECIES_OR_EGG);
        hp = GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_HP);
        if (partyIndex < partyCount && species != SPECIES_NONE && species != SPECIES_EGG && hp != 0)
        {
            PlaySE(SE_SELECT);
            sBattleBingo.selectedPartyIndex = partyIndex;
            gTasks[taskId].tSelectedPartyIndex = partyIndex;
            RememberBattleBingoCursor(BINGO_CURSOR_AREA_BOARD, sBattleBingo.selectedCol, sBattleBingo.selectedRow);
            StartBattleBingoWildBattle(taskId);
        }
    }
}

static void CreateBattleBingoHpBar(u8 x, u8 y, u8 hpPercent)
{
    u32 i;
    u8 filledPixels = (hpPercent * BINGO_HP_BAR_WIDTH) / 100;
    u16 paletteTag = TAG_BINGO_HP_BAR_PAL_GREEN;
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_HP_BAR_GFX,
        .oam = &sBattleBingoHpBarOam,
        .anims = sBattleBingoHpBarAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    if (hpPercent <= 25)
        paletteTag = TAG_BINGO_HP_BAR_PAL_RED;
    else if (hpPercent <= 50)
        paletteTag = TAG_BINGO_HP_BAR_PAL_YELLOW;

    for (i = 0; i < BINGO_HP_BAR_SEGMENTS; i++)
    {
        u8 spriteId;
        u8 segmentStart = i * BINGO_HP_BAR_SEGMENT_WIDTH;
        u8 segmentFill;
        struct SpriteTemplate segmentTemplate = template;

        if (filledPixels <= segmentStart)
            continue;

        segmentFill = filledPixels - segmentStart;
        if (segmentFill > BINGO_HP_BAR_SEGMENT_WIDTH)
            segmentFill = BINGO_HP_BAR_SEGMENT_WIDTH;
        segmentTemplate.paletteTag = paletteTag;

        spriteId = CreateSprite(&segmentTemplate, x + segmentStart + 4, y + 4, 8);
        if (spriteId != MAX_SPRITES)
            StartSpriteAnim(&gSprites[spriteId], BINGO_HP_BAR_SEGMENT_WIDTH - segmentFill);
    }
}

static void LoadBattleBingoSquareSprites(void)
{
    u32 i;
    const struct SpritePalette palettes[] =
    {
        {sBattleBingoSquarePal1, TAG_BINGO_SQUARE_PAL_1},
        {sBattleBingoSquarePal2, TAG_BINGO_SQUARE_PAL_2},
        {},
    };

    LoadSpritePalettes(palettes);

    for (i = 0; i < BINGO_SQUARE_COUNT; i++)
    {
        const struct SpriteSheet sheet =
        {
            .data = sBattleBingoSquareGfx[i],
            .size = BINGO_SQUARE_SPRITE_SIZE,
            .tag = TAG_BINGO_SQUARE_GFX_START + i,
        };

        LoadSpriteSheet(&sheet);
    }
}

static void InitBattleBingoRuntime(enum BattleBingoBoardId boardId)
{
    u32 i;
    u32 pos;
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(boardId);
    u8 totalItemCount = CountBattleBingoItemRules(rules);
    struct BattleBingoLayoutSlot
    {
        u8 visibleSquare;
        u8 kind;
        bool8 mystery;
        u8 wildRuleIndex;
    } slots[BATTLE_BINGO_NUM_SQUARES];
    u16 wildRuleSkips[BATTLE_BINGO_NUM_SQUARES] = {0};
    u8 edgePositions[ARRAY_COUNT(sBattleBingoBossEdgePositions)];
    u8 freePositions[BATTLE_BINGO_NUM_SQUARES];
    bool8 bossPositions[BATTLE_BINGO_NUM_SQUARES] = {0};
    u8 freeCount = 0;
    u8 slotCount = 0;
    u8 itemIndex = 0;
    u8 mysteryWildCount = 0;
    u8 mysteryItemCount = 0;

    memset(&sBattleBingo, 0, sizeof(sBattleBingo));
    sBattleBingo.initialized = TRUE;
    sBattleBingo.boardId = boardId;
    RememberBattleBingoCursor(BINGO_CURSOR_AREA_BOARD, 0, 0);
    BackupBattleBingoBag();
    GiveBattleBingoStartingItems(rules);
    for (i = 0; i < BINGO_TEXT_COUNT; i++)
        sBattleBingo.textSpriteIds[i] = MAX_SPRITES;
    for (i = 0; i < BINGO_MAX_LINE_SPRITES; i++)
        sBattleBingo.lineSpriteIds[i] = MAX_SPRITES;

    for (i = 0; i < ARRAY_COUNT(edgePositions); i++)
        edgePositions[i] = sBattleBingoBossEdgePositions[i];
    Shuffle(edgePositions, ARRAY_COUNT(edgePositions), sizeof(edgePositions[0]));

    pos = 12;
    bossPositions[pos] = TRUE;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].visibleSquare = BINGO_SQUARE_BOSS;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].kind = BINGO_RUNTIME_BOSS;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].trainer = rules->bosses[0];

    for (i = 1; i < rules->bossCount; i++)
    {
        pos = edgePositions[i - 1];
        bossPositions[pos] = TRUE;
        sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].visibleSquare = BINGO_SQUARE_BOSS;
        sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].kind = BINGO_RUNTIME_BOSS;
        sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].trainer = rules->bosses[i];
    }

    for (i = 0; i < BATTLE_BINGO_NUM_SQUARES; i++)
    {
        if (!bossPositions[i])
            freePositions[freeCount++] = i;
    }
    Shuffle(freePositions, freeCount, sizeof(freePositions[0]));

    for (i = 0; i < totalItemCount; i++)
    {
        slots[slotCount].visibleSquare = BINGO_SQUARE_ITEM;
        slots[slotCount].kind = BINGO_RUNTIME_ITEM;
        slots[slotCount].mystery = FALSE;
        slotCount++;
    }
    for (i = 0; i < rules->wildRuleCount; i++)
    {
        u8 j;

        for (j = 0; j < rules->wilds[i].count; j++)
        {
            slots[slotCount].visibleSquare = GetBattleBingoTypeSquare(rules->wilds[i].type);
            slots[slotCount].kind = BINGO_RUNTIME_WILD;
            slots[slotCount].mystery = FALSE;
            slots[slotCount].wildRuleIndex = i;
            slotCount++;
        }
    }
    Shuffle(slots, slotCount, sizeof(slots[0]));

    for (i = 0; i < slotCount; i++)
    {
        if (slots[i].kind == BINGO_RUNTIME_WILD && mysteryWildCount < rules->mysteryWildCount)
        {
            slots[i].mystery = TRUE;
            mysteryWildCount++;
        }
        else if (slots[i].kind == BINGO_RUNTIME_ITEM && mysteryItemCount < rules->mysteryItemCount)
        {
            slots[i].mystery = TRUE;
            mysteryItemCount++;
        }
    }

    for (i = 0; i < freeCount; i++)
    {
        struct BattleBingoSquareState *square;

        pos = freePositions[i];
        square = &sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE];
        square->visibleSquare = slots[i].mystery ? BINGO_SQUARE_MYSTERY : slots[i].visibleSquare;
        square->kind = slots[i].kind;
        square->isMystery = slots[i].mystery;
        if (square->kind == BINGO_RUNTIME_ITEM)
            square->item = GetBattleBingoItemByIndex(rules, itemIndex++);
        else
        {
            const struct BattleBingoWildRule *wildRule = &rules->wilds[slots[i].wildRuleIndex];

            square->bingoMonId = FindBattleBingoMonByTags(wildRule->requiredTags, wildRule->forbiddenTags, wildRuleSkips[slots[i].wildRuleIndex]++);
        }
    }

    for (i = 0; i < BATTLE_BINGO_NUM_SQUARES; i++)
        sBattleBingo.squares[i / BINGO_BOARD_SIZE][i % BINGO_BOARD_SIZE].spriteId = MAX_SPRITES;
}

static u16 FindBattleBingoMonByTags(u64 requiredTags, u64 forbiddenTags, u16 skip)
{
    u16 i;
    u16 matches = 0;
    u16 fallback = 0;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if ((gBingoMons[i].tags & requiredTags) == requiredTags && (gBingoMons[i].tags & forbiddenTags) == 0)
        {
            if (matches == 0)
                fallback = i;
            if (matches == skip)
                return i;
            matches++;
        }
    }

    return fallback;
}

static u16 GetBattleBingoItemByIndex(const struct BattleBingoBoardRules *rules, u8 itemIndex)
{
    u32 i;

    for (i = 0; i < rules->itemRuleCount; i++)
    {
        if (itemIndex < rules->items[i].count)
            return rules->items[i].item;
        itemIndex -= rules->items[i].count;
    }

    return ITEM_POTION;
}

static u16 GetBattleBingoBoardMusic(const struct BattleBingoBoardRules *rules)
{
    if (rules != NULL && rules->music != MUS_NONE && rules->music != MUS_DUMMY)
        return rules->music;

    return MUS_BINGO_BOARD;
}

static u8 CountBattleBingoItemRules(const struct BattleBingoBoardRules *rules)
{
    u32 i;
    u8 count = 0;

    for (i = 0; i < rules->itemRuleCount; i++)
        count += rules->items[i].count;

    return count;
}

static u8 GetBattleBingoTypeSquare(u8 type)
{
    switch (type)
    {
    case TYPE_BUG:
        return BINGO_SQUARE_BUG;
    case TYPE_DARK:
        return BINGO_SQUARE_DARK;
    case TYPE_DRAGON:
        return BINGO_SQUARE_DRAGON;
    case TYPE_ELECTRIC:
        return BINGO_SQUARE_ELECTRIC;
    case TYPE_FAIRY:
        return BINGO_SQUARE_FAIRY;
    case TYPE_FIGHTING:
        return BINGO_SQUARE_FIGHTING;
    case TYPE_FIRE:
        return BINGO_SQUARE_FIRE;
    case TYPE_FLYING:
        return BINGO_SQUARE_FLYING;
    case TYPE_GHOST:
        return BINGO_SQUARE_GHOST;
    case TYPE_GRASS:
        return BINGO_SQUARE_GRASS;
    case TYPE_GROUND:
        return BINGO_SQUARE_GROUND;
    case TYPE_ICE:
        return BINGO_SQUARE_ICE;
    case TYPE_NORMAL:
        return BINGO_SQUARE_NORMAL;
    case TYPE_POISON:
        return BINGO_SQUARE_POISON;
    case TYPE_PSYCHIC:
        return BINGO_SQUARE_PSYCHIC;
    case TYPE_ROCK:
        return BINGO_SQUARE_ROCK;
    case TYPE_STEEL:
        return BINGO_SQUARE_STEEL;
    case TYPE_WATER:
        return BINGO_SQUARE_WATER;
    default:
        return BINGO_SQUARE_MYSTERY;
    }
}

static void UpdateBattleBingoLines(bool8 drawNewLines)
{
    u32 i;
    u8 count = 0;
    u16 newFlags = sBattleBingo.completedLineFlags;

    for (i = 0; i < BATTLE_BINGO_MAX_LINES; i++)
    {
        u16 flag = 1 << i;
        if (IsBattleBingoLineCleared(i))
        {
            if (!(newFlags & flag) && drawNewLines)
                CreateBattleBingoLineSprites(i);
            newFlags |= flag;
        }
    }

    for (i = 0; i < BATTLE_BINGO_MAX_LINES; i++)
    {
        if (newFlags & (1 << i))
            count++;
    }

    sBattleBingo.completedLineFlags = newFlags;
    sBattleBingo.bingoCount = count;
    if (drawNewLines)
        UpdateBattleBingoPointText();
}

static bool8 IsBattleBingoLineCleared(u8 line)
{
    u32 i;

    if (line < BINGO_BOARD_SIZE)
    {
        for (i = 0; i < BINGO_BOARD_SIZE; i++)
        {
            if (!sBattleBingo.squares[line][i].cleared)
                return FALSE;
        }
        return TRUE;
    }

    line -= BINGO_BOARD_SIZE;
    if (line < BINGO_BOARD_SIZE)
    {
        for (i = 0; i < BINGO_BOARD_SIZE; i++)
        {
            if (!sBattleBingo.squares[i][line].cleared)
                return FALSE;
        }
        return TRUE;
    }

    line -= BINGO_BOARD_SIZE;
    if (line == 0)
    {
        for (i = 0; i < BINGO_BOARD_SIZE; i++)
        {
            if (!sBattleBingo.squares[i][i].cleared)
                return FALSE;
        }
        return TRUE;
    }

    if (line == 1)
    {
        for (i = 0; i < BINGO_BOARD_SIZE; i++)
        {
            if (!sBattleBingo.squares[i][BINGO_BOARD_SIZE - 1 - i].cleared)
                return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

static bool8 IsBattleBingoBoardComplete(void)
{
    u32 row;
    u32 col;

    for (row = 0; row < BINGO_BOARD_SIZE; row++)
    {
        for (col = 0; col < BINGO_BOARD_SIZE; col++)
        {
            if (!sBattleBingo.squares[row][col].cleared)
                return FALSE;
        }
    }

    return TRUE;
}

static u32 GetBattleBingoClearedSquareMask(void)
{
    u32 row;
    u32 col;
    u32 mask = 0;

    for (row = 0; row < BINGO_BOARD_SIZE; row++)
    {
        for (col = 0; col < BINGO_BOARD_SIZE; col++)
        {
            if (sBattleBingo.squares[row][col].cleared)
                mask |= 1 << (row * BINGO_BOARD_SIZE + col);
        }
    }

    return mask;
}

static void UpdateBattleBingoPointText(void)
{
    ConvertIntToDecimalStringN(gStringVar1, sBattleBingo.bingoCount, STR_CONV_MODE_LEFT_ALIGN, 2);

    if (sBattleBingo.textSpriteIds[BINGO_TEXT_POINT_VALUE] != MAX_SPRITES)
        DestroySprite(&gSprites[sBattleBingo.textSpriteIds[BINGO_TEXT_POINT_VALUE]]);
    sBattleBingo.textSpriteIds[BINGO_TEXT_POINT_VALUE] = CreateBattleBingoTextLabel(BINGO_TEXT_POINT_VALUE, 64, 28, gStringVar1);
}

static void StartBattleBingoPartnerPick(u8 taskId, bool8 locked)
{
    sBattleBingo.pickingPartner = TRUE;
    sBattleBingo.partnerPickLocked = locked;
    ShowBattleBingoPrompt(sBattleBingoText_PickPartner);
    gTasks[taskId].tCursorArea = BINGO_CURSOR_AREA_PARTY;
    gTasks[taskId].tCursorX = 0;
    gTasks[taskId].tCursorY = 0;
    UpdateBattleBingoCursorSpritePosition(taskId);
}

static void CancelBattleBingoPartnerPick(u8 taskId)
{
    PlaySE(SE_SELECT);
    HideBattleBingoPrompt();
    sBattleBingo.pickingPartner = FALSE;
    sBattleBingo.partnerPickLocked = FALSE;
    RememberBattleBingoCursor(BINGO_CURSOR_AREA_BOARD, sBattleBingo.selectedCol, sBattleBingo.selectedRow);
    gTasks[taskId].tCursorArea = BINGO_CURSOR_AREA_BOARD;
    gTasks[taskId].tCursorX = sBattleBingo.selectedCol;
    gTasks[taskId].tCursorY = sBattleBingo.selectedRow;
    UpdateBattleBingoCursorSpritePosition(taskId);
}

static void ClearBattleBingoSelectedSquare(bool8 drawNewLines)
{
    struct BattleBingoSquareState *square = &sBattleBingo.squares[sBattleBingo.selectedRow][sBattleBingo.selectedCol];

    square->cleared = TRUE;
    if (square->spriteId != MAX_SPRITES)
    {
        DestroySprite(&gSprites[square->spriteId]);
        square->spriteId = CreateBattleBingoSquareSprite(sBattleBingo.selectedRow, sBattleBingo.selectedCol, BINGO_SQUARE_COMPLETE);
    }

    UpdateBattleBingoLines(drawNewLines);
    if (IsBattleBingoBoardComplete())
        sBattleBingo.boardCompletePending = TRUE;
}

static void TryStartBattleBingoBoardComplete(u8 taskId)
{
    if (!sBattleBingo.boardCompletePending)
        return;

    sBattleBingo.pickingPartner = FALSE;
    sBattleBingo.partnerPickLocked = FALSE;
    ShowBattleBingoPrompt(sBattleBingoText_BoardComplete);
    PlayBGM(MUS_VICTORY_LEAGUE);
    gTasks[taskId].func = Task_BattleBingoBoardCompleteMessage;
}

static void ShowBattleBingoPrompt(const u8 *text)
{
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
    HideBattleBingoPrompt();
    DrawDialogueFrame(BINGO_WIN_MESSAGE, FALSE);
    AddTextPrinterParameterized2(BINGO_WIN_MESSAGE, FONT_NORMAL, text, TEXT_SKIP_DRAW, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    PutWindowTilemap(BINGO_WIN_MESSAGE);
    CopyWindowToVram(BINGO_WIN_MESSAGE, COPYWIN_FULL);
}

static void HideBattleBingoPrompt(void)
{
    ClearDialogWindowAndFrame(BINGO_WIN_MESSAGE, FALSE);
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
}

static void GiveBattleBingoSelectedItem(u8 taskId)
{
    struct BattleBingoSquareState *square = &sBattleBingo.squares[sBattleBingo.selectedRow][sBattleBingo.selectedCol];

    AddBagItem(square->item, 1);
    StringCopy(gStringVar1, GetItemName(square->item));
    StringExpandPlaceholders(sBattleBingo.message, sBattleBingoText_GotItem);
            ClearBattleBingoSelectedSquare(TRUE);
    ShowBattleBingoPrompt(sBattleBingo.message);
    PlayFanfare(MUS_OBTAIN_ITEM);
    gTasks[taskId].func = Task_BattleBingoBoardWaitItemFanfare;
}

static void StartBattleBingoWildBattle(u8 taskId)
{
    struct BattleBingoSquareState *square = &sBattleBingo.squares[sBattleBingo.selectedRow][sBattleBingo.selectedCol];

    HideBattleBingoPrompt();
    BackupBattleBingoPartnerParty(sBattleBingo.selectedPartyIndex);
    PrepareBattleBingoWildParty(square->bingoMonId);
    gMain.savedCallback = CB2_ReturnToBattleBingoBoardAfterBattle;
    gBattleTypeFlags = BATTLE_TYPE_BINGO;
    gBattleOutcome = 0;
    PlayMapChosenOrBattleBGM(0);
    BattleTransition_Start(GetWildBattleTransition());
    gTasks[taskId].func = Task_BattleBingoBoardWaitForBattleTransition;
}

static void StartBattleBingoBossBattle(u8 taskId)
{
    struct BattleBingoSquareState *square = &sBattleBingo.squares[sBattleBingo.selectedRow][sBattleBingo.selectedCol];
    const struct Trainer *trainer = GetTrainerStructFromId(square->trainer);
    u32 i;

    HideBattleBingoPrompt();
    InitTrainerBattleParameter();
    TRAINER_BATTLE_PARAM.opponentA = square->trainer;
    TRAINER_BATTLE_PARAM.playMusicA = TRUE;
    TRAINER_BATTLE_PARAM.introTextA = (u8 *)sBattleBingoText_BossIntro;
    TRAINER_BATTLE_PARAM.defeatTextA = (u8 *)sBattleBingoText_BossDefeat;

    for (i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][i]);
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_A], trainer);
    gPartiesCount[B_TRAINER_OPPONENT_A] = CalculatePartyCount(B_TRAINER_OPPONENT_A);

    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_BINGO;
    if (GetTrainerBattleType(square->trainer) == TRAINER_BATTLE_TYPE_DOUBLES)
        gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;

    gMain.savedCallback = CB2_ReturnToBattleBingoBoardAfterBattle;
    gBattleOutcome = 0;
    PlayMapChosenOrBattleBGM(0);
    BattleTransition_Start(GetTrainerBattleTransition());
    gTasks[taskId].func = Task_BattleBingoBoardWaitForBattleTransition;
}

static void PrepareBattleBingoWildParty(u16 bingoMonId)
{
    u32 i;
    struct Trainer trainer = {0};
    struct TrainerGenerator trainerGen;

    for (i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][i]);

    StringCopy(trainer.trainerName, COMPOUND_STRING("BINGO"));
    MakeTrainerGenerator(&trainerGen, &trainer);
    GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][0], &gBingoMons[bingoMonId], &trainerGen);
    sBattleBingo.wildBattleMon = gParties[B_TRAINER_OPPONENT_A][0];
    sBattleBingo.wildBattleMonValid = TRUE;
    gPartiesCount[B_TRAINER_OPPONENT_A] = 1;
}

static void BackupBattleBingoPartnerParty(u8 partyIndex)
{
    u32 i;

    sBattleBingo.partyBackedUp = TRUE;
    sBattleBingo.originalPartyCount = CalculatePlayerPartyCount();
    for (i = 0; i < PARTY_SIZE; i++)
        sBattleBingo.partyBackup[i] = gParties[B_TRAINER_PLAYER][i];

    gParties[B_TRAINER_PLAYER][0] = sBattleBingo.partyBackup[partyIndex];
    for (i = 1; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
    CalculatePlayerPartyCount();
}

static bool8 TryGetBattleBingoCaughtMon(struct Pokemon *capturedMon)
{
    struct Trainer trainer = {0};
    struct TrainerGenerator trainerGen;
    u32 ball = gBattleResults.caughtMonBall;
    enum Species species;
    struct BattleBingoSquareState *square = &sBattleBingo.squares[sBattleBingo.selectedRow][sBattleBingo.selectedCol];

    if (square->kind != BINGO_RUNTIME_WILD || square->bingoMonId >= BINGO_MON_COUNT)
        return FALSE;

    StringCopyN(trainer.trainerName, gSaveBlock2Ptr->playerName, TRAINER_NAME_LENGTH + 1);
    trainer.gender = gSaveBlock2Ptr->playerGender;
    MakeTrainerGenerator(&trainerGen, &trainer);
    trainerGen.otID = OTID_STRUCT_PLAYER_ID;
    trainerGen.localRngState = LocalRandomSeed(Random32());
    GenerateMonFromTrainerMon(capturedMon, &gBingoMons[square->bingoMonId], &trainerGen);

    species = GetMonData(capturedMon, MON_DATA_SPECIES_OR_EGG);
    if (species != SPECIES_NONE && species != SPECIES_EGG)
    {
        if (ball == 0)
            ball = GetMonData(&sBattleBingo.wildBattleMon, MON_DATA_POKEBALL);
        SetMonData(capturedMon, MON_DATA_POKEBALL, &ball);
        return TRUE;
    }

    return FALSE;
}

static void RestoreBattleBingoPartnerParty(bool8 captured)
{
    u32 i;
    struct Pokemon battledMon = gParties[B_TRAINER_PLAYER][0];
    struct Pokemon capturedMon;
    bool8 hasCapturedMon = FALSE;

    if (captured)
        hasCapturedMon = TryGetBattleBingoCaughtMon(&capturedMon);

    for (i = 0; i < PARTY_SIZE; i++)
        gParties[B_TRAINER_PLAYER][i] = sBattleBingo.partyBackup[i];

    gParties[B_TRAINER_PLAYER][sBattleBingo.selectedPartyIndex] = battledMon;

    if (hasCapturedMon)
    {
        for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
        {
            enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
            if (species == SPECIES_NONE || species == SPECIES_EGG)
            {
                gParties[B_TRAINER_PLAYER][i] = capturedMon;
                break;
            }
        }
    }

    for (i = BATTLE_BINGO_MAX_PARTY_SIZE; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);

    sBattleBingo.partyBackedUp = FALSE;
    sBattleBingo.wildBattleMonValid = FALSE;
    CalculatePlayerPartyCount();
}

static bool8 BattleBingoPlayerHasRevives(void)
{
    return CheckBagHasItem(ITEM_REVIVE, 1) || CheckBagHasItem(ITEM_MAX_REVIVE, 1);
}

static bool8 BattleBingoPlayerHasUsableMons(void)
{
    u32 i;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        u32 hp = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP);

        if (species != SPECIES_NONE && species != SPECIES_EGG && hp != 0)
            return TRUE;
    }

    return FALSE;
}

u16 BattleBingoGetLastResult(void)
{
    return sBattleBingoLastResult;
}

u16 BattleBingoGetLastPrizeMoney(void)
{
    return sBattleBingoLastPrizeMoney;
}

u64 BattleBingoGetWildPokemonAiFlags(void)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);

    if (rules != NULL && rules->wildPokemonAi != 0)
        return rules->wildPokemonAi;

    return AI_FLAG_BASIC_TRAINER;
}

u64 BattleBingoGetBossPokemonAiFlags(void)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);

    if (rules != NULL && rules->bossPokemonAi != 0)
        return rules->bossPokemonAi;

    return AI_FLAG_BASIC_TRAINER
        | AI_FLAG_SMART_SWITCHING
        | AI_FLAG_SMART_MON_CHOICES
        | AI_FLAG_PP_STALL_PREVENTION
        | AI_FLAG_SMART_TERA
        | AI_FLAG_RANDOMIZE_SWITCHIN
        | AI_FLAG_PREDICTION
        | AI_FLAG_ASSUMPTIONS;
}

static void CreateBattleBingoBoardSprites(void)
{
    u32 row;
    u32 col;

    for (row = 0; row < BINGO_BOARD_SIZE; row++)
    {
        for (col = 0; col < BINGO_BOARD_SIZE; col++)
        {
            struct BattleBingoSquareState *square = &sBattleBingo.squares[row][col];

            square->spriteId = CreateBattleBingoSquareSprite(row, col, square->cleared ? BINGO_SQUARE_COMPLETE : square->visibleSquare);
        }
    }
}

static u8 CreateBattleBingoSquareSprite(u8 row, u8 col, u8 squareId)
{
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_SQUARE_GFX_START + squareId,
        .paletteTag = sBattleBingoSquarePaletteTags[squareId],
        .oam = &sBattleBingoSquareOam,
        .anims = sBattleBingoSquareAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    return CreateSprite(&template, sBattleBingoSquareSpriteX[col], sBattleBingoSquareSpriteY[row], 8);
}

static void CreateBattleBingoCompletedLineSprites(void)
{
    u32 i;

    sBattleBingo.lineSpriteCount = 0;
    for (i = 0; i < BINGO_MAX_LINE_SPRITES; i++)
        sBattleBingo.lineSpriteIds[i] = MAX_SPRITES;

    for (i = 0; i < BATTLE_BINGO_MAX_LINES; i++)
    {
        if (sBattleBingo.completedLineFlags & (1 << i))
            CreateBattleBingoLineSprites(i);
    }
}

static void CreateBattleBingoLineSprites(u8 line)
{
    u32 i;

    if (line < BINGO_BOARD_SIZE)
    {
        s16 y = sBattleBingoSquareSpriteY[line];
        for (i = 0; i < BINGO_BOARD_SIZE - 1; i++)
        {
            s16 x = (sBattleBingoSquareSpriteX[i] + sBattleBingoSquareSpriteX[i + 1]) / 2;
            CreateBattleBingoLineChunk(x, y, BINGO_LINE_HORIZONTAL);
        }
        return;
    }

    line -= BINGO_BOARD_SIZE;
    if (line < BINGO_BOARD_SIZE)
    {
        s16 x = sBattleBingoSquareSpriteX[line];
        for (i = 0; i < BINGO_BOARD_SIZE - 1; i++)
        {
            s16 y = (sBattleBingoSquareSpriteY[i] + sBattleBingoSquareSpriteY[i + 1]) / 2;
            CreateBattleBingoLineChunk(x, y, BINGO_LINE_VERTICAL);
        }
        return;
    }

    line -= BINGO_BOARD_SIZE;
    for (i = 0; i < BINGO_BOARD_SIZE - 1; i++)
    {
        s16 x1 = (line == 0) ? sBattleBingoSquareSpriteX[i] : sBattleBingoSquareSpriteX[BINGO_BOARD_SIZE - 1 - i];
        s16 x2 = (line == 0) ? sBattleBingoSquareSpriteX[i + 1] : sBattleBingoSquareSpriteX[BINGO_BOARD_SIZE - 2 - i];
        s16 x = (x1 + x2) / 2;
        s16 y = (sBattleBingoSquareSpriteY[i] + sBattleBingoSquareSpriteY[i + 1]) / 2;
        CreateBattleBingoLineChunk(x, y, line == 0 ? BINGO_LINE_DIAGONAL_DOWN_RIGHT : BINGO_LINE_DIAGONAL_DOWN_LEFT);
    }
}

static void CreateBattleBingoLineChunk(s16 x, s16 y, u8 lineGfx)
{
    u8 spriteId;
    const struct SpriteTemplate template =
    {
        .tileTag = TAG_BINGO_LINE_GFX_START + lineGfx,
        .paletteTag = TAG_BINGO_LINE_PAL,
        .oam = &sBattleBingoLineOam,
        .anims = sBattleBingoTextAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    if (sBattleBingo.lineSpriteCount >= BINGO_MAX_LINE_SPRITES)
        return;

    spriteId = CreateSprite(&template, x, y, 7);
    if (spriteId != MAX_SPRITES)
        sBattleBingo.lineSpriteIds[sBattleBingo.lineSpriteCount++] = spriteId;
}
