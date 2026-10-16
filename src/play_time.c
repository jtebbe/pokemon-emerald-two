#include "global.h"
#include "play_time.h"
#include "fake_rtc.h"
#include "field_player_avatar.h"

enum
{
    STOPPED,
    RUNNING,
    MAXED_OUT
};

static u8 sPlayTimeCounterState;

void PlayTimeCounter_Reset(void)
{
    sPlayTimeCounterState = STOPPED;

    gSaveData.playTimeHours = 0;
    gSaveData.playTimeMinutes = 0;
    gSaveData.playTimeSeconds = 0;
    gSaveData.playTimeVBlanks = 0;
}

void PlayTimeCounter_Start(void)
{
    sPlayTimeCounterState = RUNNING;

    if (gSaveData.playTimeHours > 999)
        PlayTimeCounter_SetToMax();
}

void PlayTimeCounter_Update(void)
{
    if (sPlayTimeCounterState != RUNNING)
        return;

    gSaveData.playTimeVBlanks++;
    UpdateSpinData();

    if (gSaveData.playTimeVBlanks < 60)
        return;

    gSaveData.playTimeVBlanks = 0;
    gSaveData.playTimeSeconds++;
    FakeRtc_TickTimeForward();

    if (gSaveData.playTimeSeconds < 60)
        return;

    gSaveData.playTimeSeconds = 0;
    gSaveData.playTimeMinutes++;

    if (gSaveData.playTimeMinutes < 60)
        return;

    gSaveData.playTimeMinutes = 0;
    gSaveData.playTimeHours++;

    if (gSaveData.playTimeHours > 999)
        PlayTimeCounter_SetToMax();
}

void PlayTimeCounter_SetToMax(void)
{
    sPlayTimeCounterState = MAXED_OUT;

    gSaveData.playTimeHours = 999;
    gSaveData.playTimeMinutes = 59;
    gSaveData.playTimeSeconds = 59;
    gSaveData.playTimeVBlanks = 59;
}
