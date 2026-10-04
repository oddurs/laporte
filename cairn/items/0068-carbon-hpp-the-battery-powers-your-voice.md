---
id: 68
uid: 691b844a-d7d9-4e90-a23a-32939220cade
title: 'carbon.hpp: the battery powers your voice'
type: apparatus
status: backlog
milestone: v0.4
labels:
- thesis
- derivation
depends_on:
- 19
- 66
- 67
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: station
effort: l
---

## What it is

The carbon transmitter: a cup of carbon granules behind a diaphragm. Sound
pressure compresses the granules, their resistance falls, the loop current
rises. The telephone signal is that variation in current — supplied by the
battery at the exchange. A carbon transmitter is an amplifier: it controls
more power than the voice puts in, which is why telephones needed no
electronics for seventy years.

Credit, in the header: Edison, Hughes and Hunnings, with the ledger's dates.

## What it must derive

Signal current from loop current: so a long loop, which carries less
current, makes a quieter voice. The distortion of carbon, from its
nonlinearity.

## What it may touch

The set's transmitter terminals in the network.

## What is not modelled

Packing of the granules, their noise (the "frying" of a carbon
transmitter), aging.

## Acceptance criteria

- [ ] The battery check passes
