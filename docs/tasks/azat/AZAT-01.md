# AZAT-01 Ray intersection edge cases

Owner: **Azat Dauletbek** · **easy** · **1.5 hours**

Branch: `feature/azat-01-ray-edge-tests`

## Goal

Add three focused CPU tests for the wall raycaster without changing the raycaster.

## Background

The visibility system stops a ray at the closest wall it intersects. A hit depends on both the ray's forward direction and whether the intersection lies on the finite wall segment. For unit-length ray directions, the return value is a distance in world units.

## Starting point

`native/gfxcore/src/visibility.cpp`, `Visibility::ray`; compare `native/tests/tests.cpp` and open the new `native/tests/azat_visibility_cases.cpp`.

- `native/tests/azat_visibility_cases.cpp`

## Steps

1. Read the existing tests and keep their coverage intact.
2. Add separate doctest cases for an intersection behind the origin, an intersection beyond the distance limit, and two valid hits listed farther-first.
3. Compute the expected result on paper for each fixture before running it.

## Hints

Use small integer coordinates and a unit direction. Use a tolerance for floating-point comparisons; the starter case shows how this test file is picked up, not the solution to these cases.

## Acceptance criteria

- [ ] Each new case has a descriptive name and checks the expected numerical distance.
- [ ] The behind-origin and out-of-range cases return the supplied limit; the farther-first case returns the nearer hit.
- [ ] CTest passes; production geometry is unchanged.

## Build and test

Use the [Windows native build baseline](../../GIT_WORKFLOW.md#windows-native-build-baseline). Build the shared Release baseline, then run `ctest --test-dir build-team -C Release --output-on-failure`. For individual names, run `build-team/Release/tls_tests.exe --list-test-cases`.

## Pull request evidence

Summarize the three fixtures and paste the CTest result plus the new case names. A test log is enough evidence.

## Defence questions

1. Why does a wall behind the origin not count?
2. What does the return value mean if the direction has length one?
3. Why test farther-first ordering?
