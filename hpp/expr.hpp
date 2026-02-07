#ifndef EXPR_HPP
#define EXPR_HPP

// This file is dedicated to defining s-expressions used for this project, as well as
// functions that operate on them such as unifications.

#include <string>
#include <map>
#include <variant>
#include "box.hpp"

struct expr {
    struct atom {std::string m_value;};
    struct cons {
        cons();
        cons(const expr&, const expr&);
        cons(const cons&);
        cons& operator=(const cons&);
        std::unique_ptr<expr> m_lhs, m_rhs;
    };
    struct var {uint32_t m_index;};
    std::variant<atom, cons, var> m_content;
};

bool unify(expr& a_lhs, expr& a_rhs, std::map<uint32_t, expr>& a_bindings);

#endif

