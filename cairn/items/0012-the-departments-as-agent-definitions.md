---
id: 12
uid: 2268337d-eac7-4173-a5ed-ca232014498b
title: The departments, as agent definitions
type: chore
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 11
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: build
effort: s
---

Turn the roles in `AGENTS.md` into `.claude/agents/<role>.md`, one per
department: `systems`, `switching`, `transmission`, `test`, `writer`,
`inspector`. Each runs on Sonnet.

Each definition holds: the department's brief (what it owns, what it may
never do), the loop from `AGENTS.md` it follows, and the tools it needs.
The inspector gets no `Write` or `Edit`: it reads, runs, and writes notes
into cairn, and nothing else.

The definitions do not restate `CLAUDE.md`; every agent reads that anyway.
They say only what is different about being in that department.

## Acceptance criteria

- [ ] Six definitions, each under a page
- [ ] The inspector cannot edit files
- [ ] A dry run: the director hands a trivial chore to `systems` in a
      worktree, the inspector reviews it, and it lands
