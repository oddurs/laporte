---
id: 52
uid: a1630d24-df65-4b69-b664-06c4e9dd74dd
title: The tones, against the plan
type: verify
status: backlog
milestone: v0.3
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

Dial tone, busy, reorder and ringback have the frequencies, levels and
cadences of the tone plan the era spike chose.

## Judged against

The ledger. If the era is 1965 Bell, the Precise Tone Plan (dial tone
350 + 440 Hz; ringback 440 + 480 Hz, 2 s on, 4 s off; busy 480 + 620 Hz,
0.5 s on and off; reorder the same pair, faster) — if it was in service
in that year, which the ledger has to establish.

## How it is checked

At the caller's pair, a single-bin DFT at each frequency; cadence by the
envelope.

## Acceptance criteria

- [ ] Every tone, within 0.5 % in frequency and one tick in cadence;
      expected to fail until the tones land
