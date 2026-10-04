---
id: 83
uid: 972847ad-dc05-4107-ba7f-6f46a1a1e1dd
title: 'sf.hpp: a trunk is idle when it is singing'
type: apparatus
status: backlog
milestone: v0.5
labels:
- thesis
depends_on:
- 78
- 80
- 81
- 82
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: signalling
effort: l
---

## What it is

The single-frequency signalling unit at each end of the trunk. Toward the
channel: 2600 Hz while the office side is idle, silence while it is
seized, and bursts of 2600 Hz while it repeats dial pulses. From the
channel: a detector that turns the presence of 2600 Hz back into an
on-hook condition on the office side, and its bursts back into loop
breaks. A guard circuit compares 2600 Hz energy with energy elsewhere in
the band, so that speech containing 2600 Hz does not hang up the call.

The signalling and the voice share the channel. The unit has no way of
knowing which a tone came from. That is the design.

## What it must derive

Nothing a person did. It responds only to what is in the channel.

## Acceptance criteria

- [ ] The supervision and guard checks pass
- [ ] The frequency appears in this file and in the hand's whistle, and
      nowhere else in the repository outside prose and the ledger
