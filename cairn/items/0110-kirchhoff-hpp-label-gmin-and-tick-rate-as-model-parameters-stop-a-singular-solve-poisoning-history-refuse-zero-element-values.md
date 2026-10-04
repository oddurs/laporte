---
id: 110
uid: 56d9a83c-cef0-4b86-a810-12a7a683677f
title: 'kirchhoff.hpp: label gmin and tick_rate as model parameters, stop a singular solve poisoning history, refuse zero element values'
type: bug
status: planned
milestone: v0.1
created: 2026-10-03
updated: 2026-10-03
priority: p2
role: systems
area: circuit
effort: s
---

## What the inspector found (PR #16, non-blocking)

1. `gmin{1e-12}` and `tick_rate{48000}` are bare literals in `include/` with no note that they are specified model parameters or that the justification lives in items 0013/0014 and the circuits check's 1e-3 V leakage bound on a 10 Mohm divider. Rule 2: say next to the number why it is what it is.
2. A singular solve writes NaN into the element history, and it survives every later tick, even after a contact change. A singular solve should fail loudly, not poison state.
3. Raw `double` sits beside the unit types at the public boundary.
4. Zero element values are not refused.
5. `<utility>` is used but not included directly.

## Reproduction

See the inspector's comment on PR #16.
