---
id: 45
uid: ab214786-0668-47f8-ba4e-36ca0f092c08
title: 'The office as built: how many lines, which levels, which numbers'
type: spike
status: backlog
milestone: v0.3
owner: oddurs
labels:
- decision
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: frame
effort: m
---

## Question

What exactly is wired in `laporte.hpp` from v0.3 onwards?

## Why it has to be answered first

The selector, connector, ring-trip and call items all build against it, and
the netlist's size decides whether the solver is fast enough.

## Options

**Recommended:** three-digit numbers. Eight subscribers, on two hundreds
levels (say 3 and 4). Two connectors per level, so that three simultaneous
calls to one level exercise "all paths busy". Every subscriber owns a first
selector outright — as in the earliest offices — so that line finders,
which are a second hunting mechanism with nothing new to say, can wait for
`later`. Level 1 is unwired (v0.2's derivation); level 0, the operator, is
unwired and admitted. Level 9 is reserved for v0.5's trunk.

Each switch path carries three wires: tip, ring, and sleeve. The sleeve is
how a busy line is marked — a potential on a third wire — and how a hunting
selector knows a path is taken. Everything is the wire, including busy.

**Smaller:** a 100-line office, connector only, two-digit numbers. Loses
the selector's hunt, which is half the thesis.

## What would settle it

The owner agrees the numbers; the solver prototype runs this office
faster than real time by 20×.

## Answer
