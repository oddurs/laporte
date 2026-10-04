---
id: 19
uid: a284bc2e-6fe1-49b4-9e34-2939777c64db
title: 'kirchhoff.hpp: the office is one netlist'
type: apparatus
status: planned
milestone: v0.1
labels:
- foundation
- thesis
depends_on:
- 13
- 14
- 15
- 17
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: circuit
effort: l
---

## What it is

The netlist and its solver, as the solver spike decided. Nodes are names.
Elements are resistors, capacitors, inductors, voltage sources, and
contacts. Each element knows its two nodes and nothing else.

Named for Gustav Kirchhoff, whose two laws (1845) are the entire content
of the solver; the header credits Ho, Ruehli and Brennan for the
formulation, and Nagel for `GMIN`.

## What it must derive

Every voltage and every current in the office, from the elements and
nothing else.

## What it may touch

Everything, because it is the wire.

## What is not modelled

Every element is lumped. A pair of copper wires several kilometres long
is a transmission line with capacitance and inductance per metre; here it
is a resistor. The header says so, says what it costs (no loading coils,
no line capacitance distorting dial pulses, no echo from impedance
mismatch on a loop), and points at the `later` item that would fix it.

## Acceptance criteria

- [ ] Every circuit in the solver check passes
- [ ] Apparatus is constructed from node names only; a part holding a
      pointer or reference to another part does not compile
- [ ] The header opens with the argument for one netlist, not a summary of the code
