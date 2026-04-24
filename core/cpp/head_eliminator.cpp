#include "../hpp/head_eliminator.hpp"

head_eliminator::head_eliminator(
    bind_map& bm,
    expr_pool& ep,
    goal_store& gs,
    candidate_store& cs) : bm(bm), ep(ep), gs(gs), cs(cs) {

}

void head_eliminator::extract_rep_vars(const expr* e, std::unordered_set<uint32_t>& reps) {
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

void head_eliminator::watch(const std::unordered_set<uint32_t>& reps, const std::unordered_set<const goal_lineage*>& goals) {
    for (uint32_t rep : reps)
        rep_to_goals[rep].insert(goals.begin(), goals.end());
    for (const goal_lineage* gl : goals)
        goal_to_reps[gl].insert(reps.begin(), reps.end());
}

std::unordered_set<const goal_lineage*> head_eliminator::unwatch(uint32_t rep) {
    auto it = rep_to_goals.find(rep);
    for (const goal_lineage* gl : it->second) {
        auto reps = goal_to_reps.at(gl);
        reps.erase(rep);
        if (reps.empty())
            goal_to_reps.erase(gl);
    }
    auto node = rep_to_goals.extract(it);
    return std::move(node.mapped());
}

std::unordered_set<uint32_t> head_eliminator::unwatch(const goal_lineage* gl) {
    auto it = goal_to_reps.find(gl);
    for (uint32_t rep : it->second) {
        auto goals = rep_to_goals.at(rep);
        goals.erase(gl);
        if (goals.empty())
            rep_to_goals.erase(rep);
    }
    auto node = goal_to_reps.extract(it);
    return std::move(node.mapped());
}

void head_eliminator::execute() {
    std::unordered_set<const goal_lineage*> touched_goals;

    auto& changed_reps = bm.changed_reps;
    
    while (!changed_reps.empty()) {
        // pop the changed rep
        uint32_t rep = changed_reps.front();
        changed_reps.pop();

        // get the goals that are watching this representative
        auto it = rep_to_goals.find(rep);

        if (it == rep_to_goals.end())
            continue;
        
        // basic deduplication per pipe() call
        for (const goal_lineage* gl : it->second) {
            touched_goals.insert(gl);
        }

        // update the watches given the rep update
        update_rep_watches(rep);
        
    }

    for (const goal_lineage* gl : touched_goals) {
        // get the candidates for this goal
        const std::vector<size_t>& candidates = cs.at(gl);

        for (size_t c : candidates) {
            
        }
        const std::vector<rule>& rules = db.rules(gl);
        // get the new candidates for this goal
        std::vector<size_t> new_candidates = expand(candidates, rules);
        // update the candidates for this goal
        cs.at(gl) = new_candidates;
    }
}

void head_eliminator::update_rep_watches(uint32_t rep) {
    // 1. pop the goals that are watching this representative
    std::unordered_set<const goal_lineage*> goals = unwatch(rep);

    // 2. rewatch based on the new representatives
    std::unordered_set<uint32_t> new_reps;
    extract_rep_vars(ep.var(rep), new_reps);
    watch(new_reps, goals);
}
