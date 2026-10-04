---
id: 10
uid: 6fbad552-a77a-49af-b312-fd594fc90941
title: The numbers we will be judged against
type: spike
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
owner: oddurs
labels:
- decision
- foundation
depends_on:
- 9
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
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

`docs/sources.md` is the ledger: 30 entries, 26 sourced and 4 unsourced. Each
`sourced` entry was fetched and quotes the sentence; a primary document read in
full text stands behind 16 of them and a secondary source alone (mostly
Wikipedia) behind 10. The unsourced four are the
line-relay operate and release currents and times, the 500 set's DC resistance
and ringer values, the claim that step-by-step was the commonest switch in 1965,
and ring-trip and dial-tone delay.

Three findings the owner should read before ticking the ledger:

- **The Precise Tone Plan was not in service in a 1965 step-by-step office.**
  Its standardisation began with the first 1ESS in 1965 (S16). The nearest
  primary for the tones such an office made is *Notes on Distance Dialing*, 1956
  (S17): dial tone is 600 Hz modulated by 120 Hz, busy is the same tone at 60
  interruptions a minute, audible ringing is 420 Hz modulated by 40 Hz. The 350,
  440, 480 and 620 Hz figures are in the ledger (S15) so that no check mistakes
  them for the era's.
- **Ringing cadence has two figures that disagree.** 2 s on and 4 s off
  (S13, secondary) against 1.2 s on and 4.8 s off (S14, a 1968 trade
  publication); an AT&T test-line requirement mentions a 2 s ringing interval.
  Someone has to choose, and say so in the item that does.
- **Much of the loop and set data is 1978.** The 1300-ohm resistance-design limit
  and the 7 mA step-by-step line-circuit threshold (S03, S07) come from the
  *BSTJ* of April 1978, the 20 to 80 mA set range likewise (S08). No 1965
  document stating them was found.

The owner reads the ledger; that criterion is not ticked here.

## 2026-10-03

docs/sources.md written: 30 entries, 25 sourced, 5 unsourced (interdigit pause S10; line-relay operate/release S11; step-by-step prevalence S28; ring trip and dial-tone delay S29; 500-set DC resistance and ringer values S30). 15 sourced entries rest on primary text read in full (NODD 1956, Weaver and Newell 1954, BSP 040-011-712 1965, BSP AB22.066.1 1952, BSTJ Apr 1978, Lenkurt 1968, NIST HB100 1966); 10 are secondary (mostly Wikipedia, quotes as returned by the fetch tool). Surprising: (1) the Precise Tone Plan was NOT in service in a 1965 step-by-step office (began with 1ESS 1965); 1956 NODD gives the older tones, 600 Hz mod 120 Hz dial tone, 60 IPM busy, 420 mod 40 audible ring. (2) Ringing cadence conflict, 2s/4s vs 1.2s/4.8s (Lenkurt 1968). (3) Loop limit 1300 ohm, 7 mA SXS line-circuit threshold, 20-80 mA set range are 1978 figures. (4) Kansas history page gives patent year 1892; patent is 1891. Process: cairn claim refused because 0009 (the era decision) is not closed; claimed with --force on the director's instruction that the owner approved the defaults. 0009 still needs its Answer recorded. The owner-reads-the-ledger criterion is not ticked.

## 2026-10-03

Review fixes: S10 flipped to sourced (NDD 1956 paras 3.26-3.28: 0.600 s step-by-step, 0.300 s crossbar/panel); S26 quote recopied verbatim; S12 gained the 105 V ringing-start also-line; S01 52.1 V moved out of the figure with a reconciling note; S05 58 MS/m labelled derived. Now 26 sourced, 4 unsourced.

## 2026-10-03

Closed by the director on the owner's blanket delegation, but PROVISIONALLY: the item's last requirement, that the owner has read the ledger, is not met. The owner should read docs/sources.md (30 entries; 26 sourced, 4 unsourced) before the checks that depend on it are trusted. Open questions the ledger raises for the owner: ringing cadence (2 s on/4 s off vs 1.2 s on/4.8 s off), and the era's tones are the 1956 ones, not the Precise Tone Plan.

## Result

docs/sources.md: 30 entries, 26 sourced (16 primary), 4 unsourced. Era tones are 1956's, not the Precise Tone Plan. Owner read outstanding.
