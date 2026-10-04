// Spike 0013: how the office is solved. Throwaway.
//
//   c++ -std=c++23 -O2 -Iapps/spikes apps/spikes/solve.cpp -o /tmp/solve && /tmp/solve
//
// Builds the v0.1 loop and the two-loop AC-coupled transmission bridge as one
// netlist, solves it by modified nodal analysis with dense LU, and says how
// fast, relative to real time, at 8000 Hz.
//
// The element values are placeholders of the right order of magnitude (a relay
// of a few hundred ohms and a fraction of a henry, 150 ohms of copper a side, a
// set that looks like 300 ohms). They are not ledger figures; nothing here is
// a claim about the real office, only about the solver.
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <numbers>
#include <vector>

#include "mna.hpp"

using namespace mna;

namespace {

constexpr double pi = std::numbers::pi;

struct Sub {
    int tip_office, ring_office, phone_tip, phone_mid;
    int phone_r;       // element index of the set's DC resistance
    int coil_tip_l, coil_ring_l;
};

// One subscriber's loop: battery -> coil (L, then winding R) -> copper -> set
// -> copper -> coil -> earth. The set is 300 ohms in series with its
// transmitter, which is a voltage source here because the carbon is not the
// subject of this spike.
Sub add_subscriber(Net& n, int bat, double talk_volts, double talk_hz) {
    Sub s{};
    const int n1 = n.node(), n2 = n.node(), n3 = n.node(), n4 = n.node(),
              n5 = n.node(), n6 = n.node(), n7 = n.node();
    s.coil_tip_l = n.L(bat, n1, 0.2);
    n.R(n1, n2, 200);
    n.R(n2, n3, 150);
    s.phone_r = n.R(n3, n4, 300);
    n.V(n4, n5, [=](double t) { return talk_volts * std::sin(2 * pi * talk_hz * t); });
    n.R(n5, n6, 150);
    n.R(n6, n7, 200);
    s.coil_ring_l = n.L(n7, 0, 0.2);
    s.tip_office = n2;
    s.ring_office = n6;
    s.phone_tip = n3;
    s.phone_mid = n4;
    return s;
}

void bridge(Net& n, const Sub& a, const Sub& b) {
    n.C(a.tip_office, b.tip_office, 2e-6);
    n.C(a.ring_office, b.ring_office, 2e-6);
}

struct Phasor { double amp, phase; };
Phasor measure(const std::vector<double>& y, double fs, double f) {
    double s = 0, c = 0;
    for (std::size_t k = 0; k < y.size(); ++k) {
        const double w = 2 * pi * f * static_cast<double>(k) / fs;
        s += y[k] * std::sin(w);
        c += y[k] * std::cos(w);
    }
    const double nrm = 2.0 / static_cast<double>(y.size());
    return {nrm * std::hypot(s, c), std::atan2(c, s)};
}

double seconds_since(std::chrono::steady_clock::time_point t0) {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
}

}  // namespace

int main(int argc, char** argv) {
    // argv[1]: tick rate for section 3 (default 8000)
    const double fs3 = argc > 1 ? std::atof(argv[1]) : 8000.0;
    std::printf("== 1. the v0.1 loop alone: battery, line relay, copper, set ==\n");
    {
        Net n;
        const int bat = n.node();
        n.battery(bat, 0, 48.0);
        const Sub s = add_subscriber(n, bat, 0.0, 1000);
        const double fs = 8000;
        n.prepare(1 / fs);
        // loop resistance by hand: 200+150+300+150+200 ohms; two 0.2 H coils
        const double r_loop = 1000.0, i_hand = 48.0 / r_loop, tau = 0.4 / r_loop;
        double t63 = -1;
        for (int k = 0; k < 4000; ++k) {
            n.step(Method::Trapezoidal);
            const double i = n.e[static_cast<std::size_t>(s.phone_r)].i;
            if (t63 < 0 && i >= 0.632 * i_hand) t63 = n.t;
        }
        const double i_end = n.e[static_cast<std::size_t>(s.phone_r)].i;
        std::printf("  matrix %zu x %zu\n", n.size(), n.size());
        std::printf("  loop current after 0.5 s: %.6f mA   by hand (48 V / %.0f ohm): %.6f mA\n",
                    i_end * 1e3, r_loop, i_hand * 1e3);
        std::printf("  time to 63.2%% of final: %.3f ms   by hand (L/R = %.3f ms; tick is 0.125 ms)\n",
                    t63 * 1e3, tau * 1e3);
    }

    std::printf("\n== 2. two loops AC-coupled through 2 uF a side (transmission bridge) ==\n");
    {
        const double fs_list[] = {8000, 48000};
        for (Method m : {Method::Trapezoidal, Method::BackwardEuler}) {
            for (double fs : fs_list) {
                Net n;
                const int bat = n.node();
                n.battery(bat, 0, 48.0);
                const Sub a = add_subscriber(n, bat, 1.0, 1000);
                const Sub b = add_subscriber(n, bat, 0.0, 1000);
                bridge(n, a, b);
                n.prepare(1 / fs);
                const int settle = static_cast<int>(fs * 0.2), win = static_cast<int>(fs * 0.25);
                std::vector<double> y;
                for (int k = 0; k < settle + win; ++k) {
                    n.step(m);
                    if (k >= settle) y.push_back(n.v(b.phone_tip) - n.v(b.phone_mid));
                }
                const Phasor p = measure(y, fs, 1000);
                std::printf("  %-6s %5.0f Hz: DC loop B %.3f mA, A talks 1 V @1 kHz, B hears %.4f V peak (%.2f dB)\n",
                            m == Method::Trapezoidal ? "trap" : "BE", fs,
                            n.e[static_cast<std::size_t>(b.phone_r)].i * 1e3, p.amp, 20 * std::log10(p.amp));
            }
        }
    }

    std::printf("\n== 3. twelve subscribers, six calls, one netlist, %.0f Hz, trapezoidal ==\n", fs3);
    {
        const double fs = fs3;
        auto build = [&](Net& n) {
            const int bat = n.node();
            n.battery(bat, 0, 48.0);
            std::vector<Sub> subs;
            for (int k = 0; k < 12; ++k)
                subs.push_back(add_subscriber(n, bat, k % 2 == 0 ? 1.0 : 0.0, 800.0 + 50.0 * k));
            for (int k = 0; k < 12; k += 2) bridge(n, subs[static_cast<std::size_t>(k)], subs[static_cast<std::size_t>(k + 1)]);
            n.prepare(1 / fs);
            return subs;
        };
        const double sim_seconds = fs3 > 8000 ? 1.0 : 4.0;
        const int ticks = static_cast<int>(sim_seconds * fs);

        struct Mode { const char* name; int refactor_every; };
        const Mode modes[] = {
            {"A. refactor every tick (matrix changes every tick)", 1},
            {"B. refactor every 80th tick (a contact somewhere, ~10 pulses/s * 8)", 80},
            {"C. factor once, back-substitute (matrix unchanged)", 0},
        };
        for (const auto& mode : modes) {
            Net n;
            const auto subs = build(n);
            const auto t0 = std::chrono::steady_clock::now();
            for (int k = 0; k < ticks; ++k)
                n.step(Method::Trapezoidal, mode.refactor_every && k % mode.refactor_every == 0);
            const double wall = seconds_since(t0);
            std::printf("  %s\n    matrix %zu x %zu, %ld factorisations, %.3f s wall for %.1f s of office: %.1fx real time (%.2f us/tick)\n",
                        mode.name, n.size(), n.size(), n.factorisations, wall, sim_seconds,
                        sim_seconds / wall, wall / ticks * 1e6);
            if (&mode == &modes[2])
                std::printf("    sanity: sub 0 loop current %.3f mA\n",
                            n.e[static_cast<std::size_t>(subs[0].phone_r)].i * 1e3);
        }

        {
            // D. The same twelve subscribers solved as six independent
            // two-subscriber netlists. With an ideal battery nothing couples one
            // call to the next, so this is what solving each connected component
            // of the one netlist separately would cost; it is measured here with
            // six Nets only because the prototype has no component finder.
            std::vector<Net> parts(6);
            for (auto& n : parts) {
                const int bat = n.node();
                n.battery(bat, 0, 48.0);
                const Sub a = add_subscriber(n, bat, 1.0, 800.0);
                const Sub b = add_subscriber(n, bat, 0.0, 800.0);
                bridge(n, a, b);
                n.prepare(1 / fs);
            }
            const auto t0 = std::chrono::steady_clock::now();
            for (int k = 0; k < ticks; ++k)
                for (auto& n : parts) n.step(Method::Trapezoidal, true);
            const double wall = seconds_since(t0);
            std::printf("  D. six 26-unknown components, refactor every tick\n    %.3f s wall for %.1f s of office: %.1fx real time (%.2f us/tick)\n",
                        wall, sim_seconds, sim_seconds / wall, wall / ticks * 1e6);
        }

        // The cost that matters is the worst honest case, so also scale the
        // office: the lesson of the dense matrix is its cube.
        std::printf("\n  scaling, mode A (refactor every tick) and mode C (cached), 1 s of office:\n");
        for (int nsub : {6, 12, 24, 48, 96}) {
            double rate[2];
            std::size_t sz = 0;
            for (int mode = 0; mode < 2; ++mode) {
                Net n;
                const int bat = n.node();
                n.battery(bat, 0, 48.0);
                std::vector<Sub> subs;
                for (int k = 0; k < nsub; ++k)
                    subs.push_back(add_subscriber(n, bat, k % 2 == 0 ? 1.0 : 0.0, 800.0 + 10.0 * k));
                for (int k = 0; k + 1 < nsub; k += 2) bridge(n, subs[static_cast<std::size_t>(k)], subs[static_cast<std::size_t>(k + 1)]);
                n.prepare(1 / fs);
                sz = n.size();
                const int tk = mode == 0 && nsub >= 48 ? 800 : 8000;
                const auto t0 = std::chrono::steady_clock::now();
                for (int k = 0; k < tk; ++k) n.step(Method::Trapezoidal, mode == 0);
                rate[mode] = (tk / fs) / seconds_since(t0);
            }
            std::printf("    %3d subscribers, %4zu unknowns: A %8.1fx   C %8.1fx\n", nsub, sz, rate[0], rate[1]);
        }
    }
    return 0;
}
