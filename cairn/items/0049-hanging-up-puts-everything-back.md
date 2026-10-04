---
id: 49
uid: ce78a6c1-ad21-4298-bffa-f7f35d7aa1d2
title: Hanging up puts everything back
type: verify
status: backlog
milestone: v0.3
depends_on:
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p1
role: test
area: verification
effort: s
---

## The claim

When the caller hangs up, every switch in the train returns to normal and
every sleeve it marked returns to idle, and how long that takes falls out
of the slow-release relays.

## How it is checked

After a completed call, the caller replaces the handset; measure until
every shaft is at normal; then a new call through the same path succeeds.

## Acceptance criteria

- [ ] Release time printed; a second call succeeds; expected to fail until the connector lands
