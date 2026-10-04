// copper.hpp — a pair's resistance, from the metal and the gauge.
//
// Nobody types "83 ohms per kilofoot". The resistance of a pair of wires is
// copper's resistivity, divided by the area the gauge defines, times the
// length, times two, because a loop is a conductor out and a conductor back.
// Forgetting the second is the classic mistake and it is a factor of two, so
// this check is shaped to be unable to miss it.
//
// It judges `cable.hpp` (item 23), which does not exist yet, and it has two
// legs that are not equally well sourced.
//
//   * Against the derivation. Resistivity is S05 (NBS Handbook 100, 1966) and
//     the gauge is S06's definition, d = 0.005 in x 92^((36-n)/39). The
//     reference computation below is real code and is the check's own oracle;
//     it never touches the cable. 19, 22, 24 and 26 AWG, at 1, 5 and 10
//     kilofeet, so that a length that is not carried through shows.
//   * Against the table. S06 also carries Handbook Table 5, ohms per 1000 ft
//     of a *single* annealed conductor at 20 C: 22 AWG 16.2, 24 AWG 25.7,
//     26 AWG 41.0. A loop is twice that. THE LEDGER HAS NO FIGURE FOR 19 AWG,
//     so 19 AWG has no published leg: it is judged against the derivation
//     only, and that is said here rather than a number invented to fill the
//     row. If somebody fetches Table 5's 19 AWG line into the ledger, the
//     table leg gains a gauge; until then it does not.
//
// The oracle is itself judged, against the table, by a check that can pass
// today (`copper.oracle_vs_table`): if the formula is wrong the cable has
// nothing sound to be measured against, and the way to learn that is not to
// discover it the day the cable lands.
//
// HOOK-UP, for the author of item 23 (the check itself may not be edited; this
// is the whole change, and it is the body of `cable_loop_resistance` at the
// bottom):
//
//     #include <laporte/cable.hpp>
//     ...
//     return laporte::Cable{awg, length}.loop_resistance();   // replace the NaN
//
// returning `Ohms`, the resistance of the pair as a loop (tip and ring in
// series, far end shorted) at 20 C. `awg` is an `int`, `length` is `Metres`.
// If the interface wants something else, adapt it inside that one function;
// a change anywhere else in this file is a sign the interface has drifted.
//
// What is not modelled:
//
//   * Temperature. Table 5 is 20 C and so is the oracle; S05 gives 0.393 % per
//     degree, and a cable that models it is compared at 20 C.
//   * Annealed against drawn. S05 and S06 are annealed copper; cable
//     conductors were drawn, a little more resistive. The 2 % tolerance
//     covers the Handbook's rounding of three significant figures, not that.
//   * Anything but DC. This is a resistance, not an impedance.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>

#include <laporte/units.hpp>

#include "verify.hpp"

namespace laporte::checks::copper {

// S05: 0.017 241 ohm-mm^2/metre at 20 C, i.e. ohm-metres once the square
// millimetre is a millionth of a square metre.
inline constexpr double resistivity_ohm_m = 0.017241 / 1e6;

// S06: the Handbook's geometric progression, in metres. The inch is the foot
// divided by twelve, so the one defined constant in units.hpp is the only one.
inline double awg_diameter_m(int n) {
    const double inch = metres_per_foot / 12.0;
    return 0.005 * inch * std::pow(92.0, (36.0 - n) / 39.0);
}

// The reference: both conductors of the loop, resistivity over area, times
// length.
inline Ohms reference_loop(int awg, Metres length) {
    const double d = awg_diameter_m(awg);
    const double area = std::numbers::pi * d * d / 4.0;
    return Ohms{2.0 * resistivity_ohm_m * length.v / area};
}

// S06, Table 5, one conductor, ohms per 1000 ft at 20 C. 19 AWG is absent on
// purpose: the ledger has no figure for it.
struct Published { int awg; double single_ohm_per_kft; };
inline constexpr std::array<Published, 3> table5{{{22, 16.2}, {24, 25.7}, {26, 41.0}}};
inline constexpr std::array<int, 4> gauges{19, 22, 24, 26};
inline constexpr std::array<double, 3> kilofeet{1.0, 5.0, 10.0};

// A callable `Ohms(int, Metres)` is all the check asks of a cable.
template <class F>
concept LoopResistance = requires(F f, int awg, Metres len) {
    { f(awg, len) } -> std::convertible_to<Ohms>;
};

inline constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

// Worst relative deviation, in percent, of `cable` from the derivation, over
// every gauge and length. A non-finite or non-positive answer is not "close to
// anything": it makes the whole result NaN, which no tolerance accepts.
template <LoopResistance F>
double worst_percent_vs_derivation(F&& cable) {
    double worst = 0.0;
    for (const int awg : gauges)
        for (const double kft : kilofeet) {
            const Metres len = Metres{kft * 1e3 * metres_per_foot};
            const double got = as::ohms(Ohms{cable(awg, len)});
            if (!std::isfinite(got) || got <= 0.0) return not_a_number;
            const double want = as::ohms(reference_loop(awg, len));
            worst = std::max(worst, 100.0 * std::abs(got - want) / want);
        }
    return worst;
}

// Worst deviation, in percent, from twice the Handbook's single-conductor
// figure, per kilofoot, over the gauges the ledger has and the same lengths.
template <LoopResistance F>
double worst_percent_vs_table(F&& cable) {
    double worst = 0.0;
    for (const Published& p : table5)
        for (const double kft : kilofeet) {
            const Metres len = Metres{kft * 1e3 * metres_per_foot};
            const double got = as::ohms(Ohms{cable(p.awg, len)}) / kft;
            if (!std::isfinite(got) || got <= 0.0) return not_a_number;
            const double want = 2.0 * p.single_ohm_per_kft;
            worst = std::max(worst, 100.0 * std::abs(got - want) / want);
        }
    return worst;
}

// Fakes, for the self-test only. They are the check's own attempts to be
// wrong in the ways a cable could be, so that the check is shown to notice.
inline Ohms fake_right(int awg, Metres l)        { return reference_loop(awg, l); }
inline Ohms fake_one_wire(int awg, Metres l)     { return reference_loop(awg, l) / 2.0; }
inline Ohms fake_ignores_length(int awg, Metres) { return reference_loop(awg, Metres{1000.0 * metres_per_foot}); }
inline Ohms fake_nan(int, Metres)                { return Ohms{not_a_number}; }
inline Ohms fake_zero(int, Metres)               { return Ohms{0.0}; }

// The oracle against the table, once, with the real figures. 2 %, because
// Table 5 prints three significant figures and the least of them (16.2 for a
// true 16.15) is already 0.3 % off.
inline double oracle_vs_table_measure() { return worst_percent_vs_table(fake_right); }

// 1 if every fake is treated as it should be, else 0. A check that has never
// been shown to fail has not been shown to check anything.
inline double selftest_measure() {
    const bool right_ok   = worst_percent_vs_derivation(fake_right) < 1e-9;
    const bool one_wire   = std::abs(worst_percent_vs_derivation(fake_one_wire) - 50.0) < 1e-6;
    const bool one_wire_t = std::abs(worst_percent_vs_table(fake_one_wire) - 50.0) < 1.0;
    const bool no_length  = worst_percent_vs_derivation(fake_ignores_length) > 50.0;
    const bool nan_out    = std::isnan(worst_percent_vs_derivation(fake_nan)) &&
                            std::isnan(worst_percent_vs_table(fake_nan));
    const bool zero_out   = std::isnan(worst_percent_vs_derivation(fake_zero));
    // And the harness itself must not read a NaN as agreement, with any tolerance.
    const verify::Check probe{"probe", "selftest", "", 0.0, 1e9, 0, nullptr};
    const bool judged = verify::judge(probe, not_a_number) == verify::Verdict::fail;
    return right_ok && one_wire && one_wire_t && no_length && nan_out && zero_out && judged
               ? 1.0 : 0.0;
}

// The one function the author of item 23 edits. Until then it answers NaN,
// which no tolerance accepts, so the registered checks cannot pass by default.
inline Ohms cable_loop_resistance(int, Metres) {
    return Ohms{not_a_number};  // not built yet: replace with the cable's loop resistance
}

inline double cable_vs_derivation_measure() { return worst_percent_vs_derivation(cable_loop_resistance); }
inline double cable_vs_table_measure()      { return worst_percent_vs_table(cable_loop_resistance); }

inline const bool registered_oracle = verify::add({
    .name = "copper.oracle_vs_table", .source = "S05 S06", .unit = "%",
    .published = 0.0, .tolerance = 2.0, .until_item = 0, .measure = oracle_vs_table_measure});
inline const bool registered_selftest = verify::add({
    .name = "copper.selftest", .source = "selftest", .unit = "",
    .published = 1.0, .tolerance = 0.0, .until_item = 0, .measure = selftest_measure});
inline const bool registered_derivation = verify::add({
    .name = "copper.cable_vs_derivation", .source = "S05 S06", .unit = "%",
    .published = 0.0, .tolerance = 0.01, .until_item = 23, .measure = cable_vs_derivation_measure});
inline const bool registered_table = verify::add({
    .name = "copper.cable_vs_table", .source = "S06 (no 19 AWG)", .unit = "%",
    .published = 0.0, .tolerance = 2.0, .until_item = 23, .measure = cable_vs_table_measure});

}  // namespace laporte::checks::copper
