# ALIMA-02 Flashlight colour presets

Owner: **Alima Beisembayeva** · **easy** · **1.5 hours**

Branch: `feature/alima-02-torch-colour-presets`

## Goal

Add a T key that cycles the current torch colour, a warm amber and a cold cyan.

## Background

The light shader already receives an RGB colour uniform from each C++ `Light`. Choosing a different value changes the illumination without changing the visibility polygon. Comparing colours at equal radius and strength isolates the artistic choice.

## Starting point

`native/testbench/main.cpp`, `main`: the player flashlight entry in `lights`; trace `light_color` in `Renderer::composite` and `native/shaders/light.frag`.

- `native/testbench/main.cpp`

## Steps

1. Keep the original torch colour as the initial preset.
2. Add testbench-only input for T and cycle once per press. Change only the flashlight's colour value.
3. Compare the three presets at the same position and aim, including a surface close to a wall.

## Hints

Use named RGB values and leave the two point-lamp entries and all cone settings alone. Keep the preset state across F off/on.

## Acceptance criteria

- [ ] T cycles through three distinguishable colours and wraps to the original.
- [ ] F still toggles the torch; after re-enabling it, the selected preset is retained.
- [ ] Walls still block the same shape, and fixed lamps do not change colour.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the demo and capture the three views with the camera and mouse held still. Run the normal smoke afterward.

## Pull request evidence

Attach three comparable screenshots and explain which palette looks most suitable and why.

## Defence questions

1. What does the `light_color` uniform represent?
2. Why hold strength and angle constant for the comparison?
3. Why does changing colour not move a shadow edge?
