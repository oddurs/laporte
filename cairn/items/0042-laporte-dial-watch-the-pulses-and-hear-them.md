---
id: 42
uid: db73e252-2076-427c-a5e3-4d90b708a450
title: './laporte dial: watch the pulses, and hear them'
type: instrument
status: backlog
milestone: v0.2
depends_on:
- 21
- 36
- 40
- 41
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: switching
area: instrument
effort: m
---

## What it witnesses

A dialled number, from the telephone's terminals to the selector's shaft.

## What it prints

The pulse train as a trace, the shaft height after each train, and the
loop current written as a WAV — what a lineman with a test set across the
pair would have heard.

## What it may not do

Print a digit it did not read off a shaft.

## Acceptance criteria

- [ ] `./laporte dial 5` shows five breaks and a shaft at level five
