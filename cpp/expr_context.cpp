#include <stdexcept>
#include "../hpp/expr_context.hpp"

expr_context::expr_context(var_context& var_context_ref, expr_pool& expr_pool_ref)
    : var_context_ref(var_context_ref), expr_pool_ref(expr_pool_ref) {

}

const expr* expr_context::fresh() {
    return expr_pool_ref.var(var_context_ref.next());
}

const expr* expr_context::copy(const expr* e, std::map<uint32_t, uint32_t>& variable_map) {
    // If the expression is an atom, return the atom unchanged
    if (std::holds_alternative<expr::atom>(e->content))
        return e;

    // If the expression is a variable
    if (const expr::var* v = std::get_if<expr::var>(&e->content)) {
        // See if the variable has already been copied
        auto it = variable_map.find(v->index);
        
        // If the variable has does not have a mapping, create one
        if (it == variable_map.end())
            it = variable_map.insert({v->index, var_context_ref.next()}).first;

        // Return the mapped variable
        return expr_pool_ref.var(it->second);
    }

    // If the expression is a cons cell, copy the car and cdr
    if (const expr::cons* c = std::get_if<expr::cons>(&e->content)) {
        return expr_pool_ref.cons(
            copy(c->lhs, variable_map),
            copy(c->rhs, variable_map));
    }

    throw std::runtime_error("Unsupported expression type");
}