#ifndef COPIER_HPP
#define COPIER_HPP

#include <map>
#include "var_context.hpp"
#include "expr.hpp"

struct copier {
    copier(var_context&, expr_pool&);
    const expr* operator()(const expr*, std::map<uint32_t, uint32_t>&);
#ifndef DEBUG
private:
#endif
    var_context& var_context_ref;
    expr_pool& expr_pool_ref;
};

#endif
