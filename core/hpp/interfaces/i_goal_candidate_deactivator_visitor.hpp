#ifndef I_GOAL_CANDIDATE_DEACTIVATOR_VISITOR_HPP
#define I_GOAL_CANDIDATE_DEACTIVATOR_VISITOR_HPP

#include <cstddef>
#include "../interfaces/i_visitor.hpp"

struct i_goal_candidate_deactivator_visitor : i_visitor<size_t> {
    virtual ~i_goal_candidate_deactivator_visitor() = default;
};

#endif
