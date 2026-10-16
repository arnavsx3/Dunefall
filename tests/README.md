# Tests

Run `./scripts/dev.sh check` for formatting, a Debug build, and all eleven CTest
checks. Use the `release` and `sanitize` presets for optimized and instrumented runs.
No third-party test framework is required.

- `simulation/world_tests.cpp`: bounds, gravity, collisions, conservation,
  deterministic replay, reset, water flow, sinking, containment, and tiny worlds.
- `terminal/input_tests.cpp`: fragmented escape sequences, arrows, modifiers,
  shortcuts, uppercase keys, and ignored input.
- `terminal/renderer_tests.cpp`: viewport bounds, minimum terminal size,
  read-only rendering, compact help, caching, and output-volume measurement.
- `app/clock_tests.cpp`: fractional time, bounded catch-up, pause, and negative durations.
- `app/cli_test.py`: errors, seed limits, noninteractive safety, snapshots,
  deterministic replay, conservation, and benchmark output.
- `terminal/session_test.py`: actual pseudo-terminal restoration after normal exit,
  exceptions, output pressure, resize, SIGINT, SIGTERM, SIGHUP, and SIGQUIT.
- `terminal/game_test.py`: actual gameplay input, painting, erasing, brush sizes,
  pause with no redraws, single-step, help, clear, demo, resize, and safe shutdown.

Four small direct CLI checks also cover help, version, invalid options, and
rejection of interactive gameplay without a terminal. Python is a test dependency;
the game itself does not use a Python runtime.
