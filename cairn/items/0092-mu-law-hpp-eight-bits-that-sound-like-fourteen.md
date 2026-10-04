---
id: 92
uid: 9208195f-50b6-4205-b1dd-0b990ff99c6c
title: 'mu_law.hpp: eight bits that sound like fourteen'
type: apparatus
status: backlog
milestone: v0.6
labels:
- derivation
depends_on:
- 91
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: transmission
effort: m
---

## What it is

Companding: quiet sounds get fine steps and loud ones coarse, because the
ear hears ratios. Derived from the continuous law with mu = 255, then the
15-segment piecewise-linear approximation G.711 actually standardised,
with both in the file so the reader sees the trade.

## What it must derive

The segment table, at compile time, from the law. Nobody types the table.

## Acceptance criteria

- [ ] The G.711 check passes
