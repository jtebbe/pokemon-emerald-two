#include "global.h"
#include "event_data.h"
#include "load_save.h"
#include "save.h"
#include "string_util.h"
#include "trainer_card.h"
#include "test/test.h"

TEST("Link trainer-card generation stays inside the compact card buffer")
{
    struct {
        u32 before;
        struct TrainerCard card;
        u32 after;
    } buffer;
    memset(&buffer, 0xA5, sizeof(buffer));
    ResetSaveData();
    StringCopy(gSaveData.playerName, COMPOUND_STRING("PLAYER"));
    gSaveData.money = 1234;
    gSaveData.playTimeHours = 18;
    gSaveData.playTimeMinutes = 34;
    gSaveData.playTimeSeconds = 56;
    RecordHallOfFameEntry();

    TrainerCard_GenerateCardForLinkPlayer(&buffer.card);
    EXPECT_EQ(buffer.before, 0xA5A5A5A5);
    EXPECT_EQ(buffer.after, 0xA5A5A5A5);
    EXPECT_EQ(StringCompare(buffer.card.playerName, COMPOUND_STRING("PLAYER")), 0);
    EXPECT_EQ(buffer.card.money, 1234);
    EXPECT_EQ(buffer.card.hofDebutHours, 18);
    EXPECT_EQ(buffer.card.hofDebutMinutes, 34);
    EXPECT_EQ(buffer.card.hofDebutSeconds, 56);
    EXPECT_EQ(buffer.card.version, GAME_VERSION);
}

TEST("Link trainer-card copies preserve live data without legacy fixed-size reads")
{
    struct TrainerCard source = {0};
    struct {
        u32 before;
        struct TrainerCard card;
        u32 after;
    } buffer;
    memset(&buffer, 0xA5, sizeof(buffer));
    source.money = 4321;
    source.hofDebutHours = 42;
    source.monSpecies[PARTY_SIZE - 1] = SPECIES_PIKACHU;
    StringCopy(source.playerName, COMPOUND_STRING("PARTNER"));
    CopyTrainerCardData(&buffer.card, &source, GAME_VERSION);
    EXPECT_EQ(buffer.before, 0xA5A5A5A5);
    EXPECT_EQ(buffer.after, 0xA5A5A5A5);
    EXPECT_EQ(buffer.card.money, 4321);
    EXPECT_EQ(buffer.card.hofDebutHours, 42);
    EXPECT_EQ(buffer.card.monSpecies[PARTY_SIZE - 1], SPECIES_PIKACHU);
    EXPECT_EQ(StringCompare(buffer.card.playerName, COMPOUND_STRING("PARTNER")), 0);
    EXPECT_EQ(buffer.card.version, GAME_VERSION);
}
