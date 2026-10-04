---
id: 40
uid: 89eefe16-8668-4af6-b5bb-5059a14d0ed5
title: 'selector.hpp, the vertical motion: a digit is a height'
type: apparatus
status: backlog
milestone: v0.2
labels:
- thesis
depends_on:
- 25
- 26
- 37
- 38
- 39
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: switching
effort: l
---

## What it is

The first motion of the two-motion selector. The line relay follows the
dial pulses. Each time it releases, it pulses the vertical magnet, whose
armature drives a pawl into the ratchet on the shaft and lifts it one
level. A slow-release relay stays operated through the whole train —
because the pulses are shorter than it takes to let go — and drops when
the train ends. That release is how the selector learns the digit is over.
It does not learn the digit. The digit is how high the shaft is.

The release magnet, when the call ends, withdraws the detent and the
shaft falls back to normal under its own weight and a spring.

## What it must derive

The end of a pulse train, from the slow-release relay's timing — and so
the minimum gap between digits a caller must leave.

## What it may touch

The line circuit's nodes; its own magnets' coils; its off-normal springs.

## What is not modelled

The rotary motion, which is v0.3. Mechanical bounce of the shaft.

## Acceptance criteria

- [ ] The steps check passes
- [ ] No integer in this file counts pulses; the shaft position is the count
