# Development

## Requirements

- Linux with Bash 4+ and a C++20 compiler (GCC or Clang).
- Python 3 and uv to install developer tools.
- Git and access to `arnavsx3/Dunefall` to push changes.

The current checkout uses GCC 15.2.0. CMake 4.4.4, Ninja 1.13.2, and clang-format
23.1.3 are pinned in `tools/requirements.lock`. A game framework is not required.
Python installs the developer tools; the built executable runs independently.

## First setup

From the repository root:

```bash
./scripts/bootstrap.sh
./scripts/dev.sh check
./scripts/dev.sh run --version
```

Bootstrap uses the existing Python interpreter and uv. It installs tools inside
`.tools/venv/` with package hash verification and does not require administrator
access. Re-run it after the lock file changes. The wrapper prepends local tools
to PATH for its own process; it does not change your shell profile.

If the compiler is missing on Ubuntu, install `build-essential` using your system
package manager. For uv installation, follow its
[official instructions](https://docs.astral.sh/uv/getting-started/installation/).
The bootstrap script reports missing prerequisites instead of changing them.

## Build configurations

| Preset | Purpose | Executable |
| --- | --- | --- |
| `debug` | Day-to-day work, debug information, warnings as errors | `build/debug/bin/dunefall` |
| `release` | Optimized build | `build/release/bin/dunefall` |
| `sanitize` | Debug with AddressSanitizer and UndefinedBehaviorSanitizer | `build/sanitize/bin/dunefall` |

```bash
./scripts/dev.sh build
./scripts/dev.sh test
./scripts/dev.sh check release
./scripts/dev.sh check sanitize
./scripts/dev.sh run --help
```

`run` uses the existing Debug executable; build it again after changing source.
The sanitizer preset currently supports GCC and Clang. Compile commands for
editor integration are generated inside each preset's build directory.

To use CMake directly:

```bash
export PATH="$PWD/.tools/venv/bin:$PATH"
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

## Formatting

```bash
./scripts/dev.sh format
./scripts/dev.sh format-check
```

The project uses four-space C++ indentation and a 100-column clang-format style.
Formatting checks cover C++ files under `src/`, `include/`, and `tests/`.

## Updating developer tools

Edit the version bounds in `tools/requirements.in` if needed, then regenerate the
lock and reinstall:

```bash
uv pip compile tools/requirements.in --generate-hashes \
  --output-file tools/requirements.lock --no-header --no-annotate
./scripts/bootstrap.sh
./scripts/dev.sh check
./scripts/dev.sh check release
./scripts/dev.sh check sanitize
```

Commit the inputs and hashed lock together. Update documented versions and verify
new tool behavior before pushing. CI uses the same bootstrap and check commands.

## Current verification

CTest runs eleven checks covering help, version, CLI errors, and the gameplay modules.
Native physics and input tests plus pseudo-terminal tests cover the playable
app, resize, exceptions, backpressure, and signal cleanup.
GitHub Actions runs formatting, builds, and CTest on all three presets.

## Headless snapshots and benchmarks

These modes work without an interactive terminal and emit no ANSI escapes:

```bash
./scripts/dev.sh run --headless --seed 7 --width 40 --height 16 --steps 120
./scripts/dev.sh build release
./build/release/bin/dunefall --benchmark --width 120 --height 44 --steps 1000
```

Snapshots use `.` for empty, `s` for sand, `w` for water, and `#` for walls.
The same seed, dimensions, scene, and step count reproduce the same snapshot.
Dimensions and step count flags require `--headless` or `--benchmark`. Width is
limited to 512, height to 256, and step count to 100000. Benchmarks measure only
physics, excluding scene construction, printing, and terminal rendering.
