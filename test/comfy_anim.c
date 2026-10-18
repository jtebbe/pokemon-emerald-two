#include "comfy_anim.h"
#include "test/test.h"

static void SettleSpring(struct ComfyAnim *anim)
{
    for (u32 frame = 0; frame < 600; frame++)
    {
        TryAdvanceComfyAnim(anim);
        if (anim->completed)
            break;
    }
    EXPECT(anim->completed);
    EXPECT_EQ(anim->position, anim->config.data.spring.to);
    EXPECT_EQ(anim->velocity, 0);
}

TEST("Comfy Anim configs initialize defaults and clear prior settings")
{
    struct ComfyAnimEasingConfig easing;
    struct ComfyAnimSpringConfig spring;
    memset(&easing, 0xFF, sizeof(easing));
    memset(&spring, 0xFF, sizeof(spring));
    InitComfyAnimConfig_Easing(&easing);
    InitComfyAnimConfig_Spring(&spring);
    EXPECT_EQ(easing.from, 0);
    EXPECT_EQ(easing.to, 0);
    EXPECT_EQ(easing.durationFrames, 0);
    EXPECT_EQ(easing.delayFrames, 0);
    EXPECT_EQ(easing.easingFunc, ComfyAnimEasing_Linear);
    EXPECT_EQ(spring.from, 0);
    EXPECT_EQ(spring.to, 0);
    EXPECT_EQ(spring.delayFrames, 0);
    EXPECT_EQ(spring.clampAfter, 0);
    EXPECT_EQ(spring.mass, COMFY_ANIM_SPRING_DEFAULT_MASS);
    EXPECT_EQ(spring.tension, COMFY_ANIM_SPRING_DEFAULT_TENSION);
    EXPECT_EQ(spring.friction, COMFY_ANIM_SPRING_DEFAULT_FRICTION);
    EXPECT_EQ(sizeof(gComfyAnims), 448);
}

TEST("Comfy Anim easing interpolates in both directions and finishes exactly on time")
{
    bool32 reverse;
    u32 duration;
    PARAMETRIZE { reverse = FALSE; duration = 1; }
    PARAMETRIZE { reverse = TRUE; duration = 1; }
    PARAMETRIZE { reverse = FALSE; duration = 4; }
    PARAMETRIZE { reverse = TRUE; duration = 7; }
    struct ComfyAnimEasingConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Easing(&config);
    config.from = reverse ? Q_24_8(20) : -Q_24_8(20);
    config.to = -config.from;
    config.durationFrames = duration;
    InitComfyAnim_Easing(&config, &anim);
    EXPECT(anim.inUse);
    EXPECT(!anim.completed);
    EXPECT_EQ(anim.position, config.from);
    for (u32 frame = 1; frame <= duration; frame++)
    {
        TryAdvanceComfyAnim(&anim);
        if (frame < duration)
        {
            s32 progress = frame * Q_24_8(1) / duration;
            EXPECT_EQ(anim.position, config.from + progress * (config.to - config.from) / Q_24_8(1));
            EXPECT(!anim.completed);
        }
        else
        {
            EXPECT_EQ(anim.position, config.to);
            EXPECT_EQ(anim.velocity, 0);
            EXPECT(anim.completed);
        }
    }
    TryAdvanceComfyAnim(&anim);
    EXPECT_EQ(anim.position, config.to);
    EXPECT_EQ(anim.state.easingState.curFrame, duration);
}

TEST("Comfy Anim easing and springs honor their full start delay")
{
    bool32 spring;
    u32 delay;
    PARAMETRIZE { spring = FALSE; delay = 1; }
    PARAMETRIZE { spring = FALSE; delay = 3; }
    PARAMETRIZE { spring = TRUE; delay = 1; }
    PARAMETRIZE { spring = TRUE; delay = 3; }
    struct ComfyAnim anim;
    if (spring)
    {
        struct ComfyAnimSpringConfig config;
        InitComfyAnimConfig_Spring(&config);
        config.to = Q_24_8(40);
        config.delayFrames = delay;
        InitComfyAnim_Spring(&config, &anim);
    }
    else
    {
        struct ComfyAnimEasingConfig config;
        InitComfyAnimConfig_Easing(&config);
        config.to = Q_24_8(40);
        config.durationFrames = 4;
        config.delayFrames = delay;
        InitComfyAnim_Easing(&config, &anim);
    }
    for (u32 frame = 0; frame < delay; frame++)
    {
        TryAdvanceComfyAnim(&anim);
        EXPECT_EQ(anim.position, 0);
        EXPECT_EQ(anim.velocity, 0);
        EXPECT(!anim.completed);
        EXPECT_EQ(anim.delayFrames, delay - frame - 1);
    }
    TryAdvanceComfyAnim(&anim);
    EXPECT_GT(anim.position, 0);
}

TEST("Comfy Anim zero-duration easing snaps to its destination after its delay")
{
    u32 delay;
    PARAMETRIZE { delay = 0; }
    PARAMETRIZE { delay = 2; }
    struct ComfyAnimEasingConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Easing(&config);
    config.from = -Q_24_8(8);
    config.to = Q_24_8(15);
    config.delayFrames = delay;
    InitComfyAnim_Easing(&config, &anim);
    for (u32 frame = 0; frame < delay; frame++)
    {
        TryAdvanceComfyAnim(&anim);
        EXPECT_EQ(anim.position, config.from);
        EXPECT(!anim.completed);
    }
    TryAdvanceComfyAnim(&anim);
    EXPECT(anim.completed);
    EXPECT_EQ(anim.position, config.to);
    EXPECT_EQ(anim.velocity, 0);
}

TEST("Comfy Anim missing easing callback falls back to linear")
{
    struct ComfyAnimEasingConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Easing(&config);
    config.to = Q_24_8(16);
    config.durationFrames = 4;
    config.easingFunc = NULL;
    InitComfyAnim_Easing(&config, &anim);
    TryAdvanceComfyAnim(&anim);
    EXPECT_EQ(anim.position, Q_24_8(4));
}

TEST("Comfy Anim easing curves preserve endpoints and interpolate the midpoint")
{
    ComfyAnimEasingFunc func;
    PARAMETRIZE { func = ComfyAnimEasing_Linear; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseInQuad; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseOutQuad; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseInOutQuad; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseInCubic; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseOutCubic; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseInOutCubic; }
    PARAMETRIZE { func = ComfyAnimEasing_EaseInOutBack; }
    EXPECT_EQ(func(0), 0);
    EXPECT_EQ(func(Q_24_8(1)), Q_24_8(1));
    EXPECT_GT(func(Q_24_8(1) / 2), 0);
    EXPECT_LT(func(Q_24_8(1) / 2), Q_24_8(1));
    struct ComfyAnimEasingConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Easing(&config);
    config.from = Q_24_8(10);
    config.to = Q_24_8(110);
    config.durationFrames = 8;
    config.easingFunc = func;
    InitComfyAnim_Easing(&config, &anim);
    for (u32 frame = 0; frame < config.durationFrames; frame++)
        TryAdvanceComfyAnim(&anim);
    EXPECT(anim.completed);
    EXPECT_EQ(anim.position, config.to);
}

TEST("Comfy Anim springs settle, stay still when completed, and can be retargeted")
{
    bool32 reverse;
    PARAMETRIZE { reverse = FALSE; }
    PARAMETRIZE { reverse = TRUE; }
    struct ComfyAnimSpringConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Spring(&config);
    config.from = reverse ? Q_24_8(40) : -Q_24_8(40);
    config.to = -config.from;
    InitComfyAnim_Spring(&config, &anim);
    SettleSpring(&anim);
    for (u32 frame = 0; frame < 10; frame++)
    {
        TryAdvanceComfyAnim(&anim);
        EXPECT(anim.completed);
        EXPECT_EQ(anim.position, config.to);
        EXPECT_EQ(anim.velocity, 0);
    }
    anim.config.data.spring.to = config.from;
    TryAdvanceComfyAnim(&anim);
    EXPECT(!anim.completed);
    EXPECT_NE(anim.position, config.to);
    SettleSpring(&anim);
}

TEST("Comfy Anim stationary springs finish after their delay")
{
    struct ComfyAnimSpringConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Spring(&config);
    config.from = config.to = Q_24_8(8);
    config.delayFrames = 2;
    InitComfyAnim_Spring(&config, &anim);
    TryAdvanceComfyAnim(&anim);
    EXPECT(!anim.completed);
    TryAdvanceComfyAnim(&anim);
    EXPECT(!anim.completed);
    TryAdvanceComfyAnim(&anim);
    EXPECT(anim.completed);
    EXPECT_EQ(anim.position, config.to);
}

TEST("Comfy Anim spring clamp stops at the requested target crossing")
{
    bool32 reverse;
    PARAMETRIZE { reverse = FALSE; }
    PARAMETRIZE { reverse = TRUE; }
    struct ComfyAnimSpringConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Spring(&config);
    config.to = reverse ? -Q_24_8(100) : Q_24_8(100);
    config.clampAfter = 1;
    InitComfyAnim_Spring(&config, &anim);
    SettleSpring(&anim);
    EXPECT_EQ(anim.state.springState.overshootCount, 1);
    anim.config.data.spring.to = config.from;
    TryAdvanceComfyAnim(&anim);
    EXPECT(!anim.completed);
    EXPECT_EQ(anim.state.springState.overshootCount, 0);
    SettleSpring(&anim);
    EXPECT_EQ(anim.state.springState.overshootCount, 1);
}

TEST("Comfy Anim nonpositive spring mass uses the default")
{
    s32 mass;
    PARAMETRIZE { mass = 0; }
    PARAMETRIZE { mass = -Q_24_8(1); }
    struct ComfyAnimSpringConfig config;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Spring(&config);
    config.to = Q_24_8(40);
    config.mass = mass;
    InitComfyAnim_Spring(&config, &anim);
    EXPECT_EQ(anim.config.data.spring.mass, COMFY_ANIM_SPRING_DEFAULT_MASS);
    SettleSpring(&anim);
}

TEST("Comfy Anim smoothing rounds positive and negative positions to pixels")
{
    struct ComfyAnim anim = {0};
    anim.position = Q_24_8(3) + 0x7F;
    EXPECT_EQ(ReadComfyAnimValueSmooth(&anim), 3);
    anim.position++;
    EXPECT_EQ(ReadComfyAnimValueSmooth(&anim), 4);
    anim.position = -Q_24_8(3) - 0x7F;
    EXPECT_EQ(ReadComfyAnimValueSmooth(&anim), -3);
    anim.position -= 2;
    EXPECT_EQ(ReadComfyAnimValueSmooth(&anim), -4);
}

TEST("Comfy Anim pool exhaustion is safe and released slots can be reused")
{
    struct ComfyAnimEasingConfig easing;
    struct ComfyAnimSpringConfig spring;
    InitComfyAnimConfig_Easing(&easing);
    InitComfyAnimConfig_Spring(&spring);
    ReleaseComfyAnims();
    for (u32 i = 0; i < NUM_COMFY_ANIMS; i++)
        EXPECT_EQ(CreateComfyAnim_Easing(&easing), i);
    EXPECT_EQ(CreateComfyAnim_Easing(&easing), INVALID_COMFY_ANIM);
    EXPECT_EQ(CreateComfyAnim_Spring(&spring), INVALID_COMFY_ANIM);
    ReleaseComfyAnim(INVALID_COMFY_ANIM);
    ReleaseComfyAnim(0xFFFFFFFF);
    for (u32 i = 0; i < NUM_COMFY_ANIMS; i++)
        EXPECT(gComfyAnims[i].inUse);
    ReleaseComfyAnim(3);
    EXPECT(!gComfyAnims[3].inUse);
    EXPECT_EQ(CreateComfyAnim_Spring(&spring), 3);
    EXPECT_EQ(gComfyAnims[3].config.type, COMFY_ANIM_TYPE_SPRING);
    ReleaseComfyAnims();
    for (u32 i = 0; i < NUM_COMFY_ANIMS; i++)
        EXPECT(!gComfyAnims[i].inUse);
}

TEST("Comfy Anim global updates skip released slots and preserve finished slots until release")
{
    struct ComfyAnimEasingConfig config;
    InitComfyAnimConfig_Easing(&config);
    config.to = Q_24_8(16);
    config.durationFrames = 2;
    ReleaseComfyAnims();
    u32 active = CreateComfyAnim_Easing(&config);
    u32 released = CreateComfyAnim_Easing(&config);
    ReleaseComfyAnim(released);
    AdvanceComfyAnimations();
    EXPECT_EQ(gComfyAnims[active].position, Q_24_8(8));
    EXPECT_EQ(gComfyAnims[released].position, 0);
    AdvanceComfyAnimations();
    EXPECT(gComfyAnims[active].completed);
    EXPECT(gComfyAnims[active].inUse);
    EXPECT_EQ(gComfyAnims[active].position, config.to);
    AdvanceComfyAnimations();
    EXPECT_EQ(gComfyAnims[active].position, config.to);
    EXPECT_EQ(CreateComfyAnim_Easing(&config), released);
    ReleaseComfyAnims();
}

TEST("Comfy Anim owned instances do not consume pool slots and reinitialization resets state")
{
    struct ComfyAnimEasingConfig easing;
    struct ComfyAnimSpringConfig spring;
    struct ComfyAnim anim;
    InitComfyAnimConfig_Easing(&easing);
    InitComfyAnimConfig_Spring(&spring);
    easing.to = Q_24_8(16);
    easing.durationFrames = 4;
    spring.to = Q_24_8(50);
    ReleaseComfyAnims();
    InitComfyAnim_Easing(&easing, &anim);
    TryAdvanceComfyAnim(&anim);
    EXPECT_EQ(anim.state.easingState.curFrame, 1);
    InitComfyAnim_Spring(&spring, &anim);
    EXPECT_EQ(anim.state.springState.overshootCount, 0);
    EXPECT_EQ(anim.velocity, 0);
    EXPECT_EQ(anim.position, spring.from);
    EXPECT_EQ(anim.config.type, COMFY_ANIM_TYPE_SPRING);
    TryAdvanceComfyAnim(&anim);
    InitComfyAnim_Easing(&easing, &anim);
    EXPECT_EQ(anim.state.easingState.curFrame, 0);
    EXPECT_EQ(anim.velocity, 0);
    EXPECT_EQ(anim.position, easing.from);
    EXPECT_EQ(anim.config.type, COMFY_ANIM_TYPE_EASING);
    for (u32 i = 0; i < NUM_COMFY_ANIMS; i++)
        EXPECT(!gComfyAnims[i].inUse);
}
