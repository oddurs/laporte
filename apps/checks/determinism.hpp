// determinism.hpp — the same office twice, in any order.
//
// The two-phase tick is a promise: every part is solved against every other
// part's *old* state, and only then does any part act. If the promise holds,
// the order parts were added to the netlist cannot matter, and the office is a
// function of its description. If one element advances its state while the
// solve is still going, what it does depends on who was solved before it, and
// the same office built in a different order walks a different road. That bug
// is silent in a one-order test, which is why this check builds the office
// more than once.
//
// It is arguing with "it looked the same when I ran it". A scenario that
// passes in one order proves nothing about the other, and a hash of the final
// state proves nothing about the road there. So every node voltage on every
// tick goes into the hash, as the exact bit pattern of the double: not a
// tolerance, not a rounding, because a deterministic office has no excuse for
// a last-bit difference.
//
// The check is written before the office exists, so it asks for an office
// through a concept (`Office`, below) and not through any header. Nothing here
// includes the netlist or the clock. Until item 29 lands, the registered check
// measures 0 and says why.
//
// HOOK-UP. The live line is not the apparatus author's to touch: items 19, 20
// and 29 build the office and may not edit this file (house rule 6). The test
// department, this check's author, provides the `Office` class, `v01_elements()`
// and the hand scenario, written when item 29's office exists; never the author
// of 19, 20 or 29. It then replaces the body of `determinism_measure` with
//
//     return identical_in_either_order<laporte::Office>(
//         v01_elements(), lift_wait_replace, ticks);
//
// and deletes the marker. `Office` is constructible from
// `std::span<const Element>` and has `tick()`, `nodes()` and `voltage(i)`;
// `v01_elements()` is the battery, relay, pair and telephone as a vector;
// `lift_wait_replace` is `void(Office&, int tick)` and moves only the handset.
// The marker says item 29, not 20: the clock alone cannot be wired to a
// battery, a relay, a pair and a telephone.
//
// WHAT A MISMATCH MEANS. The comparison is bit for bit, so it is a verdict on
// the tick only if the solve itself is deterministic. A floating-point sum is
// not associative: a netlist that stamps elements into node equations in the
// order they were given will differ in the last bit between two orders with a
// perfectly correct two-phase tick. So the apparatus must solve in a canonical
// order (node-index order, node indices taken from node names and not from the
// order nodes were first met), so that a mismatch is a real defect, state
// leaking mid-solve, and not arithmetic. That is a requirement on items 19 and
// 20, stated in item 18. -0.0 is folded to +0.0 before hashing, since no
// instrument sees its sign; any NaN or infinity is a failure, never a match.
//
// Vacuity is refused in the check. A pass needs the voltages to move over the
// run, and the hand to matter: the same run with a hand that does nothing must
// hash differently; and removing any one element must change the hash, so no
// element is inert. A constant office, an office that ignores its elements
// (even one that answers the hand) and a hand that never lifts all measure 0.
// Removal shows an element exists in the trace, not that its value matters;
// perturbing values would need an interface the concept does not have. `determinism.detector` below runs the
// machinery against small fake offices, one per way the check could be fooled,
// so the templates are compiled under `make strict` and the detector is itself
// judged.
//
// What is not modelled:
//
//   * Machines. "Any machine" is asserted by hashing bit patterns, not by
//     running on two; that needs CI on two platforms. The check removes the
//     excuse (no tolerance); it cannot supply the second machine.
//   * Orders. Up to six elements, every permutation (at most 720): an order
//     bug of any shape is covered. Beyond six, the given order is compared
//     with each rotation, the reversal and 24 fixed-seed shuffles. Rotations
//     put every element ahead of every other, so a pairwise leak is flipped by
//     some order, but a leak that fires only for one particular adjacency
//     (element a solved immediately before b) is NOT guaranteed to be hit by
//     the larger set, only by the exhaustive one. The v0.1 line is four
//     elements, so it is exhaustive today; later offices must be handled by
//     the seeded shuffles or by splitting the check. Three or more elements
//     are required; fewer is a failure, not a pass.
//   * Reduction order versus leaked state. If a mismatch appears, this check
//     cannot say which of the two it is; the requirement on the apparatus
//     above is what makes it mean the second.

#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <utility>
#include <vector>

#include "verify.hpp"

namespace laporte::checks::determinism {

// What the check needs of an office, and nothing it could use to cheat: build
// it from elements in a given order, tick it, read node voltages as doubles.
template <class O, class Element>
concept Office = std::constructible_from<O, std::span<const Element>> &&
    requires(O& o, const O& c, std::size_t i) {
        o.tick();
        { c.nodes() } -> std::convertible_to<std::size_t>;
        { c.voltage(i) } -> std::convertible_to<double>;
    };

// FNV-1a over bytes. Chosen for being a dozen lines, with no table to get wrong.
class Fnv1a {
public:
    void word(std::uint64_t w) {
        for (int i = 0; i < 8; ++i) { h_ ^= (w >> (8 * i)) & 0xffu; h_ *= 1099511628211ull; }
    }
    // The exact bits, except that -0.0 is +0.0: equal under ==, and a sign no
    // instrument can read.
    void bits(double d) { word(std::bit_cast<std::uint64_t>(d == 0.0 ? 0.0 : d)); }
    std::uint64_t value() const { return h_; }
private:
    std::uint64_t h_ = 14695981039346656037ull;
};

// Up to this many elements, every permutation is tried (6! = 720 runs).
inline constexpr std::size_t exhaustive_up_to = 6;
// Beyond it, this many fixed-seed shuffles are tried as well.
inline constexpr int seeded_shuffles = 24;

// The other orders. For n <= 6, every permutation but the given one. For
// larger n, each rotation by k in 1..n-1, the reversal, and `seeded_shuffles`
// Fisher-Yates shuffles driven by a fixed 64-bit LCG, so they are the same on
// every machine.
template <class E>
std::vector<std::vector<E>> other_orders(const std::vector<E>& in) {
    std::vector<std::vector<E>> out;
    const std::size_t n = in.size();
    if (n <= exhaustive_up_to) {
        std::vector<std::size_t> ix(n);
        for (std::size_t i = 0; i < n; ++i) ix[i] = i;
        while (std::next_permutation(ix.begin(), ix.end())) {
            std::vector<E> p;
            for (const std::size_t i : ix) p.push_back(in[i]);
            out.push_back(std::move(p));
        }
        return out;
    }
    for (std::size_t k = 1; k < n; ++k) {
        std::vector<E> r = in;
        std::rotate(r.begin(), r.begin() + static_cast<std::ptrdiff_t>(k), r.end());
        out.push_back(std::move(r));
    }
    out.emplace_back(in.rbegin(), in.rend());
    std::uint64_t s = 0x9e3779b97f4a7c15ull;
    for (int r = 0; r < seeded_shuffles; ++r) {
        std::vector<E> p = in;
        for (std::size_t i = n - 1; i > 0; --i) {
            s = s * 6364136223846793005ull + 1442695040888963407ull;
            std::swap(p[i], p[static_cast<std::size_t>(s >> 33) % (i + 1)]);
        }
        out.push_back(std::move(p));
    }
    return out;
}

struct Trace {
    std::uint64_t hash = 0;
    bool finite = true;   // no voltage was NaN or infinite
    bool varied = false;  // some node's voltage changed during the run
};

// One hash for the whole run: the node count and every voltage after every
// tick, so a difference at tick 3 that vanishes by tick 40 is still found.
template <class O, class E, class Hand>
    requires Office<O, E>
Trace run_trace(const std::vector<E>& elements, Hand&& hand, int ticks) {
    O office{std::span<const E>{elements}};
    Fnv1a h;
    Trace tr;
    std::vector<double> first;
    for (int t = 0; t < ticks; ++t) {
        hand(office, t);
        office.tick();
        const std::size_t n = office.nodes();
        h.word(static_cast<std::uint64_t>(n));
        if (t == 0) first.assign(n, 0.0);
        for (std::size_t i = 0; i < n; ++i) {
            const double v = office.voltage(i);
            if (!std::isfinite(v)) tr.finite = false;
            if (t == 0) first[i] = v;
            else if (i < first.size() && v != first[i]) tr.varied = true;
            h.bits(v);
        }
    }
    tr.hash = h.value();
    return tr;
}

// 1 if every order hashes alike, else 0. Never 1 for want of anything to
// compare: needs three elements, a finite trace that varies, and a hand that
// changes the trace (the same run with a hand that does nothing must hash
// differently), and in which removing any single element changes the hash (an
// element whose absence is invisible is inert, and an order test on it proves
// nothing).
template <class O, class E, class Hand>
    requires Office<O, E>
double identical_in_either_order(const std::vector<E>& elements, Hand&& hand, int ticks) {
    if (elements.size() < 3 || ticks < 2) return 0.0;
    const Trace base = run_trace<O>(elements, hand, ticks);
    if (!base.finite || !base.varied) return 0.0;
    const Trace idle = run_trace<O>(elements, [](O&, int) {}, ticks);
    if (idle.hash == base.hash) return 0.0;
    for (std::size_t i = 0; i < elements.size(); ++i) {
        std::vector<E> fewer = elements;
        fewer.erase(fewer.begin() + static_cast<std::ptrdiff_t>(i));
        if (run_trace<O>(fewer, hand, ticks).hash == base.hash) return 0.0;
    }
    for (const auto& order : other_orders(elements)) {
        const Trace t = run_trace<O>(order, hand, ticks);
        if (!t.finite || t.hash != base.hash) return 0.0;
    }
    return 1.0;
}

namespace fake {

// Four elements, identified by number, each a node holding a voltage. Every
// tick each moves toward its neighbour's voltage plus the drive. A leak is
// "element `to` reads element `from` after it has moved, if it was solved
// first": the bug the two-phase tick forbids. Node i is element i's whatever
// order they were given in, as the apparatus's must be.
struct Spec {
    int from = -1, to = -1;
    bool ignore_elements = false, nan = false, constant = false;
    bool blind = false;     // answers the hand, ignores its element list
    bool adjacent = false;  // leak only when `from` was solved immediately before `to`
};

template <Spec S>
struct Office {
    explicit Office(std::span<const int> e) : order(e.begin(), e.end()) {}
    void tick() {
        std::array<double, 4> next = v;
        std::array<bool, 4> moved{};
        int prev = -1;
        for (const int id : (S.blind ? std::vector<int>{0, 1, 2, 3} : order)) {
            const auto u = static_cast<std::size_t>(id);
            double seen = v[(u + 1) % 4];
            if (S.to == id && S.from >= 0 &&
                (S.adjacent ? prev == S.from : moved[static_cast<std::size_t>(S.from)]))
                seen = next[static_cast<std::size_t>(S.from)];
            next[u] = 0.5 * v[u] + 0.25 * seen + (drive ? 1.0 : 0.0);
            moved[u] = true;
            prev = id;
        }
        if (!S.ignore_elements && !S.constant) v = next;
        if (S.nan) v[0] = std::numeric_limits<double>::quiet_NaN();
    }
    std::size_t nodes() const { return 4; }
    double voltage(std::size_t i) const { return v[i]; }
    std::vector<int> order;
    std::array<double, 4> v{};
    bool drive = false;
};

template <Spec S>
void lift(Office<S>& o, int t) { if (t == 2) o.drive = true; }
template <Spec S>
void idle(Office<S>&, int) {}

}  // namespace fake

template <fake::Spec S>
bool says_alike(bool hand_lifts = true) {
    const std::vector<int> e{0, 1, 2, 3};
    using O = fake::Office<S>;
    return (hand_lifts ? identical_in_either_order<O>(e, fake::lift<S>, 12)
                       : identical_in_either_order<O>(e, fake::idle<S>, 12)) == 1.0;
}

// The leak from element F/4 to element F%4, for every ordered pair of distinct
// elements: each must be caught.
template <int... F>
int leaks_caught(std::integer_sequence<int, F...>) {
    return (int(!says_alike<fake::Spec{.from = F / 4, .to = F % 4}>()) + ...);
}

// How many of the right verdicts the detector gave: alike for the honest fake,
// different for each of the twelve leaks, and different for a constant office,
// one ignoring its elements, a NaN office, a hand that does nothing, an office
// that ignores its elements but answers the hand, and a leak that fires only
// when element 0 is solved immediately before element 3.
inline constexpr int detector_cases = 1 + 12 + 4 + 2;

inline double detector_measure() {
    int right = int(says_alike<fake::Spec{}>());
    right += leaks_caught(std::integer_sequence<int, 1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 13, 14>{});
    right += int(!says_alike<fake::Spec{.constant = true}>());
    right += int(!says_alike<fake::Spec{.ignore_elements = true}>());
    right += int(!says_alike<fake::Spec{.nan = true}>());
    right += int(!says_alike<fake::Spec{}>(false));
    right += int(!says_alike<fake::Spec{.blind = true}>());
    right += int(!says_alike<fake::Spec{.from = 0, .to = 3, .adjacent = true}>());
    return static_cast<double>(right);
}

inline const bool detector_registered = verify::add({
    .name = "determinism.detector", .source = "selftest", .unit = "",
    .published = static_cast<double>(detector_cases), .tolerance = 0.0, .until_item = 0,
    .measure = detector_measure});

// Until item 29 there is no office, so there is nothing to hash and nothing
// that can be claimed. 0 is not "different hashes": it is "not built yet",
// and the published figure is 1, so this cannot pass.
inline double determinism_measure() {
    return 0.0;  // not built yet: see HOOK-UP above
}

inline const bool determinism_registered = verify::add({
    .name = "determinism.either_order", .source = "unsourced", .unit = "",
    .published = 1.0, .tolerance = 0.0, .until_item = 29,
    .measure = determinism_measure});

}  // namespace laporte::checks::determinism
