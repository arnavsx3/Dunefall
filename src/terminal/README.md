# Terminal module (planned)

Own terminal mode, nonblocking keyboard input, ANSI rendering, resize handling,
and restoration on exit. The initial supported runtime is Linux with an
interactive ANSI-compatible terminal. Use POSIX terminal APIs and C++ RAII.

Keep operating-system details here. Render a read-only view of the simulation;
do not put particle rules in this module. See `docs/architecture.md`.
