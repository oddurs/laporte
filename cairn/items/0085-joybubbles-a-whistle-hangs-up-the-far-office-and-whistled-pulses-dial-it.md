---
id: 85
uid: a8504f4a-eead-485c-bfa4-95ef8c25054a
title: 'Joybubbles: a whistle hangs up the far office, and whistled pulses dial it'
type: verify
status: backlog
milestone: v0.5
labels:
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

During a call to office B, a hand whistles a pure 2600 Hz into the
transmitter for about a second and stops. The far SF unit hears an idle
trunk: office B releases the call and its incoming selector returns to
normal, while office A — which did not hear the whistle as signalling —
still holds the trunk. The caller now owns a seized trunk into office B.
Whistled bursts of 2600 Hz at ten a second then step office B's selector.

Nothing in the program was written to allow this. It is the consequence of
sf.hpp's design and the hand's ability to whistle.

## Judged against

The accounts of Joe Engressia (Joybubbles) and of the Cap'n Crunch whistle,
from the ledger; *Notes on Distance Dialing*, which published the
frequency.

## How it is checked

The scenario; then the inspector's grep that no file outside `sf.hpp` and
`hand.hpp` (and checks) knows 2600 Hz.

## What failure looks like

The attack failing because the guard is too strict even for a pure tone —
which would mean the guard has been tuned to make the model "safe" rather
than to match the ledger. Or the attack succeeding only because of a
special case.

## Acceptance criteria

- [ ] The attack succeeds, end to end, with no code that knows about it
