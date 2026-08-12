#ifndef GUARD_TRAINER_POOLS_H
#define GUARD_TRAINER_POOLS_H

#include "pokemon.h"
#include "data.h"
#include "global.h"

#define POOL_SLOT_DISABLED  0xff

//  Unlimited is set to 0 so that the default is unlimited
#define POOL_MEMBER_COUNT_UNLIMITED 0
#define POOL_MEMBER_COUNT_NONE      0xff

enum PoolRulesets {
    POOL_RULESET_BASIC,
    POOL_RULESET_DOUBLES,
    POOL_RULESET_WEATHER_SINGLES,
    POOL_RULESET_WEATHER_DOUBLES,
    POOL_RULESET_SUPPORT_DOUBLES,
};

enum PoolPickFunctions {
    POOL_PICK_DEFAULT,
    POOL_PICK_LOWEST,
};

enum PoolPruneOptions {
    POOL_PRUNE_NONE,
    POOL_PRUNE_TEST,
    POOL_PRUNE_RANDOM_TAG,
};

enum PoolTags {
    //  Lead and Ace has special handling, leave them be
    POOL_TAG_LEAD = 0,
    POOL_TAG_ACE = 1,
    //  No special handling for these
    POOL_TAG_WEATHER_SETTER = 2,
    POOL_TAG_WEATHER_ABUSER = 3,
    POOL_TAG_SUPPORT = 4,
    POOL_TAG_TERA = 5,
    POOL_TAG_TAG6 = POOL_TAG_TERA,
    POOL_TAG_TAG7 = 6,
    POOL_TAG_TAG8 = 7,
    POOL_TAG_NORMAL = 8,
    POOL_TAG_FIGHTING = 9,
    POOL_TAG_FLYING = 10,
    POOL_TAG_POISON = 11,
    POOL_TAG_GROUND = 12,
    POOL_TAG_ROCK = 13,
    POOL_TAG_BUG = 14,
    POOL_TAG_GHOST = 15,
    POOL_TAG_STEEL = 16,
    POOL_TAG_FIRE = 17,
    POOL_TAG_WATER = 18,
    POOL_TAG_GRASS = 19,
    POOL_TAG_ELECTRIC = 20,
    POOL_TAG_PSYCHIC = 21,
    POOL_TAG_ICE = 22,
    POOL_TAG_DRAGON = 23,
    POOL_TAG_DARK = 24,
    POOL_TAG_FAIRY = 25,
    POOL_TAG_LEGENDARY = 26,
    POOL_TAG_SUBLEGENDARY = 27,
    POOL_TAG_MYTHICAL = 28,
    POOL_TAG_PARADOX = 29,
    POOL_TAG_ULTRA_BEAST = 30,
    POOL_TAG_ULTRABEAST = POOL_TAG_ULTRA_BEAST,
    POOL_TAG_SAND = 31,
    POOL_TAG_RAIN = 32,
    POOL_TAG_SNOW = 33,
    POOL_TAG_SUN = 34,
    //  Must be the last element
    POOL_NUM_TAGS = 35
};

#define MON_POOL_TAG_LEAD           (1ULL << POOL_TAG_LEAD)
#define MON_POOL_TAG_ACE            (1ULL << POOL_TAG_ACE)
#define MON_POOL_TAG_WEATHER_SETTER (1ULL << POOL_TAG_WEATHER_SETTER)
#define MON_POOL_TAG_WEATHER_ABUSER (1ULL << POOL_TAG_WEATHER_ABUSER)
#define MON_POOL_TAG_SUPPORT        (1ULL << POOL_TAG_SUPPORT)
#define MON_POOL_TAG_TERA           (1ULL << POOL_TAG_TERA)
#define MON_POOL_TAG_TAG6           (1ULL << POOL_TAG_TAG6)
#define MON_POOL_TAG_TAG7           (1ULL << POOL_TAG_TAG7)
#define MON_POOL_TAG_TAG8           (1ULL << POOL_TAG_TAG8)
#define MON_POOL_TAG_NORMAL         (1ULL << POOL_TAG_NORMAL)
#define MON_POOL_TAG_FIGHTING       (1ULL << POOL_TAG_FIGHTING)
#define MON_POOL_TAG_FLYING         (1ULL << POOL_TAG_FLYING)
#define MON_POOL_TAG_POISON         (1ULL << POOL_TAG_POISON)
#define MON_POOL_TAG_GROUND         (1ULL << POOL_TAG_GROUND)
#define MON_POOL_TAG_ROCK           (1ULL << POOL_TAG_ROCK)
#define MON_POOL_TAG_BUG            (1ULL << POOL_TAG_BUG)
#define MON_POOL_TAG_GHOST          (1ULL << POOL_TAG_GHOST)
#define MON_POOL_TAG_STEEL          (1ULL << POOL_TAG_STEEL)
#define MON_POOL_TAG_FIRE           (1ULL << POOL_TAG_FIRE)
#define MON_POOL_TAG_WATER          (1ULL << POOL_TAG_WATER)
#define MON_POOL_TAG_GRASS          (1ULL << POOL_TAG_GRASS)
#define MON_POOL_TAG_ELECTRIC       (1ULL << POOL_TAG_ELECTRIC)
#define MON_POOL_TAG_PSYCHIC        (1ULL << POOL_TAG_PSYCHIC)
#define MON_POOL_TAG_ICE            (1ULL << POOL_TAG_ICE)
#define MON_POOL_TAG_DRAGON         (1ULL << POOL_TAG_DRAGON)
#define MON_POOL_TAG_DARK           (1ULL << POOL_TAG_DARK)
#define MON_POOL_TAG_FAIRY          (1ULL << POOL_TAG_FAIRY)
#define MON_POOL_TAG_LEGENDARY      (1ULL << POOL_TAG_LEGENDARY)
#define MON_POOL_TAG_SUBLEGENDARY   (1ULL << POOL_TAG_SUBLEGENDARY)
#define MON_POOL_TAG_MYTHICAL       (1ULL << POOL_TAG_MYTHICAL)
#define MON_POOL_TAG_PARADOX        (1ULL << POOL_TAG_PARADOX)
#define MON_POOL_TAG_ULTRA_BEAST    (1ULL << POOL_TAG_ULTRA_BEAST)
#define MON_POOL_TAG_ULTRABEAST     (1ULL << POOL_TAG_ULTRABEAST)
#define MON_POOL_TAG_SAND           (1ULL << POOL_TAG_SAND)
#define MON_POOL_TAG_RAIN           (1ULL << POOL_TAG_RAIN)
#define MON_POOL_TAG_SNOW           (1ULL << POOL_TAG_SNOW)
#define MON_POOL_TAG_SUN            (1ULL << POOL_TAG_SUN)

struct PoolRules
{
    u8 speciesClause:1;
    u8 excludeForms:1;
    u8 itemClause:1;
    u8 itemClauseExclusions:1;
    u8 megaStoneClause:1;
    u8 zCrystalClause:1;
    u8 tagMaxMembers[POOL_NUM_TAGS];
    bool8 tagRequired[POOL_NUM_TAGS];
};

struct PickFunctions
{
    u32 (*LeadFunction)(const struct Trainer *, u8 *, u32, u32, u32, struct PoolRules *);
    u32 (*AceFunction)(const struct Trainer *, u8 *, u32, u32, u32, struct PoolRules *);
    u32 (*OtherFunction)(const struct Trainer *, u8 *, u32, u32, u32, struct PoolRules *);
};

void DoTrainerPartyPool(const struct Trainer *trainer, u32 *monIndices, u8 monsCount, u32 battleTypeFlags);

#endif
