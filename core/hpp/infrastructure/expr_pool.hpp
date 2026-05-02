#ifndef EXPR_POOL_HPP
#define EXPR_POOL_HPP

#include <set>
#include "../domain/value_objects/expr.hpp"
#include "../utility/tracked.hpp"

struct expr_pool {
    expr_pool();
    const expr* functor(const std::string& name, std::vector<const expr*> args = {});
    const expr* var(uint32_t);
    const expr* import(const expr*);
#ifndef DEBUG
private:
#endif
    const expr* intern(expr&&);

    tracked<std::set<expr>> exprs;
};

#endif
