#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "context.h"
#include "rational.h"

namespace LinCAD {

  TEST_CASE("Sample linear function at point") {
    context c;

    variable x = c.add_variable("x");

    linear_expression* lp = c.add_linear_expression({{x, 1}}, -5);
    linear_expression expected({}, rational("0"));

    REQUIRE(evaluate_at(*lp, {{x, rational("5")}}) == expected);
  }
}
