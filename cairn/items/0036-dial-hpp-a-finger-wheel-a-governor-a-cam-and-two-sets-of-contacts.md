---
id: 36
uid: 136583b8-0b10-408e-8abd-0ef4812490e4
title: 'dial.hpp: a finger-wheel, a governor, a cam, and two sets of contacts'
type: apparatus
status: backlog
milestone: v0.2
labels:
- thesis
- derivation
depends_on:
- 27
- 28
- 34
- 35
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: station
effort: l
---

## What it is

The rotary dial as a mechanism. The finger pulls the wheel to the
finger-stop, winding a spring; the finger lets go; the spring drives the
wheel back through a centrifugal governor that holds it to a constant
speed; a cam on the shaft opens the pulse contacts once per digit-step on
the way back. The hole the finger was in decides how far the wheel was
wound, and therefore how many times the cam opens the contacts. Zero is
ten.

The off-normal contacts close the moment the wheel leaves rest and short
the receiver, so the caller does not hear the clicks in their own ear.

There is no digit in this file. There is an angle.

## What it must derive

Pulse rate from the governor; break/make ratio from the cam; the lost
motion before the first pulse (a dial always turns a little before it
starts breaking — which is why a dial cannot send zero pulses).

## What it may touch

The pulse contacts, in series with the loop; the off-normal contacts,
across the receiver. The hand moves the wheel.

## What is not modelled

Wear, dirt, temperature in the governor; a finger that does not let go
cleanly. The hand gains `pull(hole)` in this item.

## Acceptance criteria

- [ ] The dial check passes
- [ ] Nothing in `dial.hpp` has a type or a variable that holds a digit
