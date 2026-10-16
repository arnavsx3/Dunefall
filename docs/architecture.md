# Architecture

## Direction

Dunefall is a C++20 terminal application. Linux is the first supported runtime.
Keep the simulation independent of the terminal so that physics can be tested
without launching an interactive session.

The playable application composes the simulation and terminal libraries. Physics
advances at 30 fixed steps per second; rendering runs at up to 30 frames per
second. A 100 ms catch-up cap prevents runaway work after slow output. Pause,
help, and undersized terminals freeze the simulation. Resizing preserves the
world and crops the viewport around the cursor.

## Modules

| Area | Responsibility | Dependencies |
| --- | --- | --- |
| `src/app/` | CLI, composition, simulation loop, user actions | Simulation and terminal interfaces once implemented |
| `src/simulation/` | Grid, materials, collision and movement rules | C++ standard library only |
| `src/terminal/` | Terminal lifetime, keyboard events, ANSI output, resize | C++ standard library and POSIX terminal APIs |
| `include/dunefall/` | Small interfaces shared between modules | Avoid exposing operating-system details |
| `tests/` | Automated noninteractive checks | CTest; simulation test targets as needed |

The application owns the world and terminal session. Input becomes explicit
application actions; the simulation advances on a fixed timestep; the renderer
receives a read-only world view. Rendering frequency must not change physics.

## Simulation approach

Use a contiguous grid with explicit bounds and empty, wall, sand, and water
materials. Update from the bottom upward, with controlled directional variation
to avoid a persistent left or right bias. Use a reproducible random seed and
prevent a particle from moving more than once in a simulation step.

Water tries gravity, diagonals, then one horizontal cell. Sand swaps with water
to sink; both particles are stamped so neither can move twice in one tick. Tests
cover bounds, movement priority, containment, conservation, and seeded replay.

## Terminal approach

Use POSIX terminal input and ANSI output rather than introducing a game framework.
A terminal-session object will restore terminal state and cursor visibility on
normal exit, exceptions, and supported termination signals. Keep signal handlers
minimal: request shutdown and let the normal control flow perform restoration.
Do not promise recovery from uncatchable signals such as SIGKILL.

Check for an interactive terminal before enabling raw input. Handle terminal
resize explicitly, cap catch-up work after long pauses, and avoid an unbounded
busy loop. Defer these features until the terminal milestone.

## Build boundaries

CMake builds `dunefall_simulation`, `dunefall_terminal`, `dunefall_app`, and the `dunefall` executable. Add simulation
and terminal libraries when real code exists for them, rather than empty targets.
The shared options target sets C++20, compiler warnings, and optional sanitizers.
Build artifacts and tooling environments never belong in source control.
