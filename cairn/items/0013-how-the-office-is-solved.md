---
id: 13
uid: a8763418-64dc-4809-ba0c-cbb334444a5f
title: How the office is solved
type: spike
status: planned
milestone: v0.1
owner: oddurs
labels:
- decision
- foundation
- thesis
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: circuit
effort: m
---

## Question

How do the parts of the office learn what the wire is doing?

## Why it has to be answered first

House rule 1 — everything is the wire — is only true if there is a wire. The
representation chosen here is in the signature of every piece of apparatus
and is the most expensive decision in the project to change.

## Options

**One netlist, solved by modified nodal analysis** (recommended). The whole
office — battery, every relay coil, every contact, every pair of copper,
every telephone — is a list of two-terminal elements between named nodes.
Each tick, every element stamps its conductance (and, for capacitors and
inductors, a companion source carrying its history) into one matrix, the
matrix is solved, and every element reads the voltage across it and the
current through it. A closed contact is a small resistance; an open one is
not stamped. Floating nodes get SPICE's `GMIN`, a tiny conductance to
earth, and the reason is written down. Ho, Ruehli and Brennan published MNA
in 1975; Nagel's SPICE made it universal. Dense Gaussian elimination: an
office of a few dozen nodes is a matrix nobody needs to be clever about.

This is the only option in which "everything is the wire" is literally
true: apparatus is constructed with node names and nothing else, so it
cannot hold a reference to other apparatus, and the compiler enforces rule 1.

**Hand-written mesh equations per configuration.** Each state of the switch
train is a known small circuit; write its solution out. Fastest and most
readable for v0.1 — and wrong by v0.3, when contacts change the topology
in more ways than anyone will enumerate by hand.

**Message passing between components.** A phone tells the relay it is off
hook. Rejected: this is precisely the program the thesis forbids.

## What would settle it

A prototype of the netlist solving the v0.1 loop and the v0.3 transmission
bridge (two loops, AC-coupled through capacitors), at the clock rate the
clock spike chooses, faster than real time by at least 20× for a dozen
subscribers on one core.

## Answer
