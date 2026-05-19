#ifndef I_GET_GOAL_DB_RULES_HPP
#define I_GET_GOAL_DB_RULES_HPP

#include "../interfaces/i_rule_set.hpp"
#include "../value_objects/lineage.hpp"

struct i_get_goal_db_rules {
    virtual ~i_get_goal_db_rules() = default;
    virtual i_rule_set& get(const goal_lineage*) = 0;
};

#endif
