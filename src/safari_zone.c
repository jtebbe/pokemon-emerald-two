#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "field_player_avatar.h"
#include "overworld.h"
#include "main.h"
#include "safari_zone.h"
#include "script.h"
#include "string_util.h"
#include "field_screen_effect.h"




EWRAM_DATA u8 gNumSafariBalls = 0;
EWRAM_DATA u16 gSafariZoneStepCounter = 0;
EWRAM_DATA static u8 sSafariZoneCaughtMons = 0;


bool32 GetSafariZoneFlag(void)
{
    return FlagGet(FLAG_SYS_SAFARI_MODE);
}

void SetSafariZoneFlag(void)
{
    FlagSet(FLAG_SYS_SAFARI_MODE);
}

void ResetSafariZoneFlag(void)
{
    FlagClear(FLAG_SYS_SAFARI_MODE);
}

void EnterSafariMode(void)
{
    SetSafariZoneFlag();
    gNumSafariBalls = 30;
    if (IS_FRLG)
        gSafariZoneStepCounter = 600;
    else
        gSafariZoneStepCounter = 500;
    sSafariZoneCaughtMons = 0;
}

void ExitSafariMode(void)
{
    ResetSafariZoneFlag();
    gNumSafariBalls = 0;
    gSafariZoneStepCounter = 0;
}




void CB2_EndSafariBattle(void)
{
    if (gNumSafariBalls == 0)
        ExitSafariMode();
    SetMainCallback2(CB2_ReturnToField);
}
