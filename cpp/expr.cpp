#include "../hpp/expr.hpp"

const expr* expr_pool::atom(const std::string& a_string) {
    return intern(expr{expr::atom{a_string}});
}

const expr* expr_pool::var(uint32_t a_index) {
    return intern(expr{expr::var{a_index}});
}

const expr* expr_pool::cons(const expr* a_lhs, const expr* a_rhs) {
    return intern(expr{expr::cons{a_lhs, a_rhs}});
}

const expr* expr_pool::intern(expr&& a_expr) {
    return &*m_exprs.insert(std::move(a_expr)).first;
}

