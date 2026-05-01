#include "../../../hpp/domain/entities/goal_deactivator.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

goal_deactivator::goal_deactivator() :
    goal_deactivating_producer(resolver::resolve<i_event_producer<goal_deactivating_event>>()),
    goal_deactivated_producer(resolver::resolve<i_event_producer<goal_deactivated_event>>()) {
}

void goal_deactivator::deactivate(const goal_lineage* gl) {
    goal_deactivating_producer.produce(goal_deactivating_event{gl});
    goal_deactivated_producer.produce(goal_deactivated_event{gl});
}
