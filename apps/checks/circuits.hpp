// circuits.hpp — Kirchhoff, checked against circuits whose answers are known.
//
// A solver that is only ever shown to agree with itself has not been checked,
// and the thing most likely to be wrong in a circuit solver is the thing a
// person cannot see by looking at a waveform: a time constant that is one tick
// too long, a 3.4 kHz tone that is a quarter of a dB quieter than it should be,
// an inductor that rings for a millisecond after a contact opens. So this file
// judges the netlist and the clock against closed-form answers, written here as
// code (the oracles below), before the solver exists, with every tolerance
// declared in this comment and not tuned to anything.
//
// It is arguing with "it looks right on the scope". Item 14 set the bound
// before measuring and the pre-approved 8 kHz clock failed it; the bound is
// reused here, not relaxed:
//
//     AC steady state:  |amplitude error| <= 0.5 dB and |phase error| <= 5 deg,
//                       at 20 Hz, 2.6 kHz and 3.4 kHz, on an RC and on an RLC.
//
// What 48 kHz trapezoidal achieves, from item 14's measured cells (simulated
// minus exact, dB / deg): RC 2.6 kHz -0.07/-0.19, 3.4 kHz -0.13/-0.26; RLC
// 2.6 kHz +0.01/-0.69, 3.4 kHz -0.06/-1.77; 20 Hz zero. The worst, 0.13 dB and
// 1.77 deg, are well inside the bound, so a pass is achievable by the adopted
// method and a numerically damped solver (backward Euler is 2.9 dB, 11.6 deg
// at 48 kHz) is caught. The component values are the spike's: placeholders of
// the right order, not ledger figures.
//
// The other bounds, and how each was chosen:
//
//   * Resistive divider: 1e-6 V on a 48 V, 400/600 ohm divider. GMIN's
//     leakage there is about 1e-8 V, so a solver that adds a conductance
//     orders of magnitude too large is caught and an honest one is not.
//   * GMIN: on a 10 Mohm / 10 Mohm divider the 1e-12 S floor shifts the
//     midpoint by about 1.2e-4 V; the bound is 1e-3 V. An island of nodes
//     touching no ground (a 1 V source across a resistor, both floating) must
//     still solve: every voltage finite and within 1 V of ground, and the
//     difference across the source 1 V to within 1e-6. A matrix that is
//     singular without GMIN gives NaN or an unbounded common mode and fails.
//   * Step responses (RC, RL), against 1 - exp(-t/tau) sampled at the tick
//     times n/48000, with a step that is 0 at tick 0 and 1 from tick 1. A step
//     landing on a tick boundary is half a tick late to a trapezoid by
//     construction: the exact discrete answer is 1 - z^(n-1)/(1+a), with
//     a = h/(2 tau) and z = (1-a)/(1+a), which sits half a tick behind the
//     continuous curve (oracle `ideal_trapezoid_step`). Item 14 measured only
//     steady state, so no spike figure speaks for the transient; the bound is
//     derived instead: the pointwise error, expressed as a delay in ticks (the
//     error divided by the curve's initial slope times h), may be at most 0.75
//     tick. Half a tick is owed; a whole tick is the off-by-one in the
//     companion model that the item names, and fails. The time constant is then
//     read from the tail (ticks at 1 tau and 4 tau, where 1-v is still far
//     above rounding) and must be within 1%: the ideal trapezoid is within
//     0.14%, backward Euler is 5.8% off on this RC and 2.0% on this RL.
//   * The switched inductor: 48 V through a contact into a 400 ohm, 0.2 H relay
//     winding, opened after the current has settled. The exact answer has no
//     numerical kick. Two bounds: the peak |v| over the whole run must stay
//     within 1.5 * L*I0/h (the backward-Euler kick, 1152 V here; a trapezoidal
//     first step is twice that); and, from the ninth tick (after the eight
//     backward-Euler ticks) to the six-hundredth, |v| at the winding must be
//     under 0.05 V. Item 14 measured 5.3e-3 V for 100 kohm of leakage at
//     this rate and the floor, 2e-8 V, for none; two ticks leave 19 V and
//     four leave 0.32 V, so the bound tells eight from four. Both are run:
//     a 100 kohm quench, and nothing across the coil but GMIN.
//
// Vacuity is refused in the check. A solver that returns zeros, NaN, a
// constant, or a different tick rate than the spike adopted measures a failure
// and cannot measure a pass: every error is computed from voltages the solver
// returned, any non-finite value becomes infinity, and the switched circuit
// must first be seen energised (48 V within 1%) before it is opened.
// `circuits.detector` below runs the same machinery against a small honest
// solver (modified nodal analysis, dense, with the spike's method) and against
// one fake per way the check could be fooled, and is itself a check, so the
// templates are compiled under `make strict` and judged.
//
// HOOK-UP. The apparatus authors (items 19, 20) may not edit this file. When
// item 19 lands, the test department or the inspector changes two lines:
//
//     #include <laporte/kirchhoff.hpp>                  (top of this file)
//     using Resistive = laporte::Netlist;               (was: NotBuilt)
//
// and, when item 20 lands, `using Clocked = laporte::Office;`, then removes
// the `until_item` markers. `Netlist`/`Office` stand for the apparatus's real
// names; each need only satisfy the `Solver` concept below: construct from
// `std::span<const Part>`, `tick()`, `drive(name, volts)` (sets a source for
// the next tick), `set_contact(name, closed)` (the clock then owes eight
// backward-Euler ticks), `voltage(node)` and `tick_hz()` (must be 48000). A
// fresh netlist starts with every voltage and current at zero. The ground node
// is named "gnd". If the apparatus cannot satisfy the concept, that is a note
// on the item for the inspector, not a reason to edit this file.
//
// Markers. Item 19 delivers the netlist and its stamping, so the two
// resistive checks wait for 19. The history of a capacitor or inductor is
// state that "every element advances" (item 20's tick) and the eight
// backward-Euler ticks are the clock's rule, so every dynamic check waits for
// 20. Item 19's acceptance criterion "every circuit in the solver check
// passes" can therefore only be met by 20 landing; that is for the director.
//
// What is not modelled:
//
//   * Anything nonlinear: a carbon transmitter, a rectifier, an armature that
//     moves. Linear, lumped, one contact.
//   * The step is an ideal one at a tick boundary, and the contact a perfect
//     open. Real contacts arc and real relay windings have capacitance; item
//     14 says that belongs in `kirchhoff.hpp`'s list or the relay's.
//   * Transient accuracy at 2.6 and 3.4 kHz. Only the steady state is
//     compared there, because that is all item 14 measured.
//   * The number 8. It is not derived (item 14 says so); the check proves
//     that eight suffices and four does not at this coil and rate, not that
//     eight is the least that works for any coil.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <span>
#include <string_view>
#include <vector>

#include <laporte/units.hpp>

#include "verify.hpp"

namespace laporte::checks::circuits {

// ---------------------------------------------------------------- the bounds

inline constexpr double bound_db  = 0.5;
inline constexpr double bound_deg = 5.0;
inline constexpr double bound_divider_volts = 1e-6;
inline constexpr double bound_gmin_leak_volts = 1e-3;
inline constexpr double bound_island_volts = 1e-6;  // the difference across a floating source
inline constexpr double bound_step_ticks = 0.75;
inline constexpr double bound_tau_fraction = 0.01;
inline constexpr double bound_kick_ratio = 1.5;
inline constexpr double bound_ring_volts = 0.05;
inline constexpr int    be_ticks = 8;

inline constexpr Hertz  tick_rate{48000.0};
inline constexpr double inf = std::numeric_limits<double>::infinity();

// ------------------------------------------------------------- the interface

enum class Kind { resistor, capacitor, inductor, source, contact };

// One two-terminal part, wired by names. `value` is SI: ohms, farads, henries,
// and for a contact its resistance when closed. A source has no value; its
// voltage is whatever `drive` last set, a to b.
struct Part {
    Kind kind;
    std::string_view name, a, b;
    double value;
};

constexpr Part resistor(std::string_view n, std::string_view a, std::string_view b, Ohms r)     { return {Kind::resistor, n, a, b, r.v}; }
constexpr Part capacitor(std::string_view n, std::string_view a, std::string_view b, Farads c)  { return {Kind::capacitor, n, a, b, c.v}; }
constexpr Part inductor(std::string_view n, std::string_view a, std::string_view b, Henries l)  { return {Kind::inductor, n, a, b, l.v}; }
constexpr Part source(std::string_view n, std::string_view a, std::string_view b)               { return {Kind::source, n, a, b, 0.0}; }
constexpr Part contact(std::string_view n, std::string_view a, std::string_view b, Ohms closed) { return {Kind::contact, n, a, b, closed.v}; }

template <class S>
concept Solver = std::constructible_from<S, std::span<const Part>> &&
    requires(S& s, const S& c, std::string_view n, double x, bool closed) {
        s.tick();
        s.drive(n, x);
        s.set_contact(n, closed);
        { c.voltage(n) } -> std::convertible_to<double>;
        { c.tick_hz() } -> std::convertible_to<double>;
    };

// The apparatus is not built. Satisfies nothing, so every measure is infinite.
struct NotBuilt {};
using Resistive = NotBuilt;  // item 19: the netlist
using Clocked   = NotBuilt;  // item 20: the netlist on its clock

// Never let a NaN or an infinity be mistaken for a small number.
inline double finite_or_inf(double x) { return std::isfinite(x) ? x : inf; }

// ---------------------------------------------------------------- the oracles

constexpr Seconds h() { return period(tick_rate); }

constexpr Seconds time_constant(Ohms r, Farads c)  { return Seconds{r.v * c.v}; }
constexpr Seconds time_constant(Henries l, Ohms r) { return Seconds{l.v / r.v}; }

// 1 - exp(-t/tau): the unit step into an RC, or the RL's voltage across R.
inline double exact_step(Seconds t, Seconds tau) { return 1.0 - std::exp(-(t / tau)); }

// The exact discrete answer of trapezoidal integration to a step that is 0 at
// tick 0 and 1 afterwards. Half a tick behind the continuous curve, which is
// the whole reason the step bound is 0.75 and not 0.
inline double ideal_trapezoid_step(int n, Seconds tau) {
    const double a = (h() / tau) / 2.0;
    const double z = (1.0 - a) / (1.0 + a);
    return 1.0 - std::pow(z, n - 1) / (1.0 + a);
}

// An RC low-pass, and a series-R-and-L into a shunt C. The output is the
// capacitor's voltage.
inline std::complex<double> rc_response(Hertz f, Ohms r, Farads c) {
    const double w = 2.0 * std::numbers::pi * f.v;
    return 1.0 / std::complex<double>{1.0, w * r.v * c.v};
}
inline std::complex<double> rlc_response(Hertz f, Ohms r, Henries l, Farads c) {
    const double w = 2.0 * std::numbers::pi * f.v;
    return 1.0 / std::complex<double>{1.0 - w * w * l.v * c.v, w * r.v * c.v};
}

// ---------------------------------------------------------- the circuits

// Item 14's, so that its measured cells are the reference.
inline constexpr Ohms    rc_r{1000.0};
inline constexpr Farads  rc_c{159.155e-9};
inline constexpr Ohms    rlc_r{300.0};
inline constexpr Henries rlc_l{11.94e-3};
inline constexpr Farads  rlc_c{0.1326e-6};
// A step circuit for the RL: 400 ohm and 0.2 H, tau = 0.5 ms, 24 ticks.
inline constexpr Ohms    rl_r{400.0};
inline constexpr Henries rl_l{0.2};
// The relay winding, its battery, and the quench.
inline constexpr Volts   battery{48.0};
inline constexpr Ohms    winding{400.0};
inline constexpr Henries coil{0.2};
inline constexpr Ohms    quench{100000.0};
inline constexpr Ohms    contact_closed{0.01};

// ------------------------------------------------------- the measurements

template <class S>
bool right_rate(const S& s) { return static_cast<double>(s.tick_hz()) == tick_rate.v; }

// Worst error of 48 V across 400 and 600 ohms, against 28.8 V.
template <class S>
double divider_error() {
    if constexpr (!Solver<S>) return inf;
    else {
        const std::array<Part, 3> p{source("e", "vin", "gnd"), resistor("r1", "vin", "mid", Ohms{400.0}),
                                    resistor("r2", "mid", "gnd", Ohms{600.0})};
        S s{std::span<const Part>{p}};
        if (!right_rate(s)) return inf;
        s.drive("e", battery.v);
        s.tick();
        const Volts out = battery * (600.0 / (400.0 + 600.0));
        return finite_or_inf(std::abs(static_cast<double>(s.voltage("mid")) - out.v));
    }
}

// GMIN: the leakage on a 10 Mohm divider and the island, each as a multiple of
// its bound (so one figure, 1 is the line).
template <class S>
double gmin_over_bound() {
    if constexpr (!Solver<S>) return inf;
    else {
        const std::array<Part, 5> p{source("e", "vin", "gnd"), resistor("r1", "vin", "mid", Ohms{1e7}),
                                    resistor("r2", "mid", "gnd", Ohms{1e7}),
                                    source("i", "f1", "f2"), resistor("f", "f2", "f3", Ohms{1e3})};
        S s{std::span<const Part>{p}};
        if (!right_rate(s)) return inf;
        s.drive("e", battery.v);
        s.drive("i", 1.0);
        s.tick();
        const double leak = std::abs(static_cast<double>(s.voltage("mid")) - battery.v / 2.0);
        // The island: a source and a resistor touching no ground. Its common
        // mode is whatever GMIN makes it, which must be small and finite; its
        // difference is the source's.
        const double f1 = static_cast<double>(s.voltage("f1")), f2 = static_cast<double>(s.voltage("f2")),
                     f3 = static_cast<double>(s.voltage("f3"));
        // std::max silently drops a NaN, so refuse one before comparing.
        if (!std::isfinite(leak) || !std::isfinite(f1) || !std::isfinite(f2) || !std::isfinite(f3)) return inf;
        const double island = std::max({std::abs(f1), std::abs(f2), std::abs(f3),
                                        std::abs(f1 - f2 - 1.0) / bound_island_volts});
        return finite_or_inf(std::max(leak / bound_gmin_leak_volts, island));
    }
}

// The step into an RC (out across the capacitor) or an RL (out across the
// resistor): the samples at ticks 1..n.
template <class S>
std::vector<double> step_samples(std::span<const Part> p, int n) {
    S s{p};
    std::vector<double> v;
    if (!right_rate(s)) return v;
    s.drive("e", 1.0);
    for (int k = 1; k <= n; ++k) {
        s.tick();
        v.push_back(static_cast<double>(s.voltage("out")));
    }
    return v;
}

// The pointwise error as a delay in ticks, and the time constant's error as a
// fraction. Both infinite for a solver that gives nothing finite.
struct StepVerdict { double ticks = inf, tau = inf; };

inline int ticks_in(Seconds t) { return static_cast<int>(std::lround(t / h())); }

inline StepVerdict judge_step(const std::vector<double>& v, Seconds tau) {
    const int n = ticks_in(tau * 8.0);
    if (static_cast<int>(v.size()) != n) return {};
    for (const double x : v) if (!std::isfinite(x)) return {};  // std::max would drop a NaN
    const double slope_ticks = h() / tau;  // per tick, at t = 0
    double worst = 0.0;
    for (int k = 1; k <= n; ++k)
        worst = std::max(worst, std::abs(v[static_cast<std::size_t>(k - 1)] -
                                         exact_step(h() * static_cast<double>(k), tau)) / slope_ticks);
    const int n1 = ticks_in(tau), n2 = ticks_in(tau * 4.0);
    const double y1 = 1.0 - v[static_cast<std::size_t>(n1 - 1)], y2 = 1.0 - v[static_cast<std::size_t>(n2 - 1)];
    if (!(y1 > 0.0) || !(y2 > 0.0)) return {finite_or_inf(worst), inf};
    const double tau_est = -static_cast<double>(n2 - n1) * h().v / std::log(y2 / y1);
    return {finite_or_inf(worst), finite_or_inf(std::abs(tau_est / tau.v - 1.0))};
}

template <class S>
StepVerdict rc_step() {
    if constexpr (!Solver<S>) return {};
    else {
        const std::array<Part, 3> p{source("e", "in", "gnd"), resistor("r", "in", "out", rc_r),
                                    capacitor("c", "out", "gnd", rc_c)};
        const Seconds tau = time_constant(rc_r, rc_c);
        return judge_step(step_samples<S>(p, ticks_in(tau * 8.0)), tau);
    }
}

template <class S>
StepVerdict rl_step() {
    if constexpr (!Solver<S>) return {};
    else {
        const std::array<Part, 3> p{source("e", "in", "gnd"), inductor("l", "in", "out", rl_l),
                                    resistor("r", "out", "gnd", rl_r)};
        const Seconds tau = time_constant(rl_l, rl_r);
        return judge_step(step_samples<S>(p, ticks_in(tau * 8.0)), tau);
    }
}

// One steady-state cell: the amplitude error in dB and the phase error in
// degrees, simulated minus exact. Drive for 0.1 s to settle (the circuits'
// own time constants are tens of microseconds), then correlate input and
// output over 0.1 s, which holds a whole number of cycles of 20 Hz, 2.6 kHz
// and 3.4 kHz at 48 kHz.
struct Cell { double db = inf, deg = inf; };

template <class S>
Cell ac_cell(std::span<const Part> p, Hertz f, std::complex<double> exact) {
    S s{p};
    if (!right_rate(s)) return {};
    constexpr int settle = 4800, window = 4800;
    const double w = 2.0 * std::numbers::pi * f.v;
    std::complex<double> in{}, out{};
    for (int n = 1; n <= settle + window; ++n) {
        const double t = static_cast<double>(n) * h().v;
        const double u = std::sin(w * t);
        s.drive("e", u);
        s.tick();
        if (n > settle) {
            const std::complex<double> rot = std::polar(1.0, -w * t);
            in += u * rot;
            out += static_cast<double>(s.voltage("out")) * rot;
        }
    }
    const std::complex<double> ratio = out / in;
    if (!std::isfinite(ratio.real()) || !std::isfinite(ratio.imag()) || !(std::abs(ratio) > 0.0)) return {};
    const double db = 20.0 * std::log10(std::abs(ratio) / std::abs(exact));
    const double deg = std::arg(ratio / exact) * 180.0 / std::numbers::pi;
    return {finite_or_inf(db), finite_or_inf(deg)};
}

// Worst cell over both circuits at the three frequencies.
template <class S>
Cell ac_worst() {
    if constexpr (!Solver<S>) return {};
    else {
        const std::array<Part, 3> rc{source("e", "in", "gnd"), resistor("r", "in", "out", rc_r),
                                     capacitor("c", "out", "gnd", rc_c)};
        const std::array<Part, 4> rlc{source("e", "in", "gnd"), resistor("r", "in", "x", rlc_r),
                                      inductor("l", "x", "out", rlc_l), capacitor("c", "out", "gnd", rlc_c)};
        Cell worst{0.0, 0.0};
        for (const double hz : {20.0, 2600.0, 3400.0}) {
            const Hertz f{hz};
            for (const Cell c : {ac_cell<S>(rc, f, rc_response(f, rc_r, rc_c)),
                                 ac_cell<S>(rlc, f, rlc_response(f, rlc_r, rlc_l, rlc_c))}) {
                worst.db = std::max(worst.db, std::abs(c.db));
                worst.deg = std::max(worst.deg, std::abs(c.deg));
            }
        }
        return worst;
    }
}

// Open the contact on a coil carrying 120 mA. `kick` is the peak |v| over the
// run as a multiple of L*I0/h; `ring` the peak |v(n1)| from tick be_ticks+1 to
// 600. Infinite unless the circuit was seen energised first.
struct SwitchVerdict { double kick = inf, ring = inf; };

template <class S>
SwitchVerdict switch_one(bool with_quench) {
    std::vector<Part> p{source("e", "vin", "gnd"), contact("k", "vin", "n1", contact_closed),
                        resistor("w", "n1", "n2", winding), inductor("l", "n2", "gnd", coil)};
    if (with_quench) p.push_back(resistor("q", "n1", "gnd", quench));
    S s{std::span<const Part>{p}};
    if (!right_rate(s)) return {};
    s.drive("e", battery.v);
    for (int n = 0; n < 1000; ++n) s.tick();
    const double before = static_cast<double>(s.voltage("n1"));
    if (!std::isfinite(before) || std::abs(before - battery.v) > 0.01 * battery.v) return {};
    s.set_contact("k", false);
    const double i0 = battery.v / winding.v;
    const double kick_v = coil.v * i0 / h().v;
    double peak = 0.0, ring = 0.0;
    for (int n = 1; n <= 600; ++n) {
        s.tick();
        const double v = static_cast<double>(s.voltage("n1"));
        if (!std::isfinite(v)) return {};
        peak = std::max(peak, std::abs(v));
        if (n > be_ticks) ring = std::max(ring, std::abs(v));
    }
    return {peak / kick_v, ring};
}

template <class S>
SwitchVerdict switch_open() {
    if constexpr (!Solver<S>) return {};
    else {
        const SwitchVerdict a = switch_one<S>(true), b = switch_one<S>(false);
        return {std::max(a.kick, b.kick), std::max(a.ring, b.ring)};
    }
}

// --------------------------------------------- a small honest solver, and bugs
//
// What the check is shown against before it is shown anything real: a dense
// modified-nodal solver with item 14's method (trapezoidal, backward Euler for
// `be` ticks after a contact changes) and GMIN, and one fake per way the
// check could be fooled. Nothing here is the office.

namespace fake {

struct Spec {
    bool backward_euler_only = false;  // numerical damping
    int  be = be_ticks;                // backward-Euler ticks after a contact change
    int  gmin_exp = -12;               // GMIN = 10^gmin_exp siemens (an int: clang drops a double member of a class NTTP)
    bool no_gmin = false;
    bool stale = false;                // a history one tick too old: the off-by-one
    bool late = false;                 // answers one tick late
    bool zero = false;                 // returns zeros
    bool nan = false;                  // returns NaN
    bool wrong_rate = false;           // ticks at 8 kHz
};

template <Spec P>
class Net {
    struct El {
        Kind kind;
        std::string_view name;
        std::size_t a, b;
        double value, drive = 0.0;
        bool closed = true;
        double v_old = 0.0, v_old2 = 0.0, i_old = 0.0, g = 0.0, J = 0.0;
        std::size_t branch = 0;
    };
public:
    explicit Net(std::span<const Part> parts) {
        for (const Part& p : parts) {
            El e{p.kind, p.name, node(p.a), node(p.b), p.value};
            if (p.kind == Kind::source) e.branch = branches_++;
            e_.push_back(e);
        }
        x_.assign(size(), 0.0);
        last_ = x_;
    }
    double tick_hz() const { return P.wrong_rate ? 8000.0 : 48000.0; }
    void drive(std::string_view n, double v) { for (El& e : e_) if (e.name == n) e.drive = v; }
    void set_contact(std::string_view n, bool closed) {
        for (El& e : e_)
            if (e.name == n && e.kind == Kind::contact && e.closed != closed) {
                e.closed = closed;
                be_left_ = P.be;
            }
    }
    double voltage(std::string_view n) const {
        if (P.nan) return std::numeric_limits<double>::quiet_NaN();
        if (P.zero) return 0.0;
        for (std::size_t i = 0; i < names_.size(); ++i)
            if (names_[i] == n) return i == 0 ? 0.0 : (P.late ? last_ : x_)[i - 1];
        return std::numeric_limits<double>::quiet_NaN();
    }
    void tick() {
        const bool be = P.backward_euler_only || be_left_ > 0;
        const double hh = h().v;
        const std::size_t nn = names_.size() - 1, n = size();
        std::vector<double> A(n * n, 0.0), rhs(n, 0.0);
        for (std::size_t k = 0; k < nn; ++k) A[k * n + k] += P.no_gmin ? 0.0 : std::pow(10.0, P.gmin_exp);
        auto at = [&](std::size_t r, std::size_t c) -> double& { return A[r * n + c]; };
        for (El& e : e_) {
            const double vo = P.stale ? e.v_old2 : e.v_old;
            switch (e.kind) {
                case Kind::resistor: e.g = 1.0 / e.value; e.J = 0.0; break;
                case Kind::contact:  e.g = e.closed ? 1.0 / e.value : 0.0; e.J = 0.0; break;
                case Kind::capacitor:
                    if (be) { e.g = e.value / hh; e.J = -e.g * vo; }
                    else    { e.g = 2.0 * e.value / hh; e.J = -(e.g * vo + e.i_old); }
                    break;
                case Kind::inductor:
                    if (be) { e.g = hh / e.value; e.J = e.i_old; }
                    else    { e.g = hh / (2.0 * e.value); e.J = e.i_old + e.g * vo; }
                    break;
                case Kind::source: {
                    const std::size_t row = nn + e.branch;
                    if (e.a) { at(e.a - 1, row) += 1.0; at(row, e.a - 1) += 1.0; }
                    if (e.b) { at(e.b - 1, row) -= 1.0; at(row, e.b - 1) -= 1.0; }
                    rhs[row] = e.drive;
                    continue;
                }
            }
            if (e.a) at(e.a - 1, e.a - 1) += e.g;
            if (e.b) at(e.b - 1, e.b - 1) += e.g;
            if (e.a && e.b) { at(e.a - 1, e.b - 1) -= e.g; at(e.b - 1, e.a - 1) -= e.g; }
            if (e.a) rhs[e.a - 1] -= e.J;
            if (e.b) rhs[e.b - 1] += e.J;
        }
        last_ = x_;
        if (!solve(A, rhs, n)) x_.assign(n, std::numeric_limits<double>::quiet_NaN());
        else x_ = rhs;
        for (El& e : e_) {
            if (e.kind == Kind::source) continue;
            const double vv = at_node(e.a) - at_node(e.b);
            e.v_old2 = e.v_old;
            e.v_old = vv;
            e.i_old = e.g * vv + e.J;
        }
        if (be_left_ > 0) --be_left_;
    }
private:
    std::size_t size() const { return names_.size() - 1 + branches_; }
    double at_node(std::size_t i) const { return i == 0 ? 0.0 : x_[i - 1]; }
    std::size_t node(std::string_view n) {
        for (std::size_t i = 0; i < names_.size(); ++i) if (names_[i] == n) return i;
        names_.push_back(n);
        return names_.size() - 1;
    }
    static bool solve(std::vector<double>& A, std::vector<double>& b, std::size_t n) {
        for (std::size_t k = 0; k < n; ++k) {
            std::size_t p = k;
            for (std::size_t r = k + 1; r < n; ++r) if (std::abs(A[r * n + k]) > std::abs(A[p * n + k])) p = r;
            if (A[p * n + k] == 0.0) return false;
            if (p != k) {
                for (std::size_t c = 0; c < n; ++c) std::swap(A[k * n + c], A[p * n + c]);
                std::swap(b[k], b[p]);
            }
            for (std::size_t r = k + 1; r < n; ++r) {
                const double f = A[r * n + k] / A[k * n + k];
                for (std::size_t c = k; c < n; ++c) A[r * n + c] -= f * A[k * n + c];
                b[r] -= f * b[k];
            }
        }
        for (std::size_t k = n; k-- > 0;) {
            for (std::size_t c = k + 1; c < n; ++c) b[k] -= A[k * n + c] * b[c];
            b[k] /= A[k * n + k];
        }
        return true;
    }
    std::vector<El> e_;
    std::vector<std::string_view> names_{"gnd"};
    std::vector<double> x_, last_;
    std::size_t branches_ = 0;
    int be_left_ = 0;
};

}  // namespace fake

// The ten claims, in one list, so the detector can ask any solver any of them.
enum class Claim { divider, gmin, rc_step, rc_tau, rl_step, rl_tau, ac_amplitude, ac_phase, kick, ring };

template <class S>
double measure(Claim c) {
    switch (c) {
        case Claim::divider:      return divider_error<S>();
        case Claim::gmin:         return gmin_over_bound<S>();
        case Claim::rc_step:      return rc_step<S>().ticks;
        case Claim::rc_tau:       return rc_step<S>().tau;
        case Claim::rl_step:      return rl_step<S>().ticks;
        case Claim::rl_tau:       return rl_step<S>().tau;
        case Claim::ac_amplitude: return ac_worst<S>().db;
        case Claim::ac_phase:     return ac_worst<S>().deg;
        case Claim::kick:         return switch_open<S>().kick;
        case Claim::ring:         return switch_open<S>().ring;
    }
    return inf;
}

constexpr double tolerance(Claim c) {
    switch (c) {
        case Claim::divider:      return bound_divider_volts;
        case Claim::gmin:         return 1.0;
        case Claim::rc_step:
        case Claim::rl_step:      return bound_step_ticks;
        case Claim::rc_tau:
        case Claim::rl_tau:       return bound_tau_fraction;
        case Claim::ac_amplitude: return bound_db;
        case Claim::ac_phase:     return bound_deg;
        case Claim::kick:         return bound_kick_ratio;
        case Claim::ring:         return bound_ring_volts;
    }
    return 0.0;
}

template <class S>
bool passes(Claim c) { return measure<S>(c) <= tolerance(c); }

constexpr unsigned bit(Claim c) { return 1u << static_cast<unsigned>(c); }
inline constexpr unsigned all_claims = (1u << 10) - 1;

// True if every claim in `must_fail` fails and every claim in `must_pass`
// passes, for the fake described by P.
template <fake::Spec P>
bool behaves(unsigned must_fail, unsigned must_pass) {
    using S = fake::Net<P>;
    for (unsigned i = 0; i < 10; ++i) {
        const auto c = static_cast<Claim>(i);
        if ((must_fail & bit(c)) && passes<S>(c)) return false;
        if ((must_pass & bit(c)) && !passes<S>(c)) return false;
    }
    return true;
}

// The oracles themselves: the ideal trapezoid is inside the step bound and a
// one-tick-late curve is outside it, for both circuits; the closed-form
// responses hit their known points (the RC is 3.01 dB down at its corner).
inline bool oracles_are_sane() {
    bool ok = true;
    for (const Seconds tau : {time_constant(rc_r, rc_c), time_constant(rl_l, rl_r)}) {
        double ideal = 0.0, late = 0.0;
        for (int n = 1; n <= ticks_in(tau * 8.0); ++n) {
            const Seconds t = h() * static_cast<double>(n);
            ideal = std::max(ideal, std::abs(ideal_trapezoid_step(n, tau) - exact_step(t, tau)) / (h() / tau));
            late = std::max(late, std::abs(exact_step(t - h(), tau) - exact_step(t, tau)) / (h() / tau));
        }
        ok = ok && ideal <= bound_step_ticks && late > bound_step_ticks;
    }
    const Hertz corner{1.0 / (2.0 * std::numbers::pi * time_constant(rc_r, rc_c).v)};
    ok = ok && std::abs(20.0 * std::log10(std::abs(rc_response(corner, rc_r, rc_c))) + 3.0103) < 1e-3;
    ok = ok && std::abs(rlc_response(Hertz{0.0}, rlc_r, rlc_l, rlc_c) - 1.0) < 1e-12;
    return ok;
}

// How many right verdicts the detector gave, out of `detector_cases`: the
// oracles; the honest fake passing every claim and landing on item 14's own
// cells; and each bug failing what it must.
inline constexpr int detector_cases = 19;

inline double detector_measure() {
    using fake::Spec;
    int right = int(oracles_are_sane());
    right += int(behaves<Spec{}>(0, all_claims));
    {
        const Cell c = ac_worst<fake::Net<Spec{}>>();  // item 14: 0.13 dB, 1.77 deg
        right += int(c.db > 0.10 && c.db < 0.16 && c.deg > 1.5 && c.deg < 2.0);
    }
    right += int(behaves<Spec{.backward_euler_only = true}>(bit(Claim::ac_amplitude) | bit(Claim::ac_phase), 0));
    right += int(behaves<Spec{.gmin_exp = -9}>(bit(Claim::gmin), 0));
    right += int(behaves<Spec{.no_gmin = true}>(bit(Claim::gmin), bit(Claim::divider)));
    right += int(behaves<Spec{.stale = true}>(bit(Claim::rc_step) | bit(Claim::rl_step), 0));
    right += int(behaves<Spec{.late = true}>(bit(Claim::rc_step) | bit(Claim::rl_step), 0));
    right += int(behaves<Spec{.be = 0}>(bit(Claim::kick) | bit(Claim::ring), bit(Claim::ac_amplitude)));
    right += int(behaves<Spec{.be = 1}>(bit(Claim::ring), 0));
    right += int(behaves<Spec{.be = 2}>(bit(Claim::ring), 0));
    right += int(behaves<Spec{.be = 4}>(bit(Claim::ring), 0));
    right += int(behaves<Spec{.zero = true}>(all_claims, 0));
    right += int(behaves<Spec{.nan = true}>(all_claims, 0));
    right += int(behaves<Spec{.wrong_rate = true}>(all_claims, 0));
    right += int(behaves<Spec{.backward_euler_only = true}>(0, bit(Claim::rc_step) | bit(Claim::divider)));
    right += int(!Solver<NotBuilt> && Solver<fake::Net<Spec{}>>);
    right += int(!passes<NotBuilt>(Claim::divider) && !passes<NotBuilt>(Claim::ac_amplitude) &&
                 !passes<NotBuilt>(Claim::ring) && !passes<NotBuilt>(Claim::gmin));
    right += int(!passes<fake::Net<Spec{.stale = true}>>(Claim::rc_tau));
    return static_cast<double>(right);
}

inline const bool detector_registered = verify::add({
    .name = "circuits.detector", .source = "selftest", .unit = "",
    .published = static_cast<double>(detector_cases), .tolerance = 0.0, .until_item = 0,
    .measure = detector_measure});

// ------------------------------------------------------- the registered checks
//
// Published figure 0 for an error, tolerance the bound; infinity (not built)
// cannot be within it. The markers say which item makes each pass.

inline double m_divider()   { return measure<Resistive>(Claim::divider); }
inline double m_gmin()      { return measure<Resistive>(Claim::gmin); }
inline double m_rc_step()   { return measure<Clocked>(Claim::rc_step); }
inline double m_rc_tau()    { return measure<Clocked>(Claim::rc_tau); }
inline double m_rl_step()   { return measure<Clocked>(Claim::rl_step); }
inline double m_rl_tau()    { return measure<Clocked>(Claim::rl_tau); }
inline double m_ac_db()     { return measure<Clocked>(Claim::ac_amplitude); }
inline double m_ac_deg()    { return measure<Clocked>(Claim::ac_phase); }
inline double m_kick()      { return measure<Clocked>(Claim::kick); }
inline double m_ring()      { return measure<Clocked>(Claim::ring); }

inline const bool circuits_registered =
    verify::add({.name = "circuits.divider", .source = "closed-form", .unit = "V", .published = 0.0,
                 .tolerance = bound_divider_volts, .until_item = 19, .measure = m_divider}) &&
    verify::add({.name = "circuits.gmin", .source = "closed-form", .unit = "x bound", .published = 0.0,
                 .tolerance = 1.0, .until_item = 19, .measure = m_gmin}) &&
    verify::add({.name = "circuits.rc_step", .source = "closed-form", .unit = "ticks late", .published = 0.0,
                 .tolerance = bound_step_ticks, .until_item = 20, .measure = m_rc_step}) &&
    verify::add({.name = "circuits.rc_tau", .source = "closed-form", .unit = "of tau", .published = 0.0,
                 .tolerance = bound_tau_fraction, .until_item = 20, .measure = m_rc_tau}) &&
    verify::add({.name = "circuits.rl_step", .source = "closed-form", .unit = "ticks late", .published = 0.0,
                 .tolerance = bound_step_ticks, .until_item = 20, .measure = m_rl_step}) &&
    verify::add({.name = "circuits.rl_tau", .source = "closed-form", .unit = "of tau", .published = 0.0,
                 .tolerance = bound_tau_fraction, .until_item = 20, .measure = m_rl_tau}) &&
    verify::add({.name = "circuits.ac_amplitude", .source = "closed-form", .unit = "dB", .published = 0.0,
                 .tolerance = bound_db, .until_item = 20, .measure = m_ac_db}) &&
    verify::add({.name = "circuits.ac_phase", .source = "closed-form", .unit = "deg", .published = 0.0,
                 .tolerance = bound_deg, .until_item = 20, .measure = m_ac_deg}) &&
    verify::add({.name = "circuits.switch_kick", .source = "closed-form", .unit = "x L*I0/h", .published = 0.0,
                 .tolerance = bound_kick_ratio, .until_item = 20, .measure = m_kick}) &&
    verify::add({.name = "circuits.switch_ring", .source = "closed-form", .unit = "V", .published = 0.0,
                 .tolerance = bound_ring_volts, .until_item = 20, .measure = m_ring});

}  // namespace laporte::checks::circuits
