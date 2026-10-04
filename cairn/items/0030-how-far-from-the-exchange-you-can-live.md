---
id: 30
uid: d7a49267-2284-44e6-8c73-6ee32fb1b0d0
title: How far from the exchange you can live
type: verify
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- derivation
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
priority: p0
role: test
area: verification
effort: m
---

## The claim

The longest loop on which the line relay still operates when the handset
is lifted falls out of the battery, the relay's operate current, the
relay's windings, the set's resistance and the cable — and agrees with the
limit the Bell System engineered loops to.

## Judged against

The resistance-design loop limit of the chosen year (the ledger: commonly
quoted as 1300 Ω, later 1500 Ω, to be sourced), and the loop lengths that
implies on 26- and 24-gauge cable.

## How it is checked

Sweep cable length until the relay no longer operates; report it in metres
and in kilofeet, and as total loop resistance; compare.

## What failure looks like

Agreement that is too good. Bell's limit carried a margin for the
worst-case relay, battery and temperature; a model that lands exactly on
it without modelling any of those has probably been nudged. The check
reports the margin and the README explains what it is made of.

## Acceptance criteria

- [x] The derived limit and the published one, side by side, with the
      difference explained; expected to fail until the loop instrument lands

## 2026-10-03

apps/checks/loop_limit.hpp. The check sweeps cable length (500 ft steps, then bisection) asking whether the line relay operates, never computing the limit; it turns the length into loop ohms with its own copy of the S05/S06 copper derivation (duplicate of item 22's oracle, PR on a separate branch; dedupe after both land). loop.limit_vs_design: derived limit vs S03's 1300 ohm, tol 650 (a stated judgement, argued in the header). Era caveat is printed in the source column: S03 is BSTJ 1978, not 1965, DC only. loop.margin_is_real is the 'agreement too good' check: the derived limit must clear 1300 by more than 1 %, since Bell's limit carries a margin for the worst-case relay, battery and temperature; landing on it, or short of it, fails. loop.gauge_independent: limits on 22/24/26 AWG, in ohms, must be one number. All three are expected to fail until item 31 (the loop instrument, which depends on this item and 29) and answer NaN until then; loop.selftest (passes) shows a sound fake office, one tuned to 1300, one short, one limited by length, and never-fails/never-operates/flickering ones are told apart. Operate current is unsourced (S11), so the fake office's numbers are labelled synthetic. Hook-up for item 31's author: replace the body of line_relay_operates(int awg, Metres length) with a fresh office, handset lifted, settled, returning whether the line relay is closed. Nothing else may change.

## 2026-10-03

Returned by the inspector; revised. Band is now 1300 +/- 390 (30 %), argued in the header as a judgement the inspector may overrule; the arithmetic claim is gone. loop.margin_is_real's floor is 1300 x (1 + 0.00393 x 20) = 1402 ohm: S05's temperature coefficient over a 20 C span that is this check's own unsourced choice; the selftest now shows a model nudged to 1320 fails it and 1500 passes. The header no longer cites S03/S07 for direction: it says the ledger does not settle it and that the direction is item 30's own margin reasoning. The duplicated oracle is deleted: loop_limit.hpp includes copper.hpp (item 22, landed) and calls copper::reference_loop.

## 2026-10-03

Header now says S03's 1300 ohm being a reference-temperature figure is a premise outside the ledger, and '30 %' replaces 'a third'.

## Result

Loop-limit check registered, expected to fail until item 31: band 1300 +/- 30% against the 1978 ledger figure (S03), too-good floor 1402 from S05's temperature coefficient. Inspector PASS after one return.
