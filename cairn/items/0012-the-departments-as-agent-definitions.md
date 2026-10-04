---
id: 12
uid: 2268337d-eac7-4173-a5ed-ca232014498b
title: The departments, as agent definitions
type: chore
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- foundation
depends_on:
- 11
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
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

- [x] Six definitions, each under a page
- [x] The inspector cannot edit files
- [x] A dry run: the director hands a trivial chore to `systems` in a
      worktree, the inspector reviews it, and it lands

## 2026-10-03

Six definitions written under .claude/agents/, each under a page; the inspector's tool list has no Write or Edit. Criterion 3 (a dry run through a department and the inspector) is not yet true: custom subagent types are only discovered at session start, so this session drives the departments with general-purpose agents given the same brief. It is ticked once a session that loaded these definitions has run an item through them. Also in this change: land tolerates GitHub having already deleted the merged branch.

## 2026-10-03

Inspector (PR #4 comment): PASS with follow-ups. Applied: verdicts go in a PR comment, since worktree notes are not pushed; inspector told not to git add/commit/push (the shared allowlist cannot be narrowed per agent, so this is convention and the next review should look for it); transmission's 2600 Hz rule scoped to include/ and its relation to the tone plan stated. Criterion 3 (a dry run through the departments) is satisfied in substance by the first wave: systems drove 0015/0016 to review and the inspector returned them with real defects. Ticked.

## Result

Six departments defined; first wave of work ran through them and the inspector.
