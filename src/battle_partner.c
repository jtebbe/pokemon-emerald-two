#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_partner.h"
#include "data.h"
#include "difficulty.h"
#include "malloc.h"
#include "string_util.h"
#include "trainer_util.h"
#include "text.h"

#include "constants/abilities.h"
#include "constants/battle_ai.h"

#if !TESTING
const struct Trainer gBattlePartners[DIFFICULTY_COUNT][PARTNER_COUNT] =
{
#include "data/battle_partners.h"
};
#endif

#define STEVEN_OTID 61226

void FillPartnerParty(u16 trainerId)
{
    ZeroPartyMons(gParties[B_TRAINER_PARTNER]);

    const struct Trainer *partner = GetTrainerStructFromId(trainerId);
    struct TrainerGenerator partnerGen;
    MakePartnerGenerator(&partnerGen, partner);
    if (trainerId == TRAINER_PARTNER(PARTNER_STEVEN))
        partnerGen.otID = OTID_STRUCT_PRESET(STEVEN_OTID);

    s32 lastIndex = AreMultiPartiesFullTeams() ? PARTY_SIZE : MULTI_PARTY_SIZE;
    for (s32 i = 0; i < lastIndex && i < partner->partySize; i++)
        GenerateMonFromTrainerMon(&gParties[B_TRAINER_PARTNER][i], &partner->party[i], &partnerGen);
}
