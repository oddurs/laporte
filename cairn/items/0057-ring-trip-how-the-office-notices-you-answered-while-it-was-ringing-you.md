---
id: 57
uid: 2c67948c-e008-4453-abbb-f51a8e39898f
title: 'Ring trip: how the office notices you answered while it was ringing you'
type: apparatus
status: backlog
milestone: v0.3
labels:
- derivation
depends_on:
- 50
- 54
- 55
- 56
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: signalling
effort: m
---

## What it is

A relay in the ringing path that responds to direct current and not to the
20 Hz ringing, by a slow armature or a shunting capacitor. When it
operates, the connector cuts off ringing and cuts through the call.

## What it must derive

Its immunity to ringing current, from its own time constant.

## Acceptance criteria

- [ ] The ring-trip check passes
