---
id: 38
uid: e6b685fc-ced6-4242-bafd-6013906d1a93
title: Tap the hook five times and you have dialled five
type: verify
status: backlog
milestone: v0.2
labels:
- thesis
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

A hand that taps the switchhook at about ten times a second raises the
selector exactly as the dial would. Nobody wrote this.

## Judged against

The dial check's tolerances, and the folklore — to be sourced if a source
exists, stated as folklore if not.

## How it is checked

A scenario with five taps; read the shaft height. Then the same at a
speed outside tolerance.

## What failure looks like

A pass that depends on code that knows about tapping. The inspector greps
for it.

## Acceptance criteria

- [ ] Five taps, level five; expected to fail until the selector lands
