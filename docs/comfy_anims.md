# Comfy Anims

Imported from [huderlem's Comfy Anims](https://github.com/huderlem/pokeemerald/tree/comfy_anims),
commit `9d0b1d9000b395fb10547bb30276eb64be9f0fc5`.
The implementation is in `src/comfy_anim.c`; its API is in `include/comfy_anim.h`.

This is a reusable spring/easing animation library. The start menu arrow and both
Options menu arrows use six-frame ease-out cubic movement. Logical selections
change immediately, and rapid inputs retarget from the current animated position.
The Options arrows also ease horizontally when the selected row's bounds change,
settling at their usual two-pixel gap. Initial positions snap into place when
opening the menus. No battle animations are changed.

There is no global frame/task hook: each screen explicitly owns its animation
updates and cleanup. The start menu guards against advancing twice when link
processing runs its task more than once in the same frame.
No save data or heap allocations are needed. The optional eight-animation pool
occupies 448 bytes of EWRAM when linked; callers can also own individual instances.

## Using it

Values such as positions, tension, friction, and mass use `Q_24_8` fixed point.
Durations, delays, and clamp counts are ordinary integers.

```c
#include "comfy_anim.h"

// Create an easing animation for a screen-owned position.
struct ComfyAnimEasingConfig config;
InitComfyAnimConfig_Easing(&config);
config.from = Q_24_8(0);
config.to = Q_24_8(100);
config.durationFrames = 12;
config.easingFunc = ComfyAnimEasing_EaseOutCubic;
u32 id = CreateComfyAnim_Easing(&config);
if (id == INVALID_COMFY_ANIM)
{
    // Pool full: use the final position or another screen-specific fallback.
}
```

For pooled animations, call `AdvanceComfyAnimations()` once per screen frame,
typically after `RunTasks()` and before `AnimateSprites()`. Read positions with
`ReadComfyAnimValueSmooth(&gComfyAnims[id])`. Check `completed` and release each
finished animation with `ReleaseComfyAnim(id)`. Completion does not free the slot:
the owner must read the final position before releasing it. On leaving the screen,
release all its IDs; use `ReleaseComfyAnims()` only when the screen owns the entire
pool, typically alongside its `ResetTasks()`.

The menu integrations use owned instances instead of the shared pool. Only the
start menu opts into `Menu_SetCursorEasing(TRUE)`; normal and grid menu initializers
disable it so save confirmations and other menus retain their existing behavior.
The start menu task calls `Menu_UpdateCursorEasing()` while active. Window removal
and screen cleanup stop it safely, including when closing midway through a move.
Options advances its three owned axes once per menu callback, shares the vertical
axis between both arrow sprites, and clears them when the sprites are destroyed.

Alternatively, keep a `struct ComfyAnim` in screen-owned memory, initialize it
with `InitComfyAnim_Easing()` or `InitComfyAnim_Spring()`, and call
`TryAdvanceComfyAnim()` once per frame yourself. Do not also update that instance
through the pool. Do not put these temporary animations in the save data.

Spring configurations can be initialized with `InitComfyAnimConfig_Spring()`.
Changing `anim.config.data.spring.to` retargets a running or settled spring.
`clampAfter = 0` lets it settle naturally; positive values stop it after that many
crossings of its destination. Completed spring slots remain owned until released.

## Local fixes

- Easing animations now honor `delayFrames`, just like springs.
- Zero-duration easing snaps to the destination on its first update after any
  delay; all easing completions land exactly on the destination.
- Completion clears residual velocity, and retargeting a settled spring clears
  its completed flag while it moves toward the new destination.
- A missing easing callback uses linear easing. Nonpositive spring mass uses the
  default mass, avoiding an animation that cannot advance.
- The public header includes its required types and has a corrected guard comment.

Regression tests are in `test/comfy_anim.c`, `test/menu_cursor.c`, and
`test/option_menu.c`.
