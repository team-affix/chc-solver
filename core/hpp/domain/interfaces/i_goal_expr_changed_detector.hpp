#ifndef I_GOAL_EXPR_CHANGED_DETECTOR_HPP
#define I_GOAL_EXPR_CHANGED_DETECTOR_HPP

#include <cstdint>
#include "../value_objects/lineage.hpp"

struct i_goal_expr_changed_detector {
    virtual ~i_goal_expr_changed_detector() = default;
    virtual void goal_activated(const goal_lineage*) = 0;
    virtual void goal_deactivated(const goal_lineage*) = 0;
    virtual void rep_updated(uint32_t) = 0;
};

#endif
