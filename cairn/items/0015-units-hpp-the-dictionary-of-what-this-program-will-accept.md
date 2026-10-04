---
id: 15
uid: cf896c51-4e8a-45f0-9171-5b59d36b9c00
title: 'units.hpp: the dictionary of what this program will accept'
type: apparatus
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

- [ ] Every literal has an inverse in `namespace as`
- [ ] `Volts{1} + Amperes{1}` does not compile, and a check asserts that
- [ ] No file outside `units.hpp` and `apps/` contains a conversion factor
