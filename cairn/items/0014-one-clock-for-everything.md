---
id: 14
uid: 72be446d-11ba-4154-8d0b-bdd50e05127b
title: One clock for everything
type: spike
status: planned
milestone: v0.1
owner: oddurs
labels:
- decision
- foundation
depends_on:
- 13
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: circuit
effort: m
---

## Question

At what rate does the office tick, and how are capacitors and inductors
advanced from one tick to the next?

## Why it has to be answered first

Relays operate in milliseconds, dial pulses are 100 ms long, ringing is
20 Hz, the voice reaches 3.4 kHz and SF signalling sits at 2.6 kHz. One
clock has to carry all of them, because two clocks would need something
to translate between them, and that something would be a message.

## Options

**8000 Hz** (recommended if it passes). The rate of a digital telephone
channel; v0.6's trunk is a stream at exactly this rate, so the office and
the network share a heartbeat and nothing resamples. The question is
whether 3.4 kHz and 2.6 kHz survive integration at 8 kHz.

**48000 Hz, decimated to 8000 at the trunk.** Six times the work, and a
filter at the boundary that has to be explained.

Integration: **backward Euler** is unconditionally stable and damps high
frequencies; **trapezoidal** preserves them and can ring on a switched
contact. The likely answer is trapezoidal with a backward-Euler step on
the tick a contact changes state, which is what SPICE does — but measure.

## What would settle it

Drive an RC and an LC through a 20 Hz, a 1 kHz, a 2.6 kHz and a 3.4 kHz
sine at each candidate rate and method; tabulate amplitude and phase error
against the exact answer. Pick the cheapest option that keeps every error
under a figure written down before measuring (proposed: 0.5 dB, 5°).

## Answer
