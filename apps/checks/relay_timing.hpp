// relay_timing.hpp — a relay lets go when its numbers say, and not before.
//
// A relay is the office's whole computer, and its two jobs are not the ones
// people remember. It is a switch with memory: it pulls in when the current in
// its coil reaches the operate value, and drops out only when the current
// falls to a lower release value, so that a current hovering near either
// threshold does not make it buzz. And it is slow, by an amount that follows
// from the coil's own inductance and resistance, plus (in a slow-release
// relay) a copper slug that holds the flux up after the coil lets go. That
// slowness is the only way a step-by-step office remembers you are still
// dialling.
//
// This check is arguing with two plausible relays. One has operate and release
// currents equal, so noise on the threshold toggles it every tick (chatter).
// The other is fast because somebody typed its timing instead of letting the
// coil's L/R produce it. The timing has a closed form, and this is it.
//
//   operate:  coil on a step V, I(t) = I_f (1 - e^(-t/tau)), tau = L/R,
//             I_f = V/R, so the contacts close at  t = -tau ln(1 - I_op/I_f).
//   release:  coil current already settled at I0 and the voltage removed (the
//             coil closing through its own resistance), I(t) = I0 e^(-t/tau),
//             so the contacts open at              t =  tau ln(I0/I_rel).
//   slug:     a copper slug is a shorted turn on the core: a second circuit,
//             L2 and R2, tau_slug = L2/R2, coupled to the coil (L1, R1) by M.
//             With the source removed and the coil still closed through it
//             (which is how this check drives release):
//                 L1 i1' + M i2' = -R1 i1      M i1' + L2 i2' = -R2 i2.
//             Perfect coupling is M^2 = L1 L2, so with n = M/L1 the flux
//             linkages are l1 = L1 (i1 + n i2) and l2 = n l1, and the
//             equations say l1' = -R1 i1 and n l1' = -R2 i2, giving
//             i2 = n (R1/R2) i1 and l1 = L1 i1 (1 + n^2 R1/R2). Then
//             l1' = -R1 i1 = -l1 / (L1/R1 + n^2 L1/R2) = -l1 / (tau + tau_slug),
//             because n^2 L1 = L2. The flux decays with tau + tau_slug, and
//             falls to the release value (flux is proportional to the
//             ampere-turns the settled coil carried, I0) at
//                 t = (tau + tau_slug) ln(I0/I_rel).
//             This is the CLOSED-coil result and is the one the drive uses:
//             the zero-volt source leaves the coil circuit closed.
//             The OTHER case is a coil opened at release (i1 = 0): then only
//             the slug circuit is left, l2 = L2 i2, and the flux decays with
//             tau_slug alone, t = tau_slug ln(I0/I_rel). It is not what is
//             driven, and the self-test shows that a relay doing it fails.
//             Both are the perfect-coupling limit; a real slug is coupled
//             less than perfectly. An apparatus that models imperfect
//             coupling notes it on item 25 and the inspector decides; it
//             does not edit this.
//
// WHAT THE NUMBERS ARE. The operate and release currents of a real line relay
// are UNSOURCED (ledger S11: "not found"; S07's 7 to 16 mA is a threshold
// range across office types, not a relay's operate current). So this check
// invents none and judges none. The coil it drives is SYNTHETIC, named as such
// in `synthetic_coil`, chosen for slow, easily-resolved timings and for
// currents a factor of two apart; nothing here says a 1965 line relay looks
// like it, and the source printed beside each result is "unsourced (S11)" so
// that nobody reads a pass as a published agreement. What is judged is the
// relay against *its own numbers* by the closed form, which needs no ledger.
//
// THE TICK CONVENTION. `closed[k]` is read after tick k, at (k + 1) tick, so a
// contact that closes during tick k is seen at the end of it. Measured, with
// the self-test's fakes: an exact-exponential relay scores 0.5 to 0.8 ticks of
// quantisation; a backward-Euler companion model that also acts one tick
// after it measures (house rule 4) scores 2.3 to 2.5. The tolerance is 3
// ticks: it ALLOWS the companion model's own lag and one tick of
// act-after-measure latency, and nothing more. At the 48 kHz clock (item 14) a
// tick is 21 us and tau is 1920 ticks, so a coil whose L/R is wrong by 0.5 %
// is 34 ticks out, a relay 4 ticks late scores 4.8, and a typed-in timing is
// further. All of those fail; the self-test asserts it, at 48 kHz and 1 kHz.
//
// HOOK-UP, for the author of item 25 (the check itself may not be edited; this
// is the whole change, and it is the body of `drive_relay` at the bottom):
//
//     #include <laporte/relay.hpp>  // and whatever builds a one-relay office
//     ...
//     build an office of a source (value from `volts_at(t)`) in series with the
//     coil `c` (R, L, i_operate, i_release, slug); for each tick, set the source
//     from volts_at(k * tick), tick the office, and push_back whether the
//     relay's contacts are closed; return {tick period, those states}.
//
// `closed[k]` is the state after tick `k`, i.e. at time (k + 1) * tick. The
// number of states must cover `duration`. Anything the hook-up has to change
// besides that one function is a sign the interface drifted.
//
// What is not modelled:
//
//   * Contact bounce, armature travel, saturation and residual magnetism
//     (item 25 does not model them either), so the 3-tick tolerance leaves no room
//     for a half-millisecond of armature.
//   * Real noise. The chatter probe is a deterministic square dither of 1 %
//     about each threshold, in blocks long enough for the coil current to
//     follow. A relay that chatters only under random noise, or under a dither
//     faster than L/R (which a coil filters away), is not caught here.
//   * The slug beyond its perfect-coupling limit (see above), and the slug's
//     other effect: in a real relay it also delays operate, and only release
//     is judged here.

#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <functional>
#include <limits>
#include <vector>

#include <laporte/units.hpp>

#include "verify.hpp"

namespace laporte::checks::relay_timing {

inline constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

// A relay's coil, as a specification. SYNTHETIC: see the header.
struct Coil {
    Ohms     r;
    Henries  l;
    Amperes  i_operate;
    Amperes  i_release;
    Seconds  slug;       // 0 for an ordinary relay
};

inline Coil synthetic_coil(Seconds slug) {
    return Coil{.r = Ohms{400.0}, .l = Henries{16.0},          // tau = 40 ms
                .i_operate = Amperes{0.012}, .i_release = Amperes{0.006}, .slug = slug};
}

// What the apparatus answers: its tick, and the contacts after each tick.
struct Response {
    Seconds tick;
    std::vector<bool> closed;
};

using Schedule = std::function<Volts(Seconds)>;

// The one thing the check asks of a relay: drive the coil with `volts_at(t)`
// for `duration` and say what the contacts did.
template <class D>
concept Drive = requires(D d, const Coil& c, Seconds s, const Schedule& v) {
    { d(c, s, v) } -> std::convertible_to<Response>;
};

namespace detail {

inline bool usable(const Response& r) {
    return std::isfinite(r.tick.v) && r.tick.v > 0.0 && !r.closed.empty();
}

inline double tau_of(const Coil& c) { return c.l.v / c.r.v; }

// Time, from the start of tick 0, of the first tick whose state is `want`,
// counted from index `from`; NaN if there is none.
inline double first_state(const Response& r, bool want, std::size_t from) {
    for (std::size_t k = from; k < r.closed.size(); ++k)
        if (r.closed[k] == want) return static_cast<double>(k + 1) * r.tick.v;
    return not_a_number;
}

inline std::size_t transitions(const Response& r) {
    std::size_t n = 0;
    for (std::size_t k = 1; k < r.closed.size(); ++k) n += r.closed[k] != r.closed[k - 1];
    return n + (r.closed.front() ? 1u : 0u);  // starting closed is a transition from rest
}

}  // namespace detail

// Worst timing error, in ticks, over operate, release, and slow release.
// NaN if the relay does not do each thing at all, or answers nonsense.
template <Drive D>
double timing_error_ticks(D&& drive) {
    using namespace detail;
    const Coil plain = synthetic_coil(Seconds{0.0});
    const Coil slow  = synthetic_coil(Seconds{0.100});
    const double tau = tau_of(plain);
    const double v_final = 2.0 * plain.i_operate.v * plain.r.v;   // I_f = 2 I_op
    const double i_f = v_final / plain.r.v;

    // Operate, from rest. This call also tells us the tick.
    const Response up = drive(plain, Seconds{12.0 * tau},
                              [&](Seconds) { return Volts{v_final}; });
    if (!usable(up)) return not_a_number;
    const double tick = up.tick.v;
    const double t_op = first_state(up, true, 0);
    if (!std::isfinite(t_op)) return not_a_number;
    const double want_op = -tau * std::log(1.0 - plain.i_operate.v / i_f);
    double worst = std::abs(t_op - want_op) / tick;

    // Release, plain and slugged: on until settled, then off, aligned to a tick.
    for (const Coil* c : {&plain, &slow}) {
        // Settled means fifteen of the slowest time constant, which is the
        // coupled one (tau + tau_slug) when there is a slug.
        const double t_on = std::ceil(15.0 * (tau + c->slug.v) / tick) * tick;
        const double i0 = i_f * (1.0 - std::exp(-t_on / tau));
        const Response down = drive(*c, Seconds{t_on + 15.0 * (tau + c->slug.v)},
                                    [&](Seconds t) { return Volts{t.v < t_on ? v_final : 0.0}; });
        if (!usable(down)) return not_a_number;
        const std::size_t from = static_cast<std::size_t>(std::llround(t_on / tick));
        const double t_off = first_state(down, false, from);
        if (!std::isfinite(t_off)) return not_a_number;
        const double want = (tau + c->slug.v) * std::log(i0 / c->i_release.v);
        worst = std::max(worst, std::abs((t_off - t_on) - want) / tick);
    }
    return worst;
}

// Extra transitions, summed over two dithers, beyond the one each should make.
//   A: from rest, dither 1 % about the operate current. The relay must operate
//      once and stay, because 1 % below operate is far above release.
//   B: operated, dither 1 % about the release current, starting below. It must
//      release once and stay out, because 1 % above release is far below operate.
// A relay with no hysteresis makes one transition per block in both. A relay
// that never operates, or never releases, makes none, and is also wrong.
template <Drive D>
double chatter_count(D&& drive) {
    using namespace detail;
    const Coil c = synthetic_coil(Seconds{0.0});
    const double tau = tau_of(c);
    const double v_op = c.i_operate.v * c.r.v, v_rel = c.i_release.v * c.r.v;
    const double eps = 0.01;

    const Response probe = drive(c, Seconds{tau}, [&](Seconds) { return Volts{2.0 * v_op}; });
    if (!usable(probe)) return not_a_number;
    const double tick = probe.tick.v;
    const double block = std::ceil(10.0 * tau / tick) * tick;
    const int blocks = 8;
    const auto block_of = [&](Seconds t) { return static_cast<int>(std::floor((t.v + 0.5 * tick) / block)); };

    const Response a = drive(c, Seconds{blocks * block}, [&](Seconds t) {
        return Volts{block_of(t) % 2 == 0 ? v_op * (1.0 + eps) : v_op * (1.0 - eps)}; });
    if (!usable(a)) return not_a_number;

    const double t_on = std::ceil(15.0 * tau / tick) * tick;
    const Response b = drive(c, Seconds{t_on + blocks * block}, [&](Seconds t) {
        if (t.v < t_on) return Volts{2.0 * v_op};
        return Volts{block_of(Seconds{t.v - t_on}) % 2 == 0 ? v_rel * (1.0 - eps) : v_rel * (1.0 + eps)}; });
    if (!usable(b)) return not_a_number;

    // B starts operated, so its transition count is releases and re-operations
    // after the first operate, which is one transition from rest.
    const double na = static_cast<double>(transitions(a));
    const double nb = static_cast<double>(transitions(b)) - 1.0;
    return std::abs(na - 1.0) + std::abs(nb - 1.0);
}

// ── The self-test: fakes of the check's own, labelled as such ─────────────
//
// A fake relay is a coil stepped exactly (or by backward Euler, the way a
// companion model would), with hysteresis, and a slug that sets the release
// decay to tau + tau_slug (closed coil, perfect coupling), or, for the broken
// one, to tau_slug alone (an opened coil). It is what a correct
// apparatus would do. The broken ones are the failure modes the item names.

struct Fake {
    bool coupled = false;         // slug as two coupled circuits, integrated, not the closed form
    bool open_coil = false;       // slug released as if the coil were opened: tau_slug alone
    bool euler = false;           // backward-Euler companion model, not the exact exponential
    double tau_scale = 1.0;       // L/R wrong by this factor
    bool no_hysteresis = false;   // release at the operate current: chatters
    bool dead = false;            // never operates
    int  late_ticks = 0;          // contacts follow, but late
    double tick = 0.001;
};

inline Response fake_drive(const Fake& f, const Coil& c, Seconds duration, const Schedule& v) {
    const double tau = c.l.v / c.r.v;
    const double tau_dn = f.open_coil && c.slug.v > 0.0 ? c.slug.v : tau + c.slug.v;
    const auto decay = [&](double t) {
        const double x = f.tick / (t * f.tau_scale);
        return f.euler ? 1.0 / (1.0 + x) : std::exp(-x);
    };
    const double decay_up = decay(tau), decay_dn = decay(tau_dn);
    const double i_rel = f.no_hysteresis ? c.i_operate.v : c.i_release.v;
    Response out{Seconds{f.tick}, {}};
    std::vector<bool> raw;
    double i = 0.0;
    bool closed = false;
    const auto n = static_cast<std::size_t>(std::ceil(duration.v / f.tick));
    // The coupled fake: L1 = coil, L2 = slug with R2 = 1 ohm, M^2 = L1 L2,
    // backward Euler in twenty substeps a tick. The contact follows the
    // ampere-turns i1 + n i2, which is what the core flux is made of.
    const bool phys = f.coupled && c.slug.v > 0.0;
    const double l1 = c.l.v, r1 = c.r.v, l2 = c.slug.v, r2 = 1.0, m = std::sqrt(l1 * l2);
    double i1 = 0.0, i2 = 0.0;
    for (std::size_t k = 0; k < n; ++k) {
        if (phys) {
            const double volts = v(Seconds{static_cast<double>(k) * f.tick}).v, h = f.tick / 20.0;
            const double a = l1 + h * r1, d = l2 + h * r2, det = a * d - m * m;
            for (int s = 0; s < 20; ++s) {
                const double b1 = l1 * i1 + m * i2 + h * volts, b2 = m * i1 + l2 * i2;
                i1 = (d * b1 - m * b2) / det;
                i2 = (a * b2 - m * b1) / det;
            }
            i = i1 + (m / l1) * i2;
            if (!f.dead && !closed && i >= c.i_operate.v) closed = true;
            else if (closed && i <= i_rel) closed = false;
            raw.push_back(closed);
            continue;
        }
        const double target = v(Seconds{static_cast<double>(k) * f.tick}).v / c.r.v;
        i = target + (i - target) * (target < i ? decay_dn : decay_up);
        if (!f.dead && !closed && i >= c.i_operate.v) closed = true;
        else if (closed && i <= i_rel) closed = false;
        raw.push_back(closed);
    }
    for (std::size_t k = 0; k < n; ++k) {
        const std::size_t src = k >= static_cast<std::size_t>(f.late_ticks)
                                    ? k - static_cast<std::size_t>(f.late_ticks) : n;
        out.closed.push_back(src < n && raw[src]);
    }
    return out;
}

// The stub's contract in miniature: an answer that cannot be mistaken for
// success, which is why the check on it is unpassable by default.
inline Response drive_relay(const Coil&, Seconds, const Schedule&) {
    return Response{Seconds{not_a_number}, {}};  // not built yet: drive the real relay here
}

inline double timing_measure()  { return timing_error_ticks(drive_relay); }
inline double chatter_measure() { return chatter_count(drive_relay); }

// 1 if every fake is told apart correctly, else 0.
inline double selftest_measure() {
    const auto as_drive = [](Fake f) {
        return [f](const Coil& c, Seconds d, const Schedule& v) { return fake_drive(f, c, d, v); };
    };
    bool ok = true;
    for (const double tick : {1.0 / 48000.0, 0.001}) {
        const Fake good{.tick = tick};
        ok = ok && timing_error_ticks(as_drive(good)) < 1.0;
        ok = ok && timing_error_ticks(as_drive(Fake{.euler = true, .late_ticks = 1, .tick = tick})) < 3.0;
        ok = ok && timing_error_ticks(as_drive(Fake{.tau_scale = 1.005, .tick = tick})) > (tick < 0.0005 ? 3.0 : 0.0);
        ok = ok && chatter_count(as_drive(good)) == 0.0;
        // A physically coupled slug, integrated from the two circuits, stays
        // green; one that releases as an opened coil would does not.
        ok = ok && timing_error_ticks(as_drive(Fake{.coupled = true, .tick = tick})) < 3.0;
        ok = ok && timing_error_ticks(as_drive(Fake{.open_coil = true, .tick = tick})) > 3.0;
        ok = ok && chatter_count(as_drive(Fake{.no_hysteresis = true, .tick = tick})) > 0.0;
        ok = ok && std::isnan(timing_error_ticks(as_drive(Fake{.dead = true, .tick = tick})));
        ok = ok && timing_error_ticks(as_drive(Fake{.late_ticks = 4, .tick = tick})) > 3.0;
    }
    return ok ? 1.0 : 0.0;
}

inline const bool registered_selftest = verify::add({
    .name = "relay.selftest", .source = "selftest", .unit = "",
    .published = 1.0, .tolerance = 0.0, .until_item = 0, .measure = selftest_measure});
inline const bool registered_timing = verify::add({
    .name = "relay.timing", .source = "unsourced (S11)", .unit = "ticks",
    .published = 0.0, .tolerance = 3.0, .until_item = 25, .measure = timing_measure});
inline const bool registered_chatter = verify::add({
    .name = "relay.no_chatter", .source = "unsourced (S11)", .unit = "extra transitions",
    .published = 0.0, .tolerance = 0.0, .until_item = 25, .measure = chatter_measure});

}  // namespace laporte::checks::relay_timing
