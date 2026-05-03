#include "../../../hpp/domain/entities/goal_expr_changed_detector.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

goal_expr_changed_detector::goal_expr_changed_detector() :
    goal_expr_changed_producer(resolver::resolve<i_event_producer<goal_expr_changed_event>>()),
    bm(resolver::resolve<i_bind_map>()),
    ep(resolver::resolve<i_expr_pool>()),
    ges(resolver::resolve<i_goal_expr_store>()) {
}

void goal_expr_changed_detector::goal_activated(const goal_lineage* gl) {
    std::unordered_set<uint32_t> reps;
    extract_rep_vars(ges.at(gl), reps);
    watch(reps, {gl});
}

void goal_expr_changed_detector::goal_deactivated(const goal_lineage* gl) {
    unwatch(gl);
}

void goal_expr_changed_detector::rep_updated(uint32_t rep) {
    // get the goals that are watching this representative
    auto it = rep_to_goals.find(rep);

    if (it == rep_to_goals.end())
        return;
    
    // for every goal that was touched, emit event
    for (const goal_lineage* gl : it->second)
        goal_expr_changed_producer.produce(goal_expr_changed_event{gl});
    
    // update the watches given the rep update
    update_rep_watches(rep);
}

void goal_expr_changed_detector::extract_rep_vars(const expr* e, std::unordered_set<uint32_t>& reps) {
    const expr* e_rep = bm.whnf(e);

    if (const expr::var* v = std::get_if<expr::var>(&e_rep->content)) {
        reps.insert(v->index);
    }
    else {
        const expr::functor& f = std::get<expr::functor>(e_rep->content);
        for (const expr* arg : f.args)
            extract_rep_vars(arg, reps);
    }
}

void goal_expr_changed_detector::watch(const std::unordered_set<uint32_t>& reps, const std::unordered_set<const goal_lineage*>& goals) {
    for (uint32_t rep : reps)
        rep_to_goals[rep].insert(goals.begin(), goals.end());
    for (const goal_lineage* gl : goals)
        goal_to_reps[gl].insert(reps.begin(), reps.end());
}

std::unordered_set<const goal_lineage*> goal_expr_changed_detector::unwatch(uint32_t rep) {
    auto it = rep_to_goals.find(rep);
    for (const goal_lineage* gl : it->second) {
        auto& reps = goal_to_reps.at(gl);
        reps.erase(rep);
        if (reps.empty())
            goal_to_reps.erase(gl);
    }
    auto node = rep_to_goals.extract(it);
    return std::move(node.mapped());
}

std::unordered_set<uint32_t> goal_expr_changed_detector::unwatch(const goal_lineage* gl) {
    auto it = goal_to_reps.find(gl);
    for (uint32_t rep : it->second) {
        auto& goals = rep_to_goals.at(rep);
        goals.erase(gl);
        if (goals.empty())
            rep_to_goals.erase(rep);
    }
    auto node = goal_to_reps.extract(it);
    return std::move(node.mapped());
}

void goal_expr_changed_detector::update_rep_watches(uint32_t rep) {
    // 1. pop the goals that are watching this representative
    std::unordered_set<const goal_lineage*> goals = unwatch(rep);

    // 2. rewatch based on the new representatives
    std::unordered_set<uint32_t> new_reps;
    extract_rep_vars(ep.var(rep), new_reps);
    watch(new_reps, goals);
}
