---
id: 24
uid: fa2c3544-4216-4e81-bfe4-55343b7be48d
title: The relay operates and releases where its own numbers say it should
type: verify
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

A relay operates when the current in its coil reaches its operate value,
releases when it falls below its release value, and takes the time its
coil's L/R says it takes to get there. A slow-release relay holds for the
time its copper slug says.

## Judged against

Closed form for the timing (`t = −τ ln(1 − I_op/I_final)`), and the ledger
for the operate and release currents of a typical line relay of the era.

## How it is checked

Step a voltage onto the coil; measure the tick the contacts change.
Remove it; measure again. Repeat for the slow-release variant.

## What failure looks like

A relay that chatters at its threshold: operate and release currents equal,
so there is no hysteresis, so noise near the threshold toggles it every
tick.

## Acceptance criteria

- [x] Operate and release times within one tick of closed form
- [x] No chatter with the coil current held at the threshold

## 2026-10-03

apps/checks/relay_timing.hpp. relay.timing: worst error in ticks, tol 1, over operate (-tau ln(1 - I_op/I_f)), release (tau ln(I0/I_rel)) and slow release (check's stated reading: decay constant tau + tau_slug; a reading, to be argued on item 25 if modelled otherwise). relay.no_chatter: a 1 % dither about the operate current from rest, and about the release current from operated, must give exactly one transition each; equal operate and release currents chatter and are caught. Operate and release currents are UNSOURCED (S11); the coil is a labelled SYNTHETIC one (400 ohm, 16 H, 12 mA / 6 mA, 100 ms slug) and each result prints 'unsourced (S11)' as its source. Both are expected to fail until item 25 and answer NaN until then; relay.selftest (passes) drives fakes of the check's own and shows a correct relay, a no-hysteresis one, a dead one and a 3-tick-late one are told apart at two tick lengths. Hook-up for item 25's author: include relay.hpp and replace the body of drive_relay(const Coil&, Seconds duration, const Schedule&) in relay_timing.hpp: build source+coil, per tick set the source from volts_at(k*tick), tick, push back contacts closed; return {tick, states}. Nothing else may change.

## 2026-10-03

Returned by the inspector; revised. (1) Slug leg now derived: a shorted turn, perfect-coupling limit, flux decays with tau_slug = L_slug/R_slug, release at t = tau_slug ln(I0/I_rel); the header says it is the limit, and that slug-delayed operate is not judged. (2) Tolerance measured, not guessed: exact exponential scores 0.5-0.8 ticks, a backward-Euler companion model with one tick of act-after-measure latency 2.3-2.5, at 48 kHz and 1 kHz; tolerance is 3 ticks, and the selftest asserts a 0.5 % wrong L/R (34 ticks at 48 kHz) and a 4-tick-late relay (4.8) fail. The latency convention is in the header. (3) relay.selftest no longer touches the stub drive_relay; the audit of copper.hpp and loop_limit.hpp found none that do (their selftests use only fakes).

## 2026-10-03

Returned again; the slug is restored to the closed-coil result. Derived in the header from the coupled L/R equations (perfect coupling, M^2 = L1 L2): flux decays with tau + tau_slug, release at (tau + tau_slug) ln(I0/I_rel). The open-coil case (tau_slug alone) is stated as the other case; the drive leaves the coil closed through the zero-volt source, so it is not the one judged. relay.selftest now includes a fake that integrates the two coupled circuits (backward Euler, 20 substeps a tick), which scores identically to the closed form at 48 kHz and 1 kHz and stays green, and an open-coil fake that fails (2661 ticks at 48 kHz). Settling time before release is now 15 of the slowest constant (tau + tau_slug) so I0 is the settled value.

## Result

Relay timing check registered, expected to fail until item 25: coil L/R closed form, hysteresis, slug release as tau + tau_slug (perfect-coupling derivation, closed coil), tolerance 3 ticks from measured scores. Inspector PASS after two returns; one of them corrected an earlier director error.
