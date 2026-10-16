# Architecture

Dunefall is a C++20 terminal sandbox. Linux is the first supported runtime. The
simulation is independent of terminal I/O and can run headlessly.

## Modules

| Area | Responsibility | Dependencies |
| --- | --- | --- |
| `src/app/` | CLI, composition, brush actions, fixed-step loop, headless mode | Simulation and terminal interfaces |
| `src/simulation/` | Bounded grid, materials, movement rules, seeded demo | C++ standard library |
| `src/terminal/` | Session lifetime, input decoding, ANSI output, viewport | Standard library and POSIX APIs |
| `include/dunefall/` | Small interfaces shared between modules | Operating-system details stay in the terminal interface |
| `tests/` | Native, CLI, timing, and pseudo-terminal checks | CTest and Python standard library |

CMake builds three libraries (`dunefall_simulation`, `dunefall_terminal`, and
`dunefall_app`) plus the `dunefall` executable. First-party targets share C++20,
strict compiler warnings, and optional sanitizers.

## Simulation

The world is a contiguous grid with explicit bounds. Cells are empty, sand,
water, or wall. Bottom-up scans vary their horizontal direction using a seeded
`std::mt19937`. Decisions use engine bits directly for reproducible replay.

Sand tries down, then diagonals in seeded order. Water tries down, diagonals,
then adjacent horizontal cells. Sand can swap with water to sink; water cannot
displace sand or walls. Per-cell tick stamps prevent particles from moving twice,
including water pushed upward by sand. Walls and world edges contain particles.

Physics conserves each material. Editing is intentional: particles fill empty
cells, walls can replace particles, and the eraser can remove any cell. Clear and
demo reload reset the random engine and tick counter. Grid dimensions are bounded
to 512 by 256; the interactive app initially fits the terminal up to 120 by 44.

## Application timing

`FixedStep` accumulates elapsed time for 30 simulation steps per second. At most
three catch-up steps can run per frame, even with leftover time before a stall.
Rendering runs at up to 30 frames per second. The loop sleeps instead of spinning.

Pause, help, and undersized terminals discard accumulated time. Resuming never
tries to replay the time spent paused. A single-step action advances physics
once while paused. Painting and continuous spray are application actions; the
simulation does not know about brushes or keyboard events.

## Terminal lifecycle and rendering

`Session` checks terminal compatibility before changing state. It owns
noncanonical input, nonblocking reads, saved input flags, alternate-screen mode,
cursor visibility, and supported signal handlers. Normal exit, exceptions,
SIGINT, SIGTERM, SIGHUP, and SIGQUIT restore the saved state. Signal handlers only
request shutdown; ordinary control flow performs cleanup. SIGKILL cannot be handled.

Input decoding preserves partial CSI and application-mode arrow sequences across
reads. Both reads and catch-up work are bounded. Output handles partial writes
and backpressure without ignoring shutdown requests.

Each cell uses two terminal columns. Resizing preserves world dimensions and
crops a viewport around the cursor. Below 42 columns by 12 rows, physics pauses
and a resize message replaces the grid. Help fits the same minimum dimensions.

The renderer uses explicit light foreground colors on a dark background, groups
adjacent tiles by color, and caches rows. Only changed rows are emitted. Static
paused frames generate no output; resize and layout changes force a full redraw.

## Verification

Tests cover boundaries, collisions, one-move-per-step behavior, mixed-material
conservation, containment, reset and seeded replay, input sequences, viewport
clamping, cached output, bounded timing, and public CLI errors. Pseudo-terminal
checks drive the actual app and verify painting, erasing, pause, single-step,
help, resize, demo reload, exceptions, output pressure, and terminal restoration.
