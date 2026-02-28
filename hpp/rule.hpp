#ifndef RULE_HPP
#define RULE_HPP

#include <list>
#include "expr.hpp"

using rule_id = uint32_t;

struct rule {
    const expr* head;
    std::list<const expr*> body;
};

#endif
