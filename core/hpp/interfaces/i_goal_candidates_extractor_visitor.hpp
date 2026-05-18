#ifndef I_GOAL_CANDIDATES_EXTRACTOR_VISITOR_HPP
#define I_GOAL_CANDIDATES_EXTRACTOR_VISITOR_HPP

#include "../interfaces/i_visitor.hpp"
#include "../value_objects/lineage.hpp"

struct i_goal_candidates_extractor_visitor : i_visitor<const resolution_lineage*> {
    virtual ~i_goal_candidates_extractor_visitor() = default;
};

#endif
