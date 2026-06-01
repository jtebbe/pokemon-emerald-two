#ifndef GUARD_RANDOM_BATTLE_H
#define GUARD_RANDOM_BATTLE_H

#include "global.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/trainers.h"

struct ScriptContext;
struct Trainer;

struct RandomBattleOptions
{
    u8 numPlayerMons;
    u8 numOpponentMons;
    u16 trainerPic;
    const u8 *trainerName;
    bool8 isDouble;
    bool8 preEvos;
    bool8 noSpeciesClause;
    bool8 noItemClause;
    bool8 noTypeDiversity;
    bool8 noLegendaries;
    bool8 preselectedMoves;
    bool8 itemsOff;
    bool8 itemsPreselected;
    bool8 teraTypesOff;
    bool8 zCrystalsOff;
    bool8 megaStonesOff;
};

bool32 IsRandomBattleTrainer(u16 trainerId);
const struct Trainer *RandomBattle_GetTrainerStruct(void);
void RandomBattle_RestorePlayerParty(void);
bool32 RandomBattle_GenerateTrueBattle(const struct RandomBattleOptions *options);
bool32 RandomBattle_GeneratePoolBattle(const struct RandomBattleOptions *options);
bool32 RandomBattle_IsLegendarySpecies(enum Species species);
bool32 RandomBattle_IsFullyEvolvedSpecies(enum Species species);

void ScrCmd_truerandombattle(struct ScriptContext *ctx);
void ScrCmd_poolrandombattle(struct ScriptContext *ctx);

#endif // GUARD_RANDOM_BATTLE_H
