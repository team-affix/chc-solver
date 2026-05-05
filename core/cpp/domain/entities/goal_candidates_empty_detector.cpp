#include "../../../hpp/domain/entities/goal_candidates_empty_detector.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

goal_candidates_empty_detector::goal_candidates_empty_detector() :
    goal_candidates_store(resolver::resolve<i_goal_candidates_store>()),
    goal_candidates_empty_event_producer(resolver::resolve<i_event_producer<goal_candidates_empty_event>>()) {
}

void goal_candidates_empty_detector::candidates_changed(const goal_lineage* gl) {
    if (goal_candidates_store.at(gl).candidates.empty())
        goal_candidates_empty_event_producer.produce(goal_candidates_empty_event{gl});
}
