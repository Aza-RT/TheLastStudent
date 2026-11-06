# The Last Student

**AITU Night Shift — Escape** is a top-down campus horror game. Search three university blocks for the pieces of a torn student ID, restore it, and escape through the atrium while avoiding the Night Shade.

## Build and play

See [BUILD.md](BUILD.md) for Windows x64 prerequisites and build instructions. The game uses Godot 4.7.2 in Compatibility mode and a C++17 extension. Build the extension before opening the Godot project.

```powershell
.\tools\build.ps1
.\tools\run.ps1 -Godot 'C:\Tools\Godot_v4.7.2-stable_win64_console.exe'
```

| Key | Action |
| --- | --- |
| WASD / arrows | Move |
| Mouse | Aim flashlight |
| F | Toggle flashlight |
| Shift | Sprint |
| E | Interact, hide, or leave hiding |
| Tab / I | Open inventory and combine selected materials |
| Esc | Pause or close a panel |
| F3 | Show graphics and game diagnostics |
| F4 | Switch between native OpenGL and fallback lighting |

## Source and tests

- `game/` contains the playable Godot project and assets.
- `native/` contains the OpenGL renderer, GLSL shaders, Shade AI, Godot bindings, and standalone graphics testbench.
- `tools/` contains build, asset generation, and smoke-test scripts.

Run `tools/smoke.ps1` with the same Godot executable after building. The native tests cover geometry and AI; the game smoke test exercises the full puzzle and inventory flow. The standalone OpenGL demonstration builds to `build/Release/gl_testbench.exe`.

See [CREDITS.md](CREDITS.md) for asset and dependency notices.
