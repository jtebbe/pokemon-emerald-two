#include "global.h"
#include "battle.h"
#include "battle_bingo.h"
#include "battle_factory_screen.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "bg.h"
#include "bingo_mons.h"
#include "data.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "item.h"
#include "item_menu.h"
#include "line_break.h"
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
#include "script.h"
#include "script_menu.h"
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
#include "constants/flags.h"
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
#define BINGO_MESSAGE_TEXT_WIDTH     104
#define BINGO_MESSAGE_MAX_LINES      2
#define BINGO_BOARD_SIZE            5

#define tCursorArea data[0]
#define tCursorX data[1]
#define tCursorY data[2]
#define tCursorSpriteId data[3]
#define tSelectedPartyIndex data[4]
#define tSelectedSquareRow data[5]
#define tSelectedSquareCol data[6]
#define tExitResult data[7]
#define tBoardResourcesClosed data[8]
#define tPartyActionMenuActive data[9]
#define tSwitchSourcePartyIndex data[10]

#define BINGO_NO_SWITCH_SOURCE (-1)

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
    BINGO_WIN_PARTY_ACTIONS,
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
    BINGO_PARTY_ACTION_SUMMARY,
    BINGO_PARTY_ACTION_SWITCH,
    BINGO_PARTY_ACTION_CANCEL,
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
    BINGO_SQUARE_HEAL,
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
    BINGO_RUNTIME_HEAL,
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
    u8 partyIconSpriteIds[BATTLE_BINGO_MAX_PARTY_SIZE];
    u8 partyHpBarSpriteIds[BATTLE_BINGO_MAX_PARTY_SIZE][BINGO_HP_BAR_SEGMENTS];
    u8 partyStatusSpriteIds[BATTLE_BINGO_MAX_PARTY_SIZE];
    u8 textSpriteIds[BINGO_TEXT_COUNT];
    u8 lineSpriteIds[BINGO_MAX_LINE_SPRITES];
    u8 message[64];
};

static void CB2_InitBattleBingoBoard(void);
static void CB2_WaitForBattleBingoBoardFadeOut(void);
static void CB2_WaitForSelectedBattleBingoBoardFadeOut(void);
static void CB2_RunBattleBingoBoard(void);
static void CB2_ReturnToBattleBingoBoard(void);
static void CB2_ReturnToBattleBingoBoardAfterBattle(void);
static void VBlankCB_BattleBingoBoard(void);
static void Task_BattleBingoResumeScript(u8 taskId);
static void Task_BattleBingoBoard(u8 taskId);
static void Task_BattleBingoBoardFadeOut(u8 taskId);
static void Task_BattleBingoBoardFadeToBag(u8 taskId);
static void Task_BattleBingoBoardFadeToSummary(u8 taskId);
static void Task_BattleBingoBoardFadeToBossBattle(u8 taskId);
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
static void GivePurchasedBattleBingoCheatItems(void);
static void RestoreBattleBingoBag(void);
static void RestorePurchasedBattleBingoCheatItems(void);
static void WipeBattleBingoParty(void);
static void CloseBattleBingoBoardResources(void);
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
static void CreateBattleBingoHpBar(u8 x, u8 y, u8 hpPercent, u8 *spriteIds);
static void InitBattleBingoPartySpriteIds(void);
static void DestroyBattleBingoPartySprites(void);
static void RefreshBattleBingoPartySprites(void);
static u8 CreateBattleBingoCursorSprite(void);
static void RememberBattleBingoCursor(u8 area, u8 x, u8 y);
static void RestoreBattleBingoCursor(u8 taskId);
static void MoveBattleBingoCursor(u8 taskId, s8 deltaX, s8 deltaY);
static void UpdateBattleBingoCursorSpritePosition(u8 taskId);
static void HandleBattleBingoCursorSelect(u8 taskId);
static void HandleBattleBingoPartnerPickInput(u8 taskId);
static void HandleBattleBingoPartyActionMenuInput(u8 taskId);
static void HandleBattleBingoPartySwitchInput(u8 taskId);
static void ShowBattleBingoPartyActionMenu(u8 taskId);
static void HideBattleBingoPartyActionMenu(u8 taskId);
static void StartBattleBingoPartySwitch(u8 taskId);
static void CancelBattleBingoPartySwitch(u8 taskId);
static bool8 BattleBingoPartySlotHasMon(u8 partyIndex);
static void SwapBattleBingoPartyMons(u8 first, u8 second);
static void InitBattleBingoRuntime(enum BattleBingoBoardId boardId);
static u16 FindBattleBingoMonByTags(u64 requiredTags, u64 forbiddenTags, const u16 *excludedSpecies, u8 excludedSpeciesCount, const u16 *excludedMons, u8 excludedMonCount);
static u16 GetBattleBingoItemByIndex(const struct BattleBingoBoardRules *rules, u8 itemIndex);
static u16 GetBattleBingoBoardMusic(const struct BattleBingoBoardRules *rules);
static u8 CountBattleBingoItemRules(const struct BattleBingoBoardRules *rules);
static u8 CountBattleBingoHealRules(const struct BattleBingoBoardRules *rules);
static u8 GetBattleBingoWildSquare(const struct BattleBingoWildRule *wildRule, u16 bingoMonId);
static u64 GetBattleBingoTypeTag(u8 type);
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
static void DrawBattleBingoPartyActionFrame(u8 windowId, bool8 copyToVram);
static void GiveBattleBingoSelectedItem(u8 taskId);
static void UseBattleBingoHealSquare(u8 taskId);
static void HealBattleBingoParty(void);
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
static void CreateFixedBattleBingoStarter(u16 bingoMonId);
static bool8 BattleBingoPlayerHasCardForBoard(enum BattleBingoBoardId boardId);

struct BattleBingoCardDefinition
{
    u16 item;
    enum BattleBingoBoardId boardId;
    const u8 *name;
};

struct BattleBingoCheatItem
{
    u16 item;
    u16 flag;
    u32 price;
};

static const struct BattleBingoCheatItem sBattleBingoCheatItems[] =
{
    {ITEM_HEAVY_DUTY_BOOTS, FLAG_BINGO_CHEAT_HEAVY_DUTY_BOOTS, 50000},
    {ITEM_CLEAR_AMULET, FLAG_BINGO_CHEAT_CLEAR_AMULET, 50000},
    {ITEM_BRIGHT_POWDER, FLAG_BINGO_CHEAT_BRIGHT_POWDER, 50000},
    {ITEM_SITRUS_BERRY, FLAG_BINGO_CHEAT_SITRUS_BERRY, 50000},
    {ITEM_QUICK_CLAW, FLAG_BINGO_CHEAT_QUICK_CLAW, 50000},
    {ITEM_FOCUS_SASH, FLAG_BINGO_CHEAT_FOCUS_SASH, 100000},
    {ITEM_LIFE_ORB, FLAG_BINGO_CHEAT_LIFE_ORB, 100000},
    {ITEM_LEFTOVERS, FLAG_BINGO_CHEAT_LEFTOVERS, 100000},
    {ITEM_EXPERT_BELT, FLAG_BINGO_CHEAT_EXPERT_BELT, 100000},
    {ITEM_ASSAULT_VEST, FLAG_BINGO_CHEAT_ASSAULT_VEST, 150000},
    {ITEM_FULL_RESTORE, FLAG_BINGO_CHEAT_FULL_RESTORE, 150000},
    {ITEM_CHOICE_BAND, FLAG_BINGO_CHEAT_CHOICE_BAND, 200000},
    {ITEM_CHOICE_SPECS, FLAG_BINGO_CHEAT_CHOICE_SPECS, 200000},
    {ITEM_CHOICE_SCARF, FLAG_BINGO_CHEAT_CHOICE_SCARF, 250000},
    {ITEM_MAX_REVIVE, FLAG_BINGO_CHEAT_MAX_REVIVE, 250000},
};

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
static const u8 sBattleBingoText_Summary[] = _("Summary");
static const u8 sBattleBingoText_Switch[] = _("Switch");
static const u8 sBattleBingoText_Cancel[] = _("Cancel");
static const u8 sBattleBingoText_SwitchPrompt[] = _("Switch with which POKéMON?");
static const u8 sBattleBingoText_FWGCard[] = _("FWG BC");
static const u8 sBattleBingoText_NormalCard[] = _("Normal BC");
static const u8 sBattleBingoText_LCCard[] = _("Little Cup BC");
static const u8 sBattleBingoText_CipherCard[] = _("Cipher BC");
static const u8 sBattleBingoText_LegendCard[] = _("Legendary BC");
static const u8 sBattleBingoText_DragonCard[] = _("Dragon BC");
static const u8 sBattleBingoText_StrangeCard[] = _("Strange BC");
static const u8 sBattleBingoText_MusicCard[] = _("Music BC");
static const u8 sBattleBingoText_MonsterCard[] = _("Monster BC");
static const struct MenuAction sBattleBingoPartyActionMenu[] =
{
    {sBattleBingoText_Summary},
    {sBattleBingoText_Switch},
    {sBattleBingoText_Cancel},
};
static const struct BattleBingoCardDefinition sBattleBingoCards[] =
{
    {ITEM_FWG_BINGO_CARD, BATTLE_BINGO_BOARD_FWG, sBattleBingoText_FWGCard},
    {ITEM_NORMAL_BINGO_CARD, BATTLE_BINGO_BOARD_NORMAL, sBattleBingoText_NormalCard},
    {ITEM_LC_BINGO_CARD, BATTLE_BINGO_BOARD_LC, sBattleBingoText_LCCard},
    {ITEM_CIPHER_BINGO_CARD, BATTLE_BINGO_BOARD_CIPHER, sBattleBingoText_CipherCard},
    {ITEM_LEGEND_BINGO_CARD, BATTLE_BINGO_BOARD_LEGENDARY, sBattleBingoText_LegendCard},
    {ITEM_DRAGON_BINGO_CARD, BATTLE_BINGO_BOARD_DRAGON, sBattleBingoText_DragonCard},
    {ITEM_STRANGE_BINGO_CARD, BATTLE_BINGO_BOARD_STRANGE, sBattleBingoText_StrangeCard},
    {ITEM_MUSIC_BINGO_CARD, BATTLE_BINGO_BOARD_MUSIC, sBattleBingoText_MusicCard},
    {ITEM_MONSTER_BINGO_CARD, BATTLE_BINGO_BOARD_MONSTER, sBattleBingoText_MonsterCard},
};
static EWRAM_DATA struct BattleBingoRuntime sBattleBingo = {0};
static EWRAM_DATA u16 sBattleBingoLastResult = BATTLE_BINGO_RESULT_LOSS;
static EWRAM_DATA u16 sBattleBingoLastPrizeMoney = 0;
static EWRAM_DATA enum BattleBingoBoardId sBattleBingoLastCompletedBoardId = 0;
static EWRAM_DATA bool8 sBattleBingoLastCompleteWasFirstClear = FALSE;
static EWRAM_DATA enum BattleBingoBoardId sBattleBingoSelectedBoardId = BATTLE_BINGO_BOARD_FWG;

bool32 BattleBingoIsCheatItemPurchased(enum Item itemId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCheatItems); i++)
    {
        if (sBattleBingoCheatItems[i].item == itemId)
            return FlagGet(sBattleBingoCheatItems[i].flag);
    }

    return FALSE;
}

void BattleBingoSetCheatItemPurchased(enum Item itemId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCheatItems); i++)
    {
        if (sBattleBingoCheatItems[i].item == itemId)
        {
            FlagSet(sBattleBingoCheatItems[i].flag);
            return;
        }
    }
}

u32 BattleBingoGetCheatItemPrice(enum Item itemId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCheatItems); i++)
    {
        if (sBattleBingoCheatItems[i].item == itemId)
            return sBattleBingoCheatItems[i].price;
    }

    return GetItemPrice(itemId);
}

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
static const u32 sBattleBingoSquareHeal_Gfx[] = INCGFX_U32("graphics/bingo/squares/bingo_heal.png", ".4bpp");
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
    [BINGO_WIN_PARTY_ACTIONS] =
    {
        .bg = 1,
        .tilemapLeft = 20,
        .tilemapTop = 8,
        .width = 8,
        .height = 6,
        .paletteNum = DLG_WINDOW_PALETTE_NUM,
        .baseBlock = 0x1D0,
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
    .priority = 0,
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
    [BINGO_SQUARE_HEAL] = sBattleBingoSquareHeal_Gfx,
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
    [BINGO_SQUARE_HEAL] = TAG_BINGO_SQUARE_PAL_1,
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

void ShowBattleBingoBoard(void)
{
    if (!sBattleBingo.initialized)
        InitBattleBingoRuntime(sBattleBingoSelectedBoardId);

    sBattleBingo.playBoardMusic = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    SetMainCallback2(CB2_WaitForBattleBingoBoardFadeOut);
}

void ShowBattleBingoBoardFromBlack(void)
{
    if (!sBattleBingo.initialized)
        InitBattleBingoRuntime(sBattleBingoSelectedBoardId);

    sBattleBingo.playBoardMusic = TRUE;
    SetMainCallback2(CB2_InitBattleBingoBoard);
}

static void CB2_WaitForBattleBingoBoardFadeOut(void)
{
    UpdatePaletteFade();

    if (!gPaletteFade.active)
        SetMainCallback2(CB2_InitBattleBingoBoard);
}

static void CB2_WaitForSelectedBattleBingoBoardFadeOut(void)
{
    const struct BattleBingoBoardRules *rules;

    UpdatePaletteFade();
    if (gPaletteFade.active)
        return;

    rules = GetBattleBingoBoardRules(sBattleBingoSelectedBoardId);
    if (rules != NULL && rules->starterMode == BATTLE_BINGO_STARTER_CHOICE)
    {
        DoBattleBingoStarterSelectScreen();
        return;
    }

    if (rules != NULL && rules->starterCount != 0)
    {
        u16 starter = rules->starters[0];

        if (rules->starterMode == BATTLE_BINGO_STARTER_RANDOM)
            starter = rules->starters[Random() % rules->starterCount];

        CreateFixedBattleBingoStarter(starter);
    }
    ShowBattleBingoBoardFromBlack();
}

static void CB2_InitBattleBingoBoard(void)
{
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);
    u32 i;

    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetVBlankCallback(NULL);
    CpuFill32(0, (void *)VRAM, VRAM_SIZE);
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
    InitBattleBingoPartySpriteIds();
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
        gTasks[taskId].tPartyActionMenuActive = FALSE;
        gTasks[taskId].tSwitchSourcePartyIndex = BINGO_NO_SWITCH_SOURCE;
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

static void Task_BattleBingoResumeScript(u8 taskId)
{
    ScriptContext_Enable();
    DestroyTask(taskId);
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

    if (gTasks[taskId].tPartyActionMenuActive)
    {
        HandleBattleBingoPartyActionMenuInput(taskId);
        return;
    }

    if (gTasks[taskId].tSwitchSourcePartyIndex != BINGO_NO_SWITCH_SOURCE)
    {
        HandleBattleBingoPartySwitchInput(taskId);
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

static void Task_BattleBingoBoardFadeToBossBattle(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CloseBattleBingoBoardResources();
        ResetSpriteData();
        FreeAllSpritePalettes();
        ResetOamRange(0, 128);
        LoadOam();
        CpuFill32(0, (void *)VRAM, VRAM_SIZE);
        gTasks[taskId].tBoardResourcesClosed = TRUE;
        BattleTransition_Start(GetTrainerBattleTransition());
        gTasks[taskId].func = Task_BattleBingoBoardWaitForBattleTransition;
    }
}

static void Task_BattleBingoBoardWaitForBattleTransition(u8 taskId)
{
    if (IsBattleTransitionDone() == TRUE)
    {
        if (!gTasks[taskId].tBoardResourcesClosed)
            CloseBattleBingoBoardResources();
        DestroyTask(taskId);
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
    CloseBattleBingoBoardResources();
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
    sBattleBingoLastCompletedBoardId = BATTLE_BINGO_BOARD_COUNT;
    sBattleBingoLastCompleteWasFirstClear = FALSE;
    if ((result == BATTLE_BINGO_RESULT_PRIZE || result == BATTLE_BINGO_RESULT_COMPLETE) && rules != NULL)
        sBattleBingoLastPrizeMoney = BattleBingoGetPrizeMoney(sBattleBingo.bingoCount, rules->prizeMultiplierQ8_8);
    else
        sBattleBingoLastPrizeMoney = 0;

    RestoreBattleBingoBag();
    WipeBattleBingoParty();
    if (result == BATTLE_BINGO_RESULT_COMPLETE && rules != NULL && rules->clearedFlag != 0)
    {
        sBattleBingoLastCompletedBoardId = sBattleBingo.boardId;
        sBattleBingoLastCompleteWasFirstClear = !FlagGet(rules->clearedFlag);
        FlagSet(rules->clearedFlag);
    }

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
    if (rules == NULL)
        return;

    if (!rules->startWithoutBall)
        AddBagItem(ITEM_MASTER_BALL, 1);
    GivePurchasedBattleBingoCheatItems();
}

static void GivePurchasedBattleBingoCheatItems(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCheatItems); i++)
    {
        if (FlagGet(sBattleBingoCheatItems[i].flag) && !CheckBagHasItem(sBattleBingoCheatItems[i].item, 1))
            AddBagItem(sBattleBingoCheatItems[i].item, 1);
    }
}

static void RestoreBattleBingoBag(void)
{
    if (sBattleBingo.bagBackedUp)
    {
        gSaveBlock1Ptr->bag = sBattleBingo.bagBackup;
        RestorePurchasedBattleBingoCheatItems();
    }
}

static void RestorePurchasedBattleBingoCheatItems(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCheatItems); i++)
    {
        if (FlagGet(sBattleBingoCheatItems[i].flag) && !CheckBagHasItem(sBattleBingoCheatItems[i].item, 1))
            AddBagItem(sBattleBingoCheatItems[i].item, 1);
    }
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
    CloseBattleBingoBoardResources();
    DestroyTask(taskId);
}

static void CloseBattleBingoBoardResources(void)
{
    void *bg0Tilemap = GetBgTilemapBuffer(0);
    void *bg1Tilemap = GetBgTilemapBuffer(1);

    SetVBlankCallback(NULL);
    FreeAllWindowBuffers();
    if (bg0Tilemap != NULL)
    {
        Free(bg0Tilemap);
        UnsetBgTilemapBuffer(0);
    }
    if (bg1Tilemap != NULL)
    {
        Free(bg1Tilemap);
        UnsetBgTilemapBuffer(1);
    }
    FreeMonIconPalettes();
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
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingo.boardId);

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
    const struct BattleBingoBoardRules *rules = GetBattleBingoBoardRules(sBattleBingoSelectedBoardId);

    sBattleBingo.initialized = FALSE;
    if (rules != NULL && rules->starterMode != BATTLE_BINGO_STARTER_CHOICE && rules->starterCount != 0)
    {
        u16 starter = rules->starters[0];

        if (rules->starterMode == BATTLE_BINGO_STARTER_RANDOM)
            starter = rules->starters[Random() % rules->starterCount];

        CreateFixedBattleBingoStarter(starter);
        CreateTask(Task_BattleBingoResumeScript, 0);
        return;
    }

    DoBattleBingoStarterSelectScreen();
}

static void CreateFixedBattleBingoStarter(u16 bingoMonId)
{
    u32 i;
    struct TrainerGenerator trainerGen =
    {
        .gender = gSaveBlock2Ptr->playerGender,
        .isFrontier = FALSE,
        .trainerClass = 0,
        .otID = OTID_STRUCT_PLAYER_ID,
        .localRngState = LocalRandomSeed(Random32()),
    };

    if (bingoMonId >= BINGO_MON_COUNT)
        return;

    StringCopyN(trainerGen.name, gSaveBlock2Ptr->playerName, TRAINER_NAME_LENGTH + 1);
    for (i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);

    GenerateMonFromTrainerMon(&gParties[B_TRAINER_PLAYER][0], &gBingoMons[bingoMonId], &trainerGen);
    CalculatePlayerPartyCount();
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
        {
            gSprites[spriteId].oam.priority = 1;
            sBattleBingo.partyIconSpriteIds[i] = spriteId;
        }
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
        CreateBattleBingoHpBar(32, sBattleBingoPartyHpBarY[i], hpPercent, sBattleBingo.partyHpBarSpriteIds[i]);
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
            sBattleBingo.partyStatusSpriteIds[i] = spriteId;
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
    struct BattleBingoSquareState *square;

    switch (gTasks[taskId].tCursorArea)
    {
    case BINGO_CURSOR_AREA_PARTY:
        partyIndex = gTasks[taskId].tCursorY;
        if (BattleBingoPartySlotHasMon(partyIndex))
        {
            PlaySE(SE_SELECT);
            gTasks[taskId].tSelectedPartyIndex = partyIndex;
            RememberBattleBingoCursor(BINGO_CURSOR_AREA_PARTY, 0, partyIndex);
            ShowBattleBingoPartyActionMenu(taskId);
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
        case BINGO_RUNTIME_HEAL:
            UseBattleBingoHealSquare(taskId);
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

static void CreateBattleBingoHpBar(u8 x, u8 y, u8 hpPercent, u8 *spriteIds)
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
        {
            StartSpriteAnim(&gSprites[spriteId], BINGO_HP_BAR_SEGMENT_WIDTH - segmentFill);
            if (spriteIds != NULL)
                spriteIds[i] = spriteId;
        }
    }
}

static void InitBattleBingoPartySpriteIds(void)
{
    u32 i;
    u32 j;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        sBattleBingo.partyIconSpriteIds[i] = MAX_SPRITES;
        sBattleBingo.partyStatusSpriteIds[i] = MAX_SPRITES;
        for (j = 0; j < BINGO_HP_BAR_SEGMENTS; j++)
            sBattleBingo.partyHpBarSpriteIds[i][j] = MAX_SPRITES;
    }
}

static void DestroyBattleBingoPartySprites(void)
{
    u32 i;
    u32 j;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        if (sBattleBingo.partyIconSpriteIds[i] != MAX_SPRITES)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sBattleBingo.partyIconSpriteIds[i]]);
            sBattleBingo.partyIconSpriteIds[i] = MAX_SPRITES;
        }

        if (sBattleBingo.partyStatusSpriteIds[i] != MAX_SPRITES)
        {
            DestroySprite(&gSprites[sBattleBingo.partyStatusSpriteIds[i]]);
            sBattleBingo.partyStatusSpriteIds[i] = MAX_SPRITES;
        }

        for (j = 0; j < BINGO_HP_BAR_SEGMENTS; j++)
        {
            if (sBattleBingo.partyHpBarSpriteIds[i][j] != MAX_SPRITES)
            {
                DestroySprite(&gSprites[sBattleBingo.partyHpBarSpriteIds[i][j]]);
                sBattleBingo.partyHpBarSpriteIds[i][j] = MAX_SPRITES;
            }
        }
    }
}

static void RefreshBattleBingoPartySprites(void)
{
    DestroyBattleBingoPartySprites();
    CreateBattleBingoPartyIcons();
    CreateBattleBingoPartyHpBars();
    CreateBattleBingoPartyStatusIcons();
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
    u8 totalItemCount;
    u8 totalHealCount;
    struct BattleBingoLayoutSlot
    {
        u8 visibleSquare;
        u8 kind;
        bool8 mystery;
        u8 wildRuleIndex;
    } slots[BATTLE_BINGO_NUM_SQUARES];
    u16 chosenWildMons[BATTLE_BINGO_NUM_SQUARES] = {0};
    u16 excludedStarterSpecies[BATTLE_BINGO_MAX_PARTY_SIZE] = {0};
    u8 cornerPositions[BATTLE_BINGO_BOSS_CORNER_COUNT];
    u8 freePositions[BATTLE_BINGO_NUM_SQUARES];
    bool8 bossPositions[BATTLE_BINGO_NUM_SQUARES] = {0};
    u8 freeCount = 0;
    u8 slotCount = 0;
    u8 itemIndex = 0;
    u8 mysteryWildCount = 0;
    u8 mysteryItemCount = 0;
    u8 chosenWildMonCount = 0;
    u8 excludedStarterSpeciesCount = 0;

    if (!BattleBingoBoardRulesAreValid(rules))
    {
        boardId = BATTLE_BINGO_BOARD_FWG;
        rules = GetBattleBingoBoardRules(boardId);
    }
    totalItemCount = CountBattleBingoItemRules(rules);
    totalHealCount = CountBattleBingoHealRules(rules);

    memset(&sBattleBingo, 0, sizeof(sBattleBingo));
    sBattleBingo.initialized = TRUE;
    sBattleBingo.boardId = boardId;
    RememberBattleBingoCursor(BINGO_CURSOR_AREA_BOARD, 0, 0);
    BackupBattleBingoBag();
    GiveBattleBingoStartingItems(rules);
    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);

        if (species != SPECIES_NONE && species != SPECIES_EGG)
            excludedStarterSpecies[excludedStarterSpeciesCount++] = species;
    }
    for (i = 0; i < BINGO_TEXT_COUNT; i++)
        sBattleBingo.textSpriteIds[i] = MAX_SPRITES;
    for (i = 0; i < BINGO_MAX_LINE_SPRITES; i++)
        sBattleBingo.lineSpriteIds[i] = MAX_SPRITES;

    for (i = 0; i < ARRAY_COUNT(cornerPositions); i++)
        cornerPositions[i] = BattleBingoGetBossCornerPosition(i);
    Shuffle(cornerPositions, ARRAY_COUNT(cornerPositions), sizeof(cornerPositions[0]));

    pos = 12;
    bossPositions[pos] = TRUE;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].visibleSquare = BINGO_SQUARE_BOSS;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].kind = BINGO_RUNTIME_BOSS;
    sBattleBingo.squares[pos / BINGO_BOARD_SIZE][pos % BINGO_BOARD_SIZE].trainer = rules->bosses[0];

    for (i = 1; i < rules->bossCount; i++)
    {
        pos = cornerPositions[i - 1];
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
    for (i = 0; i < totalHealCount; i++)
    {
        slots[slotCount].visibleSquare = BINGO_SQUARE_HEAL;
        slots[slotCount].kind = BINGO_RUNTIME_HEAL;
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
        else if (square->kind == BINGO_RUNTIME_WILD)
        {
            const struct BattleBingoWildRule *wildRule = &rules->wilds[slots[i].wildRuleIndex];

            square->bingoMonId = FindBattleBingoMonByTags(wildRule->requiredTags, wildRule->forbiddenTags, excludedStarterSpecies, excludedStarterSpeciesCount, chosenWildMons, chosenWildMonCount);
            chosenWildMons[chosenWildMonCount++] = square->bingoMonId;
            if (!square->isMystery)
                square->visibleSquare = GetBattleBingoWildSquare(wildRule, square->bingoMonId);
        }
    }

    for (i = 0; i < BATTLE_BINGO_NUM_SQUARES; i++)
        sBattleBingo.squares[i / BINGO_BOARD_SIZE][i % BINGO_BOARD_SIZE].spriteId = MAX_SPRITES;
}

static u16 FindBattleBingoMonByTags(u64 requiredTags, u64 forbiddenTags, const u16 *excludedSpecies, u8 excludedSpeciesCount, const u16 *excludedMons, u8 excludedMonCount)
{
    u16 i;
    u8 j;
    u16 matches = 0;
    u16 fallback = 0;
    u16 chosenMatch;

    for (i = 0; i < BINGO_MON_COUNT; i++)
    {
        if ((gBingoMons[i].tags & requiredTags) == requiredTags && (gBingoMons[i].tags & forbiddenTags) == 0)
        {
            bool8 excluded = FALSE;

            for (j = 0; j < excludedSpeciesCount; j++)
            {
                if (gBingoMons[i].species == excludedSpecies[j])
                {
                    excluded = TRUE;
                    break;
                }
            }
            if (excluded)
                continue;
            for (j = 0; j < excludedMonCount; j++)
            {
                if (i == excludedMons[j])
                {
                    excluded = TRUE;
                    break;
                }
            }
            if (excluded)
                continue;
            if (matches == 0)
                fallback = i;
            matches++;
        }
    }

    if (matches != 0)
    {
        chosenMatch = Random() % matches;
        for (i = 0; i < BINGO_MON_COUNT; i++)
        {
            if ((gBingoMons[i].tags & requiredTags) == requiredTags && (gBingoMons[i].tags & forbiddenTags) == 0)
            {
                bool8 excluded = FALSE;

                for (j = 0; j < excludedSpeciesCount; j++)
                {
                    if (gBingoMons[i].species == excludedSpecies[j])
                    {
                        excluded = TRUE;
                        break;
                    }
                }
                if (excluded)
                    continue;
                for (j = 0; j < excludedMonCount; j++)
                {
                    if (i == excludedMons[j])
                    {
                        excluded = TRUE;
                        break;
                    }
                }
                if (excluded)
                    continue;
                if (chosenMatch == 0)
                    return i;
                chosenMatch--;
            }
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

static u8 CountBattleBingoHealRules(const struct BattleBingoBoardRules *rules)
{
    u32 i;
    u8 count = 0;

    for (i = 0; i < rules->healRuleCount; i++)
        count += rules->heals[i].count;

    return count;
}

static u8 GetBattleBingoWildSquare(const struct BattleBingoWildRule *wildRule, u16 bingoMonId)
{
    if (wildRule->type != TYPE_MYSTERY && (wildRule->requiredTags & GetBattleBingoTypeTag(wildRule->type)))
        return GetBattleBingoTypeSquare(wildRule->type);

    if (bingoMonId >= BINGO_MON_COUNT)
        return BINGO_SQUARE_MYSTERY;

    return GetBattleBingoTypeSquare(GetSpeciesType(gBingoMons[bingoMonId].species, 0));
}

static u64 GetBattleBingoTypeTag(u8 type)
{
    switch (type)
    {
    case TYPE_BUG:
        return MON_POOL_TAG_BUG;
    case TYPE_DARK:
        return MON_POOL_TAG_DARK;
    case TYPE_DRAGON:
        return MON_POOL_TAG_DRAGON;
    case TYPE_ELECTRIC:
        return MON_POOL_TAG_ELECTRIC;
    case TYPE_FAIRY:
        return MON_POOL_TAG_FAIRY;
    case TYPE_FIGHTING:
        return MON_POOL_TAG_FIGHTING;
    case TYPE_FIRE:
        return MON_POOL_TAG_FIRE;
    case TYPE_FLYING:
        return MON_POOL_TAG_FLYING;
    case TYPE_GHOST:
        return MON_POOL_TAG_GHOST;
    case TYPE_GRASS:
        return MON_POOL_TAG_GRASS;
    case TYPE_GROUND:
        return MON_POOL_TAG_GROUND;
    case TYPE_ICE:
        return MON_POOL_TAG_ICE;
    case TYPE_NORMAL:
        return MON_POOL_TAG_NORMAL;
    case TYPE_POISON:
        return MON_POOL_TAG_POISON;
    case TYPE_PSYCHIC:
        return MON_POOL_TAG_PSYCHIC;
    case TYPE_ROCK:
        return MON_POOL_TAG_ROCK;
    case TYPE_STEEL:
        return MON_POOL_TAG_STEEL;
    case TYPE_WATER:
        return MON_POOL_TAG_WATER;
    default:
        return 0;
    }
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
    u8 prompt[128];

    StringCopy(prompt, text);
    BreakStringAutomatic(prompt, BINGO_MESSAGE_TEXT_WIDTH, BINGO_MESSAGE_MAX_LINES, FONT_NORMAL, HIDE_SCROLL_PROMPT);

    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
    HideBattleBingoPrompt();
    DrawDialogueFrame(BINGO_WIN_MESSAGE, FALSE);
    AddTextPrinterParameterized2(BINGO_WIN_MESSAGE, FONT_NORMAL, prompt, TEXT_SKIP_DRAW, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    PutWindowTilemap(BINGO_WIN_MESSAGE);
    CopyWindowToVram(BINGO_WIN_MESSAGE, COPYWIN_FULL);
}

static void HideBattleBingoPrompt(void)
{
    ClearDialogWindowAndFrameToTransparent(BINGO_WIN_MESSAGE, TRUE);
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
}

static void ShowBattleBingoPartyActionMenu(u8 taskId)
{
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
    DrawBattleBingoPartyActionFrame(BINGO_WIN_PARTY_ACTIONS, FALSE);
    PrintMenuTable(BINGO_WIN_PARTY_ACTIONS, ARRAY_COUNT(sBattleBingoPartyActionMenu), sBattleBingoPartyActionMenu);
    InitMenuInUpperLeftCornerNormal(BINGO_WIN_PARTY_ACTIONS, ARRAY_COUNT(sBattleBingoPartyActionMenu), 0);
    PutWindowTilemap(BINGO_WIN_PARTY_ACTIONS);
    CopyWindowToVram(BINGO_WIN_PARTY_ACTIONS, COPYWIN_FULL);
    gTasks[taskId].tPartyActionMenuActive = TRUE;
}

static void HideBattleBingoPartyActionMenu(u8 taskId)
{
    ClearDialogWindowAndFrameToTransparent(BINGO_WIN_PARTY_ACTIONS, TRUE);
    FillBgTilemapBufferRect(1, 0, 0, 0, 32, 32, 0);
    CopyBgTilemapBufferToVram(1);
    gTasks[taskId].tPartyActionMenuActive = FALSE;
}

static void DrawBattleBingoPartyActionFrame(u8 windowId, bool8 copyToVram)
{
    u8 bg = GetWindowAttribute(windowId, WINDOW_BG);
    u8 left = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u8 top = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u8 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u8 height = GetWindowAttribute(windowId, WINDOW_HEIGHT);

    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 1, left - 2, top - 1, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 3, left - 1, top - 1, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 4, left, top - 1, width - 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 5, left + width - 1, top - 1, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 6, left + width, top - 1, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 7, left - 2, top, 1, height, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 9, left - 1, top, width + 1, height, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + 10, left + width, top, 1, height, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + BG_TILE_V_FLIP(1), left - 2, top + height, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + BG_TILE_V_FLIP(3), left - 1, top + height, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + BG_TILE_V_FLIP(4), left, top + height, width - 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + BG_TILE_V_FLIP(5), left + width - 1, top + height, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillBgTilemapBufferRect(bg, DLG_WINDOW_BASE_TILE_NUM + BG_TILE_V_FLIP(6), left + width, top + height, 1, 1, DLG_WINDOW_PALETTE_NUM);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    PutWindowTilemap(windowId);
    if (copyToVram)
        CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void HandleBattleBingoPartyActionMenuInput(u8 taskId)
{
    switch (Menu_ProcessInputNoWrap())
    {
    case BINGO_PARTY_ACTION_SUMMARY:
        HideBattleBingoPartyActionMenu(taskId);
        RememberBattleBingoCursor(BINGO_CURSOR_AREA_PARTY, 0, gTasks[taskId].tSelectedPartyIndex);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_BattleBingoBoardFadeToSummary;
        break;
    case BINGO_PARTY_ACTION_SWITCH:
        HideBattleBingoPartyActionMenu(taskId);
        StartBattleBingoPartySwitch(taskId);
        break;
    case BINGO_PARTY_ACTION_CANCEL:
        HideBattleBingoPartyActionMenu(taskId);
        break;
    case MENU_B_PRESSED:
        PlaySE(SE_SELECT);
        HideBattleBingoPartyActionMenu(taskId);
        break;
    case MENU_NOTHING_CHOSEN:
        break;
    }
}

static void StartBattleBingoPartySwitch(u8 taskId)
{
    gTasks[taskId].tSwitchSourcePartyIndex = gTasks[taskId].tSelectedPartyIndex;
    ShowBattleBingoPrompt(sBattleBingoText_SwitchPrompt);
}

static void CancelBattleBingoPartySwitch(u8 taskId)
{
    PlaySE(SE_SELECT);
    gTasks[taskId].tSwitchSourcePartyIndex = BINGO_NO_SWITCH_SOURCE;
    HideBattleBingoPrompt();
}

static void HandleBattleBingoPartySwitchInput(u8 taskId)
{
    s16 source = gTasks[taskId].tSwitchSourcePartyIndex;
    u8 partyIndex;

    if (JOY_REPEAT(DPAD_LEFT))
        MoveBattleBingoCursor(taskId, -1, 0);
    else if (JOY_REPEAT(DPAD_RIGHT))
        MoveBattleBingoCursor(taskId, 1, 0);
    else if (JOY_REPEAT(DPAD_UP))
        MoveBattleBingoCursor(taskId, 0, -1);
    else if (JOY_REPEAT(DPAD_DOWN))
        MoveBattleBingoCursor(taskId, 0, 1);
    else if (JOY_NEW(B_BUTTON))
        CancelBattleBingoPartySwitch(taskId);
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].tCursorArea != BINGO_CURSOR_AREA_PARTY)
        {
            PlaySE(SE_FAILURE);
            return;
        }

        partyIndex = gTasks[taskId].tCursorY;
        if (source < 0 || !BattleBingoPartySlotHasMon(partyIndex))
        {
            PlaySE(SE_FAILURE);
            return;
        }

        PlaySE(SE_SELECT);
        if (partyIndex != source)
        {
            SwapBattleBingoPartyMons(source, partyIndex);
            RefreshBattleBingoPartySprites();
        }
        gTasks[taskId].tSelectedPartyIndex = partyIndex;
        gTasks[taskId].tSwitchSourcePartyIndex = BINGO_NO_SWITCH_SOURCE;
        RememberBattleBingoCursor(BINGO_CURSOR_AREA_PARTY, 0, partyIndex);
        HideBattleBingoPrompt();
    }
}

static bool8 BattleBingoPartySlotHasMon(u8 partyIndex)
{
    enum Species species;

    if (partyIndex >= BATTLE_BINGO_MAX_PARTY_SIZE || partyIndex >= CalculatePlayerPartyCount())
        return FALSE;

    species = GetMonData(&gParties[B_TRAINER_PLAYER][partyIndex], MON_DATA_SPECIES_OR_EGG);
    return species != SPECIES_NONE && species != SPECIES_EGG;
}

static void SwapBattleBingoPartyMons(u8 first, u8 second)
{
    struct Pokemon temp;

    temp = gParties[B_TRAINER_PLAYER][first];
    gParties[B_TRAINER_PLAYER][first] = gParties[B_TRAINER_PLAYER][second];
    gParties[B_TRAINER_PLAYER][second] = temp;
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

static void UseBattleBingoHealSquare(u8 taskId)
{
    HideBattleBingoPrompt();
    HealBattleBingoParty();
    ClearBattleBingoSelectedSquare(TRUE);
    RefreshBattleBingoPartySprites();
    PlaySE(SE_USE_ITEM);
    TryStartBattleBingoBoardComplete(taskId);
    if (!sBattleBingo.boardCompletePending)
        gTasks[taskId].func = Task_BattleBingoBoard;
}

static void HealBattleBingoParty(void)
{
    u32 i;

    for (i = 0; i < BATTLE_BINGO_MAX_PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);

        if (species != SPECIES_NONE && species != SPECIES_EGG)
            HealPokemon(mon);
    }
    CalculatePlayerPartyCount();
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
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_BattleBingoBoardFadeToBossBattle;
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

u16 BattleBingoCountClearedBoards(void)
{
    u16 count = 0;
    u32 i;

    for (i = 0; i < BATTLE_BINGO_BOARD_COUNT; i++)
    {
        if (gBattleBingoBoardRules[i].clearedFlag != 0 && FlagGet(gBattleBingoBoardRules[i].clearedFlag))
            count++;
    }

    return count;
}

u16 BattleBingoShouldAwardCipherCard(void)
{
    if (FlagGet(FLAG_RECEIVED_CIPHER_BINGO_CARD_ATTENDANT)
     || BattleBingoPlayerHasCardForBoard(BATTLE_BINGO_BOARD_CIPHER)
     || FlagGet(FLAG_CLEARED_CIPHER_BINGO_CARD))
        return FALSE;

    return BattleBingoCountClearedBoards() >= BATTLE_BINGO_BOARD_COUNT - 1;
}

u16 BattleBingoDidFirstClearCipherBoard(void)
{
    return sBattleBingoLastResult == BATTLE_BINGO_RESULT_COMPLETE
        && sBattleBingoLastCompletedBoardId == BATTLE_BINGO_BOARD_CIPHER
        && sBattleBingoLastCompleteWasFirstClear;
}

u16 BattleBingoGetSelectedBoardId(void)
{
    return sBattleBingoSelectedBoardId;
}

u16 BattleBingoPrepareCardSelection(void)
{
    u32 i;
    u16 count = 0;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCards); i++)
    {
        if (CheckBagHasItem(sBattleBingoCards[i].item, 1))
        {
            u8 *nameBuffer = Alloc(100);
            struct ListMenuItem item;

            StringExpandPlaceholders(nameBuffer, sBattleBingoCards[i].name);
            item.name = nameBuffer;
            item.id = sBattleBingoCards[i].boardId;
            MultichoiceDynamic_PushElement(item);
            count++;
        }
    }

    return count;
}

void StartSelectedBattleBingoBoard(void)
{
    BattleBingoSetSelectedBoard();
    sBattleBingo.initialized = FALSE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    SetMainCallback2(CB2_WaitForSelectedBattleBingoBoardFadeOut);
}

void BattleBingoSetSelectedBoard(void)
{
    if (gSpecialVar_Result < BATTLE_BINGO_BOARD_COUNT
     && BattleBingoPlayerHasCardForBoard(gSpecialVar_Result))
        sBattleBingoSelectedBoardId = gSpecialVar_Result;
}

static bool8 BattleBingoPlayerHasCardForBoard(enum BattleBingoBoardId boardId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sBattleBingoCards); i++)
    {
        if (sBattleBingoCards[i].boardId == boardId)
            return CheckBagHasItem(sBattleBingoCards[i].item, 1);
    }

    return FALSE;
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
