---
id: 25
uid: b38e9a85-621c-44d4-983b-d0c3e5769160
title: 'relay.hpp: a coil, an armature, a slug, and contacts'
type: apparatus
status: planned
milestone: v0.1
labels:
- foundation
depends_on:
- 19
- 20
- 24
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: switching
area: relay
effort: l
---

## What it is

The relay, which is the entire computer of a step-by-step office. A coil
is a resistor and an inductor in the netlist. The armature is a state with
hysteresis: it pulls in at one current and drops out at a lower one. Its
contacts are contact elements on other nodes, which open and close at the
end of a tick.

A copper slug on the core makes a slow-release relay — the eddy current in
the slug holds the flux up after the coil current stops — and that delay
is the office's only memory. Step-by-step offices remember that you are
still dialling because a relay has not yet let go.

## What it must derive

Operate and release times, from the coil's inductance and resistance and
the armature's thresholds. A slow-release time from the slug, modelled as
a second, slower decay on release.

## What it may touch

The coil's two nodes; each contact's two nodes.

## What is not modelled

Contact bounce; armature travel time beyond one tick; magnetic saturation;
residual magnetism.

## Acceptance criteria

- [ ] The relay check passes
- [ ] The header explains why timing is memory in this office
