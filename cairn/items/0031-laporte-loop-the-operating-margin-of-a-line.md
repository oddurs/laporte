---
id: 31
uid: 2a25666a-7d1d-4ec0-ab09-655a0c595d93
title: './laporte loop: the operating margin of a line'
type: instrument
status: planned
milestone: v0.1
labels:
- derivation
depends_on:
- 21
- 29
- 30
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: switching
area: instrument
effort: m
---

## What it witnesses

One line, over a range of cable lengths and gauges.

## What it prints

For each length: loop resistance, loop current, whether the line relay
operates and how fast. Then the derived limit for each gauge. The same
"everything above was specified, everything below came out" divider as
`windsor spec`.

## What it may not do

Compute the answer analytically. It builds the office at each length and
lifts the handset.

## Acceptance criteria

- [ ] The loop-limit check passes
- [ ] Output is deterministic and quotable in the README
