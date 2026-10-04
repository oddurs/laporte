---
id: 27
uid: 53bf2492-398b-4e83-a567-8ef70c9f5d07
title: The switchhook, and the telephone as a resistance
type: apparatus
status: planned
milestone: v0.1
depends_on:
- 19
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: switching
area: station
effort: s
---

## What it is

The first part of the telephone set. On hook, the switchhook holds the
pair open to direct current. Off hook, it closes the pair through the
set's DC resistance. That is all the set does in v0.1, and it is enough for
the office to know you are there.

## What it must derive

Nothing yet; the set's resistance comes from the ledger. Later items will
replace that single resistance with the transmitter, receiver and network
whose series combination it is — at which point it must be *derived*, and
the check that it still matches the ledger is in v0.4.

## What it may touch

The set's tip and ring terminals.

## What is not modelled

The ringer, which is v0.3; everything audio, which is v0.4.

## Acceptance criteria

- [ ] A hand lifting the handset closes the loop on the next tick
