#ifndef GUARD_DEBUG_PC_FILL_H
#define GUARD_DEBUG_PC_FILL_H

enum DebugPCFillResult
{
    DEBUG_PC_FILL_WORKING,
    DEBUG_PC_FILL_DONE,
    DEBUG_PC_FILL_ERROR,
};

// Zero-initialize before the first step. Progress counts only committed boxes.
struct DebugPCFill
{
    enum Species species;
    u8 nextBox;
    bool8 started;
    bool8 failed;
    bool8 finished;
};

// Performs initial save preparation, or fills and saves at most one cache window.
// Call on separate frames so the progress UI can redraw between transactions.
enum DebugPCFillResult DebugPCFillStep(struct DebugPCFill *fill);

#endif
