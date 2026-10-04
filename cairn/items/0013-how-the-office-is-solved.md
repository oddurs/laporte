---
id: 13
uid: a8763418-64dc-4809-ba0c-cbb334444a5f
title: How the office is solved
type: spike
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
owner: oddurs
labels:
- decision
- foundation
- thesis
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
priority: p0
role: systems
area: circuit
effort: m
---

## Question

How do the parts of the office learn what the wire is doing?

## Why it has to be answered first

House rule 1 — everything is the wire — is only true if there is a wire. The
representation chosen here is in the signature of every piece of apparatus
and is the most expensive decision in the project to change.

## Options

**One netlist, solved by modified nodal analysis** (recommended). The whole
office — battery, every relay coil, every contact, every pair of copper,
every telephone — is a list of two-terminal elements between named nodes.
Each tick, every element stamps its conductance (and, for capacitors and
inductors, a companion source carrying its history) into one matrix, the
matrix is solved, and every element reads the voltage across it and the
current through it. A closed contact is a small resistance; an open one is
not stamped. Floating nodes get SPICE's `GMIN`, a tiny conductance to
earth, and the reason is written down. Ho, Ruehli and Brennan published MNA
in 1975; Nagel's SPICE made it universal. Dense Gaussian elimination: an
office of a few dozen nodes is a matrix nobody needs to be clever about.

This is the only option in which "everything is the wire" is literally
true: apparatus is constructed with node names and nothing else, so it
cannot hold a reference to other apparatus, and the compiler enforces rule 1.

**Hand-written mesh equations per configuration.** Each state of the switch
train is a known small circuit; write its solution out. Fastest and most
readable for v0.1 — and wrong by v0.3, when contacts change the topology
in more ways than anyone will enumerate by hand.

**Message passing between components.** A phone tells the relay it is off
hook. Rejected: this is precisely the program the thesis forbids.

## What would settle it

A prototype of the netlist solving the v0.1 loop and the v0.3 transmission
bridge (two loops, AC-coupled through capacitors), at the clock rate the
clock spike chooses, faster than real time by at least 20× for a dozen
subscribers on one core.

## Answer

*Proposed by systems; the owner ratifies (label `decision` stays).*

**Recommendation.** One netlist, modified nodal analysis, dense Gaussian
elimination, `GMIN` to ground: the representation is sound, and the prototype
reproduces the hand calculations (below). **But the speed target is only just
met, and only under a condition the item did not state.** Solving the dense
matrix from scratch every tick is **3.4-4x** real time for twelve subscribers at
8000 Hz, a **FAIL** against the 20x bound set in advance (median 4.3x, min 3.4x,
max 4.3x over 7 runs). With the factorisation kept and reused while the matrix is
unchanged (back-substitution only), or with the office solved one connected
component at a time, the median over 7 runs is 21.7x (min 18.5x, max 22.2x):
**on the bound within noise**, neither a clear pass nor a clear fail. The same
case has been seen at 2.7x on a heavily loaded box, so these figures are
load-sensitive and the order of magnitude is the claim, not the digit.
At the clock rate spike 0014 recommends, it is far worse (see below and 0014).
So: adopt MNA, and write the solver's interface so that a part changes its stamp
**only on a state change** (rule 4 already has parts act after measuring, so
this is natural) and the solver can reuse its factorisation. Do not promise 20x
until the clock is decided.

### What was built

`apps/spikes/mna.hpp` (netlist, companion models, dense LU with partial
pivoting, one cached factorisation per integration method) and
`apps/spikes/solve.cpp` (the three experiments). Not wired into the Makefile.
Build and run:

    c++ -std=c++23 -O2 -Iapps/spikes apps/spikes/solve.cpp -o /tmp/solve && /tmp/solve
    /tmp/solve 48000     # section 3 at another tick rate

Each subscriber: battery, 0.2 H coil, 200 ohm winding, 150 ohm copper, the set
(300 ohm plus a voltage source standing in for the transmitter), 150 ohm copper,
200 ohm winding, 0.2 H coil, earth. Placeholder values of the right order, not
ledger figures. A call is two 2 uF capacitors, tip to tip and ring to ring,
between two subscribers' office ends: the AC-coupled transmission bridge.
Twelve subscribers and six calls make 98 unknowns (85 non-ground nodes, 13
source currents).

### Measured (clang 21, -O2, one core, Apple silicon, other agents running)

```
== 1. the v0.1 loop alone: battery, line relay, copper, set ==
  matrix 10 x 10
  loop current after 0.5 s: 48.000000 mA   by hand (48 V / 1000 ohm): 48.000000 mA
  time to 63.2% of final: 0.500 ms   by hand (L/R = 0.400 ms; tick is 0.125 ms)

== 2. two loops AC-coupled through 2 uF a side (transmission bridge) ==
  trap    8000 Hz: DC loop B 48.271 mA, A talks 1 V @1 kHz, B hears 0.2412 V peak (-12.35 dB)
  trap   48000 Hz: DC loop B 48.283 mA, A talks 1 V @1 kHz, B hears 0.2403 V peak (-12.38 dB)
  BE      8000 Hz: DC loop B 48.221 mA, A talks 1 V @1 kHz, B hears 0.2112 V peak (-13.50 dB)
  BE     48000 Hz: DC loop B 48.273 mA, A talks 1 V @1 kHz, B hears 0.2350 V peak (-12.58 dB)

== 3. twelve subscribers, six calls, one netlist, 8000 Hz, trapezoidal ==
  A. refactor every tick (matrix changes every tick)
    matrix 98 x 98, 32000 factorisations, 1.178 s wall for 4.0 s of office: 3.4x real time (36.80 us/tick)
  B. refactor every 80th tick (a contact somewhere, ~10 pulses/s * 8)
    matrix 98 x 98, 400 factorisations, 0.202 s wall for 4.0 s of office: 19.8x real time (6.30 us/tick)
  C. factor once, back-substitute (matrix unchanged)
    matrix 98 x 98, 1 factorisations, 0.194 s wall for 4.0 s of office: 20.7x real time (6.05 us/tick)
    sanity: sub 0 loop current 48.060 mA
  D. six 26-unknown components, refactor every tick
    0.190 s wall for 4.0 s of office: 21.0x real time (5.95 us/tick)

  scaling, mode A (refactor every tick) and mode C (cached), 1 s of office:
      6 subscribers,   50 unknowns: A     18.3x   C     58.3x
     12 subscribers,   98 unknowns: A      4.0x   C     16.7x
     24 subscribers,  194 unknowns: A      1.1x   C      5.4x
     48 subscribers,  386 unknowns: A      0.3x   C      1.3x
     96 subscribers,  770 unknowns: A      0.1x   C      0.3x
```

Reading the numbers:

- The loop current lands on 48.000000 mA, the hand figure. The 63.2% time is
  0.500 ms against a hand L/R of 0.400 ms because it is read at tick boundaries
  0.125 ms apart: 0.375 ms has not reached the level and 0.500 ms has.
- The bridge passes speech: B hears A at about -12.4 dB at 1 kHz, with 8000 Hz
  and 48000 Hz trapezoidal agreeing to 0.03 dB. Backward Euler at 8000 Hz is
  1.1 dB off (more in 0014). (The DC figure for loop B, 48.27 mA, has the
  speech riding on it at the instant it is read.)
- Cost is dominated by the matrix: about 6 us per tick for 98 unknowns when
  cached, 30-37 us refactored. The cube is visible: 194 unknowns is 5x real
  time cached and 1x refactored; 386 is 1.3x and 0.3x. A real office has more
  nodes per subscriber than this prototype (the selector train, relay coils and
  contacts), so 98 is a floor for twelve subscribers.
- At other tick rates (medians in the table below): 32000 Hz is 5.4x cached
  (1.1x refactored); 48000 Hz is 3.6x cached (0.7x refactored).
- Seven runs of `solve <rate>` each, back to back, on a 10-core machine that
  was not idle (1-minute load average 4.2 to 7.3 during the runs; other agents
  were building and running). x real time, 12 subscribers, 98 unknowns:

```
32000 A min 1.1 median 1.1 max 1.1 n=7
32000 B min 4.2 median 5.2 max 5.3 n=7
32000 C min 5.3 median 5.4 max 5.6 n=7
32000 D min 4.6 median 6.1 max 6.4 n=7
48000 A min 0.6 median 0.7 max 0.7 n=7
48000 B min 3.1 median 3.4 max 3.5 n=7
48000 C min 2.6 median 3.6 max 3.7 n=7
48000 D min 3.7 median 4.2 max 4.2 n=7
8000 A min 3.4 median 4.3 max 4.3 n=7
8000 B min 14.8 median 20.2 max 21.0 n=7
8000 C min 18.5 median 21.7 max 22.2 n=7
8000 D min 16.7 median 24.5 max 25.4 n=7
loadavg1 before/after across runs: min 4.2 max 7.3
```

  (A refactor every tick; B refactor every 80th tick; C factor once;
  D six independent 26-unknown components, refactor every tick.) The spread is
  wide in B and D at 8 kHz (14.8-21.0, 16.7-25.4) because they are the cases the
  scheduler can disturb most; an earlier loaded run gave the same case as low
  as 2.7x. Quote the median and the range, not a single run.

### What this does not settle

- The carbon transmitter's resistance varies every tick on a talking line,
  which is a matrix change every tick: the 3.4x case. An idea, **not tested
  here**: model it as a fixed series resistance and a voltage source of
  -dR(t) x I(previous tick). Rule 4 already means the part acts on the previous
  tick's current, and the matrix then stays constant. It needs its own spike or
  the transmitter item.
- A sparse factorisation was not tried. The matrix has about four non-zeros a
  row, while the dense cost is the cube of the size and the office will grow.
  The prototype's per-element bookkeeping (`std::function` waves, a vector of
  structs) was not optimised, so both the 20x and the 3.4x describe an
  unoptimised kernel.
- Per-component solving was measured by building six separate nets, not by a
  component finder. It is equivalent only because the battery is ideal (zero
  impedance), so no call couples to another through it. A battery with internal
  resistance couples every subscriber and removes this saving.
- No contact, selector or relay was in the speed runs. The "refactor every 80th
  tick" rate is **invented**: a figure I made up to stand in for contact
  activity, not derived from dial pulses or any ledger entry.
- Pre-warping (choosing the integrator's time scale so that trapezoidal is exact
  at one chosen frequency) was not tried; see 0014. It would not rescue a whole
  band.
- An open contact in series with a coil leaves it floating on `GMIN`, a
  stiffness no integrator copes with unaided; see the switched-inductor table
  in 0014.

### The other two options

Hand-written mesh equations and message passing were not prototyped. The first
was rejected in the item because it cannot scale to the switch train, which
nothing here contradicts; the second contradicts rule 1.

## 2026-10-03

Director's disposition (2026-10-04), provisional until the owner reads it: ADOPTED on the owner's blanket delegation. MNA on one netlist, dense LU, GMIN to ground, with the factorisation reused while the matrix is unchanged. Inspector PASS after one return.

## Result

MNA on one netlist, dense LU, GMIN, factorisation reused. Provisional until the owner reads it. Inspector PASS after one return.
