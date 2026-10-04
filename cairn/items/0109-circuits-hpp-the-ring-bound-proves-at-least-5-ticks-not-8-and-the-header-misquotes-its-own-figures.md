---
id: 109
uid: 2b3c5f1e-4dcd-46ea-b220-55db3ff67a8a
title: 'circuits.hpp: the ring bound proves ''at least 5 ticks'', not 8, and the header misquotes its own figures'
type: bug
status: planned
milestone: v0.1
created: 2026-10-03
updated: 2026-10-03
priority: p2
role: test
area: verification
effort: s
---

## What the check does

`circuits.detector` accepts a ring peak under 0.05 V after the backward-Euler ticks. The inspector ran the in-file fake at 0 to 12 ticks: 4 ticks gives 0.100 V (fails), 5 gives 0.0129 V (passes), 8 gives 2.7e-5 V. So the check proves "at least 5 ticks", and the honest 8 sits about 1800x inside the bound.

## What it should do

Tighten the bound to about 1e-3 V (fails 5 and 6, keeps a ~37x margin over 8), and correct the header's quoted figures for four and two ticks (it says 0.32 V and 19 V; measured 0.10 V and 6.1 V).

## Also

The hook-up described for item 19 is more than two lines: the `Solver` concept requires `tick()`, `tick_hz()` and `set_contact()` even for the resistive netlist, so the divider stays at infinity until item 19 adds a clock accessor. Either relax the concept for the resistive checks or say so in the item.

## Reproduction

See the inspector's comment on PR #15.
