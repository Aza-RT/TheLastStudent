# AZAT-04 Manual PPM screenshot key

Owner: **Azat Dauletbek** · **medium** · **2 hours**

Branch: `feature/azat-04-manual-screenshot`

## Goal

Let a player save the current demo frame with P without losing the automatic smoke capture.

## Background

OpenGL reads pixels from a framebuffer, with the first row at the bottom. A typical PPM reader expects the first row at the top. Pixel packing also determines whether rows contain padding, so width and alignment matter when writing RGB bytes.

## Starting point

`native/testbench/main.cpp`, `main`: the `smoke && frames==20` capture block just before `glfwSwapBuffers`.

- `native/testbench/main.cpp`
- `native/testbench/azat_capture.hpp (new)`

## Steps

1. Move the existing PPM read/write steps into a small testbench helper, retaining the smoke filename and timing.
2. Add a P key press edge in the capture section and a separate manual filename, such as `testbench-manual.ppm`.
3. Report the output path or write error; restore any GL pack alignment you change.

## Hints

Capture after the scene and HUD have drawn and before buffer swap. Reuse the existing smoke code for orientation rather than introducing a PNG dependency.

## Acceptance criteria

- [ ] One press produces one capture; holding P does not repeatedly write.
- [ ] The image opens upright at framebuffer resolution, and manual capture does not replace `testbench.ppm`.
- [ ] `--smoke` still produces its original automatic file and exits successfully.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run the demo, aim the beam, press and hold P, and inspect the output. Try an odd-width framebuffer if possible, then run `--smoke`.

## Pull request evidence

Attach a readable PNG conversion of the manually produced PPM, and state where the original PPM was written. Leave generated captures out of Git.

## Defence questions

1. Why capture before swap?
2. Why reverse the rows?
3. How can pack alignment affect an odd-width RGB image?
