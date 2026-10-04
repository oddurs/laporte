---
id: 56
uid: d28f3c3b-4732-407a-8552-2718799e4dc7
title: Ring trip, and the ringing that must not trip it
type: verify
status: backlog
milestone: v0.3
labels:
- derivation
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

Lifting the handset while the bell rings trips the ringing within one
ringing cycle — because the DC under the ringing now has a path — and the
ringing current through the ringer alone never trips it, on any loop up to
the limit.

## How it is checked

Answer at every phase of the cadence; measure time to trip. Ring an
unanswered line on the longest and shortest loops; the ring-trip relay
must never operate. Report the margin.

## What failure looks like

A ring-trip relay that operates on the AC: a telephone that answers itself.

## Acceptance criteria

- [ ] Trip time and false-trip margin, printed; expected to fail until ring trip lands
