---
id: 69
uid: 60a32782-1564-4f96-8007-16529569afe0
title: 'receiver.hpp: a magnet, a diaphragm, and a resonance'
type: apparatus
status: backlog
milestone: v0.4
depends_on:
- 19
- 66
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: station
effort: m
---

## What it is

A permanent magnet holds an iron diaphragm under tension; the voice
current through a coil adds to and subtracts from the field; the diaphragm
moves. The permanent magnet is why the diaphragm moves at the voice's
frequency rather than twice it — an argument the header makes.

## What it must derive

Output pressure from current, through the resonance the spike chose.

## Acceptance criteria

- [ ] The fit, if it is one, is named and sourced
