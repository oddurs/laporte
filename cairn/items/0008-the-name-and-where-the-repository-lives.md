---
id: 8
uid: e704a1d5-ef21-4b3d-b69c-a5b5bef666fc
title: The name, and where the repository lives
type: spike
status: done
milestone: v0.1
assignee: Oddur Sigurdsson
owner: oddurs
labels:
- decision
- foundation
created: 2026-10-03
updated: 2026-10-03
closed_at: 2026-10-03
priority: p0
role: systems
area: build
effort: s
---

## Question

What is the project called, and where is it published?

## Why it has to be answered first

The binary is `./laporte` in every item on this roadmap, every instrument's
usage line, every check and every commit message. Renaming after v0.1 is a
search-and-replace across prose that will be quoted elsewhere.

## Options

**`laporte`** (recommended). La Porte, Indiana, where the first commercial
automatic exchange opened in 1892 (to be sourced). Matches the place-name
convention of `cornell` and `windsor`, and names the beginning of the
technology rather than the technology.

**`strowger`**. Names the inventor. Breaks the convention, and rule 8
reserves people's names for the files whose ideas they are.

**`telephone`**. The directory's current name. Honest and generic.

Where: `github.com/oddurs/<name>`, public, like the siblings. Creating the
repository is outward-facing and is the owner's call.

## What would settle it

The owner says so.

## Answer

## 2026-10-03

Answer: laporte. Repository: github.com/oddurs/laporte, public, created 2026-10-04 on the owner's instruction ("bootstrap public repo on github"). Rebase-merge only, branches deleted on merge.
