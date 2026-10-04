---
id: 34
uid: 5dffc2c0-88d4-482d-bfb4-cb1f3477db83
title: 'The dial keeps time: ten pulses a second, sixty-one percent break'
type: verify
status: backlog
milestone: v0.2
labels:
- derivation
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

The dial's pulse rate falls out of its governor, and its break/make ratio
falls out of its cam. Both are inside the tolerances the Bell System
specified for dials in service.

## Judged against

The ledger: nominal 10 pulses per second and a break of about 61 % (Bell
practice; 60 % or 67 % elsewhere), and the acceptance range for each.

## How it is checked

Dial every digit; measure every break and make at the telephone's
terminals by tick.

## What failure looks like

A rate that is exactly 10.000 and a ratio that is exactly 0.610 for every
pulse of every digit. A governor reaches its speed after the first pulse;
a mechanism with no transient was typed in, not modelled.

## Acceptance criteria

- [ ] Rate and ratio, per pulse, within tolerance; expected to fail until the dial lands
