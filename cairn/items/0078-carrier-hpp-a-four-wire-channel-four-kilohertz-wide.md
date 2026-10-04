---
id: 78
uid: 39a3ba5a-0d4c-4a11-b5b6-890e3a58e905
title: 'carrier.hpp: a four-wire channel four kilohertz wide'
type: apparatus
status: backlog
milestone: v0.5
labels:
- derivation
- admission
depends_on:
- 10
- 77
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: transmission
effort: m
---

## What it is

One voice channel of a frequency-division carrier system, one direction
each way. Channels were stacked 4 kHz apart; the channel filters that
separated them pass roughly 300–3400 Hz. This is where the telephone's
band edge actually comes from, and it is the one place a filter is
legitimate, because the filter is the apparatus.

## What it must derive

The band edges, from the channel spacing and the filter's design — or, if
the filter is taken from a published response, it says so.

## What is not modelled

The modulation itself: the channel is simulated at baseband. Carrier
leak, noise, level drift.

## Acceptance criteria

- [ ] Band edges from the ledger, with handles
