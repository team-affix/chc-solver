#ifndef A02_DEFS_HPP
#define A02_DEFS_HPP

#include "a01_defs.hpp"

using a02_goals = a01_goals;
using a02_goal_store = a01_goal_store;
using a02_candidate_store = a01_candidate_store;
using a02_database = a01_database;
using a02_goal_rewards = std::map<const goal_lineage*, double>;

#endif
