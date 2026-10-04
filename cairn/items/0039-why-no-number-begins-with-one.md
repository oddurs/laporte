---
id: 39
uid: e614603e-cf5f-4837-92fd-4ffa45f1528a
title: Why no number begins with one
type: verify
status: backlog
milestone: v0.2
labels:
- derivation
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p2
role: test
area: verification
effort: s
---

## The claim

A single short break in the loop — a handset jostled in its cradle, a hook
bumped while picking up — raises the selector to level one. So level one
cannot begin a number.

## Judged against

The mechanism alone. If the ledger has a source that gives this as the
reason level one was avoided, cite it; if not, the README says this is the
mechanism's argument, not history's.

## How it is checked

Lift with a bounce: a break of 30, 50, 80 ms. Report the shortest break
that steps the shaft.

## Acceptance criteria

- [ ] The shortest break that dials one, printed; expected to fail until the selector lands
