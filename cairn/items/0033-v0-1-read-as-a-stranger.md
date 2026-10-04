---
id: 33
uid: 99504f69-1376-464f-86a0-4f2789f9ee9e
title: v0.1, read as a stranger
type: verify
status: planned
milestone: v0.1
depends_on:
- 8
- 9
- 10
- 11
- 12
- 13
- 14
- 15
- 16
- 17
- 18
- 19
- 20
- 21
- 22
- 23
- 24
- 25
- 26
- 27
- 28
- 29
- 30
- 31
- 32
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: inspector
area: verification
effort: m
---

## The claim

At this milestone the repository builds, runs, and makes exactly the claims
it has earned and no others.

## Judged against

The house rules in `CLAUDE.md`, and a clean clone.

## How it is checked

The inspector, in a fresh worktree with no history of the work, follows
the README literally. Every quoted figure is reproduced. Every header is
read for its opening argument and for a typed constant that should have
been derived. `./laporte verify` shows no unexpected failures and no
expected failures for items in this milestone. The owner reads the result
and tags the release.

## What failure looks like

A figure in prose the program no longer prints; a check that is still
marked expected-to-fail for work that has landed; a literal in a header
with no ledger handle beside it.

## Acceptance criteria

- [ ] Every figure reproduced
- [ ] Findings filed as items, not fixed in the review
- [ ] The owner has tagged `v0.1`
