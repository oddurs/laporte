---
id: 19
uid: a284bc2e-6fe1-49b4-9e34-2939777c64db
title: 'kirchhoff.hpp: the office is one netlist'
type: apparatus
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
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
closed_at: 2026-10-03
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

- [x] The two resistive checks in `apps/checks/circuits.hpp` (divider, GMIN) pass; the eight dynamic ones belong to item 20, because history and the backward-Euler ticks live in the clock
- [x] Apparatus is constructed from node names only; a part holding a
      pointer or reference to another part does not compile
- [x] The header opens with the argument for one netlist, not a summary of the code

## 2026-10-03

Delivered include/laporte/kirchhoff.hpp (plan: earlier note, lost in a rebase: one netlist, nodes by name, canonical stamping order, MNA, dense LU with partial pivoting, GMIN 1e-12 S, trapezoidal or backward Euler per tick, factorisation reused until a contact changes; not modelled: transmission lines, loading coils, line capacitance, contact bounce, winding capacitance, anything nonlinear). Not included by main.cpp yet; compiled alone under the strict flags. The coordinator reworded criterion 1 to the two resistive checks. In a scratch hook-up of circuits.hpp (reverted; the comment there gives the two-line hook-up to the test department or inspector) divider measured 6.9e-9 V and gmin 0.67 of bound, both within bound, so criterion 1 is left unticked until the hook-up lands and the stale markers are removed. The dynamic cells also match item 14 (ac 0.134 dB, 1.77 deg). switch_kick measures 2.0 x L*I0/h against a 1.5 bound and FAILS, and switch_ring 2304 V, until item 20's eight backward-Euler ticks; tick(Method) is the door for that. Decisions: a contact starts closed; Part, Kind and factories live in laporte::kirchhoff so the check's own resistor() is not ambiguous; Netlist accepts any Wired type, so a part with a pointer for a connection does not compile.

## 2026-10-03

Landed in review, NOT closed: criterion 1 (the two resistive checks pass) needs the circuits.hpp hook-up, which is the test department's to make, with the divider and gmin expected-failure markers removed in the same change. Inspector PASS with non-blocking notes, filed as a bug. 0020 and everything downstream waits on this item closing.

## 2026-10-03

Hook-up: apps/checks/circuits.hpp includes kirchhoff.hpp, Resistive = laporte::Netlist; until_item markers removed from divider and gmin only (dynamic checks stay marked 20). Netlist satisfies the Solver concept as is, no relaxation. Measured: divider 6.912e-09 V (bound 1e-6), gmin 0.667 x bound; detector 19/19; make strict && verify: 26 checks, 0 failures, 17 expected to fail.

## Result

kirchhoff.hpp: one netlist, nodes by name, MNA with dense LU and GMIN, factorisation reused while the matrix is unchanged; the two resistive circuit checks pass against the real solver. Inspector PASS twice. Non-blocking notes filed as a bug.
