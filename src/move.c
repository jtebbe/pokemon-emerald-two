#include "global.h"
#include "battle.h"
#include "main.h"
#include "move.h"

#include "data/moves_info.h"

bool32 IsHornMove(enum Move move)
{
    switch (move)
    {
    case MOVE_HORN_ATTACK:
    case MOVE_FURY_ATTACK:
    case MOVE_HORN_DRILL:
    case MOVE_MEGAHORN:
    case MOVE_HORN_LEECH:
    case MOVE_SECRET_SWORD:
    case MOVE_SMART_STRIKE:
    case MOVE_SEA_HORN:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 IsKickingMove(enum Move move)
{
    switch (move)
    {
    case MOVE_BLAZE_KICK:
    case MOVE_JUMP_KICK:
    case MOVE_HIGH_JUMP_KICK:
    case MOVE_TRIPLE_AXEL:
    case MOVE_TRIPLE_KICK:
    case MOVE_STOMP:
    case MOVE_MEGA_KICK:
    case MOVE_LOW_KICK:
    case MOVE_ROLLING_KICK:
    case MOVE_TROP_KICK:
    case MOVE_PYRO_BALL:
    case MOVE_HIGH_HORSEPOWER:
    case MOVE_DOUBLE_KICK:
    case MOVE_AQUA_STEP:
        return TRUE;
    default:
        return FALSE;
    }
}
