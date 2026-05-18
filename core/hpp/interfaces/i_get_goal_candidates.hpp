#ifndef I_GET_GOAL_CANDIDATES_HPP
#define I_GET_GOAL_CANDIDATES_HPP

#include "../interfaces/i_candidate_set.hpp"
#include "../value_objects/lineage.hpp"

struct i_get_goal_candidates {
    virtual ~i_get_goal_candidates() = default;
    virtual i_candidate_set& get(const goal_lineage*) = 0;
};

#endif
