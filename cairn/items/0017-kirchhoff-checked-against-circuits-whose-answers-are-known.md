---
id: 17
uid: 47ef78f7-0f89-475a-bb0c-960528c885e8
title: Kirchhoff, checked against circuits whose answers are known
type: verify
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- foundation
depends_on:
- 14
- 16
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
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

- [x] Every circuit above, written and expected to fail until the solver lands

## 2026-10-03

Written: apps/checks/circuits.hpp, ten registered checks (divider, gmin, rc_step, rc_tau, rl_step, rl_tau, ac_amplitude, ac_phase, switch_kick, switch_ring) plus circuits.detector, all judged against closed-form oracles in the file. Markers: divider and gmin until 19 (netlist and stamping); every dynamic check until 20 (history is state the tick advances; the eight backward-Euler ticks are the clock's rule). Consequence for the director: item 19's criterion 'every circuit in the solver check passes' cannot be met by 19 alone, only once 20 lands. Bounds: item 14's 0.5 dB / 5 deg, reused; the honest fake lands on item 14's own 48 kHz cells (0.134 dB, 1.77 deg). Step bound 0.75 tick is derived, not from the spike (item 14 measured only steady state): trapezoid is half a tick late by construction, one tick is the off-by-one; tau from the tail within 1%. Switch: kick <= 1.5 L*I0/h, ring < 0.05 V from tick 9 to 600; the fake shows 8 BE ticks give 2.7e-5 V and 4 give 0.1 V, so the bound separates 4 from 8 by 2x only. The detector runs a small in-file MNA fake against honest/BE-only/off-by-one/late/no-GMIN/GMIN too big/zero/NaN/wrong-rate/0,1,2,4 BE ticks. Found while writing: std::max drops a NaN silently (fixed, detector catches it), and Apple clang loses a double member of a class-type NTTP (gmin is therefore an exponent in the fake). Hook-up is in the header: two lines for 19, one for 20; apparatus authors may not edit the file. The checks are expected to fail because the apparatus is not built, which satisfies 'written, expected to fail'.

## Result

Ten circuit checks with closed-form oracles registered, expected to fail until 19 (divider, GMIN) and 20 (dynamic); circuits.detector proves what each catches. Inspector PASS; weak ring bound and header misquotes filed as a bug.
