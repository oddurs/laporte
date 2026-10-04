---
id: 84
uid: 9b2bf3b2-11e3-462b-ae6f-2e1d7720c928
title: Outgoing and incoming trunk circuits
type: apparatus
status: backlog
milestone: v0.5
depends_on:
- 47
- 77
- 83
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: switching
effort: l
---

## What it is

At office A, the bank of level 9 terminates on outgoing trunk circuits:
seized by a hunting selector, they present a loop to the SF unit and
repeat the caller's line-relay pulses onward. At office B, an incoming
trunk circuit takes the SF unit's loop breaks into an incoming selector,
which is an ordinary selector.

## Acceptance criteria

- [ ] A call from A to B completes, in the trunk check
