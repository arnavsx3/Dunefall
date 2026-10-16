# Dunefall working instructions

- This is a C++20 terminal sandbox. Read `docs/architecture.md` and
  `docs/roadmap.md` before implementing a milestone.
- Keep application coordination, simulation, and terminal code separate. The
  simulation must remain testable without terminal I/O.
- Inspect Git status before editing. Preserve unrelated user changes and stage
  only files belonging to the current task.
- Use `./scripts/bootstrap.sh` for local tools and `./scripts/dev.sh check` for
  formatting, build, and smoke checks. Use the Release and sanitizer presets
  when native-code changes warrant them.
- Keep commits focused and descriptive. The owner has authorized committing and
  pushing each verified chunk to `origin/main`; do that unless a later instruction
  changes the workflow. Never force-push or rewrite published history without
  separate explicit authorization.
- Do not commit `build/`, `.tools/`, credentials, or generated artifacts.
- Update docs when behavior, setup, or controls change. State clearly which
  features exist and which are planned.
- Stay within the current requested milestone. Do not add deferred
  materials, save files, or terminal backends without a new request.
