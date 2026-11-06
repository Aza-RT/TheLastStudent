# AZAT-03 Rolling frame time in the window title

Owner: **Azat Dauletbek** · **medium** · **2 hours**

Branch: `feature/azat-03-rolling-frame-time`

## Goal

Add a rolling average frame time in milliseconds to the window title, alongside the existing FPS display.

## Background

FPS measures frames per second; milliseconds measure how much time one frame takes. The movement step caps its delta, so that capped value can hide a real pause. A short rolling window gives a steadier timing readout than a single frame.

## Starting point

`native/testbench/main.cpp`, `main`: the `before`/`now` timing code and `glfwSetWindowTitle` block.

- `native/testbench/main.cpp`
- `native/testbench/azat_frame_time.hpp (new)`

## Steps

1. Write a small helper for a bounded history of frame intervals; choose and document a sample count.
2. Feed it the unclamped interval, leaving movement timing alone.
3. Show the average in milliseconds in the title only. Keep the existing F3 overlay and its controls.

## Hints

Think about the first sample and partially filled buffers. A short list is fine; the important parts are valid sample counting and bounded storage.

## Acceptance criteria

- [ ] Title displays a finite positive rolling value after sampling starts, with `ms` units.
- [ ] The value uses the raw frame interval and history stays bounded.
- [ ] Existing movement, FPS overlay and smoke mode still work.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the Release demo with F3 on, then resize or briefly drag the window and watch the value settle. Run `--smoke` after testing.

## Pull request evidence

Attach a screenshot with the title visible, and say what the sample count is and whether VSync was on.

## Defence questions

1. Why not use the movement delta?
2. How do you avoid dividing by empty slots at startup?
3. Why is this not a measurement of GPU execution time alone?
