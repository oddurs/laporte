---
id: 29
uid: f45d5116-f296-4393-b7cb-f5958f29c6eb
title: 'laporte.hpp, first draft: a battery, a relay, a pair, a telephone'
type: apparatus
status: planned
milestone: v0.1
depends_on:
- 23
- 26
- 27
- 28
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: systems
area: frame
effort: s
---

## What it is

The office as built, like `windsor.hpp` and `cornell.hpp`: the one file
where things are specified rather than derived. In v0.1 it is one line
circuit, one length of cable, one telephone.

## What it must derive

Nothing. Everything else derives from it.

## What it may touch

It is where nodes are named and parts are wired between them.

## What is not modelled

Everything v0.2 onwards adds.

## Acceptance criteria

- [ ] Reading it top to bottom describes the circuit in the order a
      lineman would trace it
