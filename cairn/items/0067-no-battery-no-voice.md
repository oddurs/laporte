---
id: 67
uid: ef92ccc2-1ff7-453d-9a9e-cf2efdfa5da2
title: No battery, no voice
type: verify
status: backlog
milestone: v0.4
labels:
- derivation
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

The voice signal on the line is powered by the office battery: remove the
battery and the far end hears nothing; lengthen the loop and the far end
hears less, by the amount the loop current fell.

## Judged against

The ledger's figures for transmission loss with loop length, where they
exist; the model's own loop current where they do not.

## Acceptance criteria

- [ ] Level against loop length, tabulated; zero with no battery;
      expected to fail until the transmitter lands
