# Roadmap

## 0. Initial workspace

- [x] Separate application, simulation, terminal, interfaces, tests, and docs.
- [x] Install pinned developer tools inside the checkout.
- [x] Configure Debug, Release, and sanitizer builds.
- [x] Add an honest executable scaffold and CLI smoke checks.
- [x] Add formatting rules, a contributor workflow, and GitHub Actions.

Stop here for the initial setup request. The game is not implemented in this milestone.

## 1. Particle world and sand

- [ ] Introduce grid dimensions, cells, and empty/wall/sand materials.
- [ ] Add safe access, placement, and deterministic simulation steps.
- [ ] Implement falling and diagonal sand movement.
- [ ] Test edges, walls, movement order, and particle conservation.

Done when a seeded world evolves reproducibly in noninteractive tests, with no
out-of-bounds access or more than one movement per particle per step.

## 2. First playable terminal sandbox

- [ ] Add a terminal-session lifecycle with reliable restoration.
- [ ] Render the world using ANSI colors and a compact status bar.
- [ ] Add cursor movement, sand painting, wall placement, and quit.
- [ ] Introduce fixed simulation timing and terminal resize handling.

Done when a user can build a wall and pile sand against it, then exit with the
terminal settings and cursor restored. Verify exit and resize in a real terminal.

## 3. Water and world controls

- [ ] Add water gravity, lateral flow, and defined sand/water interactions.
- [ ] Add material selection, erase, pause/resume, and world reset.
- [ ] Cover containment, flow, and mixed-material conservation with tests.

Done when water flows around a sand pile and stays contained by walls, and
pause/reset work predictably without losing terminal control.

## 4. Polish

- [ ] Measure representative world sizes and reduce unnecessary redraws.
- [ ] Add a useful help overlay and finalize documented controls.
- [ ] Add a short demo recording and playable build instructions.
- [ ] Validate graceful shutdown, small terminals, and piped input behavior.

Defer extra materials, save files, and cross-platform terminal backends until the
small sandbox is stable and enjoyable.
