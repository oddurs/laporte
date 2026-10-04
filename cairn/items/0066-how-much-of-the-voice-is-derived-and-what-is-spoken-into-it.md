---
id: 66
uid: 5a32e766-92a4-4f3b-988a-a8d19f959cf7
title: How much of the voice is derived, and what is spoken into it
type: spike
status: backlog
milestone: v0.4
owner: oddurs
labels:
- decision
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: transmission
area: transmission
effort: m
---

## Question

Which parts of the voice path are physics, which are fits, and what sound
goes in?

## Why it has to be answered first

It is the milestone where "derive, never declare" is most tempting to
abandon: a 300–3400 Hz bandpass sounds exactly like a telephone and is a
lie. The rule is that the response falls out of the parts or the model
says plainly which part is a fit.

## Options

*Transmitter.* A carbon granule resistance varying with diaphragm pressure:
`R(p) = R₀ − k·p`, with the nonlinearity that gives carbon its sound,
powered by loop current. Its diaphragm as a single mechanical resonance —
a fit to a published response, named as one (house rule 8: a law and a
fit must not be spelled the same way).

*Receiver.* A moving-iron receiver as an inductance and a resistance with
one mechanical resonance. Same treatment.

*Network.* The 500 set's induction-coil hybrid in full, as windings and a
balance network in the netlist — the derivation is the point of the
milestone.

*Input.* A short public-domain recording of speech shipped in the
repository (LibriVox recordings are public domain in the US), plus `--in`
for your own; synthesized vowels as a fallback if the owner does not want
audio in the repository. v0.5's guard check needs some minutes of speech.

## What would settle it

The owner agrees what is fitted, and the input.

## Answer
