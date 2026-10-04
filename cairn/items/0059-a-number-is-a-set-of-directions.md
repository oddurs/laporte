---
id: 59
uid: 2e3d844e-abfb-4a43-96f5-8ec6e183eb7f
title: A number is a set of directions
type: verify
status: backlog
milestone: v0.3
labels:
- thesis
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

There is no table from numbers to telephones in the office. A subscriber's
number is the route to their pair through the switches, and moving their
pair on the frame moves their number with it.

## How it is checked

Behaviour: build the office twice, with one jumper moved; the same dialled
digits ring a different telephone, and `spec` reports the changed number.

Construction: `include/` contains no associative container, and no
decimal literal that is a subscriber's number. The second is enforced by
a script in the check that the inspector can read, not by trust.

## What failure looks like

A convenience map "just for the instruments" inside `include/`. Instruments
derive numbers by reading shaft positions and the frame; if that is
awkward, the instrument is wrong, not the rule.

## Acceptance criteria

- [ ] Both halves; expected to fail until the frame lands
