#include "../../hpp/infrastructure/goal_candidates_store.hpp"

void goal_candidates_store::insert(const goal_lineage* gl, const candidate_set& cs) {
    goal_candidates.insert({gl, cs});
}

void goal_candidates_store::erase(const goal_lineage* gl) {
    goal_candidates.erase(gl);
}

void goal_candidates_store::eliminate(const resolution_lineage* rl) {
    goal_candidates.at(rl->parent).candidates.erase(rl->idx);
}

void goal_candidates_store::clear() {
    goal_candidates.clear();
}

const candidate_set& goal_candidates_store::at(const goal_lineage* gl) const {
    return goal_candidates.at(gl);
}
