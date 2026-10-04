// selftest_xfail.hpp — a check that is expected to fail, forever, on purpose.
//
// The other path through the harness: it prints, it does not count, and it
// would count if it ever passed. Item 9999 does not exist, so the marker can
// never go stale. It measures 1 against a published 2, which is not a claim
// about anything. Delete it when a real check is waiting on a real item.

#pragma once

#include "verify.hpp"

namespace laporte::checks {

inline double selftest_xfail_measure() { return 1.0; }

inline const bool selftest_xfail_registered = verify::add({
    .name = "selftest.xfail", .source = "selftest", .unit = "", .published = 2.0,
    .tolerance = 0.0, .until_item = 9999, .measure = selftest_xfail_measure});

}  // namespace laporte::checks
