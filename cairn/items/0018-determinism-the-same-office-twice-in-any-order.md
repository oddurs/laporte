---
id: 18
uid: f1b0ad24-8e3b-4744-9e81-98cf0597c5ae
title: 'Determinism: the same office twice, in any order'
type: verify
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- foundation
depends_on:
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

A scenario produces bit-identical output every time it runs, on any
machine, whatever order the apparatus was added to the netlist in.

## Judged against

Itself, run twice. The second run builds the office with its elements
shuffled by a fixed permutation.

## How it is checked

Hash every node voltage on every tick of a v0.1 scenario (lift, wait,
replace). Compare the hashes.

Written as `apps/checks/determinism.hpp`, registered as
`determinism.either_order`, expected to fail until item 29 (laporte.hpp, the
first full office: battery, relay, pair and telephone). Item 20 is too early:
the clock alone cannot be wired to a line. The check hashes (FNV-1a over the
exact bits of every node voltage on every tick, -0.0 folded to +0.0, no
tolerance) once for the given order, then for other orders: for six elements or
fewer, every permutation (the v0.1 line is four, so exhaustive); for more, every
rotation, the reversal and 24 fixed-seed shuffles, which does not guarantee
hitting a leak that needs one particular adjacency. It returns 1 only if all
hashes agree. It returns 0 for fewer than three elements, any NaN or infinity,
a trace in which no voltage changes, a hand that does nothing (the same run with
a no-op hand must hash differently), or an inert element (removing any single
element must change the hash; this shows the element is present in the trace,
not that its value matters). Until the office exists it is a stub measuring 0
("not built yet") against a published 1.

`determinism.detector` (a real check, not expected to fail) runs the machinery
against fake offices so the templates are built under `make strict`: an honest
two-phase office is alike; all twelve element-to-element leaks are caught; a
leak that fires only when element 0 is solved immediately before element 3 is
caught at n=4; and a constant office, one ignoring its elements, one that
ignores its elements but answers the hand, a NaN office and an idle hand are
refused.

Requirements on items 19 and 20, because the comparison is bit for bit:
the solve must stamp and sum in canonical node-index order, with node indices
taken from node names and not from the order nodes were first met. A
floating-point sum is not associative, so otherwise a correct two-phase tick
fails on arithmetic and the failure is indistinguishable from the bug.

Who writes what: the test department, this check's author, provides the
`Office` class, `v01_elements()` and the hand scenario (`lift_wait_replace`),
written when item 29's office exists, and replaces the body of
`determinism_measure` with
`identical_in_either_order<laporte::Office>(v01_elements(), lift_wait_replace,
ticks)` and removes the marker. The author of items 19, 20 or 29 never edits
the check or supplies any of those three; if they believe the check is wrong
they note it and the inspector decides.

## What failure looks like

An element that advances its state during the solve rather than after it,
so that what it does depends on who was solved first. That is the bug the
two-phase tick exists to make impossible, and this is how we find out it
did not.

## Acceptance criteria

- [x] Identical hashes in either order; expected to fail until the tick lands

## 2026-10-03

Check written as apps/checks/determinism.hpp, one registry line, xfail until 20. Stub measures 0 vs published 1 so it cannot pass vacuously; hook-up is in the item body and the header. Catches the two-phase tick bug (element advancing during the solve). Detector verified against a throwaway fake office (order-independent 1, mid-solve updater 0); not committed. Limits: one permutation, one machine.

## 2026-10-03

Returned and revised. Criterion 1 UNTICKED: no hash is compared until item 29 supplies an office, so the criterion is not yet true; it becomes true when the stub is replaced and the check passes. What exists: the check registered, expected to fail until item 29 (not 20: the first full office is laporte.hpp), plus determinism.detector proving the machinery against fakes (all rotations, all 12 leak directions, constant/NaN/idle-hand/ignoring-elements refused). Bit-exact needs canonical node-index solve order in 19/20; recorded as a requirement.

## 2026-10-03

Second return addressed: removal-sensitivity (inert element returns 0), all permutations for n<=6 and rotations+reversal+24 seeded shuffles beyond, detector now 19 cases incl. a blind-but-answers-hand office and an adjacency-only leak (0 before 3) caught at n=4, and the item states the test department writes Office/v01_elements/hand when 29 exists.

## 2026-10-03

Closed by the director: the deliverable is a written, registered, expected-failing check with a detector proving what it catches; the hook-up to a real office is item 29's, and is the test department's to make. Criterion ticked on that reading: the check is registered and expected to fail until the tick lands.

## Result

determinism.either_order registered, expected to fail until item 29; determinism.detector proves it catches 12+ leak shapes and refuses vacuous offices.
