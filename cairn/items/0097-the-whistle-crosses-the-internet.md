---
id: 97
uid: b1061b8e-a3a4-4923-ab04-4abb64540f8f
title: The whistle crosses the internet
type: verify
status: backlog
milestone: v0.6
labels:
- thesis
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p2
role: test
area: verification
effort: s
---

## The claim

v0.5's attack works unchanged across the localhost trunk, because nothing
about it was ever about the trunk's medium.

## Acceptance criteria

- [ ] The Joybubbles scenario, in `--file` mode across two processes
