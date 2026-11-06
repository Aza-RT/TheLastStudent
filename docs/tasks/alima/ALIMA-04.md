# ALIMA-04 Optional palette limiting

Owner: **Alima Beisembayeva** · **medium** · **2.5 hours**

Branch: `feature/alima-04-palette-limiting`

## Goal

Add a G toggle that limits the lit scene to a small number of tonal steps in the native demo.

## Background

A fragment shader computes the colour of a pixel. Palette limiting groups nearby colours into visible bands instead of allowing every intermediate tone. In the existing compositor, scene lighting, vignette, and grain already have different roles, so the point where you apply the grading matters.

## Starting point

`native/shaders/post.frag`, `main`: the final world colour before `frag`; follow `Renderer::composite` to the post uniform uploads.

- `native/shaders/post.frag`
- `native/gfxcore/include/tls/renderer.hpp`
- `native/gfxcore/src/renderer.cpp`
- `native/testbench/main.cpp`

## Steps

1. Choose a small set of tonal steps and experiment with it on the already-lit world colour. Keep the HUD early return unchanged.
2. Add one default-off renderer setting and upload a matching uniform in the post portion of `Renderer::composite`. Use the shared library's existing parameter style; leave `Shader::compile` alone.
3. Toggle the setting only in the testbench. Keep the existing `renderer.composite(...)` call intact so the CRT task can work immediately after it.

## Hints

Try a flat gradient or the torch falloff to see the bands. Consider what happens to small grain variations if you limit tones before versus after grain.

## Acceptance criteria

- [ ] G toggles a clearly visible change; off is the existing look.
- [ ] Godot's default native rendering and HUD behaviour are unchanged.
- [ ] Existing post effects and the native smoke run still work.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Rebuild from the repository root so CMake refreshes the embedded shader. Run the demo and compare G off/on at the same viewpoint. Run `--smoke`.

## Pull request evidence

Attach a matched pair of screenshots and note how many tonal levels you chose and why.

## Defence questions

1. What is colour quantization?
2. Where do you grade relative to lighting and grain, and why?
3. How do you keep the effect disabled for Godot by default?
