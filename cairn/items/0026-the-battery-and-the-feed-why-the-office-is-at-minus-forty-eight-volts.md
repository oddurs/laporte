---
id: 26
uid: 176b153b-faa5-4be8-9f99-09b3cdcaacc1
title: 'The battery and the feed: why the office is at minus forty-eight volts'
type: apparatus
status: planned
milestone: v0.1
labels:
- derivation
- admission
depends_on:
- 19
- 25
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: switching
area: signalling
effort: m
---

## What it is

The office battery and the line relay that feeds it to a subscriber's
pair: battery through one winding of the line relay to the ring wire;
earth through the other winding to the tip. When the pair is closed at
the far end, current flows through both windings and the relay operates.
That is the whole of "off hook".

## What it must derive

Loop current, from the battery, the relay windings, the cable and the set.

## What it may touch

Battery, earth, tip, ring.

## What is not modelled

The battery is ideal: no internal resistance, no charging plant, no
discharge curve. The admission goes in the header. The polarity is not
a simplification: positive earth, with the plant negative, is chosen to
limit electrolytic corrosion of buried copper — to be sourced, and stated
with its source.

## Acceptance criteria

- [ ] Polarity and voltage from the ledger, with their source handles in the header
