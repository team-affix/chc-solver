#ifndef PROGRAM_EXPR_CONTEXT_HPP
#define PROGRAM_EXPR_CONTEXT_HPP

#include <map>
#include "expr.hpp"
#include "trail.hpp"

struct program_expr_context {
    program_expr_context(trail&, expr_pool&);
    const expr* next_variable();
    const expr* copy_expr(const expr*, std::map<uint32_t, const expr*>&);
#ifndef DEBUG
private:
#endif
    trail& trail_ref;
    expr_pool& expr_pool_ref;
    uint32_t variable_count;
};

#endif
