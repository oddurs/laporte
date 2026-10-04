---
id: 94
uid: a2ff6ac0-e87b-4fc2-99e7-be02ece0f198
title: 'The one platform file: a microphone, a speaker, a clock'
type: apparatus
status: backlog
milestone: v0.6
labels:
- admission
depends_on:
- 90
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: audio
effort: l
---

## What it is

`apps/platform/coreaudio.hpp`: opens the default input and output at
8000 Hz mono (or the device rate with a stated resampler), and calls the
office's tick from the device callback. Microphone samples become pressure
at the transmitter's diaphragm; the receiver's diaphragm becomes speaker
samples.

## What it may not do

Appear in `include/`. Allocate or lock in the callback.

## Acceptance criteria

- [ ] A local call — two telephones in one office on one machine, mic to speaker — works live

## 2026-10-03

Consequence of 0014: the platform edge opens the device at 48 kHz (the office's tick rate), not 8 kHz; the decimation to the trunk's 8 kHz belongs to the DS0 item, not here.
