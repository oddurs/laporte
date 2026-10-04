---
id: 111
uid: ff43f91a-b4c8-400b-b0f1-dcd2038fde42
title: 'wav.hpp: mark refusals [[nodiscard]], don''t leave a truncated file on a failed write, and use a unique temp name in its check'
type: bug
status: backlog
milestone: v0.2
created: 2026-10-03
updated: 2026-10-03
priority: p3
role: systems
area: audio
effort: s
---

## What the inspector found (PR #17, non-blocking)

Refusals return nothing and nothing is `[[nodiscard]]`, so a caller can ignore one silently. `write_bytes` can leave a truncated file if the stream fails mid-write. The round-trip check uses a fixed temp filename. gcc is untested. Item 0041's note still carries text from before the amendment.
