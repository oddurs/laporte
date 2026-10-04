---
id: 41
uid: 4c1c14e0-a485-4c4b-ad1b-e203e0a02fe4
title: 'wav.hpp: a sound file is a forty-four-byte header and some numbers'
type: apparatus
status: backlog
milestone: v0.2
depends_on:
- 15
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: systems
area: audio
effort: s
---

## What it is

Reading and writing 16-bit mono PCM WAV at the clock rate. Written by
hand, like cornell's PPM, because it is small enough to be read in one
sitting.

## What it must derive

Nothing. Scale is stated: which current or voltage maps to full scale is
a parameter of the instrument writing the file, printed when it writes.

## What is not modelled

Every other WAV variant. Reading refuses anything but what it writes.

## Acceptance criteria

- [ ] A file written plays in QuickTime and reads back bit for bit
