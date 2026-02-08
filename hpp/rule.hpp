#ifndef RULE_HPP
#define RULE_HPP

#include <list>
#include "expr.hpp"

struct rule {
private:
    expr m_head;
    std::list<expr> m_body;
};

#endif

