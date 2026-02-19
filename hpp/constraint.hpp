#ifndef CONSTRAINT_HPP
#define CONSTRAINT_HPP

#include <cstdint>
#include "rule.hpp"
#include "expr.hpp"

struct constraint_id {
    const constraint_id* parent;
    rule_id chosen_rule;
    uint32_t body_index;
    auto operator<=>(const constraint_id&) const = default;
};

struct constraint {
    const constraint_id* id;
    const expr* internal_expr;
};

struct constraint_id_pool {
    const constraint_id* fulfillment_child(const constraint_id*, rule_id, uint32_t);
    size_t size() const;
#ifndef DEBUG
private:
#endif
    const constraint_id* intern(constraint_id&&);
    std::set<constraint_id> constraint_ids;
};

#endif

