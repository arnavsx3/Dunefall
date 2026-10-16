#!/usr/bin/env bash
# Install pinned build tools in this checkout without system-wide changes.
set -euo pipefail

project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$project_root"

if ! command -v c++ >/dev/null 2>&1; then
  echo "A C++20 compiler is required. On Ubuntu: sudo apt install build-essential" >&2
  exit 1
fi
if ! command -v uv >/dev/null 2>&1; then
  echo "Install uv first: https://docs.astral.sh/uv/getting-started/installation/" >&2
  exit 1
fi
if ! command -v python3 >/dev/null 2>&1; then
  echo "Python 3 is required to install the build tools." >&2
  exit 1
fi

uv venv --python python3 --no-managed-python --allow-existing .tools/venv
uv pip sync --python .tools/venv/bin/python --require-hashes tools/requirements.lock

.tools/venv/bin/cmake --version
.tools/venv/bin/ninja --version
.tools/venv/bin/clang-format --version
printf '\nDeveloper tools are ready in %s/.tools/venv/bin\n' "$project_root"
