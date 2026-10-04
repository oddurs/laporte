---
id: 100
uid: eedf5860-2340-4ba2-b3c5-9b90e82168d6
title: v0.6, read as a stranger
type: verify
status: backlog
milestone: v0.6
depends_on:
- 90
- 91
- 92
- 93
- 94
- 95
- 96
- 97
- 98
- 99
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: inspector
area: verification
effort: m
---

## The claim

At this milestone the repository builds, runs, and makes exactly the
claims it has earned. See the v0.1 review for the method — with a second
machine.

The particular thing to look for at v0.6: the platform leaking inward. Any
include of a system audio or socket header outside `apps/platform/`; any
real time in `include/`.

## Acceptance criteria

- [ ] A real call between two machines; findings filed; owner has tagged `v0.6`
