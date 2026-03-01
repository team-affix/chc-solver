#ifndef RULE_HPP
#define RULE_HPP

#include <list>
#include "expr.hpp"

struct rule {
    const expr* head;
    std::list<const expr*> body;
    auto operator<=>(const rule&) const = default;
};

#endif
