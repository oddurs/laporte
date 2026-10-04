---
id: 21
uid: 49162786-486f-43a2-aae8-5fc63b3b4fb8
title: './laporte trace: an oscilloscope in a terminal'
type: instrument
status: planned
milestone: v0.1
depends_on:
- 20
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: systems
area: instrument
effort: m
---

## What it witnesses

Any node voltage or branch current, over time, for any scenario.

## What it prints

An oscillogram in text: time across, value down, with the axis labelled
in units. A second form writes the same samples as columns for anyone
who wants to plot them elsewhere. Everything from v0.2 onwards — dial
pulses, ringing, ring trip, SF bursts — is shown in the README through
this instrument.

## What it may not do

Probe anything an engineer with a scope could not. It reads the netlist
and the mechanical positions an engineer could see; it never reads
apparatus internals.

## Acceptance criteria

- [ ] `./laporte trace loop-current --scenario lift` shows the current step
      when the handset is lifted, labelled in milliamperes
