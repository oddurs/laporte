---
id: 82
uid: 06074d71-5042-4fe0-8607-91273daa0285
title: Speech never hangs up the trunk
type: verify
status: backlog
milestone: v0.5
depends_on:
- 16
- 66
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: m
---

## The claim

Minutes of speech through the trunk never trip the far SF receiver.

## How it is checked

The speech the voice spike chose, spoken into a call across the trunk;
count false disconnects. Also: a sung or played 2600 Hz *with* other
energy (a chord) does not trip it either, and a pure 2600 Hz does — which
is the next check.

## Acceptance criteria

- [ ] Zero false disconnects; expected to fail until SF lands
