---
id: 90
uid: 4355b087-b2b5-4c3f-840e-ebdf0dbaccd6
title: What crosses the network, and whose clock the office runs on
type: spike
status: backlog
milestone: v0.6
owner: oddurs
labels:
- decision
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: audio
effort: l
---

## Question

How does the trunk cross a real network between two machines, each running
its own office in real time?

## Why it has to be answered first

It is the only place in the project where the outside world's time and
the office's tick meet, and the only place a dependency enters.

## Options

**Recommended.** Each machine runs one office with one telephone. The
trunk between them is a DS0: 8000 mu-law bytes a second each way, sent as
UDP datagrams of 20 ms (160 bytes) with a sequence number, through a small
jitter buffer. The office's tick is driven by the audio device's clock at
8000 Hz. The two machines' sound cards disagree about how long a second
is, so the buffer occasionally gains or loses a frame — a *slip*, which is
exactly what T1 carrier did when two offices' clocks drifted, and is
counted and reported, not hidden.

Platform audio: CoreAudio (AudioToolbox), macOS only, in one file under
`apps/platform/`, and nowhere in `include/`. It is the one sanctioned
dependency, exactly as `cie.hpp` is cornell's one sanctioned RGB. Linux
(ALSA) is `later`. A `--file` mode runs the same code with sound files
instead of a device, so CI can test it without audio.

**TCP.** Retransmission makes a late byte later; a telephone channel would
rather lose it. Rejected, with the reason in the header.

## What would settle it

A loopback prototype on one machine holds a call for ten minutes with
slips counted and no audible artefacts beyond them.

## Answer
