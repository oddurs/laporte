---
id: 55
uid: 6785e655-f2ca-4d83-a1ea-e102ac274a2a
title: 'ringer.hpp: a capacitor, a coil, and a bell'
type: apparatus
status: backlog
milestone: v0.3
labels:
- derivation
depends_on:
- 10
- 19
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: station
effort: m
---

## What it is

The bell in the telephone, across the pair even when on hook: a capacitor
in series with a polarised ringer coil. The capacitor blocks direct
current, so an idle telephone is invisible to the line relay; the 20 Hz
ringing passes, and the coil's armature swings the clapper between two
gongs once per half-cycle.

## What it must derive

That the ringer draws no DC; how much ringing current it draws, and
therefore how many bells a line can ring.

## What it may touch

The set's tip and ring, on the line side of the switchhook.

## What is not modelled

The sound of the gong — unless the office-sound instrument wants it.

## Acceptance criteria

- [ ] Component values from the ledger (500 set ringer)
- [ ] A ringing scenario counts clapper strikes at 40 per second of ringing
