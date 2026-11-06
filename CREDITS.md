# Credits and asset register

Original game code, shaders, procedural art, audio, map design and writing: **The Last Student contributors**, 2026. Created for a three-person Computer Graphics Fundamentals project at Astana IT University. No real teachers are named and no university logo was used.

Original project sources and generated creative assets are covered by the root **MIT License**. External dependencies retain their own licenses. AITU is the setting, not a claim of official endorsement.

## Every shipped creative asset

| Asset | Source / generation | License |
|---|---|---|
| `game/assets/art/atlas.png` | Original deterministic Python pixel art, `tools/generate_assets.py`; 256×256 RGBA, 8×8 cells of 32px | Project MIT |
| Atlas 0–2 | Scuffed stone flooring with per-pixel deterministic detail | Project MIT |
| Atlas 3 | Masonry wall and baseboard | Project MIT |
| Atlas 4–7 | Screens, vending/utility surfaces and paper/whiteboard | Project MIT |
| Atlas 8–15 | Desk, bookshelf, lockers, refrigerator, locked turnstile, bench, ceiling lamp, toilet | Project MIT |
| Atlas 16–23 | Three distinct ID fragments, two keys, glue, restored ID, note icon | Project MIT |
| Atlas 24–27 | Four-frame Night Shade animation | Project MIT |
| Atlas 28–31 | Coffee, plant, canteen table, counter | Project MIT |
| Atlas 32–47 | Four directions × four walk frames of original student character | Project MIT |
| Atlas 48 | Green/unlocked turnstile with folded arms | Project MIT |
| Atlas 49–63 | Reserved transparent replacement cells | Project MIT |
| `game/assets/art/debug_font.png` | Original hand-specified 5×7 glyph bitmaps in the same generator; lowercase maps to uppercase; no external font file | Project MIT |
| Menu campus facade / torn-ID illustration | Procedural shapes in `game/scripts/ui/menu_backdrop.gd` | Project MIT |
| UI panels, frames, selection effects, glow markers | Procedural drawing / StyleBoxFlat in main and object scripts | Project MIT |
| `game/assets/audio/hum.wav` | 55/110 Hz harmonics, low noise and slow modulation; Python synthesis | Project MIT |
| `step.wav` | Short decaying noise and low body tone; walk/sprint use different gain/pitch | Project MIT |
| `heartbeat.wav` | Two decaying bass pulses | Project MIT |
| `click.wav` | Short filtered-by-envelope noise burst | Project MIT |
| `pickup.wav` | Two sine pitches with a smooth envelope | Project MIT |
| `lock.wav` | Decaying low mechanical tone/noise | Project MIT |
| `sting.wav` | Detuned rising tones and noise | Project MIT |
| `escape.wav` | C-major chord and smooth envelope | Project MIT |
| All eight WAV files | `tools/generate_audio.py`, deterministic mono 16-bit PCM, 22,050 Hz | Project MIT |
| `game/data/dialogue.json`, `items.json`, `map_layout.json` | Original English monologues, notes, puzzle clues and fictionalized layout; editable generation source in `tools/generate_world.py` | Project MIT |
| `game/icon.svg` | Original vector torn-ID motif | Project MIT |
| Runtime UI font | Godot's embedded fallback font; no font file copied into this project | Included with Godot; see [Godot license](https://godotengine.org/license/) |

PNG import sidecars and `.uid` files are Godot metadata, not additional artworks.

## Libraries and tools

| Component | Pin / source | License / notice |
|---|---|---|
| Godot engine | Installed `4.7.2.stable.official.ed1daf0bf`; external prerequisite | [MIT and bundled notices](https://godotengine.org/license/) |
| godot-cpp | Upstream commit `2bb1acf6f8ae1ce96ac7b9ba3a10b499c59c49ec`, 4.7-capable generator, custom exact 4.7.2 API dump | MIT, `docs/licenses/godot-cpp.txt` |
| GLM | 1.0.1 | MIT option selected, `docs/licenses/GLM.txt` |
| GLFW | 3.4 | zlib/libpng, `docs/licenses/GLFW.txt` |
| GLEW / glew-cmake | 2.2.0 | BSD-style / MIT / Khronos notices, `docs/licenses/GLEW.txt` |
| stb_image | Commit `f0569113c93ad095470c54bf34a17b36646bbbb5` | MIT option selected, `docs/licenses/stb.txt` |
| doctest | 2.4.11 | MIT, `docs/licenses/doctest.txt` |
| Python | Asset/audio tools and binding generation, not redistributed | PSF |
| Pillow | Optional original-art PNG generation tool, not redistributed | MIT-CMU |
| CMake / MSVC | External build tools, not redistributed | Their respective upstream licenses |

Archive versions and SHA-256 pins are in `CMakeLists.txt`. Dependencies are fetched, not committed as embedded Git repositories. GLEW is statically linked; the same notice accompanies both the standalone app and DLL. Keep the license directory with any binary distribution.
