---
id: 41
uid: 4c1c14e0-a485-4c4b-ad1b-e203e0a02fe4
title: 'wav.hpp: a sound file is a forty-four-byte header and some numbers'
type: apparatus
status: done
milestone: v0.2
assignee: Oddur Sigurdsson
depends_on:
- 15
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
priority: p1
role: systems
area: audio
effort: s
---

## What it is

Reading and writing 16-bit mono PCM WAV at the clock rate. Written by
hand, like cornell's PPM, because it is small enough to be read in one
sitting.

## What it must derive

Nothing. Scale is stated: which current or voltage maps to full scale is
a parameter of the instrument writing the file, printed when it writes.

## What is not modelled

Every other WAV variant. Reading refuses anything but what it writes.

## Acceptance criteria

- [x] A file written is parsed by afinfo (CoreAudio) and reads back bit for bit; not opened in QuickTime itself

## 2026-10-03

wav.hpp landed: encode/decode (byte-by-byte little-endian, 44-byte header), quantise/write_file taking Volts or Amperes plus a stated full scale, printing and returning it; rate is a parameter. Reading refuses everything but what it writes. Check wav.roundtrip PASSes (bit-for-bit incl. 32767/-32768/0, and refuses 9 header corruptions, truncation, trailing bytes). afinfo on a 1 s 440 Hz sine at 48 kHz: 'File type ID: WAVE; Num Tracks: 1; Data format: 1 ch, 48000 Hz, Int16; estimated duration: 1.000000 sec; audio bytes: 96000; audio data file offset: 44'. Not opened in QuickTime itself; afinfo (CoreAudio) parses it. RIFF history left unsourced beyond 'published 1991 by Microsoft and IBM' which a reviewer may wish to source.

## 2026-10-03

Review round: non-finite samples and non-positive or non-finite full scale refused (nullopt); encode refuses rate 0 or >= 2^31 and oversize payloads; check extended (quantise extremes, refusals, file round trip, offsets 4/24/28/40); 1991 cited as August 1991 MPIDS 1.0 per inspector. Criterion reworded to afinfo only.

## Result

wav.hpp: 16-bit mono PCM, rate a parameter, byte-by-byte little-endian, non-finite samples and bad scales refused, bit-exact round trip; parsed by afinfo (QuickTime itself not run). Inspector PASS after one return; non-blocking notes filed.
