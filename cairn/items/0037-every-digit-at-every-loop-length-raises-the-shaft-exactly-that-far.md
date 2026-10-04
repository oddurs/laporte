---
id: 37
uid: 6fe5cb6f-9239-4b37-a3e4-215c136e4982
title: Every digit, at every loop length, raises the shaft exactly that far
type: verify
status: backlog
milestone: v0.2
labels:
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

For every digit and every loop length up to the derived limit, the first
selector's shaft ends the train at the level dialled — and outside the
dial's tolerances it fails, in the way a real office failed.

## Judged against

The dial tolerances in the ledger; the loop limit from v0.1.

## How it is checked

A grid: digits 1–0, loop lengths from zero to the limit, dial speeds
across and beyond tolerance. Report the shaft height. Mark the region in
which it is correct.

## What failure looks like

A region of success with no edge. If a dial at 20 pulses a second still
dials correctly, the selector is counting events rather than responding to
a magnet that cannot keep up, and something was written that should not
have been.

## Acceptance criteria

- [ ] The grid and its edge, printed; expected to fail until the selector lands
