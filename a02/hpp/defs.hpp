#ifndef A02_DEFS_HPP
#define A02_DEFS_HPP

#include "../../common/hpp/defs.hpp"

using a02_goals           = goals;
using a02_goal_store      = goal_store;
using a02_candidate_store = candidate_store;
using a02_database        = database;
using a02_goal_rewards    = std::map<const goal_lineage*, double>;

#endif
