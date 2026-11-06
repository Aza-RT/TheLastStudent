# Team contribution tasks

These are proposed assignments, not completed work or authorship credits. Each student writes and tests their own implementation, makes their own commits, and submits their own PR. Keep the scope in the native demo or native tests; do not edit Godot scenes, GDScript, or game logic.

| Task ID | Owner | Title | Difficulty | Estimated time | Files touched | Branch name |
| --- | --- | --- | --- | --- | --- | --- |
| [AZAT-01](tasks/azat/AZAT-01.md) | Azat Dauletbek | Ray intersection edge cases | easy | 1.5 h | `native/tests/azat_visibility_cases.cpp` | `feature/azat-01-ray-edge-tests` |
| [AZAT-02](tasks/azat/AZAT-02.md) | Azat Dauletbek | Shader compile and link diagnostics | easy | 1.5 h | `native/gfxcore/src/renderer.cpp` | `feature/azat-02-shader-diagnostics` |
| [AZAT-03](tasks/azat/AZAT-03.md) | Azat Dauletbek | Rolling frame time in the window title | medium | 2 h | `native/testbench/main.cpp`, `native/testbench/azat_frame_time.hpp (new)` | `feature/azat-03-rolling-frame-time` |
| [AZAT-04](tasks/azat/AZAT-04.md) | Azat Dauletbek | Manual PPM screenshot key | medium | 2 h | `native/testbench/main.cpp`, `native/testbench/azat_capture.hpp (new)` | `feature/azat-04-manual-screenshot` |
| [AZAT-05](tasks/azat/AZAT-05.md) | Azat Dauletbek | Separate framebuffer binding regression | stretch | 2.5 h | `native/testbench/main.cpp`, `native/testbench/azat_gl_state_checks.hpp (new)` | `feature/azat-05-framebuffer-state-test` |
| [ALIMA-01](tasks/alima/ALIMA-01.md) | Alima Beisembayeva | Original prop strip for the native demo | easy | 2 h | `native/testbench/assets/alima/props.png (new)`, `native/testbench/main.cpp` | `feature/alima-01-prop-strip` |
| [ALIMA-02](tasks/alima/ALIMA-02.md) | Alima Beisembayeva | Flashlight colour presets | easy | 1.5 h | `native/testbench/main.cpp` | `feature/alima-02-torch-colour-presets` |
| [ALIMA-03](tasks/alima/ALIMA-03.md) | Alima Beisembayeva | Animated lamp fixture sprite | medium | 2 h | `native/testbench/assets/alima/lamp_strip.png (new)`, `native/testbench/main.cpp` | `feature/alima-03-animated-lamp-strip` |
| [ALIMA-04](tasks/alima/ALIMA-04.md) | Alima Beisembayeva | Optional palette limiting | medium | 2.5 h | `native/shaders/post.frag`, `native/gfxcore/include/tls/renderer.hpp`, `native/gfxcore/src/renderer.cpp`, `native/testbench/main.cpp` | `feature/alima-04-palette-limiting` |
| [ALIMA-05](tasks/alima/ALIMA-05.md) | Alima Beisembayeva | Static CRT scanline overlay | stretch | 3 h | `native/shaders/alima_crt.frag`, `native/testbench/main.cpp`, `native/testbench/alima_crt.hpp (new)` | `feature/alima-05-crt-scanline-overlay` |

**Suggested order:** Azat 01 → 02 → 03 → 04 → 05 (**9.5 h**). Alima 01 → 02 → 03 → 04 → 05 (**11 h**). The estimates are for implementation and testing after the toolchain is set up. Tasks do not require one another's code.

Several tasks use the same small `native/testbench/main.cpp` entry point, but the cards name separate edit points. Keep new helpers or assets in their task-specific files. Keep input edits beside the feature they control, use the assigned keys, and avoid reformatting or moving the main loop. If another PR merged first, update your branch from `master` and rerun the smoke check before merging.

Use the [Git workflow](GIT_WORKFLOW.md) and [module ownership](MODULE_OWNERSHIP.md). The [issue drafts](tasks/GITHUB_ISSUES.md) can be copied into GitHub. Attach evidence to issues or PRs rather than committing generated captures.
