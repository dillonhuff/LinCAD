#include "context.h"

#include <cassert>

using namespace std;

namespace LinCAD {

  std::vector<rational> build_test_points(const std::vector<rational>& sorted_roots) {
    if (sorted_roots.size() == 0) {
      return {rational("0")};
    }

    rational fst_root = sorted_roots.front();
    rational last_root = sorted_roots.back();

    vector<rational> test_points;
    test_points.push_back(fst_root - rational("1"));

    for (int i = 0; i < ((int) sorted_roots.size()); i++) {
      test_points.push_back(sorted_roots[i]);

      if (i < (((int)sorted_roots.size()) - 1)) {
        test_points.push_back((sorted_roots[i] + sorted_roots[i + 1]) / rational("2"));
      }
    }

    test_points.push_back(last_root + rational("1"));

    return test_points;
  }

  std::vector<rational>
  ordered_roots(const std::vector<linear_expression*>& base_set,
                const map<variable, rational>& test_point) {
    vector<rational> results;

    for (auto expr : base_set) {
      linear_expression res = expr->evaluate_at(test_point);
      assert(res.num_non_zero_coeffs() == 1);
      rational b = res.get_const();
      rational a = res.get_only_non_zero_coeff();

      assert(a != rational("0"));

      results.push_back((-b) / a);
    }
    return sort_unique(results);
  }

  linear_expression
  linear_expression::evaluate_at(const std::map<variable, rational>& var_values) const {

    map<variable, rational> un_evaluated;
    rational fresh_const = get_const();
    for (auto cf : coeffs) {
      if (contains_key(cf.first, var_values)) {
        fresh_const = fresh_const + map_find(cf.first, var_values)*cf.second;
      } else {
        un_evaluated.insert(cf);
      }
    }

    return linear_expression(un_evaluated, fresh_const);
  }

  
  void lift(const std::vector<std::vector<linear_expression*> >& projection_sets,
            const std::vector<variable>& variable_order,
            const int i,
            const map<variable, rational>& test_point,
            cell* cl) {

    //cout << "i = " << i << endl;
    assert(projection_sets.size() == variable_order.size());
    assert(i <= ((int) projection_sets.size()));

    if (i == -1) {
      return;
    }

    const vector<linear_expression*>& base_set = projection_sets[i];
    vector<rational> roots = ordered_roots(base_set, test_point);
    // Here: order all functions in the base set

    // vector<pair<linear_expression*, rational> > expr_values;
    // for (auto r : roots) {
    //   expr_values.
    // }

    // sort_lt(expr_values, [](const pair<linear_expression*, rational>& p) {
    //     return p.second;
    //   });
    
    // cout << "Base roots" << endl;
    // for (auto r : roots) {
    //   cout << "\t" << r << endl;
    // }

    vector<rational> test_points = build_test_points(roots);
    cout << "Base test points" << endl;
    for (auto r : test_points) {
      cout << "\t" << r << endl;
    }

    //vector<vector<linear_expression*, rational> > values_at_roots;
    
    variable var = variable_order[i];
    //cout << "Lifting next dim" << endl;
    for (auto r : test_points) {
      map<variable, rational> fresh_test_point = test_point;
      fresh_test_point[var] = r;
      cell* fresh_cell = cl->add_child();
      lift(projection_sets, variable_order, i - 1, fresh_test_point, fresh_cell);
    }

    
  }

  std::vector<linear_expression*>
  context::project_away(const std::vector<linear_expression*>& exprs,
                        const variable var) {
    // TODO: Add expressions that do not depend on var (vertical lines wrt var)

    // Add all possible root expressions
    vector<linear_expression*> proj_set;
    for (int i = 0; i < (int) exprs.size(); i++) {
      linear_expression* la = exprs[i];
      for (int j = i + 1; j < (int) exprs.size(); j++) {
        linear_expression* lb = exprs[j];

        // Compute cof(var, lb)*drop(var, la) - cof(var, la)*drop(var, lb)
        linear_expression res =
          la->drop(var).scalar_mul(lb->cof(var)).subtract(lb->drop(var).scalar_mul(la->cof(var)));
          
        linear_expression* res_e = add_linear_expression(res);
        proj_set.push_back(res_e);
      }
    }
    return proj_set;
  }

  sign_invariant_partition
  context::build_sign_invariant_partition() {
    // Choose variable order
    vector<variable> variable_order;
    for (int i = 0; i < next_var; i++) {
      variable_order.push_back(i);
    }

    // Projection phase: build each projection set
    vector<vector<linear_expression*> > projection_sets;
    projection_sets.push_back(vector<linear_expression*>(begin(exprs), end(exprs)));

    for (int i = 1; i < variable_order.size(); i++) {
      variable var = variable_order[i];
      projection_sets.push_back(project_away(projection_sets[i - 1], var));
    }

    cout << "Projection sets" << endl;
    for (int i = 0; i < projection_sets.size(); i++) {
      cout << "\tProjection set " << i << endl;
      for (auto p : projection_sets[i]) {
        cout << "\t\t" << *p << endl;
      }
    }

    assert(projection_sets.size() == ((int) variable_order.size()));

    // Base and lift phase: Solve one dimensional system wrt variable 0,
    // then back-substitute
    sign_invariant_partition sid;

    map<variable, rational> test_point{};

    cell* c = sid.get_root_cell();
    lift(projection_sets,
         variable_order,
         ((int) projection_sets.size()) - 1,
         test_point,
         c);


    // for (int i = (projection_sets.size() - 1); i >= 0; i--) {
    //   // Lifting procedure:
    //   // substitute in test_point,
    //   // find roots in 1d substitute system,
    //   // create test points from root list
    //   // for each test point:
    //   //   create cell in decomp tree for that point
    //   //   if i > 0: repeat lifting procedure on projection_set (i - 1)
    //   vector<linear_expression*>& base_set = projection_sets.back();
    //   vector<rational> roots = ordered_roots(base_set, {});

    
    //   cout << "Base roots" << endl;
    //   for (auto r : roots) {
    //     cout << "\t" << r << endl;
    //   }

    //   vector<rational> test_points = build_test_points(roots);
    //   cout << "Base test points" << endl;
    //   for (auto r : test_points) {
    //     cout << "\t" << r << endl;
    //   }
    // }
    
    return sid;
  }

}
