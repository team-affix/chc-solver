#ifndef EXPR_CONTEXT_HPP
#define EXPR_CONTEXT_HPP

#include <map>
#include "var_context.hpp"
#include "expr.hpp"

struct expr_context {
    expr_context(var_context&, expr_pool&);
    const expr* fresh();
    const expr* copy(const expr*, std::map<uint32_t, uint32_t>&);
#ifndef DEBUG
private:
#endif
    var_context& var_context_ref;
    expr_pool& expr_pool_ref;
};

#endif