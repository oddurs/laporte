// loop_limit.hpp — how far from the exchange you can live.
//
// The longest loop on which the line relay still operates when the handset is
// lifted is not a number anyone types. It falls out of the battery, the
// relay's winding and operate current, the telephone's resistance and the
// copper, and the claim is that what falls out lands where Bell's engineers
// drew the line: the resistance-design limit, 1300 ohms of looped cable (S03).
//
// This check sweeps. It asks the office, one cable length at a time, "does the
// relay operate?", walks outward until the answer is no, and bisects. It never
// computes the limit; the only arithmetic it does on the answer is turning a
// length into the loop resistance that length is, using the copper oracle of
// item 22 (`copper::reference_loop`, S05 and S06), so that the apparatus is
// not marking its own paper.
//
// WHAT IS EXPECTED, AND ON WHAT AUTHORITY. 1300 ohms is S03: N. G. Long, "The
// Loop Plant: Part I", BSTJ April 1978: "The standard resistance limit is 1300
// ohms total (combined or looped resistance of the two pair wires)." That is
// thirteen years after the era, no 1965 statement of it has been found, and
// the same article proposes 1500 ohms for electronic offices. The era caveat
// is printed in the source column of every result. The ledger says nothing
// about which side of 1300 a step-by-step office's own limit falls; S03 says
// only that for such an office the line-circuit threshold (S07) "is what
// bites", and the relay's operate current is unsourced (S11). The direction
// below is therefore NOT from the ledger. It is item 30's own reasoning: a
// standard is a limit with a margin in it, drawn so that a worst-case relay,
// battery and temperature still work, so a nominal relay on a nominal battery
// at a nominal temperature should operate beyond it.
//
// A premise outside the ledger: that S03's 1300 ohms is a figure at a
// reference temperature (taken here as 20 C, S05's), so that comparing it with
// a model at 20 C is like with like. S03 does not say so. If 1300 ohms was a
// worst-case-temperature figure, the margin below is already spent in it.
//
// Hence the shape of the checks, which is one band made of two parts.
//
//   * `loop.limit_vs_design`, 1300 ohms +/- 390 (30 %). The point of the
//     derivation is that the limit lands near Bell's. The 30 % is a stated
//     judgement, not a figure: the margin is made of effects this model does
//     not have (worst-case relay, battery, temperature, S03), so some
//     disagreement is the expected result, but a limit more than 30 %
//     away from the standard is not the same engineering and means the model
//     is wrong. The only observed movement of the standard itself is 1300 to
//     1500 (S03, 15 %). The inspector may overrule the 30 %.
//   * `loop.margin_is_real`, the "agreement too good" check. The derived
//     limit must clear 1300 ohms by more than a copper-temperature margin:
//     the resistance of the cable rises by 0.393 % per degree (S05), and the
//     model is at 20 C, so a plant that is 20 C warmer than that has 7.9 %
//     more resistance. The 20 C span is this check's own modest choice
//     (unsourced; the coefficient is S05's). A model that lands inside that
//     margin, or exactly on 1300, has modelled neither the margin nor the
//     things it is made of, and has probably been nudged. This is the lower
//     edge of the band, 1402 ohms; the upper edge is 1690.
//
// `loop.gauge_independent` is the cross-check that the limit is a resistance,
// not a length: the sweep is run on 22, 24 and 26 AWG, and since the loop is
// DC the three limits, expressed in ohms, are one number. Different answers in
// ohms would mean something other than resistance is limiting the loop.
//
// HOOK-UP, for the author of item 31 (the check itself may not be edited; this
// is the whole change, and it is the body of `line_relay_operates` at the
// bottom):
//
//     #include <laporte/laporte.hpp>   // the office of item 29
//     ...
//     build the office with a pair of this gauge and length, lift the handset,
//     tick until settled, and return whether the line relay's contacts are
//     closed;  // replace the `return false`
//
// `awg` is an `int` and `length` is `Metres`. The office must be built fresh
// for each call. Anything the hook-up has to change besides that one function
// is a sign the interface drifted. The stub answers "no", so the sweep fails
// at zero length and the result is NaN.
//
// What is not modelled:
//
//   * Anything but DC. No line capacitance, so no pulse rounding and no
//     transmission loss: this is the DC limit, which is the one Bell's
//     1300 ohms is, and it says so.
//   * Temperature. The copper is at 20 C (S05); Bell's limit is a worst case
//     over temperature, which is part of the margin the check expects.
//   * The worst-case relay. The check judges the nominal one and expects a
//     margin; it cannot say how much margin a worst-case relay would use.
//   * Non-monotonic answers. A relay that operates at 5 kft, fails at 8 and
//     operates at 9 is not a limit; the sweep says NaN rather than choose.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <limits>
#include <numbers>

#include <laporte/units.hpp>

#include "copper.hpp"
#include "verify.hpp"

namespace laporte::checks::loop_limit {

inline constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

// S03. 1978; see the header for what that does and does not license.
inline constexpr double design_limit_ohms = 1300.0;

// The oracle is item 22's, not a copy: the same derivation, judged against
// the Handbook's table by `copper.oracle_vs_table`.
inline Ohms loop_ohms(int awg, Metres length) { return copper::reference_loop(awg, length); }

// S05: the temperature coefficient of the standard, per degree C at 20 C, and
// this check's own choice of how much warmer than 20 C a plant may be.
inline constexpr double copper_per_degree = 0.00393;
inline constexpr double warmer_by_degrees = 20.0;
inline constexpr double margin_floor_ohms =
    design_limit_ohms * (1.0 + copper_per_degree * warmer_by_degrees);

// All the check asks of an office: for this gauge and length, with the handset
// lifted, does the line relay operate?
template <class F>
concept Operates = requires(F f, int awg, Metres len) {
    { f(awg, len) } -> std::convertible_to<bool>;
};

// The longest length that operates, in metres, found by sweeping and then
// bisecting; NaN unless the answers are those of a limit: operates at zero,
// fails somewhere in range, operates everywhere well below and fails
// everywhere well above.
template <Operates F>
double sweep_limit_metres(F&& operates, int awg) {
    const double step = 500.0 * metres_per_foot;
    const double reach = 300.0e3 * metres_per_foot;
    const auto yes = [&](double m) { return static_cast<bool>(operates(awg, Metres{m})); };

    if (!yes(0.0)) return not_a_number;
    double lo = 0.0, hi = -1.0;
    for (double m = step; m <= reach; m += step) {
        if (!yes(m)) { hi = m; break; }
        lo = m;
    }
    if (hi < 0.0) return not_a_number;
    for (int i = 0; i < 40; ++i) {
        const double mid = 0.5 * (lo + hi);
        (yes(mid) ? lo : hi) = mid;
    }
    const double limit = 0.5 * (lo + hi);
    for (const double f : {0.25, 0.5, 0.9})
        if (!yes(limit * f)) return not_a_number;
    for (const double f : {1.1, 1.5, 2.0, 4.0})
        if (yes(limit * f)) return not_a_number;
    return limit;
}

inline constexpr std::array<int, 3> gauges{22, 24, 26};

// The derived limits as loop resistance, one per gauge; any NaN poisons the lot.
template <Operates F>
std::array<double, 3> limits_ohms(F&& operates) {
    std::array<double, 3> out{};
    for (std::size_t g = 0; g < gauges.size(); ++g) {
        const double m = sweep_limit_metres(operates, gauges[g]);
        out[g] = std::isfinite(m) ? as::ohms(loop_ohms(gauges[g], Metres{m})) : not_a_number;
    }
    return out;
}

inline bool all_finite(const std::array<double, 3>& v) {
    return std::all_of(v.begin(), v.end(), [](double x) { return std::isfinite(x); });
}

// The figure compared with 1300: the mean over gauges (they are asserted to be
// one number by the spread check, so the mean is not hiding a difference).
inline double mean_ohms(const std::array<double, 3>& v) {
    return all_finite(v) ? (v[0] + v[1] + v[2]) / 3.0 : not_a_number;
}

// 1 if every gauge clears the design limit by the temperature margin; 0 if any does not;
// NaN if there is no limit to speak of.
inline double margin_is_real(const std::array<double, 3>& v) {
    if (!all_finite(v)) return not_a_number;
    return *std::min_element(v.begin(), v.end()) > margin_floor_ohms ? 1.0 : 0.0;
}

// Spread of the limit across gauges, in percent of the mean.
inline double spread_percent(const std::array<double, 3>& v) {
    if (!all_finite(v)) return not_a_number;
    const auto [lo, hi] = std::minmax_element(v.begin(), v.end());
    return 100.0 * (*hi - *lo) / mean_ohms(v);
}

// ── The self-test: offices of the check's own, labelled as such ───────────
//
// A fake office is a battery, a relay of a given operate current and a fixed
// series resistance, over the copper oracle. It is the answer the check itself
// is allowed to know, which is exactly why it is a fake and never the thing
// judged. SYNTHETIC: 48 V, 16 mA and 600 ohm are nobody's relay.

inline double fake_limit_ohms(double operate_a, double fixed_ohms) {
    return 48.0 / operate_a - fixed_ohms;
}

inline bool fake_operates(double operate_a, double fixed_ohms, int awg, Metres len) {
    return 48.0 / (as::ohms(loop_ohms(awg, len)) + fixed_ohms) >= operate_a;
}

inline double selftest_measure() {
    const auto office = [](double op, double fixed) {
        return [=](int awg, Metres len) { return fake_operates(op, fixed, awg, len); };
    };
    bool ok = true;

    // A sound office: the sweep finds the analytic limit, in ohms, at every gauge.
    const auto sound = limits_ohms(office(0.016, 600.0));
    const double truth = fake_limit_ohms(0.016, 600.0);   // 2400
    ok = ok && all_finite(sound);
    for (const double v : sound) ok = ok && std::abs(v - truth) < 1e-3;
    ok = ok && spread_percent(sound) < 1e-3 && margin_is_real(sound) == 1.0;

    // Agreement too good: tuned to land on 1300 to the ohm. Must read as not real.
    const auto tuned = limits_ohms(office(0.016, 48.0 / 0.016 - design_limit_ohms));
    ok = ok && margin_is_real(tuned) == 0.0;
    // Nudged to 1320, inside the temperature margin: also not real.
    ok = ok && margin_is_real(limits_ohms(office(0.016, 48.0 / 0.016 - 1320.0))) == 0.0;
    // And 1500, past the margin and inside the band: real.
    ok = ok && margin_is_real(limits_ohms(office(0.016, 48.0 / 0.016 - 1500.0))) == 1.0;
    // And one that lands short of the standard.
    ok = ok && margin_is_real(limits_ohms(office(0.016, 2000.0))) == 0.0;

    // A limit set by length, not resistance, differs by gauge.
    const auto by_length = limits_ohms([](int awg, Metres len) {
        return len.v < 5000.0 * metres_per_foot * 1.0 && awg > 0; });
    ok = ok && spread_percent(by_length) > 10.0;

    // Not a limit at all: never fails, never operates, flickers.
    ok = ok && !all_finite(limits_ohms([](int, Metres) { return true; }));
    ok = ok && !all_finite(limits_ohms([](int, Metres) { return false; }));
    ok = ok && !all_finite(limits_ohms([](int, Metres len) {
        return len.v < 4000.0 || (len.v > 5500.0 && len.v < 7000.0); }));
    ok = ok && std::isnan(mean_ohms(limits_ohms([](int, Metres) { return false; })));
    return ok ? 1.0 : 0.0;
}

// The one function the author of item 31 edits. Until then the relay never
// operates, so there is no limit and every result is NaN, which no tolerance
// accepts.
inline bool line_relay_operates(int, Metres) {
    return false;  // not built yet: replace with the office's line relay, handset lifted
}

inline double limit_vs_design_measure() { return mean_ohms(limits_ohms(line_relay_operates)); }
inline double margin_is_real_measure()  { return margin_is_real(limits_ohms(line_relay_operates)); }
inline double gauge_spread_measure()    { return spread_percent(limits_ohms(line_relay_operates)); }

inline const bool registered_selftest = verify::add({
    .name = "loop.selftest", .source = "selftest", .unit = "",
    .published = 1.0, .tolerance = 0.0, .until_item = 0, .measure = selftest_measure});
inline const bool registered_limit = verify::add({
    .name = "loop.limit_vs_design", .source = "S03 (BSTJ 1978, not 1965; DC only)", .unit = "ohm",
    .published = design_limit_ohms, .tolerance = 390.0, .until_item = 31,
    .measure = limit_vs_design_measure});
inline const bool registered_margin = verify::add({
    .name = "loop.margin_is_real", .source = "S03 (BSTJ 1978, not 1965; DC only)", .unit = "",
    .published = 1.0, .tolerance = 0.0, .until_item = 31, .measure = margin_is_real_measure});
inline const bool registered_spread = verify::add({
    .name = "loop.gauge_independent", .source = "S05 S06", .unit = "%",
    .published = 0.0, .tolerance = 1.0, .until_item = 31, .measure = gauge_spread_measure});

}  // namespace laporte::checks::loop_limit
