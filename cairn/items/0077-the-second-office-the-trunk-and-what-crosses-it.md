---
id: 77
uid: 34273d57-ff0a-4067-8b49-b04da3d9771e
title: The second office, the trunk, and what crosses it
type: spike
status: backlog
milestone: v0.5
owner: oddurs
labels:
- decision
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: signalling
effort: m
---

## Question

How are two offices joined, and how does a call cross?

## Why it has to be answered first

It is the design of the whole milestone, and of v0.6's network.

## Options

**Recommended:** level 9 of office A's first selectors is a trunk group:
the selector hunts it for an idle outgoing trunk exactly as it hunts
connectors. "Dial 9 to get out" falls out. The trunk is a four-wire
channel (one pair each way) of a carrier system, with a term set (hybrid)
at each end to join it to the two-wire office. Supervision is
single-frequency: 2600 Hz on the channel when idle, silence when seized.
The outgoing trunk repeats the caller's dial pulses as bursts of 2600 Hz;
the far SF unit turns them back into loop breaks, which step an incoming
selector at office B. No register, no sender: the pulses go straight
through, as they did on many such trunks.

**Multi-frequency (KP … ST) with senders.** More accurate for toll calls,
adds a register that stores digits — which v0.2 worked hard not to have.
`later`, with the blue box proper.

## What would settle it

The owner agrees; the ledger has SF's frequency, level, timing and guard
behaviour.

## Answer
