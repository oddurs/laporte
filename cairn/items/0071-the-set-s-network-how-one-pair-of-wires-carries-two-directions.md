---
id: 71
uid: ee50ecf5-fcd7-4f50-a9b0-5aa2ccab8220
title: 'The set''s network: how one pair of wires carries two directions'
type: apparatus
status: backlog
milestone: v0.4
labels:
- derivation
depends_on:
- 66
- 68
- 69
- 70
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: station
effort: l
---

## What it is

The heart of the telephone set. Transmitter and receiver share one pair
to the office. Connected naively, your own voice — much louder at your end
than the far party's — drowns the receiver. The network is a bridge of
induction-coil windings and a balance impedance arranged so that the
transmitter's current divides and cancels in the receiver's winding while
the line's signal does not. The cancellation is only as good as the
balance matches the line, so some of your voice always comes through:
sidetone, which telephone engineers wanted, at a level.

## What it must derive

The sidetone level, from the balance network's match to the line. That
the set's DC resistance now equals the ledger's, as the sum of its parts
(replacing v0.1's single resistor).

## What is not modelled

The 500 set's varistors, which adjust the network for loop length — a
`later` item, and an admission until then.

## Acceptance criteria

- [ ] The sidetone check passes
- [ ] v0.1's loop limit is unchanged, or the change is explained
