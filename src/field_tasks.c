#include "global.h"
#include "clock.h"
#include "field_tasks.h"
#include "main.h"
#include "overworld.h"
#include "script.h"
#include "task.h"

#define TIME_UPDATE_INTERVAL (1 << 12)

static void Task_RunTimeBasedEvents(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (ArePlayerFieldControlsLocked())
        return;

    if (data[0] == 0 && (gMain.vblankCounter1 & TIME_UPDATE_INTERVAL))
    {
        DoTimeBasedEvents();
        data[0] = 1;
    }
    else if (data[0] == 1 && !(gMain.vblankCounter1 & TIME_UPDATE_INTERVAL))
    {
        data[0] = 0;
    }

    UpdateAmbientCry(&data[1], (u16 *)&data[2]);
}

void SetUpFieldTasks(void)
{
    if (!FuncIsActiveTask(Task_RunTimeBasedEvents))
        CreateTask(Task_RunTimeBasedEvents, 80);
}

void ResetFieldTasksArgs(void)
{
    u8 taskId = FindTaskIdByFunc(Task_RunTimeBasedEvents);

    if (taskId != TASK_NONE)
    {
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = 0;
    }
}
