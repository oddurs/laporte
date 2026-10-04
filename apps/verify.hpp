// verify.hpp — the harness that judges every claim, and lets some of them fail.
//
// A model that is only ever shown to agree with itself has not been checked.
// So `./laporte verify` does one thing: it takes a measured figure, takes the
// figure somebody else published, takes the source handle from the ledger
// (`docs/sources.md`) the published one came from, and says whether they agree
// to within a stated tolerance. It prints that as one line, because a failure
// is something to be read, and a table that scrolls is not.
//
// It is arguing with the green tick. A suite that is red until the day
// everything works is a suite everyone learns to ignore, and a suite that is
// green because the checks were written after the apparatus, to pass, proves
// the apparatus agrees with its author. House rule 6 answers both: checks are
// written first, and land marked as *expected to fail until item N*. This
// harness is the mechanism for that sentence.
//
//   * An expected failure prints, and does not count.
//   * An expected failure that *passes* counts as a failure. Either the marker
//     is stale — the item landed and nobody removed it — or the check measures
//     nothing, and both are worth stopping for.
//   * The exit status is the number of failures, so CI needs no parser.
//
// The apparatus item's author may not edit a check, so the marker is removed by
// the inspector or the test department, and the unexpected pass is what tells
// them to.
//
// Layout is arranged for many departments writing checks at once. A check is
// one file under `apps/checks/`, and it announces itself by what it defines;
// the registry (`apps/checks/registry.hpp`) is one `#include` line per file.
// Two branches adding a check therefore conflict on a single line, or, if
// they add different lines in different places, not at all.
//
// Checks run in order of inclusion, which is the registry's order and visible
// there. Branches that insert their line by sorted file name do not collide;
// two that both append at the end do, on adjacent lines, and one line is the
// whole conflict.
//
// What is not modelled:
//
//   * Statistics. A tolerance is a single absolute number a person chose and
//     defended in the check, not a confidence interval. A check on a noisy
//     quantity states its own repetitions.
//   * Isolation. Checks run in one process and in sequence; one that crashes
//     takes the run with it, which is loud, and loud is what is wanted.
//   * More than 255 failures: the exit status of a process is a byte, so it is
//     clamped there. Exit 127 means the name asked for is not a check, and is
//     therefore also what exactly 127 failures looks like; the ambiguity is
//     accepted rather than hidden. Zero failures is the only success.
//   * A tolerance that is negative, NaN or infinite is not rejected, and an
//     `until_item` is never compared with cairn: a marker for an item that has
//     closed is found only because its check starts to pass.
//
// A check may not reach into apparatus (house rule 1): it builds an office from
// parts, drives it with a hand, and reads what an instrument could. That rule
// is kept by what checks include, not by anything here.

#pragma once

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

namespace laporte::verify {

// What a check computes. A check that judges a yes-or-no claim measures 1 or 0
// and publishes 1; the harness has one kind of figure and wants to keep it so.
using Measure = double (*)();

struct Check {
    std::string_view name;        // what `./laporte verify <name>` takes
    std::string_view source;      // a ledger handle (S01), or "unsourced"
    std::string_view unit;        // printed after the figures, never converted
    double           published;   // somebody else's number
    double           tolerance;   // absolute, in the same unit
    int              until_item;  // 0, or the item that makes it pass
    Measure          measure;
};

enum class Verdict { pass, fail, expected_fail, unexpected_pass };

// The whole judgement, kept pure so that its four outcomes can be asserted
// without running a check, and are (below).
constexpr Verdict judge(const Check& c, double measured) {
    const double d = measured > c.published ? measured - c.published : c.published - measured;
    const bool agrees = d <= c.tolerance;
    if (c.until_item == 0) return agrees ? Verdict::pass : Verdict::fail;
    return agrees ? Verdict::unexpected_pass : Verdict::expected_fail;
}

constexpr bool counts_as_failure(Verdict v) {
    return v == Verdict::fail || v == Verdict::unexpected_pass;
}

namespace {
constexpr Check probe{"probe", "selftest", "", 10.0, 0.5, 0, nullptr};
constexpr Check probe_xf{"probe", "selftest", "", 10.0, 0.5, 7, nullptr};
}
static_assert(judge(probe, 10.4) == Verdict::pass);
static_assert(judge(probe, 11.0) == Verdict::fail);
static_assert(judge(probe_xf, 11.0) == Verdict::expected_fail);
static_assert(judge(probe_xf, 10.4) == Verdict::unexpected_pass);
static_assert(!counts_as_failure(Verdict::expected_fail));
static_assert(counts_as_failure(Verdict::unexpected_pass));

inline std::vector<Check>& registry() {
    static std::vector<Check> checks;
    return checks;
}

// Called by the check files, at namespace scope, once each. The return value
// exists so that a file can say `inline const bool registered = add(...)` and
// have nowhere to put a call that is not a declaration.
inline bool add(const Check& c) {
    registry().push_back(c);
    return true;
}

inline void print_line(const Check& c, double m, Verdict v) {
    const char* word = "PASS ";
    switch (v) {
        case Verdict::pass:            word = "PASS "; break;
        case Verdict::fail:            word = "FAIL "; break;
        case Verdict::expected_fail:   word = "XFAIL"; break;
        case Verdict::unexpected_pass: word = "FAIL "; break;
    }
    std::printf("%s  %-28s measured %.6g %.*s  published %.6g %.*s [%.*s]  tol +/-%.3g",
                word, std::string{c.name}.c_str(), m, int(c.unit.size()), c.unit.data(),
                c.published, int(c.unit.size()), c.unit.data(),
                int(c.source.size()), c.source.data(), c.tolerance);
    if (v == Verdict::expected_fail)
        std::printf("  (expected to fail until item %d)", c.until_item);
    if (v == Verdict::unexpected_pass)
        std::printf("  (expected to fail until item %d, and passed: stale marker, or it checks nothing)",
                    c.until_item);
    std::printf("\n");
}

// `quiet` is for the harness's own self-test, which must call this very
// function, with its one return, and must not print the FAIL line it provokes.
inline bool run_one(const Check& c, bool quiet = false) {
    const double m = c.measure();
    const Verdict v = judge(c, m);
    if (!quiet) print_line(c, m, v);
    return counts_as_failure(v);
}

// The harness is judged by the rule it enforces. `judge()` is asserted at
// compile time, but what makes a verdict *count* is `run_one`'s return value,
// and a mutation there (stop counting a stale marker) builds clean and leaves
// every static_assert green. So the full run calls the real `run_one` on four
// checks built here, outside the registry, and reports one line for the lot.
inline bool harness_is_honest() {
    constexpr Measure ten = +[] { return 10.0; };
    const Check ok    {"h.ok",    "selftest", "", 10.0, 0.5, 0, ten};
    const Check bad   {"h.bad",   "selftest", "", 20.0, 0.5, 0, ten};
    const Check xfail {"h.xfail", "selftest", "", 20.0, 0.5, 7, ten};
    const Check stale {"h.stale", "selftest", "", 10.0, 0.5, 7, ten};
    return !run_one(ok, true) && run_one(bad, true) &&
           !run_one(xfail, true) && run_one(stale, true);
}

// `./laporte verify` runs everything; `./laporte verify <name>` runs one.
inline int main(int argc, char** argv) {
    const auto& all = registry();
    int failures = 0, ran = 0, expected = 0;

    if (argc >= 1) {
        const std::string_view want{argv[0]};
        const auto it = std::find_if(all.begin(), all.end(),
                                     [&](const Check& c) { return c.name == want; });
        if (it == all.end()) {
            std::fprintf(stderr, "laporte verify: no check named '%s'. They are:\n", argv[0]);
            for (const Check& c : all)
                std::fprintf(stderr, "  %.*s\n", int(c.name.size()), c.name.data());
            return 127;
        }
        failures = run_one(*it) ? 1 : 0;
        return failures;
    }

    const bool honest = harness_is_honest();
    std::printf("%s  %-28s a stale expected-failure counts, an honest one does not\n",
                honest ? "PASS " : "FAIL ", "harness.counts");
    if (!honest) ++failures;

    for (const Check& c : all) {
        ++ran;
        if (c.until_item != 0) ++expected;
        if (run_one(c)) ++failures;
    }
    std::printf("%d checks, %d failures, %d marked as expected to fail\n", ran, failures, expected);
    return std::min(failures, 255);
}

}  // namespace laporte::verify
