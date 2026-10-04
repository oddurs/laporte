---
id: 10
uid: 6fbad552-a77a-49af-b312-fd594fc90941
title: The numbers we will be judged against
type: spike
status: planned
milestone: v0.1
owner: oddurs
labels:
- decision
- foundation
depends_on:
- 9
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: l
---

## Question

For every figure a check will be judged against, what is the figure, and
where did somebody publish it?

## Why it has to be answered first

House rule 5: every claim is checked against something outside the
project. A check judged against a number nobody can trace is a check written
to pass, and the team writing it is a language model that will produce a
plausible figure on request. So nothing enters a check unless it is in the
ledger, and nothing enters the ledger unless somebody fetched the source.

## Options

There is one option; the question is the contents. Write `docs/sources.md`:
one entry per figure, each with a stable handle (`S01`, `S02`, …), the
figure and its units, the source (title, author or issuing body, date,
section or page), a URL that was actually fetched, and the sentence quoted.

Where to look, in order of preference: Bell System Practices (many are on
the Internet Archive and telecom-history mirrors); the Bell System Technical
Journal; *Notes on Distance Dialing* (AT&T, 1956 and later editions); *A
History of Engineering and Science in the Bell System*; ITU/CCITT
recommendations (G.711); patents (Strowger, US 447,918).

The figures v0.1–v0.5 need at minimum: central-office battery voltage and
polarity; the resistance-design loop limit; copper resistivity and the AWG
definition; line-relay operate and release currents; dial speed and
break/make ratio, and their tolerances; ringing voltage, frequency and
cadence; the tone plan of the chosen year; 500-set DC resistance and
ringer components; SF frequency, level and guard behaviour; the voice-band
edges; mu-law's definition.

An entry may say **unsourced** — the figure is a commonly quoted value we
could not trace. A check may use an unsourced figure only if the README
says so next to the result.

## What would settle it

Every figure above has an entry, and the owner has read the ledger.

## Answer
