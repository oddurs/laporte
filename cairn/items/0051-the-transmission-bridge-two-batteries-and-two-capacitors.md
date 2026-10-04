---
id: 51
uid: 52b07539-5fd2-49da-bed0-5a5ba15ecc85
title: 'The transmission bridge: two batteries and two capacitors'
type: apparatus
status: backlog
milestone: v0.3
depends_on:
- 19
- 25
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: transmission
effort: m
---

## What it is

Each party on a call is fed battery through their own relay windings, so
each side can be supervised separately; the two sides are joined for
speech only through capacitors, which pass the voice and block the
direct current. This is how a call is two circuits and one conversation.

## What it must derive

Supervision on each side independently; the low-frequency edge of the
voice path, from the capacitors and the windings.

## What it may touch

Both sides' tip and ring.

## What is not modelled

The repeating-coil (transformer) bridge; the choice is stated.

## Acceptance criteria

- [ ] Either side hanging up is seen by its own relay only
