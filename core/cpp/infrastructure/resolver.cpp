#include "../../hpp/infrastructure/resolver.hpp"

resolver::resolver(
    i_lineage_pool& lp,
    i_goal_activator& goal_activator,
    i_goal_deactivator& goal_deactivator,
    i_get_goal_db_rules& ggdr,
    i_get_goal_candidate_rules& ggcr,
    i_goal_candidate_activator_visitor_factory& gca,
    i_goal_candidate_deactivator_visitor_factory& gcd,
    i_conflict_detector& cd,
    i_unit_goal_detector& ugd,
    i_unit_goals& ug)
    :
    lp(lp),
    goal_activator(goal_activator),
    goal_deactivator(goal_deactivator),
    ggdr(ggdr),
    ggcr(ggcr),
    gca(gca),
    gcd(gcd),
    cd(cd),
    ugd(ugd),
    ug(ug) {
}

bool resolver::resolve(const resolution_lineage* rl) {
    // 1. get the rule for the goals
    const rule* r = rl->idx;
    // 2. activate the subgoals
    for (auto e : r->body) {
        const goal_lineage* gl = lp.goal(rl, e);
        goal_activator.activate(gl);
        // create a visitor for the db rules
        auto vis = gca.make(gl);
        // get db rules for this goal
        auto& db_rules = ggdr.get(gl);
        // visit the db rules
        db_rules.accept(*vis);
        // check for conflicts
        if (cd.detect(gl))
            return false;
        // check for unit goals
        if (ugd.detect(gl))
            ug.push(gl);
    }
    // get parent goal lineage
    const goal_lineage* gl = rl->parent;
    // create a visitor for the candidate rules
    auto vis = gcd.make(gl);
    // get candidate rules for this goal
    auto& candidate_rules = ggcr.get(gl);
    // visit the candidate rules
    candidate_rules.accept(*vis);
    // 4. deactivate the parent goal
    goal_deactivator.deactivate(gl);
    // 5. return success
    return true;
}
