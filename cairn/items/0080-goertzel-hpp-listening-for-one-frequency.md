---
id: 80
uid: c795afc5-179d-4c03-9873-5e8bfb2669ad
title: 'goertzel.hpp: listening for one frequency'
type: apparatus
status: backlog
milestone: v0.5
depends_on:
- 15
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: signalling
effort: s
---

## What it is

Goertzel's algorithm (1958): the power at one frequency, one sample at a
time, with two multiplies and two additions. Why it, rather than a
bandpass filter of relays and coils as the real SF receiver used, is an
admission the header makes: the detector's *behaviour* is matched to the
ledger (bandwidth, timing, guard), not its circuit.

## Acceptance criteria

- [ ] Response at and around 2600 Hz printed by its check
