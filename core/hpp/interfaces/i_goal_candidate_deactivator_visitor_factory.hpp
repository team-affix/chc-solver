#ifndef I_GOAL_CANDIDATE_DEACTIVATOR_VISITOR_FACTORY_HPP
#define I_GOAL_CANDIDATE_DEACTIVATOR_VISITOR_FACTORY_HPP

#include "../interfaces/i_factory.hpp"
#include "../interfaces/i_goal_candidate_deactivator_visitor.hpp"
#include "../value_objects/lineage.hpp"

struct i_goal_candidate_deactivator_visitor_factory : i_factory<i_goal_candidate_deactivator_visitor, const goal_lineage*> {
    virtual ~i_goal_candidate_deactivator_visitor_factory() = default;
};

#endif
