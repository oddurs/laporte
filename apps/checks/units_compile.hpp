// units_compile.hpp — evidence that the dictionary refuses what it should.
//
// A unit system is claimed by what it rejects, and a rejection is not
// something a running program can observe: the program that would show it does
// not exist. So this file is all `static_assert`, and the assertion is that an
// expression is *ill-formed*, which C++ lets us ask with a requires-expression.
// If somebody makes `Volts` and `Amperes` addable, this header stops compiling
// and so does the build, which is a stronger failure than a printed line
// anyone could scroll past.
//
// It also asserts that every literal has an inverse in `as`, by round trip.
// Those are compile-time too, because the literals are `consteval`.
//
// Not covered: that the refusals are the right *shape* of error. A requires
// expression says "does not compile", not why.

#pragma once

#include <type_traits>

#include <laporte/units.hpp>

namespace laporte::checks::units {

template <class A, class B> concept Addable  = requires(A a, B b) { a + b; };
template <class A, class B> concept Subtract = requires(A a, B b) { a - b; };
template <class A, class B> concept Equal    = requires(A a, B b) { a == b; };
template <class A, class B> concept Multiply = requires(A a, B b) { a * b; };
template <class A, class B> concept Divide   = requires(A a, B b) { a / b; };
template <class A, class B> concept Assign   = requires(A& a, B b) { a = b; };

// `Volts{1} + Amperes{1}` does not compile.
static_assert(!Addable<Volts, Amperes>);
static_assert(!Subtract<Volts, Amperes>);
static_assert(!Equal<Volts, Amperes>);
static_assert(!Assign<Volts, Amperes>);
static_assert(!Addable<Ohms, Siemens>);
static_assert(!Addable<Seconds, Hertz>);
static_assert(!Addable<Volts, double>);          // a bare number is not a quantity
static_assert(!Assign<Volts, double>);
static_assert(!Multiply<Volts, Volts>);          // there are no Watts here, by accident or otherwise
static_assert(!Multiply<Volts, Amperes>);
static_assert(!Multiply<Ohms, Ohms>);
static_assert(!Divide<Amperes, Ohms>);           // current over resistance is nothing

// What does compile is Ohm's law, and nothing else.
static_assert(Addable<Volts, Volts>);
static_assert(std::is_same_v<decltype(Ohms{} * Amperes{}), Volts>);
static_assert(std::is_same_v<decltype(Volts{} / Ohms{}), Amperes>);
static_assert(std::is_same_v<decltype(Volts{} / Amperes{}), Ohms>);
static_assert(std::is_same_v<decltype(Siemens{} * Volts{}), Amperes>);
static_assert(std::is_same_v<decltype(Volts{} / Volts{}), double>);

static_assert(48.0_V / 1000.0_ohm == 0.048_A);
static_assert(std::is_same_v<decltype(10_ohm * 2_A), Volts>);

consteval bool near(double a, double b) {
    double d = a > b ? a - b : b - a;
    return d <= 1e-12 * (b < 0 ? -b : b);
}

// Every literal has an inverse in `as`.
static_assert(near(as::volts(48.0_V), 48.0));
static_assert(near(as::millivolts(250.0_mV), 250.0));
static_assert(near(as::kilovolts(1.5_kV), 1.5));
static_assert(near(as::amperes(2.0_A), 2.0));
static_assert(near(as::milliamperes(23.0_mA), 23.0));
static_assert(near(as::microamperes(7.0_uA), 7.0));
static_assert(near(as::ohms(400.0_ohm), 400.0));
static_assert(near(as::kilohms(1.3_kohm), 1.3));
static_assert(near(as::megohms(2.2_Mohm), 2.2));
static_assert(near(as::siemens(0.5_S), 0.5));
static_assert(near(as::millisiemens(3.0_mS), 3.0));
static_assert(near(as::microsiemens(9.0_uS), 9.0));
static_assert(near(as::farads(1.0_F), 1.0));
static_assert(near(as::microfarads(0.47_uF), 0.47));
static_assert(near(as::nanofarads(33.0_nF), 33.0));
static_assert(near(as::picofarads(10.0_pF), 10.0));
static_assert(near(as::henries(2.0_H), 2.0));
static_assert(near(as::millihenries(5.0_mH), 5.0));
static_assert(near(as::microhenries(8.0_uH), 8.0));
static_assert(near(as::seconds(3.0_s), 3.0));
static_assert(near(as::milliseconds(60.0_ms), 60.0));
static_assert(near(as::microseconds(40.0_us), 40.0));
static_assert(near(as::hertz(20.0_Hz), 20.0));
static_assert(near(as::kilohertz(2.6_kHz), 2.6));
static_assert(near(as::metres(5.0_m), 5.0));
static_assert(near(as::kilometres(4.0_km), 4.0));
static_assert(near(as::feet(100.0_ft), 100.0));
static_assert(near(as::kilofeet(12.0_kft), 12.0));
static_assert(near(as::pascals(101325.0_Pa), 101325.0));
static_assert(near(as::kilopascals(101.325_kPa), 101.325));

// The foot is a definition: a thousand of them are 304.8 metres.
static_assert(near(as::metres(1.0_kft), 304.8));
static_assert(near(as::hertz(frequency(100.0_ms)), 10.0));

}  // namespace laporte::checks::units
