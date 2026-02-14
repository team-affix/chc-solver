#ifndef EXPR_HPP
#define EXPR_HPP

// This file is dedicated to defining s-expressions used for this project, as well as
// functions that operate on them such as unifications.

#include <string>
#include <variant>
#include <set>

struct expr {
    struct atom { std::string value; auto operator<=>(const atom&) const = default; };
    struct var  { uint32_t index;    auto operator<=>(const var&) const = default; };
    struct cons { const expr* lhs; const expr* rhs; auto operator<=>(const cons&) const = default; };
    std::variant<atom, cons, var> content;
    auto operator<=>(const expr&) const = default;
};

struct expr_pool {
    const expr* atom(const std::string& s);
    const expr* var(uint32_t i);
    const expr* cons(const expr* l, const expr* r);
private:
    const expr* intern(expr&& e);
    std::set<expr> m_exprs;
};

#endif

