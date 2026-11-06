# ALIMA-05 Static CRT scanline overlay

Owner: **Alima Beisembayeva** · **stretch** · **3 hours**

Branch: `feature/alima-05-crt-scanline-overlay`

## Goal

Add an optional screen-space overlay with fine, regular CRT lines, distinct from the current threat-driven scanline displacement.

## Background

The current effect moves occasional strips of the scene as threat increases. CRT scanlines can instead use a repeating pixel-row pattern, independent of the scene and threat. A fullscreen triangle and alpha blending can add a subtle overlay without reading back the scene.

## Starting point

`native/shaders/alima_crt.frag` (unused starter), `native/shaders/fullscreen.vert`, and `native/testbench/main.cpp`, `main`: immediately after `renderer.composite(...)`, before F3 text draws.

- `native/shaders/alima_crt.frag`
- `native/testbench/main.cpp`
- `native/testbench/alima_crt.hpp (new)`

## Steps

1. Use the starter fragment shader and the existing fullscreen vertex shader, loaded from `TLS_SHADER_DIR` in the standalone demo only. Add the row-pattern math yourself.
2. Use a small standalone helper or local draw setup, an empty VAO and alpha blending. Add a C toggle, default off, after compositing and before drawing debug text.
3. Keep the effect subtle, and ensure any changed GL state does not leak into the HUD or next frame.

## Hints

Start by getting the transparent starter pass to compile and link. Think about whether `gl_FragCoord.y` keeps row spacing in pixels when the window is resized. Do not add this fragment shader to the shared embedded shader list.

## Acceptance criteria

- [ ] C toggles stable fine horizontal bands, clearly different from moving threat strips.
- [ ] Resizing keeps the row spacing in pixels; F3 text stays readable.
- [ ] Default-off smoke passes; no Godot code or shader settings change.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the demo at two window sizes and compare C off/on. Toggle F3 and test C again. Run `--smoke` afterward.

## Pull request evidence

Attach off/on screenshots and one resized view. State the spacing and alpha you chose.

## Defence questions

1. Why does the fullscreen draw need a VAO in a core context?
2. Why use framebuffer coordinates rather than UV for fixed pixel spacing?
3. How does the blend state mix the overlay with the scene?
