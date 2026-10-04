// Throwaway prototype for spikes 0013 and 0014. Not part of the office, not
// built by `make`, not to be included from include/. If the spike is ratified
// the real thing is rewritten as kirchhoff.hpp, in the house voice.
//
// A netlist of two-terminal elements, one matrix, modified nodal analysis.
// Capacitors and inductors are companion models: a conductance and a current
// source carrying history, so the matrix only ever holds resistors.
//
//   i(a->b) = g*v + J,  v = V(a) - V(b)
//
//   capacitor, backward Euler : g = C/h      J = -g*v_old
//   capacitor, trapezoidal    : g = 2C/h     J = -(g*v_old + i_old)
//   inductor,  backward Euler : g = h/L      J = i_old
//   inductor,  trapezoidal    : g = h/(2L)   J = i_old + g*v_old
#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

namespace mna {

enum class Method { BackwardEuler, Trapezoidal };
enum class Kind { R, C, L, V };

struct Elem {
    Kind kind;
    int a, b;                            // node numbers, 0 is ground
    double value;                        // ohms, farads, henries
    std::function<double(double)> wave;  // V only: volts as a function of time
    int branch = -1;                     // V only: index of its current unknown
    double v_old = 0, i_old = 0, g = 0, J = 0;
    double i = 0;                        // current a->b after the last solve
};

// Dense LU with partial pivoting, kept so that a matrix which did not change
// can be solved again by substitution alone.
struct Lu {
    std::size_t n = 0;
    std::vector<double> a;
    std::vector<std::size_t> piv;
    void factor(std::vector<double> m, std::size_t n_) {
        n = n_;
        a = std::move(m);
        piv.assign(n, 0);
        for (std::size_t k = 0; k < n; ++k) {
            std::size_t p = k;
            for (std::size_t r = k + 1; r < n; ++r)
                if (std::abs(a[r * n + k]) > std::abs(a[p * n + k])) p = r;
            piv[k] = p;
            if (p != k)
                for (std::size_t c = 0; c < n; ++c) std::swap(a[k * n + c], a[p * n + c]);
            const double d = a[k * n + k];
            for (std::size_t r = k + 1; r < n; ++r) {
                const double f = a[r * n + k] / d;
                a[r * n + k] = f;
                if (f != 0.0)
                    for (std::size_t c = k + 1; c < n; ++c) a[r * n + c] -= f * a[k * n + c];
            }
        }
    }
    void solve(std::vector<double>& x) const {
        for (std::size_t k = 0; k < n; ++k) std::swap(x[k], x[piv[k]]);
        for (std::size_t r = 1; r < n; ++r)
            for (std::size_t c = 0; c < r; ++c) x[r] -= a[r * n + c] * x[c];
        for (std::size_t k = n; k-- > 0;) {
            for (std::size_t c = k + 1; c < n; ++c) x[k] -= a[k * n + c] * x[c];
            x[k] /= a[k * n + k];
        }
    }
};

inline constexpr double gmin = 1e-12;  // SPICE's floor: a floating node still has a voltage

class Net {
public:
    int nodes = 1;  // node 0 is ground
    std::vector<Elem> e;
    double h = 0, t = 0;
    std::vector<double> x;  // node voltages 1.., then branch currents
    Lu lu[2];               // one factorisation per method, both kept
    bool valid[2] = {false, false};
    long factorisations = 0;
    std::vector<double> work;  // scratch, so a tick allocates nothing it can keep

    int node() { return nodes++; }
    int add(Kind k, int a, int b, double val, std::function<double(double)> w = {}) {
        Elem el;
        el.kind = k; el.a = a; el.b = b; el.value = val; el.wave = std::move(w);
        e.push_back(std::move(el));
        valid[0] = valid[1] = false;
        return static_cast<int>(e.size()) - 1;
    }
    int R(int a, int b, double r) { return add(Kind::R, a, b, r); }
    int C(int a, int b, double c) { return add(Kind::C, a, b, c); }
    int L(int a, int b, double l) { return add(Kind::L, a, b, l); }
    int V(int a, int b, std::function<double(double)> w) { return add(Kind::V, a, b, 0, std::move(w)); }
    int battery(int a, int b, double volts) { return V(a, b, [volts](double) { return volts; }); }
    // A contact is a resistance that changes, so the matrix must be refactored.
    void set_resistance(int idx, double r) {
        e[static_cast<std::size_t>(idx)].value = r;
        valid[0] = valid[1] = false;
    }

    std::size_t size() const {
        std::size_t nb = 0;
        for (const auto& el : e) if (el.kind == Kind::V) ++nb;
        return static_cast<std::size_t>(nodes - 1) + nb;
    }
    double v(int n) const { return n == 0 ? 0.0 : x[static_cast<std::size_t>(n - 1)]; }

    void prepare(double step) {
        h = step;
        t = 0;
        std::size_t nb = 0;
        for (auto& el : e) if (el.kind == Kind::V) el.branch = static_cast<int>(nb++);
        x.assign(size(), 0.0);
        valid[0] = valid[1] = false;
    }

    // One tick: solve with every element in its old state, then let every
    // element take what it measured. `refactor` forces a fresh factorisation,
    // as a matrix that changes every tick (a carbon transmitter) would.
    void step(Method m, bool refactor = false) {
        const int mi = m == Method::Trapezoidal ? 1 : 0;
        const std::size_t n = size();
        const std::size_t nn = static_cast<std::size_t>(nodes - 1);
        t += h;
        std::vector<double>& rhs = work;
        rhs.assign(n, 0.0);
        const bool build = refactor || !valid[mi];
        std::vector<double> A;
        if (build) {
            A.assign(n * n, 0.0);
            for (std::size_t k = 0; k < nn; ++k) A[k * n + k] += gmin;
        }
        auto ix = [](int nd) { return static_cast<std::size_t>(nd - 1); };
        auto stamp_g = [&](int a, int b, double g) {
            if (a) A[ix(a) * n + ix(a)] += g;
            if (b) A[ix(b) * n + ix(b)] += g;
            if (a && b) { A[ix(a) * n + ix(b)] -= g; A[ix(b) * n + ix(a)] -= g; }
        };
        for (auto& el : e) {
            switch (el.kind) {
            case Kind::R:
                el.g = el.value > 0 ? 1.0 / el.value : 0.0;
                el.J = 0;
                break;
            case Kind::C:
                if (m == Method::Trapezoidal) { el.g = 2 * el.value / h; el.J = -(el.g * el.v_old + el.i_old); }
                else { el.g = el.value / h; el.J = -el.g * el.v_old; }
                break;
            case Kind::L:
                if (m == Method::Trapezoidal) { el.g = h / (2 * el.value); el.J = el.i_old + el.g * el.v_old; }
                else { el.g = h / el.value; el.J = el.i_old; }
                break;
            case Kind::V: {
                const std::size_t row = nn + static_cast<std::size_t>(el.branch);
                if (build) {
                    if (el.a) { A[ix(el.a) * n + row] += 1; A[row * n + ix(el.a)] += 1; }
                    if (el.b) { A[ix(el.b) * n + row] -= 1; A[row * n + ix(el.b)] -= 1; }
                }
                rhs[row] = el.wave(t);
                continue;
            }
            }
            if (build) stamp_g(el.a, el.b, el.g);
            if (el.a) rhs[ix(el.a)] -= el.J;  // the history source pushes current a->b
            if (el.b) rhs[ix(el.b)] += el.J;
        }
        if (build) { lu[mi].factor(std::move(A), n); valid[mi] = true; ++factorisations; }
        lu[mi].solve(rhs);
        x.swap(rhs);
        for (auto& el : e) {
            const double vv = v(el.a) - v(el.b);
            el.v_old = vv;
            if (el.kind == Kind::V) { el.i = x[nn + static_cast<std::size_t>(el.branch)]; continue; }
            el.i = el.g * vv + el.J;
            el.i_old = el.i;
        }
    }
};

}  // namespace mna
