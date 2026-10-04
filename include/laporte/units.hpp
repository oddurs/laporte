// units.hpp — the dictionary of what this program will accept.
//
// This is the first file written and the only one allowed to be a list. Every
// other file in the office touches it, and it touches nothing.
//
// It is arguing with the `double`. A telephone exchange is a place where a
// voltage, a current and a resistance are all "a number of the thing", where a
// cable is quoted in ohms per thousand feet by the people who built it and in
// ohms per kilometre by the people who check it, and where the likeliest
// arithmetic mistake is to add a volt to an ampere because both were declared
// `double` and both were, at that moment, close to 0.02. The language allows
// that. This file is how it stops.
//
// So `Volts` and `Amperes` are different types, and the only operator that
// turns one into the other is Ohm's law: a resistance times a current is a
// voltage, a voltage over a resistance is a current, and there is no spelling
// of either that is not one of those. Georg Ohm's relation is the only thing
// connecting the two quantities; the type system is merely agreeing with him.
// Everything the office will later derive — a loop limit, a relay's operate
// point, a ring trip — is arithmetic on these types, and the compiler is the
// first reader of every equation.
//
// Inside the office everything is SI: volts, amperes, ohms, siemens, farads,
// henries, seconds, hertz, metres, pascals. Other units exist at the two
// surfaces of the program — what a person types in and what is printed back —
// and the surfaces are `consteval` literals going in (`48.0_V`, `20.0_Hz`,
// `0.47_uF`) and named functions in `namespace as` coming out
// (`as::kilofeet(length)`). There is nothing in between. Bell measured cable
// in thousands of feet and quoted a loop limit in ohms; reproducing their
// figure means writing their unit down once, here, and converting at the edge.
//
// Most of the literals are never used. That is deliberate: this file is not a
// call graph, it is a statement of what the program will and will not accept,
// and an unused `consteval` generates nothing at all. Every literal has an
// inverse in `as`, and `apps/checks/units_compile.hpp` asserts it by round
// trip, to a relative 1e-12. That is the claim: the inverse recovers the
// value to far better than any figure this project quotes, not that it is
// bit-exact. There is no macro generating the literals, because there is no
// `#define` in this project; the cost is that the list is long and plain.
//
// What is not modelled:
//
//   * Dimensional analysis in general. There is no `Quantity<Exponents...>`
//     and no `Watts`: a power is a voltage times a current and nothing in the
//     office needs to carry one around. A type is added when a part needs it,
//     with an argument, not before.
//   * Decibels. They are a way of printing a ratio, not a unit, and live
//     with the instruments.
//   * Temperature, which enters (as copper's resistivity) only when a part
//     derives it.
//   * Integers. Every quantity is a double; a tick count is a tick count and
//     is not here.
//
// The one non-SI conversion factor is the foot, and it is a definition, not a
// measurement: the international foot is exactly 0.3048 metres, so there is no
// uncertainty to carry and the digits are the whole number. It is written once
// (`metres_per_foot`) and the thousand-foot unit Bell used is derived from it.
// The 1959 yard-and-pound agreement is where that definition is usually
// attributed; no source for it has been fetched into `docs/sources.md`, so
// treat the attribution as unsourced and the value as the definition it is.
//
// Negative powers of ten are spelled as division (`v / 1e6`, never
// `v * 1e-6`): every power of ten up to 1e22 is exactly representable and IEEE
// division is correctly rounded, so dividing lands on the same double the
// decimal literal would, where `1e-6` is not representable and multiplying by
// it rounds twice. (Found, with a bit pattern, in cornell's `si.hpp`; also
// unsourced here, because it is a note about a sibling, not a figure.)

#pragma once

#include <compare>

namespace laporte {

// The international foot, in metres. Defined, not measured.
inline constexpr double metres_per_foot = 0.3048;

// One template, one tag per dimension, and no conversion between tags.
// Construction is explicit so that a bare `double` never silently becomes a
// quantity; the literals below are the way in.
template <class Tag>
struct Quantity {
    double v{};

    constexpr Quantity() = default;
    constexpr explicit Quantity(double x) : v{x} {}

    friend constexpr bool operator==(Quantity, Quantity) = default;
    friend constexpr auto operator<=>(Quantity, Quantity) = default;

    constexpr Quantity operator-() const { return Quantity{-v}; }
    friend constexpr Quantity operator+(Quantity a, Quantity b) { return Quantity{a.v + b.v}; }
    friend constexpr Quantity operator-(Quantity a, Quantity b) { return Quantity{a.v - b.v}; }
    constexpr Quantity& operator+=(Quantity o) { v += o.v; return *this; }
    constexpr Quantity& operator-=(Quantity o) { v -= o.v; return *this; }

    // Scaling by a pure number keeps the dimension.
    friend constexpr Quantity operator*(Quantity a, double k) { return Quantity{a.v * k}; }
    friend constexpr Quantity operator*(double k, Quantity a) { return Quantity{k * a.v}; }
    friend constexpr Quantity operator/(Quantity a, double k) { return Quantity{a.v / k}; }

    // A ratio of two like quantities is a number.
    friend constexpr double operator/(Quantity a, Quantity b) { return a.v / b.v; }
};

struct VoltTag {};   struct AmpereTag {};  struct OhmTag {};    struct SiemensTag {};
struct FaradTag {};  struct HenryTag {};   struct SecondTag {}; struct HertzTag {};
struct MetreTag {};  struct PascalTag {};

using Volts    = Quantity<VoltTag>;
using Amperes  = Quantity<AmpereTag>;
using Ohms     = Quantity<OhmTag>;
using Siemens  = Quantity<SiemensTag>;
using Farads   = Quantity<FaradTag>;
using Henries  = Quantity<HenryTag>;
using Seconds  = Quantity<SecondTag>;
using Hertz    = Quantity<HertzTag>;
using Metres   = Quantity<MetreTag>;
using Pascals  = Quantity<PascalTag>;

// ── Ohm's law, and the only door between Volts and Amperes ────────────────
//
// Conductance is the reciprocal of resistance and is a separate type because
// a relay winding is quoted in ohms and a line's leakage in siemens, and the
// reciprocal is where one is mistaken for the other.

constexpr Volts   operator*(Ohms r, Amperes i)    { return Volts{r.v * i.v}; }
constexpr Volts   operator*(Amperes i, Ohms r)    { return Volts{i.v * r.v}; }
constexpr Amperes operator/(Volts e, Ohms r)      { return Amperes{e.v / r.v}; }
constexpr Ohms    operator/(Volts e, Amperes i)   { return Ohms{e.v / i.v}; }

constexpr Amperes operator*(Siemens g, Volts e)   { return Amperes{g.v * e.v}; }
constexpr Amperes operator*(Volts e, Siemens g)   { return Amperes{e.v * g.v}; }
constexpr Siemens operator/(Amperes i, Volts e)   { return Siemens{i.v / e.v}; }
constexpr Volts   operator/(Amperes i, Siemens g) { return Volts{i.v / g.v}; }

constexpr Siemens conductance(Ohms r)    { return Siemens{1.0 / r.v}; }
constexpr Ohms    resistance(Siemens g)  { return Ohms{1.0 / g.v}; }

// A period and a frequency are each other's reciprocal; nothing else here is
// allowed to be inverted into a different type.
constexpr Hertz   frequency(Seconds t)   { return Hertz{1.0 / t.v}; }
constexpr Seconds period(Hertz f)        { return Seconds{1.0 / f.v}; }

// ── Literals: the surface where a specification enters ────────────────────
//
//      auto battery = 48.0_V;
//      auto relay   = 400.0_ohm;
//      auto cap     = 0.47_uF;
//      auto pulses  = 10.0_Hz;
//
// Each is offered for a floating spelling and an integer one (`48_V`).

inline namespace literals {

consteval Volts   operator""_V   (long double x)        { return Volts{double(x)}; }
consteval Volts   operator""_V   (unsigned long long x) { return Volts{double(x)}; }
consteval Volts   operator""_mV  (long double x)        { return Volts{double(x) / 1e3}; }
consteval Volts   operator""_mV  (unsigned long long x) { return Volts{double(x) / 1e3}; }
consteval Volts   operator""_kV  (long double x)        { return Volts{double(x) * 1e3}; }
consteval Volts   operator""_kV  (unsigned long long x) { return Volts{double(x) * 1e3}; }

consteval Amperes operator""_A   (long double x)        { return Amperes{double(x)}; }
consteval Amperes operator""_A   (unsigned long long x) { return Amperes{double(x)}; }
consteval Amperes operator""_mA  (long double x)        { return Amperes{double(x) / 1e3}; }
consteval Amperes operator""_mA  (unsigned long long x) { return Amperes{double(x) / 1e3}; }
consteval Amperes operator""_uA  (long double x)        { return Amperes{double(x) / 1e6}; }
consteval Amperes operator""_uA  (unsigned long long x) { return Amperes{double(x) / 1e6}; }

consteval Ohms    operator""_ohm (long double x)        { return Ohms{double(x)}; }
consteval Ohms    operator""_ohm (unsigned long long x) { return Ohms{double(x)}; }
consteval Ohms    operator""_kohm(long double x)        { return Ohms{double(x) * 1e3}; }
consteval Ohms    operator""_kohm(unsigned long long x) { return Ohms{double(x) * 1e3}; }
consteval Ohms    operator""_Mohm(long double x)        { return Ohms{double(x) * 1e6}; }
consteval Ohms    operator""_Mohm(unsigned long long x) { return Ohms{double(x) * 1e6}; }

consteval Siemens operator""_S   (long double x)        { return Siemens{double(x)}; }
consteval Siemens operator""_S   (unsigned long long x) { return Siemens{double(x)}; }
consteval Siemens operator""_mS  (long double x)        { return Siemens{double(x) / 1e3}; }
consteval Siemens operator""_mS  (unsigned long long x) { return Siemens{double(x) / 1e3}; }
consteval Siemens operator""_uS  (long double x)        { return Siemens{double(x) / 1e6}; }
consteval Siemens operator""_uS  (unsigned long long x) { return Siemens{double(x) / 1e6}; }

consteval Farads  operator""_F   (long double x)        { return Farads{double(x)}; }
consteval Farads  operator""_F   (unsigned long long x) { return Farads{double(x)}; }
consteval Farads  operator""_uF  (long double x)        { return Farads{double(x) / 1e6}; }
consteval Farads  operator""_uF  (unsigned long long x) { return Farads{double(x) / 1e6}; }
consteval Farads  operator""_nF  (long double x)        { return Farads{double(x) / 1e9}; }
consteval Farads  operator""_nF  (unsigned long long x) { return Farads{double(x) / 1e9}; }
consteval Farads  operator""_pF  (long double x)        { return Farads{double(x) / 1e12}; }
consteval Farads  operator""_pF  (unsigned long long x) { return Farads{double(x) / 1e12}; }

consteval Henries operator""_H   (long double x)        { return Henries{double(x)}; }
consteval Henries operator""_H   (unsigned long long x) { return Henries{double(x)}; }
consteval Henries operator""_mH  (long double x)        { return Henries{double(x) / 1e3}; }
consteval Henries operator""_mH  (unsigned long long x) { return Henries{double(x) / 1e3}; }
consteval Henries operator""_uH  (long double x)        { return Henries{double(x) / 1e6}; }
consteval Henries operator""_uH  (unsigned long long x) { return Henries{double(x) / 1e6}; }

consteval Seconds operator""_s   (long double x)        { return Seconds{double(x)}; }
consteval Seconds operator""_s   (unsigned long long x) { return Seconds{double(x)}; }
consteval Seconds operator""_ms  (long double x)        { return Seconds{double(x) / 1e3}; }
consteval Seconds operator""_ms  (unsigned long long x) { return Seconds{double(x) / 1e3}; }
consteval Seconds operator""_us  (long double x)        { return Seconds{double(x) / 1e6}; }
consteval Seconds operator""_us  (unsigned long long x) { return Seconds{double(x) / 1e6}; }

consteval Hertz   operator""_Hz  (long double x)        { return Hertz{double(x)}; }
consteval Hertz   operator""_Hz  (unsigned long long x) { return Hertz{double(x)}; }
consteval Hertz   operator""_kHz (long double x)        { return Hertz{double(x) * 1e3}; }
consteval Hertz   operator""_kHz (unsigned long long x) { return Hertz{double(x) * 1e3}; }

consteval Metres  operator""_m   (long double x)        { return Metres{double(x)}; }
consteval Metres  operator""_m   (unsigned long long x) { return Metres{double(x)}; }
consteval Metres  operator""_km  (long double x)        { return Metres{double(x) * 1e3}; }
consteval Metres  operator""_km  (unsigned long long x) { return Metres{double(x) * 1e3}; }
consteval Metres  operator""_ft  (long double x)        { return Metres{double(x) * metres_per_foot}; }
consteval Metres  operator""_ft  (unsigned long long x) { return Metres{double(x) * metres_per_foot}; }
consteval Metres  operator""_kft (long double x)        { return Metres{double(x) * 1e3 * metres_per_foot}; }
consteval Metres  operator""_kft (unsigned long long x) { return Metres{double(x) * 1e3 * metres_per_foot}; }

consteval Pascals operator""_Pa  (long double x)        { return Pascals{double(x)}; }
consteval Pascals operator""_Pa  (unsigned long long x) { return Pascals{double(x)}; }
consteval Pascals operator""_kPa (long double x)        { return Pascals{double(x) * 1e3}; }
consteval Pascals operator""_kPa (unsigned long long x) { return Pascals{double(x) * 1e3}; }

}  // namespace literals

// ── as: the surface where a quantity leaves ───────────────────────────────
//
// One inverse per literal, named for what it prints. Each undoes its literal's
// scaling (a division where the literal multiplies and the reverse).

namespace as {

constexpr double volts(Volts q)             { return q.v; }
constexpr double millivolts(Volts q)        { return q.v * 1e3; }
constexpr double kilovolts(Volts q)         { return q.v / 1e3; }

constexpr double amperes(Amperes q)         { return q.v; }
constexpr double milliamperes(Amperes q)    { return q.v * 1e3; }
constexpr double microamperes(Amperes q)    { return q.v * 1e6; }

constexpr double ohms(Ohms q)               { return q.v; }
constexpr double kilohms(Ohms q)            { return q.v / 1e3; }
constexpr double megohms(Ohms q)            { return q.v / 1e6; }

constexpr double siemens(Siemens q)         { return q.v; }
constexpr double millisiemens(Siemens q)    { return q.v * 1e3; }
constexpr double microsiemens(Siemens q)    { return q.v * 1e6; }

constexpr double farads(Farads q)           { return q.v; }
constexpr double microfarads(Farads q)      { return q.v * 1e6; }
constexpr double nanofarads(Farads q)       { return q.v * 1e9; }
constexpr double picofarads(Farads q)       { return q.v * 1e12; }

constexpr double henries(Henries q)         { return q.v; }
constexpr double millihenries(Henries q)    { return q.v * 1e3; }
constexpr double microhenries(Henries q)    { return q.v * 1e6; }

constexpr double seconds(Seconds q)         { return q.v; }
constexpr double milliseconds(Seconds q)    { return q.v * 1e3; }
constexpr double microseconds(Seconds q)    { return q.v * 1e6; }

constexpr double hertz(Hertz q)             { return q.v; }
constexpr double kilohertz(Hertz q)         { return q.v / 1e3; }

constexpr double metres(Metres q)           { return q.v; }
constexpr double kilometres(Metres q)       { return q.v / 1e3; }
constexpr double feet(Metres q)             { return q.v / metres_per_foot; }
constexpr double kilofeet(Metres q)         { return q.v / metres_per_foot / 1e3; }

constexpr double pascals(Pascals q)         { return q.v; }
constexpr double kilopascals(Pascals q)     { return q.v / 1e3; }

}  // namespace as

}  // namespace laporte
