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
  }

  TEST_CASE("Do sign invariant decomposition") {
    context c;
    variable x = c.add_variable("x");
    variable y = c.add_variable("y");

    auto xmy = c.add_linear_expression({{x, 1}, {y, -1}}, 0);
    auto mxy = c.add_linear_expression({{-x, 1}, {y, 1}}, 0);

    sign_invariant_partition sid =
      c.build_sign_invariant_partition({xmy, mxy});

    REQUIRE(sid.num_leaf_cells() == 13);

    cout << "Test points" << endl;
    for (auto pt : sid.test_points()) {
      cout << "\t(";
      for (auto val : pt) {
        cout << "$v" << val.first << " -> " << val.second << ", ";
      }
      cout << " )" << endl;
    }
  }

  TEST_CASE("Projecting away a horizontal line") {
    context c;

    variable x = c.add_variable("x");
    variable y = c.add_variable("y");

    auto xm3 = c.add_linear_expression({{x, 1}}, -3);

    vector<linear_expression*> proj_set =
      c.project_away({xm3}, y);

    REQUIRE(proj_set.size() == 1);
    
  }

  TEST_CASE("One variable SAT") {
    context c;

    variable a = c.add_variable("a");

    auto p3xp4 = c.add_linear_expression({{a, 3}}, 0);
    auto m2m7 = c.add_linear_expression({{a, 5}}, 0);

    c.add_constraint(p3xp4, EQUAL_ZERO);
    c.add_constraint(m2m7, EQUAL_ZERO);

    maybe<map<variable, rational> > model =
      c.solve_constraints();

    REQUIRE(model.has_value());
  }

  TEST_CASE("Two variable SAT") {
    context c;

    variable a = c.add_variable("a");
    variable b = c.add_variable("b");

    auto f0 = c.add_linear_expression({{a, 3}, {b, -2}}, -7);
    auto f1 = c.add_linear_expression({{a, 5}, {b, 5}}, 4);

    c.add_constraint(f0, EQUAL_ZERO);
    c.add_constraint(f1, EQUAL_ZERO);

    maybe<map<variable, rational> > model =
      c.solve_constraints();

    REQUIRE(model.has_value());
  }
  
}
