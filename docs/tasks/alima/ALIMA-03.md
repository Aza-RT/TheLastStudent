# ALIMA-03 Animated lamp fixture sprite

Owner: **Alima Beisembayeva** · **medium** · **2 hours**

Branch: `feature/alima-03-animated-lamp-strip`

## Goal

Draw and animate a small lamp fixture where the first point light already sits.

## Background

The renderer already varies the brightness of the emitted light. This task adds the visible fixture and animates its sprite. A horizontal strip holds multiple frames; elapsed time selects which UV rectangle to draw.

## Starting point

`native/testbench/main.cpp`, `main`: after the existing player batch flush and before the `lights` vector; use the first lamp location in that vector.

- `native/testbench/assets/alima/lamp_strip.png (new)`
- `native/testbench/main.cpp`

## Steps

1. Draw four 32×32 RGBA frames of the same fixture, with changes to the glowing parts and a stable silhouette.
2. Load the strip through `TLS_TESTBENCH_ASSET_DIR`. Draw it in a dedicated world-space sprite batch before lighting is composited.
3. Pick a readable frame rate and wrap the frame index. Do not modify the existing lamp or torch light settings.

## Hints

Keep the fixture centred on the first light. Use elapsed time rather than incrementing once per rendered frame, so playback does not depend on FPS.

## Acceptance criteria

- [ ] A fixture at the first point light cycles through all four frames, without position jumps or UV bleeding.
- [ ] The animation is visible when standing still; the existing light falloff remains intact.
- [ ] No changes to Godot, the shared atlas, or other props.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the demo, walk to the first light near world (230, 130), watch several loops, and run `--smoke`.

## Pull request evidence

Attach the authored strip and a short GIF or two screenshots showing different frames. Explain your chosen frame timing.

## Defence questions

1. How is a sprite animation different from light intensity flicker?
2. How does time become a frame index?
3. Why do the frames share the same pivot and silhouette?
