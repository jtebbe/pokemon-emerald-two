#include "global.h"
#include "battle.h"
#include "battle_pike.h"
#include "battle_tower.h"
#include "battle_util.h"
#include "event_data.h"
#include "item_use.h"
#include "malloc.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/battle_frontier.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include "constants/vars.h"

static void SetPikeLayout(void)
{
    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS;
}

static void SetPyramidLayout(void)
{
    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR;
    gSaveBlock2Ptr->frontier.curChallengeBattleNum = 0;
    gSaveBlock2Ptr->frontier.pyramidRandoms[3] = 0;
}

static bool32 TryRunWithSlowGhostInCurrentLocation(void)
{
    struct BattleStruct *oldBattleStruct = gBattleStruct;
    u16 oldLayoutId = gMapHeader.mapLayoutId;
    struct BattleStruct *battleStruct = AllocZeroed(sizeof(*battleStruct));
    bool32 result;

    gBattleStruct = battleStruct;
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattlersCount = 2;
    gBattleTypeFlags = 0;
    gBattleMons[B_POSITION_PLAYER_LEFT].hp = 1;
    gBattleMons[B_POSITION_PLAYER_LEFT].speed = 0;
    gBattleMons[B_POSITION_PLAYER_LEFT].types[0] = TYPE_GHOST;
    gBattleMons[B_POSITION_PLAYER_LEFT].types[1] = TYPE_GHOST;
    gBattleMons[B_POSITION_PLAYER_LEFT].types[2] = TYPE_MYSTERY;
    gBattleMons[B_POSITION_OPPONENT_LEFT].hp = 1;
    gBattleMons[B_POSITION_OPPONENT_LEFT].speed = 255;

    result = TryRunFromBattle(B_POSITION_PLAYER_LEFT);

    Free(battleStruct);
    gBattleStruct = oldBattleStruct;
    gMapHeader.mapLayoutId = oldLayoutId;

    return result;
}

static bool32 CanEscapePreventedGhostEscapeInCurrentLocation(void)
{
    struct BattleStruct *oldBattleStruct = gBattleStruct;
    u16 oldLayoutId = gMapHeader.mapLayoutId;
    struct BattleStruct *battleStruct = AllocZeroed(sizeof(*battleStruct));
    bool32 result;

    gBattleStruct = battleStruct;
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattleMons[B_POSITION_PLAYER_LEFT].types[0] = TYPE_GHOST;
    gBattleMons[B_POSITION_PLAYER_LEFT].types[1] = TYPE_GHOST;
    gBattleMons[B_POSITION_PLAYER_LEFT].types[2] = TYPE_MYSTERY;
    gBattleMons[B_POSITION_PLAYER_LEFT].volatiles.escapePrevention = TRUE;

    result = CanBattlerEscape(B_POSITION_PLAYER_LEFT);

    Free(battleStruct);
    gBattleStruct = oldBattleStruct;
    gMapHeader.mapLayoutId = oldLayoutId;

    return result;
}

TEST("Infinite Repel cannot be used in the Battle Pike")
{
    u16 oldLayoutId = gMapHeader.mapLayoutId;
    bool8 canUse;

    SetPikeLayout();
    canUse = CanUseInfiniteRepel();
    gMapHeader.mapLayoutId = oldLayoutId;

    EXPECT(!canUse);
}

TEST("Ghost types do not always flee in the Battle Pike")
{
    SetPikeLayout();

    EXPECT(!TryRunWithSlowGhostInCurrentLocation());
}

TEST("Ghost types do not always flee in the Battle Pyramid")
{
    SetPyramidLayout();

    EXPECT(!TryRunWithSlowGhostInCurrentLocation());
}

TEST("Ghost types still always flee outside the Battle Pyramid and Battle Pike")
{
    gMapHeader.mapLayoutId = LAYOUT_LITTLEROOT_TOWN;

    EXPECT(TryRunWithSlowGhostInCurrentLocation());
}

TEST("Ghost types cannot ignore escape prevention in the Battle Pike or Battle Pyramid")
{
    SetPikeLayout();
    EXPECT(!CanEscapePreventedGhostEscapeInCurrentLocation());

    SetPyramidLayout();
    EXPECT(!CanEscapePreventedGhostEscapeInCurrentLocation());

    gMapHeader.mapLayoutId = LAYOUT_LITTLEROOT_TOWN;
    EXPECT(CanEscapePreventedGhostEscapeInCurrentLocation());
}

TEST("CreateFacilityMon sets the real nature from the Frontier set")
{
    u32 i;
    u32 nature = NATURE_HARDY;
    struct Pokemon mon;
    struct TrainerMon fmon =
    {
        .species = SPECIES_WOBBUFFET,
        .moves = { MOVE_SPLASH, MOVE_NONE, MOVE_NONE, MOVE_NONE },
        .nature = NATURE_HARDY,
        .gender = TRAINER_MON_RANDOM_GENDER,
    };

    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE
        {
            nature = i;
        }
    }

    fmon.nature = nature;
    CreateFacilityMon(&fmon, 50, 0, 0, 0, &mon);

    EXPECT_EQ(GetNature(&mon), nature);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), nature);
}

TEST("Battle Arena bans trainer sets with switch-out moves")
{
    struct TrainerMon fmon =
    {
        .species = SPECIES_WOBBUFFET,
        .moves = { MOVE_U_TURN, MOVE_PARTING_SHOT, MOVE_BATON_PASS, MOVE_SHED_TAIL },
        .nature = NATURE_HARDY,
    };

    EXPECT(IsBattleArenaBannedTrainerMon(&fmon));

    fmon.moves[0] = MOVE_SCRATCH;
    fmon.moves[1] = MOVE_SPLASH;
    fmon.moves[2] = MOVE_CELEBRATE;
    fmon.moves[3] = MOVE_NONE;

    EXPECT(!IsBattleArenaBannedTrainerMon(&fmon));
}
