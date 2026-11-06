# Ready to paste GitHub issues

Create issues from the GitHub interface. These are drafts; no issues or assignments have been created remotely. Copy the title and body for one task at a time, replace the task-card placeholder, and apply the listed labels in GitHub.

## AZAT-01 Ray intersection edge cases

Suggested labels: `opengl`, `cpp`, `good first task`

**Goal:** Add three focused CPU tests for the wall raycaster without changing the raycaster.

**Task card:** AZAT-01 (attach the card or link to the team copy)

**Branch:** `feature/azat-01-ray-edge-tests`

**Done when:**

- [ ] Each new case has a descriptive name and checks the expected numerical distance.
- [ ] The behind-origin and out-of-range cases return the supplied limit; the farther-first case returns the nearer hit.
- [ ] CTest passes; production geometry is unchanged.

**PR evidence:** Summarize the three fixtures and paste the CTest result plus the new case names. A test log is enough evidence.

## AZAT-02 Shader compile and link diagnostics

Suggested labels: `opengl`, `cpp`, `shader`, `good first task`

**Goal:** Make shader failures say which step failed and retain the full driver log.

**Task card:** AZAT-02 (attach the card or link to the team copy)

**Branch:** `feature/azat-02-shader-diagnostics`

**Done when:**

- [ ] The console distinguishes vertex compile, fragment compile, and program link failures.
- [ ] Logs are read using the reported length rather than a fixed cap.
- [ ] Only diagnostic code is committed; valid shaders still run.

**PR evidence:** Paste both failure messages and the restored smoke pass. Explain how you kept the failure cleanup intact.

## AZAT-03 Rolling frame time in the window title

Suggested labels: `opengl`, `cpp`

**Goal:** Add a rolling average frame time in milliseconds to the window title, alongside the existing FPS display.

**Task card:** AZAT-03 (attach the card or link to the team copy)

**Branch:** `feature/azat-03-rolling-frame-time`

**Done when:**

- [ ] Title displays a finite positive rolling value after sampling starts, with `ms` units.
- [ ] The value uses the raw frame interval and history stays bounded.
- [ ] Existing movement, FPS overlay and smoke mode still work.

**PR evidence:** Attach a screenshot with the title visible, and say what the sample count is and whether VSync was on.

## AZAT-04 Manual PPM screenshot key

Suggested labels: `opengl`, `cpp`

**Goal:** Let a player save the current demo frame with P without losing the automatic smoke capture.

**Task card:** AZAT-04 (attach the card or link to the team copy)

**Branch:** `feature/azat-04-manual-screenshot`

**Done when:**

- [ ] One press produces one capture; holding P does not repeatedly write.
- [ ] The image opens upright at framebuffer resolution, and manual capture does not replace `testbench.ppm`.
- [ ] `--smoke` still produces its original automatic file and exits successfully.

**PR evidence:** Attach a readable PNG conversion of the manually produced PPM, and state where the original PPM was written. Leave generated captures out of Git.

## AZAT-05 Separate framebuffer binding regression

Suggested labels: `opengl`, `cpp`

**Goal:** Extend the existing smoke checks to prove separate read and draw framebuffer bindings survive a guarded render pass.

**Task card:** AZAT-05 (attach the card or link to the team copy)

**Branch:** `feature/azat-05-framebuffer-state-test`

**Done when:**

- [ ] Two different starting IDs are confirmed, and both match their original bindings afterward.
- [ ] Smoke output makes the new check visible and a mismatch fails the run.
- [ ] The rest of the existing smoke suite still passes; production rendering code is unchanged.

**PR evidence:** Paste the successful output and describe the negative control. Do not commit the deliberate failure.

## ALIMA-01 Original prop strip for the native demo

Suggested labels: `opengl`, `art`, `good first task`

**Goal:** Draw three distinct pixel-art props and show them as a row in the standalone demo.

**Task card:** ALIMA-01 (attach the card or link to the team copy)

**Branch:** `feature/alima-01-prop-strip`

**Done when:**

- [ ] Three original and distinguishable props are visible in the demo, with no neighbouring-cell bleed.
- [ ] Transparent areas show the floor and edges stay pixel-sharp.
- [ ] The game atlas and player appearance are unchanged; normal demo and smoke pass.

**PR evidence:** Attach the authored strip and a screenshot of all three props in the lit demo. Explain your palette and cell order; commit the PNG asset itself.

## ALIMA-02 Flashlight colour presets

Suggested labels: `opengl`, `art`, `good first task`

**Goal:** Add a T key that cycles the current torch colour, a warm amber and a cold cyan.

**Task card:** ALIMA-02 (attach the card or link to the team copy)

**Branch:** `feature/alima-02-torch-colour-presets`

**Done when:**

- [ ] T cycles through three distinguishable colours and wraps to the original.
- [ ] F still toggles the torch; after re-enabling it, the selected preset is retained.
- [ ] Walls still block the same shape, and fixed lamps do not change colour.

**PR evidence:** Attach three comparable screenshots and explain which palette looks most suitable and why.

## ALIMA-03 Animated lamp fixture sprite

Suggested labels: `opengl`, `art`

**Goal:** Draw and animate a small lamp fixture where the first point light already sits.

**Task card:** ALIMA-03 (attach the card or link to the team copy)

**Branch:** `feature/alima-03-animated-lamp-strip`

**Done when:**

- [ ] A fixture at the first point light cycles through all four frames, without position jumps or UV bleeding.
- [ ] The animation is visible when standing still; the existing light falloff remains intact.
- [ ] No changes to Godot, the shared atlas, or other props.

**PR evidence:** Attach the authored strip and a short GIF or two screenshots showing different frames. Explain your chosen frame timing.

## ALIMA-04 Optional palette limiting

Suggested labels: `opengl`, `art`, `shader`

**Goal:** Add a G toggle that limits the lit scene to a small number of tonal steps in the native demo.

**Task card:** ALIMA-04 (attach the card or link to the team copy)

**Branch:** `feature/alima-04-palette-limiting`

**Done when:**

- [ ] G toggles a clearly visible change; off is the existing look.
- [ ] Godot's default native rendering and HUD behaviour are unchanged.
- [ ] Existing post effects and the native smoke run still work.

**PR evidence:** Attach a matched pair of screenshots and note how many tonal levels you chose and why.

## ALIMA-05 Static CRT scanline overlay

Suggested labels: `opengl`, `art`, `shader`

**Goal:** Add an optional screen-space overlay with fine, regular CRT lines, distinct from the current threat-driven scanline displacement.

**Task card:** ALIMA-05 (attach the card or link to the team copy)

**Branch:** `feature/alima-05-crt-scanline-overlay`

**Done when:**

- [ ] C toggles stable fine horizontal bands, clearly different from moving threat strips.
- [ ] Resizing keeps the row spacing in pixels; F3 text stays readable.
- [ ] Default-off smoke passes; no Godot code or shader settings change.

**PR evidence:** Attach off/on screenshots and one resized view. State the spacing and alpha you chose.
