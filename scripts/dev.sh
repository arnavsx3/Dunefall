#!/usr/bin/env bash
set -euo pipefail

project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$project_root"
export PATH="$project_root/.tools/venv/bin:$PATH"

if [[ ! -x .tools/venv/bin/cmake || ! -x .tools/venv/bin/ninja || ! -x .tools/venv/bin/clang-format ]]; then
  echo "Developer tools are missing. Run ./scripts/bootstrap.sh first." >&2
  exit 1
fi

command_name="${1:-help}"
if [[ $# -gt 0 ]]; then shift; fi

format_sources() {
  local mode="$1"
  local -a sources=()
  mapfile -d '' sources < <(find src include tests -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0)
  if [[ ${#sources[@]} -gt 0 ]]; then
    if [[ "$mode" == check ]]; then
      clang-format --dry-run --Werror "${sources[@]}"
    else
      clang-format -i "${sources[@]}"
    fi
  fi
}

case "$command_name" in
  build|test|check)
    preset="${1:-debug}"
    case "$preset" in
      debug|release|sanitize) ;;
      *) echo "Unknown build preset: $preset" >&2; exit 2 ;;
    esac
    if [[ "$command_name" == check ]]; then format_sources check; fi
    cmake --preset "$preset"
    cmake --build --preset "$preset"
    if [[ "$command_name" != build ]]; then ctest --preset "$preset"; fi
    ;;
  run)
    if [[ ! -x build/debug/bin/dunefall ]]; then
      echo "Build first: ./scripts/dev.sh build" >&2
      exit 1
    fi
    exec build/debug/bin/dunefall "$@"
    ;;
  format) format_sources write ;;
  format-check) format_sources check ;;
  help|--help|-h)
    cat <<'USAGE'
Usage: ./scripts/dev.sh COMMAND [PRESET or application options]

  build [debug|release|sanitize]  Configure and build (default: debug)
  test  [debug|release|sanitize]  Configure, build, and run CTest
  check [debug|release|sanitize]  Check formatting, build, and run CTest
  run [--help|--version]          Run the debug executable
  format                         Format C++ sources in place
  format-check                   Check C++ formatting without editing
USAGE
    ;;
  *) echo "Unknown command: $command_name. Run ./scripts/dev.sh help." >&2; exit 2 ;;
esac
