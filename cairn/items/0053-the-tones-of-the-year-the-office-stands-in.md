---
id: 53
uid: 0bd7e655-c798-4b0b-a781-00b097195361
title: The tones of the year the office stands in
type: apparatus
status: backlog
milestone: v0.3
depends_on:
- 9
- 10
- 19
- 52
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: transmission
area: signalling
effort: m
---

## What it is

The tone sources, as voltage sources in the netlist, wired to the bank
positions and relay contacts that apply them. Dial tone is applied by the
first selector when it seizes; ringback and busy by the connector.

## What it must derive

Nothing; the frequencies are the plan's. The *cadence*, though, comes from
an interrupter — a motor-driven cam opening and closing contacts — and is
built that way, not as a timer.

## What is not modelled

The tone plant's generator. A sine is a sine.

## Acceptance criteria

- [ ] The tones check passes
