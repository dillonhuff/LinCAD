#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "context.h"
#include "rational.h"

using namespace std;

namespace LinCAD {

  TEST_CASE("Sample linear function at point") {
    context c;

    variable x = c.add_variable("x");

    linear_expression* lp = c.add_linear_expression({{x, 1}}, -5);
    linear_expression expected({}, rational("0"));

    REQUIRE(lp->evaluate_at({{x, rational("5")}}) == expected);
  }

  TEST_CASE("Projecting linear functions") {
    context c;
    variable x = c.add_variable("x");
    variable y = c.add_variable("y");

    auto xmy = c.add_linear_expression({{x, 1}, {y, -1}}, 0);
    auto mxy = c.add_linear_expression({{-x, 1}, {y, 1}}, 0);

    vector<linear_expression*> proj_set =
      c.project_away({xmy, mxy}, y);

    REQUIRE(proj_set.size() == 1);

    linear_expression* pj = proj_set[0];

    REQUIRE(pj->cof(y).sign() == 0);

    cout << "pj = " << *pj << endl;
  }
}
