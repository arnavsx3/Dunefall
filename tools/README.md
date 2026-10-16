# Developer toolchain

`requirements.in` declares supported versions of CMake, Ninja, and clang-format.
`requirements.lock` pins exact releases and package hashes. Python and uv install
these developer tools; neither is part of the C++ application runtime.

From the repository root, run `./scripts/bootstrap.sh`. Tools are installed in
`.tools/venv/`, which is ignored by Git. No system packages or shell profiles are
modified. Do not commit the environment or generated build files.

## Demo recorder

After building Release, run `python3 tools/record_demo.py` from the repository
root. It drives the actual C++ executable through a pseudo-terminal and saves
`docs/assets/demo.cast` (asciinema v2) and `docs/assets/demo.svg` (a captured frame).
It checks terminal restoration and uses only Python's standard library. Preview
playback through an asciinema-compatible viewer is optional; no player is needed
to build or run the game.
