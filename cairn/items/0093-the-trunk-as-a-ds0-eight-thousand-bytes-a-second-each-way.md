---
id: 93
uid: a3120ed7-4210-4e86-af92-c4a247947a20
title: 'The trunk as a DS0: eight thousand bytes a second, each way'
type: apparatus
status: backlog
milestone: v0.6
depends_on:
- 86
- 90
- 92
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: transmission
effort: m
---

## What it is

The four-wire trunk's two directions as byte streams, so that the far end
of the trunk can be in another process — and therefore another machine.
Within one process the result must be identical to v0.5's in-process trunk
except for the quantisation mu-law adds.

## What is not modelled

The rest of T1: framing bits, robbed-bit signalling (which is how real
digital trunks signalled — in-band in a different sense, and an admission
here, since this trunk keeps SF).

## Acceptance criteria

- [ ] The v0.5 checks pass with the trunk replaced by a DS0 in-process
