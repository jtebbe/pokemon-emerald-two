#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include <string.h>
#include <limits.h>
#include "config/general.h" // we need to define config before gba headers as print stuff needs the functions nulled before defines.
#include "gba/gba.h"
#include "assertf.h"
#include "gametypes.h"
#include "siirtc.h"
#include "fpmath.h"
#include "metaprogram.h"
#include "constants/global.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/species.h"
#include "constants/pokedex.h"
#include "constants/apricorn_tree.h"
#include "constants/berry.h"
#include "constants/maps.h"
#include "constants/mass_outbreak.h"
#include "constants/pokemon.h"
#include "constants/easy_chat.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "config/save.h"

// Prevent cross-jump optimization.
#define BLOCK_CROSS_JUMP asm("");

// to help in decompiling
#define asm_unified(x) asm(".syntax unified\n" x "\n.syntax divided")
#define NAKED __attribute__((naked))

#if MODERN
#define asm __asm__
#endif

/// IDE support
#if defined(__APPLE__) || defined(__CYGWIN__) || defined(__INTELLISENSE__)
// We define these when using certain IDEs to fool preproc
#define _(x)        {x}
#define __(x)       {x}
#define COMPOUND_STRING(x) 0
#define INCBIN(...) {0}
#define INCBIN_U8   INCBIN
#define INCBIN_U16  INCBIN
#define INCBIN_U32  INCBIN
#define INCBIN_COMP INCBIN
#define INCGFX(...) {0}
#define INCGFX_U8   INCGFX
#define INCGFX_U16  INCGFX
#define INCGFX_U32  INCGFX
#define INCGFX_COMP INCGFX
#endif // IDE support

#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))

// GameFreak used a macro called "NELEMS", as evidenced by
// AgbAssert calls.
#define NELEMS(arr) (sizeof(arr)/sizeof(*(arr)))

#define SWAP(a, b, temp)    \
{                           \
    temp = a;               \
    a = b;                  \
    b = temp;               \
}

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))

#if MODERN
#define abs(x) (((x) < 0) ? -(x) : (x))
#endif

// Used in cases where division by 0 can occur in the retail version.
// Avoids invalid opcodes on some emulators, and the otherwise UB.
#ifdef UBFIX
#define SAFE_DIV(a, b) (((b) != 0) ? (a) / (b) : 0)
#else
#define SAFE_DIV(a, b) ((a) / (b))
#endif

#define IS_POW_OF_TWO(n) (((n) & ((n)-1)) == 0)

// The below macro does a%n, but (to match) will switch to a&(n-1) if n is a power of 2.
// There are cases where GF does a&(n-1) where we would really like to have a%n, because
// if n is changed to a value that isn't a power of 2 then a&(n-1) is unlikely to work as
// intended, and a%n for powers of 2 isn't always optimized to use &.
#define MOD(a, n) (((n) & ((n)-1)) ? ((a) % (n)) : ((a) & ((n)-1)))

// Increments 'a' by 1, wrapping back to 0 when it reaches 'n'. If 'n' is a power of two,
// the wrap is implemented using a bit mask: (a + 1) & (n - 1), which is slightly faster.
// This is intended to be used when 'n' is known at compile time.
#define INCREMENT_OR_WRAP(a, n) ((IS_POW_OF_TWO(n)) ? (((a) + 1) & ((n) - 1)) : (((a) + 1) >= (n) ? 0 : ((a) + 1)))

// Extracts the upper 16 bits of a 32-bit number
#define HIHALF(n) (((n) & 0xFFFF0000) >> 16)

// Extracts the lower 16 bits of a 32-bit number
#define LOHALF(n) ((n) & 0xFFFF)

// There are many quirks in the source code which have overarching behavioral differences from
// a number of other files. For example, diploma.c seems to declare rodata before each use while
// other files declare out of order and must be at the beginning. There are also a number of
// macros which differ from one file to the next due to the method of obtaining the result, such
// as these below. Because of this, there is a theory (Two Team Theory) that states that these
// programming projects had more than 1 "programming team" which utilized different macros for
// each of the files that were worked on.
#define T1_READ_8(ptr)  ((ptr)[0])
#define T1_READ_16(ptr) ((ptr)[0] | ((ptr)[1] << 8))
#define T1_READ_32(ptr) ((ptr)[0] | ((ptr)[1] << 8) | ((ptr)[2] << 16) | ((ptr)[3] << 24))
#define T1_READ_PTR(ptr) (u8 *) T1_READ_32(ptr)

// T2_READ_8 is a duplicate to remain consistent with each group.
#define T2_READ_8(ptr)  ((ptr)[0])
#define T2_READ_16(ptr) ((ptr)[0] + ((ptr)[1] << 8))
#define T2_READ_32(ptr) ((ptr)[0] + ((ptr)[1] << 8) + ((ptr)[2] << 16) + ((ptr)[3] << 24))
#define T2_READ_PTR(ptr) (void *) T2_READ_32(ptr)

#define PACK(data, shift, mask)   ( ((data) << (shift)) & (mask) )
#define UNPACK(data, shift, mask) ( ((data) & (mask)) >> (shift) )

// Macros for checking the joypad
#define TEST_BUTTON(field, button) ((field) & (button))
#define JOY_NEW(button) TEST_BUTTON(gMain.newKeys,  button)
#define JOY_HELD(button)  TEST_BUTTON(gMain.heldKeys, button)
#define JOY_HELD_RAW(button) TEST_BUTTON(gMain.heldKeysRaw, button)
#define JOY_REPEAT(button) TEST_BUTTON(gMain.newAndRepeatedKeys, button)

#define S16TOPOSFLOAT(val)   \
({                           \
    s16 v = (val);           \
    float f = (float)v;      \
    if (v < 0) f += 65536.0f;\
    f;                       \
})

#define DIV_ROUND_UP(val, roundBy) (((val) / (roundBy)) + (((val) % (roundBy)) ? 1 : 0))

#define ROUND_BITS_TO_BYTES(numBits) DIV_ROUND_UP(numBits, 8)

#define NUM_DEX_FLAG_BYTES ROUND_BITS_TO_BYTES(POKEMON_SLOTS_NUMBER)
#define NUM_FLAG_BYTES ROUND_BITS_TO_BYTES(FLAGS_COUNT)
#define NUM_APRICORN_TREE_BYTES ROUND_BITS_TO_BYTES(APRICORN_TREE_COUNT)

// This produces an error at compile-time if expr is zero.
// It looks like file.c:line: size of array `id' is negative
#define STATIC_ASSERT(expr, id) typedef char id[(expr) ? 1 : -1];

#define FEATURE_FLAG_ASSERT(flag, id) STATIC_ASSERT(flag > TEMP_FLAGS_END || flag == 0, id)

#define READ_OTID_FROM_SAVE T1_READ_32(gSaveData.playerTrainerId)

// NOTE: This uses hardware timers 2 and 3; this will not work during active link connections or with the eReader
static inline void CycleCountStart()
{
    REG_TM2CNT_H = 0;
    REG_TM3CNT_H = 0;

    REG_TM2CNT_L = 0;
    REG_TM3CNT_L = 0;

    // init timers (tim3 count up mode, tim2 every clock cycle)
    REG_TM3CNT_H = TIMER_ENABLE | TIMER_COUNTUP;
    REG_TM2CNT_H = TIMER_1CLK | TIMER_ENABLE;
}

static inline u32 CycleCountEnd()
{
    // stop timers
    REG_TM2CNT_H = 0;
    REG_TM3CNT_H = 0;

    // return result
    return REG_TM2CNT_L | (REG_TM3CNT_L << 16u);
}

struct Coords8
{
    s8 x;
    s8 y;
};

struct UCoords8
{
    u8 x;
    u8 y;
};

struct Coords16
{
    s16 x;
    s16 y;
};

struct UCoords16
{
    u16 x;
    u16 y;
};

struct Coords32
{
    s32 x;
    s32 y;
};

struct UCoords32
{
    u32 x;
    u32 y;
};

struct Time
{
    s16 days;
    s8 hours;
    s8 minutes;
    s8 seconds;
};

struct NPCFollowerPadding
{
    u8 padding1;
    u8 padding2;
    u8 padding3;
};

struct NPCFollower
{
    u8 inProgress:1;
    u8 warpEnd:1;
    u8 createSurfBlob:2;
    u8 comeOutDoorStairs:2;
    u8 forcedMovement:2;
    u8 objId;
    u8 currentSprite;
    u8 delayedState;
    struct NPCFollowerPadding padding;
    struct Coords16 log;
    const u8 *script;
    u16 flag;
    u16 graphicsId;
    u16 flags;
    u8 battlePartner; // If you have more than 255 total battle partners defined, change this to a u16
};

#include "constants/items.h"
#define ITEM_FLAGS_COUNT ((ITEMS_COUNT / 8) + ((ITEMS_COUNT % 8) ? 1 : 0))



struct Pokedex
{
    u8 order;
    u8 mode;
    u8 nationalMagic; // must equal 0xDA in order to have National mode
    u32 unownPersonality; // set when you first see Unown
    u32 spindaPersonality; // set when you first see Spinda
};



extern u8 UpdateSpritePaletteWithTime(u8);

#include "global.fieldmap.h"
#include "global.berry.h"
#include "pokemon.h"

struct WarpData
{
    s8 mapGroup;
    s8 mapNum;
    s8 warpId;
    //u8 padding;
    s16 x, y;
};

struct ItemSlot
{
    enum Item itemId;
    u16 quantity;
};

struct Roamer
{
    u32 personality;
    enum Species species;
    u16 hp;
    u8 level;
    u8 statusA;
    bool8 active;
    u8 statusB; // Stores frostbite
    bool8 shiny;
};

struct Mail
{
    u16 words[MAIL_WORDS_COUNT];
    u8 playerName[PLAYER_NAME_LENGTH + 1];
    u8 trainerId[TRAINER_ID_LENGTH];
    enum Species species;
    enum Item itemId;
};

struct DaycareMail
{
    struct Mail message;
    u8 otName[PLAYER_NAME_LENGTH + 1];
    u8 monName[VANILLA_POKEMON_NAME_LENGTH + 1];
    u8 gameLanguage:4;
    u8 monLanguage:4;
};

struct DaycareMon
{
    struct BoxPokemon mon;
    struct DaycareMail mail;
    u32 steps;
};

struct DayCare
{
    struct DaycareMon mons[DAYCARE_MON_COUNT];
    u32 offspringPersonality;
    u32 stepCounter;
};

struct Bag
{
    struct ItemSlot items[BAG_ITEMS_COUNT];
    struct ItemSlot keyItems[BAG_KEYITEMS_COUNT];
    struct ItemSlot pokeBalls[BAG_POKEBALLS_COUNT];
    struct ItemSlot TMsHMs[BAG_TMHM_COUNT];
    struct ItemSlot berries[BAG_BERRIES_COUNT];
};



#include "pokemon_storage_system.h"
#include "trade_journal.h"

struct SaveData
{
    u8 playerName[PLAYER_NAME_LENGTH + 1];
    u8 playerGender; // MALE, FEMALE
    u8 specialSaveWarpFlags;
    u8 playerTrainerId[TRAINER_ID_LENGTH];
    u16 playTimeHours;
    u8 playTimeMinutes;
    u8 playTimeSeconds;
    u8 playTimeVBlanks;
    u8 optionsButtonMode;  // OPTIONS_BUTTON_MODE_[NORMAL/LR/L_EQUALS_A]
    u16 optionsTextSpeed:3; // OPTIONS_TEXT_SPEED_[SLOW/MID/FAST]
    u16 optionsWindowFrameType:5; // Specifies one of the 20 decorative borders for text boxes
    u16 optionsSound:1; // OPTIONS_SOUND_[MONO/STEREO]
    u16 optionsBattleStyle:1; // OPTIONS_BATTLE_STYLE_[SHIFT/SET]
    u16 optionsBattleSceneOff:1; // whether battle animations are disabled
    struct Pokedex pokedex;
    struct Time localTimeOffset;
    struct Time lastBerryTreeUpdate;

    struct Coords16 pos;
    struct WarpData location;
    struct WarpData continueGameWarp;
    struct WarpData dynamicWarp;
    struct WarpData lastHealLocation; // used by white-out and teleport
    struct WarpData escapeWarp; // used by Dig and Escape Rope
    u16 savedMusic;
    u8 weather;
    u8 weatherCycleStage;
    u8 flashLevel;
    u16 mapLayoutId;
    u16 mapView[0x100];
    u8 playerPartyCount;
    struct BoxPokemon playerParty[PARTY_SIZE];
    u8 playerPartyMail[PARTY_SIZE];
    // Trade updates are confined to the first logical flash sector.
    struct Mail mail[MAIL_COUNT];
    u8 dexSeen[NUM_DEX_FLAG_BYTES];
    u8 dexCaught[NUM_DEX_FLAG_BYTES];
    struct TradeJournalMetadata linkTrade;
    u32 money;
    u16 coins;
    u16 registeredItem; // registered for use with SELECT button
    struct ItemSlot pcItems[PC_ITEMS_COUNT];
    struct Bag bag;
    u32 dailySeed;
    struct ObjectEvent objectEvents[OBJECT_EVENTS_COUNT];
    struct ObjectEventTemplate objectEventTemplates[OBJECT_EVENT_TEMPLATES_COUNT];
    u8 flags[NUM_FLAG_BYTES];
    u16 vars[VARS_COUNT];
    struct BerryTree berryTrees[BERRY_TREES_COUNT];
    struct DayCare daycare;
    struct Roamer roamer[ROAMER_COUNT];
#if IS_FRLG
    u8 rivalName[PLAYER_NAME_LENGTH + 1];
    struct DaycareMon route5DayCareMon;
#endif

#if OW_USE_FAKE_RTC
    struct SiiRtcInfo fakeRTC;
#endif
#if FNPC_ENABLE_NPC_FOLLOWERS
    struct NPCFollower NPCfollower;
#endif
#if OW_SHOW_ITEM_DESCRIPTIONS == OW_ITEM_DESCRIPTIONS_FIRST_TIME
    u8 itemFlags[ITEM_FLAGS_COUNT];
#endif
#if USE_DEXNAV_SEARCH_LEVELS == TRUE
    u8 dexNavSearchLevels[NUM_SPECIES];
#endif
    u8 dexNavChain;
#if APRICORN_TREE_COUNT > 0
    u8 apricornTrees[NUM_APRICORN_TREE_BYTES];
#endif
    u16 hallOfFameEntries;
    u32 firstHallOfFameTime;
    u8 currentBox;
    u8 boxNames[TOTAL_BOXES_COUNT][BOX_NAME_LENGTH + 1];
    u8 boxWallpapers[TOTAL_BOXES_COUNT];
    struct BoxPokemon fusions[MAX_FUSION_STORAGE];
    u8 fusionMail[MAX_FUSION_STORAGE];
};

extern struct SaveData gSaveData;

struct MapPosition
{
    s16 x;
    s16 y;
    s8 elevation;
};

#if TESTING
extern bool32 gLoadFail;
extern bool32 gCountAllocs;
extern s32 gSpriteAllocs;
#endif // TESTING

#endif // GUARD_GLOBAL_H
