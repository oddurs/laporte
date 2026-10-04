---
id: 22
uid: 6c8caef4-e201-4505-baee-da185aa7b915
title: Copper, derived from resistivity and checked against the published table
type: verify
status: planned
milestone: v0.1
labels:
- derivation
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

The resistance of a pair of copper wires of given gauge and length falls
out of copper's resistivity and the definition of the American wire
gauge, and matches the figure the industry published.

## Judged against

The sources ledger: annealed copper's resistivity at 20 °C (IACS); the AWG
diameter definition; a published ohms-per-thousand-feet figure for 19, 22,
24 and 26 gauge cable pairs.

## How it is checked

Compute loop resistance per kilofoot for each gauge; compare.

## What failure looks like

A factor of two: a loop is two conductors, and forgetting the return wire
is the classic mistake.

## Acceptance criteria

- [ ] Four gauges, within 2 %, expected to fail until the cable lands
