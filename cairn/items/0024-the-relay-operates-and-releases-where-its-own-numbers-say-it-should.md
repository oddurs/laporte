---
id: 24
uid: fa2c3544-4216-4e81-bfe4-55343b7be48d
title: The relay operates and releases where its own numbers say it should
type: verify
status: planned
milestone: v0.1
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

A relay operates when the current in its coil reaches its operate value,
releases when it falls below its release value, and takes the time its
coil's L/R says it takes to get there. A slow-release relay holds for the
time its copper slug says.

## Judged against

Closed form for the timing (`t = −τ ln(1 − I_op/I_final)`), and the ledger
for the operate and release currents of a typical line relay of the era.

## How it is checked

Step a voltage onto the coil; measure the tick the contacts change.
Remove it; measure again. Repeat for the slow-release variant.

## What failure looks like

A relay that chatters at its threshold: operate and release currents equal,
so there is no hysteresis, so noise near the threshold toggles it every
tick.

## Acceptance criteria

- [ ] Operate and release times within one tick of closed form
- [ ] No chatter with the coil current held at the threshold
