---
id: 28
uid: 44f14abc-0325-4d3a-9d07-030d4a25287a
title: 'hand.hpp: what a hand can do to a telephone'
type: apparatus
status: planned
milestone: v0.1
labels:
- thesis
depends_on:
- 15
- 20
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: systems
area: frame
effort: m
---

## What it is

The only interface between the outside world and the office. A scenario is
a list of physical actions at times: lift the handset, replace it, tap the
switchhook, pull the dial to the finger-stop at hole *n* and let go (v0.2),
speak a sound file into the transmitter (v0.4), whistle at a frequency
(v0.5). Each action moves a mechanical part of a telephone. There is no
action called `dial(347)`.

A person knows a telephone number; the office does not. So numbers are
allowed here, in the hand, and nowhere in `include/` past this file.

## What it must derive

Nothing. It is a script.

## What it may touch

Mechanical parts of a telephone set: the handset's position, the dial's
finger-wheel, the air in front of the transmitter. Never a node.

## What is not modelled

Hesitation. A hand pulls the dial at a constant speed and lets go cleanly.

## Acceptance criteria

- [ ] Scenarios are `constexpr` lists, defined in C++, with no parser
- [ ] `lift`, `replace` and `tap` exist; later actions are added by the items that need them
