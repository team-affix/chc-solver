#ifndef I_GOAL_EXPANDER_HPP
#define I_GOAL_EXPANDER_HPP

#include "../value_objects/lineage.hpp"

struct i_goal_store_manager {
    virtual ~i_goal_store_manager() = default;
    virtual void start_expansion(const resolution_lineage*) = 0;
    virtual void expand_child(const goal_lineage*) = 0;
    virtual void set_up_initial_goal(const goal_lineage*) = 0;
};

#endif
