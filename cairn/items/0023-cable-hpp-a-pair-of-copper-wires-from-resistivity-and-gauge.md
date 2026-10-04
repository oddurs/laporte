---
id: 23
uid: 47cf5836-2d6b-4e4f-98a7-9dcb680f09fb
title: 'cable.hpp: a pair of copper wires, from resistivity and gauge'
type: apparatus
status: planned
milestone: v0.1
labels:
- derivation
depends_on:
- 15
- 19
- 22
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: cable
effort: s
---

## What it is

The outside plant: a twisted pair of a given gauge and length, as two
resistors (tip and ring) between the office's main frame and the house.

## What it must derive

Its resistance, from resistivity, the AWG diameter formula and the
length. Nobody types "83 ohms per kilofoot".

## What it may touch

Four nodes: tip and ring at each end.

## What is not modelled

Capacitance between the wires (about 0.08 µF per mile, to be sourced),
inductance, loading coils, temperature. These are what make long loops
distort pulses and attenuate speech, and they are the `later` item on
transmission lines; the README's loop-length figure is therefore a DC
figure and says so.

## Acceptance criteria

- [ ] The copper check passes
