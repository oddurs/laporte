---
id: 72
uid: 8a7b8dd7-be13-49fa-b555-9e3f90c6fddf
title: The station-to-station response, swept
type: verify
status: backlog
milestone: v0.4
labels:
- derivation
- admission
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: m
---

## The claim

Speak a sweep into one telephone; the response at the other's receiver
falls out of the parts, and its edges and shape are those published for
the 500 set over a typical loop.

## What failure looks like

A flat response between two sharp edges. That is a filter, and nobody was
supposed to have written one.

## Acceptance criteria

- [ ] Response printed against the published curve; differences explained
