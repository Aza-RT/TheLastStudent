# AZAT-05 Separate framebuffer binding regression

Owner: **Azat Dauletbek** · **stretch** · **2.5 hours**

Branch: `feature/azat-05-framebuffer-state-test`

## Goal

Extend the existing smoke checks to prove separate read and draw framebuffer bindings survive a guarded render pass.

## Background

OpenGL tracks different framebuffer bindings for drawing and reading. The `GL_FRAMEBUFFER` target changes both bindings at once. A shared renderer must restore them separately if the caller supplied different values.

## Starting point

`native/testbench/main.cpp`, `main`: the `if(smoke)` state-restoration block near startup; read `GLStateGuard` in `native/gfxcore/src/renderer.cpp`.

- `native/testbench/main.cpp`
- `native/testbench/azat_gl_state_checks.hpp (new)`

## Steps

1. Make a focused smoke helper using two small valid framebuffer objects. Bind distinct IDs to read and draw before entering the guarded scope.
2. Run a small guarded pass that changes the bindings, then query both bindings after scope exit.
3. Report expected and actual IDs for failures. Leave the normal loop's starting state intact and keep the current smoke checks.

## Hints

The existing `tls::Framebuffer` owns its resources; keep these objects alive until after your assertions. Take your starting snapshot before setting up the temporary test state.

## Acceptance criteria

- [ ] Two different starting IDs are confirmed, and both match their original bindings afterward.
- [ ] Smoke output makes the new check visible and a mismatch fails the run.
- [ ] The rest of the existing smoke suite still passes; production rendering code is unchanged.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Run `gl_testbench.exe --smoke`. As a temporary negative control, use an intentionally wrong expected ID and confirm the run fails, then undo that change.

## Pull request evidence

Paste the successful output and describe the negative control. Do not commit the deliberate failure.

## Defence questions

1. What operation uses the read framebuffer?
2. What does binding `GL_FRAMEBUFFER` change?
3. Why must the test FBOs stay alive until verification finishes?
