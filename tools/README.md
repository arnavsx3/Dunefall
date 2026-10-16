# Developer toolchain

`requirements.in` declares supported versions of CMake, Ninja, and clang-format.
`requirements.lock` pins exact releases and package hashes. Python and uv install
these developer tools; neither is part of the C++ application runtime.

From the repository root, run `./scripts/bootstrap.sh`. Tools are installed in
`.tools/venv/`, which is ignored by Git. No system packages or shell profiles are
modified. Do not commit the environment or generated build files.
