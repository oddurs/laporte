// kirchhoff.hpp — the office is one netlist.
//
// There is one rule about this office that the rest of the project is built
// to protect: nothing in it knows anything except what is on its wire. A
// relay does not ask the battery what it is doing; it has a coil across two
// nodes and the voltage across that coil is its whole knowledge of the world.
// Off-hook, dialling, ringing, busy and the voice itself are states of
// voltage and current, so they have to be states of *one* system of
// equations. Solve the line and the relay in two places and hand a number
// from one to the other and you have a function call standing where a
// conductor should be, and the model has stopped being true. So this file is
// the wire: every part of the office is a few two-terminal elements between
// named nodes, and every voltage and current anywhere is read out of one
// solution of one matrix.
//
// The two laws that do it are Gustav Kirchhoff's, 1845: the currents into a
// node sum to zero, and the voltages around a loop sum to zero. Nothing else
// is in the solver. The formulation is modified nodal analysis, which is
// Ho, Ruehli and Brennan (1975): node voltages are the unknowns, and a
// voltage source adds one unknown, its own current, so a battery across two
// nodes needs no special treatment. A capacitor and an inductor are each
// turned, for one tick, into a conductance and a current source that carries
// the element's history (the "companion model"), so the matrix only ever
// holds resistors. Every node is tied to ground through a tiny conductance,
// GMIN, as Laurence Nagel's SPICE does, so that a node an open contact has
// left floating still has a voltage, and the matrix is never singular
// because of how the office happens to be switched.
//
// What is not modelled. Every element here is lumped, and the cost is real:
//
//   * A pair of copper wires several kilometres long is a transmission line,
//     with capacitance and inductance per metre. Here it is a resistor: no
//     line capacitance rounding the corners of dial pulses on a long loop, no
//     loading coils, no echo from an impedance mismatch.
//   * A contact is a perfect switch: open is not in the matrix at all, closed
//     is a small resistance. No bounce, no arc, no contact capacitance.
//   * A winding has no capacitance between its turns unless the part that
//     owns it adds a capacitor for it.
//   * Nothing nonlinear. The matrix is linear and changes only when a contact
//     does, which is what lets the factorisation be reused.
//
// The numerical decisions were made by two spikes (items 13 and 14) and are
// kept as they were: dense LU with partial pivoting, because the office is
// hundreds of nodes and not millions; a 48 kHz tick; the trapezoidal rule,
// which does not damp a 3.4 kHz tone the way backward Euler does. Backward
// Euler is here too, for the ticks after a contact opens an inductive circuit,
// where trapezoidal rings. *When* to use it is the clock's business (item 20);
// this file only does what it is told, one tick at a time.
//
// Determinism. A part is built from node names, and the matrix is stamped in
// canonical order: nodes are numbered by sorting their names, and elements are
// stamped sorted by name. The order the parts were handed over in therefore
// cannot reach the arithmetic.
//
// Apparatus never refers to apparatus (house rule 1). The netlist is the only
// door: it takes names and numbers, and its constructor requires that the two
// connections of a part convert to a string view. A part whose connection is a
// pointer, or a reference to another part, does not compile.

#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <laporte/units.hpp>

namespace laporte {

// Nagel's floor: every node leaks this much to ground, so a floating node has
// a voltage and a switched-off island does not make the matrix singular.
inline constexpr Siemens gmin{1e-12};

// The tick the clock spike chose. A tick is the only unit of time in here.
inline constexpr Hertz tick_rate{48000.0};

// The ground node's name. Every other name is a node like any other.
inline constexpr std::string_view ground_name = "gnd";

// The parts live in their own namespace so that a caller can have its own
// `resistor` without the units' namespace dragging this one in by argument lookup.
namespace kirchhoff {

enum class Kind { resistor, capacitor, inductor, source, contact };

// A two-terminal part, wired by names and nothing else. `value` is SI: ohms,
// farads, henries; for a contact its resistance when closed; unused for a
// source, whose voltage is set with `drive`, a to b.
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

// What the netlist will accept as a part: names for the connections, a number
// for the value, a kind in the order of `Kind`. A pointer to another part is
// not a name, so a part that carries one is refused here, at compile time.
template <class P>
concept Wired = requires(const P& p) {
    { p.name } -> std::convertible_to<std::string_view>;
    { p.a }    -> std::convertible_to<std::string_view>;
    { p.b }    -> std::convertible_to<std::string_view>;
    { p.value } -> std::convertible_to<double>;
    static_cast<int>(p.kind);
};

}  // namespace kirchhoff

using kirchhoff::Kind;
using kirchhoff::Wired;

enum class Method { backward_euler, trapezoidal };

class Netlist {
public:
    template <Wired P>
    explicit Netlist(std::span<const P> parts) {
        std::vector<std::string> names;
        for (const P& p : parts) {
            names.emplace_back(std::string_view{p.a});
            names.emplace_back(std::string_view{p.b});
        }
        std::sort(names.begin(), names.end());
        names.erase(std::unique(names.begin(), names.end()), names.end());
        std::erase(names, std::string{ground_name});
        nodes_ = std::move(names);

        for (const P& p : parts) {
            Element e;
            e.name = std::string{std::string_view{p.name}};
            e.kind = static_cast<Kind>(static_cast<int>(p.kind));
            e.a = node_index(std::string_view{p.a});
            e.b = node_index(std::string_view{p.b});
            e.value = static_cast<double>(p.value);
            elements_.push_back(std::move(e));
        }
        std::stable_sort(elements_.begin(), elements_.end(),
                         [](const Element& x, const Element& y) { return x.name < y.name; });

        std::size_t branches = 0;
        for (Element& e : elements_)
            if (e.kind == Kind::source) e.branch = branches++;
        n_ = nodes_.size() + branches;
        x_.assign(n_, 0.0);
    }

    template <Wired P>
    explicit Netlist(const std::vector<P>& parts) : Netlist(std::span<const P>{parts}) {}

    // The tick is 1/48000 s and is the only time there is.
    double tick_hz() const { return tick_rate.v; }
    Seconds tick_period() const { return period(tick_rate); }
    long ticks() const { return ticks_; }

    // A source's voltage a to b, held until it is set again.
    void drive(std::string_view name, Volts e) { drive(name, e.v); }
    void drive(std::string_view name, double volts) {
        for (Element& s : elements_)
            if (s.kind == Kind::source && s.name == name) { s.dc = volts; s.sine = false; }
    }
    // A sinusoid, evaluated at the tick count: amplitude * sin(2 pi f n h).
    void drive_sine(std::string_view name, Volts amplitude, Hertz f) {
        for (Element& s : elements_)
            if (s.kind == Kind::source && s.name == name) {
                s.dc = amplitude.v; s.omega = 2.0 * std::numbers::pi * f.v; s.sine = true;
            }
    }

    // A contact is closed when the netlist is built. Open is not stamped; closed is the contact's resistance. A change is the
    // only thing that makes the matrix different, so it is the only thing that
    // costs a factorisation.
    void set_contact(std::string_view name, bool closed) {
        for (Element& e : elements_)
            if (e.kind == Kind::contact && e.name == name && e.closed != closed) {
                e.closed = closed;
                valid_[0] = valid_[1] = false;
            }
    }

    // Solve with every element in its old state, then let every element take
    // what the solution measured. No element sees another's new state until
    // the next tick.
    void tick(Method m = Method::trapezoidal) {
        const int mi = m == Method::trapezoidal ? 1 : 0;
        const double h = period(tick_rate).v;
        ++ticks_;
        const double t = static_cast<double>(ticks_) * h;
        const std::size_t nn = nodes_.size();

        rhs_.assign(n_, 0.0);
        const bool build = !valid_[mi];
        if (build) {
            a_.assign(n_ * n_, 0.0);
            for (std::size_t k = 0; k < nn; ++k) a_[k * n_ + k] += gmin.v;
        }

        for (Element& e : elements_) {
            switch (e.kind) {
            case Kind::resistor: e.g = 1.0 / e.value; e.j = 0.0; break;
            case Kind::contact:  e.g = e.closed ? 1.0 / e.value : 0.0; e.j = 0.0; break;
            case Kind::capacitor:
                e.g = (m == Method::trapezoidal ? 2.0 : 1.0) * e.value / h;
                e.j = -(e.g * e.v_old + (m == Method::trapezoidal ? e.i_old : 0.0));
                break;
            case Kind::inductor:
                e.g = h / ((m == Method::trapezoidal ? 2.0 : 1.0) * e.value);
                e.j = e.i_old + (m == Method::trapezoidal ? e.g * e.v_old : 0.0);
                break;
            case Kind::source: {
                const std::size_t row = nn + e.branch;
                if (build) {
                    if (e.a != ground) { a_[e.a * n_ + row] += 1.0; a_[row * n_ + e.a] += 1.0; }
                    if (e.b != ground) { a_[e.b * n_ + row] -= 1.0; a_[row * n_ + e.b] -= 1.0; }
                }
                rhs_[row] = e.sine ? e.dc * std::sin(e.omega * t) : e.dc;
                continue;
            }
            }
            if (e.kind == Kind::contact && !e.closed) continue;
            if (build) stamp(e.a, e.b, e.g);
            if (e.a != ground) rhs_[e.a] -= e.j;
            if (e.b != ground) rhs_[e.b] += e.j;
        }

        if (build) { ok_[mi] = factor(mi); valid_[mi] = true; ++factorisations_; }
        if (ok_[mi]) { substitute(mi); x_ = rhs_; }
        else x_.assign(n_, std::numeric_limits<double>::quiet_NaN());

        for (Element& e : elements_) {
            const double v = at(e.a) - at(e.b);
            e.v_old = v;
            if (e.kind == Kind::source) continue;
            e.i_old = e.g * v + e.j;
        }
    }

    // What an engineer could measure: the potential of a node against ground,
    // NaN for a name nothing is wired to. `voltage` is the same as a plain
    // number, for the check that asks for one.
    Volts potential(std::string_view node) const { return Volts{voltage(node)}; }
    double voltage(std::string_view node) const {
        if (node == ground_name) return 0.0;
        const std::size_t i = node_index(node);
        return i == ground ? std::numeric_limits<double>::quiet_NaN() : x_[i];
    }

    // The current an element carries, a to b, after the last tick.
    Amperes current(std::string_view name) const {
        for (const Element& e : elements_)
            if (e.name == name)
                return Amperes{e.kind == Kind::source ? x_[nodes_.size() + e.branch] : e.i_old};
        return Amperes{std::numeric_limits<double>::quiet_NaN()};
    }

    // How often the matrix was factored: once per change of switching, not
    // once per tick.
    long factorisations() const { return factorisations_; }

private:
    static constexpr std::size_t ground = static_cast<std::size_t>(-1);

    struct Element {
        std::string name;
        Kind kind{};
        std::size_t a = ground, b = ground, branch = 0;
        double value = 0.0;
        bool closed = true;  // a contact starts closed; its owner opens it
        double dc = 0.0, omega = 0.0;
        bool sine = false;
        double g = 0.0, j = 0.0, v_old = 0.0, i_old = 0.0;
    };

    // Ground is never a row: its index is the sentinel, and it reads as zero.
    std::size_t node_index(std::string_view name) const {
        if (name == ground_name) return ground;
        const auto it = std::lower_bound(nodes_.begin(), nodes_.end(), name,
                                         [](const std::string& s, std::string_view n) { return s < n; });
        return it != nodes_.end() && *it == name ? static_cast<std::size_t>(it - nodes_.begin()) : ground;
    }

    double at(std::size_t node) const { return node == ground ? 0.0 : x_[node]; }

    void stamp(std::size_t a, std::size_t b, double g) {
        if (a != ground) a_[a * n_ + a] += g;
        if (b != ground) a_[b * n_ + b] += g;
        if (a != ground && b != ground) { a_[a * n_ + b] -= g; a_[b * n_ + a] -= g; }
    }

    // Dense LU with partial pivoting, in place in `a_`, kept (one per method)
    // so a tick whose matrix did not change is substitution alone.
    bool factor(int mi) {
        piv_.assign(n_, 0);
        for (std::size_t k = 0; k < n_; ++k) {
            std::size_t p = k;
            for (std::size_t r = k + 1; r < n_; ++r)
                if (std::abs(a_[r * n_ + k]) > std::abs(a_[p * n_ + k])) p = r;
            piv_[k] = p;
            if (a_[p * n_ + k] == 0.0) return false;
            if (p != k)
                for (std::size_t c = 0; c < n_; ++c) std::swap(a_[k * n_ + c], a_[p * n_ + c]);
            for (std::size_t r = k + 1; r < n_; ++r) {
                const double f = a_[r * n_ + k] / a_[k * n_ + k];
                a_[r * n_ + k] = f;
                if (f != 0.0)
                    for (std::size_t c = k + 1; c < n_; ++c) a_[r * n_ + c] -= f * a_[k * n_ + c];
            }
        }
        lu_[mi] = a_;
        pivots_[mi] = piv_;
        return true;
    }

    void substitute(int mi) {
        const std::vector<double>& lu = lu_[mi];
        const std::vector<std::size_t>& piv = pivots_[mi];
        for (std::size_t k = 0; k < n_; ++k) std::swap(rhs_[k], rhs_[piv[k]]);
        for (std::size_t r = 1; r < n_; ++r)
            for (std::size_t c = 0; c < r; ++c) rhs_[r] -= lu[r * n_ + c] * rhs_[c];
        for (std::size_t k = n_; k-- > 0;) {
            for (std::size_t c = k + 1; c < n_; ++c) rhs_[k] -= lu[k * n_ + c] * rhs_[c];
            rhs_[k] /= lu[k * n_ + k];
        }
    }

    std::vector<std::string> nodes_;  // sorted; the index is the matrix row
    std::vector<Element> elements_;   // sorted by name; the stamping order
    std::size_t n_ = 0;               // nodes plus source branches
    long ticks_ = 0, factorisations_ = 0;

    std::vector<double> x_, rhs_, a_;
    std::vector<std::size_t> piv_;
    std::vector<double> lu_[2];
    std::vector<std::size_t> pivots_[2];
    bool valid_[2] = {false, false};
    bool ok_[2] = {false, false};
};

}  // namespace laporte
