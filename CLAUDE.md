# laporte

A step-by-step telephone exchange, modelled from first principles in C++23,
for no reason.

This is not a telephone simulator that happens to be readable. It is a piece of
writing that happens to run. If a change makes the office more accurate and the
source less beautiful, it is the wrong change.

It is a sibling to `windsor` and `cornell`, and it inherits their argument: that
a model earns the right to be believed by deriving what it claims rather than
declaring it, and that the derivation is the thing worth reading.

How the work is organised — the departments, the loop each item goes through,
and the git workflow — is in `AGENTS.md`, which every agent reads:

@AGENTS.md

---

## What the thing is

A telephone exchange of about 1965 (the era spike settles the year): rotary
telephones on copper loops, a 48-volt battery, and a switch made of relays,
ratchets and shafts that climb and turn. Later, a second office and a trunk
between them; last, two real computers and a socket standing in for the trunk,
so that two people can call each other.

The physics is honest — Kirchhoff's laws over one netlist, relays that operate
because of the current in their coils, a dial whose rate comes from its
governor — but accuracy is a means, not the end. The end is that a reader who
has never seen a telephone exchange finishes the repository understanding why
dialling a number made a particular telephone ring.

The thesis lives in `selector.hpp` and everything else serves it:

> **A phone number is not an address. It is a set of directions.** A
> step-by-step office has no directory, no routing table and no lookup. When
> you dial 4, a shaft climbs to level 4 and hunts sideways for a free path;
> the next digits move the next switch. The number is the route through the
> brass. A subscriber's number is wherever their pair is soldered on the
> frame, and `./laporte spec` reports it as a result.

If an associative container from numbers to lines ever appears in `include/`,
the model has quietly stopped being true and the project is over.

The second claim, which belongs to the trunk:

> **Phreaking has no code.** Tapping the switchhook dials, because the office
> only counts breaks in the current. Whistling 2600 Hz into the mouthpiece
> hangs up the far office, because the trunk signals in the same channel it
> carries your voice in. None of this is a feature. It is what an honest model
> of in-band signalling does. There must never be a `blue_box()`.

---

## The homage

This is C++, for the reason `windsor` and `cornell` are — and one more. C++ was
written at Bell Labs in Murray Hill, the research arm of the company that ran
this network, by a researcher who wanted a better language for simulating
distributed systems (to be sourced, like everything else). The language was
invented in the building that owned the wires.

It means the same specific things it means in the siblings.

**Represent ideas directly in code.** `Volts` and `Amperes` are distinct types
and Ohm's law is the only way between them. Apparatus is constructed from the
names of the nodes it is wired to, and nothing else, so a telephone cannot hold
a pointer to the exchange. The compiler enforces house rule 1.

**Zero overhead.** No virtual dispatch in the tick. A tagged union is what you
would have written for a tagged union, so that is what we write.

**Prefer compile-time.** `consteval` literals. A `constexpr` office. The mu-law
table computed at compile time from the law it approximates.

**Don't make it look like C.** No output parameters, no raw owning pointers, no
`#define`, no arrays that decay. Values, references, `std::array`, and names.

---

## House rules

### 1. Everything is the wire

The telephone, the switch and the far office share nothing but conductors.
Off-hook, dialling, ringing, busy, answer, hang-up, and the voice itself are
states of voltage and current in one netlist, solved on one clock.

- **Apparatus never refers to apparatus.** A part is built from node names.
  It learns what the world is doing by reading the voltage across it and the
  current through it, and acts on the world by changing its own resistance,
  its own source, or its own contacts.
- **The hand touches only what a hand can touch.** The handset, the dial's
  finger-wheel, the air in front of the transmitter. Never a node.
- **Instruments are witnesses.** They read nodes and mechanical positions an
  engineer could see. None of them may reach into apparatus to make its own job
  easier, and none of them may change anything.

If any part ever learns something through a function call instead of by
measuring the line, the model has stopped being true.

### 2. Derive, never declare

The spine of this project is a list of things that must fall out:

- **How far from the exchange you can live** falls out of the battery, the
  line relay, the copper and the telephone. Nobody types 1300 ohms.
- **A copper pair's resistance** falls out of copper's resistivity and the
  definition of the wire gauge.
- **Ten pulses a second** falls out of the dial's governor, and the break
  ratio out of its cam.
- **The end of a digit** falls out of a slow-release relay letting go.
- **A busy line** is a potential on its sleeve. **All paths busy** is a
  selector riding past the tenth contact.
- **Ring trip** falls out of the battery under the ringing.
- **Your voice** is the office battery's current, varied by carbon. Sidetone
  falls out of the network's balance; a long loop's faintness out of its
  current.
- **The mu-law table** falls out of the law.
- **Your telephone number** falls out of the frame.

The moment you type a derived quantity in as a literal, the model stops being a
model and becomes a lookup table with opinions.

What is legitimately specified lives in `laporte.hpp` and in the ledger's
published figures, with their handles beside them: the battery's voltage, the
relay's operate current, the tone plan's frequencies. Those are the world's
inputs, not the model's outputs. When a derivation reproduces something real —
the loop limit landing near Bell's, the dial inside tolerance — say so in the
commit message. Those are the moments the project is for.

### 3. There is no digit, and there is no number

A digit never exists in this office. It is a train of breaks on a wire, then a
height of a shaft. A number exists in the hand (people know numbers), in the
frame as positions, and in prose — and nowhere in `include/` past `hand.hpp`.
Instruments that print a digit read it off a shaft.

The exceptions are later, and each will be argued in its own item: Touch-Tone
and multi-frequency senders both need a register that stores digits. That is the
moment memory enters the office, and it is worth a paragraph when it does.

### 4. One clock, and no wall time

The office ticks at the rate the clock spike chose. Each tick solves the netlist
with every part in its old state, then lets every part act on what it measured.
No part sees another's new state until the next tick, so the order parts were
added in cannot matter, and `./laporte verify` checks that rather than
asserting it.

Time inside `include/` is a tick count. There is no `<chrono>` there. Real time
enters the program in exactly one place, v0.6's platform edge, which drives the
tick from outside.

### 5. No dependencies

The standard library and nothing else. `make && ./laporte` on a clean machine
with a C++23 compiler. A WAV is a forty-four-byte header and some numbers;
write it yourself. The repository should still build in fifteen years.

The one sanctioned exception is v0.6's platform edge — a microphone, a speaker
and a socket — which lives in `apps/platform/`, converts at its own boundary,
and says so loudly in a comment. If you find yourself wanting a second, you are
about to make the project unbuildable on the next machine.

### 6. Verify every claim, against somebody else's figure

Every figure a check is judged against is an entry in `docs/sources.md`, with
the source it came from and the sentence quoted. Nothing enters the ledger
unless somebody fetched the source. A figure that is commonly quoted and cannot
be traced is entered as **unsourced**, and a result judged against it says so
next to the result.

This rule is stricter here than in the siblings because the team writing this
is a set of language models, which will produce a plausible figure on request
with total confidence. The ledger is the defence.

Checks are written **before** the apparatus they judge, and land marked as
expected to fail. A check written after the thing it checks is a check written
to pass. Never weaken a check to make it pass: file a bug, or argue in the
item's notes that the check was wrong, and let the inspector decide.

### 7. Every figure quoted outside the code is a copy, and copies rot

The README will quote loop limits, pulse rates, release times and band edges.
Every one is a copy of something the program prints, and the program changes.
Show the command above every figure. After any change to the model, re-run the
instruments and reconcile every number in the README against what they printed.
When editing prose programmatically, assert the anchor exists.

### 8. Say what is not modelled

Every element is lumped: a copper pair several kilometres long is a resistor,
not a transmission line, and the cost — no line capacitance, no loading coils,
no rounded dial pulses on long loops — is listed by name at the top of
`kirchhoff.hpp`. Every header opens with prose explaining *why the part exists
and what it is arguing with*. Not what the code does — the code does that.
Never write a comment that restates the line beneath it.

### 9. One idea per file, and people get named

A file is named for a part or for a person: `relay.hpp`, `dial.hpp`,
`selector.hpp`, `carbon.hpp`, `kirchhoff.hpp`, `goertzel.hpp`. The naming
carries information. A law and a fit must not be spelled the same way:
`kirchhoff.hpp` is exact; a transmitter's diaphragm resonance fitted to a
published curve says it is a fit, in its name or its first line.

---

## Commits

Atomic, one subsystem each, written in the same voice as the code. Present
tense, no ceremony, and say what the part *does* rather than what you did to it.

    The line relay, and how far from the exchange you can live
    A digit is a height
    Admit that the pair is not a transmission line

Not `feat: add relay module` and not `fix stuff`. The commit message is the pull
request description; there is one thing to write, not two. No attribution
trailers of any kind.

Never commit a state that does not build. The gate in `.claude/hooks/` runs
`make` and `cairn check` before every commit, and CI builds every pull request
from clean under gcc and clang.

---

## Layout

    include/laporte/    the apparatus. header-only, one idea per file.
    apps/               the instruments you point at it, and main.cpp.
    apps/checks/        one check per file; `./laporte verify` runs them.
    apps/platform/      v0.6 only: the one place the outside world gets in.
    docs/sources.md     the ledger: every published figure, with its source.
    cairn/              the roadmap, as items. `cairn next --view next`.
    Makefile            `make`. that's it.

There is no `src/`. The office knows nothing about output, sound files or
terminals. It is a sealed mechanism that answers one question — what is every
wire doing on this tick — and the apps are witnesses to it.

---

## The roadmap

It is in the repository, as Markdown, under `cairn/`.

    cairn list --view next      ready work
    cairn board                 where everything stands
    cairn roadmap               the milestones, in order
    cairn list --view thesis    the items the project exists for

Do not invent a parallel list of things to do; this is the list.
