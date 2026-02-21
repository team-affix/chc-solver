#include <stdexcept>
#include "../hpp/program_expr_context.hpp"

program_expr_context::program_expr_context(trail& trail_ref, expr_pool& expr_pool_ref)
    : trail_ref(trail_ref), expr_pool_ref(expr_pool_ref), variable_count(0) {

}

const expr* program_expr_context::next_variable() {
    trail_ref.log([this]{--variable_count;});
    return expr_pool_ref.var(variable_count++);
}

const expr* program_expr_context::copy_expr(const expr* e, std::map<uint32_t, const expr*>& variable_map) {
    // If the expression is an atom, return the atom unchanged
    if (std::holds_alternative<expr::atom>(e->content))
        return e;

    // If the expression is a variable
    if (const expr::var* v = std::get_if<expr::var>(&e->content)) {
        // See if the variable has already been copied
        auto it = variable_map.find(v->index);
        
        // If the variable has already been copied, return the copy
        if (it != variable_map.end())
            return it->second;

        // If the variable has not been copied, copy it
        return variable_map[v->index] = next_variable();
    }

    // If the expression is a cons cell, copy the car and cdr
    if (const expr::cons* c = std::get_if<expr::cons>(&e->content)) {
        return expr_pool_ref.cons(
            copy_expr(c->lhs, variable_map),
            copy_expr(c->rhs, variable_map));
    }

    throw std::runtime_error("Unsupported expression type");
}
