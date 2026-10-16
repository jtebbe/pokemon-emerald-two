#ifndef GUARD_TRAINER_CARD_H
#define GUARD_TRAINER_CARD_H

#include "constants/trainer_card.h"
#include "constants/species.h"

struct TrainerCard
{
    u8 gender;
    u8 stars;
    bool8 hasPokedex;
    bool8 caughtAllHoenn;
    u16 hofDebutHours;
    u16 hofDebutMinutes;
    u16 hofDebutSeconds;
    u16 caughtMonsCount;
    u16 trainerId;
    u16 playTimeHours;
    u16 playTimeMinutes;
    u32 money;
    u8 playerName[PLAYER_NAME_LENGTH + 1];
    u8 version;
    u8 monIconTint; // FRLG only
    u8 unionRoomClass;
    enum Species monSpecies[PARTY_SIZE]; // FRLG only
};

extern struct TrainerCard gTrainerCards[4];

u32 CountPlayerTrainerStars(void);
u8 GetTrainerCardStars(u8 cardId);
void CopyTrainerCardData(struct TrainerCard *dst, struct TrainerCard *src, u8 gameVersion);
void ShowPlayerTrainerCard(void (*callback)(void));
void ShowTrainerCardInLink(u8 cardId, void (*callback)(void));
void TrainerCard_GenerateCardForLinkPlayer(struct TrainerCard *trainerCard);

#endif // GUARD_TRAINER_CARD_H
