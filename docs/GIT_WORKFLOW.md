# Git workflow for the team

Each student uses their own GitHub account and writes their own code, commits, and PR description. [Pick a task card](TEAM_TASKS.md) and use its branch name. Work on one task per branch.

Start from the latest `master`, which includes the task cards and scaffolding. If you cloned the repository before these files were added, update `master` before creating your feature branch.

## Clone and make a branch

In **PowerShell** or **Git Bash**, run:

```bash
git clone https://github.com/Aza-RT/TheLastStudent.git
cd TheLastStudent
git switch master
git pull --ff-only origin master
git switch -c feature/azat-01-ray-edge-tests
```

Alima uses `git switch -c feature/alima-01-prop-strip` instead for her first task. Use the corresponding branch name from each later card. The target repository currently uses `master`; if the maintainer shares the scaffold on another base, use the specified base rather than branching from a copy missing the scaffolding.

Check `git config user.name` and `git config user.email` before committing. Configure your own account's identity for this clone if necessary; don't copy a teammate's settings. On a shared PC, the HTTPS credential prompt must also sign in to your own GitHub account.

## Windows native build baseline

Install Git, CMake 3.24 or newer, and the **Desktop development with C++** workload with a Windows SDK. The commands below use Visual Studio 2022. With VS 2026, use `-G 'Visual Studio 18 2026'` and a fresh build directory if one was configured for another generator. Internet access is required for the first CMake configure to fetch pinned dependencies.

Run these from the repository root in **PowerShell** or **Git Bash**:

```bash
cmake -S . -B build-team -G 'Visual Studio 17 2022' -A x64 -DTLS_BUILD_EXTENSION=OFF
cmake --build build-team --config Release --target gl_testbench tls_tests --parallel 4
ctest --test-dir build-team -C Release --output-on-failure
./build-team/Release/gl_testbench.exe --smoke
```

For the interactive demo:

```powershell
.\build-team\Release\gl_testbench.exe
```

```bash
./build-team/Release/gl_testbench.exe
```

Use the first launch command in PowerShell and the second in Git Bash. The demo and GL smoke check require a desktop and an OpenGL 3.3-capable driver. The CPU test executable can run without an OpenGL window. None of these assignments require opening Godot; for a full game build, use [BUILD.md](../BUILD.md) and Godot 4.7.2. Shared shaders such as `post.frag` are embedded during the CMake build, so rebuild after editing them. The standalone executable resolves checked-in art from its configured source checkout; rebuild after moving the checkout.

## Make a small commit

Run `git status --short` and `git diff`. Stage specific files rather than an entire workspace. Example commands (PowerShell or Git Bash):

```bash
git add native/tests/azat_visibility_cases.cpp
git diff --cached
git commit -m "Test ray distance limits and hit ordering"
```

For the prop strip task, an example commit is:

```bash
git add native/testbench/assets/alima/props.png native/testbench/main.cpp
git diff --cached
git commit -m "Render three original props in the OpenGL demo"
```

Stage the PNG artwork if it is part of the feature, but leave build directories, captured screenshots, demo output, and credentials out of commits. Attach evidence to the PR.

## Push and open a pull request

Push your current task branch, never `master` directly:

```bash
git push -u origin HEAD
```

If you don't have write access, fork the repository using your account. Keep `origin` pointing at the team repository and add the fork as another remote. Replace `YOUR-LOGIN` with your actual GitHub login:

```bash
git remote add my-fork https://github.com/YOUR-LOGIN/TheLastStudent.git
git push -u my-fork HEAD
```

Open the repository in GitHub, choose **Pull requests → New pull request**, set the base to the team's `master` and compare it with your task branch. For a fork, use **compare across forks**. Fill out the template with the task ID, a short description, test results and the screenshots or logs named by the card. Ask a teammate to review; don't approve your own work.

The reviewer reads **Files changed**, checks that the change stays within scope, and tries the stated test. The author makes fixes on the same branch and pushes them. A teammate with merge rights merges after approval and passing checks. Start the next task from updated `master`, not the previous task branch.

## Resolve a simple conflict

When another PR changes the same area, stay on your feature branch and bring in the latest base:

```bash
git status --short
git fetch origin
git merge origin/master
git status --short
```

If Git marks `native/testbench/main.cpp` as conflicted, open it and look for `<<<<<<<`, `=======` and `>>>>>>>`. Read both edits: for example, keep a teammate's torch preset and your timing change if both are needed. Remove the markers, save, rebuild and rerun smoke. Then:

```bash
git add native/testbench/main.cpp
git diff --cached
git commit -m "Merge master and keep both testbench changes"
git push
```

If you need to back out of an in-progress merge, use `git merge --abort`. Begin merges with a clean worktree and ask a reviewer when the correct combined behaviour is unclear. Avoid force pushes. To reduce conflicts up front, use each card's named edit point and do not reformat unrelated code.
