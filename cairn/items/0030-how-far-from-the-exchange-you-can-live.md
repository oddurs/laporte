---
id: 30
uid: d7a49267-2284-44e6-8c73-6ee32fb1b0d0
title: How far from the exchange you can live
type: verify
status: planned
milestone: v0.1
labels:
- derivation
- thesis
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
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

- [ ] The derived limit and the published one, side by side, with the
      difference explained; expected to fail until the loop instrument lands
