---
id: 91
uid: ecb48b62-c030-4faa-99b7-afa238129afd
title: mu-law, bit for bit against G.711
type: verify
status: backlog
milestone: v0.6
depends_on:
- 10
- 16
created: 2026-10-03
updated: 2026-10-03
priority: p0
role: test
area: verification
effort: s
---

## The claim

The encoder and decoder agree with ITU-T G.711's mu-law tables for every
one of the 256 codes and every 14-bit input, and the quantisation SNR for a
full-scale sine is the figure the theory gives.

## Judged against

G.711, from the ledger. Bit-exact: no tolerance.

## Acceptance criteria

- [ ] All 256 codes; expected to fail until mu-law lands
