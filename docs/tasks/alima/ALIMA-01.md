# ALIMA-01 Original prop strip for the native demo

Owner: **Alima Beisembayeva** · **easy** · **2 hours**

Branch: `feature/alima-01-prop-strip`

## Goal

Draw three distinct pixel-art props and show them as a row in the standalone demo.

## Background

An atlas packs multiple sprites into one texture, and each sprite samples only its assigned UV rectangle. The current `SpriteBatch` uses one texture per flush. A separate texture batch lets the demo use new art while keeping the game's shared atlas unchanged.

## Starting point

`native/testbench/main.cpp`, `main`: the six-prop loop before the player sprite; read `SpriteBatch::flush` in `native/gfxcore/src/renderer.cpp`.

- `native/testbench/assets/alima/props.png (new)`
- `native/testbench/main.cpp`

## Steps

1. Draw a transparent 96×32 RGBA PNG with three original 32×32 cells, for example a locker, vending machine and bench.
2. Load the strip via `TLS_TESTBENCH_ASSET_DIR`. Replace only the existing prop loop with your strip draw, flushing queued world geometry before switching textures.
3. Use the same camera transform for the prop strip, then start the normal batch again for the player.

## Hints

Think in normalized texture coordinates rather than pixel positions when selecting each cell. Leave padding within each cell and test the art under the torch.

## Acceptance criteria

- [ ] Three original and distinguishable props are visible in the demo, with no neighbouring-cell bleed.
- [ ] Transparent areas show the floor and edges stay pixel-sharp.
- [ ] The game atlas and player appearance are unchanged; normal demo and smoke pass.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the demo, walk down to the existing prop row around world y=540, and sweep the flashlight across it. Run `--smoke` afterward.

## Pull request evidence

Attach the authored strip and a screenshot of all three props in the lit demo. Explain your palette and cell order; commit the PNG asset itself.

## Defence questions

1. What is a UV rectangle?
2. Why flush before changing textures?
3. How does nearest filtering help pixel art?
