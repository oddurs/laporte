---
id: 98
uid: e4197331-2273-4748-82c3-3986a95cb6aa
title: './laporte line: an office and a telephone on each of two machines'
type: instrument
status: backlog
milestone: v0.6
depends_on:
- 93
- 94
- 95
- 96
- 97
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: instrument
effort: l
---

## What it witnesses

A live call between two machines.

## What it prints

`./laporte line --office a --listen 5004` on one machine,
`./laporte line --office b --peer host:5004` on the other. A status line:
hook state, shaft positions, trunk state, slips. On hang-up, the session's
event log.

## Acceptance criteria

- [ ] Two Macs on one network hold a call, dial each other, and hang up
