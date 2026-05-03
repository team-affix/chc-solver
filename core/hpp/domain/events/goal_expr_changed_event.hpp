#ifndef GOAL_EXPR_CHANGED_EVENT_HPP
#define GOAL_EXPR_CHANGED_EVENT_HPP

#include "../value_objects/lineage.hpp"

struct goal_expr_changed_event {
    const goal_lineage* gl;
};

#endif
