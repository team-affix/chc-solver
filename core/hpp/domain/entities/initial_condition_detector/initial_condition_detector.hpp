#ifndef INITIAL_CONDITION_CHECKER_HPP
#define INITIAL_CONDITION_CHECKER_HPP

#include "../../value_objects/lineage.hpp"
#include "../candidate_store/candidate_store.hpp"
#include "../../../infrastructure/event_topic.hpp"

struct initial_condition_checker {
    initial_condition_checker();
    void operator()(const goal_lineage*);
#ifndef DEBUG
private:
#endif
    lineage_pool& lp;
    candidate_store& cs;
    topic<const resolution_lineage*>& unit_topic;

    topic<const goal_lineage*>::subscription goal_inserted_subscription;
};

#endif
