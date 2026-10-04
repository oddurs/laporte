// Spike 0014: one clock for everything. Throwaway.
//
//   c++ -std=c++23 -O2 -Iapps/spikes apps/spikes/clock.cpp -o /tmp/clock && /tmp/clock
//
// Part 1. Drive an RC and an RLC low-pass with sines at 20 Hz, 1 kHz, 2.6 kHz
// and 3.4 kHz, at several tick rates and with each integration method, and
// tabulate amplitude and phase error against the exact transfer function
// H(jw). The bound was written down before any of this was run:
//
//     0.5 dB and 5 degrees, every cell.
//
// Part 2. Open a contact in series with an inductor (a relay coil) and watch
// what each method does on the ticks after it.
//
// Circuit values are placeholders chosen to put a corner or a resonance inside
// the voice band, where the integrator is hardest to forgive. They are not
// ledger figures.
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdio>
#include <numbers>
#include <string>
#include <vector>

#include "mna.hpp"

using namespace mna;
using cplx = std::complex<double>;
constexpr double pi = std::numbers::pi;

constexpr double bound_db = 0.5, bound_deg = 5.0;

struct Circuit {
    const char* name;
    cplx (*exact)(double w);
    void (*build)(Net&, int in, int out);
};

// RC: 1 kohm and 159.155 nF, corner at 1 kHz.
constexpr double Rrc = 1000;
double Crc = 1 / (2 * pi * 1000 * Rrc);  // set per row by the corner sweep
cplx rc_exact(double w) { return 1.0 / cplx(1, w * Rrc * Crc); }
void rc_build(Net& n, int in, int out) { n.R(in, out, Rrc); n.C(out, 0, Crc); }

// RLC low-pass: 300 ohm source, 300 ohm characteristic impedance, resonant at
// 4 kHz, so Q = 1. The LC of the brief, with the loss any real one has.
constexpr double f0 = 4000, Z0 = 300, Rs = 300;
constexpr double Llc = Z0 / (2 * pi * f0), Clc = 1 / (2 * pi * f0 * Z0);
cplx lc_exact(double w) { return 1.0 / cplx(1 - w * w * Llc * Clc, w * Rs * Clc); }
void lc_build(Net& n, int in, int out) {
    const int mid = n.node();
    n.R(in, mid, Rs); n.L(mid, out, Llc); n.C(out, 0, Clc);
}

struct Result { double db_err, deg_err; };

// Drive for a second to settle, then correlate over a second that holds a whole
// number of cycles of every test frequency at every candidate rate.
Result run(const Circuit& c, double fs, Method m, double f) {
    Net n;
    const int in = n.node(), out = n.node();
    n.V(in, 0, [f](double t) { return std::sin(2 * pi * f * t); });
    c.build(n, in, out);
    n.prepare(1 / fs);
    const int settle = static_cast<int>(fs), win = static_cast<int>(fs);
    double si = 0, ci = 0, so = 0, co = 0;
    for (int k = 0; k < settle + win; ++k) {
        n.step(m);
        if (k >= settle) {
            const double w = 2 * pi * f * n.t, vi = n.v(in), vo = n.v(out);
            si += vi * std::sin(w); ci += vi * std::cos(w);
            so += vo * std::sin(w); co += vo * std::cos(w);
        }
    }
    const cplx gain = cplx(so, co) / cplx(si, ci);  // output phasor over input phasor
    const cplx ex = c.exact(2 * pi * f);
    double dph = (std::arg(gain) - std::arg(ex)) * 180 / pi;
    while (dph > 180) dph -= 360;
    while (dph < -180) dph += 360;
    return {20 * std::log10(std::abs(gain) / std::abs(ex)), dph};
}

int main() {
    const double freqs[] = {20, 1000, 2600, 3400};
    const double rates[] = {8000, 16000, 24000, 48000};
    struct M { const char* name; Method m; };
    const M methods[] = {{"BE", Method::BackwardEuler}, {"trap", Method::Trapezoidal}};
    const Circuit circuits[] = {{"RC (corner 1 kHz)", rc_exact, rc_build},
                                {"RLC (f0 4 kHz, Q 1)", lc_exact, lc_build}};

    std::printf("Pass bound (set before measuring): |amplitude error| <= %.1f dB and |phase error| <= %.1f deg, every cell.\n", bound_db, bound_deg);
    std::printf("Cells are amplitude error dB / phase error deg, simulated minus exact. * marks a cell outside the bound.\n");
    std::printf("In steady state no contact changes, so trap+BE-on-switch is the trap column.\n");

    struct Tally { double worst_db = 0, worst_deg = 0; };
    Tally tally[4][2];
    for (const auto& c : circuits) {
        std::printf("\n%s\n  %-8s", c.name, "f (Hz)");
        for (double fs : rates)
            for (const auto& m : methods) std::printf("  %6.0f %-4s     ", fs, m.name);
        std::printf("\n");
        for (double f : freqs) {
            std::printf("  %-8.0f", f);
            for (std::size_t r = 0; r < 4; ++r)
                for (std::size_t mi = 0; mi < 2; ++mi) {
                    const Result x = run(c, rates[r], methods[mi].m, f);
                    const bool bad = std::abs(x.db_err) > bound_db || std::abs(x.deg_err) > bound_deg;
                    std::printf("  %+7.2f/%+7.2f%s", x.db_err, x.deg_err, bad ? "*" : " ");
                    tally[r][mi].worst_db = std::max(tally[r][mi].worst_db, std::abs(x.db_err));
                    tally[r][mi].worst_deg = std::max(tally[r][mi].worst_deg, std::abs(x.deg_err));
                }
            std::printf("\n");
        }
    }
    std::printf("\nWorst cell over both circuits and all four frequencies:\n");
    for (std::size_t r = 0; r < 4; ++r)
        for (std::size_t mi = 0; mi < 2; ++mi) {
            const auto& t = tally[r][mi];
            std::printf("  %6.0f Hz %-4s  %6.2f dB  %6.2f deg   %s\n", rates[r], methods[mi].name, t.worst_db, t.worst_deg,
                        t.worst_db <= bound_db && t.worst_deg <= bound_deg ? "PASS" : "FAIL");
        }

    // The same table restricted to the frequencies a speech channel carries
    // as a service (no 3.4 kHz), in case only the top edge is the problem.
    std::printf("\nWorst cell with 3.4 kHz left out (20 Hz, 1 kHz, 2.6 kHz only):\n");
    for (std::size_t r = 0; r < 2; ++r)
        for (std::size_t mi = 0; mi < 2; ++mi) {
            double wdb = 0, wdeg = 0;
            for (const auto& c : circuits)
                for (double f : {20.0, 1000.0, 2600.0}) {
                    const Result x = run(c, rates[r], methods[mi].m, f);
                    wdb = std::max(wdb, std::abs(x.db_err)); wdeg = std::max(wdeg, std::abs(x.deg_err));
                }
            std::printf("  %6.0f Hz %-4s  %6.2f dB  %6.2f deg   %s\n", rates[r], methods[mi].name, wdb, wdeg,
                        wdb <= bound_db && wdeg <= bound_deg ? "PASS" : "FAIL");
        }

    // Rates between 24 and 48 kHz that are whole multiples of 8 kHz, so a
    // trunk could still decimate by an integer.
    std::printf("\nIntermediate multiples of 8 kHz, trapezoidal, worst cell over both circuits and all four frequencies:\n");
    for (double fs : {32000.0, 40000.0}) {
        double wdb = 0, wdeg = 0;
        for (const auto& c : circuits)
            for (double f : freqs) {
                const Result x = run(c, fs, Method::Trapezoidal, f);
                wdb = std::max(wdb, std::abs(x.db_err)); wdeg = std::max(wdeg, std::abs(x.deg_err));
            }
        std::printf("  %6.0f Hz trap  %6.2f dB  %6.2f deg   %s\n", fs, wdb, wdeg,
                    wdb <= bound_db && wdeg <= bound_deg ? "PASS" : "FAIL");
    }

    // Where the corner is does not matter: trapezoidal's error is a warp of the
    // frequency axis, w -> (2/h) tan(wh/2), whatever the circuit. Sweep the RC
    // corner from 30 Hz to 10 kHz and see.
    std::printf("\nRC corner sweep, amplitude dB / phase deg error at 2.6 kHz and 3.4 kHz (* outside the bound):\n");
    std::printf("  %-10s %-9s", "corner Hz", "f (Hz)");
    const double sweep_rates[] = {8000, 16000, 32000, 48000};
    for (double fs : sweep_rates) std::printf("  %5.0f trap     ", fs);
    std::printf("  8000 BE\n");
    for (double corner : {30.0, 100.0, 300.0, 1000.0, 3000.0, 10000.0}) {
        Crc = 1 / (2 * pi * corner * Rrc);
        for (double f : {2600.0, 3400.0}) {
            std::printf("  %-10.0f %-9.0f", corner, f);
            auto cell = [&](double fs, Method m) {
                const Result x = run(circuits[0], fs, m, f);
                const bool bad = std::abs(x.db_err) > bound_db || std::abs(x.deg_err) > bound_deg;
                std::printf("  %+7.2f/%+7.2f%s", x.db_err, x.deg_err, bad ? "*" : " ");
            };
            for (double fs : sweep_rates) cell(fs, Method::Trapezoidal);
            cell(8000, Method::BackwardEuler);
            std::printf("\n");
        }
    }
    Crc = 1 / (2 * pi * 1000 * Rrc);

    // ---- Part 2: a contact opens in series with an inductor. ----
    std::printf("\n== Switched inductive contact ==\n");
    std::printf("48 V -> contact -> n1 -> [400 ohm winding + 0.2 H coil] -> earth, with a quench/leakage resistor Rq from n1 to earth.\n");
    std::printf("The contact opens at t = 0. Exact: v(n1) = -I0*Rq*exp(-t/tau), I0 = 120 mA, tau = L/(Rw+Rq).\n");
    std::printf("v1 = first tick after opening. ring = peak |v| over ticks 6..60 (exact is ~0 there, except Rq=1k where tau is about one tick). tail = peak |v| over ticks 500..600: has it decayed or does it persist. flips = sign changes in ticks 1..600.\n\n");
    std::printf("  %-8s %-22s %12s %12s %12s %12s %6s\n", "Rq", "method", "v1 (V)", "exact v1 (V)", "ring (V)", "tail (V)", "flips");
    const double L = 0.2, Rw = 400;
    struct Case { const char* name; double rq; };
    const Case cases[] = {{"1k", 1e3}, {"10k", 1e4}, {"100k", 1e5}, {"GMIN", 1e12}};
    struct Mode { const char* name; int be_ticks; bool all_be; };
    const Mode modes[] = {{"backward Euler", 0, true}, {"trapezoidal", 0, false},
                          {"trap + BE 1 tick", 1, false}, {"trap + BE 2 ticks", 2, false},
                          {"trap + BE 4 ticks", 4, false}, {"trap + BE 8 ticks", 8, false}};
    for (const double fs : {8000.0, 48000.0})
    for (const auto& cs : cases) {
        const double h = 1 / fs;
        if (cs.rq == 1e3) std::printf("  -- tick rate %.0f Hz --\n", fs);
        for (const auto& md : modes) {
            Net n;
            const int bat = n.node(), n1 = n.node(), n2 = n.node();
            n.battery(bat, 0, 48);
            const int contact = n.R(bat, n1, 0.1);
            n.R(n1, n2, Rw);
            n.L(n2, 0, L);
            n.R(n1, 0, cs.rq);
            n.prepare(h);
            for (int k = 0; k < static_cast<int>(fs / 10); ++k) n.step(Method::Trapezoidal);  // 0.1 s: DC
            const double i_coil = n.e[2].i;  // current through the winding, the thing that must continue
            n.set_resistance(contact, 1e12);
            std::vector<double> v;
            for (int k = 0; k < 600; ++k) {
                const Method m = md.all_be || k < md.be_ticks ? Method::BackwardEuler : Method::Trapezoidal;
                n.step(m);
                v.push_back(n.v(n1));
            }
            const double tau = L / (Rw + cs.rq);
            const double exact1 = -i_coil * cs.rq * std::exp(-h / tau);
            double ring = 0, tail = 0; int flips = 0;
            for (std::size_t k = 5; k < 60; ++k) ring = std::max(ring, std::abs(v[k]));
            for (std::size_t k = 499; k < v.size(); ++k) tail = std::max(tail, std::abs(v[k]));
            for (std::size_t k = 1; k < v.size(); ++k) if (v[k] * v[k - 1] < 0) ++flips;
            std::printf("  %-8s %-22s %12.4g %12.4g %12.4g %12.4g %6d\n", cs.name, md.name, v[0], exact1, ring, tail, flips);
        }
        std::printf("\n");
    }
    return 0;
}
