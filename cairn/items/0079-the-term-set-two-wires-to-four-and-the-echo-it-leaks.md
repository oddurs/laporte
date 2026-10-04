---
id: 79
uid: 33f49f2b-d8d8-4d5f-a2d6-b74744cc943b
title: 'The term set: two wires to four, and the echo it leaks'
type: apparatus
status: backlog
milestone: v0.5
labels:
- admission
depends_on:
- 71
- 77
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: transmission
effort: m
---

## What it is

A hybrid — the same idea as the set's network — joining the office's
two-wire path to the trunk's two one-way pairs. Its imperfect balance
returns some of each direction's signal into the other: echo.

## What it must derive

The trans-hybrid loss, from the balance.

## What is not modelled

Echo suppressors. On a trunk this short the delay is too small to hear,
and the header says why.

## Acceptance criteria

- [ ] The return loss printed by the trunk check
