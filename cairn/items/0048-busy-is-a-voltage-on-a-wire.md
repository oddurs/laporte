---
id: 48
uid: 535e15f4-53a3-4b63-b0d4-7d3f46b46f62
title: Busy is a voltage on a wire
type: verify
status: backlog
milestone: v0.3
labels:
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

Calling a subscriber who is already on a call returns busy tone, at the
tone plan's cadence, and the only thing that decided it was the potential
on the called line's sleeve.

## Judged against

The busy cadence in the ledger.

## Acceptance criteria

- [ ] Busy tone at the right cadence; expected to fail until the connector lands
