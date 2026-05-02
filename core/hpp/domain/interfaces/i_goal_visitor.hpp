#ifndef I_GOAL_VISITOR_HPP
#define I_GOAL_VISITOR_HPP

#include "../value_objects/lineage.hpp"

struct i_goal_visitor {
    virtual ~i_goal_visitor() = default;
    virtual void visit(const goal_lineage*) = 0;
};

#endif
