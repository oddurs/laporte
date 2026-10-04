---
id: 47
uid: ba3fcf01-90f5-4a0a-8af5-05629db866fb
title: 'The selector''s second motion: hunting for a free path'
type: apparatus
status: backlog
milestone: v0.3
labels:
- thesis
depends_on:
- 40
- 45
- 46
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: switching
effort: l
---

## What it is

When the vertical train ends, the selector's rotary magnet steps the wiper
across the ten bank contacts at that level, by itself, testing each sleeve.
The first sleeve without a busy potential stops it, a cut-through relay
connects tip and ring onward, and the selector grounds that sleeve to mark
it busy for everyone else. If all ten are busy it steps to the eleventh
position, where busy tone is wired.

## What it must derive

Which path a call takes, from the state of the sleeves alone.

## What it may touch

Its bank contacts' tip, ring and sleeve at the level it stopped on.

## What is not modelled

Two selectors seizing the same idle path on the same tick. The real switch
had a guard interval; the header says what the model does instead, and
the check proves it is safe.

## Acceptance criteria

- [ ] The hunt check passes
