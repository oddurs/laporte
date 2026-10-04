# How the work is done

`CLAUDE.md` says what this project is and the rules it keeps. This file says how
the work is organised: who does what, the path every item takes, and where the
work happens. It is written for the agents that do the work and for the person
who owns it.

---

## The laboratory

The project is built by a small organisation of agents, modelled loosely on the
departments that built the real thing. Every department runs on Sonnet. Each has
a definition in `.claude/agents/<role>.md`; the `role` field on every item says
which department picks it up by default.

### The owner

A person. Owns every item, decides every item labelled `decision`, approves
anything outward-facing (creating the GitHub repository, publishing), and tags
each release after reading the milestone review. Nothing in this file overrides
the owner.

### The director

The session that runs the team. It does not write apparatus. It:

- reads `cairn list --view next` and the role views, and hands items to
  departments, honouring dependencies and the limits below;
- launches each agent in its own worktree, with the item's `cairn prompt` and
  the department's brief;
- stops and asks the owner when it reaches a `decision` item, and records the
  answer in the spike;
- sends items in `review` to the inspector, and lands what the inspector
  passes;
- promotes the next milestone's items from `backlog` to `planned` once the
  current milestone's review is done and tagged;
- keeps `CLAUDE.md`, `AGENTS.md`, `cairn.toml` and `.claude/` — no department
  edits those. A department that thinks one of them is wrong says so in a note.

### Systems engineering — `systems`

Owns the decisions everything else is built on, and the parts nobody else can
own: `units.hpp`, `kirchhoff.hpp`, `clock.hpp`, `hand.hpp`, `laporte.hpp` (the
office as built), `wav.hpp`, the verify harness, `trace`, `spec`, the build and
CI, and v0.6's network and platform edge. Writes the spikes and proposes their
answers; the owner decides the ones labelled `decision`.

Systems is the only department that may change an interface other departments
depend on, and when it does, it says so in the item and in the commit.

### Switching — `switching`

The machinery that moves: `relay.hpp`, the switchhook and the dial,
`selector.hpp`, `connector.hpp`, the ringing machine, ring trip, trunk circuits.
Switching is where the thesis is most easily broken, because it is where it is
most tempting to store a digit or look up a number. Do neither.

### Transmission — `transmission`

Everything that carries a signal rather than moves a shaft: `cable.hpp`, the
transmission bridge, the tones, the ringer, the transmitter, receiver and the
set's network, the carrier channel and term set, `goertzel.hpp`, `sf.hpp`,
`mu_law.hpp`, and the `call`, `listen`, `sweep` and `whistle` instruments.
Transmission is where it is most tempting to apply a filter that sounds right.
Don't; build the part whose response it is.

### Test — `test`

Owns `docs/sources.md` and every `verify` item.

- The **ledger** first: every figure a check uses, with its source fetched and
  quoted. Test is the department most at risk of inventing a plausible number,
  so it is held to the strictest rule: a figure with no fetched source is
  entered as **unsourced**, never as fact.
- **Checks before apparatus.** A verify item lands before the apparatus it
  judges (the dependencies on the roadmap enforce this), marked as expected to
  fail until that item. The apparatus item's author may not edit the check.
  If they believe it is wrong, they note it and the inspector decides.
- Checks are written to find the failure modes their items describe, not to
  confirm the happy path.

### The writer — `writer`

Owns the README and the opening argument of every header, in the voice of
`windsor` and `cornell`. Implementers write the first draft of their own
header's argument; the writer edits it for voice and checks it against rule 7.
The writer reconciles every figure in the README with what the program prints,
every milestone, and files a bug for any that no longer match.

### The inspector — `inspector`

Reads what somebody else wrote. Has no `Write` or `Edit`: it builds, runs,
reads, and writes notes into cairn, and nothing else. It never reviews its own
department's work, because it has none.

For every item in `review` it checks, in order:

1. `make strict` builds and `./laporte verify` has no unexpected results.
2. The item's acceptance criteria are actually true, not merely ticked.
3. House rule 1: no apparatus refers to other apparatus; no instrument changes
   anything; the hand touches no node.
4. House rule 2: no derived quantity typed as a literal. Every literal in
   `include/` is either derived on the spot or carries a ledger handle.
5. House rule 3: no digit stored and no number in `include/` past `hand.hpp`.
6. House rule 6: no check was weakened; no expected-failure marker was moved
   by the item that was supposed to make it pass.
7. The header opens with an argument and says what is not modelled.
8. The commit message is in the house voice.

Then either **passes** it (a note saying so) or **returns** it (`status=doing`,
and a note listing what must change, each point specific enough to act on). It
does not fix things itself. Problems outside the item's scope become new items.

At the end of each milestone the inspector also runs the milestone's "read as a
stranger" item: a clean clone, the README followed literally, every quoted
figure reproduced.

---

## The path every item takes

    backlog ──► planned ──► doing ──► review ──► done
                              ▲          │
                              └──────────┘  returned, with notes

1. **planned.** The director promotes items when their milestone opens. Only
   `planned` items whose dependencies are done appear in `--view next`.
2. **doing.** The director launches the department's agent in a worktree named
   for the item. The agent runs `cairn claim <ID>`, then reads
   `cairn prompt <ID>`, which includes what every dependency concluded.
3. The agent does the work, notes what it decided and why
   (`cairn note <ID> "..."`), ticks only the criteria that are true, runs
   `make && ./laporte verify`, and commits — one item, one commit, in the house
   voice.
4. **review.** The agent sets `status=review` in the same commit, runs
   `.claude/propose` to open the pull request, and stops. It does **not** close
   its own item.
5. The inspector reviews the branch. If it returns the item, the director
   resumes the same agent with the inspector's notes, and the item goes round
   again on the same branch.
6. **done.** When the inspector passes it, the director runs `cairn close <ID>`
   on the branch, amends it into the commit so that the code and the roadmap
   agree about what is finished, re-runs `.claude/propose`, and then
   `.claude/land`.

**blocked.** An agent that cannot proceed — a dependency's interface is wrong, a
figure cannot be sourced, the item contradicts a house rule — sets
`status=blocked`, writes a note saying exactly what it needs, commits nothing
half-done, and stops. It does not work around the block.

**Spikes** follow the same path, except that their deliverable is the `Answer`
section, and a spike labelled `decision` passes through the owner between
review and done.

**Bugs** found in review or in passing are new items, filed with
`cairn new ... --type bug`, never fixed silently inside another item.

---

## Limits

These exist because several agents writing one codebase at once will otherwise
overwrite each other, and because Sonnet works best on a well-bounded task.

- **At most three implementing agents at once**, plus the inspector.
- **One agent per `area` at a time.** Two agents in the same subsystem means
  two agents in the same header.
- **An item is one header, or one instrument, or one check.** If it turns out
  to need two headers, it is two items: the agent says so in a note and the
  director splits it (`cairn split`).
- **An agent touches only its item's files**, plus at most one line in each
  shared registry (`apps/main.cpp`'s command list, `apps/checks/` registry).
  Anything else it needs changed is a note, and probably an item.
- **`effort=l` items get a plan first.** Before writing code, the agent notes
  its plan in the item (the types it will add, the nodes it touches, what it
  will not model), and the director reads it before the agent continues.
- **Returned twice is a signal.** An item the inspector returns twice goes to
  the director, who decides whether the item, the agent, or the check is wrong.

---

## The git loop

**One item, one worktree, one branch, one commit, one pull request.** Identical
to `cornell`'s, and for the same reasons.

```sh
git worktree add .claude/worktrees/0026-relay -b 0026-relay origin/main
cd .claude/worktrees/0026-relay
cairn claim 26
# ... the work ...
make && ./laporte verify
git add -A && git commit
.claude/propose        # rebase onto origin/main, build, push, open the PR
# ... inspector reviews; director closes the item and amends ...
.claude/land           # wait for CI, rebase-merge, remove branch and worktree
```

Agents launched with worktree isolation get the same layout. The branch is named
for the item, id first, so `git branch` sorts into roadmap order.

There are no merge commits. `main` is a single line of argument.

`cairn/items/*.md` and `ROADMAP.md` carry `merge=cairn` in `.gitattributes`, so
two branches that each touched the backlog reconcile without a person. That does
not extend to the apparatus: two branches editing the same header is a
conflict, and the limits above exist so that it does not happen.

### The two gates

`.claude/hooks/build-gate.sh` runs `make` and `cairn check` before every
`git commit` an agent makes. CI builds every pull request from a clean checkout
under gcc and clang, runs `./laporte verify`, and builds again with every warning
fatal. The local gate makes the cycle seconds instead of minutes; CI is what the
claim rests on.

---

## Starting a department's agent

What the director passes, every time:

1. The department's definition (`subagent_type: <role>`, model Sonnet,
   worktree isolation).
2. The output of `cairn prompt <ID>`.
3. One sentence of context the item cannot know: what landed since its
   dependencies closed, or what the inspector said last time.

Nothing else. If the item needs more context than that to be done well, the
item is under-written, and the fix is to edit the item, not the prompt — so the
next agent gets it too.

---

## The cairn reference

The section below is generated by `cairn agent` and describes cairn's generic
loop. Where it differs from *The path every item takes* above, this file wins:
in particular, an implementing agent sets `status=review` and stops; it never
runs `cairn close` on its own item.

<!-- cairn:begin -->
## Roadmap and issues

This project tracks its roadmap and issues with `cairn`. Every item is a Markdown file under `cairn/items`, described by the schema in `cairn.toml`.

**Do not create ad-hoc TODO, PLAN or NOTES files.** Create a cairn item instead, so the work appears on the board and in the generated roadmap.

### The loop

1. `cairn next --view next` — what is ready to start. It excludes anything blocked by unfinished dependencies and puts work already in progress first.
2. `cairn claim <ID>` — take it before you start, so no one duplicates the work. `cairn claim --next --view next` picks and claims the top-ranked unclaimed item in one step. Then read `cairn prompt <ID>`: the item with everything it rests on — the outcome it serves, what its dependencies concluded, what done means and what earlier runs learned.
3. Do the work. Record what you learn: `cairn set <ID> <field>=<value>` for fields, `cairn note <ID> "<TEXT>"` for anything that needs a sentence — why you chose something, what you tried, what to watch for.
4. `cairn tick <ID> <N>` as each acceptance criterion becomes true — `cairn show <ID> --criteria` lists them numbered. Tick what is true, not what would let you close.
5. `cairn close <ID>` when it is done, or `cairn release <ID>` to hand it back.
6. `cairn check` before you report finished. It must pass.

### Commands

```sh
cairn next --view next --json                 # ready work, ranked
cairn claim --next --view next                # take the next ready item
cairn search <TEXT> --json        # titles, bodies and labels
cairn list --json                 # all open items
cairn list --filter 'blocked=false,priority=p0'
cairn prompt <ID>                 # the item as a prompt, with what it rests on
cairn show <ID> --json            # one item, including its body
cairn new "<TITLE>" --type <TYPE> --milestone <MILESTONE>
cairn set <ID> status=<STATUS>    # also labels+=x, or any field below
cairn note <ID> "<TEXT>"          # append reasoning; never replaces
cairn show <ID> --criteria        # acceptance criteria, numbered
cairn tick <ID> <N>               # tick one; --all for every one
cairn close <ID>
cairn check                       # validate; run before finishing
cairn render                      # regenerate ROADMAP.md
```

Selection uses saved view `next`. Additional filters only narrow it; the view's sort and columns do not change `next` ranking. Over MCP, pass `{"view":"next"}` to `next_items` and to `claim_item` without an id. A direct claim is an explicit assignment outside this selection policy. Regenerate these instructions with `cairn agent --view next --write AGENTS.md`.

Claims coordinate writers in the same item directory and are seen across the worktrees of this repository: `next` leaves out what another worktree has claimed, `claim` refuses it, and `cairn worktrees` shows what each is doing. Separate clones are not read. Agree on assignments before splitting work across clones.

Items are numbered: `0012`, and commands accept the bare number too. Write the number in `depends_on` and other id references. Each item also carries a `uid` tag; leave it alone. If a merge gives two items one number, `cairn renumber` moves the one that arrived and retargets the references that came with it.

### Schema

- **Types**: `apparatus`, `instrument`, `verify`, `prose`, `spike`, `bug`, `chore`, `milestone`
- **Statuses**: `backlog` (open), `planned` (open), `doing` (active), `review` (active), `blocked` (active), `done` (done), `dropped` (dropped)
- **`due`**: date, YYYY-MM-DD — when a milestone is meant to land
- **`part_of`**: names any items, by id, several allowed — a larger piece of work this belongs to
- **`priority`**: one of p0, p1, p2, p3 — p0 blocks the milestone
- **`effort`**: one of s, m, l, xl — Rough size, not an estimate
- **`role`**: one of systems, switching, transmission, test, writer, inspector — Which department picks this up
- **`area`**: one of units, circuit, cable, station, relay, switching, signalling, transmission, frame, audio, instrument, verification, prose, build — Subsystem this touches
- **Milestones**: `v0.1`, `v0.2`, `v0.3`, `v0.4`, `v0.5`, `v0.6`, `later`
- **Saved views** (`cairn list --view NAME`): `now`, `next`, `review`, `decisions`, `systems`, `switching`, `transmission`, `test`, `writer`, `thesis`, `derivations`, `honesty`, `triage`

### Rules

1. Before starting work, find or create the item and set it to an active status.
2. Use the fields above rather than inventing new ones; add new fields to `cairn.toml` first.
3. Never hand-edit the generated roadmap file — change items and run `cairn render`.
4. `cairn check` must pass before the work is considered done.

<!-- cairn:end -->
