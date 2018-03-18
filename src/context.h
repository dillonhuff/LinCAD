#pragma once

#include "algorithm.h"

#include "rational.h"

using namespace dbhc;

namespace LinCAD {

  typedef int variable;

  class linear_expression {
    std::map<variable, rational> coeffs;
    rational c;

  public:

    linear_expression(const std::vector<std::pair<variable, int> >& coeffs_,
                      const int c_) : c(rational("0")) {
      c = rational(std::to_string(c_));

      coeffs = {};
      for (auto cf : coeffs_) {
        coeffs.insert({cf.first, rational(std::to_string(cf.second))});
      }
    }

    linear_expression(const std::map<variable, rational>& coeffs_,
                      const rational& c_) : coeffs(coeffs_), c(c_) {}

    rational get_const() const {
      return c;
    }

    bool equals(const linear_expression& other) const {
      if (c != other.c) {
        return false;
      }

      for (auto cf : coeffs) {
        variable v = cf.first;
        if (!contains_key(v, other.coeffs)) {
          return false;
        } else {
          if (cf.second != map_find(v, other.coeffs)) {
            return false;
          }
        }
      }

      return true;
    }
  };

  static inline
  bool operator==(const linear_expression& x, const linear_expression& y) {
    return x.equals(y);
  }

  linear_expression evaluate_at(const linear_expression& l,
                                const std::map<variable, rational>& var_values);

  class context {
    std::set<linear_expression*> exprs;
    std::map<int, std::string> var_names;
    variable next_var;

  public:

    context() : next_var(0) {}

    variable add_variable(const std::string var_name) {
      variable nv = next_var;

      // No duplicate names
      for (auto v : var_names) {
        assert(!(v.second == var_name));
      }

      var_names[nv] = var_name;
      next_var++;
      return nv;
    }

    linear_expression*
    add_linear_expression(const std::vector<std::pair<variable, int>>& coeffs, const int c) {
      linear_expression* expr = new linear_expression(coeffs, c);
      exprs.insert(expr);
      return expr;
    }

    ~context() {
      for (auto expr : exprs) {
        delete expr;
      }
    }
  };
}
