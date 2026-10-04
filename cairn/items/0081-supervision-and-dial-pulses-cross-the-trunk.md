---
id: 81
uid: 05bd9b44-8307-454e-8159-62e67a627ca7
title: Supervision and dial pulses cross the trunk
type: verify
status: backlog
milestone: v0.5
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

Seizure, answer, hang-up and every dialled digit cross the trunk; the
pulses arrive at office B's selector within the tolerance its own
selectors accept; the pulse distortion added by the SF units is measured.

## Judged against

SF timing figures from the ledger; v0.2's selector tolerance.

## Acceptance criteria

- [ ] Every digit at B's selector; distortion printed; expected to fail until SF lands
