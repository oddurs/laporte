---
id: 73
uid: 3d3442ce-b8ef-424b-8ffd-7edade937af0
title: './laporte listen: speak into one telephone, hear the other'
type: instrument
status: backlog
milestone: v0.4
depends_on:
- 62
- 71
- 72
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: instrument
effort: m
---

## What it witnesses

A conversation.

## What it prints

For a call with sound files spoken into each transmitter: what each
receiver produced, as WAV, including sidetone. With `--loop` to change the
cable length and hear the voice get fainter.

## Acceptance criteria

- [ ] `./laporte listen --in voice.wav` renders both ends
