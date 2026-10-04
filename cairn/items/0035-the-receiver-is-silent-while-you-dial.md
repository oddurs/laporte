---
id: 35
uid: 731ccbf0-abae-497e-8b65-9b7c8fdb993d
title: The receiver is silent while you dial
type: verify
status: backlog
milestone: v0.2
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p2
role: test
area: verification
effort: s
---

## The claim

The off-normal contacts keep the dial pulses out of the caller's receiver.

## How it is checked

Measure the current through the receiver's position during a dialled
digit, with the off-normal contacts and with them removed (a scenario
flag, not a code path). Report both.

## Acceptance criteria

- [ ] The ratio printed; expected to fail until the dial lands
