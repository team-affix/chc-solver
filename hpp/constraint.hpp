#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include <vector>
#include "expr.hpp"

struct constraint {
private:
    std::vector<uint16_t> m_id;
    expr m_expr;
};

#endif

