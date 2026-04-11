#include <stdexcept>
#include <memory>
#include "../hpp/expr.hpp"

expr_pool::expr_pool(trail& t) : exprs(t, {}) {

}

const expr* expr_pool::atom(const std::string& s) {
    return intern(expr{expr::atom{s}});
}

const expr* expr_pool::var(uint32_t i) {
    return intern(expr{expr::var{i}});
}

const expr* expr_pool::cons(const expr* l, const expr* r) {
    return intern(expr{expr::cons{l, r}});
}

const expr* expr_pool::import(const expr* e) {
    // if the expression is a leaf, just intern it
    if (std::holds_alternative<expr::atom>(e->content) ||
        std::holds_alternative<expr::var>(e->content))
        return intern(expr{e->content});
    
    // if the expression is a cons cell, copy the lhs and rhs
    if (const expr::cons* c = std::get_if<expr::cons>(&e->content))
        return cons(import(c->lhs), import(c->rhs));

    throw std::runtime_error("Unsupported expression type");
}

size_t expr_pool::size() const {
    return exprs.get().size();
}

const expr* expr_pool::intern(const expr& e) {
    exprs.insert(e);
    return &*exprs.get().find(e);
}
