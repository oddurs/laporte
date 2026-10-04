---
id: 20
uid: d61072c7-1e83-4f02-a3be-c155ee08be36
title: 'clock.hpp: solve, then let everything act on what it measured'
type: apparatus
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 18
- 19
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: circuit
effort: m
---

## What it is

The office's heartbeat. Each tick has two phases: the netlist is solved
with every element in the state it held at the end of the last tick; then
every element advances — a relay armature moves, a shaft steps, a
capacitor's history updates — based only on what it measured in the
solve. No element sees another's new state until the next tick.

Time is a tick count. There is no wall clock in `include/` at all; real
time appears once, in v0.6's platform edge, and drives this from outside.

## What it must derive

Order-independence, by construction rather than care.

## What it may touch

The netlist, and the list of elements that have state.

## What is not modelled

Anything faster than one tick. A contact that bounces for 50 µs bounces
for zero ticks; the header names what that hides.

## Acceptance criteria

- [ ] The determinism check passes
- [ ] The eight dynamic checks in `apps/checks/circuits.hpp` pass (RC, RL, AC at 20 Hz / 2.6 kHz / 3.4 kHz, the switched coil)
- [ ] `grep -r chrono include/` is empty
