#include "graphics/gimmicks.h"

// Gimmick data

const struct GimmickInfo gGimmicksInfo[GIMMICKS_COUNT] =
{
    [GIMMICK_NONE] = {0},
    [GIMMICK_MEGA] =
    {
        .triggerSheet = &sSpriteSheet_MegaTrigger,
        .triggerPal = &sSpritePalette_MegaTrigger,
        .triggerTemplate = &sSpriteTemplate_GimmickTrigger,
        .indicatorData = sMegaIndicatorGfx,
        .indicatorPalTag = TAG_MEGA_INDICATOR_PAL,
        .CanActivate = CanMegaEvolve,
        .ActivateGimmick = ActivateMegaEvolution,
        .attackerSlideType = TRAINER_SLIDE_SELF_MEGA_EVOLUTION,
        .opponentSlideType = TRAINER_SLIDE_OPPONENT_MEGA_EVOLUTION,
    },
    [GIMMICK_TERA] =
    {
        .triggerSheet = &sSpriteSheet_TeraTrigger,
        .triggerPal = &sSpritePalette_TeraTrigger,
        .triggerTemplate = &sSpriteTemplate_GimmickTrigger,
        .indicatorData = NULL, // handled separately
        .indicatorPalTag = TAG_TERA_INDICATOR_PAL,
        .CanActivate = CanTerastallize,
        .ActivateGimmick = ActivateTera,
        .attackerSlideType = TRAINER_SLIDE_SELF_TERA,
        .opponentSlideType = TRAINER_SLIDE_OPPONENT_TERA,
    }
};
