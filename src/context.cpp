#include "context.h"

#include <cassert>

using namespace std;

namespace LinCAD {

  linear_expression evaluate_at(const linear_expression& l,
                                const std::map<variable, rational>& var_values) {
    map<variable, rational> un_evaluated;
    rational fresh_const = l.get_const();

    return linear_expression(un_evaluated, fresh_const);
  }

}
