---
id: 50
uid: 007d9e48-9d7c-45ff-8a2d-bb7edb362902
title: 'connector.hpp: the last two digits, and the test for busy'
type: apparatus
status: backlog
milestone: v0.3
labels:
- thesis
depends_on:
- 40
- 45
- 48
- 49
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: switching
effort: l
---

## What it is

The final switch in the train. The tens digit lifts it vertically; the
units digit steps it rotarily — directly, not hunting, because the units
digit names one line. Then it tests that line's sleeve: busy, and it
returns busy tone; idle, and it marks it busy and starts ringing.

It also holds the transmission bridge that feeds the called party, and the
ring-trip relay that notices the answer.

## What it must derive

The called line, from two shaft positions.

## What it may touch

Its incoming tip, ring and sleeve; the bank contact it rests on.

## What is not modelled

Hunting connectors for PBX groups; reverse-battery signalling.

## Acceptance criteria

- [ ] The busy check passes
