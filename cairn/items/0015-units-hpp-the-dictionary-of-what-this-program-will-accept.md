---
id: 15
uid: cf896c51-4e8a-45f0-9171-5b59d36b9c00
title: 'units.hpp: the dictionary of what this program will accept'
type: apparatus
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
area: units
effort: m
---

## What it is

The unit convention, and the first file written. Volts, amperes, ohms,
siemens, farads, henries, seconds, hertz, metres, pascals. `consteval`
literal operators in (`48.0_V`, `20.0_Hz`, `0.47_uF`), named conversions
out (`as::kilofeet(length)` exists for quoting Bell, who measured cable in
thousands of feet), and nothing in between.

`Volts` and `Amperes` are distinct types. Ohm's law is the operator that
turns one into the other, and it is the only one.

## What it must derive

Nothing. This is the one file allowed to be a list.

## What it may touch

Nothing. Everything touches it.

## What is not modelled

Decibels are not a unit here; they are a way of printing a ratio, and live
with the instruments.

## Acceptance criteria

- [x] Every literal has an inverse in `namespace as`
- [x] `Volts{1} + Amperes{1}` does not compile, and a check asserts that
- [x] No file outside `units.hpp` and `apps/` contains a conversion factor

## 2026-10-03

Claimed with --force: dependency 0011's scaffolding (Makefile, .claude/, CI) is already in the repo; only its cairn status was stale, per the director. Types: one Quantity<Tag> template, ten tags; Ohm's law (and its siemens form) are the only cross-type operators. No macros (house rule), so literals are spelled out, floating and integer. Negative evidence is pure C++: requires-expression static_asserts in apps/checks/units_compile.hpp, included by main.cpp (one line), so a regression breaks the build. Confirmed by hand that Volts{1}+Amperes{1} is rejected.

## 2026-10-03

Returned: header was ignored by .gitignore's bare 'laporte' (fixed on main as /laporte) and is now tracked. The foot is defined once (metres_per_foot, defined not measured, attribution unsourced) and kft/kilofeet derive from it. Header claim now matches the check: round trip to relative 1e-12.

## Result

units.hpp: ten distinct quantity types, consteval literals with inverses in as::, Ohm's law the only way between V and A; negative-compile checks are static_asserts. Inspector PASS after one return (gitignore, foot defined once).
