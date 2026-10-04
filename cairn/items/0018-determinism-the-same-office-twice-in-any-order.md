---
id: 18
uid: f1b0ad24-8e3b-4744-9e81-98cf0597c5ae
title: 'Determinism: the same office twice, in any order'
type: verify
status: planned
milestone: v0.1
labels:
- foundation
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

A scenario produces bit-identical output every time it runs, on any
machine, whatever order the apparatus was added to the netlist in.

## Judged against

Itself, run twice. The second run builds the office with its elements
shuffled by a fixed permutation.

## How it is checked

Hash every node voltage on every tick of a v0.1 scenario (lift, wait,
replace). Compare the hashes.

## What failure looks like

An element that advances its state during the solve rather than after it,
so that what it does depends on who was solved first. That is the bug the
two-phase tick exists to make impossible, and this is how we find out it
did not.

## Acceptance criteria

- [ ] Identical hashes in either order; expected to fail until the tick lands
