#include "global.h"
#include "load_save.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"

TEST("All fusion partner species round-trip with their complete Pokemon data")
{
    const enum Species partners[] = {
        SPECIES_RESHIRAM, SPECIES_ZEKROM,
        SPECIES_SOLGALEO, SPECIES_LUNALA,
        SPECIES_GLASTRIER, SPECIES_SPECTRIER,
    };
    ResetSaveData();
    for (u32 i = 0; i < ARRAY_COUNT(partners); i++)
    {
        struct Pokemon original, restored;
        u32 slot = i % MAX_FUSION_STORAGE;
        u32 value;
        enum Item item = ITEM_LEFTOVERS;
        enum Move move = MOVE_PSYCHIC;
        CreateMon(&original, partners[i], 57, 0x12345678, OTID_STRUCT_PRESET(0x87654321));
        value = 100;
        SetMonData(&original, MON_DATA_ATK_EV, &value);
        CalculateMonStats(&original);
        value = original.maxHP - 9;
        SetMonData(&original, MON_DATA_HP, &value);
        value = STATUS1_PARALYSIS;
        SetMonData(&original, MON_DATA_STATUS, &value);
        SetMonData(&original, MON_DATA_HELD_ITEM, &item);
        SetMonData(&original, MON_DATA_MOVE1, &move);
        SetMonData(&original, MON_DATA_NICKNAME, COMPOUND_STRING("PARTNER"));
        original.mail = 2;

        // These are the same storage/reconstruction operations used by the item menu.
        SaveMonToBox(&gSaveData.fusions[slot], &original);
        gSaveData.fusionMail[slot] = original.mail;
        BoxMonToMon(&gSaveData.fusions[slot], &restored);
        restored.mail = gSaveData.fusionMail[slot];

        EXPECT_EQ(memcmp(&original.box, &restored.box, sizeof(original.box)), 0);
        EXPECT_EQ(restored.hp, original.hp);
        EXPECT_EQ(restored.status, original.status);
        EXPECT_EQ(restored.mail, original.mail);
        EXPECT_EQ(restored.level, original.level);
        EXPECT_EQ(restored.maxHP, original.maxHP);
        EXPECT_EQ(restored.attack, original.attack);
        EXPECT_EQ(restored.defense, original.defense);
        EXPECT_EQ(restored.speed, original.speed);
        EXPECT_EQ(restored.spAttack, original.spAttack);
        EXPECT_EQ(restored.spDefense, original.spDefense);
    }
}
