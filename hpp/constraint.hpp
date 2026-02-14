#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include <list>
#include <cstdint>
#include "expr.hpp"

using constraint_id = std::list<uint32_t>;

struct constraint {
    constraint_id id;
    const expr* internal_expr;
};

#endif

