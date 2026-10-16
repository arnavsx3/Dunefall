# Dunefall

A tiny world of sand, water, and beautiful chaos.

Dunefall is a terminal physics playground written in C++20. The goal is to
sprinkle sand, pour water, and build little worlds that react in real time.

**Current status:** the development workspace is ready. The executable is a
startup scaffold with `--help` and `--version`; particle physics, rendering,
and interactive controls are planned next.

## Get started

The initial development platform is Linux. You need a C++20 compiler, Bash 4+,
Python 3, and [uv](https://docs.astral.sh/uv/getting-started/installation/).
CMake, Ninja, and clang-format are installed locally by the bootstrap script.

```bash
git clone git@github.com:arnavsx3/Dunefall.git
cd Dunefall
./scripts/bootstrap.sh
./scripts/dev.sh check
./scripts/dev.sh run
```

Already cloned? Run the last three commands from your project folder.
No activation step is needed. The application itself has no Python runtime or
third-party library dependency.

## Workspace

```text
Dunefall/
├── .github/workflows/       # Build, formatting, and smoke checks on GitHub
├── docs/                   # Architecture, development guide, and roadmap
├── include/dunefall/        # Module interfaces
├── scripts/                # Tool bootstrap and developer commands
├── src/
│   ├── app/                # Entry point and application coordination
│   ├── simulation/         # Particle engine (planned)
│   └── terminal/           # Input, rendering, terminal lifecycle (planned)
├── tests/                  # CTest checks; future simulation tests
├── tools/                  # Pinned, hash-verified developer tool versions
├── CMakeLists.txt          # Build targets and compiler settings
└── CMakePresets.json       # Debug, Release, and sanitizer configurations
```

Generated files stay in ignored `build/` and `.tools/` directories.

## Everyday commands

```bash
./scripts/dev.sh check           # Formatting, Debug build, smoke tests
./scripts/dev.sh check release   # Release build and the same checks
./scripts/dev.sh check sanitize  # Debug build with ASan and UBSan
./scripts/dev.sh format          # Apply C++ formatting
./scripts/dev.sh run --help
```

## Plan

1. Build a deterministic grid and falling-sand rules.
2. Add terminal rendering, keyboard painting, walls, and a fixed simulation step.
3. Add flowing water, material selection, pause, and reset.
4. Polish resize handling, terminal restoration, performance, and instructions.

See the [roadmap](docs/roadmap.md), [architecture](docs/architecture.md),
[development guide](docs/development.md), and [contribution guide](CONTRIBUTING.md).
