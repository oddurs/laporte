---
id: 62
uid: fa7af771-7ac7-4943-927f-df3f7a7e79f7
title: './laporte call: a whole call, heard from both ends'
type: instrument
status: backlog
milestone: v0.3
depends_on:
- 21
- 41
- 60
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: instrument
effort: l
---

## What it witnesses

One call, start to finish, from both telephones.

## What it prints

Two WAV files — what each receiver received — and a log of every event an
engineer with a scope would have seen on the wires, by time: line relay
operates; dial tone applied; shaft at level 4; rotary hunt stops at
contact 2; connector at 7, 3; sleeve idle; ringing applied; ring trip;
cut through; release. Every entry is read off a node or a shaft.

No voice yet: the caller hears dial tone, their own dialling (faint, past
the shunt), ringback and the answer; the called party hears nothing until
they lift, because the bell is not in the receiver. A test tone through
the bridge is allowed, and labelled.

## What it may not do

Narrate anything it did not measure.

## Acceptance criteria

- [ ] `./laporte call 347 --from 412` renders two files and a log
- [ ] Deterministic: the log is quotable in the README
