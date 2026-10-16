# Tests

CTest currently runs executable smoke checks for noninteractive rejection, help, version, and
invalid arguments. No third-party test framework is required for this scaffold.

When simulation work begins, add focused tests for grid bounds, conservation of
particles, collisions, water flow, and deterministic updates. Tests must run
without an interactive terminal. Terminal integration tests should exercise
restoration and resize behavior separately.

Input decoding has native unit tests. `terminal/session_test.py` uses Python's
standard-library pseudo-terminal support to verify the C++ session's restoration
on normal exit, exceptions, and shutdown signals. Python remains a test-only
dependency; the game itself is C++.

`terminal/game_test.py` drives the playable app: painting, brush sizes, pause,
help, clear, demo reload, resize, and safe shutdown.
