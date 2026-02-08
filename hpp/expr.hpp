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
    const std::string& value() const;
private:
    std::string m_value;
};

struct cons {
    cons(const cons&);
    cons& operator=(const cons&);
    cons(cons&&);
    cons& operator=(cons&&);
    cons(const expr&, const expr&);
    const expr& lhs() const;
    const expr& rhs() const;
private:
    std::unique_ptr<expr> m_lhs, m_rhs;
};

struct var {
    var(uint32_t);
    uint32_t index() const;
private:
    uint32_t m_index;
};

struct expr {
    expr(const std::variant<atom, cons, var>&);
    const std::variant<atom, cons, var>& content() const;
private:
    std::variant<atom, cons, var> m_content;
};

bool unify(expr& a_lhs, expr& a_rhs, std::map<uint32_t, expr>& a_bindings);

#endif

