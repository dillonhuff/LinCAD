#include "context.h"

#include <cassert>

using namespace std;

namespace LinCAD {

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
  
}
