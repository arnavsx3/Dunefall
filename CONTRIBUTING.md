# Contributing

Keep changes small, reviewable, and connected to the roadmap.

1. Check `git status` before editing and preserve unrelated local changes.
2. Make one cohesive change at a time.
3. Run `./scripts/dev.sh check` and relevant additional checks before committing.
4. Review `git diff --check`, the diff, and the staged file list.
5. Commit with a descriptive subject, for example `feat(simulation): add sand gravity`.
6. Push the verified commit. The owner's current workflow uses `main` directly;
   pull requests are also supported for outside contributions.

Use `chore`, `build`, `ci`, `docs`, `feat`, `fix`, or `test` prefixes when helpful.
Keep tool upgrades separate from gameplay changes. Avoid formatting unrelated
files. Never commit generated builds, `.tools/`, credentials, or unfinished
experiments. Do not amend or force-push published history without explicit owner
authorization.

Add behavior-focused tests with simulation work, especially for grid boundaries,
conservation, and deterministic updates. Interactive terminal changes also need
manual verification of terminal restoration.
