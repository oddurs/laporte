---
id: 22
uid: 6c8caef4-e201-4505-baee-da185aa7b915
title: Copper, derived from resistivity and checked against the published table
type: verify
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- derivation
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
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

- [x] Four gauges, within 2 %, expected to fail until the cable lands

## 2026-10-03

apps/checks/copper.hpp. Oracle (real code, units.hpp) derives loop ohms from S05 resistivity and S06's d = 0.005 in x 92^((36-n)/39), both conductors; it agrees with S06 Table 5 (doubled) to 0.45 % worst case, checked now by copper.oracle_vs_table (passes). The table leg is UNSOURCED for 19 AWG: S06 gives 22, 24 and 26 AWG only (16.2, 25.7, 41.0 ohm/kft single conductor, 20 C, annealed). 19 AWG is judged against the derivation alone (copper.cable_vs_derivation, tol 0.01 %); copper.cable_vs_table covers 22/24/26 (tol 2 %). Both are expected to fail until item 23 and answer NaN until then (no tolerance accepts NaN; a self-test, copper.selftest, shows right/one-wire/length-ignoring/NaN/zero fakes are told apart). Hook-up for item 23's author: include cable.hpp and replace the body of cable_loop_resistance(int awg, Metres) with the cable's loop resistance (Ohms); nothing else may change.

## Result

Copper oracle derived from resistivity and the AWG definition; agrees with ledger S06 Table 5 doubled to 0.45% (22/24/26 AWG); 19 AWG table leg unsourced. Registered expected-fail until item 23. Inspector PASS.
