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

}
