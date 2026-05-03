#include "../../../hpp/domain/entities/candidate_not_applicable_detector.hpp"
#include "../../../hpp/bootstrap/resolver.hpp"

candidate_not_applicable_detector::candidate_not_applicable_detector() :
    candidate_not_applicable_producer(resolver::resolve<i_event_producer<candidate_not_applicable_event>>()),
    bm(resolver::resolve<i_bind_map>()),
    ges(resolver::resolve<i_goal_expr_store>()),
    gcs(resolver::resolve<i_goal_candidates_store>()),
    db(resolver::resolve<i_database>()),
    lp(resolver::resolve<i_lineage_pool>()),
    t(resolver::resolve<i_trail>()) {
}

void candidate_not_applicable_detector::goal_expr_changed(const goal_lineage* gl) {
    // get the candidates for this goal
    const candidate_set& candidates = gcs.at(gl);

    // for each candidate, check if it is applicable
    for (size_t candidate_idx : candidates.candidates) {
        if (!candidate_applicable(gl, candidate_idx)) {
            candidate_not_applicable_producer.produce(
                candidate_not_applicable_event{lp.resolution(gl, candidate_idx)});
        }
    }
}

bool candidate_not_applicable_detector::candidate_applicable(const goal_lineage* gl, size_t candidate_idx) {
    // get the goal expression
    const expr* goal = ges.at(gl);

    // get the candidate rule
    const rule& candidate = db.at(candidate_idx);
    
    // push the trail
    t.push();

    // create temp rep_changed queue
    std::queue<uint32_t> temp_rep_changed;

    // check if the unification succeeds
    bool result = bm.unify(goal, candidate.head, temp_rep_changed);

    // pop the trail
    t.pop();

    return result;
}
