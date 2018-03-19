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

      remove_zero_coeffs();
    }

    linear_expression(const std::map<variable, rational>& coeffs_,
                      const rational& c_) : coeffs(coeffs_), c(c_) {
      remove_zero_coeffs();
    }

    rational cof(const variable var) const {
      if (!contains_key(var, coeffs)) {
        return rational("0");
      }

      return map_find(var, coeffs);
    }

    linear_expression scalar_mul(const rational& r) const {
      std::map<variable, rational> mul_coeffs;
      for (auto cf : coeffs) {
        mul_coeffs.insert({cf.first, cf.second*r});
      }
      return linear_expression(mul_coeffs, r*get_const());

    }

    linear_expression drop(const variable v) const {
      std::map<variable, rational> dropped_coeffs = coeffs;
      dropped_coeffs.erase(v);
      return linear_expression(dropped_coeffs, get_const());
    }

    linear_expression
    evaluate_at(const std::map<variable, rational>& var_values) const;
    
    void remove_zero_coeffs() {
      std::vector<variable> zero_vars;
      for (auto cf : coeffs) {
        if (cf.second.sign() == 0) {
          zero_vars.push_back(cf.first);
        }
      }

      for (auto v : zero_vars) {
        coeffs.erase(v);
      }
    }

    rational get_const() const {
      return c;
    }

    linear_expression subtract(const linear_expression& other) const {

      std::map<variable, rational> sub_coeffs;
      for (auto cf : coeffs) {
        if (contains_key(cf.first, other.coeffs)) {
          sub_coeffs.insert({cf.first, cf.second - map_find(cf.first, other.coeffs)});
        } else {
          sub_coeffs.insert(cf);
        }
      }

      for (auto other_cf : other.coeffs) {
        if (!contains_key(other_cf.first, this->coeffs)) {
          sub_coeffs.insert({other_cf.first, -other_cf.second});
        }
      }

      rational cs = get_const() - other.get_const();

      return linear_expression(sub_coeffs, cs);
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

    int num_non_zero_coeffs() const {
      return coeffs.size();
    }

    rational get_only_non_zero_coeff() const {
      assert(coeffs.size() == 1);
      return begin(coeffs)->second;
    }

    std::map<variable, rational> coefficient_map() const {
      return coeffs;
    }
  };

  static inline
  std::ostream&
  operator<<(std::ostream& out, const linear_expression& l) {
    // TODO: Fill in coeffs
    for (auto c : l.coefficient_map()) {
      out << c.second << " * $v" << c.first << " + ";
    }

    out << l.get_const();
    return out;
  }

  static inline
  bool operator==(const linear_expression& x, const linear_expression& y) {
    return x.equals(y);
  }

  linear_expression evaluate_at(const linear_expression& l,
                                const std::map<variable, rational>& var_values);

  class cell {

    std::map<variable, rational> test_point;
    std::vector<cell*> children;
    
  public:

    cell(const std::map<variable, rational>& test_point_) :
      test_point(test_point_) {}

    int num_leaf_cells() const {
      if (children.size() == 0) {
        return 1;
      }

      int total = 0;
      for (auto c : children) {
        total += c->num_leaf_cells();
      }

      return total;
    }

    cell* add_child(const std::map<variable, rational>& test_pt) {
      cell* c = new cell(test_pt);
      children.push_back(c);

      return c;
    }

    std::vector<std::map<variable, rational> > test_points() const {
      if (children.size() == 0) {
        return {test_point};
      }

      std::vector<std::map<variable, rational> > pts;
      for (auto c : children) {
        auto tp = c->test_points();
        std::cout << "child has " << tp.size() << " test points" << std::endl;
        concat(pts, tp);
      }
      return pts;
    }

    ~cell() {
      for (auto c : children) {
        delete c;
      }
    }
  };

  class sign_invariant_partition {

    cell root;
    
  public:

    sign_invariant_partition() : root({}) {}

    std::vector<std::map<variable, rational> > test_points() const {
      return root.test_points();
    }

    cell* get_root_cell() { return &root; }

    int num_leaf_cells() const {
      return root.num_leaf_cells();
    }
  };

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

    linear_expression*
    add_linear_expression(const linear_expression& l) {
      linear_expression* expr = new linear_expression(l);
      exprs.insert(expr);
      return expr;
    }
    
    std::vector<linear_expression*>
    project_away(const std::vector<linear_expression*>& exprs,
                 const variable var);

    sign_invariant_partition
    build_sign_invariant_partition();

    ~context() {
      for (auto expr : exprs) {
        delete expr;
      }
    }
  };
}
