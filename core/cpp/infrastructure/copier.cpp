#include <stdexcept>
#include "../../hpp/infrastructure/copier.hpp"
#include "../../hpp/bootstrap/resolver.hpp"

copier::copier() :
    var_seq_ref(resolver::resolve<i_var_sequencer>()),
    expr_pool_ref(resolver::resolve<i_expr_pool>()) {
}

const expr* copier::copy(const expr* e, std::unordered_map<uint32_t, uint32_t>& variable_map) {
    // If the expression is a variable
    if (const expr::var* v = std::get_if<expr::var>(&e->content)) {
        // See if the variable has already been copied
        auto it = variable_map.find(v->index);
        
        // If the variable does not have a mapping, create one
        if (it == variable_map.end())
            it = variable_map.insert({v->index, var_seq_ref.next()}).first;

        // Return the mapped variable
        return expr_pool_ref.var(it->second);
    }

    // If the expression is a functor, copy all args recursively
    if (const expr::functor* f = std::get_if<expr::functor>(&e->content)) {
        std::vector<const expr*> copied_args;
        copied_args.reserve(f->args.size());
        for (const expr* arg : f->args)
            copied_args.push_back(copy(arg, variable_map));
        return expr_pool_ref.functor(f->name, std::move(copied_args));
    }

    throw std::runtime_error("Unsupported expression type");
}