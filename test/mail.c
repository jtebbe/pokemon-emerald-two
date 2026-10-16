#include "global.h"
#include "item.h"
#include "constants/easy_chat.h"
#include "easy_chat.h"
#include "international_string_util.h"
#include "load_save.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "test/test.h"

TEST("Mail words remain available without interview or phrase-save data")
{
    u8 text[32];
    CopyEasyChatWord(text, EC_WORD_HELLO);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("HELLO")), 0);
    CopyEasyChatWord(text, EC_WORD_WORLD);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("WORLD")), 0);
    EXPECT(CopyEasyChatWord(text, EC_EMPTY_WORD) == text);
    EXPECT_EQ(text[0], EOS);
}

TEST("Unused Easy Chat groups reject words without shifting retained mail IDs")
{
    u8 text[32];
    EXPECT_EQ(EC_GROUP_POKEMON_NATIONAL, 21);
    EXPECT_EQ(EasyChat_GetNumWordsInGroup(20), 0);
    EXPECT_EQ(EasyChat_GetNumWordsInGroup(EC_NUM_GROUPS), 0);
    for (u32 index = 0; index <= EC_MASK_INDEX; index++)
    {
        CopyEasyChatWord(text, EC_WORD(20, index));
        EXPECT_EQ(StringCompare(text, COMPOUND_STRING("???")), 0);
    }
}

TEST("Mail words still render in rows with spaces and line breaks")
{
    const u16 words[] = {EC_WORD_HELLO, EC_WORD_WORLD, EC_WORD_HOW, EC_WORD_ARE};
    u8 text[64];
    u8 *end = ConvertEasyChatWordsToString(text, words, 2, 2);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("HELLO WORLD\nHOW ARE")), 0);
    EXPECT_EQ(*end, EOS);
}

TEST("An empty mail message initializes all nine word slots")
{
    u16 words[9] = {EC_WORD_HELLO};
    InitializeEasyChatWordArray(words, ARRAY_COUNT(words));
    for (u32 i = 0; i < ARRAY_COUNT(words); i++)
        EXPECT_EQ(words[i], EC_EMPTY_WORD);
}

TEST("International string copying works independently of TV shows")
{
    u8 text[32];
    CopyInternationalString(text, COMPOUND_STRING("DAY CARE"), LANGUAGE_ENGLISH);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("DAY CARE")), 0);
}

TEST("Ordinary box names and wallpapers initialize without Walda data")
{
    SetBagItemsPointers();
    ResetPokemonStorageSystem();
    EXPECT_EQ(gSaveData.currentBox, 0);
    EXPECT_EQ(StringCompare(GetBoxNamePtr(0), COMPOUND_STRING("BOX1")), 0);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        EXPECT_EQ(gSaveData.boxWallpapers[box], box % 4);
}
