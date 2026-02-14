#include "../hpp/expr.hpp"

expr_pool::expr_pool(trail& t) : trail_ref(t) {

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

size_t expr_pool::size() const {
    return exprs.size();
}

const expr* expr_pool::intern(expr&& e) {
    auto [it, inserted] = exprs.insert(std::move(e));
    if (inserted) trail_ref.log([this, it]() { exprs.erase(it); });
    return &*it;
}
