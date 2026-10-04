---
id: 16
uid: 35a6a5ad-bc43-4665-a090-bed7e1ad8e2f
title: './laporte verify: the harness, and checks that are allowed to fail'
type: instrument
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
labels:
- foundation
depends_on:
- 11
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
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

- [x] `./laporte verify` runs, and `./laporte verify <name>` runs one check
- [x] Expected-failure and unexpected-pass both behave as described
- [x] Adding a check touches one new file and one line

## 2026-10-03

Claimed with --force (0011 scaffolding already present). Harness in apps/verify.hpp; checks self-register by defining an inline bool at namespace scope, so a new check is one file plus one #include line in apps/checks/registry.hpp. judge() is pure and its four verdicts (pass, fail, expected-fail, unexpected-pass) are static_assert'd, since a permanently-failing check cannot be registered to exercise the last path at run time. Exit: failures clamped to 255; 127 for an unknown check name. selftest.pass and selftest.xfail (until item 9999, which does not exist) exercise the run-time paths. main.cpp is edited at the top (includes) and dispatch; PR for 0015 also adds one include line near the std includes, kept on different lines so the rebase is clean.

## 2026-10-03

Returned: run_one now has one return shared by the loud and quiet paths, and the full run calls it on a stale marker, an honest xfail, a pass and a fail built outside the registry (harness.counts). Mutating run_one to ignore stale markers now turns the run red; confirmed. Selftests cite 'selftest', not S00. Header notes the 127 ambiguity, unvalidated tolerance, and registry append conflicts.

## Result

./laporte verify: one check per file, one registry line, expected-failure and stale-marker paths exercised at run time. Inspector PASS after one return.
