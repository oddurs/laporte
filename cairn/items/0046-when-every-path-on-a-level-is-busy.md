---
id: 46
uid: 61f7c88d-8281-40f6-8d3f-d985584613d9
title: When every path on a level is busy
type: verify
status: backlog
milestone: v0.3
labels:
- derivation
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

A selector that finds every connector on a level busy rides past the last
one to an eleventh position and returns busy tone — "all circuits busy" —
and nothing anywhere decides that except the sleeves.

## How it is checked

Two calls in progress to level 4; a third caller dials 4. Read the
selector's rotary position and the tone on the caller's pair.

## Acceptance criteria

- [ ] Eleventh step, busy tone; expected to fail until the rotary motion lands
