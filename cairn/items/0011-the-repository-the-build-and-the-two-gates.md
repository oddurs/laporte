---
id: 11
uid: 832658b5-b98e-40a7-b411-25aba14b5909
title: The repository, the build, and the two gates
type: chore
status: doing
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

- [x] `make && ./laporte` builds and prints a usage line on a clean clone
- [x] A commit with a broken build is refused by the local gate
- [ ] CI runs on a pull request and fails on a warning under `-Werror`
- [ ] `.claude/propose` and `.claude/land` work end to end on a trivial PR

## 2026-10-03

Done by the director at bootstrap: git init, cairn init --git, Makefile (one translation unit, make strict), .claude/{settings.json,hooks/build-gate.sh,propose,land}, .github/workflows/ci.yml (gcc-14 linux, clang macos, absence greps, strict rebuild). main built green on both. Note for agents: a zsh/fish shell function shadows make — use /usr/bin/make; and a global git rule rewrites https://github.com/ to ssh, which hangs in this environment, so origin is https://oddurs@github.com/oddurs/laporte.git.

## 2026-10-03

Reopened by the director: criteria 2-4 were ticked before they were demonstrated. 1 holds (CI built main green on gcc and clang). 2 (gate refuses a broken build), 3 (CI fails on a warning under -Werror) and 4 (propose and land end to end) are demonstrated below or not at all.

## 2026-10-03

Criterion 2 demonstrated 2026-10-04: with a syntactically broken apps/broken.cpp in the tree, the gate exited 2 with 'Refusing the commit: the tree does not build', and in the live Claude Code harness refused the real command. Ticked. 3 and 4 remain: 4 is demonstrated by landing PR #1 through propose and land; 3 needs a deliberately warning-laden throwaway PR that CI must reject.
