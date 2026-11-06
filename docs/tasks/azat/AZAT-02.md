# AZAT-02 Shader compile and link diagnostics

Owner: **Azat Dauletbek** · **easy** · **1.5 hours**

Branch: `feature/azat-02-shader-diagnostics`

## Goal

Make shader failures say which step failed and retain the full driver log.

## Background

The vertex and fragment shaders compile separately, then link into a program. A compile failure may be a syntax error; a link failure may be an interface mismatch. The current implementation throws driver text from fixed-size buffers, without a label for the failed step.

## Starting point

`native/gfxcore/src/renderer.cpp`, `Shader::compile` only.

- `native/gfxcore/src/renderer.cpp`

## Steps

1. Label the two compile failure paths and the program link failure path.
2. Query the reported log length for the object that failed; handle an empty log sensibly.
3. Keep the current cleanup and successful program replacement behaviour. Use temporary local edits to trigger a fragment syntax error and a link error, then restore them.

## Hints

Shader objects and program objects have different status and log queries. Look at where the old program is deleted; an unsuccessful replacement should never reach that point.

## Acceptance criteria

- [ ] The console distinguishes vertex compile, fragment compile, and program link failures.
- [ ] Logs are read using the reported length rather than a fixed cap.
- [ ] Only diagnostic code is committed; valid shaders still run.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Build Release and run `gl_testbench.exe --smoke`. For failure evidence, locally introduce a fragment syntax error and then a mismatch between a used fragment input and its matching vertex output. Rebuild for each and restore the shader files before committing.

## Pull request evidence

Paste both failure messages and the restored smoke pass. Explain how you kept the failure cleanup intact.

## Defence questions

1. What is the difference between compiling and linking?
2. Why query log length?
3. What happens to the currently working program if replacement fails?
