---
id: 17
uid: 47ef78f7-0f89-475a-bb0c-960528c885e8
title: Kirchhoff, checked against circuits whose answers are known
type: verify
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 14
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

The netlist solves circuits correctly, and the clock does not distort the
signals the office carries beyond the bound the clock spike set.

## Judged against

Closed-form solutions, not published figures: a resistive divider; an RC
step response against `1 − e^(−t/RC)`; an RL against the same; an LC
driven at 20 Hz, 2.6 kHz and 3.4 kHz against the analytic steady state; a
switched contact that opens an inductive circuit (the solver must not
blow up, and the spike's chosen method must not ring).

## How it is checked

Each circuit is a netlist in the check's own file; the tolerance for each
is written before the solver exists.

## What failure looks like

A divider off by `GMIN`'s leakage; a step response that is right in shape
and wrong in time constant by one tick (an off-by-one in the companion
model); a 3.4 kHz amplitude several dB low (numerical damping).

## Acceptance criteria

- [ ] Every circuit above, written and expected to fail until the solver lands
