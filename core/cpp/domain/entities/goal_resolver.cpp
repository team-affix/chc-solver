#ifndef RESOLVER_CPP
#define RESOLVER_CPP

#include "../../../hpp/domain/entities/goal_resolver.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

goal_resolver::goal_resolver() :
    db(resolver::resolve<i_database>()),
    lp(resolver::resolve<i_lineage_pool>()),
    goal_resolving_event_producer(resolver::resolve<i_event_producer<goal_resolving_event>>()),
    goal_resolved_event_producer(resolver::resolve<i_event_producer<goal_resolved_event>>()),
    goal_activator(resolver::resolve<i_goal_activator>()),
    goal_deactivator(resolver::resolve<i_goal_deactivator>()) {
}

void goal_resolver::resolve(const resolution_lineage* rl) {
    // signal goal resolving and resolution
    const goal_lineage* gl = rl->parent;
    goal_resolving_event_producer.produce(goal_resolving_event{rl});
    goal_resolved_event_producer.produce(goal_resolved_event{rl});
    goal_deactivator.deactivate(gl);

    // get the rule
    const rule& r = db.at(rl->idx);
    
    // signal goal activation for subgoals
    for (size_t i = 0; i < r.body.size(); ++i)
        goal_activator.activate(lp.goal(rl, i));
}

#endif
