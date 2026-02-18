#ifndef FULFILL_HPP
#define FULFILL_HPP

#include "constraint.hpp"
#include "rule.hpp"

struct fulfillment {
    constraint_id clause_id;
    rule_id candidate_id;
    auto operator<=>(const fulfillment&) const = default;
};

#endif
