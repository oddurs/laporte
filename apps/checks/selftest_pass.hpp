// selftest_pass.hpp — a check that passes, so the harness can be seen to say so.
//
// Not a claim about a telephone. It exists because a harness whose only checks
// are failing ones cannot be told apart from a harness that always prints
// FAIL, and this is the other half of that pair. Delete it when a real check
// exercises the same path.

#pragma once

#include "verify.hpp"

namespace laporte::checks {

inline double selftest_pass_measure() { return 6.0 * 7.0; }

inline const bool selftest_pass_registered = verify::add({
    .name = "selftest.pass", .source = "selftest", .unit = "", .published = 42.0,
    .tolerance = 0.0, .until_item = 0, .measure = selftest_pass_measure});

}  // namespace laporte::checks
