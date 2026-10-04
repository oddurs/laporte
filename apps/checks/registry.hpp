// registry.hpp — every check there is, one line each.
//
// Adding a check is one new file under apps/checks/ and one line here. Keep
// the lines sorted by file name so that two branches adding different checks
// touch different lines, and two adding the same one collide where they should.

#pragma once

#include "checks/determinism.hpp"
#include "checks/selftest_pass.hpp"
#include "checks/selftest_xfail.hpp"
