---
id: 96
uid: 9f1210e1-f890-4c99-90e5-e4d9364642db
title: Across localhost, bit for bit, except for the slips
type: verify
status: backlog
milestone: v0.6
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

Two processes on one machine, joined by UDP over localhost in `--file`
mode, produce the same receiver output as the in-process trunk, sample for
sample, apart from slips, which are counted.

## Acceptance criteria

- [ ] Identical outside slip boundaries; runs in CI without a sound card;
      expected to fail until the line lands
