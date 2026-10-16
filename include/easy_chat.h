#ifndef GUARD_EASYCHAT_H
#define GUARD_EASYCHAT_H

#include "main.h"

struct EasyChatScreenTemplate
{
    u8 numColumns;
    u8 numRows;
    u8 frameId;
    const u8 *titleText;
    const u8 *instructionsText1;
    const u8 *instructionsText2;
    const u8 *confirmText1;
    const u8 *confirmText2;
};

struct EasyChatScreen
{
    u8 templateId;
    u8 numColumns;
    u8 numRows;
    u8 inputState;
    s8 mainCursorColumn;
    s8 mainCursorRow;
    u8 maxWords;
    u8 inputStateBackup;
    bool8 inAlphabetMode;
    s8 keyboardColumn;
    s8 keyboardRow;
    u8 keyboardScrollOffset;
    u8 keyboardLastRow;
    u8 wordSelectScrollOffset;
    u8 wordSelectLastRow;
    s8 wordSelectColumn;
    s8 wordSelectRow;
    const u8 *titleText;
    u16 *savedPhrase;
    u16 currentPhrase[9];
};

struct EasyChatScreenControl
{
    u16 funcState;
    u16 windowId;
    u16 currentFuncId;
    u8 curWindowAnimState;
    u8 destWindowAnimState;
    s8 windowAnimStateDir;
    u8 modeWindowState;
    u8 phrasePrintBuffer[193];
    u8 wordSelectPrintBuffer[514];
    u16 scrollOffset;
    int scrollDest;
    int scrollSpeed;
    struct Sprite *mainCursorSprite;
    struct Sprite *rectangleCursorSpriteRight;
    struct Sprite *rectangleCursorSpriteLeft;
    struct Sprite *wordSelectCursorSprite;
    struct Sprite *buttonWindowSprite;
    struct Sprite *modeWindowSprite;
    struct Sprite *scrollIndicatorUpSprite;
    struct Sprite *scrollIndicatorDownSprite;
    struct Sprite *startButtonSprite;
    struct Sprite *selectButtonSprite;
    u16 bg1TilemapBuffer[BG_SCREEN_SIZE / 2];
    u16 bg3TilemapBuffer[BG_SCREEN_SIZE / 2];
};

struct EasyChatPhraseFrameDimensions
{
    u8 left:5;
    u8 top:3;
    u8 width;
    u8 height;
    u8 footerId;
};

struct EasyChatWordInfo
{
    const u8 *text;
    int alphabeticalOrder;
    int enabled;
};

typedef union
{
    const u16 *valueList;
    const struct EasyChatWordInfo *words;
} EasyChatGroupWordData;

struct EasyChatGroup
{
    const u8 *name;
    EasyChatGroupWordData wordData;
    u16 numWords;
    u16 numEnabledWords;
};

struct EasyChatScreenWordData
{
    u16 numUnlockedGroups;
    u16 unlockedGroupIds[EC_NUM_GROUPS];
    u16 numUnlockedAlphabetWords[EC_NUM_ALPHABET_GROUPS];
    u16 unlockedAlphabetWords[EC_NUM_ALPHABET_GROUPS][EC_MAX_WORDS_IN_GROUP];
    u8 unused[44];
    u16 selectedGroupWords[EC_MAX_WORDS_IN_GROUP];
    u16 numSelectedGroupWords;
}; /*size = 0x3BA4*/

struct EasyChatWordsByLetter
{
    const u16 *words;
    int numWords;
};

u8 *CopyEasyChatWord(u8 *dest, u16 easyChatWord);
void InitializeEasyChatWordArray(u16 *words, u16 length);
u8 *ConvertEasyChatWordsToString(u8 *dest, const u16 *src, u16 columns, u16 rows);
u16 EasyChat_GetNumWordsInGroup(u8 groupId);
void DoEasyChatMailScreen(u16 *words, MainCallback exitCallback);

#endif // GUARD_EASYCHAT_H
