#ifndef FULFILL_HPP
#define FULFILL_HPP

#include "constraint.hpp"
#include "rule.hpp"

struct fulfillment {
    constraint_id constraint_id;
    rule_id rule_id;
};

#endif
