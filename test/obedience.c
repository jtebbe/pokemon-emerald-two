#include "global.h"
#include "battle_util.h"
#include "constants/battle_move_resolution.h"
#include "test/test.h"

TEST("Obedience checks are disabled")
{
    EXPECT_EQ(GetAttackerObedienceForAction(), OBEYS);
}
