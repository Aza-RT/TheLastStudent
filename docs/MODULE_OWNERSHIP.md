# Planned module ownership

These roles describe upcoming responsibilities. They do not credit work that has not been written and do not change authorship for the existing game.

| Teammate | Role | Proposed scope |
| --- | --- | --- |
| Azat Dauletbek | Lead Programmer / Tech Lead | Native visibility tests, shader diagnostics, testbench timing and screenshot tooling, OpenGL state regression checks |
| Alima Beisembayeva | Art & Visual Design | Original testbench textures, fixture animation, flashlight colour, postprocess palette and CRT look |

Each teammate implements the tasks on their own branches and opens PRs under their own account. Ask the other teammate to review changes, especially edits near the shared testbench loop. For shared renderer code, Azat's diagnostic task owns `Shader::compile`; Alima's palette task owns only the additional postprocess setting and its uniform upload in `Renderer::composite`. Neither role requires changes to Godot scenes, GDScript, or game logic.

See [the task overview](TEAM_TASKS.md) for the exact assignments and branch names.
