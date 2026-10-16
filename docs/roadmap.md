# Roadmap

## 0. Initial workspace

- [x] Separate application, simulation, terminal, interfaces, tests, and docs.
- [x] Install pinned developer tools inside the checkout.
- [x] Configure Debug, Release, and sanitizer builds.
- [x] Add an honest executable scaffold and CLI smoke checks.
- [x] Add formatting rules, a contributor workflow, and GitHub Actions.

Workspace setup is complete. All four gameplay milestones below are complete.

## 1. Particle world and sand

- [x] Introduce grid dimensions, cells, and empty/wall/sand materials.
- [x] Add safe access, placement, and deterministic simulation steps.
- [x] Implement falling and diagonal sand movement.
- [x] Test edges, walls, movement order, and particle conservation.

Done when a seeded world evolves reproducibly in noninteractive tests, with no
out-of-bounds access or more than one movement per particle per step.

## 2. First playable terminal sandbox

- [x] Add a terminal-session lifecycle with reliable restoration.
- [x] Render the world using ANSI colors and a compact status bar.
- [x] Add cursor movement, sand painting, wall placement, and quit.
- [x] Introduce fixed simulation timing and terminal resize handling.

Done when a user can build a wall and pile sand against it, then exit with the
terminal settings and cursor restored. Exit and resize are verified through
pseudo-terminal checks; an interactive terminal run also verified painting,
help, and Ctrl+C restoration.

## 3. Water and world controls

- [x] Add water gravity, lateral flow, and defined sand/water interactions.
- [x] Add material selection, erase, pause/resume, and world reset.
- [x] Cover containment, flow, and mixed-material conservation with tests.

Done when water flows around a sand pile and stays contained by walls, and
pause/reset work predictably without losing terminal control.

## 4. Polish

- [x] Measure representative world sizes and reduce unnecessary redraws.
- [x] Add a useful help overlay and finalize documented controls.
- [x] Add a short demo recording and playable build instructions.
- [x] Validate graceful shutdown, small terminals, and piped input behavior.

Defer extra materials, save files, and cross-platform terminal backends until the
small sandbox is stable and enjoyable.
