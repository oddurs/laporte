---
id: 16
uid: 35a6a5ad-bc43-4665-a090-bed7e1ad8e2f
title: './laporte verify: the harness, and checks that are allowed to fail'
type: instrument
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 11
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: systems
area: verification
effort: m
---

## What it witnesses

Every claim the project makes, each against the `docs/sources.md` entry it
is judged by.

## What it prints

One line per check: pass or fail, the measured figure, the published
figure and its source handle, the tolerance. Exit status is the number of
failures.

Checks are written before the apparatus they judge (see `AGENTS.md`), so a
check may be marked **expected to fail until item N**. An expected failure
prints, and does not count. An expected failure that *passes* is a
failure: it means the marker is stale or the check is not checking
anything, and either is worth stopping for.

## What it may not do

Reach into apparatus. A check builds an office from `laporte.hpp` parts,
drives it with a hand, and reads what an instrument could read.

Layout matters because several departments add checks at once: one check
per file under `apps/checks/`, and a one-line registry, so that two
branches adding checks conflict on one line or not at all.

## Acceptance criteria

- [ ] `./laporte verify` runs, and `./laporte verify <name>` runs one check
- [ ] Expected-failure and unexpected-pass both behave as described
- [ ] Adding a check touches one new file and one line
