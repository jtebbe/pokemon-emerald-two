#include "global.h"
#include "battle.h"
#include "berry.h"
#include "item.h"
#include "test/test.h"

TEST("Every remaining berry maps to its item and back")
{
    for (u32 berry = BERRY_ID_CHERI; berry <= NUM_BERRIES; berry++)
    {
        enum Item item = BerryTypeToItemId(berry);
        EXPECT_NE(item, ITEM_NONE);
        EXPECT_EQ(ItemIdToBerryType(item), berry);
        EXPECT_EQ(GetItemPocket(item), POCKET_BERRIES);
        EXPECT(GetBerryInfo(berry)->growthDuration > 0);
    }
    EXPECT_EQ(BerryTypeToItemId(BERRY_ID_NONE), ITEM_NONE);
    EXPECT_EQ(ItemIdToBerryType(ITEM_POTION), BERRY_ID_NONE);
}

TEST("Invalid berry data falls back to the ordinary Cheri Berry")
{
    EXPECT_EQ(GetBerryInfo(BERRY_ID_NONE), GetBerryInfo(BERRY_ID_CHERI));
    EXPECT_EQ(GetBerryInfo(NUM_BERRIES + 1), GetBerryInfo(BERRY_ID_CHERI));
}

TEST("Link battle header carries only version and party health")
{
    EXPECT_EQ(sizeof(struct LinkBattlerHeader), 4);
    EXPECT_EQ(offsetof(struct LinkBattlerHeader, versionSignatureLo), 0);
    EXPECT_EQ(offsetof(struct LinkBattlerHeader, versionSignatureHi), 1);
    EXPECT_EQ(offsetof(struct LinkBattlerHeader, vsScreenHealthFlagsLo), 2);
    EXPECT_EQ(offsetof(struct LinkBattlerHeader, vsScreenHealthFlagsHi), 3);
}
