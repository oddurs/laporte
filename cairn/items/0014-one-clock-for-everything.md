---
id: 14
uid: 72be446d-11ba-4154-8d0b-bdd50e05127b
title: One clock for everything
type: spike
status: review
milestone: v0.1
assignee: Oddur Sigurdsson
claimed: 2026-10-03
owner: oddurs
labels:
- decision
- foundation
depends_on:
- 13
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: circuit
effort: m
---

## Question

At what rate does the office tick, and how are capacitors and inductors
advanced from one tick to the next?

## Why it has to be answered first

Relays operate in milliseconds, dial pulses are 100 ms long, ringing is
20 Hz, the voice reaches 3.4 kHz and SF signalling sits at 2.6 kHz. One
clock has to carry all of them, because two clocks would need something
to translate between them, and that something would be a message.

## Options

**8000 Hz** (recommended if it passes). The rate of a digital telephone
channel; v0.6's trunk is a stream at exactly this rate, so the office and
the network share a heartbeat and nothing resamples. The question is
whether 3.4 kHz and 2.6 kHz survive integration at 8 kHz.

**48000 Hz, decimated to 8000 at the trunk.** Six times the work, and a
filter at the boundary that has to be explained.

Integration: **backward Euler** is unconditionally stable and damps high
frequencies; **trapezoidal** preserves them and can ring on a switched
contact. The likely answer is trapezoidal with a backward-Euler step on
the tick a contact changes state, which is what SPICE does — but measure.

## What would settle it

Drive an RC and an LC through a 20 Hz, a 1 kHz, a 2.6 kHz and a 3.4 kHz
sine at each candidate rate and method; tabulate amplitude and phase error
against the exact answer. Pick the cheapest option that keeps every error
under a figure written down before measuring (proposed: 0.5 dB, 5°).

## Answer

*Proposed by systems; the owner ratifies (label `decision` stays).*

**Recommendation: the 8000 Hz option does not pass, and I propose that the
office does not tick at 8000 Hz.** Against the bound written before measuring
(0.5 dB and 5 degrees, every cell), the cheapest measured rate that passes is
**32000 Hz trapezoidal** (worst cell 0.31 dB, 4.08 degrees: a pass with little
margin on phase). 48000 Hz trapezoidal passes comfortably (0.13 dB, 1.77
degrees). 24000 Hz fails at 3.4 kHz (0.56 dB, 7.5 degrees). 8000 Hz fails badly
under both methods (trapezoidal 17 dB and 84 degrees, backward Euler 6.9 dB and
64 degrees), and it fails at 2.6 kHz too, not only at the band edge
(trapezoidal 3.7 dB and 46 degrees). **Backward Euler fails at every rate
tested**, 48 kHz included (2.9 dB, 11.6 degrees), so it cannot be the method.
**Trapezoidal with a single backward-Euler step on a contact change does not stop
the ringing** the item feared (second table): it takes several backward-Euler
ticks, and how many depends on the tick rate and the coil.

**The cost is a conflict with 0013 that the owner must see.** The solver
spike's 20x speed bound is on the line at 8000 Hz (median 21.7x over 7 runs,
range 18.5-22.2x, with a cached factorisation; a loaded run gave 2.7x). At
32000 Hz the same prototype manages a median 5.4x, at 48000 Hz 3.6x, and
1.1x and 0.7x if the matrix is refactored every tick. Accuracy and speed
cannot both be had on this prototype for twelve subscribers. The owner's choices:

1. **32 kHz trapezoidal**: the strict pass, thin on phase, about 5x real time,
   4 to 1 decimation at the trunk.
2. **48 kHz trapezoidal**: a clear pass, 6 to 1 decimation, about 3.6x.
3. **Relax the bound.** Read from the table, 24 kHz passes if 3.4 kHz is left
   out (worst of the 20 Hz, 1 kHz and 2.6 kHz cells: 0.30 dB, 2.9 degrees). That
   says the model is good to 2.6 kHz, true of SF signalling and untrue of the
   voice band's top edge. I do not recommend doing it silently.
4. **Make the kernel faster** (sparse factorisation, a constant matrix) and keep
   48 kHz. Plausible, **unmeasured**.

My proposal is 2 (48 kHz), backed by 4 if it must run faster: what this project
trades in is that the model is believed, and a 0.13 dB, 1.8 degree steady-state
error is the only comfortable row in which 3.4 kHz is honestly present.

### What was built

`apps/spikes/clock.cpp` on `apps/spikes/mna.hpp` (shared with 0013). Not wired
into the Makefile:

    c++ -std=c++23 -O2 -Iapps/spikes apps/spikes/clock.cpp -o /tmp/clock && /tmp/clock

The test circuits put a corner or a resonance inside the band deliberately: an
RC (1 kohm, 159.155 nF, corner 1 kHz) and an RLC low-pass (300 ohm source, 300
ohm characteristic impedance, resonant at 4 kHz, Q = 1: 11.94 mH and 0.1326 uF).
Output is the voltage on the capacitor. Each run drives the sine for a second to
settle, then correlates input and output over a second that holds a whole number
of cycles of every test frequency at every rate; the amplitude ratio and phase
difference are compared with the exact H(jw). Component values are
placeholders, not ledger figures.

**A correction to the first version of this note**, which said that a corner
placed far below the band would show smaller errors and that these circuits were
the hard cases. That was backwards. Trapezoidal integration does not misplace
the circuit's response, it warps the frequency axis, w -> (2/h) tan(wh/2),
whatever the circuit is; at 8 kHz and 3.4 kHz, wh/2 = 1.34 rad and tan is 4.1.
So the corner position is not what saves you. The sweep in the first table
(RC corner from 30 Hz to 10 kHz, the extra rows) shows it: at 8 kHz
trapezoidal the amplitude error is -9.9 dB at 3.4 kHz and -4.1 dB at 2.6 kHz
for corners of 30 and 100 Hz, -9.6 dB for a 1 kHz corner, and a 10 kHz corner
still gives -2.8 dB and -27.9 degrees at 3.4 kHz. Every 8 kHz trapezoidal cell
in the sweep fails the bound. So "8 kHz fails" is **more general** than the two
circuits above first suggested: it is a property of the tick rate, not of my
choice of circuit. At 2.6 and 3.4 kHz, 32 kHz and 48 kHz pass every corner in
the sweep; 16 kHz fails at 3.4 kHz for every corner up to 3 kHz and at 2.6 kHz
for every corner up to 1 kHz, passing only the 10 kHz corner and, at 2.6 kHz,
the 3 kHz corner.

### Accuracy, measured

```
Pass bound (set before measuring): |amplitude error| <= 0.5 dB and |phase error| <= 5.0 deg, every cell.
Cells are amplitude error dB / phase error deg, simulated minus exact. * marks a cell outside the bound.
In steady state no contact changes, so trap+BE-on-switch is the trap column.

RC (corner 1 kHz)
  f (Hz)      8000 BE           8000 trap        16000 BE          16000 trap        24000 BE          24000 trap        48000 BE          48000 trap     
  20          -0.00/  +0.00     -0.00/  -0.00     -0.00/  +0.00     -0.00/  -0.00     -0.00/  +0.00     -0.00/  -0.00     -0.00/  +0.00     -0.00/  -0.00 
  1000        -1.30/ +11.74*    -0.24/  -1.53     -0.75/  +5.78*    -0.06/  -0.37     -0.52/  +3.82*    -0.02/  -0.16     -0.27/  +1.89     -0.01/  -0.04 
  2600        -0.84/ +47.27*    -3.72/  -7.51*    -0.94/ +24.55*    -0.71/  -1.72*    -0.74/ +16.56*    -0.30/  -0.75     -0.43/  +8.38*    -0.07/  -0.19 
  3400        +0.22/ +63.98*    -9.56/ -11.00*    -0.73/ +33.62*    -1.35/  -2.40*    -0.67/ +22.75*    -0.56/  -1.04*    -0.43/ +11.55*    -0.13/  -0.26 

RLC (f0 4 kHz, Q 1)
  f (Hz)      8000 BE           8000 trap        16000 BE          16000 trap        24000 BE          24000 trap        48000 BE          48000 trap     
  20          -0.00/  +0.00     -0.00/  -0.00     -0.00/  -0.00     -0.00/  -0.00     -0.00/  -0.00     -0.00/  -0.00     -0.00/  -0.00     -0.00/  -0.00 
  1000        -0.97/  +0.68*    +0.03/  -0.89     -0.49/  -0.15     +0.01/  -0.21     -0.33/  -0.21     +0.00/  -0.09     -0.16/  -0.16     +0.00/  -0.02 
  2600        -5.76/ +29.50*    -1.57/ -45.98*    -4.17/  +9.99*    +0.03/  -7.04*    -3.09/  +4.69*    +0.02/  -2.88     -1.69/  +1.00*    +0.01/  -0.69 
  3400        -6.86/ +62.61*   -17.35/ -84.35*    -6.16/ +29.59*    -1.00/ -18.51*    -4.90/ +18.14*    -0.31/  -7.50*    -2.93/  +7.69*    -0.06/  -1.77 

Worst cell over both circuits and all four frequencies:
    8000 Hz BE      6.86 dB   63.98 deg   FAIL
    8000 Hz trap   17.35 dB   84.35 deg   FAIL
   16000 Hz BE      6.16 dB   33.62 deg   FAIL
   16000 Hz trap    1.35 dB   18.51 deg   FAIL
   24000 Hz BE      4.90 dB   22.75 deg   FAIL
   24000 Hz trap    0.56 dB    7.50 deg   FAIL
   48000 Hz BE      2.93 dB   11.55 deg   FAIL
   48000 Hz trap    0.13 dB    1.77 deg   PASS

Worst cell with 3.4 kHz left out (20 Hz, 1 kHz, 2.6 kHz only):
    8000 Hz BE      5.76 dB   47.27 deg   FAIL
    8000 Hz trap    3.72 dB   45.98 deg   FAIL
   16000 Hz BE      4.17 dB   24.55 deg   FAIL
   16000 Hz trap    0.71 dB    7.04 deg   FAIL

Intermediate multiples of 8 kHz, trapezoidal, worst cell over both circuits and all four frequencies:
   32000 Hz trap    0.31 dB    4.08 deg   PASS
   40000 Hz trap    0.19 dB    2.57 deg   PASS

RC corner sweep, amplitude dB / phase deg error at 2.6 kHz and 3.4 kHz (* outside the bound):
  corner Hz  f (Hz)      8000 trap       16000 trap       32000 trap       48000 trap       8000 BE
  30         2600         -4.07/  -0.25*    -0.80/  -0.06*    -0.19/  -0.01     -0.08/  -0.01     +1.46/ +58.25*
  30         3400         -9.88/  -0.34*    -1.44/  -0.08*    -0.33/  -0.02     -0.15/  -0.01     +2.65/ +76.15*
  100        2600         -4.07/  -0.82*    -0.80/  -0.19*    -0.19/  -0.05     -0.08/  -0.02     +1.23/ +57.62*
  100        3400         -9.88/  -1.14*    -1.44/  -0.26*    -0.33/  -0.06     -0.14/  -0.03     +2.42/ +75.34*
  300        2600         -4.04/  -2.45*    -0.79/  -0.58*    -0.19/  -0.14     -0.08/  -0.06     +0.64/ +55.61*
  300        3400         -9.85/  -3.42*    -1.43/  -0.77*    -0.33/  -0.19     -0.14/  -0.08     +1.82/ +72.91*
  1000       2600         -3.72/  -7.51*    -0.71/  -1.72*    -0.17/  -0.42     -0.07/  -0.19     -0.84/ +47.27*
  1000       3400         -9.56/ -11.00*    -1.35/  -2.40*    -0.31/  -0.58     -0.13/  -0.26     +0.22/ +63.98*
  3000       2600         -2.22/ -13.26*    -0.36/  -2.64     -0.08/  -0.63     -0.04/  -0.28     -1.97/ +27.75*
  3000       3400         -7.72/ -25.63*    -0.87/  -4.66*    -0.19/  -1.08     -0.08/  -0.47     -1.58/ +42.48*
  10000      2600         -0.41/  -7.99*    -0.06/  -1.34     -0.01/  -0.31     -0.01/  -0.14     -1.23/  +9.11*
  10000      3400         -2.80/ -27.91*    -0.17/  -3.10     -0.04/  -0.68     -0.02/  -0.29     -1.41/ +16.11*

```

In steady state no contact changes, so "trapezoidal with backward Euler on a
contact change" is the trapezoidal column.

### Switched inductive contact, measured

```
== Switched inductive contact ==
48 V -> contact -> n1 -> [400 ohm winding + 0.2 H coil] -> earth, with a quench/leakage resistor Rq from n1 to earth.
The contact opens at t = 0. Exact: v(n1) = -I0*Rq*exp(-t/tau), I0 = 120 mA, tau = L/(Rw+Rq).
v1 = first tick after opening. ring = peak |v| over ticks 6..60 (exact is ~0 there, except Rq=1k where tau is about one tick). tail = peak |v| over ticks 500..600: has it decayed or does it persist. flips = sign changes in ticks 1..600.

  Rq       method                       v1 (V) exact v1 (V)     ring (V)     tail (V)  flips
  -- tick rate 8000 Hz --
  1k       backward Euler               -63.98       -50.01        2.761    1.371e-08      1
  1k       trapezoidal                  -83.45       -50.01       0.7656    1.371e-08      1
  1k       trap + BE 1 tick             -63.98       -50.01        0.587    1.371e-08      1
  1k       trap + BE 2 ticks            -63.98       -50.01          0.8    1.371e-08      1
  1k       trap + BE 4 ticks            -63.98       -50.01        1.486    1.371e-08      1
  1k       trap + BE 8 ticks            -63.98       -50.01        2.761    1.371e-08      1

  10k      backward Euler                 -160       -1.804     0.006741    1.846e-08      1
  10k      trapezoidal                  -282.3       -1.804        11.74    1.846e-08     37
  10k      trap + BE 1 tick               -160       -1.804        6.652    1.846e-08     35
  10k      trap + BE 2 ticks              -160       -1.804        1.675    1.846e-08     33
  10k      trap + BE 4 ticks              -160       -1.804       0.1063    1.846e-08     27
  10k      trap + BE 8 ticks              -160       -1.804     0.006741    1.846e-08     13

  100k     backward Euler               -188.2   -6.716e-24    1.596e-07    1.912e-08      1
  100k     trapezoidal                  -370.6   -6.716e-24        269.4    1.913e-08    371
  100k     trap + BE 1 tick             -188.2   -6.716e-24        136.8    1.913e-08    361
  100k     trap + BE 2 ticks            -188.2   -6.716e-24        2.287    1.912e-08    295
  100k     trap + BE 4 ticks            -188.2   -6.716e-24    0.0006394    1.912e-08    165
  100k     trap + BE 8 ticks            -188.2   -6.716e-24    1.596e-07    1.912e-08      1

  GMIN     backward Euler                 -192           -0     1.92e-08     1.92e-08      1
  GMIN     trapezoidal                  -383.9           -0        383.9        383.9    599
  GMIN     trap + BE 1 tick               -192           -0          192        191.9    599
  GMIN     trap + BE 2 ticks              -192           -0    1.248e-06    1.248e-06    598
  GMIN     trap + BE 4 ticks              -192           -0     1.92e-08     1.92e-08      1
  GMIN     trap + BE 8 ticks              -192           -0     1.92e-08     1.92e-08      1

  -- tick rate 48000 Hz --
  1k       backward Euler               -104.7       -103.7           53    1.371e-08      1
  1k       trapezoidal                  -111.8       -103.7        53.86    1.371e-08      1
  1k       trap + BE 1 tick             -104.7       -103.7        50.43    1.371e-08      1
  1k       trap + BE 2 ticks            -104.7       -103.7        50.93    1.371e-08      1
  1k       trap + BE 4 ticks            -104.7       -103.7        51.96    1.371e-08      1
  1k       trap + BE 8 ticks            -104.7       -103.7           53    1.371e-08      1

  10k      backward Euler               -575.9       -406.1        14.67    1.846e-08      1
  10k      trapezoidal                  -778.2       -406.1        1.807    1.846e-08      1
  10k      trap + BE 1 tick             -575.9       -406.1        1.337    1.846e-08      1
  10k      trap + BE 2 ticks            -575.9       -406.1        2.159    1.846e-08      1
  10k      trap + BE 4 ticks            -575.9       -406.1        5.629    1.846e-08      1
  10k      trap + BE 8 ticks            -575.9       -406.1        14.67    1.846e-08      1

  100k     backward Euler                -1047      -0.3444     0.005301    1.912e-08      1
  100k     trapezoidal                   -1926      -0.3444        277.8    1.912e-08     65
  100k     trap + BE 1 tick              -1047      -0.3444          151    1.912e-08     63
  100k     trap + BE 2 ticks             -1047      -0.3444        19.41    1.912e-08     57
  100k     trap + BE 4 ticks             -1047      -0.3444       0.3208    1.912e-08     45
  100k     trap + BE 8 ticks             -1047      -0.3444     0.005301    1.912e-08     19

  GMIN     backward Euler                -1152           -0     1.92e-08     1.92e-08      1
  GMIN     trapezoidal                   -2303           -0         2303         2303    599
  GMIN     trap + BE 1 tick              -1152           -0         1152         1152    599
  GMIN     trap + BE 2 ticks             -1152           -0    4.424e-05    4.424e-05    598
  GMIN     trap + BE 4 ticks             -1152           -0     1.92e-08     1.92e-08      1
  GMIN     trap + BE 8 ticks             -1152           -0     1.92e-08     1.92e-08      1
```

Reading it:

- Trapezoidal rings, as feared. With the coil driving a stiff load (hR/2L well
  above 1: Rq of 10 kohm and up at 8 kHz, 100 kohm at 48 kHz) the voltage changes
  sign every tick and decays slowly (the 600-tick window): 100 kohm at 8 kHz
  flips 371 times in 600 ticks, peaks at 269 V and has fallen to the 2e-8 V
  floor by tick 500. On GMIN alone it **does not decay at all**: 599 flips in 600
  ticks, tail still 384 V at 8 kHz and 2303 V at 48 kHz, in a circuit whose exact
  answer is a single pulse.
- **One backward-Euler tick (the SPICE habit the item proposed) is not enough.**
  It halves the first-tick kick, but the next trapezoidal step inherits the
  large voltage in the inductor's history and rings just the same: 100 kohm at
  8 kHz gives 137 V against 269 V, and on GMIN 192 V against 384 V with the tail
  still at 192 V and 599 flips: undamped.
- Two ticks cut the 8 kHz ring to 2.3 V at 100 kohm, but on GMIN the alternation
  that remains (1.2e-6 V at 8 kHz, 4.4e-5 V at 48 kHz) is still undamped after
  600 ticks (598 flips), and at 48 kHz 100 kohm is 19 V. Four ticks end the
  GMIN ringing at both rates (the tail is the 1.9e-8 V floor) but leave 45-165
  flips at 100 kohm.
- **Eight ticks, stated exactly:** in every stiff case at both rates the peak
  over ticks 6..60 falls to between 1.6e-7 V and 6.7e-3 V (the 1 kohm rows and
  10 kohm at 48 kHz being genuine tails, not ringing), and by ticks 500..600 the tail is at the 2e-8 V floor, the same as
  backward Euler. The 13 and 19 sign flips still counted (10 kohm at 8 kHz, 100
  kohm at 48 kHz) are the milliVolt-scale tail dying out inside the window. The
  window is 600 ticks, which is 75 ms at 8 kHz but 12.5 ms at 48 kHz; I have not
  shown the decay is monotonic, only that it is gone by the end of the window.
  The number 8 is not derived, and the cure needs more ticks as hR/2L grows, so
  it wants a rule rather than a constant. A rule to try in 0015:
  backward Euler for as many ticks after a change as it takes the companion
  conductance to leave the stiff regime. A proposal; **not tested**.
- The 1 kohm rows (and 10 kohm at 48 kHz) are not stiff: the exact decay takes
  several ticks, so "ring" there is the true tail, not an artefact. Read the
  stiff rows.
- The GMIN row is the point. An inductor with its contact open and nothing else
  across it kicks by L*I/h volts, which depends on the tick rate (192 V at
  8 kHz, 1152 V at 48 kHz) and is not a physical number. Real relays have winding
  capacitance and contacts have quench networks; the model must supply a finite
  stiffness or say that it does not (house rule 8). That belongs in the list of
  what is not modelled at the top of `kirchhoff.hpp`, or in the relay item.

### What this does not settle

- Only an RC and an RLC, only these corners, only sines: not a switch train, a
  carbon transmitter or any nonlinear element.
- Steady state, not transient; the step response would also differ at 8 kHz.
- The frequencies are the four in the item. 32 kHz passes the bound by 0.9
  degrees of phase; a different circuit could move that either way.
- Only whole multiples of 8 kHz were tried (8, 16, 24, 32, 40, 48), so that a
  trunk could decimate by an integer. 40 kHz also passes (0.19 dB, 2.57 degrees).
- Speed figures are from the 0013 prototype, unoptimised, and load-sensitive;
  see the table of medians and ranges there. The "refactor every 80th tick" case
  uses an invented rate.
- **Pre-warping** was not tried. Trapezoidal can be made exact at one chosen
  frequency by scaling the time step (w -> (2/h) tan(wh/2) set equal to w at the
  pre-warp point). It would give an 8 kHz clock one good frequency (say 2.6 kHz
  for SF signalling) but not the band: the warp is nonlinear, so every other
  frequency stays wrong, and a pre-warp is itself a declared constant, against
  rule 2. It does not rescue "8 kHz passes".
- The decimation filter at the trunk (48 to 8 kHz) was neither built nor
  measured; its cost is what the item said it would be.

## 2026-10-03

Director's disposition (2026-10-04), provisional until the owner reads it. THE PRE-APPROVED 8 kHz CLOCK FAILED ITS OWN BOUND, so it is NOT adopted. ADOPTED: 48 kHz, trapezoidal, with eight backward-Euler ticks after a contact changes state (the number is not derived; the item records what it does and does not show). The office therefore runs at about 3.6x real time for 12 subscribers, which is above real time and enough for v0.6's live calls; the trunk to the network will decimate to 8 kHz mu-law at its boundary, so 8 kHz survives as the DS0 rate and nowhere else. The owner may overrule with 32 kHz (narrower margin, 5x) or by relaxing the 3.4 kHz bound. Open: the model must supply winding capacitance or admit it has none (an open inductor kicks to an unphysical voltage on GMIN alone).
