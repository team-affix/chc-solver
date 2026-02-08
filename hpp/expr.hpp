#ifndef EXPR_HPP
#define EXPR_HPP

// This file is dedicated to defining s-expressions used for this project, as well as
// functions that operate on them such as unifications.

#include <string>
#include <map>
#include <variant>
#include <memory>

struct expr;

struct atom {
    atom(const std::string&);
private:
    std::string m_value;
};

struct cons {
    cons(const cons&);
    cons& operator=(const cons&);
    cons(cons&&);
    cons& operator=(cons&&);
    cons(const expr&, const expr&);
private:
    std::unique_ptr<expr> m_lhs, m_rhs;
};

struct var {
    var(uint32_t);
private:
    uint32_t m_index;
};

struct expr {
    expr(const std::variant<atom, cons, var>&);
private:
    std::variant<atom, cons, var> m_content;
};

bool unify(expr& a_lhs, expr& a_rhs, std::map<uint32_t, expr>& a_bindings);

#endif

