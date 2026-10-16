#include "global.h"
#include "main.h"
#include "battle_special.h"
#include "battle.h"
#include "event_data.h"
#include "overworld.h"

void CB2_EndSpecialTrainerBattle(void)
{
    if (gBattleScripting.specialTrainerBattleType == SPECIAL_BATTLE_MULTI)
    {
        if (!AreMultiPartiesFullTeams())
        {
            for (u32 i = 0; i < MULTI_PARTY_SIZE; i++)
            {
                if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES))
                    SavePlayerPartyMon(i, &gParties[B_TRAINER_PLAYER][i]);
            }
        }
        gSpecialVar_Result = gBattleOutcome;
    }

    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
