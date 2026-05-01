#ifndef GOAL_DEACTIVATOR_HPP
#define GOAL_DEACTIVATOR_HPP

#include "../interfaces/i_goal_deactivator.hpp"
#include "../interfaces/i_event_producer.hpp"
#include "../events/goal_deactivating_event.hpp"
#include "../events/goal_deactivated_event.hpp"

struct goal_deactivator : i_goal_deactivator {
    goal_deactivator();
    void deactivate(const goal_lineage*) override;
#ifndef DEBUG
private:
#endif
    i_event_producer<goal_deactivating_event>& goal_deactivating_producer;
    i_event_producer<goal_deactivated_event>& goal_deactivated_producer;
};

#endif
