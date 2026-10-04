---
id: 11
uid: 832658b5-b98e-40a7-b411-25aba14b5909
title: The repository, the build, and the two gates
type: chore
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 8
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: build
effort: m
---

Adapted from `cornell`, which has solved all of this already. Copy, then
rename; do not reinvent.

- `git init`; `cairn init --git` so items and `ROADMAP.md` merge themselves.
- `Makefile`: one translation unit, `apps/main.cpp`, everything else
  headers. `-std=c++23 -O2 -Wall -Wextra -Wpedantic`. `make` builds
  `./laporte`; `make strict` adds `-Werror`.
- `.claude/settings.json`, `.claude/hooks/build-gate.sh` (runs `make` and
  `cairn check` before any `git commit`), `.claude/propose`,
  `.claude/land` — from cornell, with the names changed.
- `.github/workflows/ci.yml`: build from clean on Linux/gcc and
  macOS/clang, run `./laporte verify`, build again with `-Werror`.
- Creating the GitHub repository is outward-facing: the owner approves it
  (see the naming spike) before it happens.

## Acceptance criteria

- [ ] `make && ./laporte` builds and prints a usage line on a clean clone
- [ ] A commit with a broken build is refused by the local gate
- [ ] CI runs on a pull request and fails on a warning under `-Werror`
- [ ] `.claude/propose` and `.claude/land` work end to end on a trivial PR
